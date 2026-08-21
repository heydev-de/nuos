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
if (! cms_load("search")) ifc_inactive($ifc_page);

//permission
ifc_permission(["" => CMS_L_ACCESS]);

//instantiate search object
$search = new search();
if (! $search->enabled) ifc_inactive($ifc_page);

//initiate
init($object);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "scan":
//..................................................................................................

    $array = [CMS_SEARCH_SCAN_UNKNOWN_ERROR               => CMS_L_IFC_SEARCH_043,
              CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED  => CMS_L_IFC_SEARCH_013,
              CMS_SEARCH_SCAN_INVALID_ADDRESS             => CMS_L_IFC_SEARCH_033,
              CMS_SEARCH_SCAN_ADDRESS_REJECTED            => CMS_L_IFC_SEARCH_051,
              CMS_SEARCH_SCAN_NO_CONNECTION               => CMS_L_IFC_SEARCH_034,
              CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT => CMS_L_IFC_SEARCH_035,
              CMS_SEARCH_SCAN_NO_MODIFICATION             => CMS_L_IFC_SEARCH_036,
              CMS_SEARCH_SCAN_DATA_FETCH_FAILED           => CMS_L_IFC_SEARCH_038,
              CMS_SEARCH_SCAN_NO_CONTENT                  => CMS_L_IFC_SEARCH_039,
              CMS_SEARCH_SCAN_INDEXING_UNDESIRED          => CMS_L_IFC_SEARCH_040,
              CMS_SEARCH_SCAN_INDEXED                     => CMS_L_IFC_SEARCH_041,
              CMS_SEARCH_SCAN_INDEXING_FAILED             => CMS_L_IFC_SEARCH_042];

    $ifc_param1 = translate_url($ifc_param1, NULL, CMS_LANGUAGE, TRUE);
    $result     = $search->scan($ifc_param1, isset($ifc_param2));

    $ifc_response = (CMS_SEARCH_SCAN_ERROR & $result) ? CMS_MSG_ERROR : CMS_MSG_DONE;
    if (isset($array[$result])) $ifc_response .= $array[$result];
    break;

case "queue_add":
//..................................................................................................

    if (blank($list)) break; //no selection

    $error = FALSE;

    if ($result = mysql_query("SELECT " . CMS_DB_SEARCH_ENTRY_ADDRESS . " " .
                              "FROM " .   CMS_DB_SEARCH_ENTRY . " " .
                              "WHERE " .  CMS_DB_SEARCH_ENTRY_INDEX . " " .
                              "IN ('" .   implode("', '", sqlesc($list)) . "')"))
    {
        while ($resultrow = mysql_fetch_row($result))
            $error |= ! $search->queue_add($resultrow[0], CMS_SEARCH_QUEUE_TYPE_SELECTION);
    };

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "queue_add_all":
//..................................................................................................

    $ifc_response = $search->queue_add_all() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "queue_add_internal":
//..................................................................................................

    $language = explode(",", CMS_LANGUAGE_ENABLED);
    $count    = 0;

    foreach ($language AS $value)
    {
        $value = stre($value) ? "" : "$value.";
        $map   = new map("#system/" . $value . "directory.content");
        $list  = $map->get_value_list();

        foreach ($list AS $_value)
            $count += $search->queue_add($_value, CMS_SEARCH_QUEUE_TYPE_INTERNAL);
    };

    $ifc_response = $count ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "queue_remove":
//..................................................................................................

    $ifc_response = $search->queue_remove($ifc_param) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "queue_process":
//..................................................................................................

    $array = [CMS_L_IFC_SEARCH_019 => CMS_SEARCH_QUEUE_TYPE_ALL,
              CMS_L_IFC_SEARCH_020 => CMS_SEARCH_QUEUE_TYPE_INTERNAL,
              CMS_L_IFC_SEARCH_027 => CMS_SEARCH_QUEUE_TYPE_SELECTION,
              CMS_L_IFC_SEARCH_021 => CMS_SEARCH_QUEUE_TYPE_SUBMISSION,
              CMS_L_IFC_SEARCH_022 => CMS_SEARCH_QUEUE_TYPE_REFERENCE,
              CMS_L_IFC_SEARCH_023 => CMS_SEARCH_QUEUE_TYPE_UPDATE];

    //display
    $ifc = new ifc(NULL, NULL, NULL);

    //javascript

