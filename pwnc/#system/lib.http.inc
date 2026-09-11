<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   HTTP FUNCTIONS
//==================================================================================================

//constants
define("CMS_HTTP_TIMEOUT_TOTAL",      10);
define("CMS_HTTP_TIMEOUT_CHUNK",       3);
define("CMS_HTTP_SIZE_CHUNK",     524289);
define("CMS_HTTP_LIMIT",         1048576);

//related functions
//--------------------------------------------------------------------------------------------------

function http_fopen($url,
                    $post_data = NULL,
                    &$header   = NULL)
{
    //break address into components
    $url = analyze_url($url);
    if ($url === FALSE)        return FALSE; //invalid
    if ($url["host"] === NULL) return FALSE; //no host

    //scheme
    switch ($url["scheme"])
    {
    case "":
    case "http":

        $transport    = "tcp://";
        $port_default = 80;
        $context      = NULL;
        break;

    case "https":

        $transport    = "ssl://";
        $port_default = 443;
        $context      = stream_context_create(["ssl" => ["verify_peer" => FALSE]]);
        break;

    default:

        return FALSE; //impermissible scheme
    };

    //address components
    $host  = $url["host"];
    $path  = ($url["path"] === NULL)  ? "/"           : $url["path"];
    $port  = ($url["port"] === NULL)  ? $port_default : (int)$url["port"];
    $query = ($url["query"] === NULL) ? ""            : "?" . $url["query"];

    //authorization
    $authorization = "";

    if ($url["user"] !== NULL)
    {
        $authorization = $url["user"];
        if (isset($url["pass"])) $authorization .= ":" . $url["pass"];
        $authorization = "Authorization: Basic " . base64_encode($authorization) . "\r\n";
    };

    //connect
    $hfile = stream_socket_client(
        $transport . $host . ":$port",
        $error_number,
        $error,
        CMS_HTTP_TIMEOUT_TOTAL,
        STREAM_CLIENT_CONNECT,
        $context);
    if ($hfile === FALSE) return FALSE; //connection error

    //prepare request
    if (is_array($post_data))
    {
        $command = "POST";
        list($head, $body) = http_build_post($post_data);
    }
    else
    {
        //$command = $get_header_only ? "HEAD" : "GET";
        $command = "GET";
        $head    = "";
        $body    = [];
    };

    //send request header
    $status = http_send(
        $hfile,
        "$command $path$query HTTP/1.1\r\n" .
        "Host: $host\r\n" .
        "User-Agent: " . CMS_USER_AGENT . "\r\n" .
        $authorization .
        "Accept-Encoding: identity\r\n" .
        "Connection: close\r\n" .
        $head .
        "\r\n");

    //send request body
    if ($status !== FALSE)
    {
        foreach ($body AS $value)
        {
            $status = http_send($hfile, $value);
            if ($status === FALSE) break;
        };
    };

    if ($status === FALSE) { fclose($hfile); return FALSE; }; //error

    //receive http status code
    for ($i = 0; $i < 10; $i++)
    {
        $_header = http_fetch_header($hfile);
        if ($_header === FALSE) { fclose($hfile); return FALSE; }; //error

        //check http status code
        if (! preg_match("/^(HTTP\/1\.\d)\s+([1-5]\d{2})(?:\s+(.+))?/", $_header, $match))
        {
            //invalid http status
            fclose($hfile);
            return FALSE;
        };

        //continue on 1xx status
        if ($match[2] >= 200) break;
    };

    //store status
    $header = ["#version" => $match[1],
               "#status"  => $match[2]];
    if (isset($match[3])) $header["#message"] = $match[3];

    //retrieve header fields
    while (($_header = http_fetch_header($hfile)) !== FALSE)
    {
        //check for end of header (empty line)
        if ($_header === "") break; //end of header

        //keep header field
        if ($offset = strpos($_header, ":"))
        {
            $key   = strtolower(substr($_header, 0, $offset));
            $value = ltrim(substr($_header, $offset + 1));
            if (streq($key, "set-cookie")) $header[$key][]  = $value;
            elseif (isset($header[$key]))  $header[$key]   .= ", " . $value;
            else                           $header[$key]    = $value;
        };
    };

    //check transfer encoding for chunked
    if (
         isset($header["transfer-encoding"])
         &&
         preg_match("/\bchunked\b/i", $header["transfer-encoding"])
       )
        http_chunked($hfile, TRUE);

    //return connection
    return $hfile;
};

//--------------------------------------------------------------------------------------------------

