<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    $time   = NULL;
    break;

case "time":
//..................................................................................................

    $object = NULL;
    $time   = $ifc_param;
    break;

case "add":
//..................................................................................................

    if ($key = $desktop->data->seek(["#subtype" => "appointment"]))
    {
        $desktop->data->set_buffer(
            [["name"    => CMS_L_DESKTOP_APPOINTMENT_001,
              "#type"   => "appointment",
              "time"    => $ifc_param,
              "#expire" => $ifc_param + 86400]]);

        if (
             ($_object = $desktop->data->insert($key))
             &&
             $desktop->save()
           )
        {
            $object       = $_object;
            $time         = NULL;
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save":
//..................................................................................................

    $ifc_param2 = mktime($ifc_param5, $ifc_param6, 0, $ifc_param3, $ifc_param2, $ifc_param4);
    $desktop->object_set($object, "name",        $ifc_param1);
    $desktop->object_set($object, "time",        $ifc_param2);
    $desktop->object_set($object, "#expire",     isset($ifc_param7) ? $ifc_param2 + 86400 : NULL);
    $desktop->object_set($object, "content",     $ifc_param8);
    $desktop->object_set($object, "place",       $ifc_param9);
    $desktop->object_set($object, "participant", $ifc_param10);

    if ($desktop->save())
    {
        $time         = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if ($desktop->delete_object($object))
    {
        $object       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$time_current = time();

if (
     empty($time)
     &&
     (! $time = $desktop->object_get($object, "time"))
   )
    $time = $time_current;

$time           -= $time % 900;
$hour            = (int)date("G", $time);
$minute          = (int)date("i", $time);
$day             = (int)date("j", $time);
$days_per_month  = (int)date("t", $time);
$month           = (int)date("n", $time);
$year            = (int)date("Y", $time);
$day_count       = NULL;
$array           = NULL;

if ($hour < 6)
{
    $hour += 24;
    $day  -=  1;

    if ($day < 1)
    {
        if (--$month < 1)
        {
            $month  = 12;
            $year  -= 1;
        };

        $days_per_month = (int)date("t", mktime(6, 0, 0, $month, 1, $year));
        $day            = $days_per_month;
    };
};

//appointment list
$desktop->data->move("first");
while ($key = $desktop->data->move("next"))
{
    if (streq($desktop->object_type($key), "appointment"))
    {
        if (! $_time = $desktop->object_get($key, "time")) $_time = $time_current;
        $_time  -= $_time % 900;
        $_hour   = (int)date("G", $_time);
        $_day    = (int)date("j", $_time);
        $_month  = (int)date("n", $_time);
        $_year   = (int)date("Y", $_time);

        if ($_hour < 6)
        {
            $_hour += 24;
            $_day  -=  1;

            if ($_day < 1)
            {
                if (--$_month < 1)
                {
                    $_month  = 12;
                    $_year  -=  1;
                };

                $_day = (int)date("t", mktime(6, 0, 0, $_month, 1, $_year));
            };
        };

        if (($_year === $year) && ($_month === $month))
        {
            if (isset($day_count[$_day])) $day_count[$_day] += 1;
            else                          $day_count[$_day]  = 1;

            if ($_day === $day)
            {
                $_minute                    = (int)date("i", $_time);
                $array[$_hour][$_minute][]  = $key;
            };
        };
    };
};

$buffer_year  = NULL;
$buffer_month = NULL;
$buffer_day   = NULL;
$_buffer_day  = NULL;
$buffer_hour  = NULL;

for ($_year = $year - 5; $_year <= $year + 5; $_year++)
{
    if (($_year >= 1970) && ($_year <= 2037))
    {
        $_days_per_month  = (int)date("t", mktime(6, 0, 0, $month, 1, $_year));
        $_day             = ($day > $_days_per_month) ? $_days_per_month : $day;
        $buffer_year     .= "<td " . (($_year === $year) ? "class=\"enabled\" " : "") .
                                "style=\"TEXT-ALIGN:center\">" .
                            "<a href=\"javascript:ifc_post('time','" . mktime(6, 0, 0, $month, $_day, $_year) . "');\">" .
                            (($_year === $year) ? sprintf("<strong>%s</strong>", $_year) : $_year) .
                            "</a>" .
                            "</td>";

        if ($_year == $year)
        {
            for ($_month = 1; $_month <= 12; $_month++)
            {
                $_days_per_month  = (int)date("t", mktime(6, 0, 0, $_month, 1, $year));
                $_day             = ($day > $_days_per_month) ? $_days_per_month : $day;
                $buffer_month    .= "<td " . (($_month === $month) ? "class=\"enabled\" " : "") .
                                        "style=\"TEXT-ALIGN:center\">" .
                                    "<a href=\"javascript:ifc_post('time','" . mktime(6, 0, 0, $_month, $_day, $year) . "');\">" .
                                    (($_month === $month) ? sprintf("<strong>%s</strong>", month($_month)) : month($_month)) .
                                    "</a>" .
                                    "</td>";

                if ($_month === $month)
                {
                    for ($_day = 1; $_day <= $days_per_month; $_day++)
                    {
                        $_time    = mktime(6, 0, 0, $month, $_day, $year);
                        $day_week = (int)date("w", $_time);
                        $day_name = substr(weekday($day_week), 0, 2);

                        if (isset($day_count[$_day]))
                            $buffer_day .= "<td class=\"highlight\"";

                        elseif (in_array($day_week, [0, 6]))
                            $buffer_day .= "<td class=\"varied\"";

                        else
                            $buffer_day .= "<td";

                        $buffer_day  .= " style=\"TEXT-ALIGN:center\">" .
                                        $day_name .
                                        "</td>";
                        $_buffer_day .= "<td " . (($_day === $day)         ? "class=\"enabled\" "   :
                                                 (isset($day_count[$_day]) ? "class=\"highlight\" " : "")) .
                                            "style=\"TEXT-ALIGN:center\">" .
                                        "<a href=\"javascript:ifc_post('time','$_time');\">" .
                                            (($_day === $day)         ? sprintf("<strong>%s</strong>", $_day) : $_day) .
                                            (isset($day_count[$_day]) ? sprintf(" (%d)", $day_count[$_day])   : NULL) .
                                        "</a>" .
                                        "</td>";

                        if ($_day === $day)
                        {
                            for ($_hour = 6; $_hour < 30; $_hour++)
                            {
                                $_varied = ifc_varied(NULL, 1);

                                if ($_hour === 24)
                                {
                                    $_time        = mktime($_hour, 0, 0, $month, $day, $year);
                                    $buffer_hour .= "<th colspan=\"3\" " .
                                                        "style=\"TEXT-ALIGN:center\">" .
                                                    weekday(date("w", $_time)) . ", " . date(CMS_L_DATE_FORMAT, $_time) .
                                                    "</th>" .
                                                    "</tr>" .
                                                    "<tr>";
                                };

                                $buffer_hour .= "<td$_varied rowspan=\"4\">" .
                                                "<strong>" .
                                                sprintf("%02d:", ($_hour >= 24) ? $_hour - 24 : $_hour) .
                                                "</strong>" .
                                                "</td>";

                                for ($_minute = 0; $_minute <= 45; $_minute += 15)
                                {
                                    $_varied      = ifc_varied(NULL, 2);
                                    $buffer_hour .= "<td$_varied>" .
                                                    sprintf("%02d", $_minute) .
                                                    "</td>";

                                    if (isset($array[$_hour][$_minute]))
                                    {
                                        $buffer_hour .= "<td class=\"highlight\">";

                                        foreach ($array[$_hour][$_minute] AS $value)
                                        {
                                            $name         = x($desktop->object_get($value, "name"));
                                            $buffer_hour .= "<a href=\"javascript:ifc_post('select','" . qrx($value) . "');\">" .
                                                            image("desktop/icon_appointment") . " " .
                                                            (streq($value, $object) ? sprintf("<strong>%s</strong>", $name) : $name) .
                                                            "</a><br>";
                                        };
                                    }
                                    else
                                    {
                                        $buffer_hour .= "<td$_varied>";
                                    };

                                    $buffer_hour .= "<a href=\"javascript:ifc_post('add','" . mktime($_hour, $_minute, 0, $month, $day, $year) . "');\">" .
                                                    image("desktop/command_create") . " " . CMS_L_COMMAND_ADD .
                                                    "</a>" .
                                                    "</td>" .
                                                    "</tr>" .
                                                    "<tr>";
                                };
                            };
                        };
                    };
                };
            };
        };
    };
};

//day
for ($i = 1; $i <= 31; $i++)
    $array_day[sprintf("%02d", $i)] = $i;

//month
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

//year
$i = $year - 10;
$c = $i + 20;
if ($i < 1970) $i = 1970;
if ($c > 2037) $c = 2037;
for (; $i <= $c; $i++)
    $array_year[sprintf("%04d", $i)] = $i;

//hour
for ($i = 6; $i < 30; $i++)
    $array_hour[sprintf("%02d", ($i >= 24) ? $i - 24 : $i)] = $i;

//minute
$array_minute =
    ["00" =>  0,
     "15" => 15,
     "30" => 30,
     "45" => 45];

//menu
$menu = NULL;
if ($object)
{
    $menu[CMS_L_COMMAND_SAVE .   "|desktop/command_save"]   = "save";
    $menu[CMS_L_COMMAND_DELETE . "|desktop/command_delete"] = "#delete";
};

//display
$ifc = new ifc(NULL,
               $ifc_page,
               $menu,
               array_merge(
                   $param,
                   ["object" => $object,
                    "time"   => $time]),
               NULL,
               CMS_L_DESKTOP_APPOINTMENT_002);

//year list
ifc_table_open();
echo("<tr>" .
     $buffer_year .
     "</tr>");
ifc_table_close();

//month list
ifc_table_open();
echo("<tr>" .
     $buffer_month .
     "</tr>");
ifc_table_close();

//day list
ifc_table_open();
echo("<tr>" .
     $buffer_day .
     "</tr>" .
     "<tr>" .
     $_buffer_day .
     "</tr>");
ifc_table_close();

echo("<table class=\"layout\" " .
            "style=\"WIDTH:100%\">" .
     "<tr>" .
     "<td>");

//hour list
if ($hour >= 24)
    $time = mktime($hour, $minute, 0, $month, $day - 1, $year);

ifc_table_open();
echo("<colgroup>" .
     "<col span=\"2\" " .
          "style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .
     "<tr>" .
     "<th colspan=\"3\" " .
         "style=\"TEXT-ALIGN:center\">" .
     weekday(date("w", $time)) . ", " . date(CMS_L_DATE_FORMAT, $time) .
     "</th>" .
     "</tr>" .
     "<tr>" .
     substr($buffer_hour, 0, -4));
ifc_table_close();

echo("</td>");

if ($object)
{
    //data
    echo("<td style=\"WIDTH:0\">");

    ifc_table_open();
    echo("<tr>" .
         "<th>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_005, "text 40 40",           $desktop->object_get($object, "name"));
    echo("</th>" .
         "</tr>" .

         "<tr>" .
         "<td class=\"varied\">" .
         "<label class=\"element\">" .
         "<span>" . x(friendly_date($time)) . "</span>" .
         "<table class=\"layout\">" .
         "<tr>" .
         "<td>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_007, "description b");
    $ifc->set($array_day,                    "select",               $day);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_008, "description b");
    $ifc->set($array_month,                  "select",               $month);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_009, "description b");
    $ifc->set($array_year,                   "select",               $year);
    echo("</td>" .
         "<td>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_010, "description b");
    $ifc->set($array_hour,                   "select",               $hour);
    echo("<span>:</span>");
    $ifc->set($array_minute,                 "select",               $minute);
    echo("</td>" .
         "</tr>" .
         "</table>" .
         "</label><br>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_011, "checkbox b",           TRUE, $desktop->object_get($object, "#expire"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_012, "textarea 40x8 1024 b", $desktop->object_get($object, "content"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_006, "textarea 40x4 256 b",  $desktop->object_get($object, "place"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_013, "textarea 40x4 1024",   $desktop->object_get($object, "participant"));
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();

?>