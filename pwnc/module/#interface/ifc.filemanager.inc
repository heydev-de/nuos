<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//load libraries
if (! cms_load("filemanager")) ifc_inactive($ifc_page);
if (! cms_load("flexview"))    ifc_inactive($ifc_page);
if (! cms_load("plist"))       ifc_inactive($ifc_page);

//permission
ifc_permission(["" => CMS_L_ACCESS]);

//retrieve selected object from permanent cache
cms_cache_init($object, "filemanager." . CMS_USER . ".object", CMS_ROOT_PATH, TRUE);

//sanitize selected object
while (! file_exists($object)) $object = dirname($object) . "/";
if (nstreq(substr($object, 0, strlen(CMS_ROOT_PATH)), CMS_ROOT_PATH)) $object = CMS_ROOT_PATH;

//initiate selection
if (blank($list)) $list = preg_match("/^(?:$|ifc_)/", CMS_IFC_MESSAGE ?? "") ? [$object] : [];

//initiate copy/move settings
init($status);
init($target);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = stre($ifc_param) ? CMS_ROOT_PATH : $ifc_param;
    $list   = [$object];
    $status = NULL;
    break;

case "save":
//..................................................................................................

    $ifc_param1 = dirname($object) . "/" . basename($ifc_param1);
    if (is_dir($object)) $ifc_param1 .= "/";

    $access = (isset($ifc_param2) ? 256 : 0) + (isset($ifc_param5) ? 32 : 0) + (isset($ifc_param8)  ? 4 : 0) +
              (isset($ifc_param3) ? 128 : 0) + (isset($ifc_param6) ? 16 : 0) + (isset($ifc_param9)  ? 2 : 0) +
              (isset($ifc_param4) ?  64 : 0) + (isset($ifc_param7) ?  8 : 0) + (isset($ifc_param10) ? 1 : 0);
    chmod($object, $access);

    if (rename($object, $ifc_param1))
    {
        $object       = $ifc_param1;
        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "upload":
//..................................................................................................

    $array = is_array($ifc_file1) ?
             array_combine($ifc_file1, $ifc_file1_name) :
             [$ifc_file1 => $ifc_file1_name];

    $path  = is_dir($object) ? $object : dirname($object) . "/";
    $list  = [];
    $error = FALSE;

    foreach ($array AS $key => $value)
    {
        $value = $path . $value;

        //place upload
        if (move_uploaded_file($key, $value)) $list[] = $value;
        else                                  $error  = TRUE;
    };

    $object       = reset($list);
    $status       = NULL;
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "mkdir":
//..................................................................................................

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object,
         "list"   => $list],
        "_mkdir",
        CMS_L_IFC_FILEMANAGER_006);

    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME, "text 40");

    $ifc->close();

case "_mkdir":
//..................................................................................................

    $ifc_param1 = basename($ifc_param1) . "/";
    $_object    = (is_dir($object) ? $object : dirname($object) . "/") . $ifc_param1;

    if (mkpath($_object))
    {
        $object       = $_object;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "mkfile":
//..................................................................................................

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object,
         "list"   => $list],
        "_mkfile",
        CMS_L_IFC_FILEMANAGER_007);

    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME,                "text 40 b");
    $ifc->set(CMS_L_IFC_FILEMANAGER_003, "textarea 40x20 nw");

    $ifc->close();

case "_mkfile":
//..................................................................................................

    $ifc_param1 = basename($ifc_param1);
    $_object    = (is_dir($object) ? $object : dirname($object) . "/") . $ifc_param1;

    if (
         file_exists($_object)
         ||
         (! write_file($_object, $ifc_param2))
       )
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    $object       = $_object;
    $list         = [$object];
    $ifc_response = CMS_MSG_DONE;
    break;

case "_edit":
//..................................................................................................

    echo(write_file($object, $ifc_param1) ?
         CMS_MSG_DONE : CMS_MSG_ERROR);
    exit();

