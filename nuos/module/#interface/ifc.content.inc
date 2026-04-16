<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//load libraries
if (! cms_load("content"))      ifc_inactive($ifc_page);
if (! cms_load("content_pool")) ifc_inactive($ifc_page);
if (! cms_load("directory"))    ifc_inactive($ifc_page);
if (! cms_load("document"))     ifc_inactive($ifc_page);
if (! cms_load("flexview"))     ifc_inactive($ifc_page);
if (! cms_load("template"))     ifc_inactive($ifc_page);

//load settings
if (stre($user ?? ""))           $user              = cms_cache("content." . CMS_USER . ".user");
if (! isset($object))            $object            = cms_cache("content." . CMS_USER . ".object");
if (! isset($directory_object))  $directory_object  = cms_cache("directory." . CMS_USER . ".object");
if (! isset($sql_select))        $sql_select        = cms_cache("content." . CMS_USER . ".sql_select");
if (! isset($sql_filter_field))  $sql_filter_field  = cms_cache("content." . CMS_USER . ".sql_filter_field");
if (! isset($sql_filter_option)) $sql_filter_option = cms_cache("content." . CMS_USER . ".sql_filter_option");
if (! isset($sql_filter_value))  $sql_filter_value  = cms_cache("content." . CMS_USER . ".sql_filter_value");
if (! isset($sql_order))         $sql_order         = cms_cache("content." . CMS_USER . ".sql_order");
if (! isset($page))              $page              = cms_cache("content." . CMS_USER . ".page");
if (! isset($limit))             $limit             = cms_cache("content." . CMS_USER . ".limit");

//check user
if (
     stre($user)
     ||
     (
       nstreq($user, CMS_SUPERUSER)
       &&
       (! cms_permission($user))
     )
   )
    $user = CMS_SUPERUSER;

//permission
$data = new data("#system/permission");
$name = $data->get("user.$user", "name");

ifc_permission(
    [""                                          => CMS_L_ACCESS,
     CMS_CONTENT_PERMISSION_WRITER               => CMS_L_IFC_CONTENT_009,
     CMS_CONTENT_PERMISSION_EDITOR               => CMS_L_IFC_CONTENT_015,
     CMS_CONTENT_PERMISSION_PUBLISHER            => CMS_L_IFC_CONTENT_019,
     $user                                       => "$name " . CMS_L_ACCESS,
     "$user." . CMS_CONTENT_PERMISSION_WRITER    => "$name " . CMS_L_IFC_CONTENT_009,
     "$user." . CMS_CONTENT_PERMISSION_EDITOR    => "$name " . CMS_L_IFC_CONTENT_015,
     "$user." . CMS_CONTENT_PERMISSION_PUBLISHER => "$name " . CMS_L_IFC_CONTENT_019,
     CMS_CONTENT_POOL_PERMISSION_OPERATOR        => CMS_L_IFC_CONTENT_131 . " " . CMS_L_OPERATOR,
     CMS_CONTENT_PERMISSION_OPERATOR             => CMS_L_OPERATOR]);

//instantiate content object
$content = new content($user);
if (! $content->enabled) ifc_inactive($ifc_page);

//initiate primary interface values
if (empty($object))             $object            = NULL;
if (empty($directory_object))   $directory_object  = NULL;
if (empty($sql_select))         $sql_select        = CMS_L_IFC_CONTENT_094;
if (isset($_sql_filter_field))  $sql_filter_field  = $_sql_filter_field;
if (empty($sql_filter_field))   $sql_filter_field  = CMS_DB_CONTENT_TITLE;
if (isset($_sql_filter_option)) $sql_filter_option = $_sql_filter_option;
if (empty($sql_filter_option))  $sql_filter_option = " LIKE '%#value#%'";
if (isset($_sql_filter_value))  $sql_filter_value  = trim($_sql_filter_value);
if (empty($sql_filter_value))   $sql_filter_value  = "";
if (empty($sql_order))          $sql_order         = CMS_DB_CONTENT_WRITER_TIME . " DESC";
if (isset($page))               $page              = (int)$page;
if (empty($page))               $page              = 0;
if (isset($_limit))             $limit             = (int)$_limit;
if (empty($limit))              $limit             = 25;

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;

    //seek selected object in directory
    $data   = new data("#system/directory");
    $length = strlen($object) + 10;

    //go through directory
    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        //check for linkage
        $url = $data->get($key, "url");

        if (
             streq(substr($url, 0, $length), "content://$object")
             &&
             (
               stre($url = substr($url, $length, 1))
               ||
               ($url === "?")
               ||
               ($url === "#")
             )
           )
        {
            //found topmost linkage
            $directory_object = $key;
            break;
        };
    };

    break;

case "directory_select":
//..................................................................................................

    $directory_object = $ifc_param;

    //retrieve selected object from directory
    $data = new data("#system/directory");
    $url  = $data->get($directory_object, "url");

    //directory dereferencing
    $list = [$directory_object => TRUE];
    while (strncmp($url, "directory://", 12) === 0)
    {
        //get directory index
        $key = substr($url, 12, strcspn($url, "?#", 12));

        //check for loop
        if (isset($list[$key])) break;
        $list[$key] = TRUE;

        $url = $data->get($key, "url");
    };

    //linked entry
    if (strncmp($url, "content://", 10) === 0)
    {
        //get content index
        $object = substr($url, 10);
        $object = substr($object, 0, strspn($object, "1234567890"));

        //verify object
        $query  = "SELECT " . CMS_DB_CONTENT_OWNER . " " .
                  "FROM " .   CMS_DB_CONTENT . " " .
                  "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                  "LIMIT 1";
        $result = mysql_query($query);

        //switch to owner if applicable
        if ($resultrow = mysql_fetch_assoc($result))
        {
            $_user = $resultrow[CMS_DB_CONTENT_OWNER];

            if (

            //current user is owner
                 streq($user, $_user)
                 ||
                 (

            //or owner has content access
                   cms_permission(cms_application(), NULL, NULL, $_user)
                   &&

            //and current user can access owner content
                   cms_permission($_user)
                 )
               )
            {
                //switch to owner
                $user = $_user;
                break;
            };
        };
    };

    $object = NULL; //clear
    break;

case "_meta":
//..................................................................................................

    if (

    //retrieve text of active object
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&

    //update active object
         $content->update(
             $object,                                //index
             $ifc_param1,                            //title
             $ifc_param2,                            //description
             $ifc_param3,                            //keyword
             $ifc_param4,                            //image
             $resultrow[CMS_DB_CONTENT_BUFFER_TEXT], //text
             $ifc_param5,                            //comment
             $ifc_param6)                            //template
       )
    {
        if (streq(CMS_IFC_OPTION, "external")) ifc_close_external();

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_param    = $object;
    $ifc_response = CMS_MSG_ERROR;

case "meta":
//..................................................................................................

    $object = $ifc_param;

    if (

    //retrieve meta data of active object
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_WRITER_COMMENT . ", " .
                                            CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                            CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                            CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                            CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                                            CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        //menu
        $menu = streq($ifc_option, "external") ?
            [CMS_L_COMMAND_SAVE . "|content/command_save" => "_meta"] :
            TRUE;

        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       $menu,
                       ["user"              => $user,
                        "object"            => $object,
                        "sql_select"        => $sql_select,
                        "sql_filter_field"  => $sql_filter_field,
                        "sql_filter_option" => $sql_filter_option,
                        "sql_filter_value"  => $sql_filter_value,
                        "sql_order"         => $sql_order,
                        "page"              => $page,
                        "limit"             => $limit],
                       "_meta",
                       CMS_L_IFC_CONTENT_077);

?>
<script>

function content_template_preview()
{
    var value = ifc_get("ifc_param6");
    if (value) load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "template_preview",
                                    "object"      => "\x1B%replace%"]))));?>");
};

</script>
<?php

        $buffer = "javascript:load_page('" .
                  str_replace("%replace%",
                              "'+encodeURIComponent(ifc_get('ifc_param4'))+'",
                              q(cms_url(["ifc_page"          => "image",
                                         "ifc_option"        => "external",
                                         "object"            => "\x1B%replace%",
                                         "ifc_select"        => "object",
                                         "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param4','%return%');"]))) . "');";

        $ifc->set(CMS_L_IFC_CONTENT_027,     "text 50 255 bl",              $resultrow[CMS_DB_CONTENT_BUFFER_TITLE]);
        $ifc->set(CMS_L_IFC_CONTENT_008,     "texteditor 50x8 65535 bilrt", $resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION]);
        $ifc->set(CMS_L_IFC_CONTENT_012,     "text 50 255 bl",              $resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
        $ifc->set(CMS_L_IFC_CONTENT_142,     "title");
        $ifc->set(CMS_L_IMAGE,               "text 40 l",                   $resultrow[CMS_DB_CONTENT_BUFFER_IMAGE]);
        $ifc->set("…",                       "button b",                    $buffer);
        $ifc->set(CMS_L_IFC_CONTENT_017,     "textarea 50x8 65535 b",       $resultrow[CMS_DB_CONTENT_WRITER_COMMENT]);
        $ifc->set(CMS_L_IFC_CONTENT_013,     "label");
        $ifc->set(content_template_select(), "select 40",                   $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);
        $ifc->set(CMS_L_COMMAND_SHOW,        "button b",                    "javascript:content_template_preview();");

        $ifc->close();
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "edit_range":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_range_save();"];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object" => $object,
                    "range"  => $range,
                    "type"   => "text",
                    "left"   => 0,
                    "top"    => 0,
                    "id"     => $id],
                   NULL,
                   CMS_L_COMMAND_EDIT);

    $ifc->set(CMS_L_IFC_CONTENT_028,
              "texteditor 80x20 65535 filrt",
              content_get_range($content, $object, $range, "text"),
              NULL,
              "value");

?>
<script>

function content_edit_range_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script>
<?php

    $ifc->close();

case "edit_value":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_value_save();"];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object" => $object,
                    "range"  => $range,
                    "type"   => "value",
                    "left"   => 0,
                    "top"    => 0,
                    "id"     => $id],
                   NULL,
                   CMS_L_COMMAND_EDIT);

    $ifc->set(CMS_L_IFC_CONTENT_136,
              "textarea 80x20 65535 fl",
              content_get_range($content, $object, $range, "value"),
              NULL,
              "value");

?>
<script>

function content_edit_value_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script>
<?php

    $ifc->close();

case "edit_plugin":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_plugin_save();"];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object" => $object,
                    "range"  => $range,
                    "type"   => "plugin",
                    "left"   => 0,
                    "top"    => 0,
                    "id"     => $id],
                   NULL,
                   CMS_L_COMMAND_EDIT);

    $value = content_get_range($content, $object, $range, "plugin");

    $ifc->set(CMS_L_URL,
              "text 60 4.096 f",
              streq($value, "") ? CMS_PROTOCOL . "://" : $value,
              NULL,
              "value");

?>
<script>

function content_edit_plugin_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        if (ifc_get("value") === "<?php echo(CMS_PROTOCOL);?>://") ifc_del("value");
        ifc_submit();
    };

    window.close();
};

</script>
<?php

    $ifc->close();

case "edit_href":
case "edit_href_select_language":
//..................................................................................................

    if (empty($range))   $range   = NULL;
    if (empty($anchor))  $anchor  = NULL;
    if (empty($value))   $value   = NULL;
    if (empty($_value))  $_value  = NULL;
    if (empty($__value)) $__value = NULL;
    if (empty($id))      $id      = NULL;

    switch (CMS_IFC_MESSAGE)
    {
    case "edit_href_select_language":

        $language = $ifc_param;
        break;

    default:

        if ($range) $value = content_get_range($content, $object, $range, "href");
        $language = "";
    };

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:ifc_return();"];

    //display
    $ifc  = new ifc($ifc_response,
                    $ifc_page,
                    $menu,
                    ["object"  => $object,
                     "range"   => $range,
                     "type"    => "href",
                     "value"   => $value,  //multi-language url
                     "_value"  => $_value, //link code
                     "left"    => 0,
                     "top"     => 0,
                     "id"      => $id],
                    NULL,
                    CMS_L_COMMAND_EDIT);

?>
<script>

function content_edit_href_select(value,
                                  text = "")
{
    var language = "<?php echo(q($language));?>";
    value        = value.replace(/^\s+/, ""); //remove preceding space
    value        = value.replace(/\s+$/, ""); //remove trailing space

    if ((value !== "") && (value !== "<?php echo(q(CMS_PROTOCOL));?>://"))
    {
        ifc_set("value", ifc_language_set(ifc_get("value"), value, language));

        if (value.startsWith("directory://"))
            text = text.replace(/\(\d+\)$/, "");
        else if (value.startsWith("content://"))
            text = text.replace(/^\[\d+\]/, "");

        //clean up text
        text = text.replace(/\[/g, "\\\\["); //quote block start
        text = text.replace(/^\s+/, "");     //preceding space
        text = text.replace(/\s+$/, "");     //trailing space
        text = text.replace(/\s+/g, " ");    //multiple space

        ifc_set("_value", value + " " + text);
        ifc_set("__value", value);
        return;
    };

    ifc_set("value", ifc_language_set(ifc_get("value"), "", language));
    ifc_del("_value");
};

function content_edit_href_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script>
<?php

    $value = language_get($value, $language, TRUE);
    if ($_value = template_parse_reference($value, TRUE))
    {
        $value = $_value["url"];
        $type  = $_value["scheme"];
        $index = $_value["host"];
        $flag  = stre($_value["query"] . $_value["fragment"]);
    }
    else
    {
        $type  = $index = "";
        $flag  = FALSE;
    };

    //directory
    $array = directory_get_select();
    array_shift($array);
    $_flag = $flag && streq($type, "directory");

    $ifc->set(CMS_L_IFC_CONTENT_011, "description b");
    echo("<select class=\"fullwidth\" " .
                 "onchange=\"content_edit_href_select(" .
                 "'directory://'+ifc_get(this)," .
                 "this.selectedOptions[0].text);\">" .
         "<option></option>");

    foreach ($array AS $key => $_value)
    {
        $name = strabridge($key, 60);
        echo("<option value=\"" . x($_value) . "\"" .
                      (($_flag && streq($_value, $index)) ? " selected>" : ">") .
             x($name) .
             "</option>");
    };

    echo("</select>");

    //publication
    if ($result = mysql_query("SELECT " .   CMS_DB_CONTENT_INDEX . ", " .
                                            CMS_DB_CONTENT_TITLE . " " .
                              "FROM " .     CMS_DB_CONTENT . " " .
                              "WHERE " .    CMS_DB_CONTENT_STATUS . "='" . CMS_CONTENT_STATUS_PUBLICATION . "' " .
                              "ORDER BY " . CMS_DB_CONTENT_INDEX . " DESC"))
    {
        $_flag = $flag && streq($type, "content");
        $ifc->set(CMS_L_IFC_CONTENT_042, "description b");
        echo("<select class=\"fullwidth\" " .
                     "onchange=\"content_edit_href_select(" .
                     "'content://'+ifc_get(this)," .
                     "this.selectedOptions[0].text);\">" .
             "<option></option>");

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $_value = $resultrow[CMS_DB_CONTENT_INDEX];
            $name   = strabridge("($_value) " . l($resultrow[CMS_DB_CONTENT_TITLE]), 60);
            echo("<option value=\"" . x($_value) . "\"" .
                          (($_flag && streq($_value, $index)) ? " selected>" : ">") .
                 x($name) .
                 "</option>");
        };

        echo("</select>");
    };

    //address
    $ifc->set(CMS_L_URL, "description b");
    echo("<input name=\"__value\" " .
                "type=\"text\" " .
                "value=\"" . (stre($value) ? x(CMS_PROTOCOL) . "://" : x($value)) . "\" " .
                "class=\"fullwidth\" " .
                "onchange=\"content_edit_href_select(this.value);\">");

    //language
    if (CMS_LANGUAGE_ENABLED)
    {
        $map       = new map("#system/language.image");
        $_language = explode(",", CMS_LANGUAGE_ENABLED);

        echo("<div class=\"language\">" .
             "<a href=\"javascript:ifc_post('edit_href_select_language','');\" " .
                "title=\"" . CMS_L_LANGUAGE_DEFAULT . "\" " .
                "class=\"language-" . (streq($language, "") ? "on" : "off") . "\">" .
             image("language/default", 16, 12) .
             "</a>");

        foreach ($_language AS $value)
            echo("<a href=\"javascript:ifc_post('edit_href_select_language','" . qrx($value) . "');\" " .
                    "title=\"" . x($value) . "\" " .
                    "class=\"language-" . (streq($language, $value) ? "on" : "off") . "\">" .
                 image(((($image = $map->get_value($value)) !== NULL) &&
                       is_file(CMS_DATA_PATH . "language/$image")) ?
                       CMS_DATA_URL . "language/$image" :
                       "language/default",
                       16, 12) .
                 "</a>");

        echo("</div>");
    };

    $ifc->close();

