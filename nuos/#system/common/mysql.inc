<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

define("CMS_MYSQL_IMPROVED", extension_loaded("mysqli"));
$cms_mysql_connection = NULL;

//result type constants
define("MYSQL_ASSOC", 1);
define("MYSQL_NUM",   2);
define("MYSQL_BOTH",  3);

function mysql_get_link_identifier($link_identifier = NULL)
{
    global $cms_mysql_connection;

    //mysqli
    if (CMS_MYSQL_IMPROVED)
    {
        //disable mysql error reporting as of php 8.1.0
        mysqli_report(MYSQLI_REPORT_OFF);

        if (
             is_object($link_identifier)
             &&
             streq(get_class($link_identifier), "mysqli")
           )
            return $link_identifier;

        if (
             is_object($cms_mysql_connection)
             &&
             streq(get_class($cms_mysql_connection), "mysqli")
           )
            return $cms_mysql_connection;

        return NULL;
    };

    //mysql
    if (is_resource($link_identifier))      return $link_identifier;
    if (is_resource($cms_mysql_connection)) return $cms_mysql_connection;
    return NULL;
};

//--------------------------------------------------------------------------------------------------

function mysql_affected_rows($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_affected_rows($link_identifier) :
           \mysql_affected_rows($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_client_encoding($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_character_set_name($link_identifier) :
           \mysql_client_encoding($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_close($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_close($link_identifier) :
           \mysql_close($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_connect($server      = NULL,
                       $username    = NULL,
                       $password    = NULL,
                       $new_link    = NULL,
                       $client_flag = NULL)
{
    global $cms_mysql_connection;

    if (CMS_MYSQL_IMPROVED)
    {
        $port   = NULL;
        $socket = NULL;

        if ($extension = strstr($server, ":"))
        {
            $server    = substr($server, 0, -strlen($extension));
            $extension = substr($extension, 1);

            if (is_numeric($extension)) $port   = $extension;
            else                        $socket = $extension;
        };

        $cms_mysql_connection = mysqli_connect($server, $username, $password, NULL, $port, $socket);
        return $cms_mysql_connection;
    };

    $cms_mysql_connection = \mysql_connect($server, $username, $password, $new_link, $client_flag);
    return $cms_mysql_connection;
};

//--------------------------------------------------------------------------------------------------

function mysql_data_seek($result,
                         $row_number)
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_data_seek($result, $row_number) :
           \mysql_data_seek($result, $row_number);
};

//--------------------------------------------------------------------------------------------------

function mysql_db_name($result,
                       $row,
                       $field = NULL)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_data_seek($result, $row)) return FALSE;
        $resultrow = mysqli_fetch_array($result);
        return ($field === NULL) ? $resultrow[0] : $resultrow[$field];
    };

    return \mysql_db_name($result, $row, $field);
};

//--------------------------------------------------------------------------------------------------

function mysql_errno($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_errno($link_identifier) :
           \mysql_errno($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_error($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_error($link_identifier) :
           \mysql_error($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_array($result,
                           $result_type = MYSQL_BOTH)
{
    return CMS_MYSQL_IMPROVED ?
           (mysqli_fetch_array($result, $result_type) ?: FALSE) :
           \mysql_fetch_array($result, $result_type);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_assoc($result)
{
    return CMS_MYSQL_IMPROVED ?
           (mysqli_fetch_assoc($result) ?: FALSE) :
           \mysql_fetch_assoc($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_field($result,
                           $field_offset = NULL)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if ($field_offset !== NULL) mysqli_field_seek($result, $field_offset);
        return mysqli_fetch_field($result) ?: FALSE;
    };

    return \mysql_fetch_field($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_lengths($result)
{
    return CMS_MYSQL_IMPROVED ?
           (mysqli_fetch_lengths($result) ?: FALSE) :
           \mysql_fetch_lengths($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_object($result,
                            $class_name = NULL,
                            $params     = NULL)
{
    return CMS_MYSQL_IMPROVED ?
           (mysqli_fetch_object($result, $class_name ?? "stdClass", $params) ?: FALSE) :
           \mysql_fetch_object($result, $class_name ?? "stdClass", $params);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_row($result)
{
    return CMS_MYSQL_IMPROVED ?
           (mysqli_fetch_row($result) ?: FALSE) :
           \mysql_fetch_row($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_flags($result,
                           $field_offset)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_field_seek($result, $field_offset)) return FALSE;
        $field = mysqli_fetch_field($result);
        return $field ? $field->flags : FALSE; //omitting string conversion
    };

    return \mysql_field_flags($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_len($result,
                         $field_offset)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_field_seek($result, $field_offset)) return FALSE;
        $field = mysqli_fetch_field($result);
        return $field ? $field->length : FALSE;
    };

    return \mysql_field_len($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_name($result,
                          $field_offset)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_field_seek($result, $field_offset)) return FALSE;
        $field = mysqli_fetch_field($result);
        return $field ? $field->name : FALSE;
    };

    return \mysql_field_name($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_seek($result,
                          $field_offset)
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_field_seek($result, $field_offset) :
           \mysql_field_seek($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_table($result,
                           $field_offset)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_field_seek($result, $field_offset)) return FALSE;
        $field = mysqli_fetch_field($result);
        return $field ? $field->table : FALSE;
    };

    return \mysql_field_table($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_type($result,
                          $field_offset)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_field_seek($result, $field_offset)) return FALSE;
        $field = mysqli_fetch_field($result);
        return $field ? $field->type : FALSE; //omitting string conversion
    };

    return \mysql_field_type($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_free_result($result)
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_free_result($result) :
           \mysql_free_result($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_client_info()
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_get_client_info() :
           \mysql_get_client_info();
};

//--------------------------------------------------------------------------------------------------

function mysql_get_host_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_get_host_info($link_identifier) :
           \mysql_get_host_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_proto_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_get_proto_info($link_identifier) :
           \mysql_get_proto_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_server_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_get_server_info($link_identifier) :
           \mysql_get_server_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_info($link_identifier) :
           \mysql_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_insert_id($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_insert_id($link_identifier) :
           \mysql_insert_id($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_list_dbs($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_query($link_identifier, "SHOW DATABASES") :
           \mysql_list_dbs($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_list_processes($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_query($link_identifier, "SHOW PROCESSLIST") :
           \mysql_list_processes($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_num_fields($result)
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_num_fields($result) :
           \mysql_num_fields($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_num_rows($result)
{
    return CMS_MYSQL_IMPROVED ?
           mysqli_num_rows($result) :
           \mysql_num_rows($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_pconnect($server      = NULL,
                        $username    = NULL,
                        $password    = NULL,
                        $client_flag = NULL)
{
    if (CMS_MYSQL_IMPROVED)
    {
        $port   = NULL;
        $socket = NULL;

        if ($extension = strstr($server, ":"))
        {
            $server    = substr($server, 0, -strlen($extension));
            $extension = substr($extension, 1);

            if (is_numeric($extension)) $port   = $extension;
            else                        $socket = $extension;
        };

        $cms_mysql_connection = mysqli_connect("p:$server", $username, $password, NULL, $port, $socket);
        return $cms_mysql_connection;
    };

    $cms_mysql_connection = \mysql_pconnect($server, $username, $password, $client_flag);
    return $cms_mysql_connection;
};

//--------------------------------------------------------------------------------------------------

function mysql_ping($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           (bool)mysqli_query($link_identifier, "SELECT 1") :
           \mysql_ping($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_query($query,
                     $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    $result = CMS_MYSQL_IMPROVED ?
              mysqli_query($link_identifier, $query) :
              \mysql_query($query, $link_identifier);

    if ($result === FALSE) trigger_error(mysql_error($link_identifier) . "; " . $query, E_USER_ERROR);
    return $result;
};

//--------------------------------------------------------------------------------------------------

function mysql_real_escape_string($unescaped_string,
                                  $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_real_escape_string($link_identifier, $unescaped_string) :
           \mysql_real_escape_string($unescaped_string, $link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_result($result,
                      $row,
                      $field = NULL)
{
    if (CMS_MYSQL_IMPROVED)
    {
        if (! mysqli_data_seek($result, $row)) return FALSE;
        $resultrow = mysqli_fetch_array($result);
        if ($resultrow === NULL) return FALSE;
        return ($field === NULL) ? $resultrow[0] : ($resultrow[$field] ?? FALSE);
    };

    return \mysql_result($result, $row, $field);
};

//--------------------------------------------------------------------------------------------------

function mysql_select_db($database_name,
                         $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_select_db($link_identifier, $database_name) :
           \mysql_select_db($database_name, $link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_stat($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_stat($link_identifier) :
           \mysql_stat($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_thread_id($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return CMS_MYSQL_IMPROVED ?
           mysqli_thread_id($link_identifier) :
           \mysql_thread_id($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_unbuffered_query($query,
                                $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);

    if (CMS_MYSQL_IMPROVED)
    {
        if (mysqli_real_query($link_identifier, $query))
            return mysqli_field_count($link_identifier) ?
                   mysqli_use_result($link_identifier) : TRUE;
        $result = FALSE;
    }
    else
    {
        $result = \mysql_unbuffered_query($query, $link_identifier);
    };

    if ($result === FALSE) trigger_error(mysql_error($link_identifier) . "; " . $query, E_USER_ERROR);
    return $result;
};

?>