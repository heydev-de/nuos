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
if (! cms_load("directory")) ifc_inactive($ifc_page);
if (! cms_load("flexview"))  ifc_inactive($ifc_page);

//permission
ifc_permission(["" => CMS_L_ACCESS]);

if (! isset($object)) $object = (string)cms_cache("directory." . CMS_USER . ".object");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "type":
case "type_select":
case "type_add":
case "type_save":
case "type_delete":
//..................................................................................................

    if (empty($type_object)) $type_object = 0;

    $data = new data("#system/directory.type");

    switch (CMS_IFC_MESSAGE)
    {
    case "type_select":
//..................................................................................................

        $type_object = $ifc_param;
        break;

    case "type_add":
//..................................................................................................

        $data->set_buffer([["name" => CMS_L_IFC_DIRECTORY_017]]);
        $_type_object = $data->append();
        $data->set($_type_object, $_type_object, "#subtype");

        if ($data->save())
        {
            $type_object  = $_type_object;
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "type_save":
//..................................................................................................

        mkpath(CMS_DATA_PATH . "directory.type");

        //name
        if ($ifc_param1) $data->set($ifc_param1, $type_object, "name");

        //icon normal
        if ($ifc_file1 && preg_match("/\.(?:gif|jpe?g|png|svg)$/i", $ifc_file1_name, $format))
        {
            //delete existing
            if (
                 ($_format = $data->get($type_object, "format"))
                 &&
                 is_file(CMS_DATA_PATH . "directory.type/$type_object$_format")
               )
                unlink(CMS_DATA_PATH . "directory.type/$type_object$_format");

            $format = strtolower($format[0]);
            if (streq($format, ".jpeg")) $format = ".jpg";

            if (move_uploaded_file($ifc_file1, CMS_DATA_PATH . "directory.type/$type_object$format"))
                $data->set($format, $type_object, "format");
        };

        //icon selected
        if ($ifc_file2 && preg_match("/\.(?:gif|jpe?g|png|svg)$/i", $ifc_file2_name, $format))
        {
            //delete existing
            if (
                 ($_format = $data->get($type_object, "+format"))
                 &&
                 is_file(CMS_DATA_PATH . "directory.type/$type_object$_format")
               )
                unlink(CMS_DATA_PATH . "directory.type/$type_object$_format");

            $format = strtolower($format[0]);
            if (streq($format, ".jpeg")) $format = ".jpg";

            if (move_uploaded_file($ifc_file2, CMS_DATA_PATH . "directory.type/+$type_object$format"))
                $data->set($format, $type_object, "+format");
        };

        $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "type_delete":
//..................................................................................................

        if (empty($list)) break; //no selection

        foreach ($list AS $value)
        {
            //icon normal
            if (
                 ($format = $data->get($value, "format"))
                 &&
                 is_file(CMS_DATA_PATH . "directory.type/$value$format")
               )
                unlink(CMS_DATA_PATH . "directory.type/$value$format");

            //icon selected
            if (
                 ($format = $data->get($value, "+format"))
                 &&
                 is_file(CMS_DATA_PATH . "directory.type/+$value$format")
               )
                unlink(CMS_DATA_PATH . "directory.type/+$value$format");

            $data->del($value);
        };

        //remove deleted objects from directory
        $data_directory = new data("#system/directory");

        $data_directory->move("first");
        while ($key = $data_directory->move("next"))
        {
            $subtype = $data_directory->get($key, "#subtype");
            if (in_array($subtype, $list)) $data_directory->del($key, "#subtype");
        };

        $data_directory->save();

        //deselect object if deleted
        if (in_array($type_object, $list)) $type_object = 0;

        $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    //menu
    $menu = [CMS_L_COMMAND_ADD .             "|directory/command_add"    => "type_add",
             CMS_L_COMMAND_DELETE_SELECTED . "|directory/command_delete" => "#type_delete",
             CMS_L_COMMAND_CLOSE .           "|directory/command_cancel" => NULL];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object"      => $object,
                    "type_object" => $type_object],
                   NULL,
                   CMS_L_IFC_DIRECTORY_062);

    echo("<table class=\"layout\" " .
                "style=\"WIDTH:100%\">" .

         "<colgroup>" .
         "<col>" .
         ($type_object ? "<col style=\"WIDTH:0\">" : "") .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"3\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         CMS_L_NAME .
         "</th>" .

    //icon normal
         "<th>" .
         CMS_L_IFC_DIRECTORY_064 .
         "</th>" .

    //icon selected
         "<th>" .
         CMS_L_IFC_DIRECTORY_065 .
         "</th>" .
         "</tr>");

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $name   = x($data->get($key, "name"));
        if (streq($key, $type_object)) $name = "<strong>$name</strong>";
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $key, streq($key, $type_object), "list[]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:ifc_post('type_select','" . qrx($key) . "');\">" .
             $name .
             "</a>" .

        //icon normal
             "</td>" .
             "<td class=\"image\">");
        if (($format = $data->get($key, "format")) !== NULL)
            echo(image(CMS_DATA_URL . "directory.type/$key$format", 16, 16) . " " . x($format));
        echo("</td>" .

        //icon selected
             "<td class=\"image\">");
        if (($format = $data->get($key, "+format")) !== NULL)
            echo(image(CMS_DATA_URL . "directory.type/+$key$format", 16, 16) . " " . x($format));
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

    if ($type_object)
    {
        $name = $data->get($type_object, "name");

        //data
        echo("<td>");

        ifc_table_open();

        //title
        echo("<tr>" .
             "<th>" .
             CMS_L_IFC_DIRECTORY_007 .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td>");

        //name
        $ifc->set(CMS_L_NAME,                                              "text 40 50", $name);

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,                                      "button b",   "type_save");

        //icon normal
        $ifc->set(CMS_L_IFC_DIRECTORY_064 . " (GIF, JPG, PNG, SVG, WEBP)", "file 50 b");

        //icon selected
        $ifc->set(CMS_L_IFC_DIRECTORY_065 . " (GIF, JPG, PNG, SVG, WEBP)", "file 50 b");

        //info
        $ifc->set(CMS_L_IFC_DIRECTORY_066,                                 "info");
        echo("</td>" .
             "</tr>");
        ifc_table_close();
    };
    echo("</tr>" .
         "</table>");

    $ifc->close();