case "edit":
//..................................................................................................

    if (! is_file($object)) ifc_close_external();

    if (streq(CMS_IFC_MESSAGE, "edit"))
    {
        $top = $left = 0;

        //memorize access
        $plist = new plist("#system/filemanager.edited");
        $plist->add(substr($object, strlen(CMS_ROOT_PATH)), TRUE);
        $plist->remove(15, 0);
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        [CMS_L_COMMAND_SAVE . "|filemanager/command_save"
         => "javascript:asr_form_post(document.forms['ifc'],ifc_response);"],
        ["object" => $object],
        "_edit",
        CMS_L_COMMAND_EDIT);

    switch (get_mime_type($object))
    {
    case "text/html":
    case "image/svg+xml":

        $type = "code_html";
        break;

    case "application/x-php":
    case "application/x-pwnc-include":

        $type = "code_php";
        break;

    case "text/css":

        $type = "code_style";
        break;

    case "application/javascript":

        $type = "code_script";
        break;

    default:

        $type = "textarea";
        break;
    };

    $ifc->set($object, "$type 40x20 fn", read_file($object));
    $ifc->close();

case "compress":
//..................................................................................................

    if (blank($list)) break; //no selection

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object,
         "list"   => $list],
        "_compress",
        CMS_L_IFC_FILEMANAGER_036);

    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME, "text 40", file_name($object));
    echo(".zip");

    $ifc->close();

case "_compress":
//..................................................................................................

    $ifc_param1 = basename($ifc_param1) . ".zip";
    $base       = is_dir($object) ? $object : dirname($object) . "/";

    if (($_object = filemanager_zip($list, $base . $ifc_param1, $base)) !== FALSE)
    {
        $object       = $_object;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "decompress":
//..................................................................................................

    if (($_object = filemanager_unzip($object)) !== FALSE)
    {
        $object       = $_object;
        $list         = [];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy":
case "cut":
//..................................................................................................

    $target = is_dir($object) ? $object : dirname($object) . "/";
    $status = CMS_IFC_MESSAGE;
    break;

case "target":
//..................................................................................................

    $target = $ifc_param;
    break;

case "copy_insert":
case "cut_insert":
//..................................................................................................

    if (blank($list)) break; //no selection

    if (stre($ifc_param)) $ifc_param = CMS_ROOT_PATH; //target is root
    $base  = is_dir($object) ? $object : dirname($object) . "/";
    $_list = streq(CMS_IFC_MESSAGE, "copy_insert") ?
             filemanager_copy($list, $ifc_param, $base) :
             filemanager_move($list, $ifc_param, $base);

    if ($_list === FALSE) //complete fail
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    $object = $ifc_param;
    $status = NULL;
    $count  = count($list);
    $length = strlen($object);

    //filter file objects not directly in target path
    $list = array_filter($_list, function($path) use ($length)
    {
        return strpos(substr(rtrim($path, "/"), $length), "/") === FALSE;
    });

    $ifc_response = ($count === count($list)) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "download":
//..................................................................................................

    download($object);
    exit();

case "delete":
//..................................................................................................

    if (blank($list)) break; //no selection

    $_list = filemanager_delete($list);
    if ($_list === FALSE) //complete fail
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    $list   = array_diff($list, $_list);
    $status = NULL;

    $ifc_response = $list ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "transfer":
//..................................................................................................

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object,
         "list"   => $list],
        "_transfer",
        CMS_L_IFC_FILEMANAGER_033);

    $ifc->set(CMS_L_URL,  "text 50 b", "https://");
    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME, "text 50");

    $ifc->close();

