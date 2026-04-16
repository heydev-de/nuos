/*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

self.addEventListener("fetch", (event) =>
{
    event.respondWith(fetch(event.request));
});