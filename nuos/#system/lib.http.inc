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
                    $get_header_only = FALSE,
                    $post_data       = NULL)
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

        //create name-value-pair string
        $body = "";
        foreach ($post_data AS $key => $value)
            $body .= r($key) . "=" . r($value) . "&";
        $body = substr($body, 0, -1);

        $head = "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n" .
                "Content-Length: " . strlen($body) . "\r\n";
    }
    else
    {
        //$command = $get_header_only ? "HEAD" : "GET";
        $command = "GET";
        $head    = "";
        $body    = "";
    };

    //send request
    $status = http_send(
        $hfile,
        "$command $path$query HTTP/1.1\r\n" .
        "Host: $host\r\n" .
        "User-Agent: " . CMS_USER_AGENT . "\r\n" .
        $authorization .
        "Accept-Encoding: identity\r\n" .
        "Connection: close\r\n" .
        $head .
        "\r\n" .
        $body);

    if ($status === FALSE) { fclose($hfile); return FALSE; }; //error

    //receive http status code
    for ($i = 0; $i < 10; $i++)
    {
        $header = http_fetch_header($hfile);
        if ($header === FALSE) { fclose($hfile); return FALSE; }; //error

        //check http status code
        if (! preg_match("/^HTTP\/1\.[0-9]\s+([1-4][0-9]{2})/i", $header, $_header))
        {
            //invalid http status
            fclose($hfile);
            return FALSE;
        };

        //continue on 1xx status
        if ((int)$_header[1] >= 200) break;
    };

    //store status
    $return = ["#status" => $_header[1]];

    //retrieve header fields
    while (($header = http_fetch_header($hfile)) !== FALSE)
    {
        //check for end of header (empty line)
        if ($header === "") break; //end of header

        //keep header field
        if ($offset = strpos($header, ":"))
        {
            $key   = strtolower(substr($header, 0, $offset));
            $value = ltrim(substr($header, $offset + 1));
            if (streq($key, "set-cookie")) $return[$key][]  = $value;
            elseif (isset($return[$key]))  $return[$key]   .= ", " . $value;
            else                           $return[$key]    = $value;
        };
    };

    //return header information
    if ($get_header_only) { fclose($hfile); return $return; };

    //check transfer encoding for chunked
    if (
         isset($return["transfer-encoding"])
         &&
         preg_match("/\bchunked\b/i", $return["transfer-encoding"])
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
    $time   = time();        //chunk time

    stream_set_blocking($hfile, FALSE);

    while ($length > 0)
    {
        //send data
        $_length = fwrite($hfile, $data);
        if ($_length === FALSE) return FALSE; //error

        if ($_length === 0)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //extract remaining data
        $length -= $_length;
        $data    = substr($data, -$length);
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
            else          $remaining = CMS_HTTP_SIZE_CHUNK;   //reset chunk size
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
    return http_fopen($url, TRUE);
};

//--------------------------------------------------------------------------------------------------

function http_post($url,
                   $data)
{
    if (! is_array($data)) return FALSE; //no post data

    //connect and send data
    $hfile = http_fopen($url, FALSE, $data);
    if ($hfile === FALSE) return FALSE;

    //fetch and return reply
    return http_fetch_data($hfile);
};