case "_transfer":
//..................................................................................................

    $ifc_param1 = utf8_trim($ifc_param1);
    $ifc_param2 = utf8_trim(basename($ifc_param2));
    if (stre($ifc_param2)) $ifc_param2 = basename($ifc_param1);

    $_object = (is_dir($object) ? $object : dirname($object) . "/") . $ifc_param2;

    if (retrieve_file($ifc_param1, $_object))
    {
        $object       = $_object;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

if (nstre(CMS_IFC_MESSAGE))
{
    //re-verify and re-correct selected object
    if (nstreq(substr($object, 0, strlen(CMS_ROOT_PATH)), CMS_ROOT_PATH)) $object = CMS_ROOT_PATH;
    while (! file_exists($object))                                        $object = dirname($object) . "/";

    //cache selected object permanently
    cms_cache("filemanager." . CMS_USER . ".object", $object, TRUE);
};

//sync file order settings with permanent cache
cms_cache_sync($order, "filemanager." . CMS_USER . ".order", "type");
cms_cache_sync($desc,  "filemanager." . CMS_USER . ".desc",  FALSE);

//generate url of selected file object
$url = substr($object, strlen(CMS_ROOT_PATH));
//                               -./ 0 - 9  A - Z      _    a - z
$url = preg_replace_callback("/[^\\x2D-\\x39\\x41-\\x5A\\x5F\\x61-\\x7A]/",
                             function($value) { return "%" . bin2hex($value[0]); },
                             $url);
$url = CMS_ROOT_URL . $url;

//init
$mime_list = get_mime_list();
$filename  = basename($object);

//retrieve files in selected path
//..................................................................................................

$file_list = [];
$path      = is_dir($object) ? $object : dirname($object) . "/";

if (($hdir = opendir($path)) !== FALSE)
{
    while (($file = readdir($hdir)) !== FALSE)
    {
        if (($file === ".") || ($file === "..")) continue;

        $_file = $path . $file;
        $stat  = stat($_file);
        if ($stat === FALSE) continue;

        $is_dir = is_dir($_file);
        if ($is_dir) $_file .= "/";

        $name      = file_name($_file);
        $extension = $is_dir ? "/" : file_extension($_file);
        if (stre($name)) $name = $file;

        $file_list[] =
            ["path" => $_file,
             "name" => $name,
             "type" => $extension,
             "size" => $is_dir ? 0 : $stat["size"],
             "time" => $stat["mtime"]];
    };

    closedir($hdir);
};

//sort files
//..................................................................................................

usort($file_list, function($a, $b) use ($order, $desc)
{
    $a_dir = $a["type"] === "/";
    $b_dir = $b["type"] === "/";

    if ($a_dir !== $b_dir) return $a_dir ? -1 : 1;

    switch ($order)
    {
    case "name":
    case "type":

        $value = strnatcasecmp($a[$order], $b[$order]);
        break;

    case "size":
    case "time":

        $value = $a[$order] <=> $b[$order];
        break;

    default:

        $value = 0;
        break;
    };

    if ($value === 0) $value = strnatcmp($a["name"], $b["name"]);

    return $desc ? -$value : $value;
});

//menu
//..................................................................................................

$menu = [CMS_L_IFC_FILEMANAGER_006 . "|filemanager/command_create_directory" => "mkdir",
         CMS_L_IFC_FILEMANAGER_007 . "|filemanager/command_create_file"      => "mkfile",
         CMS_L_COMMAND_SHOW .        "|filemanager/command_show"
         => "javascript:load_page('" . q(cms_url($url, NULL, TRUE)) . "');"];
if (is_file($object))
{
    if (
         streq(strrchr($filename, "."), ".dat")
         &&
         ifc_available("system")
         &&
         cms_permission("system", TRUE, NULL)
       )
        $menu[CMS_L_COMMAND_EDIT . "|filemanager/command_edit"] =
            "javascript:load_page('" .
            q(cms_url(["ifc_page"    => "system",
                       "ifc_message" => "filemanager",
                       "ifc_option"  => "external",
                       "file"        => $object])) . "');";
    else
        $menu[CMS_L_COMMAND_EDIT . "|filemanager/command_edit"] =
            "javascript:load_page('" .
            q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                       "ifc_message" => "edit",
                       "ifc_option"  => "external",
                       "object"      => $object])) . "');";
    $menu[CMS_L_IFC_FILEMANAGER_029 . "|filemanager/command_download"] =
        "javascript:ifc_download('" .
        q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                   "ifc_message" => "download",
                   "object"      => $object])) . "');";
};
$menu[CMS_L_COMMAND_COPY .            "|filemanager/command_copy"]       = "copy";
$menu[CMS_L_COMMAND_CUT .             "|filemanager/command_cut"]        = "cut";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|filemanager/command_delete"]     = "#delete";
$menu[CMS_L_IFC_FILEMANAGER_036 .     "|filemanager/command_compress"]   = "compress";
if (preg_match("/\.zip$/i", $object))
    $menu[CMS_L_IFC_FILEMANAGER_037 . "|filemanager/command_decompress"] = "#decompress";