case "create":
//..................................................................................................

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_create",
                   CMS_L_IFC_CONTENT_049);

?>
<script>

function content_template_preview()
{
    var value = ifc_get("ifc_param2");
    if (value) load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "template_preview",
                                    "object"      => "\x1B%replace%"]))));?>");
};

</script>
<?php

    $ifc->set(CMS_L_IFC_CONTENT_027,     "text 50 255 bl", CMS_L_IFC_CONTENT_021);
    $ifc->set(CMS_L_IFC_CONTENT_013,     "label");
    $ifc->set(content_template_select(), "select 40",      cms_cache("template." . CMS_USER . ".page"));
    $ifc->set(CMS_L_COMMAND_SHOW,        "button b",       "javascript:content_template_preview();");
    $ifc->set(CMS_L_IFC_CONTENT_017,     "textarea 50x8 65535");

    $ifc->close();

case "_create":
//..................................................................................................

    if ($_object = $content->create($ifc_param1,
                                    $ifc_param2,
                                    $ifc_param3))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        cms_cache("template." . CMS_USER . ".page",  $ifc_param2, TRUE);
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "display":
//..................................................................................................

    echo(content_parse($content, $object));
    exit();

case "apply":
//..................................................................................................

    $object = $ifc_param;

    //time
    if (
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_SCHEDULE_TIME . " " .
                                "FROM " .   CMS_DB_CONTENT_SCHEDULE . " " .
                                "WHERE " .  CMS_DB_CONTENT_SCHEDULE_TYPE .
                                "='" .      sqlesc(CMS_CONTENT_SCHEDULE_TYPE_APPLY) . "' " .
                                "AND " .    CMS_DB_CONTENT_SCHEDULE_CONTENT .
                                "='" .      sqlesc($object) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         ($time = $resultrow[CMS_DB_CONTENT_SCHEDULE_TIME])
       )
    {
        $flag = TRUE;
    }
    else
    {
        $time = time();
        $flag = FALSE;
    };

    $date   = getdate($time);
    $hour   = $date["hours"];
    $minute = $date["minutes"];
    $day    = $date["mday"];
    $month  = $date["mon"];
    $year   = $date["year"];

    //time selection array
    for ($i = 1; $i <= 31; $i++) $array_day[sprintf("%02d", $i)] = $i;
    $array_month =
        [CMS_L_JANUARY   =>  1,
         CMS_L_FEBRUARY  =>  2,
         CMS_L_MARCH     =>  3,
         CMS_L_APRIL     =>  4,
         CMS_L_MAY       =>  5,
         CMS_L_JUNE      =>  6,
         CMS_L_JULY      =>  7,
         CMS_L_AUGUST    =>  8,
         CMS_L_SEPTEMBER =>  9,
         CMS_L_OCTOBER   => 10,
         CMS_L_NOVEMBER  => 11,
         CMS_L_DECEMBER  => 12];
    $i = $year;
    $c = $i + 10;
    if ($i < 1970) $i = 1970;
    if ($c > 2037) $c = 2037;
    for (; $i <= $c; $i++)      $array_year[sprintf("%04d", $i)]   = $i;
    for ($i = 0; $i < 24; $i++) $array_hour[sprintf("%02d", $i)]   = $i;
    for ($i = 0; $i < 60; $i++) $array_minute[sprintf("%02d", $i)] = $i;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_apply",
                   CMS_L_IFC_CONTENT_087);

    //immediate application
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b", 0, ! $flag);

    //scheduled application
    echo("<label class=\"element\">" .
         "<table class=\"layout\">" .
         "<tr>" .
         "<td>");
    $ifc->set(NULL,                  "radio",   1, $flag);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_111, "description b");
    $ifc->set($array_day,            "select",  $day);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_112, "description b");
    $ifc->set($array_month,          "select",  $month);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_113, "description b");
    $ifc->set($array_year,           "select",  $year);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_114, "description b");
    $ifc->set($array_hour,           "select",  $hour);
    echo("<span>:</span>");
    $ifc->set($array_minute,         "select",  $minute);
    echo("</td>" .
         "</tr>" .
         "</table>" .
         "</label><br>");

    //remove schedule
    if ($flag) $ifc->set(CMS_L_IFC_CONTENT_115, "radio", 2);

    $ifc->close();

case "_apply":
//..................................................................................................

    switch ($ifc_param1)
    {
    case 0: //immediate application

        $ifc_response = $content->apply($object) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 1: //scheduled application

        $time         = mktime($ifc_param5, $ifc_param6, 0, $ifc_param3, $ifc_param2, $ifc_param4);
        $ifc_response = $content->apply($object, $time) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 2: //remove schedule

        $ifc_response = $content->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_APPLY, $object) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
    };

    break;