case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "save":
//..................................................................................................

    $directory = new directory();

    //process canonical
    $language = nstre(CMS_LANGUAGE_ENABLED) ? explode(",", "," . CMS_LANGUAGE_ENABLED) : [""];
    $source   = analyze_url(CMS_ROOT_URL);

    foreach ($language AS $value)
    {
        $_value = language_get($ifc_param12, $value, TRUE);
        if (stre($_value)) continue; //no value

        //complete imperfect address
        $target = analyze_url($_value);
        if (stre($target["scheme"])) $target["scheme"] = $source["scheme"];
        if (stre($target["host"]))   $target["host"]   = $source["host"];
        $_value = cms_build_url($target);

        $ifc_param12 = language_set($ifc_param12, $_value, $value);
    };

    //set values
    $directory->set(
        $object,            //key
        $ifc_param1,        //name
        $ifc_param5,        //description
        $ifc_param6,        //url
        $ifc_param7,        //subtype
        isset($ifc_param2), //hidden
        isset($ifc_param3), //placeholder
        isset($ifc_param4), //dynamic
        $ifc_param8,        //image_button
        $ifc_param9,        //image_hover
        $ifc_param10,       //image_active
        $ifc_param11,       //path
        $ifc_param12);      //canonical

    if ($directory->save())
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "add":
//..................................................................................................

    $ifc_param  = $object;                                                           //target
    $ifc_param1 = CMS_L_IFC_DIRECTORY_001;                                           //name
    $ifc_param2 = cms_cache("directory." . CMS_USER . ".hidden")      ? TRUE : NULL; //hidden
    $ifc_param3 = cms_cache("directory." . CMS_USER . ".placeholder") ? TRUE : NULL; //placeholder
    $ifc_param4 = cms_cache("directory." . CMS_USER . ".dynamic")     ? TRUE : NULL; //dynamic
    $ifc_param5 = NULL;                                                              //description
    $ifc_param6 = NULL;                                                              //url
    $ifc_param7 = cms_cache("directory." . CMS_USER . ".subtype");                   //subtype