$menu[CMS_L_IFC_FILEMANAGER_033 .     "|filemanager/command_transfer"]   = "transfer";
if (
     cms_available("directory")
     &&
     cms_permission("directory", TRUE, NULL)
   )
    $menu[CMS_L_IFC_FILEMANAGER_008 . "|filemanager/command_directory"] =
        "javascript:load_page('" .
        q(cms_url(["ifc_page"    => "directory",
                   "ifc_message" => "add_target",
                   "ifc_option"  => "external",
                   "ifc_param1"  => $filename,
                   "ifc_param5"  => "\x1B",
                   "ifc_param6"  => $url,
                   "ifc_param7"  => "\x1B"])) . "');";

//display
//..................................................................................................

$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object,
     "status" => $status,
     "order"  => $order,
     "desc"   => $desc]);

//folder selection
//..................................................................................................

ifc_popover_open(CMS_L_IFC_FILEMANAGER_043);

$flexview = filemanager_flexview_directory(NULL, $object);
$flexview->icon_default["entry"]      = "filemanager/icon_folder";
$flexview->icon_default["container"]  = "filemanager/icon_folder";
$flexview->icon_default["+container"] = "filemanager/icon_folderopen";
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->show_hierarchy($object, "javascript:ifc_post('select','%index%');");

ifc_popover_close();

//shortcuts
//..................................................................................................

ifc_popover_open(CMS_L_IFC_FILEMANAGER_041);

$array = [CMS_L_IFC_FILEMANAGER_024 => "design",
          CMS_L_IFC_FILEMANAGER_025 => "data",
          CMS_L_IFC_FILEMANAGER_026 => "data/#database",
          CMS_L_IFC_FILEMANAGER_028 => "data/#log",
          CMS_L_IFC_FILEMANAGER_031 => "#include",
          CMS_L_IFC_FILEMANAGER_032 => "3rdparty",
          CMS_L_IFC_FILEMANAGER_034 => "pwnc/#language",
          CMS_L_IFC_FILEMANAGER_035 => "#update/backup",
          CMS_SOFTWARE              => "pwnc"];

foreach ($array AS $key => $value)
    echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH . "$value/") . "');\">" .
         image("filemanager/icon_folder") . " $key" .
         "</a><br>");

ifc_popover_close();

//recently edited
//..................................................................................................

$plist = new plist("#system/filemanager.edited");
$array = $plist->get(0, 15);

if (count($array))
{
    ifc_popover_open(CMS_L_IFC_FILEMANAGER_042);

    foreach ($array AS $value)
    {
        $extension = file_extension($value);
        $image     = $mime_list[$extension] ?? "filemanager/icon_file";
        $dirname   = streq(($dirname = basename(dirname($value))), ".") ? "" : "$dirname/";
        $basename  = basename($value);

        echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH . $value) . "');\" " .
                "title=\"" . x("/$value") . "\">" .
             image($image) . " " . x($dirname . $basename) .
             "</a><br>");
    };

    ifc_popover_close();
};

//recently selected
//..................................................................................................

//memorize access
$plist = new plist("#system/filemanager.recent");
$plist->add(substr($object, strlen(CMS_ROOT_PATH)), TRUE);
$plist->remove(15, 0);

$array = $plist->get(0, 15);

if (count($array))
{
    ifc_popover_open(CMS_L_IFC_FILEMANAGER_044);

    foreach ($array AS $value)
    {
        $suffix    = (substr($value, -1) === "/") ? "/" : "";
        $extension = file_extension($value);
        $image     = $mime_list[$extension] ?? "filemanager/icon_file";
        $dirname   = streq(($dirname = basename(dirname($value))), ".") ? "" : "$dirname/";
        $basename  = basename($value) . $suffix;

        echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH . $value) . "');\" " .
                "title=\"" . x("/$value") . "\">" .
             image($image) . " " . x($dirname . $basename) .
             "</a><br>");
    };

    ifc_popover_close();
};

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     "<col style=\"WIDTH:" . (isset($status) ? "50%" : "0") . "\">" .
     "</colgroup>" .

     "<tr>" .