?><script>

function queue_process_start()
{
    var type         = ifc_get("ifc_param1");
    var follow_links = ifc_get("ifc_param2") ? "1" : "0";
    location.replace("<?php
        echo(str_replace(["%replace1%",
                          "%replace2%"],
                         ["\"+encodeURIComponent(type)+\"",
                          "\"+encodeURIComponent(follow_links)+\""],
                         q(cms_url(["ifc_page"     => CMS_IFC_PAGE,
                                    "ifc_message"  => "_queue_process",
                                    "ifc_option"   => "external",
                                    "ifc_param"    => "\x1B%replace1%",
                                    "follow_links" => "\x1B%replace2%"]))));?>");
};

</script><?php

    $ifc->set($array,               "select b",   $ifc_param);
    $ifc->set(CMS_L_IFC_SEARCH_016, "checkbox b", TRUE, FALSE);
    $ifc->set(CMS_L_IFC_SEARCH_057, "button",     "javascript:queue_process_start();");

    $ifc->close();

case "_queue_process":
//..................................................................................................

    $array = [CMS_SEARCH_QUEUE_TYPE_INTERNAL   => CMS_L_IFC_SEARCH_020,
              CMS_SEARCH_QUEUE_TYPE_SELECTION  => CMS_L_IFC_SEARCH_027,
              CMS_SEARCH_QUEUE_TYPE_SUBMISSION => CMS_L_IFC_SEARCH_021,
              CMS_SEARCH_QUEUE_TYPE_REFERENCE  => CMS_L_IFC_SEARCH_022,
              CMS_SEARCH_QUEUE_TYPE_UPDATE     => CMS_L_IFC_SEARCH_023,
              CMS_SEARCH_QUEUE_TYPE_ALL        => CMS_L_IFC_SEARCH_019];

    $follow_links = $follow_links ?? FALSE;

    //display
    $ifc = new ifc(NULL, NULL, NULL);

    echo("<div>" .

    //description
         "<strong>" .
         $array[$ifc_param] .
         ($follow_links ? " – " . CMS_L_IFC_SEARCH_016 : "") .
         " … " .
         "<span id=\"search_queue_remaining\">" .
         sprintf(CMS_L_IFC_SEARCH_037, $search->queue_length($ifc_param)) .
         "</span>" .
         "</strong><br>");

    //threads
    for ($i = 0; $i < 4; $i++)
        echo("<object data=\"" . x(cms_url(["ifc_page"     => CMS_IFC_PAGE,
                                            "ifc_message"  => "queue_process_thread",
                                            "ifc_param"    => $ifc_param,
                                            "follow_links" => $follow_links])) . "\" " .
                     "type=\"text/html\" " .
                     "class=\"search-thread\">" .
             "</object>");

    echo("</div>");

    $ifc->close();

case "queue_process_thread":
//..................................................................................................

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         "<title></title>" .
         "<link rel=\"stylesheet\" " .
               "href=\"" . CMS_URL . "stylesheet/" . CMS_IFC_EDITION . ".css\">" .
         "</head>" .

         "<body class=\"search-thread\">" .
         "<div id=\"loader\" " .
              "class=\"search-thread\">" .
         CMS_L_IFC_SEARCH_029 .
         "</div>" .
         "</body>" .
         "</html>");

    force_flush();

    if (! isset($ifc_param))    $ifc_param    = CMS_SEARCH_QUEUE_TYPE_ALL;
    if (! isset($follow_links)) $follow_links = FALSE;
    else                        $follow_links = (bool)$follow_links;

    //process queue
    if (($value = $search->queue_process($ifc_param, $follow_links)) !== FALSE)
    {
        if (CMS_SEARCH_SCAN_ERROR & $value) { $class = "error"; $text = CMS_L_IFC_SEARCH_030; }
        else                                { $class = "ok";    $text = CMS_L_IFC_SEARCH_031; };
        $time = (CMS_SEARCH_SCAN_ADDRESS_REJECTED & $value) ? 50 :
                (((CMS_SEARCH_SCAN_UNKNOWN_ERROR | CMS_SEARCH_SCAN_DATABASE_ERROR) & $value) ? 15000 :
                ((CMS_SEARCH_SCAN_ERROR & $value) ? 2500 : rand(500, 1500)));
    }
    else
    {
        $class = "done";
        $text  = CMS_L_IFC_SEARCH_032;
        $time  = 60000;
    };

    $buffer = jscript("var object = document.getElementById(\"loader\");" .
                      "object.className = \"search-thread-%s\";" .
                      "object.textContent = \"%s\";" .
                      "setTimeout(\"location.reload(true);\", %d);");

    printf($buffer, $class, $text, $time);

        //update queue length display