case "apply_all":
//..................................................................................................

    if ($result = mysql_query("SELECT " . CMS_DB_CONTENT_INDEX . " " .
                              "FROM " .   CMS_DB_CONTENT . " " .
                              "WHERE " .  CMS_DB_CONTENT_WRITER_TIME . "!=" . CMS_DB_CONTENT_TIME))
    {
        while ($resultrow = mysql_fetch_assoc($result))
            $content->apply($resultrow[CMS_DB_CONTENT_INDEX]);

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "revert":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->revert($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "authorize":
//..................................................................................................

    $object    = $ifc_param;
    $result    = mysql_query("SELECT " . CMS_DB_CONTENT_EDITOR_COMMENT . " " .
                             "FROM " .   CMS_DB_CONTENT . " " .
                             "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                             "LIMIT 1");
    $resultrow = mysql_fetch_assoc($result);

    //display
    $ifc       = new ifc($ifc_response,
                         $ifc_page,
                         TRUE,
                         ["user"              => $user,
                          "object"            => $object,
                          "sql_select"        => $sql_select,
                          "sql_filter_field"  => $sql_filter_field,
                          "sql_filter_option" => $sql_filter_option,
                          "sql_filter_value"  => $sql_filter_value,
                          "sql_order"         => $sql_order,
                          "page"              => $page,
                          "limit"             => $limit],
                         "_authorize",
                         CMS_L_IFC_CONTENT_029);

    $ifc->set(CMS_L_IFC_CONTENT_017,
              "textarea 50x8 65535",
              $resultrow[CMS_DB_CONTENT_EDITOR_COMMENT]);

    $ifc->close();

case "_authorize":
//..................................................................................................

    $ifc_response = $content->authorize($object, $ifc_param1) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "derive_draft":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->derive_draft($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "publish":
//..................................................................................................

    $object = $ifc_param;

    if (
         ! (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_PUBLISHER_COMMENT . ", " .
                                                CMS_DB_CONTENT_TITLE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
       )
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //time
    $date   = getdate();
    $hour   = $date["hours"];
    $minute = $date["minutes"];
    $day    = $date["mday"];
    $month  = $date["mon"];
    $year   = $date["year"];

    //preset value
    $ifc_param   = $directory_object;
    $ifc_param1  = $resultrow[CMS_DB_CONTENT_TITLE];
    $ifc_param2  = 0; //immediate publication
    $ifc_param3  = $day;
    $ifc_param4  = $month;
    $ifc_param5  = $year;
    $ifc_param6  = $hour;
    $ifc_param7  = $minute;
    $ifc_param8  = 0; //no scheduled withdrawal
    $ifc_param9  = $day;
    $ifc_param10 = $month;
    $ifc_param11 = $year;
    $ifc_param12 = $hour;
    $ifc_param13 = $minute;
    $ifc_param14 = $resultrow[CMS_DB_CONTENT_PUBLISHER_COMMENT];

case "_publish":
//..................................................................................................

    $data     = new data("#system/directory");
    $flexview = new flexview();

    $flexview->import_data($data);

    //menu
    $menu = [CMS_L_COMMAND_CANCEL . "|content/command_cancel" => FALSE];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "__publish",
                   CMS_L_IFC_CONTENT_040);

    //time selection array
    for ($i = 1; $i <= 31; $i++) $array_day[sprintf("%02d", $i)] = $i;
    $array_month =
        [CMS_L_JANUARY   =>  1,
         CMS_L_FEBRUARY  =>  2,
         CMS_L_MARCH     =>  3,
         CMS_L_APRIL     =>  4,
         CMS_L_MAY       =>  5,
         CMS_L_JUNE      =>  6,
         CMS_L_JULY      =>  7,
         CMS_L_AUGUST    =>  8,
         CMS_L_SEPTEMBER =>  9,
         CMS_L_OCTOBER   => 10,
         CMS_L_NOVEMBER  => 11,
         CMS_L_DECEMBER  => 12];
    $date = getdate();
    $i    = $date["year"];
    $c    = $i + 10;
    if ($i < 1970) $i = 1970;
    if ($c > 2037) $c = 2037;
    for (; $i <= $c; $i++)      $array_year[sprintf("%04d", $i)]   = $i;
    for ($i = 0; $i < 24; $i++) $array_hour[sprintf("%02d", $i)]   = $i;
    for ($i = 0; $i < 60; $i++) $array_minute[sprintf("%02d", $i)] = $i;

    ifc_table_open();
    echo("<colspan>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colspan>" .

         "<tr>" .
         "<th>" .
         CMS_L_IFC_CONTENT_116 .
         "</th>" .
         "<th>" .
         CMS_L_IFC_CONTENT_041 .
         "</th>" .
         "</tr>" .

    //title
         "<tr>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_027, "text 50 255 bl",      $ifc_param1);

    //publication
    $ifc->set(CMS_L_IFC_CONTENT_040, "title");

    //immediate publication
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b",             0, $ifc_param2 == 0);

    //scheduled publication
    echo("<label class=\"element\">" .
         "<table class=\"layout\">" .
         "<tr>" .
         "<td>");
    $ifc->set(NULL,                  "radio",               1, $ifc_param2 == 1);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_111, "description b");
    $ifc->set($array_day,            "select",              $ifc_param3);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_112, "description b");
    $ifc->set($array_month,          "select",              $ifc_param4);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_113, "description b");
    $ifc->set($array_year,           "select",              $ifc_param5);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_114, "description b");
    $ifc->set($array_hour,           "select",              $ifc_param6);
    echo("<span>:</span>");
    $ifc->set($array_minute,         "select",              $ifc_param7);
    echo("</td>" .
         "</tr>" .
         "</table>" .
         "</label>");

    //withdrawal
    $ifc->set(CMS_L_IFC_CONTENT_054, "title");

    //no scheduled withdrawal
    $ifc->set(CMS_L_IFC_CONTENT_117, "new radio b",         0, $ifc_param8 == 0);

    //scheduled withdrawal
    echo("<label class=\"element\">" .
         "<table class=\"layout\">" .
         "<tr>" .
         "<td>");
    $ifc->set(NULL,                  "radio",               1, $ifc_param8 == 1);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_111, "description b");
    $ifc->set($array_day,            "select",              $ifc_param9);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_112, "description b");
    $ifc->set($array_month,          "select",              $ifc_param10);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_113, "description b");
    $ifc->set($array_year,           "select",              $ifc_param11);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_114, "description b");
    $ifc->set($array_hour,           "select",              $ifc_param12);
    echo("<span>:</span>");
    $ifc->set($array_minute,         "select",              $ifc_param13);
    echo("</td>" .
         "</tr>" .
         "</table>" .
         "</label>");

    //comment
    $ifc->set(CMS_L_IFC_CONTENT_118, "title");
    $ifc->set(CMS_L_IFC_CONTENT_017, "textarea 50x8 65535", $ifc_param14);
    echo("</td>" .

    //directory
         "<td class=\"target\">");
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
    $flexview->show_target($ifc_param,
                           "javascript:ifc_post('_publish','%index%');",
                           "javascript:ifc_post('publish_insert','%index%');",
                           "javascript:ifc_post('publish_append','%index%');");
    echo("</td>" .
         "</tr>");

    //no entry
    echo("<tr>" .
         "<td class=\"varied\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_IFC_CONTENT_128, "button", "___publish");
    echo("</td>" .

    //replace
         "<td class=\"varied\" " .
             "style=\"TEXT-ALIGN:right\">");
    if ($ifc_param)
        $ifc->set(CMS_L_IFC_CONTENT_119,
        "button",
        "javascript:ifc_post('publish_replace','" . q($ifc_param) . "');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "__publish":
case "___publish":
case "publish_replace":
case "publish_insert":
case "publish_append":
//..................................................................................................

    //action
    switch (CMS_IFC_MESSAGE)
    {
    case "publish_replace":

        $action = "replace";
        break;

    case "publish_insert":

        $action = "insert";
        break;

    case "publish_append":

        $action = "append";
        break;

    default:

        $action = NULL;
        break;
    };

    //publication
    switch ($ifc_param2)
    {
    case 0: //immediate publication

        $time_publish = 0;
        break;

    case 1: //scheduled publication

        $time_publish = mktime($ifc_param6, $ifc_param7, 0, $ifc_param4, $ifc_param3, $ifc_param5);
        break;
    };

    //withdrawal
    switch ($ifc_param8)
    {
    case 0: //no scheduled withdrawal

        $time_withdraw = 0;
        break;

    case 1: //scheduled withdrawal

        $time_withdraw = mktime($ifc_param12, $ifc_param13, 0, $ifc_param10, $ifc_param9, $ifc_param11);
        break;
    };

    //publish
    if (streq(CMS_IFC_MESSAGE, "___publish"))
    {
        if ($content->publish($object,        //index
                              $ifc_param14,   //comment
                              $time_publish,  //time_publish
                              $time_withdraw) //time_withdraw
            !== FALSE)
        {
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    }

    //publish and link
    elseif ($content->publish($object,        //index
                              $ifc_param14,   //comment
                              $time_publish,  //time_publish
                              $time_withdraw, //time_withdraw
                              $ifc_param,     //directory_index
                              $action,        //directory_action
                              $ifc_param1)    //directory_title
            !== FALSE)
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "withdraw":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->withdraw($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "duplicate":
//..................................................................................................

    $object = $ifc_param;

    if ($_object = $content->duplicate($object))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy":
//..................................................................................................

    $object = $ifc_param;

    if ($_object = $content->copy($object))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "send":
//..................................................................................................

    $object = $ifc_param;

    if (
         ! (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . ", " .
                                                CMS_DB_CONTENT_WRITER . ", " .
                                                CMS_DB_CONTENT_SENDER_COMMENT . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
       )
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //get receiver list
    $data      = new data("#system/permission");
    $type      = $resultrow[CMS_DB_CONTENT_TYPE];
    $status    = $resultrow[CMS_DB_CONTENT_STATUS];
    $author    = $resultrow[CMS_DB_CONTENT_WRITER];
    $receiver  = content_get_receiver($content, $type, $status);
    $duplicate = $content->test_duplicate($object);
    $copy      = $content->test_copy($object);
    $array     = NULL;

    //go through user list
    $data->move("first");
    while ($key = $data->move("next"))
    {
        if (! permission_is_user($key)) continue; //not a user
        $key = substr($key, 5);
        if (streq($key, $user))         continue; //user equals sender

        //check permission
        $original   = isset($receiver[CMS_CONTENT_TYPE_ORIGINAL][$key]);
        $_duplicate = $duplicate && isset($receiver[CMS_CONTENT_TYPE_DUPLICATE][$key]);
        $_copy      = $copy      && isset($receiver[CMS_CONTENT_TYPE_COPY][$key]);
        if (! ($original || $_duplicate || $_copy)) continue; //no permission

        //add receiver
        $array[$key] =
            [0 => $data->get("user.$key", "name"),
             1 => $original,
             2 => $_duplicate,
             3 => $_copy];
    };

    //no receiver
    if (! is_array($array))
    {
        $ifc_response = CMS_L_IFC_CONTENT_045;
        break;
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_send",
                   CMS_L_IFC_CONTENT_046);

    ifc_table_open();
    echo("<tr>" .

    //receiver
         "<th>" .
         CMS_L_IFC_CONTENT_048 .
         "</th>" .

    //role
         "<th>" .
         CMS_L_IFC_CONTENT_044 .
         "</th>" .

    //original
         "<th>" .
         CMS_L_IFC_CONTENT_034 .
         "</th>" .

    //duplicate
         "<th>" .
         CMS_L_IFC_CONTENT_035 .
         "</th>" .

    //copy
         "<th>" .
         CMS_L_IFC_CONTENT_036 .
         "</th>" .
         "</tr>" .

    //blank
         "<tr>" .
         "<td colspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .
         "<td class=\"select\">");
    $ifc->set(NULL, "radio", NULL, TRUE, "original");
    echo("</td>" .
         "<td colspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .
         "</tr>");

    $i = 0;

    ksort($array);

    //go through receiver list
    foreach ($array AS $key => $value)
    {
        //role
        $role = "";
        if (streq($key, $author))                                               $role .= "<strong>" .
                                                                                         CMS_L_IFC_CONTENT_047 .
                                                                                         "</strong>, ";
        if (cms_permission(CMS_CONTENT_PERMISSION_WRITER,    TRUE, TRUE, $key)) $role .= CMS_L_IFC_CONTENT_009 . ", ";
        if (cms_permission(CMS_CONTENT_PERMISSION_EDITOR,    TRUE, TRUE, $key)) $role .= CMS_L_IFC_CONTENT_015 . ", ";
        if (cms_permission(CMS_CONTENT_PERMISSION_PUBLISHER, TRUE, TRUE, $key)) $role .= CMS_L_IFC_CONTENT_019 . ", ";
        $role = substr($role, 0, -2);

        //permitted action
        $original  = $value[1] ? NULL : " d";
        $duplicate = $value[2] ? NULL : " d";
        $copy      = $value[3] ? NULL : " d";

        $varied = ifc_varied();

        echo("<tr>" .

        //receiver
             "<td$varied>" .
             x($value[0]) .
             "</td>" .

        //role
             "<td$varied>" .
             $role .
             "</td>" .

        //original
             "<td$varied>");
        $ifc->set(NULL, "radio$original",     $key, NULL, "original");
        echo("</td>" .

        //duplicate
             "<td$varied>");
        $ifc->set(NULL, "checkbox$duplicate", $key, NULL, "duplicate[$i]");
        echo("</td>" .

        //copy
             "<td$varied>");
        $ifc->set(NULL, "checkbox$copy",      $key, NULL, "copy[$i]");
        echo("</td>" .
             "</tr>");

        $i++;
    };
    ifc_table_close();

    //comment
    $ifc->set(CMS_L_IFC_CONTENT_017, "textarea 50x8 65535", $resultrow[CMS_DB_CONTENT_SENDER_COMMENT]);

    $ifc->close();

case "_send":
//..................................................................................................

    $error = NULL;

    //send copy
    if (isset($copy) && is_array($copy))
    {
        foreach ($copy AS $value)
        {
            if (! $_object = $content->copy($object))
            {
                $error |= TRUE;
                continue;
            };

            if (! $content->send($_object, $value, $ifc_param1))
            {
                $content->delete($_object);
                $error |= TRUE;
            };
        };
    };

    //send duplicate
    if (isset($duplicate) && is_array($duplicate))
    {
        foreach ($duplicate AS $value)
        {
            if (! $_object = $content->duplicate($object))
            {
                $error |= TRUE;
                continue;
            };

            if (! $content->send($_object, $value, $ifc_param1))
            {
                $content->delete($_object);
                $error |= TRUE;
            };
        };
    };

    //send original
    if ($original)
    {
        if ($content->send($object, $original, $ifc_param1))
            $object  = NULL;
        else
            $error  |= TRUE;
    };

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "delete":
//..................................................................................................

    if (empty($list) || (! is_array($list))) break; //no selection

    $error = NULL;

    foreach ($list AS $value)
    {
        if (! $content->delete($value)) $error |= TRUE;
    };

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "template_preview":
//..................................................................................................

    template_preview($object);
    exit();

case "clear_cache":
//..................................................................................................

    if (

    //verify permission
         $content->publisher
         &&

    //delete cache
         cms_cache_clean(CMS_DATA_PATH . "#content/cache/", TRUE)
       )
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "version":
//..................................................................................................

    $object = $ifc_param;

    //no versions
    if (
         ! (
             ($result = mysql_query("SELECT " .   CMS_DB_CONTENT_VERSION_INDEX . ", " .
                                                  CMS_DB_CONTENT_VERSION_TIME . " " .
                                    "FROM " .     CMS_DB_CONTENT_VERSION . " " .
                                    "WHERE " .    CMS_DB_CONTENT_VERSION_CONTENT .
                                    "='" .        sqlesc($object) . "' " .
                                    "ORDER BY " . CMS_DB_CONTENT_VERSION_TIME . " DESC"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
       )
        break;

    $version = $resultrow[CMS_DB_CONTENT_VERSION_INDEX];

    //menu
    $menu = NULL;
    if ($content->test_update($object))
        $menu[CMS_L_IFC_CONTENT_086 . "|content/command_restore"] = "version_retrieve";
    $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"] = NULL;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   NULL,
                   CMS_L_IFC_CONTENT_085);

?>
<script>

function content_version_select(value)
{
    document.getElementById("content-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "version_display",
                                    "version"     => "\x1B%replace%"]))));?>";
};

</script>
<?php

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    //list
    $ifc->set(CMS_L_IFC_CONTENT_089, "description b");
    echo("<div id=\"version\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"content_version_select(this.value);\">");
    $i    = mysql_num_rows($result);
    $flag = TRUE;
    do
    {
        $name = sprintf(CMS_L_IFC_CONTENT_135, $i--) . " – " .
            friendly_date($resultrow[CMS_DB_CONTENT_VERSION_TIME]);
        echo("<label>" .
             "<input name=\"version\" " .
                    "type=\"radio\" " .
                    "value=\"" . x($resultrow[CMS_DB_CONTENT_VERSION_INDEX]) . "\"" .
                    ($flag ? " checked>" : ">") .
             x($name) .
             "</label>");
        $flag = FALSE;
    }
    while ($resultrow = mysql_fetch_assoc($result));
    echo("</div>" .
         jscript("ifc_custom_select(\"version\");") .
         "</td>" .

    //display version
         "<td class=\"iframe\">" .
         "<iframe id=\"content-display\" " .
                 "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                       "ifc_message" => "version_display",
                                       "version"     => $version])) . "\" " .
                 "sandbox=\"allow-same-origin\">" .
         CMS_L_IFC_CONTENT_090 .
         "</iframe>" .
         "</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "version_display":
//..................................................................................................

    if (
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                            CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                            CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                            CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                            CMS_DB_CONTENT_VERSION_TEMPLATE . " " .
                                "FROM " .   CMS_DB_CONTENT_VERSION . " " .
                                "WHERE " .  CMS_DB_CONTENT_VERSION_INDEX .
                                "='" .      sqlesc($version) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        $document = new document($resultrow[CMS_DB_CONTENT_VERSION_TEXT]);
        $template = new template();

        echo($template->parse(
            $document,
            $resultrow[CMS_DB_CONTENT_VERSION_TEMPLATE],
            $resultrow[CMS_DB_CONTENT_VERSION_TITLE],
            $resultrow[CMS_DB_CONTENT_VERSION_DESCRIPTION],
            $resultrow[CMS_DB_CONTENT_VERSION_KEYWORD],
            ["edit" => ["lock" => TRUE]]));
        exit();
    };

    header("Location: " . CMS_URL . "blank.htm", TRUE, 302);
    exit();

case "version_store":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->version_store($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "version_retrieve":
//..................................................................................................

    //time
    if (
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_SCHEDULE_TIME . " " .
                                "FROM " .   CMS_DB_CONTENT_SCHEDULE . " " .
                                "WHERE " .  CMS_DB_CONTENT_SCHEDULE_TYPE .
                                "='" .      sqlesc(CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE) . "' " .
                                "AND " .    CMS_DB_CONTENT_SCHEDULE_CONTENT .
                                "='" .      sqlesc($object) . "' " .
                                "AND " .    CMS_DB_CONTENT_SCHEDULE_VALUE_1 .
                                "='" .      sqlesc($version) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         ($time = $resultrow[CMS_DB_CONTENT_SCHEDULE_TIME])
       )
    {
        $flag = TRUE;
    }
    else
    {
        $time = time();
        $flag = FALSE;
    };

    $date   = getdate($time);
    $hour   = $date["hours"];
    $minute = $date["minutes"];
    $day    = $date["mday"];
    $month  = $date["mon"];
    $year   = $date["year"];

    //time selection array
    for ($i = 1; $i <= 31; $i++) $array_day[sprintf("%02d", $i)] = $i;
    $array_month =
        [CMS_L_JANUARY   =>  1,
         CMS_L_FEBRUARY  =>  2,
         CMS_L_MARCH     =>  3,
         CMS_L_APRIL     =>  4,
         CMS_L_MAY       =>  5,
         CMS_L_JUNE      =>  6,
         CMS_L_JULY      =>  7,
         CMS_L_AUGUST    =>  8,
         CMS_L_SEPTEMBER =>  9,
         CMS_L_OCTOBER   => 10,
         CMS_L_NOVEMBER  => 11,
         CMS_L_DECEMBER  => 12];
    $i = $year;
    $c = $i + 10;
    if ($i < 1970) $i = 1970;
    if ($c > 2037) $c = 2037;
    for (; $i <= $c; $i++)      $array_year[sprintf("%04d", $i)]   = $i;
    for ($i = 0; $i < 24; $i++) $array_hour[sprintf("%02d", $i)]   = $i;
    for ($i = 0; $i < 60; $i++) $array_minute[sprintf("%02d", $i)] = $i;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["user"              => $user,
                    "object"            => $object,
                    "version"           => $version,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_version_retrieve",
                   CMS_L_IFC_CONTENT_086);

    //immediate application
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b", 0, ! $flag);

    //scheduled application
    echo("<label class=\"element\">" .
         "<table class=\"layout\">" .
         "<tr>" .
         "<td>");
    $ifc->set(NULL,                  "radio",   1, $flag);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_111, "description b");
    $ifc->set($array_day,            "select",  $day);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_112, "description b");
    $ifc->set($array_month,          "select",  $month);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_113, "description b");
    $ifc->set($array_year,           "select",  $year);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_114, "description b");
    $ifc->set($array_hour,           "select",  $hour);
    echo("<span>:</span>");
    $ifc->set($array_minute,         "select",  $minute);
    echo("</td>" .
         "</tr>" .
         "</table>" .
         "</label><br>");

    //remove schedule
    if ($flag) $ifc->set(CMS_L_IFC_CONTENT_115, "radio", 2);

    $ifc->close();

case "_version_retrieve":
//..................................................................................................

    switch ($ifc_param1)
    {
    case 0: //immediate application

        $ifc_response = $content->version_retrieve($version) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 1: //scheduled application

        $time         = mktime($ifc_param5, $ifc_param6, 0, $ifc_param3, $ifc_param2, $ifc_param4);
        $ifc_response = $content->version_retrieve($version, $time) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 2: //remove schedule

        $ifc_response = $content->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE, $object, $version) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
    };

    break;

