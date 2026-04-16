<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CONTENT_POOL CLASS
//==================================================================================================

//permission
define("CMS_CONTENT_POOL_PERMISSION_OPERATOR", "pool.operator");

//related functions
//--------------------------------------------------------------------------------------------------

function content_pool_get_array($type = NULL)
{
    $data  = new data("#system/content.pool");
    $array = ["" => []];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if (($type !== NULL) && nstreq($type, $data->get($key, "type"))) continue; //inapplicable type

        $category = $data->get($key, "category");
        if (stre($category)) $category = "";
        $name     = l($data->get($key, "name"));
        if (stre($name))     $name     = $key;

        $_name    = $name;
        $i        = 1;
        while (isset($array[$category][$_name]))
            $_name = $name . " (" . $i++ . ")";

        $array[$category][$_name] = $key;
    };

    ksort($array);
    foreach ($array AS $key => $_)
        if (is_array($array[$key])) ksort($array[$key]);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function content_pool_get_select()
{
    $data  = new data("#system/content.pool");
    $array = ["" => ""];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $category                                  = $data->get($key, "category");
        $array[($category != "") ? $category : ""] = $category;
    };

    ksort($array);

    return $array;
};

class content_pool
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data     = NULL;
    public $operator = NULL;
    public $mysql    = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data     = new data("#system/content.pool");
        $this->operator = cms_permission(CMS_CONTENT_POOL_PERMISSION_OPERATOR);

        mkpath(CMS_DATA_PATH . "#content/pool");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($name,
                 $category,
                 $content_index,
                 $range,
                 $type)
    {
        if (! $this->operator) return FALSE;

        $name     = utf8_trim($name);
        $category = utf8_trim($category);

        if (stre($name)) $name = CMS_L_CONTENT_POOL_001;

        $this->data->set_buffer(
            [["name"     => $name,
              "category" => $category,
              "index"    => $content_index,
              "range"    => $range,
              "type"     => $type]]);

        if (
             (($index = $this->data->insert()) !== FALSE)
             &&
             $this->synchronize($index)
             &&
             $this->data->save()
           )
            return $index;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $name,
                 $category,
                 $content_index,
                 $range,
                 $type)
    {
        if (! $this->operator) return FALSE;

        $name     = utf8_trim($name);
        $category = utf8_trim($category);

        if (stre($name)) $name = CMS_L_CONTENT_POOL_001;

        //back up current values
        $_content_index = $this->data->get($index, "index");
        $_range         = $this->data->get($index, "range");
        $_type          = $this->data->get($index, "type");

        //set new values
        $this->data->set($name,          $index, "name");
        $this->data->set($category,      $index, "category");
        $this->data->set($content_index, $index, "index");
        $this->data->set($range,         $index, "range");
        $this->data->set($type,          $index, "type");

        //resynchronize on reference change
        if (
             nstreq($content_index, $_content_index)
             ||
             nstreq($range, $_range)
             ||
             nstreq($type, $_type)
           )
            $this->synchronize($index);

        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function get($index)
    {
        return $this->data->get($index);
    }

//--------------------------------------------------------------------------------------------------

    function get_text($index)
    {
        //retrieve from temporary cache
        if (($text = cms_cache("content.pool.$index")) === NULL)
        {
            //retrieve from file
            $text = read_file(CMS_DATA_PATH . "#content/pool/$index");

            //temporary caching
            cms_cache("content.pool.$index", $text);
        };

        return $text;
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->operator) return FALSE;
        if (stre($index))      return FALSE;

        if (is_file(CMS_DATA_PATH . "#content/pool/$index"))
        {
            if (! unlink(CMS_DATA_PATH . "#content/pool/$index")) return FALSE;

            //remove from temporary cache
            cms_cache("content.pool.$index", "");
        };

        $this->data->del($index);

        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function synchronize($index)
    {
        if (! cms_load("document")) return FALSE;

        //establish database connection
        if ($this->mysql === NULL) $this->mysql = new mysql();

        if (

        //reference exists
             (($content_index = $this->data->get($index, "index")) !== NULL)
             &&

        //retrieve content from database
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_TEXT . ", " .
                                                CMS_DB_CONTENT_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($content_index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
        {
            //process content
            $document = new document($resultrow[CMS_DB_CONTENT_TEXT],
                                     $resultrow[CMS_DB_CONTENT_TEMPLATE]);

            //extract range
            $range    = $this->data->get($index, "range");
            $type     = $this->data->get($index, "type");
            $text     = NULL;

            if ($document->get($range, $type) !== NULL)
            {
                $document = $document->extract($range);
                $text     = $document->export();
            };

            //write file
            if ($hfile = fopen(CMS_DATA_PATH . "#content/pool/$index", "wb"))
            {
                flock($hfile, LOCK_EX);
                fwrite($hfile, $text);
                fclose($hfile);

                //remove from temporary cache to save memory
                //cache will be reestablished when required
                cms_cache("content.pool.$index", "");

                return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function synchronize_content($content_index)
    {
        if (! cms_load("document")) return FALSE;

        //generate list of referenced contents
        $array = [];

        $this->data->move("first");
        while (($index = $this->data->move("next")) !== NULL)
        {
            $_content_index = $this->data->get($index, "index");
            if (streq($_content_index, $content_index)) $array[] = $index;
        };

        if (empty($array)) return TRUE; //no elements to synchronize

        //establish database connection
        if ($this->mysql === NULL) $this->mysql = new mysql();

        //retrieve contents from database
        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_TEXT . ", " .
                                                CMS_DB_CONTENT_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($content_index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
        {
            //process content
            $document = new document($resultrow[CMS_DB_CONTENT_TEXT],
                                     $resultrow[CMS_DB_CONTENT_TEMPLATE]);

            //process references
            foreach ($array AS $index)
            {
                //extract range
                $range = $this->data->get($index, "range");
                $type  = $this->data->get($index, "type");
                $text  = NULL;

                if ($document->get($range, $type) !== NULL)
                {
                    $_document = $document->extract($range);
                    $text      = $_document->export();
                };

                //write file
                if ($hfile = fopen(CMS_DATA_PATH . "#content/pool/$index", "wb"))
                {
                    flock($hfile, LOCK_EX);
                    fwrite($hfile, $text);
                    fclose($hfile);

                    //remove from temporary cache to save memory
                    //cache will be reestablished when required
                    cms_cache("content.pool.$index", "");
                };
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function synchronize_all()
    {
        if (! cms_load("document")) return FALSE;

        //generate list of referenced contents
        $array = [];
        $list  = "";

        $this->data->move("first");
        while (($index = $this->data->move("next")) !== NULL)
        {
            $content_index = $this->data->get($index, "index");

            if (isset($array[$content_index]))
            {
                $array[$content_index][] = $index;
                continue;
            };

            $array[$content_index]  = [$index];
            $list                  .= "'" . sqlesc($content_index) . "', ";
        };

        if (empty($array)) return TRUE; //no elements to synchronize

        //establish database connection
        if ($this->mysql === NULL) $this->mysql = new mysql();

        //retrieve contents from database
        if ($result = mysql_query("SELECT " . CMS_DB_CONTENT_INDEX . ", " .
                                              CMS_DB_CONTENT_TEXT . ", " .
                                              CMS_DB_CONTENT_TEMPLATE . " " .
                                  "FROM " .   CMS_DB_CONTENT . " " .
                                  "WHERE " .  CMS_DB_CONTENT_INDEX . " " .
                                  "IN (" .    substr($list, 0, -2) . ")"))
        {
            //process contents
            while ($resultrow = mysql_fetch_assoc($result))
            {
                $document      = new document($resultrow[CMS_DB_CONTENT_TEXT],
                                              $resultrow[CMS_DB_CONTENT_TEMPLATE]);
                $content_index = $resultrow[CMS_DB_CONTENT_INDEX];

                //process references
                foreach ($array[$content_index] AS $index)
                {
                    //extract range
                    $range = $this->data->get($index, "range");
                    $type  = $this->data->get($index, "type");
                    $text  = NULL;

                    if ($document->get($range, $type) !== NULL)
                    {
                        $_document = $document->extract($range);
                        $text      = $_document->export();
                    };

                    //write file
                    if ($hfile = fopen(CMS_DATA_PATH . "#content/pool/$index", "wb"))
                    {
                        flock($hfile, LOCK_EX);
                        fwrite($hfile, $text);
                        fclose($hfile);

                        //remove from temporary cache to save memory
                        //cache will be reestablished when required
                        cms_cache("content.pool.$index", "");
                    };
                };
            };
        };
    }
};

?>