//directory
//..................................................................................................

     "<td>");

?><script>

function s(index)
{
    ifc_post("select", ifc_object("list[]", index).value);
};

function o(value)
{
    ifc_set("desc",  ((value === ifc_get("order")) && (ifc_get("desc") === "")) ? 1 : "");
    ifc_set("order", value);
    ifc_post();
};

</script><?php

ifc_table_open("filemanager-table");

echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"4\">" .
     "</colgroup>");

//path up
//..................................................................................................

$array  = [];
$_path  = $path;
$length = strlen(CMS_ROOT_PATH);
$flag   = strlen($_path) <= $length;

echo("<tr>" .

//dummy
     "<td class=\"select\">" .
     "</td>" .

//path
     "<td colspan=\"" . ($flag ? 4 : 3) . "\" " .
         "class=\"highlight\">");

while (strlen($_path) > $length)
{
    $array[] = "<a href=\"javascript:ifc_post('select','" . qx($_path) . "');\">" .
               x(basename($_path)) . "/" .
               "</a>";
    $_path   = dirname($_path) . "/";
};

echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH) . "');\">" .
     image("filemanager/icon_folderopen") . " " . x(CMS_ROOT_PATH) .
     "</a>" .
     implode("", array_reverse($array)) .
     "</td>" .

//close button
     ($flag ? "" :
     "<td style=\"TEXT-ALIGN:center\">" .
     "<a href=\"javascript:ifc_post('select','" . qx(dirname($path)) . "/');\">" .
     image("filemanager/icon_up") . " " . CMS_L_COMMAND_UP .
     "</a>" .
     "</td>") .
     "</tr>" .

     "<tr>");

//table header
//..................................................................................................

$f = function($value) use ($order, $desc)
{
    if (nstreq($value, $order)) return "";
    return $desc ? " ↘" : " ↗";
};

//dummy
echo("<td class=\"select\">" .
     "</td>" .

//name
     "<th>");
$ifc->set(CMS_L_IFC_FILEMANAGER_004 . $f("name"), "button", "javascript:o('name');");
echo("</th>" .

//type
     "<th>");
$ifc->set(CMS_L_IFC_FILEMANAGER_018 . $f("type"), "button", "javascript:o('type');");
echo("</th>" .

//size
     "<th>");
$ifc->set(CMS_L_IFC_FILEMANAGER_019 . $f("size"), "button", "javascript:o('size');");
echo("</th>" .

//time
     "<th>");
$ifc->set(CMS_L_IFC_FILEMANAGER_020 . $f("time"), "button", "javascript:o('time');");
echo("</th>" .
     "</tr>");

//file list
//..................................................................................................

$i = 0;

foreach ($file_list AS $value)
{
    echo("<tr" . ifc_varied() . ">" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $value["path"], in_array($value["path"], $list), "list[]");
    echo("</td>");

    //name
    $is_dir = $value["type"] === "/";
    $image  = $is_dir ? "filemanager/icon_folder" :
              $mime_list[$value["type"]] ?? "filemanager/icon_file";
    $name   = streq($value["path"], $object) ?
              "<strong>" . x($value["name"]) . "</strong>" :
              x($value["name"]);
    echo("<td>" .
         "<a href=\"javascript:s(" . $i++ . ");\">" .
         image($image) . " $name" .
         "</a>" .
         "</td>" .

    //mime type
         "<td>" .
         x($value["type"]) .
         "</td>" .

    //size
         "<td>" .
         ($is_dir ? "" : format_bytesize($value["size"])) .
         "</td>" .

    //time
         "<td>" .
         x(friendly_date($value["time"])) .
         "</td>" .
         "</tr>");
};

//selection control
//..................................................................................................