?><script>

window.parent.document.getElementById("search_queue_remaining").textContent = "<?php
    echo(sprintf(CMS_L_IFC_SEARCH_037, $search->queue_length($ifc_param)));?>";

</script><?php

    exit();

case "entry_remove":
//..................................................................................................

    if (blank($list)) break; //no selection

    $error = FALSE;

    foreach ($list AS $value)
        $error |= ! $search->entry_remove($value);

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "data_process":
//..................................................................................................

    //display
    $ifc = new ifc(NULL, NULL, NULL);

    echo("<div>" .

    //description
         "<strong>" .
         CMS_L_IFC_SEARCH_046 .
         "</strong><br>" .

    //thread
         "<object data=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                        "ifc_message" => "data_process_thread",
                                        "_" . unique_id() => 1])) . "\" " .
                 "type=\"text/html\" " .
                 "class=\"search-thread\" " .
                 "style=\"WIDTH:100%\">" .
         "</object>" .

         "</div>");

    $ifc->close();

case "data_process_thread":
//..................................................................................................

    init($action, 0);

    switch ($action)
    {
    case 0: //initialize page score

        $iteration = 1;
        $image     = "data_processing.svg";
        $info      = CMS_L_IFC_SEARCH_047;
        break;

    case 1: //compute page score

        if ($iteration <= $search->score_compute_iteration_number)
        {
            $image = "data_processing.svg";
            $info  = sprintf(CMS_L_IFC_SEARCH_048,
                             $iteration,
                             $search->score_compute_iteration_number);
            break;
        };

        $action++;

    case 2: //finalize page score

        $iteration = $search->score_compute_link_level();
        $image     = "data_processing.svg";
        $info      = CMS_L_IFC_SEARCH_049;
        break;

    case 3: //integrate dangling links

        if ($iteration > 0)
        {
            $level = $search->score_compute_link_level();
            $image = "data_processing.svg";
            $info  = sprintf(CMS_L_IFC_SEARCH_050,
                             $level - $iteration + 1,
                             $level);
            break;
        };

        $action++;

    case 4: //find canonical entries

        $image = "data_processing.svg";
        $info  = CMS_L_IFC_SEARCH_056;
        break;

    case 5: //complete

        $image = "data_success.svg";
        $info  = CMS_L_IFC_SEARCH_032;
        break;
    };

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         "<title></title>" .
         "<link rel=\"stylesheet\" " .
               "href=\"" . CMS_URL . "stylesheet/" . CMS_IFC_EDITION . ".css\">" .
         "</head>" .
         "<body class=\"search-thread\">" .
         "<div>" .
         "<img id=\"loader\" " .
              "src=\"" . x(CMS_IMAGES_URL) . "search/$image\" " .
              "alt=\"\"> " .
         "<span id=\"info\" " .
               "class=\"br\">" .
         $info .
         "</span>" .
         "</div>" .
         "</body>" .
         "</html>");

    ob_flush();
    flush();

    switch ($action)
    {
    case 0: //initialize page score

        $result = $search->score_compute_initialize();
        $action++;
        break;

    case 1: //compute page score

        $result = $search->score_compute_iterate();
        $iteration++;
        break;

    case 2: //finalize

        $result = $search->score_compute_finalize();
        $action++;
        break;

    case 3: //integrate dangling links

        $result = $search->score_compute_iterate($iteration);
        $iteration--;
        break;

    case 4: //find canonical entries

        $result = $search->score_compute_canonical();
        $action++;
        break;

    case 5: //complete

        exit();
    };

    //success
    if ($result === TRUE)
    {
        echo(jscript("location.replace(\"" .
                     q(cms_url(["ifc_page"        => CMS_IFC_PAGE,
                                "ifc_message"     => "data_process_thread",
                                "action"          => $action,
                                "iteration"       => $iteration,
                                "_" . unique_id() => 1])) . "\");"));
        exit();
    };

    //failure
    $image = q(CMS_IMAGES_URL) . "search/data_failure.svg";
    echo(jscript("document.getElementById(\"loader\").src = \"$image\";" .
                 "document.getElementById(\"info\").textContent = \"" . qx($result) . "\";"));
    exit();

