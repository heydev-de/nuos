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

//permission
ifc_permission(["" => CMS_L_ACCESS]);

init($object);
init($type, "user");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "select_user":
//..................................................................................................

    //clear selected object when changing type
    if (nstreq($type, "user")) $object = NULL;

    $type = "user";
    break;

case "select_group":
//..................................................................................................

    //clear selected object when changing type
    if (nstreq($type, "group")) $object = NULL;

    $type = "group";
    break;

case "_add_user":
//..................................................................................................

    //password does not match repetition
    do
    {
        if (nstreq($ifc_param3, $ifc_param4))
        {
            $ifc_response = CMS_L_IFC_PERMISSION_023;
            break;
        };

        $permission = new permission();

        //user already exists
        $ifc_param2 = trim($ifc_param2);
        if ($permission->data->get("user.$ifc_param2"))
        {
            $ifc_response = CMS_L_IFC_PERMISSION_024;
            break;
        };

        //create user
        $_object = $permission->user(
            $ifc_param2,  //user
            TRUE,         //disabled
            $ifc_param1,  //name
            $ifc_param3); //password

        if ($_object !== FALSE) //success
        {
            $object       = $_object;
            $type         = "user";
            $ifc_response = CMS_MSG_DONE;
            break 2;
        };

        $ifc_response = CMS_MSG_ERROR;
    }
    while (FALSE);

case "add_user":
//..................................................................................................

    //init
    if (CMS_IFC_MESSAGE === "add_user") $ifc_param1 = $ifc_param2 = NULL;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object,
         "type"   => $type],
        "_add_user",
        CMS_L_IFC_PERMISSION_001);

    //name
    $ifc->set(CMS_L_NAME,               "text 40 40 b", $ifc_param1);

    //user id
    $ifc->set(CMS_L_IFC_PERMISSION_002, "text 40 40 b", $ifc_param2);

    //password
    $ifc->set(CMS_L_PASSWORD,           "password 40 40 b");
    $ifc->set(CMS_L_IFC_PERMISSION_003, "password 40 40");

    $ifc->close();

case "_add_group":
//..................................................................................................

    do
    {
        $permission = new permission();

        //group does not exist
        if ($permission->data->get("group.$ifc_param2"))
        {
            $ifc_response = CMS_L_IFC_PERMISSION_025;
            break;
        };

        //create group
        $_object = $permission->group(
            $ifc_param2,  //group
            TRUE,         //disabled
            $ifc_param1); //name

        if ($_object !== FALSE)
        {
            $object       = $_object;
            $type         = "group";
            $ifc_response = CMS_MSG_DONE;
            break 2;
        };

        $ifc_response = CMS_MSG_ERROR;
    }
    while (FALSE);

case "add_group":
//..................................................................................................

    //init
    if (CMS_IFC_MESSAGE === "add_group") $ifc_param1 = $ifc_param2 = NULL;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        "_add_group",
        CMS_L_IFC_PERMISSION_004);

    //name
    $ifc->set(CMS_L_NAME,               "text 40 40 b", $ifc_param1);

    //group id
    $ifc->set(CMS_L_IFC_PERMISSION_005, "text 40 40",   $ifc_param2);

    $ifc->close();