echo("<tr>" .
     "<td class=\"select\" " .
         "colspan=\"5\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//copy/move target
//..................................................................................................

if (isset($status))
{
    //target
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>" .
         (streq($status, "copy") ? CMS_L_IFC_FILEMANAGER_005 : CMS_L_IFC_FILEMANAGER_030) .
         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td class=\"target\">");
    $flexview = filemanager_flexview_directory(NULL, $target);
    $flexview->icon_default["entry"]      = "filemanager/icon_folder";
    $flexview->icon_default["container"]  = "filemanager/icon_folder";
    $flexview->icon_default["+container"] = "filemanager/icon_folderopen";
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->show_target(
        $target,
        "javascript:ifc_post('target','%index%');",
        "javascript:ifc_post('" . qr($status) . "_insert','%index%');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>" .
         "</tr>" .
         "</table>");

    $ifc->close();
};

//object data
//..................................................................................................

echo("<td>");

ifc_table_open();
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_FILEMANAGER_009 .
     "</th>" .
     "</tr>" .
     "<tr>" .
     "<td>");
if (nstreq($object, CMS_ROOT_PATH))
{
    $ifc->set(CMS_L_IFC_FILEMANAGER_004, "text 30",  $filename);
    $ifc->set(CMS_L_COMMAND_SAVE,        "button b", "save");
};

//information
$filesize         = sprintf("%u", filesize($object));
$transtime        = round($filesize / 1250000);
$disk_total_space = disk_total_space(CMS_ROOT_PATH);
$disk_free_space  = disk_free_space(CMS_ROOT_PATH);
$percentage_used  = 100 / $disk_total_space * $disk_free_space;

ifc_tab_open(CMS_L_IFC_FILEMANAGER_040);
ifc_table_open("nomargin");

//type
echo("<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_018 . ":" .
     "</td>" .
     "<td>" .
     filetype($object) .
     "</td>" .
     "</tr>" .

//mime type
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_039 . ":" .
     "</td>" .
     "<td>" .
     x(get_mime_type($object)) .
     "</td>" .
     "</tr>" .

//size
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_019 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($filesize) .
     "</td>" .
     "</tr>" .

//transmission time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_023 . ":" .
     "</td>" .
     "<td>" .
     gmdate(CMS_L_TIME_FORMAT, $transtime) .
     "</td>" .
     "</tr>" .

//modified time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_020 . ":" .
     "</td>" .
     "<td>" .
     x(friendly_date(filemtime($object))) .
     "</td>" .
     "</tr>" .

//accessed time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_021 . ":" .
     "</td>" .
     "<td>" .
     x(friendly_date(fileatime($object))) .
     "</td>" .
     "</tr>" .

//total space
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_038 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($disk_total_space) .
     "</td>" .
     "</tr>" .

//free space
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_022 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($disk_free_space) .
     " (" . number_format($percentage_used, 1, CMS_L_DECIMAL_SEPARATOR, NULL) . "%)" .
     "</td>" .
     "</tr>");
ifc_table_close();

ifc_tab_next(CMS_L_IFC_FILEMANAGER_010);

//access permission
$access = fileperms($object);

echo("<table class=\"layout\">" .
     "<tr>" .

//owner
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_011 . " (". x(fileowner($object)) . ")" .
     "</span>" .
     "</td>" .

//group
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_012 . " (" . x(filegroup($object)) . ")" .
     "</span>" .
     "</td>" .

//all
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_013 .
     "</span>" .
     "</td>" .
     "</tr>" .
     "<tr>" .

//owner
     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 0400); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015 . " *", "checkbox w", TRUE, $access & 0200); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 0100); //execute
echo("</td>" .

     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 040); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015,        "checkbox w", TRUE, $access & 020); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 010); //execute
echo("</td>" .

//all
     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 04); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015,        "checkbox w", TRUE, $access & 02); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 01); //execute
echo("</td>" .
     "</tr>" .
     "<tr>" .
     "</table>" .

     "<span class=\"element\">" .
     "<small>" .
     "* " . CMS_L_IFC_FILEMANAGER_017 .
     "</small>" .
     "</span>");

ifc_tab_close();

//file upload
$ifc->set(CMS_L_IFC_FILEMANAGER_002, "multifile 30");
$ifc->set(CMS_L_IFC_FILEMANAGER_001, "button b", "upload");
$ifc->set(CMS_L_IFC_FILEMANAGER_027, "info");
echo(jscript("ifc_autopost(\"ifc_file1[]\", \"upload\");") .

     "</td>" .
     "</tr>" .
     "</table>");

$ifc->close();