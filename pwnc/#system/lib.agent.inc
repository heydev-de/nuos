<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   AGENT CLASS
//==================================================================================================

define("CMS_AGENT_TEMPLATE_PATH",   CMS_PATH . "#json/agent/");
define("CMS_AGENT_DATA_PATH",       CMS_DATA_PATH . "#agent/");
define("CMS_AGENT_REQUEST_LIMIT",   32);
define("CMS_AGENT_REQUEST_TIMEOUT", 300);
define("CMS_AGENT_RESPONSE_LIMIT",  4194304);
define("CMS_AGENT_RESUME_LIMIT",    5);
define("CMS_AGENT_COMPACT_LIMIT",   3);
define("CMS_AGENT_COMPACT_RATIO",   0.25);
define("CMS_AGENT_COMPACT_TEXT",    "~~~ compacted ~~~");
define("CMS_AGENT_OMITTED_TEXT",    "~~~ omitted ~~~");

//related functions
//--------------------------------------------------------------------------------------------------

function agent_api_list()
{
    if (($hdir = opendir(CMS_AGENT_TEMPLATE_PATH)) === FALSE) return [];

    $array = [];
    while (($file_object = readdir($hdir)) !== FALSE)
    {
        if (preg_match("/^(.+)\.json$/i", $file_object, $file_object))
            $array[$file_object[1]] = $file_object[1];
    };

    closedir($hdir);
    ksort($array, SORT_NATURAL);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function agent_option_parse($text)
{
    $array = [];
    foreach (preg_split("/\r?\n/", $text, -1, PREG_SPLIT_NO_EMPTY) AS $line)
    {
        $pos = strpos($line, "=");
        if (! $pos) continue;

        $key = trim(substr($line, 0, $pos));
        if ($key === "") continue;

        $value = trim(substr($line, $pos + 1));
        if (is_numeric($value))                    $value = $value + 0;
        elseif (strcasecmp($value, "true")  === 0) $value = TRUE;
        elseif (strcasecmp($value, "false") === 0) $value = FALSE;

        $array[$key] = $value;
    };

    return $array;
};

//--------------------------------------------------------------------------------------------------

function agent_start($user,
                     $message,
                     $option = NULL)
{
    //precheck
    $agent = new agent($user, CMS_SUPERUSER, $option);
    if (nstre($agent->error)) return FALSE;

    //open conversation
    return $agent->send_message($message, CMS_SUPERUSER) ? $agent->id : FALSE;
};

class agent
{

//properties
//--------------------------------------------------------------------------------------------------

    public $id               = NULL;  //run id
    public $error            = NULL;  //error message
    public $resume_count     = 0;     //number of resumes

    //agent
    private $user            = NULL;  //username

    //provider
    private $template        = NULL;  //provider template
    private $option          = [];    //placeholder values
    private $option_raw      = NULL;  //original option parameter
    private $tool_list       = [];    //provider shaped tool list

    //conversation
    private $owner           = NULL;  //conversation owner
    private $data_path       = "";    //conversation directory
    private $context_offset  = 0;     //consumed context bytes
    private $message         = [];    //provider shaped message list
    private $compactable     = [];    //tool result entry index

    //mcp
    private $mcp_client      = NULL;  //mcp client object
    private $mcp_tool_list   = [];    //mcp tool list
    private $mcp_token       = NULL;  //hashed authorization token

    //run
    private $request_limit   = 0;     //max requests per run
    private $request_timeout = 0;     //seconds per request
    private $request_prev    = [];    //previous request
    private $request_size    = 0;     //last request length
    private $interrupted     = FALSE; //interrupt signal set
    private $finish_reason   = NULL;  //run finish reason

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($user,
                         $owner  = NULL,
                         $option = NULL)
    {
        //retrieve agent data
        $this->user = $user;
        $data = (new data("#system/permission"))->get("user.$user");
        if (! $data) { $this->error = "Unknown user."; return; };

        //create conversation directory
        $this->owner     = $owner ?? CMS_SUPERUSER;
        $this->data_path = CMS_AGENT_DATA_PATH .
                           encode_filename($this->owner) . "/" .
                           encode_filename($user) . "/";
        if (! mkpath($this->data_path)) { $this->error = "Can not create directory."; return; };

        //retrieve provider template
        $buffer = read_file(CMS_AGENT_TEMPLATE_PATH . ($data["api"] ?? "") . ".json");
        if ($buffer === FALSE) { $this->error = "No provider template."; return; };

        //decode and check version
        $this->template = json_decode($buffer, TRUE);
        if ((! is_array($this->template)) || ((int)($this->template["version"] ?? 0) !== 1))
            { $this->error = "Invalid provider template."; return; };

        //named fields
        $array = [];
        if (nstre($data["endpoint"] ?? ""))
            $array["endpoint"] = $data["endpoint"];

        if (nstre($data["model"] ?? ""))
            $array["model"] = $data["model"];

        if (nstre($data["api_key"] ?? ""))
        {
            $array["api_key"]       = $data["api_key"];
            $array["authorization"] = "Bearer " . $data["api_key"];
        };

        //preserve options
        $this->option_raw = $option;

        //combine options
        $this->option =
            (array)$option +
            $array +
            agent_option_parse($data["option"] ?? "") +
            (array)($this->template["default"] ?? []);

        //essential configuration
        foreach ((array)($this->template["required"] ?? []) AS $value)
        {
            if (stre($this->option[$value] ?? ""))
                { $this->error = "$value not configured."; return; };
        };

        //system prompt
        $this->option["system"] = mcp::system_prompt(permission::instruction($user), $user);
        if (isset($this->template["instruction"]))
            $this->option["instruction"] = $this->replace_placeholder(
                $this->template["instruction"], $this->option);

        //set limits
        $this->request_limit   = (int)($option["limit"]   ?? CMS_AGENT_REQUEST_LIMIT);
        $this->request_timeout = (int)($option["timeout"] ?? CMS_AGENT_REQUEST_TIMEOUT);

        //mcp endpoint
        $this->option["mcp_url"]  = $this->option["mcp_url"]  ?? CMS_MODULES_URL . "mcp.php";
        $this->option["mcp_name"] = $this->option["mcp_name"] ?? CMS_DOMAIN;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function send_message($message,
                          $user)
    {
        if (nstre($this->error)) return FALSE; //error
        if (stre($message))      return FALSE; //no message

        $this->id ??= self::generate_id();

        //existing conversation
        $path = $this->data_path . $this->id . ".json";
        if (is_file($path) && filesize($path))
        {
            //add message to context
            $entry = $this->generate_user_prompt($message, $user);
            $user  = ($user === NULL) ? NULL : "user.$user";

            if (
                 ($this->context_add($entry, "message", $user) === FALSE)
                 ||
                 (! $this->run_queue()) //continue run
               )
                return FALSE;
        }

        //new conversation
        elseif (! $this->run_queue($message, $user))
        {
            return FALSE;
        };

        cms_daemon_run();
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    static function interrupt($id)
    {
        return cms_daemon_running("agent.run.$id") ?
               cms_flag_set("agent.interrupt.$id") :
               cms_daemon_remove("agent.run.$id");
    }

//--------------------------------------------------------------------------------------------------

    private function send_request()
    {
        //load library
        if (! cms_load("http")) { $this->error = "Internal error."; return FALSE; };

        //prepare values
        $message          = array_values($this->message); //provider requires list
        $value            = $this->option;
        $value["message"] = $message;
        $value["tool"]    = $this->tool_list;

        //build request
        $url     = $this->replace_placeholder($this->template["url"], $value);
        $request = $this->replace_placeholder($this->template["request"], $value);
        $body    = json_encode($request, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);

        if ($body === FALSE) //malformed
        {
            $this->context_add(["url" => $url, "error" => json_last_error_msg()], "request");
            return FALSE;
        };

        $this->request_size = strlen($body);

        //remove message list
        if (($key = array_search($message, $request, TRUE)) !== FALSE)
            $request[$key] = CMS_AGENT_OMITTED_TEXT;

        //remove previous entries
        $diff = [];
        foreach ($request AS $key => $_value)
        {
            if (($this->request_prev[$key] ?? NULL) === $_value) continue; //omit existing
            $diff[$key] = $_value;
        };

        //store for next diff check
        $this->request_prev = $request;

        //write context
        $this->context_add(["url" => $url, "body" => $diff], "request");

        //send request
        $reply = http_request(
            $url,
            ["method"        => "POST",
             "header"        => $this->replace_placeholder($this->template["header"], $value),
             "body"          => $body,
             "timeout_total" => $this->request_timeout,
             "timeout_chunk" => $this->request_timeout,
             "limit"         => CMS_AGENT_RESPONSE_LIMIT]);

        if ($reply === FALSE) //no response
        {
            $this->context_add("no response", "response");
            return FALSE;
        };

        //decode response
        $json = json_decode($reply, TRUE);
        $this->context_add(($flag = is_array($json)) ? $json : $reply, "response");

        return $flag ? $json : FALSE;
    }

//--------------------------------------------------------------------------------------------------
//   TOKEN
//--------------------------------------------------------------------------------------------------

    private function token_issue()
    {
        //generate tagged temporary authorization token
        $token           = cms_token_tag();
        $this->mcp_token = hash64($token);

        $map = new map("#system/permission.token");
        $map->set($this->mcp_token, $this->user);
        return $map->save(NULL, TRUE) ? $token : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    private function token_revoke()
    {
        if (stre($this->mcp_token)) return FALSE;

        //remove temporary authorization token
        $map = new map("#system/permission.token");
        $map->del_key($this->mcp_token);
        if (! $map->save(NULL, TRUE)) return FALSE;

        $this->mcp_token = NULL;
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   CONTEXT
//--------------------------------------------------------------------------------------------------

    private function context_add($data,
                                 $type = "message",
                                 $from = NULL)
    {
        $hfile = fopen($this->data_path . $this->id . ".json", "ab");
        if ($hfile === FALSE) return FALSE;

        //use length of file as new entry index
        flock($hfile, LOCK_EX);
        fseek($hfile, 0, SEEK_END);
        $offset = ftell($hfile);

        //build context entry
        $entry = ["time"  => time(),
                  "index" => $offset,
                  "type"  => $type];
        if ($from !== NULL) $entry["from"] = $from;
        $entry["data"] = $data;

        //append
        $line = json_encode($entry, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE) . "\n";
        $flag = fwrite($hfile, $line);
        fclose($hfile);

        return ($flag === FALSE) ? FALSE : $offset;
    }

//--------------------------------------------------------------------------------------------------

    function context_restore()
    {
        $this->message        = [];
        $this->compactable    = [];
        $this->context_offset = 0;

        return $this->context_import();
    }

//--------------------------------------------------------------------------------------------------

    private function context_import()
    {
        $path = $this->data_path . $this->id . ".json";
        if (! is_file($path)) return TRUE; //no context to import

        $hfile = fopen($path, "rb");
        if ($hfile === FALSE) { $this->error = "Cannot open context."; return FALSE; };

        flock($hfile, LOCK_SH);
        fseek($hfile, $this->context_offset);

        //import messages since last call
        $flag = FALSE;
        while (($line = fgets($hfile)) !== FALSE)
        {
            $json = json_decode($line, TRUE);

            //context corrupted
            if (! is_array($json))
            {
                fclose($hfile);
                $this->error = "Context corrupted.";
                return FALSE;
            };

            $type  = $json["type"] ?? "";
            $index = (int)($json["index"] ?? 0);

            switch ($type)
            {
            case "message":
            case "result":

                $this->message_append($json["data"], $type, $index);
                $flag = TRUE;
                break;

            case "compress":

                $this->message_compact((int)$json["data"]);
            };
        };

        $this->context_offset = ftell($hfile); //store position
        fclose($hfile);

        //order by index
        if ($flag) ksort($this->message);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    private function context_overflow($response)
    {
        $pattern = $this->template["overflow"] ?? NULL;
        if (stre($pattern)) return FALSE;

        //reported as error
        $error = array_get_path($response, $this->template["value"]["error"] ?? "");
        if (nstre($error) && preg_match($pattern, $error)) return TRUE;

        //reported as stop reason
        $reason = array_get_path($response, $this->template["value"]["reason"] ?? "");
        return nstre($reason) && (bool)preg_match($pattern, $reason);
    }

//--------------------------------------------------------------------------------------------------

    private function context_compact($target)
    {
        $freed = 0;

        foreach (array_keys($this->compactable) AS $index)
        {
            $_freed = $this->message_compact($index);
            if ($_freed === 0) continue; //no gain

            $freed += $_freed;
            $this->context_add($index, "compress");
            if ($freed >= $target) break;
        };

        return $freed;
    }

//--------------------------------------------------------------------------------------------------
//   MESSAGE
//--------------------------------------------------------------------------------------------------

    private function message_add($message,
                                 $from = NULL,
                                 $type = "message")
    {
        $index = $this->context_add($message, $type, $from);
        if ($index === FALSE) return FALSE;

        $this->message_append($message, $type, $index);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    private function message_append($message,
                                    $type,
                                    $index)
    {
        if ($type === "result") $this->compactable[$index] = TRUE;
        $this->message[$index] = $message;
    }

//--------------------------------------------------------------------------------------------------

    private function message_compact($index)
    {
        unset($this->compactable[$index]);

        //get path to content field
        $list = $this->template["compact"]["list"] ?? "content";
        $path = explode("/", (string)($this->template["compact"]["text"] ?? "content"));

        $content = $this->message[$index][$list] ?? NULL;
        if ($flag = ! is_array($content)) $content = [[$path[0] => $content]]; //wrap

        $length = strlen(CMS_AGENT_COMPACT_TEXT);
        $freed  = 0;

        //process content blocks
        foreach ($content AS &$reference)
        {
            //traverse path to content field
            $_flag = TRUE;
            foreach ($path AS $key)
            {
                //structure ends before reaching content field
                if (! isset($reference[$key])) { $_flag = FALSE; break; };

                $reference = &$reference[$key]; //go deeper
            };

            //replace content value with compact text
            if ($_flag && (($_length = strlen((string)$reference)) > $length))
            {
                $freed     += $_length - $length;
                $reference  = CMS_AGENT_COMPACT_TEXT;
            };
        };

        $this->message[$index][$list] = $flag ? $content[0][$path[0]] : $content;

        return $freed;
    }

//--------------------------------------------------------------------------------------------------
//   RUN
//--------------------------------------------------------------------------------------------------

    private function run_queue($message = NULL,
                               $user    = NULL)
    {
        $id   = "agent.run." . $this->id;
        $flag = cms_daemon(
            "if (! cms_load(\"agent\")) return;\n" .
            "\$agent = new agent(" .     var_export($this->user, TRUE) . ", " .
                                         var_export($this->owner, TRUE) . ", " .
                                         var_export($this->option_raw, TRUE) . ");\n" .
            "if (nstre(\$agent->error)) return;\n" .
            "\$agent->id = "           . var_export($this->id, TRUE) . ";\n" .
            "\$agent->resume_count = " . var_export($this->resume_count, TRUE) . ";\n" .
            "\$agent->run_start(" .      var_export($message, TRUE) . ", " .
                                         var_export($user, TRUE) . ");",
            $id,
            0,
            "Agent " .   permission_get_name($this->user) . " (" . $this->user . ") " .
            "running " . $this->id);

        if ($flag)             return TRUE;  //daemon task created
        if ($message !== NULL) return FALSE; //new task creation failed

        //resumed run
        return cms_daemon_exists($id);
    }

//--------------------------------------------------------------------------------------------------

    function run_start($message = NULL,
                       $user    = NULL)
    {
        if (nstre($this->error)) return FALSE;

        //create run id
        $this->id ??= self::generate_id();

        //clear interrupt signal
        cms_flag_del("agent.interrupt." . $this->id);

        //create authorization token
        $token = $this->token_issue();
        if ($token === FALSE) { $this->error = "Cannot issue token."; return FALSE; };
        $this->option["mcp_token"] = $token;

        //handle timeout
        register_shutdown_function(function() { $this->run_shutdown(); });

//..................................................................................................

        //execute mcp tool

        //provider executes mcp
        if (isset($this->template["server"]) && (! ($this->option["internal"] ?? FALSE)))
        {
            $server                  = $this->template["server"];
            $this->option           += (array)($server["value"] ?? []);
            $this->option["server"]  = [$this->replace_placeholder($server["entry"],   $this->option)];
            $this->tool_list         = [$this->replace_placeholder($server["toolset"], $this->option)];
        }

        //pwnc executes mcp
        else
        {
            if (! cms_load("mcp_client")) return $this->run_stop("internal_error");

            $this->mcp_client = new mcp_client(
                $this->option["mcp_url"],
                $this->option["mcp_token"],
                ["timeout_total" => $this->request_timeout,
                 "timeout_chunk" => $this->request_timeout]);

            $list = $this->mcp_client->tools_list();
            if ($list === FALSE) return $this->run_stop($this->mcp_client->error);

            foreach ($list AS $tool)
            {
                $this->mcp_tool_list[$tool["name"]] = $tool;
                $this->tool_list[] = $this->replace_placeholder($this->template["tool"], $tool);
            };
        };

        //restore run
        if (! $this->context_restore()) return $this->run_stop("context_corrupted");

        //seed run
        if (blank($this->message))
        {
            //no new or existing context
            if (stre($message)) return $this->run_stop("no_context");

            //system prompt
            if (isset($this->template["system"]))
                $this->message_add($this->replace_placeholder(
                    $this->template["system"], $this->option), "system");

            //user message
            $this->message_add($this->generate_user_prompt($message, $user, "woke you"),
                               ($user === NULL) ? NULL : "user.$user");
        };

//..................................................................................................

        //run loop

        $compact_count = 0;
        for ($n = 0; $n < $this->request_limit; $n++)
        {
            //check for interrupt signal
            if ($this->run_interrupted()) return $this->run_stop("interrupted");

            //import new messages
            if (! $this->context_import()) return $this->run_stop("context_corrupted");

            //send request
            $response = $this->send_request();
            if ($response === FALSE) return $this->run_stop("connection_error");

            //context window overflow
            if ($this->context_overflow($response))
            {
                //limit compaction retry loops
                if (++$compact_count > CMS_AGENT_COMPACT_LIMIT)
                    return $this->run_stop("compact_limit");

                //compact context
                $target = (int)($this->request_size * CMS_AGENT_COMPACT_RATIO);
                if (! $this->context_compact($target)) return $this->run_stop("context_overflow");

                $n--;
                continue;
            };

            //resolve named values
            $value = [];
            foreach ($this->template["value"] AS $key => $path)
                $value[$key] = array_get_path($response, $path);

            //prefill rejects trailing whitespace
            if (($this->template["prefill"] ?? FALSE) && is_string($value["reply"] ?? NULL))
                $value["reply"] = rtrim($value["reply"]);

            $this->message_add($this->replace_placeholder($this->template["assistant"], $value),
                               "user." . $this->user);

            $action = $this->template["action"][(string)($value["reason"] ?? "")] ?? "error";

            switch ($action)
            {
            case "stop":
            case "tool":

                if (! $this->mcp_execute($value["call"] ?? []))
                    return $this->run_stop((string)$value["reason"]); //error
                break;

            case "resume":

                continue 2;

            default:

                //invalid
                return $this->run_stop(nstre($value["error"] ?? NULL) ? $value["error"] : "error");
            };

            $compact_count = 0; //reset
        };

        return $this->run_stop("request_limit");
    }

//--------------------------------------------------------------------------------------------------

    private function run_stop($finish_reason)
    {
        $this->finish_reason = $finish_reason;

        $this->context_add($finish_reason, "stop");
        $this->token_revoke();

        return $finish_reason;
    }

//--------------------------------------------------------------------------------------------------

    private function run_interrupted()
    {
        if ($this->interrupted) return TRUE;

        $key = "agent.interrupt." . $this->id;
        if (! cms_flag_get($key)) return FALSE;

        cms_flag_del($key);
        return $this->interrupted = TRUE;
    }

//--------------------------------------------------------------------------------------------------

    private function run_shutdown()
    {
        if (nstre($this->finish_reason)) return; //finished

        $this->run_stop("killed");

        //limit number of resumes
        if (++$this->resume_count >= CMS_AGENT_RESUME_LIMIT) return;

        //create daemon task
        $this->run_queue();
    }

//--------------------------------------------------------------------------------------------------
//   MCP
//--------------------------------------------------------------------------------------------------

    private function mcp_execute($call)
    {
        if ($this->mcp_client === NULL) return FALSE; //provider executes mcp

        $path   = $this->template["call"];
        $result = $this->template["result"];
        $block  = [];

        foreach ((array)$call AS $value)
        {
            $name = array_get_path($value, $path["name"] ?? "");
            if (stre($name)) continue; //no tool call

            $id = array_get_path($value, $path["id"] ?? "");

            //check for interrupt signal
            if ($this->run_interrupted())
            {
                $argument = NULL;
                $content  = "Interrupted.";
                $error    = TRUE;
            }

            //process tool call
            else
            {
                $argument = array_get_path($value, $path["argument"] ?? "");
                $error    = FALSE;

                //openai uses json string
                if (is_string($argument))
                {
                    $argument = json_decode($argument, TRUE);

                    //argument not found
                    if (! is_array($argument))
                    {
                        $argument = NULL;
                        $content  = "Invalid argument.";
                        $error    = TRUE;
                    };
                };

                //process result
                if (! $error)
                {
                    $mirror  = $this->mcp_tool_list[$name]["header"] ?? [];
                    $reply   = $this->mcp_client->tools_call($name, $argument, ["mirror" => $mirror]);
                    $error   = ($reply === FALSE) || (bool)($reply["isError"] ?? FALSE);
                    $content = ($reply === FALSE) ? $this->mcp_client->error : mcp_client::to_text($reply);
                };
            };

            $this->context_add(
                ["name"   => $name,
                 "input"  => $argument,
                 "error"  => $error,
                 "length" => strlen($content)],
                "tool");

            $block[] = $this->replace_placeholder(
                $result["block"],
                ["id"      => $id,
                 "name"    => $name,
                 "content" => $content,
                 "error"   => $error ? TRUE : NULL]);
        };

        if (blank($block)) return FALSE;

        //grouped
        if (isset($result["message"]))
        {
            $this->message_add($this->replace_placeholder($result["message"], ["block" => $block]),
                               "tool", "result");
            return TRUE;
        };

        //ungrouped
        foreach ($block AS $value) $this->message_add($value, "tool", "result");
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    private function replace_placeholder($array,
                                         $replacement)
    {
        if (! is_array($array))
        {
            if (! is_string($array)) return $array;

            //full replacement
            if (preg_match("/^%([^%]+)%$/", $array, $match))
                return $replacement[$match[1]] ?? NULL;

            //partial replacement
            return preg_replace_callback(
                "/%([^%]+)%/",
                function($match) use ($replacement)
                {
                    return (string)($replacement[$match[1]] ?? "");
                },
                $array);
        };

        $result  = [];
        $is_list = array_keys($array) === range(0, count($array) - 1);

        foreach ($array AS $key => $value)
        {
            $value    = $this->replace_placeholder($value, $replacement);
            $optional = is_string($key) && ($key[0] === "[") && ($key[-1] === "]");

            //omit optional field on blank value
            if (($optional || $is_list) && blank($value)) continue;

            //remove brackets
            if ($optional) $key = substr($key, 1, -1);

            $result[$key] = $value;
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    static function generate_id()
    {
        return date("Y-m-d_H-i-s") . "_" . unique_id();
    }

//--------------------------------------------------------------------------------------------------

    private function generate_user_prompt($message,
                                          $user,
                                          $action = "wrote")
    {
        if ($user !== NULL)
            $message = "`$user` (" . permission_get_name($user, "no name") . ") $action: $message";

        return $this->replace_placeholder(
            $this->template["user"], ["text" => $message] + $this->option);
    }
};