case "schedule":
case "schedule_save":
case "schedule_delete":
//..................................................................................................

    switch (CMS_IFC_MESSAGE)
    {
    case "schedule":
//..................................................................................................

        $object = $ifc_param;
        break;

    case "schedule_save":
//..................................................................................................

        if ((! isset($time)) || (! is_array($time))) break; //no selection

        $error = NULL;

        foreach ($time AS $key => $value)
        {
            $value = mktime($value[3], $value[4], 0, $value[1], $value[0], $value[2]);
            if (! mysql_query("UPDATE " . CMS_DB_CONTENT_SCHEDULE . " " .
                              "SET " .    CMS_DB_CONTENT_SCHEDULE_TIME . "='" . sqlesc($value) . "' " .
                              "WHERE " .  CMS_DB_CONTENT_SCHEDULE_HASH . "='" . sqlesc($key) . "' " .
                              "LIMIT 1"))
                $error |= TRUE;
        };

        $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
        break;

    case "schedule_delete":
//..................................................................................................

        if (empty($list) || (! is_array($list))) break; //no selection

        $error = NULL;

        foreach ($list AS $value)
        {
            if (! $content->_schedule_delete($value)) $error |= TRUE;
        };

        $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
        break;
    };

    //no versions
    if (
         ! (
             ($result = mysql_query("SELECT " .   CMS_DB_CONTENT_SCHEDULE_TIME . ", " .
                                                  CMS_DB_CONTENT_SCHEDULE_TYPE . ", " .
                                                  CMS_DB_CONTENT_SCHEDULE_CONTENT . ", " .
                                                  CMS_DB_CONTENT_SCHEDULE_VALUE_1 . ", " .
                                                  CMS_DB_CONTENT_SCHEDULE_VALUE_2 . ", " .
                                                  CMS_DB_CONTENT_SCHEDULE_HASH . " " .
                                    "FROM " .     CMS_DB_CONTENT_SCHEDULE . " " .
                                    "WHERE " .    CMS_DB_CONTENT_SCHEDULE_CONTENT .
                                    "='" .        sqlesc($object) . "' " .
                                    "ORDER BY " . CMS_DB_CONTENT_SCHEDULE_TIME))
             &&
             mysql_num_rows($result)
           )
       )
        break;

    //time selection array
    for ($i = 1; $i <= 31; $i++) $array_day[sprintf("%02d", $i)] = $i;
    $array_month =
        [CMS_L_JANUARY   =>  1,
         CMS_L_FEBRUARY  =>  2,
         CMS_L_MARCH     =>  3,
         CMS_L_APRIL     =>  4,
         CMS_L_MAY       =>  5,
         CMS_L_JUNE      =>  6,
         CMS_L_JULY      =>  7,
         CMS_L_AUGUST    =>  8,
         CMS_L_SEPTEMBER =>  9,
         CMS_L_OCTOBER   => 10,
         CMS_L_NOVEMBER  => 11,
         CMS_L_DECEMBER  => 12];
    $date = getdate();
    $i    = $date["year"];
    $c    = $i + 10;
    if ($i < 1970) $i = 1970;
    if ($c > 2037) $c = 2037;
    for (; $i <= $c; $i++)      $array_year[sprintf("%04d", $i)]   = $i;
    for ($i = 0; $i < 24; $i++) $array_hour[sprintf("%02d", $i)]   = $i;
    for ($i = 0; $i < 60; $i++) $array_minute[sprintf("%02d", $i)] = $i;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_SAVE .            "|content/command_save"   => "schedule_save",
                    CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete" => "#schedule_delete",
                    CMS_L_COMMAND_CLOSE .           "|content/command_cancel" => NULL],
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   NULL,
                   CMS_L_IFC_CONTENT_120);

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"4\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //spacer
         "<td class=\"select\">" .
         "</td>" .

    //time
         "<th>" .
         CMS_L_IFC_CONTENT_001 .
         "</th>" .

    //type
         "<th>" .
         CMS_L_IFC_CONTENT_005 .
         "</th>" .

    //value
         "<th>" .
         CMS_L_IFC_CONTENT_023 .
         "</th>" .
         "</tr>");

    $directory = new directory();

    //go through schedule
    while ($resultrow = mysql_fetch_assoc($result))
    {
        //time
        $date   = getdate($resultrow[CMS_DB_CONTENT_SCHEDULE_TIME]);
        $hour   = $date["hours"];
        $minute = $date["minutes"];
        $day    = $date["mday"];
        $month  = $date["mon"];
        $year   = $date["year"];

        $type   = $resultrow[CMS_DB_CONTENT_SCHEDULE_TYPE];
        $hash   = $resultrow[CMS_DB_CONTENT_SCHEDULE_HASH];
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL,                  "checkbox", $hash,   FALSE, "list[]");
        echo("</td>" .

        //time
             "<td$varied>" .
             "<label class=\"element\">" .
             "<span>" . x(friendly_date($resultrow[CMS_DB_CONTENT_SCHEDULE_TIME])) . "</span>" .

             "<table class=\"layout\">" .
             "<tr>" .
             "<td>");
        $ifc->set(CMS_L_IFC_CONTENT_111, "description b");
        $ifc->set($array_day,            "select",   $day,    NULL,  "time[$hash][0]");
        echo("</td>" .
             "<td>");
        $ifc->set(CMS_L_IFC_CONTENT_112, "description b");
        $ifc->set($array_month,          "select",   $month,  NULL,  "time[$hash][1]");
        echo("</td>" .
             "<td>");
        $ifc->set(CMS_L_IFC_CONTENT_113, "description b");
        $ifc->set($array_year,           "select",   $year,   NULL,  "time[$hash][2]");
        echo("</td>" .
             "<td>");
        $ifc->set(CMS_L_IFC_CONTENT_114, "description b");
        $ifc->set($array_hour,           "select",   $hour,   NULL,  "time[$hash][3]");
        echo("<span>:</span>");
        $ifc->set($array_minute,         "select",   $minute, NULL,  "time[$hash][4]");
        echo("</td>" .
             "</tr>" .
             "</table>" .

             "</label>" .
             "</td>");

        //type
        switch ($type)
        {
        case CMS_CONTENT_SCHEDULE_TYPE_APPLY:

            echo("<td$varied>" .
                 CMS_L_IFC_CONTENT_087 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE:

            echo("<td$varied>" .
                 CMS_L_IFC_CONTENT_122 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_PUBLISH:

            echo("<td" . ifc_varied("green", 1) . ">" .
                 CMS_L_IFC_CONTENT_040 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW:

            echo("<td" . ifc_varied("red", 1) . ">" .
                 CMS_L_IFC_CONTENT_054 .
                 "</td>");
            break;
        };

        //value
        echo("<td$varied>");
        switch ($type)
        {
        case CMS_CONTENT_SCHEDULE_TYPE_APPLY:
//..................................................................................................

            break;

        case CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE:
//..................................................................................................

            $value = $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1];

            if (
                 ($_result = mysql_query("SELECT " . CMS_DB_CONTENT_VERSION_TIME . ", " .
                                                     CMS_DB_CONTENT_VERSION_TITLE . " " .
                                         "FROM " .   CMS_DB_CONTENT_VERSION . " " .
                                         "WHERE " .  CMS_DB_CONTENT_VERSION_INDEX .
                                         "='" .      sqlesc($value) . "' " .
                                         "LIMIT 1"))
                 &&
                 ($_resultrow = mysql_fetch_assoc($_result))
               )
                echo("<small>" .
                     CMS_L_IFC_CONTENT_125 .
                     "</small><br>" .
                     "<a href=\"javascript:load_page('" .
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "version_display",
                                    "version"     => $value])) . "');\">" .
                     x(friendly_date($_resultrow[CMS_DB_CONTENT_VERSION_TIME]) . " " .
                       l($_resultrow[CMS_DB_CONTENT_VERSION_TITLE])) .
                     "</a>");
            else
                echo(CMS_L_IFC_CONTENT_126);
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_PUBLISH:
        case CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW:
//..................................................................................................

            $key   = $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1];
            $value = l($resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_2]);

            if (l($directory->data->get($key, "name")))
            {
                while ($key = $directory->data->move("parent", $key))
                    $value = l($directory->data->get($key, "name")) . "/$value";

                echo("<small>" .
                     CMS_L_IFC_CONTENT_123 .
                     "</small><br>" .
                     x($value));
            }
            else
            {
                echo(CMS_L_IFC_CONTENT_124);
            };
            break;
        };
        echo("</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"4\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "pool":
case "pool_select":
case "pool_select_language":
case "pool_display":
case "pool_buffer":
case "pool_add":
case "_pool_add":
case "pool_edit":
case "_pool_edit":
case "pool_source":
case "pool_delete":
case "pool_category_rename":
case "_pool_category_rename":
case "pool_synchronize_all":
//..................................................................................................

    if (empty($pool_object))   $pool_object   = NULL;
    if (empty($pool_type))     $pool_type     = NULL;
    if (empty($pool_language)) $pool_language = NULL;

    $pool = new content_pool();

    switch (CMS_IFC_MESSAGE)
    {
    case "pool_select":
//..................................................................................................

        $pool_object = language_set($pool_object, $ifc_param, $pool_language);
        break;

    case "pool_select_language":
//..................................................................................................

        $pool_language = $ifc_param;
        break;

    case "pool_display":
//..................................................................................................

        if ($pool_object)
        {
            $document = new document($pool->get_text($pool_object));

            if ($type = $pool->data->get($pool_object, "type"))
            {
                //assign data to preview base
                $array = [];

                foreach ($document->data AS $key => $value)
                {
                    if ($key === "")                         $key = "preview";
                    elseif (streq(substr($key, 0, 2), "..")) $key = substr($key, 2);
                    else                                     $key = "preview.$key";

                    $array[$key] = $value;
                };

                $document->data = $array;

                //generate preview template
                $template = new template();
                $type     = isset($template->tlist[$type]) ?
                            $template->tlist[$type] : CMS_TEMPLATE_TYPE_TEXT;

                switch ($type)
                {
                case CMS_TEMPLATE_TYPE_HREF:

                    $code = "<CMS:href id=\"preview\">" .
                            $template->tname[$type] .
                            "</CMS:href>";
                    break;

                case CMS_TEMPLATE_TYPE_PLUGIN:

                    $code = "<CMS:plugin id=\"preview\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_TEXT:

                    $code = "<CMS:text id=\"preview\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_DOWNLOAD:

                    $code = "<CMS:download id=\"preview\">" .
                            $template->tname[$type] .
                            "</CMS:download>";
                    break;

                case CMS_TEMPLATE_TYPE_IMAGE:

                    $code = "<CMS:image id=\"preview\" " .
                                       "width=\"300\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_THUMBNAIL:

                    $code = "<CMS:thumbnail id=\"preview\" " .
                                           "twidth=\"300\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_MEDIA:

                    $code = "<CMS:media id=\"preview\" " .
                                       "width=\"300\" " .
                                       "height=\"225\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_TEMPLATE:

                    $code = "<CMS:template id=\"preview\"/>";
                    break;

                case CMS_TEMPLATE_TYPE_GROUP:

                    $code = "<CMS:group id=\"preview\">" .
                            $template->tname[$type] .
                            "</CMS:group>";
                    break;

                case CMS_TEMPLATE_TYPE_REPEAT:

                    $code = "<CMS:repeat id=\"preview\">" .
                            $template->tname[$type] . "<br>" .
                            "</CMS:repeat>";
                    break;

                case CMS_TEMPLATE_TYPE_SHIFT:

                    $code = "<CMS:shift id=\"preview\">" .
                            $template->tname[$type] .
                            "</CMS:shift>";
                    break;

                case CMS_TEMPLATE_TYPE_PERMISSION:

                    $code = "<CMS:permission id=\"preview\">" .
                            $template->tname[$type] .
                            "</CMS:permission>";
                    break;
                };

                template_preview($code, FALSE, $document);
            };
        };

        $category = $pool->data->get($pool_object, "category");
        cms_cache("content_pool." . CMS_USER . ".category", $category, TRUE);
        exit();

    case "pool_buffer":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $buffer       = $pool->get_text($_pool_object);

        //permanent caching
        cms_cache("content." . CMS_USER . ".buffer", $buffer, TRUE);

        $ifc_response = CMS_MSG_DONE;
        break;

    case "pool_add":
//..................................................................................................

        $array    = content_pool_get_select();
        $category = (string)cms_cache("content_pool." . CMS_USER . ".category");
        if ((! $category) || (! in_array($category, $array))) $category = "";

        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_add",
                        CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
                       ["content_index" => $content_index,
                        "content_range" => $content_range,
                        "pool_type"     => $pool_type],
                       "_pool_add",
                       CMS_L_COMMAND_ADD);

        //name
        $ifc->set(CMS_L_NAME,             "text 40 40 b", "[$content_index] $content_range");

        //category
        $ifc->set(CMS_L_IFC_TEMPLATE_005, "label");
        $ifc->set($array,                 "list 40 40",   $category);

        $ifc->close();

    case "_pool_add":
//..................................................................................................

        if ($_pool_object = $pool->add($ifc_param1,    //name
                                       $ifc_param2,    //category
                                       $content_index, //content_index
                                       $content_range, //range
                                       $pool_type))    //type
        {
            $pool_object  = language_set($pool_object, $_pool_object, $pool_language);
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "pool_edit":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);

        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_edit",
                        CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
                       ["pool_object"   => $pool_object,
                        "pool_language" => $pool_language],
                       "_pool_edit",
                       CMS_L_COMMAND_EDIT);

        //name
        $ifc->set(CMS_L_NAME,                "text 40 40 b", $pool->data->get($_pool_object, "name"));

        //category
        $ifc->set(CMS_L_IFC_TEMPLATE_005,    "label");
        $ifc->set(content_pool_get_select(), "list 40 40",   $pool->data->get($_pool_object, "category"));

        $ifc->close();

    case "_pool_edit":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);

        if ($pool->set($_pool_object,                            //index
                       $ifc_param1,                              //name
                       $ifc_param2,                              //category
                       $pool->data->get($_pool_object, "index"), //content_index
                       $pool->data->get($_pool_object, "range"), //range
                       $pool->data->get($_pool_object, "type"))) //type
        {
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "pool_source":
//..................................................................................................

        $index = $pool->data->get($_pool_object, "index");
        $range = $pool->data->get($_pool_object, "range");

        header("Location: " . cms_url(CMS_MODULES_URL . "content.php",
                                      ["content_index"  => $index,
                                       "content_option" => CMS_TEMPLATE_OPTION_ALL]) .
               "#tp-dd-" . r(crc32_base62($range)),
               TRUE, 302);
        exit();

    case "pool_delete":
//..................................................................................................

        if (empty($_pool_object)) break; //no selection

        $value      = reset($_pool_object);
        $category   = $pool->data->get($value, "category");
        $flag_error = TRUE;

        //delete selected references
        foreach ($_pool_object AS $value)
            $flag_error &= ! $pool->delete($value);

        //remove invalid references from selection
        $array = explode(",", CMS_LANGUAGE_ENABLED);
        if (CMS_LANGUAGE_ENABLED) $array[] = FALSE;

        foreach ($array AS $value)
        {
            $_pool_object = language_get($pool_object, $value, TRUE);

            if (! $pool->data->get($_pool_object))
            {
                //active language
                if (streq($value, $pool_language))
                {
                    $pool->data->move("first");
                    $_pool_object = $pool->data->seek(["category" => $category]);
                    $pool_object  = language_set($pool_object, $_pool_object, $value);
                }

                //inactive language(s)
                else
                {
                    $pool_object  = language_set($pool_object, NULL, $value);
                };
            };
        };

        $ifc_response = $flag_error ? CMS_MSG_ERROR : CMS_MSG_DONE;
        break;

    case "pool_category_rename":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $category     = $pool->data->get($_pool_object, "category");

        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_category_rename",
                        CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
                       ["pool_object"   => $pool_object,
                        "pool_language" => $pool_language],
                       "_pool_category_rename",
                       CMS_L_IFC_CONTENT_063);

        //category
        $ifc->set(CMS_L_NAME, "text 40 40 b", $category);

        $ifc->close();

    case "_pool_category_rename":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $category     = $pool->data->get($_pool_object, "category");

        $pool->data->move("first");
        while ($key = $pool->data->move("next"))
        {
            $_category = $pool->data->get($key, "category");
            if (streq($_category, $category))
                $pool->data->set($ifc_param1, $key, "category");
        };

        $ifc_response = $pool->data->save() ? CMS_MSG_DONE : CMS_MSG_DONE;
        break;

    case "pool_synchronize_all":
//..................................................................................................

        $ifc_response = $pool->synchronize_all() ? CMS_MSG_DONE : CMS_MSG_DONE;
        break;
    };

    $_pool_object = language_get($pool_object, $pool_language, TRUE);
    $array        = content_pool_get_array(CMS_IFC_SELECT ? $pool_type : NULL);

    //get category from object
    if ($_pool_object)
    {
        $category = $pool->data->get($_pool_object, "category");
        if (! $category) $category = "";
    }

    //get object from category
    else
    {
        $category = (string)cms_cache("content_pool." . CMS_USER . ".category");
        if (nstre($category) && (! isset($array[$category])))
            $category = "";

        if (is_array($array[$category]))
        {
            $_pool_object = current($array[$category]);
            $pool_object  = language_set($pool_object, $_pool_object, $pool_language);
        };
    };

    //menu
    $menu = NULL;
    if (nstre($_pool_object))
    {
        $pool_index = $pool->data->get($_pool_object, "index");
        $pool_range = $pool->data->get($_pool_object, "range");

        if (CMS_IFC_SELECT)
            $menu[CMS_L_COMMAND_INSERT . "|content/command_apply"]    = "javascript:ifc_return();";
        $menu[CMS_L_IFC_CONTENT_133 . "|content/command_restore"]     = "pool_buffer";
        if ($pool->operator)
            $menu[CMS_L_COMMAND_EDIT .    "|content/command_edit"]    = "pool_edit";
        $menu[CMS_L_IFC_CONTENT_025 . "|content/command_source"]      =
            "javascript:load_page('" .
            str_replace("%replace%",
                        "'+ifc_get('_pool_object')+'",
                        q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "pool_source",
                                   "object"      => "\x1B%replace%"]))) . "');";
        if ($pool->operator)
        {
            $menu[CMS_L_COMMAND_DELETE .  "|content/command_delete"]  = "#pool_delete";
            $menu[CMS_L_IFC_CONTENT_063 . "|content/command_rename"]  = "pool_category_rename";
        };
    };
    if ($pool->operator)
        $menu[CMS_L_IFC_CONTENT_132 . "|content/command_synchronize"] = "pool_synchronize_all";
    if (nstreq(CMS_IFC_OPTION, "external"))
        $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"]        = NULL;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["pool_object"   => $pool_object,
                    "pool_type"     => $pool_type,
                    "pool_language" => $pool_language]);

//javascript

?>
<script>

function content_pool_select(value)
{
    var language = "<?php echo(q($pool_language));?>";
    if (ifc_language_get(ifc_get("pool_object"), language) === value) return;
    ifc_set("pool_object", ifc_language_set(ifc_get("pool_object"), value, language));
    document.getElementById("content-pool-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "pool_display",
                                    "pool_object" => "\x1B%replace%"]))));?>";
};