case "filter":
//..................................................................................................

    $system    = new system();
    $blacklist = $system->getval("search", "blacklist");
    $whitelist = $system->getval("search", "whitelist");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   NULL,
                   "_filter",
                   CMS_L_IFC_SEARCH_052);

    $ifc->set(CMS_L_IFC_SEARCH_055, "info b");
    $ifc->set(CMS_L_IFC_SEARCH_053, "textarea 60x12 b", $blacklist);
    $ifc->set(CMS_L_IFC_SEARCH_054, "textarea 60x12",   $whitelist);

    $ifc->close();

case "_filter":
//..................................................................................................

    $system = new system();

    $system->setval($ifc_param1, "search", "blacklist");
    $system->setval($ifc_param2, "search", "whitelist");

    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "stopword":
//..................................................................................................

    $data  = new data("#system/language");
    $array = [];

    $data->move("first");
    while ($key = $data->move("next"))
        $array[$key] = $data->get($key, "stopword");

    $language = array_keys($array);
    natsort($language);
    $language = implode(",", $language);

    $value = language_set_array($array);

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   NULL,
                   "_stopword",
                   CMS_L_IFC_SEARCH_059);

    $ifc->set(CMS_L_IFC_SEARCH_061, "info b");
    $ifc->set(CMS_L_IFC_SEARCH_060, "textarea 60x24 bl", $value, NULL, NULL, $language);

    $ifc->close();

case "_stopword":
//..................................................................................................

    $data  = new data("#system/language");
    $array = language_get_array($ifc_param1);

    $data->move("first");
    while ($key = $data->move("next"))
        $data->set($array[$key] ?? "", $key, "stopword");

    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "clean":
//..................................................................................................

    $ifc_response = $search->clean() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "reset":
//..................................................................................................

    $filter_field  = NULL;
    $filter_option = NULL;
    $filter_value  = NULL;
    $order         = NULL;
    $page          = NULL;
    $limit         = NULL;

    cms_cache_delete(
        ["search." . CMS_USER . ".filter_field",
         "search." . CMS_USER . ".filter_option",
         "search." . CMS_USER . ".filter_value",
         "search." . CMS_USER . ".order",
         "search." . CMS_USER . ".page",
         "search." . CMS_USER . ".limit"]);
    break;

