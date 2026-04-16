<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   SMTP CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function smtp_send($to,
                   $subject,
                   $body,
                   $html     = FALSE,
                   $reply_to = NULL)
{
    //build message
    if (! cms_load("mime")) return FALSE;
    $mime = new mime();

    //html message
    if ($html)
    {
        $key = $mime->add_multipart("alternative", //subtype
                                    $subject);     //subject

        $mime->add_text(NULL,                      //subject
                        $body,                     //body
                        "html",                    //subtype
                        CMS_MIME_CHARSET_UTF_8,    //charset
                        $key);                     //parent_key

        $mime->add_text(NULL,                      //subject
                        htmltoplain($body, TRUE),  //body
                        "plain",                   //subtype
                        CMS_MIME_CHARSET_UTF_8,    //charset
                        $key);                     //parent_key
    }

    //plain message
    else
    {
        $mime->add_text($subject, //subject
                        $body);   //body
    };

    //open connection
    $smtp = new smtp();
    if (! $smtp->enabled) return FALSE;

    //send message
    $return = $smtp->send($mime,           //mime
                          CMS_EMAIL_AGENT, //from
                          $to,             //to
                          NULL,            //cc
                          NULL,            //bcc
                          $reply_to);      //reply_to

    //close connection
    $smtp->quit();

    return $return;
};

class smtp
{

//properties
//--------------------------------------------------------------------------------------------------