</script>
<?php

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    //category
    $ifc->set(CMS_L_IFC_CONTENT_129, "description b");
    echo("<select class=\"list\" " .
                 "onchange=\"ifc_post('pool_select',this.value);\">");
    foreach ($array AS $key => $value)
    {
        $name  = strabridge($key, 40);
        $value = is_array($value) ? current($value) : NULL;
        echo("<option value=\"" . x($value) . "\"" .
                      (streq($key, $category) ? " selected>" : ">") .
             x($name) .
             "</option>");
    };
    echo("</select>");

    //list
    if (is_array($array[$category]))
    {
        $template = new template();

        echo("<br>");

        $ifc->set(CMS_L_IFC_CONTENT_130, "description b");
        echo("<div id=\"_pool_object\" " .
                  "data-size=\"14\" " .
                  "class=\"select list\" " .
                  "data-onchange=\"content_pool_select(this.value);\"" .
                  "ondblclick=\"ifc_return();\">");
        foreach ($array[$category] AS $key => $value)
        {
            //type
            $type  = (string)$pool->data->get($value, "type");
            $type  = isset($template->tlist[$type]) ?
                     $template->tlist[$type] : CMS_TEMPLATE_TYPE_NONE;
            $type  = $template->tname[$type];

            //name
            echo("<label title=\"" . x($type) . "\">" .
                 "<input name=\"_pool_object[]\" " .
                        "type=\"checkbox\" " .
                        "value=\"" . x($value) . "\"" .
                        (streq($value, $_pool_object) ? " checked>" : ">") .
                 x($key) .
                 "</label>");
        };
        echo("</div>" .
             "<div style=\"TEXT-ALIGN:right\">");
        $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_pool_object\");");
        $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_pool_object\");");
        $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_pool_object\");");
        echo("</div>" .
             jscript("ifc_custom_select(\"_pool_object\");"));
    };

    //language
    if (CMS_IFC_SELECT && CMS_LANGUAGE_ENABLED)
    {
        $map       = new map("#system/language.image");
        $_language = explode(",", CMS_LANGUAGE_ENABLED);

        echo("<div class=\"language\">" .
             "<a href=\"javascript:ifc_post('pool_select_language','');\" " .
                "title=\"" . CMS_L_LANGUAGE_DEFAULT . "\" " .
                "class=\"language-" . (streq($pool_language, "") ? "on" : "off") . "\">" .
             image("language/default", 16, 12) .
             "</a>");

        foreach ($_language AS $value)
            echo("<a href=\"javascript:ifc_post('pool_select_language','" . qrx($value) . "');\" " .
                    "title=\"" . x($value) . "\" " .
                    "class=\"language-" . (streq($pool_language, $value) ? "on" : "off") . "\">" .
                 image(((($image = $map->get_value($value)) !== NULL) &&
                       is_file(CMS_DATA_PATH . "language/$image")) ?
                       CMS_DATA_URL . "language/$image" :
                       "language/default",
                       16, 12) .
                 "</a>");

        echo("</div>");
    };
    echo("</td>" .

    //display reference
         "<td class=\"iframe\">" .
         "<iframe id=\"content-pool-display\" " .
                 "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                       "ifc_message" => "pool_display",
                                       "pool_object" => $_pool_object])) . "\" " .
                 "sandbox=\"allow-same-origin\">" .
         CMS_L_IFC_CONTENT_090 .
         "</iframe>" .
         "</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "rss":
case "rss_select":
case "rss_add":
case "rss_save":
case "rss_delete":
case "rss_default":
case "rss_assign":
case "_rss_assign":
//..................................................................................................

    if (! $content->publisher) break; //user must be publisher

    if (! cms_load("rss"))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    if (empty($rss_object))    $rss_object = 0;

    $rss = new rss();

    switch (CMS_IFC_MESSAGE)
    {
    case "rss_select":
//..................................................................................................

        $rss_object = $ifc_param;
        break;

    case "rss_add":
//..................................................................................................

        $_rss_object = $rss->add_channel(CMS_L_IFC_CONTENT_043,                      //name
                                         sprintf(CMS_L_IFC_CONTENT_058, CMS_DOMAIN), //description
                                         CMS_ROOT_URL);                              //link

        //create template
        $template           = new template();
        $template->operator = TRUE;

        if ($template_index = $template->add(CMS_L_IFC_CONTENT_043, //name
                                             NULL,                  //category
                                             NULL,                  //page
                                             "<a href=\"" . x(u(CMS_MODULES_URL . "rss.php",
                                                                ["rss_channel" => $_rss_object,
                                                                 "rss_limit"   => 25,
                                                                 "rss_order"   => "published"])) . "\">\n" .
                                             image("rss/icon_rss", NULL, NULL, CMS_L_IFC_CONTENT_078) . "\n" .
                                             CMS_L_IFC_CONTENT_079 . "\n" .
                                             "</a>"))
            $rss->data->set($template_index, $_rss_object, "template");

        if ($rss->save())
        {
            $rss_object   = $_rss_object;
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "rss_save":
//..................................................................................................

        if (empty($ifc_param1)) $ifc_param1 = CMS_L_IFC_CONTENT_043;
        if (empty($ifc_param3)) $ifc_param3 = sprintf(CMS_L_IFC_CONTENT_058, CMS_DOMAIN);
        if (empty($ifc_param4)) $ifc_param4 = CMS_ROOT_URL;

        $rss->set_channel(
            $rss_object,         //index
            $ifc_param1,         //name
            $ifc_param3,         //description
            $ifc_param4,         //link
            $ifc_param5,         //image
            $ifc_param6,         //category
            isset($ifc_param2)); //default

        //update template
        if ($template_index = $rss->data->get($rss_object, "template"))
        {
            $template           = new template();
            $template->operator = TRUE;

            if ($array = $template->get($template_index))
                $template->set($template_index,                                        //index
                               language_get($ifc_param1, FALSE),                       //name
                               isset($array["category"]) ? $array["category"] : NULL,  //category
                               isset($array["page"])     ? $array["page"]     : NULL); //page
        };

        $ifc_response = $rss->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "rss_delete":
//..................................................................................................

        if (empty($_list)) break; //no selection

        $array = [];

        foreach ($_list AS $value)
        {
            if ($_value = $rss->data->get($value, "template")) $array[] = $_value;

            $rss->del_channel($value);

            //remove deleted object from content
            $value = sqlesc("/$value/");
            mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_CHANNEL .
                        "=" .      "REPLACE(" . CMS_DB_CONTENT_CHANNEL . ", '$value', '') " .
                        "WHERE " .  CMS_DB_CONTENT_CHANNEL . " LIKE '%$value%'");
        };

        if ($rss->save())
        {
            //delete templates
            $template           = new template();
            $template->operator = TRUE;

            foreach ($array AS $value) $template->delete($value);

            //deselect object if deleted
            if (in_array($rss_object, $_list)) $rss_object = 0;

            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "rss_default":
//..................................................................................................

        if (empty($_list)) $_list = [];

        $rss->data->move("first");
        while ($key = $rss->data->move("next"))
            $rss->data->set(in_array($key, $_list), $key, "default");

        $ifc_response = $rss->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "rss_assign":
//..................................................................................................

        $object  = $ifc_param;
        $flag    = TRUE;
        $channel = NULL;

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_CHANNEL . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX .
                                    "='" .      sqlesc($object) . "' " .
                                    "LIMIT 1"))
              &&
              ($resultrow = mysql_fetch_row($result))
            )
            $channel = $resultrow[0];

        break;

    case "_rss_assign":
//..................................................................................................

        if (empty($_list)) $channel = NULL;
        else               $channel = "/" . implode("//", $_list). "/";

        if ($content->channel_set($object, $channel))
        {
            $ifc_message = CMS_MSG_DONE;
            break 2;
        };

        $ifc_message = CMS_MSG_ERROR;
        break;

    default:
//..................................................................................................

        $channel = NULL;
        $flag    = NULL;
        break;
    };

    //selected elements
    $array = $flag ? $channel : rss_get_default();
    $array = explode("//", substr($array, 1, -1));
    $array = array_flip($array);

    //menu
    $menu = [CMS_L_COMMAND_ADD .             "|content/command_add"    => "rss_add",
             CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete" => "#rss_delete"];
    if ($flag) $menu[CMS_L_IFC_CONTENT_075 . "|content/command_save"]  =  "_rss_assign";
    else       $menu[CMS_L_IFC_CONTENT_064 . "|content/command_save"]  =  "rss_default";
    $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"]             =  NULL;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit,
                    "rss_object"        => $rss_object,
                    "flag"              => $flag,
                    "channel"           => $channel],
                   NULL,
                   CMS_L_IFC_CONTENT_061);

//javascript

?>
<script>

function s(index) //select
{
    ifc_post("rss_select", index);
};

function content_select_active()
{
    var list = document.querySelectorAll("INPUT[name=\"_list[]\"]");

    list.forEach(object =>
    {
        switch (object.value)
        {
<?php

foreach ($array AS $key => $_)
    echo("case \"" . q($key) . "\":\n");

?>
            if (! object.checked) object.click();
            break;

        default:

            if (object.checked)   object.click();
        };
    });
};

</script>
<?php

    echo("<table class=\"layout\" " .
                "style=\"WIDTH:100%\">" .

         "<colgroup>" .
         "<col>");
    if ($rss_object)
        echo("<col style=\"WIDTH:0\">");
    echo("</colgroup>" .

         "<tr>" .
         "<td>");

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"4\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td rowspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         CMS_L_NAME .
         "</th>" .

    //image
         "<th rowspan=\"2\">" .
         CMS_L_IMAGE .
         "</th>" .

    //description
         "<th>" .
         CMS_L_DESCRIPTION .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //category
         "<th>" .
         CMS_L_IFC_CONTENT_062 .
         "</th>" .

    //link
         "<th>" .
         CMS_L_URL .
         "</th>" .
         "</tr>");

    data_sort($rss->data, "name");

    $rss->data->move("first");
    while ($key = $rss->data->move("next"))
    {
        //name
        $name   = l($rss->data->get($key, "name"));
        $_name  = x($name);
        if (streq($key, $rss_object)) $_name = "<strong>$_name</strong>";

        //image
        $image  = ($image = l($rss->data->get($key, "image"))) ?
                  translate_url($image) : CMS_IMAGES_URL . "content/rss.png";

        //link
        $link   = l($rss->data->get($key, "link"));
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td rowspan=\"2\" " .
                 "class=\"" . (isset($array[$key]) ? "enabled" : "select") . "\">");
        $ifc->set(NULL, "checkbox", $key, streq($key, $rss_object), "_list[]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:s('" . qrx($key) . "');\">" .
             image("content/icon_rss") . " $_name" .
             "</a>" .
             "</td>" .

        //image
             "<td rowspan=\"2\" " .
                 "class=\"image\">" .
             image($image, 75, 75) .
             "</td>" .

        //description
             "<td$varied colspan=\"2\">" .
             "<div class=\"br\">" .
             x(strabridge(l($rss->data->get($key, "description"), 250))) .
             "</div>" .
             "</td>" .
             "</tr>" .
             "<tr>" .

        //category
             "<td$varied>" .
             x(l($rss->data->get($key, "category"))) .
             "</td>" .

        //link
             "<td$varied>" .
             "<a href=\"" . x($link) . "\" " .
                "onclick=\"window.open(this.href);return false;\">" .
             x(strabridge($link)) .
             "</a>" .
             "</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"5\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_list\");");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_list\");");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_list\");");
    $ifc->set($flag ? CMS_L_IFC_CONTENT_076 : CMS_L_IFC_CONTENT_074,
              "button",
              "javascript:content_select_active();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    if ($rss_object)
    {
        //data
        echo("<td>");

        ifc_table_open();

        //title
        echo("<tr>" .
             "<th>" .
             CMS_L_IFC_CONTENT_023 .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td>");

        //name
        $ifc->set(CMS_L_NAME,            "text 40 50 l",     $rss->data->get($rss_object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,    "button b",         "rss_save");

        //default
        $ifc->set(CMS_L_IFC_CONTENT_134, "checkbox b",       TRUE, $rss->data->get($rss_object, "default"));

        //description
        $ifc->set(CMS_L_DESCRIPTION,     "textarea 50x8 bl", $rss->data->get($rss_object, "description"));

        //link
        $ifc->set(CMS_L_URL,             "text 50 bl",       $rss->data->get($rss_object, "link"));

        //image
        $buffer = "javascript:load_page('" .
                  str_replace("%replace%",
                              "'+encodeURIComponent(ifc_get('ifc_param5'))+'",
                              q(cms_url(["ifc_page"          => "image",
                                         "ifc_option"        => "external",
                                         "object"            => "\x1B%replace%",
                                         "ifc_select"        => "object",
                                         "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param5','%return%');"]))) . "');";

        $ifc->set(CMS_L_IMAGE,           "text 40 l",        $rss->data->get($rss_object, "image"));
        $ifc->set("…",                   "button b",         $buffer);

        //category
        $ifc->set(CMS_L_IFC_CONTENT_062, "text 50 l",        $rss->data->get($rss_object, "category"));
        echo("</td>" .
             "</tr>");
        ifc_table_close();
    };
    echo("</tr>" .
         "</table>");

    $ifc->close();

case "configuration":
//..................................................................................................

    if (! $content->operator) break; //user must be operator

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["user"              => $user,
                    "object"            => $object,
                    "sql_select"        => $sql_select,
                    "sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_configuration",
                   CMS_L_IFC_CONTENT_140);

    $system = new system();

    //extra column
    $ifc->set(CMS_L_IFC_CONTENT_121, "title");
    $ifc->set(CMS_L_IFC_CONTENT_144, "text 50 bl", $system->getval("content", "extra_label"));
    $ifc->set(CMS_L_IFC_CONTENT_108, "text 50",    $system->getval("content", "extra_id"));

    $ifc->close();

case "_configuration":
//..................................................................................................

    if (! $content->operator) break; //user must be operator

    $system = new system();
    $system->setval($ifc_param1, "content", "extra_label");
    $system->setval($ifc_param2, "content", "extra_id");

    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "flag":
//..................................................................................................

    $object = $ifc_param;

    if (

    //retrieve flag setting of active object
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_FLAG . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       TRUE,
                       ["user"              => $user,
                        "object"            => $object,
                        "sql_select"        => $sql_select,
                        "sql_filter_field"  => $sql_filter_field,
                        "sql_filter_option" => $sql_filter_option,
                        "sql_filter_value"  => $sql_filter_value,
                        "sql_order"         => $sql_order,
                        "page"              => $page,
                        "limit"             => $limit],
                       "_flag",
                       CMS_L_IFC_CONTENT_140);

        $flag = $resultrow[CMS_DB_CONTENT_FLAG];

        //sitemap
        $ifc->set(CMS_L_IFC_CONTENT_139, "title");

        //exclude from sitemap
        $ifc->set(CMS_L_IFC_CONTENT_057, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_SITEMAP_EXCLUDE);

        //meta robots
        $ifc->set(CMS_L_IFC_CONTENT_056, "title");

        //noindex
        $ifc->set(CMS_L_IFC_CONTENT_037, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX);

        //nofollow
        $ifc->set(CMS_L_IFC_CONTENT_038, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW);

        $ifc->close();
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "_flag":
//..................................................................................................

    $flag  = 0;
    $flag |= isset($ifc_param1) ? CMS_CONTENT_FLAG_SITEMAP_EXCLUDE      : 0;
    $flag |= isset($ifc_param2) ? CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX  : 0;
    $flag |= isset($ifc_param3) ? CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW : 0;

    $ifc_response = $content->flag_set($object, $flag) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "analyze":
case "analyze_span":
//..................................................................................................

    //retrieve document data
    if (
         ! (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                                CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                                CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                                CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                                CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
       )
        ifc_close_external();

    define("CMS_CONTENT_INDEX",           $object);
    define("CMS_CONTENT_DIRECTORY_INDEX", content_get_directory_index(CMS_CONTENT_INDEX));

    //parse document
    $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT]);
    $template = new template();
    cms_param(FALSE, "cms_instance");
    $code     = $template->parse($document,
                                 $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE],
                                 $resultrow[CMS_DB_CONTENT_BUFFER_TITLE],
                                 $resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION],
                                 $resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
    $size     = utf8_strlen($code);
    $text     = htmltoplain($code, TRUE, TRUE, TRUE);
    $_size    = utf8_strlen($text);
    $language = (CMS_LANGUAGE !== "") ? CMS_LANGUAGE : language_detect($text);
    $_text    = language_strip_stopword($text, $language);
    $text     = x($text);
    $list     = tokenize_text($_text);
    $_list    = [];
    $count    = count($list);
    $span     = streq(CMS_IFC_MESSAGE, "analyze_span") ? $ifc_param : (empty($span) ? 0 : $span);
    $buffer   = [0 => ""];

    //initiate word span window
    for ($i = 0; ($i < $span) && ($i < $count); $i++)
        array_push($buffer, utf8_strtolower($list[$i]));

    //count word span occurrences
    for (; $i < $count; $i++)
    {
        array_shift($buffer);
        array_push($buffer, utf8_strtolower($list[$i]));

        $value = implode(" ", $buffer);

        if (isset($_list[$value])) $_list[$value] += 1;
        else                       $_list[$value]  = 1;
    };

    arsort($_list);

    //keyword highlighting
    $keyword = l($resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
    $keyword = tokenize_text($keyword);
    if (! empty($keyword))
    {
        $keyword  = array_unique($keyword);
        $keyword  = array_map(__NAMESPACE__ . "\x", $keyword);
        $_keyword = array_map(function($value) { return preg_quote($value, "/"); }, $keyword);
        $text     = preg_replace("/(?:" . implode("|", $_keyword) . ")/iu",
                                 "<strong>\\0</strong>", $text);
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   NULL,
                   ["span" => $span],
                   NULL,
                   CMS_L_IFC_CONTENT_097);

    //plain text
    ifc_tab_open(CMS_L_IFC_CONTENT_096);

    echo("<div class=\"text\">" .

    //title
         "<h1>" .
         x(l($resultrow[CMS_DB_CONTENT_BUFFER_TITLE])) .
         "</h1>");

    //description
    if ($buffer = stripspaces(remove_format(l($resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION]))))
        echo("<div style=\"FONT-STYLE:italic\">" .
             x($buffer) .
             "</div>");

    //keyword
    if ($buffer = implode("</strong>, <strong>", $keyword))
        echo("<div style=\"FONT-STYLE:italic;MARGIN-TOP:5px\">" .
             CMS_L_IFC_CONTENT_012 . ": " .
             "<strong>" .
             $buffer .
             "</strong>" .
             "</div>");

    //content
    echo("<hr>" .
         $text .
         "</div>");

    //word span selection
    ifc_tab_next(CMS_L_IFC_CONTENT_066);

    ifc_table_open();
    echo("<tr>" .
         "<td>" .
         CMS_L_IFC_CONTENT_102 . ":");
    for ($i = 0; $i <= 4; )
    {
        $flag = $i == $span;
        echo(" <a href=\"javascript:ifc_post('analyze_span','$i');\">");
        if ($flag) echo("<strong>");
        echo(++$i);
        if ($flag) echo("</strong>");
        echo("</a>");
    };
    echo("</td>" .

    //markup quota
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_104 . ":" .
         "</th>" .
         "<td style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(100 * $_size / $size, 2,
                       CMS_L_DECIMAL_SEPARATOR,
                       CMS_L_THOUSAND_SEPARATOR) . "%" .
         "</strong>" .
         "</td>" .

    //content quota
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_096 . ":" .
         "</th>" .
         "<td style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(100 * ($size - $_size) / $size, 2,
                       CMS_L_DECIMAL_SEPARATOR,
                       CMS_L_THOUSAND_SEPARATOR) . "%" .
         "</strong>" .
         "</td>" .

    //spacer
         "<th rowspan=\"2\">" .
         "</th>" .
         "</tr>" .

    //word list
         "<tr>" .
         "<th colspan=\"2\">" .
         CMS_L_IFC_CONTENT_098 .
         "</th>" .
         "<th colspan=\"2\" " .
             "style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_099 .
         "</th>" .
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_100 .
         "</th>" .
         "</tr>");

    $maximum = reset($_list);

    foreach ($_list AS $key => $value)
    {
        if ($value < 2) break;

        $varied = ifc_varied();

        echo("<tr>" .
             "<td$varied colspan=\"2\">" .
             x($key) .
             "</td>" .
             "<td$varied colspan=\"2\" " .
                        "style=\"TEXT-ALIGN:right\">" .
             x($value) .
             "</td>" .
             "<td$varied style=\"TEXT-ALIGN:right\">" .
             number_format(100 * $value / $count, 2,
                           CMS_L_DECIMAL_SEPARATOR,
                           CMS_L_THOUSAND_SEPARATOR) . "%" .
             "</td>" .
             "<td$varied>" .
             "<div class=\"beam\" " .
                  "style=\"--width:" . (int)(100 * $value / $maximum) . "\">" .
             "</div> " .
             "</td>" .
             "</tr>");
    };

    $varied = ifc_varied();

    echo("<tr>" .

    //total word count
         "<th colspan=\"2\" " .
             "style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_101 . ":" .
         "</th>" .
         "<td$varied colspan=\"2\" " .
                    "style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         x($count) .
         "</strong>" .
         "</td>" .

    //total percent
         "<td$varied style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(
             100, 2,
             CMS_L_DECIMAL_SEPARATOR,
             CMS_L_THOUSAND_SEPARATOR) .
         "%" .
         "</strong>" .
         "</td>" .

    //spacer
         "<th>" .
         "</th>" .
         "</tr>");
    ifc_table_close();

    $ifc->set(CMS_L_IFC_CONTENT_103, "info");

    //source code
    ifc_tab_next(CMS_L_IFC_CONTENT_080);

    $ifc->set(NULL, "code_html 80x20 f", $code);

    ifc_tab_close();

    $ifc->close();

