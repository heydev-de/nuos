<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

require("nuos.inc");

(function() {

    header("Location: " . cms_url(CMS_MODULES_URL . "desktop.php"), TRUE, 303);
    exit();

})();

?>