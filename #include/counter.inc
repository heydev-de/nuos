<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

if (! defined("CMS_CONTENT_INDEX")) //not integrated
{
    echo(sprintf(parse_token("%%l_views%%"), 0));
    return;
};

$path   = CMS_DATA_PATH . "#counter/";        //counter file directory
if (! mkpath($path)) return;                  //verify path
$path  .= hash32(CMS_CONTENT_INDEX) . ".txt"; //current counter file
$count  = 0;                                  //current count

//open file and get counter value
if (is_file($path) && ($hfile = fopen($path, "rb")))
{
    $count = (int)fgets($hfile);
    fclose($hfile);
};

//increase and save counter value if not editing
if ((! CMS_TEMPLATE_EDIT) && ($hfile = fopen($path, "wb")))
{
    fwrite($hfile, ++$count);
    fclose($hfile);
};

//display counter value
echo(sprintf(parse_token("%%l_views%%"), $count));

?>