//..................................................................................................

case "debug":

    //template operators only
    if (! cms_permission("interface.template.operator", NULL, NULL)) ifc_close_external();

    //retrieve document data
    if (
         ! (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                                CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
       )
        ifc_close_external();

    define("CMS_CONTENT_INDEX", $object);

    //parse document
    $document = new document(
        $resultrow[CMS_DB_CONTENT_BUFFER_TEXT],
        $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);

    //group elements by parent
    $array = [];
    foreach ($document->structure AS $key => $value)
        $array[$value[2]][] = $key;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   NULL,
                   NULL,
                   NULL,
                   CMS_L_IFC_CONTENT_146);

    //generate tree view
    $stack = [];
    $key   = 0;

    echo("<ul>");
    while (TRUE)
    {
        while (list(, $value) = each($array[$key]))
        {
            $data = &$document->structure[$value];
            $id   = $data[0];

            echo("<li>" .
                 "<strong>id:</strong> " .      x($id) . " ︱ " .
                 "<strong>path:</strong> " .    x($data[1]) . " ︱ " .
                 "<strong>type:</strong> " .    x($data[3]) . " ︱ " .
                 "<strong>value:</strong> " .   x(first_words($document->data[$id][CMS_DOCUMENT_VALUE] ?? "", 50)) . " ︱ " .
                 "<strong>default:</strong> " . x(first_words($document->default[$id][CMS_DOCUMENT_VALUE] ?? "", 50)));

            if (isset($array[$value]))
            {
                echo("<ul>");
                $stack[] = $key;
                $key     = $value;
                continue;
            };

            echo("</li>");
        };

        echo("</ul>");

        if (($key = array_pop($stack)) === NULL) break;

        echo("</li>");
    };

    $ifc->close();

case "sql_select":
//..................................................................................................

    $sql_select = $ifc_param;
    $page       = 0;
    break;

case "sql_order":
//..................................................................................................

    $sql_order = streq($sql_order, "$ifc_param DESC") ? $ifc_param : "$ifc_param DESC";
    break;

case "page":
//..................................................................................................

    $page = $ifc_param;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//icon
//..................................................................................................

$icon   = [CMS_CONTENT_STATUS_DRAFT       => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_draft",
                                              CMS_CONTENT_TYPE_DUPLICATE => "content/icon_draft_duplicate",
                                              CMS_CONTENT_TYPE_COPY      => "content/icon_draft_copy"],
           CMS_CONTENT_STATUS_DOCUMENT    => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_document",
                                              CMS_CONTENT_TYPE_DUPLICATE => "content/icon_document_duplicate",
                                              CMS_CONTENT_TYPE_COPY      => "content/icon_document_copy"],
           CMS_CONTENT_STATUS_PUBLICATION => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_publication"]];
$status = [CMS_CONTENT_STATUS_DRAFT       => CMS_L_IFC_CONTENT_031,
           CMS_CONTENT_STATUS_DOCUMENT    => CMS_L_IFC_CONTENT_032,
           CMS_CONTENT_STATUS_PUBLICATION => CMS_L_IFC_CONTENT_033];
$type   = [CMS_CONTENT_TYPE_ORIGINAL      => CMS_L_IFC_CONTENT_034,
           CMS_CONTENT_TYPE_DUPLICATE     => CMS_L_IFC_CONTENT_035,
           CMS_CONTENT_TYPE_COPY          => CMS_L_IFC_CONTENT_036];

//menu
//..................................................................................................

$menu = NULL;
if ($content->test_create())
    $menu[CMS_L_IFC_CONTENT_049 . "|content/command_create"] = "create";
if ($object)
{
    if (
         (! $result = mysql_query("SELECT " . CMS_DB_CONTENT_TYPE . ", " .
                                              CMS_DB_CONTENT_STATUS . ", " .
                                              CMS_DB_CONTENT_WRITER . ", " .
                                              CMS_DB_CONTENT_WRITER_TIME . ", " .
                                              CMS_DB_CONTENT_WRITER_COMMENT . ", " .
                                              CMS_DB_CONTENT_EDITOR . ", " .
                                              CMS_DB_CONTENT_EDITOR_TIME . ", " .
                                              CMS_DB_CONTENT_EDITOR_COMMENT . ", " .
                                              CMS_DB_CONTENT_PUBLISHER . ", " .
                                              CMS_DB_CONTENT_PUBLISHER_TIME . ", " .
                                              CMS_DB_CONTENT_PUBLISHER_COMMENT . ", " .
                                              CMS_DB_CONTENT_TIME . ", " .
                                              CMS_DB_CONTENT_TITLE . ", " .
                                              CMS_DB_CONTENT_AUTHOR . ", " .
                                              CMS_DB_CONTENT_DESCRIPTION . ", " .
                                              CMS_DB_CONTENT_KEYWORD . ", " .
                                              CMS_DB_CONTENT_IMAGE . ", " .
                                              CMS_DB_CONTENT_SENDER . ", " .
                                              CMS_DB_CONTENT_SENDER_TIME . ", " .
                                              CMS_DB_CONTENT_SENDER_COMMENT . " " .
                                  "FROM " .   CMS_DB_CONTENT . " " .
                                  "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
                                  "LIMIT 1"))
         ||
         (! $resultrow = mysql_fetch_assoc($result))
       )
        $object = NULL;
};
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete"]    = "#delete";
if ($content->writer)
    $menu[CMS_L_IFC_CONTENT_095 . "|content/command_publish"]       = "#apply_all";
if ($content->publisher)
{
    $menu[CMS_L_IFC_CONTENT_061 . "|content/command_rss"]           = "rss";
    $menu[CMS_L_IFC_CONTENT_050 . "|content/command_clear"]         = "#clear_cache";
};
if ($content->operator)
    $menu[CMS_L_IFC_CONTENT_140 . "|content/command_configuration"] = "configuration";

//save settings
//..................................................................................................

cms_cache("content." . CMS_USER . ".user",              $user,              TRUE);
cms_cache("content." . CMS_USER . ".object",            $object,            TRUE);
cms_cache("directory." . CMS_USER . ".object",          $directory_object,  TRUE);
cms_cache("content." . CMS_USER . ".sql_select",        $sql_select,        TRUE);
cms_cache("content." . CMS_USER . ".sql_filter_field",  $sql_filter_field,  TRUE);
cms_cache("content." . CMS_USER . ".sql_filter_option", $sql_filter_option, TRUE);
cms_cache("content." . CMS_USER . ".sql_filter_value",  $sql_filter_value,  TRUE);
cms_cache("content." . CMS_USER . ".sql_order",         $sql_order,         TRUE);
cms_cache("content." . CMS_USER . ".page",              $page,              TRUE);
cms_cache("content." . CMS_USER . ".limit",             $limit,             TRUE);

//prepare directory
//..................................................................................................

$_data    = new data("#system/directory");
$flexview = new flexview();
$flexview->import_data($_data);

//get linked documents
$list  = stre($directory_object) ? NULL : [];
$mark  = [];
$depth = -1;
$found = FALSE;

$_data->move("first");
while ($key = $_data->move("next"))
{
    //depth
    if ($depth >= 0)
    {
        switch ($_data->get($key, "#type"))
        {
        case "container":  $depth++; break;
        case "/container": $depth--; break;
        };
    };

    //selected entry
    if (streq($key, $directory_object))
    {
        //start listing
        $depth = 0;

        //mark selected entry
        $mark[$key] = "content/icon_directory_selected";
    };

    $url = $_data->get($key, "url");

    //directory dereferencing
    $_list = [$key => TRUE];
    while (strncmp($url, "directory://", 12) === 0)
    {
        //get directory index
        $_key = substr($url, 12, strcspn($url, "?#", 12));

        //check for loop
        if (isset($_list[$_key])) break;
        $_list[$_key] = TRUE;

        $url = $_data->get($_key, "url");
    };

    //linked entry
    if (strncmp($url, "content://", 10) === 0)
    {
        //get content index
        $_key = substr($url, 10);
        $_key = substr($_key, 0, strspn($_key, "1234567890"));

        //check selected entry
        $_found  = streq($_key, $object);
        $found  |= $_found;

        //selected entry
        if ($depth === 0)
        {
            $list[$_key] = TRUE;
        }

        else
        {
            //contained entry
            if ($depth === 1)
            {
                $list[$_key] = FALSE;
                $mark[$key]  = "content/icon_directory_contained";
            };

            //linked entry
            if ($_found) $mark[$key] = "content/icon_directory_linked";
        };
    };
};

if ($list !== NULL) $list = array_reverse($list, TRUE);

//display
//..................................................................................................

$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["user"              => $user,
                "object"            => $object,
                "sql_select"        => $sql_select,
                "sql_filter_field"  => $sql_filter_field,
                "sql_filter_option" => $sql_filter_option,
                "sql_filter_value"  => $sql_filter_value,
                "sql_order"         => $sql_order,
                "page"              => $page,
                "limit"             => $limit]);

ifc_table_open();
echo("<tr>" .
     "<td class=\"highlight\">");

//user box selection
//..................................................................................................

$data  = new data("#system/permission");
$array = NULL;

$data->move("first");
while ($key = $data->move("next"))
{
    if (! permission_is_user($key)) continue;
    $_key = substr($key, 5);

    if (
         nstreq($_key, CMS_SUPERUSER)
         &&
         cms_permission(cms_application(), NULL, NULL, $_key)
         &&
         cms_permission($_key)
       )
        $array[$data->get($key, "name")] = $_key;
};

if ($array)
{

?>
<script>

function content_user_select(value)
{
    ifc_set("user", value);
    ifc_del("object");
    ifc_post();
};

</script>
<?php

    echo("<label title=\"" . CMS_L_IFC_CONTENT_091 . "\">" .
         image("content/icon_user") . " " .
         "<select onchange=\"content_user_select(this.value);\">" .
         "<option value=\"" . x(CMS_SUPERUSER) . "\"" .
                  (streq($user, CMS_SUPERUSER) ? " selected>" : ">") .
         x(CMS_NAME) .
         "</option>");

    ksort($array);

    foreach ($array AS $key => $value)
        echo("<option value=\"" . x($value) . "\"" .
                      (streq($user, $value) ? " selected>" : ">") .
             x($key) .
             "</option>");

    echo("</select>" .
         "</label> ");
};