case "configuration":
//..................................................................................................

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   NULL,
                   "_configuration",
                   CMS_L_IFC_SEARCH_062);

    //daemon
    if (($status = $search->daemon_get_status()) === FALSE)
        $status = ["enabled"      => FALSE,
                   "type"         => FALSE,
                   "follow_links" => FALSE];

    $ifc->set(CMS_L_IFC_SEARCH_065, "title");

    $ifc->set(CMS_L_IFC_SEARCH_066, "checkbox b", TRUE, $status["enabled"]);

    echo("<hr>");

    $ifc->set(CMS_L_IFC_SEARCH_020, "checkbox b", TRUE, $status["type"] & CMS_SEARCH_QUEUE_TYPE_INTERNAL);
    $ifc->set(CMS_L_IFC_SEARCH_027, "checkbox b", TRUE, $status["type"] & CMS_SEARCH_QUEUE_TYPE_SELECTION);
    $ifc->set(CMS_L_IFC_SEARCH_021, "checkbox b", TRUE, $status["type"] & CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
    $ifc->set(CMS_L_IFC_SEARCH_022, "checkbox b", TRUE, $status["type"] & CMS_SEARCH_QUEUE_TYPE_REFERENCE);
    $ifc->set(CMS_L_IFC_SEARCH_023, "checkbox b", TRUE, $status["type"] & CMS_SEARCH_QUEUE_TYPE_UPDATE);

    echo("<hr>");

    $ifc->set(CMS_L_IFC_SEARCH_016, "checkbox b", TRUE, $status["follow_links"]);

    //search
    $ifc->set(CMS_L_IFC_SEARCH_067, "title");

    //maximum bit difference
    $array    = [];
    for ($i = 0; $i <= 64; $i++) $array[$i] = $i;
    $array[2] = "2 " . CMS_L_IFC_SEARCH_064;
    $array    = array_flip($array);

    $ifc->set(CMS_L_IFC_SEARCH_063, "label");
    $ifc->set($array, "select 30", $search->entry_set_maximum_bit_difference);

    $ifc->close();

case "_configuration":
//..................................................................................................

    $type = (isset($ifc_param2) ? CMS_SEARCH_QUEUE_TYPE_INTERNAL   : 0) |
            (isset($ifc_param3) ? CMS_SEARCH_QUEUE_TYPE_SELECTION  : 0) |
            (isset($ifc_param4) ? CMS_SEARCH_QUEUE_TYPE_SUBMISSION : 0) |
            (isset($ifc_param5) ? CMS_SEARCH_QUEUE_TYPE_REFERENCE  : 0) |
            (isset($ifc_param6) ? CMS_SEARCH_QUEUE_TYPE_UPDATE     : 0);

    $search->daemon_status(isset($ifc_param1), $type, isset($ifc_param7));

    $system = new system();
    $system->setval($ifc_param8, "search", "difference");
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;

    $search->entry_set_maximum_bit_difference = $ifc_param8;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//filter option list
$filter_option_list =
    [0 => " LIKE '#value#%'",
     1 => " LIKE '%#value#%'",
     2 => " LIKE '%#value#'",
     3 => ">'#value#'",
     4 => "='#value#'",
     5 => "<'#value#'"];

//order list
$order_list =
    [0 => CMS_DB_SEARCH_ENTRY_ADDRESS,        1 => CMS_DB_SEARCH_ENTRY_ADDRESS . " DESC",
     2 => CMS_DB_SEARCH_ENTRY_TITLE,          3 => CMS_DB_SEARCH_ENTRY_TITLE . " DESC",
     4 => CMS_DB_SEARCH_ENTRY_SCORE,          5 => CMS_DB_SEARCH_ENTRY_SCORE . " DESC",
     6 => CMS_DB_SEARCH_ENTRY_TIME,           7 => CMS_DB_SEARCH_ENTRY_TIME . " DESC",
     8 => CMS_DB_SEARCH_ENTRY_UPDATE_TIME,    9 => CMS_DB_SEARCH_ENTRY_UPDATE_TIME . " DESC"];

//sync settings
cms_cache_sync($filter_field,  "search." . CMS_USER . ".filter_field",  CMS_DB_SEARCH_ENTRY_ADDRESS);
cms_cache_sync($filter_option, "search." . CMS_USER . ".filter_option",  1);
cms_cache_sync($filter_value,  "search." . CMS_USER . ".filter_value",  "");
cms_cache_sync($page,          "search." . CMS_USER . ".page",           0);
cms_cache_sync($limit,         "search." . CMS_USER . ".limit",         25);
cms_cache_sync($order,         "search." . CMS_USER . ".order",          7);

$filter_option = (int)$filter_option;
$page          = (int)$page;
$limit         = (int)$limit;
$order         = (int)$order;

//menu
$menu = [CMS_L_IFC_SEARCH_028 .          "|search/command_queue_add"
         => "queue_add",
         CMS_L_IFC_SEARCH_026 .          "|search/command_queue_add_internal"
         => "queue_add_internal",
         CMS_L_IFC_SEARCH_015 .          "|search/command_queue_add_all"
         => "queue_add_all",
         CMS_L_COMMAND_DELETE_SELECTED . "|search/command_entry_remove"
         => "#entry_remove",
         CMS_L_IFC_SEARCH_017 .          "|search/command_data_process"
         => "javascript:load_page('" .
            q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                       "ifc_message" => "data_process",
                       "ifc_option"  => "external"])) . "');",
         CMS_L_IFC_SEARCH_052 .          "|search/command_filter"
         => "filter",
         CMS_L_IFC_SEARCH_059 .          "|search/command_stopword"
         => "stopword",
         CMS_L_IFC_SEARCH_025 .          "|search/command_clean"
         => "#clean",
         CMS_L_IFC_SEARCH_062 .          "|search/command_configuration"
         => "configuration"];

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["order" => $order,
     "page"  => $page]);

//javascript

?><script>

function qp(type)
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(type)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "queue_process",
                                    "ifc_param"   => "\x1B%replace%",
                                    "ifc_option"  => "external"]))));?>");
};

function qr(type)
{
    if (confirm("<?php echo(q(CMS_L_COMMAND_DELETE));?>?")) ifc_post("queue_remove", type);
};