    public $hfile    = NULL;
    public $enabled  = NULL;
    public $mail     = NULL;
    public $username = NULL;
    public $password = NULL;
    public $response = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($host     = NULL,
                         $username = NULL,
                         $password = NULL)
    {
        $this->enabled = TRUE;

        if (stre($host))
        {
            $system = new system();

            //use mail function
            if ($this->mail = streq($system->getval("email", "method"), "mail")) return;

            $host     = $system->getval("email", "smtp");
            $username = $system->getval("email", "username");
            $password = $system->getval("email", "password");
        };

        $this->username = $username;
        $this->password = $password;

        //open connection
        if (! $this->hfile = fsockopen($host, 25))
        {
            $this->response = sprintf(CMS_L_SMTP_001, $host);
            $this->enabled  = FALSE;
            return;
        };

        if (

        //initial response
             ($line = $this->receive_line())
             &&
             ($line[0] == 220)
             &&

        //authentication
             $this->authenticate()
           )
            return;

        fclose($this->hfile);
        $this->enabled = FALSE;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function authenticate()
    {
        if ($this->mail) return FALSE;

        //initial handshake
        if (! $response = $this->send_line(250, "EHLO " . CMS_DOMAIN))
            return (bool)$this->send_line(250, "HELO " . CMS_DOMAIN);

        //starttls support
        if (
             extension_loaded("openssl")
             &&
             preg_match("/(?:^|\r\n)STARTTLS(?:$|\r\n)/i", $response)
           )
        {
            if ($this->send_line(220, "STARTTLS"))
            {
                //enable encryption
                if (! stream_socket_enable_crypto($this->hfile, TRUE, STREAM_CRYPTO_METHOD_TLS_CLIENT))
                    return FALSE;

                //reinitiate handshake
                if (! $response = $this->send_line(250, "EHLO " . CMS_DOMAIN))
                    return FALSE;
            };
        };

        //get sasl authentication methods
        if (! preg_match("/(?:^|\r\n)AUTH (.*)(?:$|\r\n)/i", $response, $response))
            return TRUE;

        $response = $response[1];

        //anonymous access
        if (preg_match("/(?:^| )ANONYMOUS(?:$| )/i", $response))
            return TRUE;

        //cram-md5 authentication
        if (preg_match("/(?:^| )CRAM-MD5(?:$| )/i", $response))
        {
            if ($message = $this->send_line(334, "AUTH CRAM-MD5"))
            {
                $message = base64_decode($message);
                $message = hmac_md5($message, $this->password);
                $message = base64_encode($this->username . " " . $message);

                if ($this->send_line(235, $message)) return TRUE;
            };
        }

        //login authentication
        if (preg_match("/(?:^| )LOGIN(?:$| )/i", $response))
        {
            if (
                 $this->send_line(334, "AUTH LOGIN")
                 &&
                 $this->send_line(334, base64_encode($this->username))
                 &&
                 $this->send_line(235, base64_encode($this->password))
               )
                return TRUE;
        };

        //plain authentication
        if (preg_match("/(?:^| )PLAIN(?:$| )/i", $response))
        {
            if ($this->send_line(235, "AUTH PLAIN " .
                                      base64_encode("\0" . $this->username .
                                                    "\0" . $this->password)))
                return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function send(&$mime,
                  $from     = NULL,
                  $to       = NULL,
                  $cc       = NULL,
                  $bcc      = NULL,
                  $reply_to = NULL)
    {
        if (! $this->enabled)   return FALSE;
        if (! cms_load("mime")) return FALSE;

        $key = $mime->data->move("first");

        //get sender address
        $sender = $from ? $from : $mime->data->get($key, "from");
        $sender = mime_extract_rfc2822_address($sender);
        $sender = key(reset($sender));

        if ($this->mail)
        {
            //get recipient address
            if (! $to) $to = $mime->data->get($key, "to");

            //extract and remove subject
            $subject = mime_encode_header($mime->data->get($key, "subject"));
            $mime->data->del($key, "subject");

            //build message
            $message = $mime->build($from, NULL, $cc, $bcc, $reply_to);
            if (preg_match("/^win/i", PHP_OS)) //windows only
                $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

            //split header and body
            list($header, $body) = explode("\r\n\r\n", $message, 2);

            //send
            if (mail($to, $subject, $body, $header, escapeshellarg("-f" . $sender))) return TRUE;

            //error
            $this->response = "The mail() function failed.";
            return FALSE;
        };

        //initiate message
        if (! $this->send_line(250, "MAIL FROM:<$sender>")) return FALSE;

        //get recipient address
        $recipient = ($to  ? $to  : $mime->data->get($key, "to")) . "," .
                     ($cc  ? $cc  : $mime->data->get($key, "cc")) . "," .
                     ($bcc ? $bcc : $mime->data->get($key, "bcc"));
        $recipient = mime_extract_rfc2822_address($recipient);

        foreach ($recipient AS $key => $value)
        {
            foreach ($value AS $key => $_)
            {
                if (! $this->send_line(250, "RCPT TO:<$key>")) return FALSE;
            };
        };

        //build message
        $message = $mime->build($from, $to, $cc, $bcc, $reply_to);
        $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

        //send
        return $this->send_line(354, "DATA")
               &&
               fwrite($this->hfile, $message)
               &&
               $this->send_line(250, ".");
    }

//--------------------------------------------------------------------------------------------------

    function receive_line()
    {
        if (! $this->enabled) return FALSE;
        if ($this->mail)      return FALSE;

        $message = NULL;

        while (
                ($response = fgets($this->hfile, 1024))
                &&
                preg_match("/^([0-9]{3})(-| )(.*)\r\n$/", $response, $response)
              )
        {
            $code     = $response[1];
            $message .= $response[3] . "\r\n";

            if ($response[2] === " ")
            {
                $message        = substr($message, 0, -2);
                $this->response = "$code $message";
                return [0 => $code, $message];
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function send_line($code,
                       $message)
    {
        if (! $this->enabled) return FALSE;
        if ($this->mail)      return FALSE;

        fwrite($this->hfile, "$message\r\n");
        $message = $this->receive_line();

        return ($message[0] == $code) ?
               ($message[1] ? $message[1] : TRUE) :
               FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function quit()
    {
        if ($this->mail) return TRUE;

        if ($this->enabled)
        {
            $this->enabled = FALSE;
            $this->send_line(221, "QUIT");
            fclose($this->hfile);
        };

        return TRUE;
    }
};

?>