//object type selection
//..................................................................................................

$_user = sqlesc(CMS_SUPERUSER);
$array = [CMS_L_IFC_CONTENT_094 =>           NULL,
          CMS_L_IFC_CONTENT_055 =>           CMS_DB_CONTENT_SENDER . "!=''"];

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $array[CMS_L_IFC_CONTENT_143] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $array[CMS_L_IFC_CONTENT_022] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "' " .
                                    "AND " . CMS_DB_CONTENT_WRITER .  "='" . $_user . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $array[CMS_L_IFC_CONTENT_039] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "' " .
                                    "AND " . CMS_DB_CONTENT_WRITER . "!='" . $_user . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DOCUMENT, CMS_CONTENT_ACTION_RECEIVE))
    $array[CMS_L_IFC_CONTENT_030] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DOCUMENT . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_PUBLICATION, CMS_CONTENT_ACTION_RECEIVE))
    $array[CMS_L_IFC_CONTENT_024] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_PUBLICATION . "' " .
                                    "AND " . CMS_DB_CONTENT_TYPE .    "='" . CMS_CONTENT_TYPE_ORIGINAL . "'";

echo("<label title=\"" . CMS_L_IFC_CONTENT_092 . "\">" .
     image("content/icon_box") . " " .
     "<select onchange=\"ifc_post('sql_select',this.value);\">");
foreach ($array AS $key => $value)
{
    $query = "SELECT COUNT(*) " .
             "FROM " .  CMS_DB_CONTENT . " " .
             "WHERE " . CMS_DB_CONTENT_OWNER . "='" . sqlesc($user) . "'";
    if ($value !== NULL) $query .= " AND " .   $value;
    $count = ($result = mysql_query($query)) ? mysql_result($result, 0) : 0;

    echo("<option value=\"$key\"" .
                  (streq($key, $sql_select) ? " selected>" : ">") .
         "$key ($count)" .
         "</option>");
};
echo("</select>" .
     "</label>" .
     "</td>" .
     "</tr>");

//sender column check
//..................................................................................................

$flag_sender = NULL;
$colspan     = 5;

if (streq($sql_select, CMS_L_IFC_CONTENT_055))
{
    $flag_sender = TRUE;
    $colspan++;
};

//author column check
//..................................................................................................

$flag_author = NULL;

if (nstreq($sql_select, CMS_L_IFC_CONTENT_022))
{
    $flag_author = TRUE;
    $colspan++;
};

//extra column check
//..................................................................................................

$flag_extra  = NULL;
$system      = new system();
$extra_label = l($system->getval("content", "extra_label"));
$extra_id    = $system->getval("content", "extra_id");

if (nstre($extra_label . $extra_id))
{
    if (stre($extra_label)) $extra_label = CMS_L_IFC_CONTENT_121;

    $flag_extra = TRUE;
    $colspan++;
};

//object filter
//..................................................................................................

echo("<tr>" .
     "<td>");
$ifc->set(CMS_L_IFC_CONTENT_093, "description");
$_array = [CMS_L_IFC_CONTENT_065   => CMS_DB_CONTENT_INDEX,
           CMS_L_IFC_CONTENT_027   => CMS_DB_CONTENT_TITLE,
           CMS_L_IFC_CONTENT_008   => CMS_DB_CONTENT_DESCRIPTION,
           CMS_L_IFC_CONTENT_012   => CMS_DB_CONTENT_KEYWORD,
           CMS_L_IFC_CONTENT_028   => CMS_DB_CONTENT_TEXT];
if ($flag_sender)
    $_array[CMS_L_IFC_CONTENT_003] =  CMS_DB_CONTENT_SENDER;
if ($flag_author)
    $_array[CMS_L_IFC_CONTENT_047] =  CMS_DB_CONTENT_AUTHOR;
if ($flag_extra)
{
    $_extra_label = $extra_label;
    $i            = 0;
    while (isset($_array[$_extra_label]))
        $_extra_label = $extra_label . " (" . ++$i . ")";
    $_array[$_extra_label] = CMS_DB_CONTENT_EXTRA_VALUE;
};
$ifc->set($_array,
          "select",
          $sql_filter_field,
          NULL,
          "_sql_filter_field");
$ifc->set([CMS_L_IFC_CONTENT_067 => " LIKE '#value#%'",
           CMS_L_IFC_CONTENT_068 => " LIKE '%#value#%'",
           CMS_L_IFC_CONTENT_069 => " LIKE '%#value#'",
           CMS_L_IFC_CONTENT_070 => ">'#value#'",
           CMS_L_IFC_CONTENT_071 => "='#value#'",
           CMS_L_IFC_CONTENT_072 => "<'#value#'"],
          "select",
          $sql_filter_option,
          NULL,
          "_sql_filter_option");
$ifc->set(NULL,                  "text 20 40 :", $sql_filter_value, NULL, "_sql_filter_value");

$ifc->set(CMS_L_COMMAND_CONFIRM, "button");
$ifc->set(CMS_L_IFC_CONTENT_073, "button",       "javascript:ifc_reset(1);ifc_submit();");

//row display limit per page
//..................................................................................................

$ifc->set(CMS_L_IFC_CONTENT_060, "label");
$ifc->set([ 10 =>  "10",
            25 =>  "25",
            50 =>  "50",
           100 => "100",
           250 => "250",
           500 => "500"],
          "select :",
          $limit,
          NULL,
          "_limit");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("<table class=\"layout\" " .
            "style=\"WIDTH:100%\">" .

     "<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//display directory
//..................................................................................................

echo("<div class=\"content-directory\">");
ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_CONTENT_011 .
     "</th>" .
     "</tr>");

//selected object not found note
if ((! $found) && nstre($object . $directory_object))
    echo("<tr>" .
         "<td class=\"info\">" .
         CMS_L_IFC_CONTENT_137 .
         "</td>" .
         "</tr>");

//directory
echo("<tr>" .
     "<td>");
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
$flexview->show_tree($directory_object,
                     "javascript:ifc_post('directory_select','%index%');",
                     "name",
                     $mark,
                     directory_get_type());
echo("</td>" .
     "</tr>");
ifc_table_close();
echo("</div>" .

     "</td>" .

//selected object information
//..................................................................................................

     "<td>");
if ($object)
{
    echo("<div class=\"content-information\">");
    ifc_table_open();

    //title
    echo("<tr>" .
         "<th colspan=\"5\" " .
             "title=\"" . CMS_L_IFC_CONTENT_138 . "\" " .
             "class=\"br\">" .
         image("content/icon_info") . " " .
         x("\"" . l($resultrow[CMS_DB_CONTENT_TITLE]) . "\"") .
         "</th>" .
         "</tr>" .
         "<tr>");

    $rowspan  = 2 +
                (($description = l($resultrow[CMS_DB_CONTENT_DESCRIPTION]))  ? 1 : 0) +
                (                  $resultrow[CMS_DB_CONTENT_WRITER_COMMENT] ? 1 : 0) +
                (($keyword     = l($resultrow[CMS_DB_CONTENT_KEYWORD]))      ? 1 : 0);
    $_colspan = " colspan=\"2\"";

    //image
    if ($image = l($resultrow[CMS_DB_CONTENT_IMAGE]))
    {
        $image = translate_url($image);
        echo("<td rowspan=\"$rowspan\">" .
             image($image, 75, 75) .
             "</td>");

        $_colspan = "";
    };

    //status
    echo("<td$_colspan class=\"varied\">" .
         CMS_L_IFC_CONTENT_006 . ":" .
         "</td>" .
         "<td>" .
         $status[$resultrow[CMS_DB_CONTENT_STATUS]]);
    if ($resultrow[CMS_DB_CONTENT_WRITER_TIME] != $resultrow[CMS_DB_CONTENT_TIME])
        echo(" " . CMS_L_IFC_CONTENT_084);
    echo("</td>" .

    //type
         "<td class=\"varied\">" .
         CMS_L_IFC_CONTENT_007 . ":" .
         "</td>" .
         "<td>" .
         $type[$resultrow[CMS_DB_CONTENT_TYPE]] .
         "</td>" .
         "</tr>" .
         "<tr>" .

    //editor
         "<td$_colspan class=\"varied\">" .
         CMS_L_IFC_CONTENT_009 . ":" .
         "</td>" .
         "<td>");
    $name = $data->get("user." . $resultrow[CMS_DB_CONTENT_WRITER], "name");
    echo(x($name ? $name : $resultrow[CMS_DB_CONTENT_WRITER]) .
         "</td>" .

    //editor time
         "<td class=\"varied\">" .
         CMS_L_IFC_CONTENT_010 . ":" .
         "</td>" .
         "<td>" .
         x(friendly_date($resultrow[CMS_DB_CONTENT_WRITER_TIME])) .
         "</td>" .
         "</tr>");

    //description
    if ($description)
        echo("<tr>" .
             "<td$_colspan class=\"varied\">" .
             CMS_L_IFC_CONTENT_008 . ":" .
             "</td>" .
             "<td colspan=\"3\" " .
                 "class=\"br\">" .
             parse_text($description) .
             "</td>" .
             "</tr>");

    //keyword
    if ($keyword)
        echo("<tr>" .
             "<td$_colspan class=\"varied\">" .
             CMS_L_IFC_CONTENT_012 . ":" .
             "</td>" .
             "<td colspan=\"3\" " .
                 "class=\"br\">" .
             x($keyword) .
             "</td>" .
             "</tr>");

    //comment
    if ($resultrow[CMS_DB_CONTENT_WRITER_COMMENT])
        echo("<tr>" .
             "<td$_colspan class=\"varied\">" .
             CMS_L_IFC_CONTENT_017 . ":" .
             "</td>" .
             "<td colspan=\"3\">" .
             "<div class=\"br\">" .
             x($resultrow[CMS_DB_CONTENT_WRITER_COMMENT]) .
             "</div>" .
             "</td>" .
             "</tr>");

    //sender
    if ($resultrow[CMS_DB_CONTENT_SENDER_TIME])
    {
        echo("<tr>" .
             "<th colspan=\"5\">" .
             CMS_L_IFC_CONTENT_002 .
             "</th>" .
             "</tr>" .
             "<tr>" .
             "<td colspan=\"2\" " .
                 "class=\"varied\">" .
             CMS_L_IFC_CONTENT_003 . ":" .
             "</td>" .
             "<td>" .
             x($resultrow[CMS_DB_CONTENT_SENDER]) .
             "</td>" .
             "<td class=\"varied\">" .
             CMS_L_IFC_CONTENT_004 . ":" .
             "</td>" .
             "<td>" .
             x(friendly_date($resultrow[CMS_DB_CONTENT_SENDER_TIME])) .
             "</td>" .
             "</tr>");
        if ($resultrow[CMS_DB_CONTENT_SENDER_COMMENT])
            echo("<tr>" .
                 "<td colspan=\"2\" " .
                     "class=\"varied\">" .
                 CMS_L_IFC_CONTENT_017 . ":" .
                 "</td>" .
                 "<td colspan=\"3\">" .
                 "<div class=\"br\">" .
                 x($resultrow[CMS_DB_CONTENT_SENDER_COMMENT]) .
                 "</div>" .
                 "</td>" .
                 "</tr>");
    };

    //authorization
    if ($resultrow[CMS_DB_CONTENT_EDITOR_TIME])
    {
        echo("<tr>" .
             "<th colspan=\"5\">" .
             CMS_L_IFC_CONTENT_014 .
             "</th>" .
             "</tr>" .
             "<tr>" .

        //editor
             "<td colspan=\"2\" " .
                 "class=\"varied\">" .
             CMS_L_IFC_CONTENT_015 . ":" .
             "</td>" .
             "<td>" .
             x($resultrow[CMS_DB_CONTENT_EDITOR]) .
             "</td>" .

        //editor time
             "<td class=\"varied\">" .
             CMS_L_IFC_CONTENT_016 . ":" .
             "</td>" .
             "<td>" .
             x(friendly_date($resultrow[CMS_DB_CONTENT_EDITOR_TIME])) .
             "</td>" .
             "</tr>");

        //editor comment
        if ($resultrow[CMS_DB_CONTENT_EDITOR_COMMENT])
            echo("<tr>" .
                 "<td colspan=\"2\" " .
                     "class=\"varied\">" .
                 CMS_L_IFC_CONTENT_017 . ":" .
                 "</td>" .
                 "<td colspan=\"3\">" .
                 "<div class=\"br\">" .
                 x($resultrow[CMS_DB_CONTENT_EDITOR_COMMENT]) .
                 "</div>" .
                 "</td>" .
                 "</tr>");
    };

    //publication
    if ($resultrow[CMS_DB_CONTENT_PUBLISHER_TIME])
    {
        echo("<tr>" .
             "<th colspan=\"5\">" .
             CMS_L_IFC_CONTENT_018 .
             "</th>" .
             "</tr>" .
             "<tr>" .

        //publisher
             "<td colspan=\"2\" " .
                 "class=\"varied\">" .
             CMS_L_IFC_CONTENT_019 . ":" .
             "</td>" .
             "<td>" .
             x($resultrow[CMS_DB_CONTENT_PUBLISHER]) .
             "</td>" .

        //publisher time
             "<td class=\"varied\">" .
             CMS_L_IFC_CONTENT_020 . ":" .
             "</td>" .
             "<td>" .
             x(friendly_date($resultrow[CMS_DB_CONTENT_PUBLISHER_TIME])) .
             "</td>" .
             "</tr>");

        //publisher comment
        if ($resultrow[CMS_DB_CONTENT_PUBLISHER_COMMENT])
            echo("<tr>" .
                 "<td colspan=\"2\" " .
                     "class=\"varied\">" .
                 CMS_L_IFC_CONTENT_017 . ":" .
                 "</td>" .
                 "<td colspan=\"3\">" .
                 "<div class=\"br\">" .
                 x($resultrow[CMS_DB_CONTENT_PUBLISHER_COMMENT]) .
                 "</div>" .
                 "</td>" .
                 "</tr>");
    };

    ifc_table_close();
    echo("</div>");
};

//generate object list query
//..................................................................................................

$_list  = ($list === NULL) ? "" : implode("', '", sqlesc(array_keys($list)));
$_query = "";

$query_version  = "SELECT DISTINCT " . CMS_DB_CONTENT_VERSION_CONTENT . " " .
                  "FROM " .            CMS_DB_CONTENT_VERSION;
$query_schedule = "SELECT DISTINCT " . CMS_DB_CONTENT_SCHEDULE_CONTENT . " " .
                  "FROM " .            CMS_DB_CONTENT_SCHEDULE;

$query   =  "SELECT " .    CMS_DB_CONTENT_INDEX . ", " .
                           CMS_DB_CONTENT_TYPE . ", " .
                           CMS_DB_CONTENT_STATUS . ", " .
                           CMS_DB_CONTENT_WRITER_TIME . ", " .
                           CMS_DB_CONTENT_TIME . ", " .
                           CMS_DB_CONTENT_TITLE . ", " .
                           CMS_DB_CONTENT_AUTHOR . ", " .
                           CMS_DB_CONTENT_INDEX . " IN ($query_version) AS `#version`, " .
                           CMS_DB_CONTENT_INDEX . " IN ($query_schedule) AS `#schedule` ";

//sender
if (streq($sql_select, CMS_L_IFC_CONTENT_055))                    $query  .= ", " .
                           CMS_DB_CONTENT_SENDER;

//extra
if ($flag_extra)                                                  $query  .= ", " .
                           CMS_DB_CONTENT_EXTRA_VALUE . ", " .
                           CMS_DB_CONTENT_EXTRA_TYPE . ", " .
                           CMS_DB_CONTENT_EXTRA_COLOR;

$query  .= " FROM " .      CMS_DB_CONTENT .
           " WHERE " .     CMS_DB_CONTENT_OWNER . "='" . sqlesc($user) . "' ";

//object type selection
if (isset($array[$sql_select]) && ($array[$sql_select] !== NULL)) $_query .=
                           $array[$sql_select];