</script><?php

echo("<div class=\"info\">" .
     sprintf(CMS_L_IFC_SEARCH_003,
             $search->queue_length(),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_ALL) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_ALL) . "');",
             $search->queue_length(CMS_SEARCH_QUEUE_TYPE_INTERNAL),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_INTERNAL) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_INTERNAL) . "');",
             $search->queue_length(CMS_SEARCH_QUEUE_TYPE_SELECTION),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_SELECTION) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_SELECTION) . "');",
             $search->queue_length(CMS_SEARCH_QUEUE_TYPE_SUBMISSION),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_SUBMISSION) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_SUBMISSION) . "');",
             $search->queue_length(CMS_SEARCH_QUEUE_TYPE_REFERENCE),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_REFERENCE) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_REFERENCE) . "');",
             $search->queue_length(CMS_SEARCH_QUEUE_TYPE_UPDATE),
             "javascript:qp('" . qx(CMS_SEARCH_QUEUE_TYPE_UPDATE) . "');",
             "javascript:qr('" . qx(CMS_SEARCH_QUEUE_TYPE_UPDATE) . "');") .
     "</div>");

//search filter
ifc_tab_open(CMS_L_IFC_SEARCH_001);

//field
echo("<div class=\"element\">");
$ifc->set([CMS_L_URL            => CMS_DB_SEARCH_ENTRY_ADDRESS,
           CMS_L_IFC_SEARCH_002 => CMS_DB_SEARCH_ENTRY_TITLE,
           CMS_L_IFC_SEARCH_004 => CMS_DB_SEARCH_ENTRY_TEXT,
           CMS_L_IFC_SEARCH_058 => CMS_DB_SEARCH_ENTRY_SCORE,
           CMS_L_IFC_SEARCH_005 => CMS_DB_SEARCH_ENTRY_TIME,
           CMS_L_IFC_SEARCH_024 => CMS_DB_SEARCH_ENTRY_UPDATE_TIME],
          "select",
          $filter_field,
          NULL,
          "filter_field");

//condition
$ifc->set([CMS_L_IFC_SEARCH_006 => 0,
           CMS_L_IFC_SEARCH_007 => 1,
           CMS_L_IFC_SEARCH_008 => 2,
           CMS_L_IFC_SEARCH_009 => 3,
           CMS_L_IFC_SEARCH_010 => 4,
           CMS_L_IFC_SEARCH_011 => 5],
          "select",
          $filter_option,
          NULL,
          "filter_option");

//value
$ifc->set(NULL, "text 20 :", $filter_value, NULL, "filter_value");
echo("</div>");
//row display limit per page
$ifc->set(CMS_L_IFC_SEARCH_012, "label");
$ifc->set([ 10 => "10",
            25 => "25",
            50 => "50",
           100 => "100",
           250 => "250",
           500 => "500"],
          "select :",
          $limit,
          NULL,
          "limit");

//apply button
$ifc->set(CMS_L_COMMAND_CONFIRM, "button");

//reset button
$ifc->set(CMS_L_IFC_SEARCH_044,  "button", "reset");

//add page
ifc_tab_next(CMS_L_IFC_SEARCH_018);

$buffer = "javascript:load_page('" .
          str_replace("%replace%",
                      "'+encodeURIComponent(ifc_get('ifc_param1'))+'",
                      q(cms_url(["ifc_page"          => "content",
                                 "ifc_message"       => "edit_href",
                                 "ifc_option"        => "external",
                                 "value"             => "\x1B%replace%",
                                 "ifc_select"        => "__value",
                                 "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set(" .
                                                        "'ifc_param1','%return%');"])) . "');");

$ifc->set(CMS_L_URL,            "text 30 :", CMS_PROTOCOL . "://");
$ifc->set("…",                  "button",    $buffer);
$ifc->set(CMS_L_IFC_SEARCH_016, "checkbox",  TRUE);
$ifc->set(CMS_L_IFC_SEARCH_014, "button",    "scan");

ifc_tab_close();

//retrieve data
$query   = "SELECT " .   CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                         CMS_DB_SEARCH_ENTRY_ADDRESS . ", " .
                         CMS_DB_SEARCH_ENTRY_TITLE . ", " .
                         CMS_DB_SEARCH_ENTRY_TIME . ", " .
                         CMS_DB_SEARCH_ENTRY_UPDATE_TIME . ", " .
                         CMS_DB_SEARCH_ENTRY_SCORE . ", " .
                         CMS_DB_SEARCH_ENTRY_ERROR . ", " .
                         CMS_DB_SEARCH_ENTRY_CANONICAL . " " .
           "FROM " .     CMS_DB_SEARCH_ENTRY . " ";