function http_send($hfile,
                   $data)
{
    $length = strlen($data); //data size
    $offset = 0;             //data offset
    $time   = time();        //chunk time

    stream_set_blocking($hfile, FALSE);

    while ($offset < $length)
    {
        //send data
        $_length = fwrite($hfile, substr($data, $offset, CMS_HTTP_SIZE_CHUNK));
        if ($_length === FALSE) return FALSE; //error

        if ($_length === 0)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //advance offset
        $offset += $_length;
    };

    stream_set_blocking($hfile, TRUE);

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function http_fetch_header($hfile)
{
    $time = time(); //chunk time

    stream_set_blocking($hfile, FALSE);

    while (! feof($hfile))
    {
        //retrieve single line
        $buffer = stream_get_line($hfile, CMS_HTTP_SIZE_CHUNK, "\n");

        if ($buffer === FALSE)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        if (($buffer === "") || ($buffer === "\r")) return ""; //end of header

        $buffer = trim($buffer);
        if ($buffer === "") continue; //ignore
        return $buffer; //data
    };

    stream_set_blocking($hfile, TRUE);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function http_fetch_data($hfile)
{
    stream_set_blocking($hfile, FALSE);
    $buffer = http_get_contents($hfile, CMS_HTTP_LIMIT);

    fclose($hfile);
    http_chunked($hfile); //delete

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function http_chunked($hfile,
                      $set = FALSE)
{
    static $array = [];

    $id = (int)$hfile;

    //flag as chunked
    if ($set !== FALSE)
    {
        $array[$id] = [$hfile, is_int($set) ? $set : 0];
        return TRUE;
    };

    //check if flagged
    if (! isset($array[$id])) return FALSE; //not found

    //check if identical
    if ($array[$id][0] !== $hfile)
    {
        //flagged resource has been closed
        unset($array[$id]);
        return FALSE;
    };

    //resource has been closed
    if (! is_resource($hfile))
    {
        //remove
        unset($array[$id]);
        return NULL;
    };

    return $array[$id][1]; //flagged
};

//--------------------------------------------------------------------------------------------------

function http_get_contents($hfile,
                           $length = NULL)
{
    //retrieve remaining value
    $remaining = http_chunked($hfile);
    if ($remaining === NULL) return FALSE; //invalid or closed handle

    $chunked = $remaining !== FALSE;
    if (! $chunked) $remaining = CMS_HTTP_SIZE_CHUNK;

    $buffer  = "";     //data buffer
    $_length = 0;      //data buffer length
    $time    = time(); //total time
    $_time   = $time;  //chunk time

    while (($length === NULL) || ($_length < $length))
    {
        //start of chunk
        if ($chunked && ($remaining === 0))
        {
            //get chunk size
            $size = stream_get_line($hfile, 1024, "\n");
            if ($size === FALSE) return FALSE; //error

            //check chunk size
            $size = trim($size);
            if (! ctype_xdigit($size)) return FALSE; //invalid

            $remaining = hexdec($size);
            if ($remaining === 0) return $buffer; //end of stream
        };

        $__length = ($length === NULL) ? $remaining : min($length - $_length, $remaining);

        //retrieve data chunk
        $_buffer = stream_get_contents($hfile, $__length);
        if ($_buffer === FALSE) return FALSE; //error

        if ($_buffer === "") //no data
        {
            if (feof($hfile)) break; //end of stream

            $__time = time(); //current time

            //check timeout
            if (
                 (($__time - $time) > CMS_HTTP_TIMEOUT_TOTAL)
                 ||
                 (($__time - $_time) > CMS_HTTP_TIMEOUT_CHUNK)
               )
                return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //append to buffer
        $buffer    .= $_buffer;
        $__length   = strlen($_buffer);
        $_length   += $__length;
        $remaining -= $__length;

        //chunk has been completed
        if ($remaining === 0)
        {
            if ($chunked) stream_get_line($hfile, 1024, "\n"); //skip end bytes
            else          $remaining = CMS_HTTP_SIZE_CHUNK;    //reset chunk size
        };

        //reset chunk time
        $_time = time();
    };

    //store remaining value
    if ($chunked) http_chunked($hfile, $remaining);

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function http_header($url)
{
    $hfile = http_fopen($url, NULL, $header);
    if ($hfile === FALSE) return FALSE;
    fclose($hfile);
    return $header;
};

//--------------------------------------------------------------------------------------------------

function http_post($url,
                   $data,
                   &$header = NULL)
{
    if (! is_array($data)) return FALSE; //no post data

    //connect and send data
    $hfile = http_fopen($url, $data, $header);
    if ($hfile === FALSE) return FALSE;

        //fetch and return reply
    return http_fetch_data($hfile);
};

//--------------------------------------------------------------------------------------------------

function http_build_post($data)
{
    //overload method so it treats file arrays as leaves
    $iterator = new \RecursiveIteratorIterator(
        new class($data) extends \RecursiveArrayIterator
        {
            public function hasChildren(): bool
            {
                $value = $this->current();
                return is_array($value) && (! isset($value["data"]));
            }
        },
        \RecursiveIteratorIterator::LEAVES_ONLY);

    $body     = [];
    $boundary = unique_id() . "-" . unique_id() . "-" . unique_id() . "-" . unique_id();

    //process data
    foreach ($iterator as $value)
    {
        //reconstruct php array notation
        $depth = $iterator->getDepth();
        $name  = (string)$iterator->getSubIterator(0)->key();
        for ($i = 1; $i <= $depth; $i++)
            $name .= "[" . (string)$iterator->getSubIterator($i)->key() . "]";

        //file
        if (is_array($value) && isset($value["data"]))
        {
            $filename = $value["filename"] ?? $value["name"] ?? $name;
            $type     = $value["type"] ?? get_mime_type($filename);

            $body[] = "--$boundary\r\n" .
                      "Content-Disposition: form-data; name=" . quote_http_param($name) .
                      (($filename !== "") ? "; filename=" . quote_http_param($filename) : "") . "\r\n" .
                      "Content-Type: $type\r\n" .
                      "Content-Transfer-Encoding: base64\r\n" .
                      "\r\n";

            //important: memory optimization, keep in separate line
            if ($value["data"] !== "") $body[] = $value["data"];

            $body[] = "\r\n";
            continue;
        };

        //scalar
        $body[] = "--$boundary\r\n" .
                  "Content-Disposition: form-data; name=" . quote_http_param($name) . "\r\n" .
                  "\r\n" .
                  "$value\r\n";
    };

    $body[] = "--$boundary--\r\n"; //end of body

    //compute content length
    $length = 0;
    foreach ($body AS $value) $length += strlen($value);

    //header
    $head = "Content-Type: multipart/form-data; boundary=$boundary\r\n" .
            "Content-Length: $length\r\n";

    return [$head, $body];
};