//object filter
if (nstre($sql_filter_value))                                     $_query .= (nstre($_query) ?
           " AND " : "") . sqlesc($sql_filter_field) .
                           str_replace("#value#", sqlesc($sql_filter_value), $sql_filter_option);
if (nstre($_query))
{
    if (nstre($_list))                                            $query  .=
            "AND (" .      CMS_DB_CONTENT_INDEX . " IN ('$_list') " .
            "OR (" .       $_query . ")) ";
    else                                                          $query  .=
            "AND " .       $_query . " ";
};

$query  .=  "ORDER BY ";
if (isset($list[$object]))                                        $query  .=
                           CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' DESC, ";
if ($list !== NULL)                                               $query  .=
            "FIELD(" .     CMS_DB_CONTENT_INDEX . ", '$_list') DESC, ";
$query  .=                 $sql_order;

//empty result
//..................................................................................................

if (
     (! $result = mysql_query($query))
     ||
     (! $count = mysql_num_rows($result))
   )
{
    $ifc->set(CMS_L_IFC_CONTENT_026, "info");
    echo("</td>" .
         "</tr>" .
         "</table>");
    $ifc->close();
};

//javascript
//..................................................................................................

?>
<script>

function s1(index) //select
{
    ifc_post("select", index);
};

function e(index) //edit
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(CMS_MODULES_URL . "content.php",
                                   ["content_index"  => "\x1B%replace%",
                                    "content_option" => CMS_TEMPLATE_OPTION_ALL,
                                    "content_user"   => $user],
                                   TRUE))));?>");
};

function m(index) //meta
{
    ifc_post("meta", index);
};

function a1(index) //apply
{
    ifc_post("apply", index);
};

function r1(index) //revert
{
    if (confirm("<?php echo(q(CMS_L_IFC_CONTENT_088))?>"))
        ifc_post("revert", index);
};

function d1(index) //display
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "ifc_option"  => "external",
                                    "object"      => "\x1B%replace%"]))));?>");
};

function vs(index) //version store
{
    ifc_post("version_store", index);
};

function v(index) //version
{
    ifc_post("version", index);
};

function a2(index) //authorize
{
    ifc_post("authorize", index);
};

function dd(index) //derive draft
{
    ifc_post("derive_draft", index);
};

function p1(index) //publish
{
    ifc_post("publish", index);
};

function r2(index) //rss
{
    ifc_post("rss_assign", index);
};

function f(index) //flag
{
    ifc_post("flag", index);
};

function w(index) //withdraw
{
    if (confirm("<?php echo(q(CMS_L_IFC_CONTENT_054))?>?"))
        ifc_post("withdraw", index);
};

function d2(index) //duplicate
{
    ifc_post("duplicate", index);
};

function c(index) //copy
{
    ifc_post("copy", index);
};

function s2(index) //send
{
    ifc_post("schedule", index);
};

function s3(index) //send
{
    ifc_post("send", index);
};

function p2(number) //page
{
    ifc_post("page", number);
};

function e2(url) //extra
{
    load_page(url);
    return false;
};

</script>
<?php

//pagination
//..................................................................................................

if ($count > $limit)
{
    $count  = ceil($count / $limit) - 1;
    if ($page > $count) $page    = $count;
    $offset = $page - 5;
    $end    = $page + 5;
    if ($offset < 0)    $end    += 0 - $offset;
    if ($end > $count)
    {
        $offset += $count - $end;
        $end     = $count;
    };
    if ($offset < 0)    $offset  = 0;

    echo("<div class=\"pagination\">" .
         "<label>" . CMS_L_IFC_CONTENT_082 . "</label>");

    //start
    if ($offset)
        echo("<a href=\"javascript:p2(0);\" " .
                "title=\"" . CMS_L_COMMAND_START . "\">" .
             "«" .
             "</a>");

    //previous
    if ($page)
        echo("<a href=\"javascript:p2(" . ($page - 1) . ");\" " .
                "title=\"" . CMS_L_COMMAND_PREVIOUS . "\">" .
             "‹" .
             "</a>");

    if ($offset)        echo(" … ");

    //page
    for ($i = $offset; $i <= $end; )
        echo("<a href=\"javascript:p2($i);\">" .
             (($i++ == $page) ? "<strong>$i</strong>" : $i) .
             "</a>");

    if ($end < $count)  echo(" … ");

    //next
    if ($page < $count)
        echo("<a href=\"javascript:p2(" . ($page + 1) . ");\" " .
                "title=\"" . CMS_L_COMMAND_NEXT . "\">" .
             "›" .
             "</a>");

    //end
    if ($end < $count)
        echo("<a href=\"javascript:p2($count);\" " .
                "title=\"" . CMS_L_COMMAND_END . "\">" .
             "»" .
             "</a>");

    echo("</div>");

    mysql_data_seek($result, $page * $limit);
};

//object list display
//..................................................................................................

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" . //selection
     "<col style=\"WIDTH:0\">"); //index
if ($flag_sender) echo("<col>"); //sender
echo("<col>" .                   //title
     "<col style=\"WIDTH:0\">"); //control
if ($flag_author) echo("<col>"); //author
echo("<col>" .                   //date
     "</colgroup>" .

     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//index
     "<th>");
$ifc->set(CMS_L_IFC_CONTENT_065,
          "button",
          "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_INDEX . "');");
echo("</th>");

//sender
if ($flag_sender)
{
    echo("<th>");
    $ifc->set(CMS_L_IFC_CONTENT_003,
              "button",
              "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_SENDER . "');");
    echo("</th>");
};

//title
echo("<th>");
$ifc->set(CMS_L_IFC_CONTENT_027,
          "button",
          "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_TITLE . "');");
echo("</th>" .

//control
     "<th>" .
     CMS_L_IFC_CONTENT_127 .
     "</th>");

//author
if ($flag_author)
{
    echo("<th>");
    $ifc->set(CMS_L_IFC_CONTENT_047,
              "button",
              "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_AUTHOR . "');");
    echo("</th>");
};

//date
echo("<th>");
$ifc->set(CMS_L_IFC_CONTENT_010,
          "button",
          "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_WRITER_TIME . "');");
echo("</th>");

//extra
if ($flag_extra)
{
    echo("<th>");
    $ifc->set($_extra_label,
              "button",
              "javascript:ifc_post('sql_order','" . CMS_DB_CONTENT_EXTRA_VALUE . "');");
    echo("</th>");
};

echo("</tr>");

$i = 0;

while (
        ($i++ < $limit)
        &&
        ($resultrow = mysql_fetch_assoc($result))
      )
{
    $index = $resultrow[CMS_DB_CONTENT_INDEX];
    $title = x(strabridge(l($resultrow[CMS_DB_CONTENT_TITLE])));

    //selected
    if (streq($resultrow[CMS_DB_CONTENT_INDEX], $object))
    {
        //edited
        if ($resultrow[CMS_DB_CONTENT_WRITER_TIME] != $resultrow[CMS_DB_CONTENT_TIME])
            $title = "<strong style=\"FONT-STYLE:italic\">" .
                     $title . " (" . CMS_L_IFC_CONTENT_141 . ")" .
                     "</strong>";

        else
            $title = "<strong>" .
                     $title .
                     "</strong>";
    }

    //edited
    elseif ($resultrow[CMS_DB_CONTENT_WRITER_TIME] != $resultrow[CMS_DB_CONTENT_TIME])
    {
        $title = "<span style=\"FONT-STYLE:italic\">" .
                 $title . " (" . CMS_L_IFC_CONTENT_141 . ")" .
                 "</span>";
    };

    $varied = ifc_varied();

    //directory
    if ($flag = isset($list[$index]))
        $varied = " class=\"" . (nstre($varied) ? "related2" : "related") . "\"";

    echo("<tr>" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $index, streq($index, $object), "list[]");
    echo("</td>" .

    //index
         "<td$varied style=\"TEXT-ALIGN:right\">" .
         x($index) .
         "</td>");

    //sender
    if (streq($sql_select, CMS_L_IFC_CONTENT_055))
        echo("<td$varied>" .
             x($resultrow[CMS_DB_CONTENT_SENDER]) .
             "</td>");

    $_index  = qrx($index);
    $_status = (int)$resultrow[CMS_DB_CONTENT_STATUS];
    $_type   = (int)$resultrow[CMS_DB_CONTENT_TYPE];

    echo("<td$varied>");

    //directory
    if ($flag)
        echo(image("content/icon_directory_" . (($list[$index] === TRUE) ? "selected" : "contained")));

    echo("<a href=\"javascript:s1('$_index');\"" .
            "title=\"" . x($status[$_status] . " " . $type[$_type]) . "\"" .
            (($_status === CMS_CONTENT_STATUS_PUBLICATION) ? " class=\"green\"" : "") . ">" .

    //icon
         image($icon[$_status][$_type]) .

    //title
         " $title" .

    //control
         "</a>" .
         "</td>" .
         "<td$varied>" .

    //display
         "<a href=\"javascript:d1('$_index');\" " .
            "title=\"" . CMS_L_COMMAND_SHOW . "\">" .
         image("content/button_display") .
         "</a>");

    //editing functions
    if ($content->test_update($index))
    {
        //edit
        echo("<a href=\"javascript:e('$_index');\" " .
                "title=\"" . CMS_L_COMMAND_EDIT . "\">" .
             image("content/button_edit") .
             "</a>" .

        //meta
             "<a href=\"javascript:m('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_077 . "\">" .
             image("content/button_meta") .
             "</a>");

        if ($resultrow[CMS_DB_CONTENT_WRITER_TIME] !== $resultrow[CMS_DB_CONTENT_TIME])
        {
            //apply
            echo("<a href=\"javascript:a1('$_index');\" " .
                    "title=\"" . CMS_L_IFC_CONTENT_087 . "\">" .
                 image("content/button_apply") .
                 "</a>" .

            //revert
                 "<a href=\"javascript:r1('$_index');\" " .
                    "title=\"" . CMS_L_IFC_CONTENT_088 . "\">" .
                 image("content/button_revert") .
                 "</a>");
        }
        else
        {
            echo(image("content/button_apply_disabled") .
                 image("content/button_revert_disabled"));
        };
    }
    else
    {
        echo(image("content/button_edit_disabled") .
             image("content/button_apply_disabled") .
             image("content/button_revert_disabled") .
             image("content/button_meta_disabled"));
    };

    //separator
    echo(image("content/separator"));

    //rss
    if ($content->test_channel_set($index))
        echo("<a href=\"javascript:r2('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_061 . "\">" .
             image("content/button_rss") .
             "</a>");

    else
        echo(image("content/button_rss_disabled"));

    //flag
    if ($content->test_flag_set($index))
        echo("<a href=\"javascript:f('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_140 . "\">" .
             image("content/button_flag") .
             "</a>");

    else
        echo(image("content/button_flag_disabled"));

    //separator
    echo(image("content/separator"));

    //version store
    if ($content->test_version_store($index))
        echo("<a href=\"javascript:vs('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_110 . "\">" .
             image("content/button_version_store") .
             "</a>");

    else
        echo(image("content/button_version_store_disabled"));

    //version
    if (($_type !== CMS_CONTENT_TYPE_COPY) && $resultrow["#version"])
        echo("<a href=\"javascript:v('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_085 . "\">" .
             image("content/button_version") .
             "</a>");

    else
        echo(image("content/button_version_disabled"));

    //separator
    echo(image("content/separator"));

    //authorize
    if ($content->test_authorize($index))
        echo("<a href=\"javascript:a2('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_029 . "\">" .
             image("content/button_authorize") .
             "</a>");

    //derive draft
    elseif ($content->test_derive_draft($index))
        echo("<a href=\"javascript:dd('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_053 . "\">" .
             image("content/button_derive_draft") .
             "</a>");

    else
        echo(image("content/button_authorize_disabled"));

    //publish
    if ($content->test_publish($index))
        echo("<a href=\"javascript:p1('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_040 . "\">" .
             image("content/button_publish") .
             "</a>");

    else
        echo(image("content/button_publish_disabled"));

    //withdraw
    if ($content->test_withdraw($index))
        echo("<a href=\"javascript:w('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_054 . "\">" .
             image("content/button_withdraw") .
             "</a>");

    else
        echo(image("content/button_withdraw_disabled"));

    echo(image("content/separator"));

    //schedule
    if ($resultrow["#schedule"])
        echo("<a href=\"javascript:s2('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_120 . "\">" .
             image("content/button_schedule") .
             "</a>");

    else
        echo(image("content/button_schedule_disabled"));

    echo(image("content/separator"));

    //duplicate
    if ($content->test_duplicate($index))
        echo("<a href=\"javascript:d2('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_051 . "\">" .
             image("content/button_duplicate") .
             "</a>");

    else
        echo(image("content/button_duplicate_disabled"));

    //copy
    if ($content->test_copy($index))
        echo("<a href=\"javascript:c('$_index');\" " .
                "title=\"" . CMS_L_IFC_CONTENT_052 . "\">" .
             image("content/button_copy") .
             "</a>");

    else
        echo(image("content/button_copy_disabled"));

    echo(image("content/separator"));

    //send
    echo("<a href=\"javascript:s3('$_index');\" " .
            "title=\"" . CMS_L_IFC_CONTENT_046 . "\">" .
         image("content/button_send") .
         "</a>" .

    "</td>");

    //author
    if ($flag_author)
        echo("<td$varied>" .
             x($resultrow[CMS_DB_CONTENT_AUTHOR]) .
             "</td>");

    //date
    echo("<td$varied>" .
         friendly_date($resultrow[CMS_DB_CONTENT_WRITER_TIME]) .
         "</td>");

    //extra
    if ($flag_extra)
    {
        $extra_value = l($resultrow[CMS_DB_CONTENT_EXTRA_VALUE]);
        $extra_type  = $resultrow[CMS_DB_CONTENT_EXTRA_TYPE];
        $extra_color = $resultrow[CMS_DB_CONTENT_EXTRA_COLOR];

        echo("<td$varied>" .
             "<div class=\"content-extra\"" .
             (nstre($extra_color) ? " style=\"BACKGROUND-COLOR:" . x($extra_color) . "\">" : ">"));

        //process extra value
        switch ($extra_type)
        {
        case "href":
            if (! preg_match("/^([^:]+):(.+)$/s", $extra_value, $value)) break;

            $_type = $value[1];
            $value = $value[2];

            switch ($_type)
            {
            case "directory":

                $data  = new data("#system/directory");
                $url   = cms_url(["ifc_page"   => "directory",
                                  "object"     => $value,
                                  "ifc_option" => "external"]);
                $value = l($data->get($value, "name"));
                break;

            case "content":

                $url   = translate_url("$_type://$value");
                $value = CMS_L_IFC_CONTENT_145 . ": " . x($value);
                break;

            default:
            case "address":

                $url = translate_url($value);
                break;
            };

            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($value) .
                 "</a>");
            break;

        case "plugin":

            $url = translate_url($extra_value);
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($extra_value) .
                 "</a>");
            break;

        case "text":

            echo(parse_text($extra_value));
            break;

        case "image":
        case "thumbnail":

            $data  = new data("#system/image");
            $url   = cms_url(["ifc_page"   => "image",
                              "object"     => $extra_value,
                              "ifc_option" => "external"]);
            $value = translate_url($extra_value);
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 image($value, 100, 75) .
                 "</a>");
            break;

        case "download":
        case "media":
        case "template":

            $data  = new data("#system/$extra_type");
            $url   = cms_url(["ifc_page"   => $extra_type,
                              "object"     => $extra_value,
                              "ifc_option" => "external"]);
            $value = l($data->get($extra_value, "name"));
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($value) .
                 "</a>");
            break;

        case "value":
        case "repeat":
        case "switch":
        default:

            echo(nl2br(x($extra_value)));
            break;
        };

        echo("</div>" .
             "</td>");
    };

    echo("</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"$colspan\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>" .
     "</tr>" .
     "</table>");

$ifc->close();

?>