case "save_user":
//..................................................................................................

    $permission = new permission();

    //no password given or password does not match repetition
    if (nstre($ifc_param3) && nstreq($ifc_param3, $ifc_param4))
    {
        $ifc_param3   = NULL;
        $ifc_response = CMS_L_IFC_PERMISSION_033;
    };

    $user        = permission_remove_prefix($object);
    $ifc_param5  = blank($ifc_param5)  ? NULL : implode("\n", $ifc_param5); //group
    $ifc_param11 = blank($ifc_param11) ? NULL : $ifc_param11;               //expire

    //apply changes
    if (
         ($permission->user($user,                   //user
                            isset($ifc_param2),      //disabled
                            $ifc_param1,             //name
                            $ifc_param3,             //password
                            $ifc_param5,             //group
                            $ifc_param6,             //permission
                            $ifc_param7,             //exclusion
                            $ifc_param9,             //email
                            $ifc_param10,            //timezone
                            $ifc_param8,             //comment
                            $ifc_param11) !== FALSE) //expire
         &&
         $permission->agent($user,
                            $ifc_param12,            //instruction
                            $ifc_param13,            //reminder interval
                            $ifc_param14)            //token expire
       )
    {
        $ifc_response = CMS_MSG_DONE . $ifc_response;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "token_rotate":
//..................................................................................................

    if (blank($user)) ifc_close_external(); //no user

    $permission = new permission();
    if (! $permission->data->get("user.$user")) ifc_close_external(); //invalid user

    //create new token
    $token = $permission->rotate_token($user);
    if ($token === FALSE) ifc_close_external(); //error

    //display
    $ifc = new ifc(CMS_L_IFC_PERMISSION_041, NULL, NULL);

    //api key
    $name = x(permission_get_name($user));
    $ifc->set(sprintf(CMS_L_IFC_PERMISSION_040, $name), "text 60 bd", $token);
    $ifc->set(CMS_L_IFC_PERMISSION_042,                 "info");

    $ifc->close();

case "save_group":
//..................................................................................................

    $permission = new permission();

    //apply changes
    if ($permission->group(permission_remove_prefix($object), //group
                           isset($ifc_param2),                //disabled
                           $ifc_param1,                       //name
                           $ifc_param4,                       //permission
                           $ifc_param5,                       //exclusion
                           $ifc_param6) !== FALSE)            //comment
    {
        init($ifc_param3, []);

        //apply group assignments to users
        $permission->data->move("first");
        while ($key = $permission->data->move("next"))
        {
            if (! permission_is_user($key))  continue; //skip groups
            $_key = permission_remove_prefix($key);

            if (in_array($key, $ifc_param3)) $permission->add_group($_key, $object);
            else                             $permission->del_group($_key, $object);
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "activate":
//..................................................................................................

    if (blank($list)) break; //no selection

    $permission = new permission();

    //delete disabled flags of selected objects
    foreach ($list AS $value)
        $permission->data->del($value, "disabled");

    //save changes
    $ifc_response = $permission->data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "deactivate":
//..................................................................................................

    if (blank($list)) break; //no selection

    $permission = new permission();

    //set disabled flags of selected objects
    foreach ($list AS $value)
        $permission->data->set(TRUE, $value, "disabled");

    //save changes
    $ifc_response = $permission->data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (blank($list)) break; //no selection

    $permission = new permission();

    //delete selected objects
    //deleted groups are automatically removed from users
    $error = FALSE;
    foreach ($list AS $value)
        $error |= ! $permission->delete($value);

    //deselect object if deleted
    if (in_array($object, $list)) $object = NULL;

    //save changes
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "set":
//..................................................................................................

    $ifc_param1 = blank($access) ? cms_cache("permission." . CMS_USER . ".access") : $access;

case "_set":
case "set_add":
case "set_add_ex":
case "set_del":
case "set_del_ex":
//..................................................................................................

    $permission = new permission();

    //permit or exclude access for selected users / groups
    if (
         ($ifc_param1 = utf8_trim($ifc_param1)) //access
         &&
         ((! blank($ifc_param2)) || (! blank($ifc_param3)))
         &&
         preg_match("/^set_(?:add|del)(?:_ex)?$/", CMS_IFC_MESSAGE)
       )
    {
        $error = FALSE;

        switch (CMS_IFC_MESSAGE)
        {
        case "set_add_ex":
//..................................................................................................

            $explicit = TRUE;

        case "set_add":
//..................................................................................................

            $explicit = isset($explicit);

            //give permission to selected users/groups
            if (! blank($ifc_param3))
            {
                foreach ($ifc_param3 AS $value)
                    $error |= ! $permission->permit($value, $ifc_param1, $explicit);
            };

            if (! blank($ifc_param2))
            {
                foreach ($ifc_param2 AS $value)
                    $error |= ! $permission->permit($value, $ifc_param1, $explicit);
            };

            break;

        case "set_del_ex":
//..................................................................................................

            $explicit = TRUE;

        case "set_del":
//..................................................................................................

            $explicit = isset($explicit);

            //deny permission from selected users / groups
            if (! blank($ifc_param2))
            {
                foreach ($ifc_param2 AS $value)
                    $error |= ! $permission->exclude($value, $ifc_param1, $explicit);
            };

            if (! blank($ifc_param3))
            {
                foreach ($ifc_param3 AS $value)
                    $error |= ! $permission->exclude($value, $ifc_param1, $explicit);
            };

            break;
        };

        //save changes
        $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    };

    //cache last access permanently
    cms_cache("permission." . CMS_USER . ".access", $ifc_param1, TRUE);

    //helper function
    $unique = function($name, &$array)
    {
        $_name = $name;
        $i     = 0;
        while (isset($array[$_name])) $_name = "$name (" . ++$i . ")";
        return $_name;
    };

    //init
    $data      = &$permission->data;
    $cache     = [];
    $permitted = [];
    $excluded  = [];

    data_sort($data, "name");

    //iterate users and groups
    $data->move("first");
    while ($key = $data->move("next"))
    {
        if ($data->get($key, "disabled")) continue; //skip disabled

        //retrieve group name
        $name = $data->get($key, "name");

        //group
        if (permission_is_group($key))
        {
            //check permission of group
            $name        = "[$name]";
            $_permission = $data->get($key, "permission") ?? "";
            $exclusion   = $data->get($key, "exclusion")  ?? "";
            $flag        = permission_match($ifc_param1, $_permission, $exclusion);

            if ($flag === TRUE)      { $list = &$permitted; $name .= " ⁺"; } //explicitly permitted
            elseif ($flag === FALSE) { $list = &$excluded;  $name .= " ⁻"; } //explicitly excluded
            else                       $list = &$excluded;                   //implicitly denied

            //add group to list
            $list[$unique($name, $list)] = $key;

            //iterate users in group
            $data->move("first");
            while ($_key = $data->move("next"))
            {
                if ($data->get($_key, "disabled")) continue; //skip disabled
                if (! permission_is_user($_key))   continue; //skip non-users

                //skip users not in group
                if (strpos("\n" . $data->get($_key, "group") . "\n", "\n$key\n") === FALSE) continue;

                //retrieve user name
                $name = "\xE2\x80\x83" . //em space
                        $data->get($_key, "name");

                //check permission of user
                $user        = permission_remove_prefix($_key);
                $_permission = $cache[$user][0] ??= $permission->get_user_permission($user);
                $exclusion   = $cache[$user][1] ??= $permission->get_user_exclusion($user);
                $flag        = permission_match($ifc_param1, $_permission, $exclusion);

                if ($flag === TRUE)      $name .= " ⁺"; //explicitly permitted
                elseif ($flag === FALSE) $name .= " ⁻"; //explicitly excluded

                //add user below group
                $list[$unique($name, $list)] = $_key;
            };

            //continue after group position
            $data->move("to", $key);
        }

        //user
        else
        {
            //check permission of user
            $user        = permission_remove_prefix($key);
            $_permission = $cache[$user][0] ??= $permission->get_user_permission($user);
            $exclusion   = $cache[$user][1] ??= $permission->get_user_exclusion($user);
            $flag        = permission_match($ifc_param1, $_permission, $exclusion);

            //explicitly permitted
            if ($flag === TRUE)
            {
                $permitted[$unique("$name ⁺", $permitted)] = $key;
            }

            //explicitly excluded
            elseif ($flag === FALSE)
            {
                $excluded[$unique("$name ⁻", $excluded)] = $key;
            }

            //implicit
            else
            {
                //check group permissions of user
                $_permission = $cache[$user][2] ??= $permission->get_group_permission($user);
                $exclusion   = $cache[$user][3] ??= $permission->get_group_exclusion($user);
                $flag        = permission_match($ifc_param1, $_permission, $exclusion);

                if ($flag) $permitted[$unique($name, $permitted)] = $key; //implicitly permitted
                else       $excluded[$unique($name, $excluded)]   = $key; //implicitly excluded
            };
        };
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        [CMS_L_COMMAND_CLOSE . "|permission/command_cancel" => FALSE],
        ["type" => $type],
        "_set",
        CMS_L_IFC_PERMISSION_014);

    ifc_table_open("min");
    echo("<tr>" .
         "<td colspan=\"3\">");

    //access
    $ifc->set(CMS_L_IFC_PERMISSION_032, "text 40",  $ifc_param1);

    //refresh button
    $ifc->set(CMS_L_COMMAND_REFRESH,    "button",   "_set");
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //permitted
         "<th>" .
         image("permission/icon_access") . " " . CMS_L_IFC_PERMISSION_015 .
         "</th>" .

    //space
         "<td class=\"select\">" .
         "</td>" .

    //excluded
         "<th>" .
         image("permission/icon_denial") . " " . CMS_L_IFC_PERMISSION_016 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //permitted
         "<td class=\"varied\">");

    //permitted users / groups list
    $ifc->set($permitted,               "multiselect 30x15 b");
    $ifc->set(CMS_L_IFC_PERMISSION_030, "info");
    echo("</td>" .

    //explicit control
         "<td class=\"select\" " .
             "style=\"TEXT-ALIGN:center;VERTICAL-ALIGN:middle\">");

    //explicit permit button
    $ifc->set("‹",                      "button b", "set_add_ex");

    //explicit exclude button
    $ifc->set("›",                      "button",   "set_del_ex");
    echo("</td>" .

    //excluded
         "<td class=\"varied\">");

    //excluded users / group list
    $ifc->set($excluded,                "multiselect 30x15 b");
    $ifc->set(CMS_L_IFC_PERMISSION_031, "info");
    echo("</td>" .
         "</tr>" .

    //control
         "<tr>" .
         "<td colspan=\"3\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:center\">");

    //delete permission button
    $ifc->set(CMS_L_IFC_PERMISSION_028, "button",   "set_del");

    //delete exclusion button
    $ifc->set(CMS_L_IFC_PERMISSION_029, "button",   "set_add");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

    $ifc->close();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$count = strlen($type);

//set type to match selected object
if ($object && strncmp($object, $type, $count))
    $type = substr($object, 0, strpos($object, "."));

//menu
$menu = [CMS_L_IFC_PERMISSION_001 .      "|permission/command_create_user"  => "add_user",
         CMS_L_IFC_PERMISSION_004 .      "|permission/command_create_group" => "add_group",
         CMS_L_IFC_PERMISSION_026 .      "|permission/command_enable"       => "activate",
         CMS_L_IFC_PERMISSION_027 .      "|permission/command_disable"      => "deactivate",
         CMS_L_COMMAND_DELETE_SELECTED . "|permission/command_delete"       => "#delete",
         CMS_L_IFC_PERMISSION_014 .      "|permission/command_permission"   => "set"];

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object,
     "type"   => $type]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open("varied");
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"2\">" .
     "</colgroup>" .

//type selection
     "<tr>" .
     "<td colspan=\"3\">");