case "add_target":
//..................................................................................................

    $data     = new data("#system/directory");
    $flexview = new flexview();

    $flexview->import_data($data);

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_CANCEL . "|directory/command_cancel" => FALSE],
                   ["object" => $object],
                   NULL,
                   CMS_L_COMMAND_ADD);

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .

    //data
         "<th>" .
         CMS_L_IFC_DIRECTORY_003 .
         "</th>" .

    //target
         "<th>" .
         CMS_L_IFC_DIRECTORY_004 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //data
         "<td>");

    //name
    $ifc->set(CMS_L_IFC_DIRECTORY_008,     "text 50 256 bl",             $ifc_param1);

    //hidden
    $ifc->set(CMS_L_IFC_DIRECTORY_005,     "checkbox",   TRUE,           isset($ifc_param2));

    //placeholder
    $ifc->set(CMS_L_IFC_DIRECTORY_010,     "checkbox",   TRUE,           isset($ifc_param3));

    //dynamic
    $ifc->set(CMS_L_IFC_DIRECTORY_016,     "checkbox b", TRUE,           isset($ifc_param4));

    //description
    $ifc->set(CMS_L_DESCRIPTION,           "texteditor 50x8 2048 bilrt", $ifc_param5);

    //url
    $buffer = "javascript:load_page('" .
              str_replace("%replace%",
                          "'+encodeURIComponent(ifc_get('ifc_param6'))+'",
                          q(cms_url(["ifc_page"          => "content",
                                     "ifc_message"       => "edit_href",
                                     "ifc_option"        => "external",
                                     "value"             => "\x1B%replace%",
                                     "ifc_select"        => "__value",
                                     "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set(" .
                                                            "'ifc_param6','%return%');"]))) . "');";

    $ifc->set(CMS_L_URL,                   "text 40",                    $ifc_param6);
    $ifc->set("…",                         "button b",                   $buffer);

    //subtype
    $ifc->set(CMS_L_IFC_DIRECTORY_063,     "label");
    $ifc->set(directory_get_type_select(), "select 50",                  $ifc_param7);
    echo("</td>" .

    //target
         "<td class=\"target\">");
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
    $flexview->show_target($ifc_param,
                           "javascript:ifc_post('add_target','%index%');",
                           "javascript:ifc_post('add_insert','%index%');",
                           "javascript:ifc_post('add_append','%index%');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "add_insert":
case "add_append":
//..................................................................................................

    //set name if none is given
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);

    $directory = new directory();
    $_object   = streq(CMS_IFC_MESSAGE, "add_insert") ?
                 $directory->insert(
                     $ifc_param,           //key
                     $ifc_param1,          //name
                     $ifc_param5,          //description
                     $ifc_param6,          //url
                     $ifc_param7,          //subtype
                     isset($ifc_param2),   //hidden
                     isset($ifc_param3),   //placeholder
                     isset($ifc_param4)) : //dynamic
                 $directory->append(
                     $ifc_param,           //key
                     $ifc_param1,          //name
                     $ifc_param5,          //description
                     $ifc_param6,          //url
                     $ifc_param7,          //subtype
                     isset($ifc_param2),   //hidden
                     isset($ifc_param3),   //placeholder
                     isset($ifc_param4));  //dynamic

    if ($directory->save())
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        cms_cache("directory." . CMS_USER . ".hidden",      isset($ifc_param2), TRUE);
        cms_cache("directory." . CMS_USER . ".placeholder", isset($ifc_param3), TRUE);
        cms_cache("directory." . CMS_USER . ".dynamic",     isset($ifc_param4), TRUE);
        cms_cache("directory." . CMS_USER . ".subtype",     $ifc_param7,        TRUE);
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy_insert":
case "copy_append":
case "cut_insert":
case "cut_append":
//..................................................................................................

    $directory = new directory();
    $data      = clone $directory->data; //use clone of data structure as source
    $ifc_param = explode(",", $ifc_param);
    $value     = $ifc_param[0];
    $ifc_param = $ifc_param[1];

    //copy / cut
    switch (CMS_IFC_MESSAGE)
    {
    case "copy_insert":
    case "copy_append":

        //buffer section in clone
        $data->copy($value);
        break;

    case "cut_insert":
    case "cut_append":

        //target is contained in source
        if ($directory->data->is_child($ifc_param, $value))
        {
            //buffer section in clone
            $data->copy($value);
        }
        else
        {
            //remove section in master
            $directory->data->del($value);

            //buffer section in clone
            $data->cut($value);
        };
        break;
    };

    //transmit buffer from clone to master
    $directory->data->buffer = $data->buffer;

    //paste
    switch (CMS_IFC_MESSAGE)
    {
    case "copy_insert":
    case "cut_insert":

        //insert buffer in master
        $_object = $directory->data->insert($ifc_param);
        break;

    case "copy_append":
    case "cut_append":

        //append buffer in master
        $_object = $directory->data->append($ifc_param);
        break;
    };

    //save changes
    if ($directory->save())
    {
        //set target as selection
        switch (CMS_IFC_MESSAGE)
        {
        case "copy_insert":
        case "cut_insert":

            $object = $ifc_param;
            break;

        case "copy_append":
        case "cut_append":

            $object = $directory->data->move("parent", $ifc_param);
            break;
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "sort":
//..................................................................................................

    //sort directory tree branches below selected entry by name

    $directory = new directory();

    //order by name
    data_sort($directory->data, "name", $object);

    $ifc_response = $directory->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "clean":
//..................................................................................................

    //cut directory tree branch endings below selected entry
    //that are no placeholder and have no url

    $directory = new directory();
    $stack     = [];
    $_object   = $object;

    //get path to object
    while ($_object = $directory->data->move("parent", $_object))
        array_unshift($stack, $_object);
    array_unshift($stack, 0);

    $array = [];
    $depth = 0;
    $limit = 0;

    $key = $object ?
           $directory->data->move("to", $object) :
           $directory->data->move("first");
    while ($key)
    {
        switch ($directory->data->get($key, "#type"))
        {
        case "container":

            $array[++$depth] = $key;
            if (
                 $directory->data->get($key, "url")
                 ||
                 $directory->data->get($key, "placeholder")
               )
                $limit = $depth;
            break;

        case "/container":

            if ($depth > $limit)                $directory->data->del($array[$depth]);
            if (streq($array[$depth], $object)) break 2;
            if (--$depth < $limit)              $limit = $depth;
        };

        $key = $directory->data->move("next");
    };

    if ($directory->save())
    {
        //select rightmost existing object in path
        while (! $directory->data->get($object))
        {
            $object = array_pop($stack);
            if ($object === 0) break;
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    $directory = new directory();
    $stack     = [];
    $_object   = $object;

    //get path to object
    while ($_object = $directory->data->move("parent", $_object))
        array_unshift($stack, $_object);
    array_unshift($stack, 0);

    //delete
    $directory->del($ifc_param);

    //save changes
    if ($directory->save())
    {
        //select last existing object in path
        while (! $directory->data->get($object))
        {
            $object = array_pop($stack);
            if ($object === 0) break;
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//cache selected object permanently
if (CMS_IFC_MESSAGE !== "") cms_cache("directory." . CMS_USER . ".object", $object, TRUE);

$data     = new data("#system/directory");
$flexview = new flexview();
$flexview->import_data($data);

//menu
$menu = NULL;
if (CMS_IFC_SELECT && $object)
    $menu[CMS_L_COMMAND_INSERT . "|image/command_apply"]    = "javascript:ifc_return(null,ifc_get('l_ifc_param1'));";
$menu[CMS_L_COMMAND_ADD .       "|directory/command_add"]   = "add";
if ($object && cms_load("template"))
{
    $url = translate_url(
        "directory://$object",
        ["content_option" => CMS_TEMPLATE_OPTION_ALL,
         "cms_instance"   => NULL],
        CMS_LANGUAGE,
        TRUE);

    $menu[CMS_L_COMMAND_SHOW . "|directory/command_view"]   = "javascript:load_page('" . q($url) . "');";
};
$menu[CMS_L_IFC_DIRECTORY_072 . "|directory/command_sort"]  = "#sort";
$menu[CMS_L_IFC_DIRECTORY_002 . "|directory/command_clear"] = "#clean";
$menu[CMS_L_IFC_DIRECTORY_062 . "|directory/command_icon"]  = "type";

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object]);

echo("<table class=\"layout\" " .
            "style=\"WIDTH:100%\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_DIRECTORY .
     "</th>" .
     "</tr>" .

//directory
     "<tr>" .
     "<td>");

?>
<script>

function directory_flexview_event(event,
                                  source,
                                  target)
{
    var source_id = "";
    var target_id = "";
    var action    = "";

    if (source !== null)
        source_id = source.getAttribute("data-fv-hir-index");

    if (target !== null)
    {
        target_id = target.getAttribute("data-fv-hir-index");
        action    = (target.className === "fv-hir-source") ? "insert" : "append";
    };

    switch (event)
    {
    case "dropon":

        if (target.id === "trashbin") { ifc_post("delete", source_id); break; };
        ifc_post("cut_" + action, source_id + "," + target_id);
        break;

    case "dropon_alt":

        ifc_post("copy_" + action, source_id + "," + target_id);
        break;
    };
};

</script>
<?php

$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
$flexview->show_hierarchy(
    $object,                                    //index
    "javascript:ifc_post('select','%index%');", //action
    "name",                                     //name key
    NULL,                                       //mark
    directory_get_type(),                       //icon
    "",                                         //base
    "directory_flexview_event");                //dragdrop_event_function
echo("</td>" .
     "</tr>" .

//trash bin
     "<tr>" .
     "<td id=\"trashbin\" " .
         "title=\"" . CMS_L_IFC_DIRECTORY_006 . "\" " .
         "class=\"varied\">" .
     image("directory/command_delete") . " " . CMS_L_COMMAND_DELETE .
     jscript("dd_register(document.getElementById(\"trashbin\"), 0, 1, true);") .
     "</td>" .
     "</tr>");

//canonical address
$url = directory_get_canonical($object);
echo("<tr>" .
     "<td class=\"highlight\">" .
     "<a href=\"" . x($url) . "\" " .
        "title=\"" . CMS_L_IFC_DIRECTORY_022 . "\" " .
        "onclick=\"load_page('" . qx(cms_url($url, TRUE)) . "');return false;\">" .
     x($url) .
     "</a>" .
     "</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//data
if ($object)
{
    //data
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>" .
         CMS_L_IFC_DIRECTORY_007 .
         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td>");

    //name
    $ifc->set(CMS_L_IFC_DIRECTORY_008,     "text 40 256 l",              $data->get($object, "name"));

    //save button
    $ifc->set(CMS_L_COMMAND_SAVE,          "button b",                   "save");

    //hidden
    $ifc->set(CMS_L_IFC_DIRECTORY_005,     "checkbox",   TRUE,           $data->get($object, "hidden"));

    //placeholder
    $ifc->set(CMS_L_IFC_DIRECTORY_010,     "checkbox",   TRUE,           $data->get($object, "placeholder"));

    //dynamic
    $ifc->set(CMS_L_IFC_DIRECTORY_016,     "checkbox b", TRUE,           $data->get($object, "dynamic"));

    //description
    $ifc->set(CMS_L_DESCRIPTION,           "texteditor 50x8 2048 bilrt", $data->get($object, "description"));

    //url
    $buffer = "javascript:load_page('" .
              str_replace("%replace%",
                          "'+encodeURIComponent(ifc_get('ifc_param6'))+'",
                          q(cms_url(["ifc_page"          => "content",
                                     "ifc_message"       => "edit_href",
                                     "ifc_option"        => "external",
                                     "value"             => "\x1B%replace%",
                                     "ifc_select"        => "__value",
                                     "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set(" .
                                                            "'ifc_param6','%return%');"]))) . "');";

    $ifc->set(CMS_L_URL,                   "text 40",                    $data->get($object, "url"));
    $ifc->set("…",                         "button b",                   $buffer);

    //graphics
    $ifc->set(CMS_L_IFC_DIRECTORY_011,     "title");

    //subtype
    $ifc->set(CMS_L_IFC_DIRECTORY_063,     "label");
    $ifc->set(directory_get_type_select(), "select 50 b",                $data->get($object, "#subtype"));

    //image button
    $buffer = str_replace(
        "%replace%",
        "'+encodeURIComponent(ifc_get('ifc_param8'))+'",
        "javascript:load_page('" .
        q(cms_url(["ifc_page"          => "image",
                   "ifc_option"        => "external",
                   "object"            => "\x1B%replace%",
                   "ifc_select"        => "object",
                   "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param8','%return%');"])) .
        "');");

    $ifc->set(CMS_L_IFC_DIRECTORY_012,     "text 40 l",                  $data->get($object, "image_button"));
    $ifc->set("…",                         "button b",                   $buffer);

    //image hover
    $buffer = str_replace(
        "%replace%",
        "'+encodeURIComponent(ifc_get('ifc_param9'))+'",
        "javascript:load_page('" .
        q(cms_url(["ifc_page"          => "image",
                   "ifc_option"        => "external",
                   "object"            => "\x1B%replace%",
                   "ifc_select"        => "object",
                   "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param9','%return%');"])) .
        "');");

    $ifc->set(CMS_L_IFC_DIRECTORY_013,     "text 40 l",                  $data->get($object, "image_hover"));
    $ifc->set("…",                         "button b",                   $buffer);

    //image active
    $buffer = str_replace(
        "%replace%",
        "'+encodeURIComponent(ifc_get('ifc_param10'))+'",
        "javascript:load_page('" .
        q(cms_url(["ifc_page"          => "image",
                   "ifc_option"        => "external",
                   "object"            => "\x1B%replace%",
                   "ifc_select"        => "object",
                   "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param10','%return%');"])) .
        "');");

    $ifc->set(CMS_L_IFC_DIRECTORY_009,     "text 40 l",                  $data->get($object, "image_active"));
    $ifc->set("…",                         "button b",                   $buffer);

    //extended
    $ifc->set(CMS_L_IFC_DIRECTORY_014,     "title");

    //overwrite path
    $ifc->set(CMS_L_IFC_DIRECTORY_015,     "text 40 256 l",              $data->get($object, "path"));
    echo(".php<br>");

    //canonical base
    $canonical = $data->get($object, "canonical");
    $ifc->set(CMS_L_IFC_DIRECTORY_021,     "text 40 256 bl",             $canonical);

    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};
echo("</tr>" .
     "</table>");

$ifc->close();

?>