if (nstre($filter_value)) $query .=
           "WHERE " .    sqlesc($filter_field) .
                         str_replace("#value#", sqlesc($filter_value),
                                     $filter_option_list[$filter_option] ?? $filter_option_list[1]) . " ";
$query  .= "ORDER BY " . sqlesc($order_list[$order] ?? $order_list[7]);
$result  = mysql_query($query);

//javascript

?><script>

function o(value)
{
    ifc_set("order", value);
    ifc_post();
};

function p(number)
{
    ifc_set("page", number);
    ifc_post();
};

</script><?php

//pagination
$count = mysql_num_rows($result);

if ($count > $limit)
{
    $count = ceil($count / $limit);
    $page  = min((int)$page, $count - 1);

    pagination(
        "javascript:p(%page%);",
        $page,
        $count,
        CMS_L_COMMAND_NEXT,
        "pagination");

    mysql_data_seek($result, $page * $limit);
};

//helper functions
$f1 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    case $asc: return " ↗";
    case $dsc: return " ↘";
    };

    return "";
};

$f2 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    default:
    case $dsc: return "javascript:o($asc);";
    case $asc: return "javascript:o($dsc);";
    };
};

//page list
ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"5\">" .
     "</colgroup>" .

//header
     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//address
     "<th>");
$ifc->set(CMS_L_URL .            $f1($order, 0, 1), "button", $f2($order, 0, 1));
echo("</th>" .

//title
     "<th>");
$ifc->set(CMS_L_IFC_SEARCH_002 . $f1($order, 2, 3), "button", $f2($order, 2, 3));
echo("</th>" .

//score
     "<th>");
$ifc->set(CMS_L_IFC_SEARCH_058 . $f1($order, 4, 5), "button", $f2($order, 4, 5));
echo("</th>" .

//time
     "<th>");
$ifc->set(CMS_L_IFC_SEARCH_005 . $f1($order, 6, 7), "button", $f2($order, 6, 7));
echo("</th>" .

//update time
     "<th>");
$ifc->set(CMS_L_IFC_SEARCH_024 . $f1($order, 8, 9), "button", $f2($order, 8, 9));
echo("</th>" .
     "</tr>");

$i = 0;

while (
        ($i++ < $limit)
        &&
        ($resultrow = mysql_fetch_assoc($result))
      )
{
    $varied = ifc_varied();

    //icon
    $image = ((int)$resultrow[CMS_DB_SEARCH_ENTRY_ERROR] > 0) ?
             "icon_page_error" :
             ($resultrow[CMS_DB_SEARCH_ENTRY_CANONICAL] ?
             "icon_page_canonical" :
             "icon_page");

    echo("<tr>" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $resultrow[CMS_DB_SEARCH_ENTRY_INDEX], FALSE, "list[]");
    echo("</td>" .

    //address
         "<td$varied>" .
         "<a href=\"" . x(cms_url($resultrow[CMS_DB_SEARCH_ENTRY_ADDRESS], NULL, TRUE)) . "\" " .
            "onclick=\"load_page(this.href);return false;\">" .
         image("search/" . $image) . " " . x(strabridge($resultrow[CMS_DB_SEARCH_ENTRY_ADDRESS])) .
         "</a>" .
         "</td>" .

    //title
         "<td$varied>" .
         x(strabridge($resultrow[CMS_DB_SEARCH_ENTRY_TITLE], 60)) .
         "</td>" .

    //score
         "<td$varied>" .
         x(number_format($resultrow[CMS_DB_SEARCH_ENTRY_SCORE], 2,
                         CMS_L_DECIMAL_SEPARATOR,
                         CMS_L_THOUSAND_SEPARATOR)) .
         "</td>" .

    //time
         "<td$varied>" .
         x(friendly_date($resultrow[CMS_DB_SEARCH_ENTRY_TIME])) .
         "</td>" .

    //update time
         "<td$varied>" .
         x(friendly_date($resultrow[CMS_DB_SEARCH_ENTRY_UPDATE_TIME])) .
         "</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"6\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();