$ifc->set(CMS_L_IFC_PERMISSION_021, "button", "select_user",  $type === "user");
$ifc->set(CMS_L_IFC_PERMISSION_022, "button", "select_group", $type === "group");
echo("</td>" .
     "</tr>" .

//header
     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//name
     "<th>" .
     CMS_L_NAME .
     "</th>" .

//description
     "<th>" .
     CMS_L_IFC_PERMISSION_006 .
     "</th>" .
     "</tr>");

$permission = new permission();
$data       = &$permission->data;
data_sort($data, "name");

$initial = NULL;
$_list   = NULL;
$__list  = NULL;

$data->move("first");
while ($key = $data->move("next"))
{
    $name = $data->get($key, "name");

    if (strncmp($key, $type, $count)) //not selected type
    {
        //get associations
        if (nstre($object))
        {
            $_list[$name] = $key;

            if (
                 ($type === "group")
                 &&
                 (strpos("\n" . $data->get($key, "group") . "\n", "\n$object\n") !== FALSE)
               )
                $__list[] = $key;
        };

        continue; //skip display
    };

    $_initial = utf8_strtoupper(utf8_substr($name, 0, 1));

    //initial separator
    if (nstreq($initial, $_initial))
    {
        $initial = $_initial;
        echo("<tr>" .

        //spacer
             "<td class=\"highlight\">" .
             "</td>" .

        //initial
             "<td colspan=\"2\" " .
                 "class=\"highlight\">" .
             x($initial) .
             "</td>" .
             "</tr>");
    };

    echo("<tr>" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $key, streq($key, $object), "list[]");
    echo("</td>" .

    //name
         "<td>" .

    //icon
         "<a href=\"javascript:ifc_post('select','" . qx($key) . "');\">" .
         image($data->get($key, "disabled") ?
               "permission/icon_" . $type . "_disabled" :
               "permission/icon_" . $type));

    //name
    $name = x($name);
    if (streq($key, $object))
        $name = "<strong>$name</strong>";

    echo(" $name" .
         "</a>" .
         "</td>" .

    //comment
         "<td>" .
         "<span class=\"br\">" .
         x($data->get($key, "comment")) .
         "</span>" .
         "</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"3\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//properties
if ($object)
{
    echo("<td>");

    ifc_table_open();

    switch ($type)
    {
    case "group":
//..................................................................................................

        //group id
        echo("<tr>" .
             "<th>" .
             x(permission_remove_prefix($object)) .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td class=\"varied\">");

        //name
        $ifc->set(CMS_L_NAME,               "text 30 40",          $data->get($object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,       "button",              "save_group");

        ifc_tab_open("Basic");

        //disabled flag
        $ifc->set(CMS_L_IFC_PERMISSION_008, "checkbox b",          TRUE, $data->get($object, "disabled"));

        //users
        $ifc->set(CMS_L_IFC_PERMISSION_012, "label");
        $ifc->set($_list,                   "multiselect 40x12 b", $__list);

        //access
        ifc_tab_next(CMS_L_IFC_PERMISSION_019);

        //permissions
        $ifc->set(CMS_L_IFC_PERMISSION_009, "textarea 40x8 b",     $data->get($object, "permission"));

        //exclusions
        $ifc->set(CMS_L_IFC_PERMISSION_018, "textarea 40x8 b",     $data->get($object, "exclusion"));
        $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

        //extended
        ifc_tab_next(CMS_L_IFC_PERMISSION_020);

        //comment
        $ifc->set(CMS_L_IFC_PERMISSION_006, "textarea 40x4 250",   $data->get($object, "comment"));

        break;

    case "user":
//..................................................................................................

        //user id
        $user = permission_remove_prefix($object);

        echo("<tr>" .
             "<th>" .
             x($user) .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td class=\"varied\">");

        //name
        $ifc->set(CMS_L_NAME,               "text 30 40",          $data->get($object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,       "button",              "save_user");

        ifc_tab_open(CMS_L_IFC_PERMISSION_034);

        //disabled flag
        $ifc->set(CMS_L_IFC_PERMISSION_008, "checkbox b",          TRUE, $data->get($object, "disabled"));

        //password
        $ifc->set(CMS_L_PASSWORD,           "password 40 40 b",    NULL);
        $ifc->set(CMS_L_IFC_PERMISSION_003, "password 40 40 b",    NULL);

        //groups
        $ifc->set(CMS_L_IFC_PERMISSION_007, "label");
        $ifc->set($_list,                   "multiselect 40x12 b", preg_split("/\n/", $data->get($object, "group") ?? ""));

        //access
        ifc_tab_next(CMS_L_IFC_PERMISSION_019);

        //permissions
        $ifc->set(CMS_L_IFC_PERMISSION_009, "textarea 40x8 b",     $data->get($object, "permission"));

        //exclusions
        $ifc->set(CMS_L_IFC_PERMISSION_018, "textarea 40x8 b",     $data->get($object, "exclusion"));
        $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

        //extended
        ifc_tab_next(CMS_L_IFC_PERMISSION_020);

        //comment
        $ifc->set(CMS_L_IFC_PERMISSION_006, "textarea 40x4 250 b", $data->get($object, "comment"));

        //email
        $ifc->set(CMS_L_EMAIL,              "text 40 40 b",        $data->get($object, "email"));

        //timezone
        $array = timezone_identifiers_list();
        $array = array_combine($array, $array);
        $value = $data->get($object, "timezone");
        if (! isset($array[$value ?? ""])) $value = CMS_TIMEZONE_DEFAULT;
        $ifc->set(CMS_L_IFC_PERMISSION_017, "label");
        $ifc->set($array,                   "select 40 b",         $value);

        //expiry date
        ($object === "user.admin") ? $ifc->dummy() :
        $ifc->set(CMS_L_IFC_PERMISSION_011, "datetime",            $data->get($object, "expire"));

        //agent
        ifc_tab_next(CMS_L_IFC_PERMISSION_035);

        //instruction
        $ifc->set(CMS_L_IFC_PERMISSION_036, "textarea 40x12 b",    $permission->instruction($user));

        //reminder interval
        $ifc->set(CMS_L_IFC_PERMISSION_039, "text 40 40",          $data->get($object, "instruction_interval"));

        echo("<hr>");

        //rotate api key
        $ifc->set(CMS_L_IFC_PERMISSION_037,
                  "button b",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                             "ifc_message" => "token_rotate",
                             "user"        => $user])) . "');");

        //key expiry data
        $ifc->set(CMS_L_IFC_PERMISSION_038, "datetime b",          $data->get($object, "token_expire"));

        ifc_tab_close();
        break;
    };

    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();