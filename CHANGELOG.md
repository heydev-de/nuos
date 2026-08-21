# PWNC Changelog

## Version 26.8.14.2 (2026-08-14 18:39)

﻿* Use ifc_post(...) instead of ifc_submit()

---

## Version 26.8.14.1 (2026-08-14 18:01)

﻿* Refactor IFC interface/JS and add backend JS docs

---

## Version 26.8.14.0 (2026-08-14 02:38)

﻿* Use <button> for IFC controls and fix styles

---

## Version 26.8.13.9 (2026-08-13 15:29)

﻿* Clarify PWNC HTTP and auth protocol
* Expand PWNC discovery and HTTP docs

---

## Version 26.8.13.7 (2026-08-13 12:12)

﻿* Clarify PWNC docs and examples
* Clarify PWNC HTTP interaction docs

---

## Version 26.8.13.5 (2026-08-13 11:18)

﻿* Return capabilities as empty JSON object

---

## Version 26.8.13.4 (2026-08-13 11:09)

﻿* Move serverInfo _meta into result of MCP init

---

## Version 26.8.13.3 (2026-08-13 09:53)


---

## Version 26.8.13.2 (2026-08-13 09:52)

﻿* Add MCP protocol support
* Return JSON-RPC 'Method not found' response
* Update Link header and remove X-WebMCP flag
* Use CMS_ACTIVE_URL for Link header

---

## Version 26.8.12.0 (2026-08-12 08:27)

﻿* Refactor loading animation and small JS fixes

---

## Version 26.8.11.2 (2026-08-11 21:03)

﻿* Fix mailform cache initialization

---

## Version 26.8.11.1 (2026-08-11 19:02)

﻿* Use blank() and ?? for input checks

---

## Version 26.8.11.0 (2026-08-11 05:56)


---

## Version 26.8.9.1 (2026-08-11 05:56)

﻿* Use cms_cache_init for object initialization

---

## Version 26.8.9.1 (2026-08-09 23:43)

﻿* Replace empty() with blank(); fix token rename error
* Replace isset ternaries with ?? and minor cleanup

---

## Version 26.8.8.0 (2026-08-08 21:40)

﻿* Refactor input checks, SQL escape, and template fixes

---

## Version 26.8.7.1 (2026-08-07 10:31)

﻿* Replace empty() with blank() and fix save errors

---

## Version 26.8.7.0 (2026-08-07 01:44)

﻿* Use null coalescing and nstre for defaults

---

## Version 26.8.6.0 (2026-08-06 06:31)


---

## Version 26.8.5.0 (2026-08-06 06:19)

﻿* Avoid undefined IFC variable notices

---

## Version 26.8.5.0 (2026-08-05 21:21)

﻿* Fix image preview sizing and daemon scheduling

---

## Version 26.8.4.7 (2026-08-04 22:16)

﻿* Return empty string for disabled users in permission check

---

## Version 26.8.4.6 (2026-08-04 21:36)

﻿* Guard header values with nstre() check

---

## Version 26.8.4.5 (2026-08-04 21:11)


---

## Version 26.8.4.4 (2026-08-04 20:50)

﻿* Fix RFC 2231 parameter handling and header folding

---

## Version 26.8.4.4 (2026-08-04 05:09)

﻿* Add interface analyses; update ifc.database & languages
* Avoid value overwrite; rename chars/page
* Cast bit length, store offset, fix header var
* Fix database UI field quoting and ordering
* Refactor edit_table field/order handling and cache
* Refactor profile & search interfaces
* Refactor profile filter vars and add ordering

---

## Version 26.7.31.0 (2026-07-31 01:23)

﻿* Fallback to email when name is empty

---

## Version 26.7.30.4 (2026-07-30 14:40)


---

## Version 26.7.29.7 (2026-07-29 23:09)

﻿* Escape backslashes in hex/regex literals

---

## Version 26.7.29.6 (2026-07-29 20:49)

﻿* Default missing host when calling punycode
* Use empty string fallback for preg_match scope

---

## Version 26.7.29.4 (2026-07-29 19:57)


---

## Version 26.7.29.3 (2026-07-29 19:56)

﻿* force_flush: set headers and sanitize output

---

## Version 26.7.29.2 (2026-07-29 14:32)

﻿* Add addressbook links and mailbox UI improvements
* Add header folding and fix RFC2822 param wrapping
* Add MIME helpers and improve encoding handling
* Add RFC2822 param builder and refactor header encoding
* Decode MIME headers and bodies to UTF-8
* Default mailbox subject to empty string
* Fix header line length calculation in mime builder
* Fixed ifc
* Implement line-folding in mime_build_rfc2822_address
* Improve MIME and POP parsing/encoding
* Improve MIME header encoding with fill-level aware folding
* Improve MIME RFC2047 decode/encode and quoting
* Improve RFC2047/RFC2822 MIME header encoding
* Refactor mailbox path handling and fix POP parsing
* Refactor MIME header and message construction
* Refactor MIME header encoding/decoding
* Simplify MIME header encoding logic
* Update lib.mime.inc

---

## Version 26.7.23.2 (2026-07-23 10:40)

﻿* Add null coalescing to data accessor calls
* Simplify null-coalescing and direct indexing
* Use strict comparisons and type safety

---

## Version 26.7.22.1 (2026-07-22 04:32)

﻿* Improve remote image cache cooldown

---

## Version 26.7.21.3 (2026-07-21 16:35)

﻿* Ensure url variable has default value

---

## Version 26.7.21.2 (2026-07-21 16:30)


---

## Version 26.7.7.2 (2026-07-07 19:05)

﻿* Fix user verification logic in desktop module

---

## Version 26.7.7.1 (2026-07-07 04:29)

﻿* Refine encode_filename truncation and encoding

---

## Version 26.7.6.3 (2026-07-07 00:32)

﻿* Add filename encode/decode and mailbox fixes
* Normalize mailbox paths and refactor training
* removed misplaced semicolon

---

## Version 26.7.6.1 (2026-07-06 03:39)

﻿* Fix parent index restoration and each() return handling
* Null-safe check for auto-id attribute

---

## Version 26.7.5.5 (2026-07-05 21:05)

﻿* Add user input interruption to scroll animation
* Make sodium extension optional
* Normalize zoom calculation by initial pointer delta
* Normalize zoom wheel delta increments
* Smooth-scroll fx_scrollto and tp_flp fix

---

## Version 26.7.4.2 (2026-07-04 17:53)

﻿* Replace className regex with classList.contains()

---

## Version 26.7.4.1 (2026-07-04 17:37)

﻿* Improve image processing validation
* Replace type casting with null coalescing operator

---

## Version 26.7.3.0 (2026-07-03 20:20)

﻿* Replace date dropdowns with datetime-local input

---

## Version 26.7.3.0 (2026-07-03 15:04)


---

## Version 26.6.30.3 (2026-07-03 15:01)

﻿* Update rss.php

---

## Version 26.6.30.3 (2026-06-30 16:34)

﻿* Update desktop.mailbox.inc

---

## Version 26.6.30.2 (2026-06-30 16:12)

﻿* Update desktop.mailbox.inc

---

## Version 26.6.30.1 (2026-06-30 16:03)


---

## Version 26.6.30.0 (2026-06-30 16:03)

﻿* Update desktop.mailbox.inc

---

## Version 26.6.30.0 (2026-06-30 13:47)

﻿* Refactor HTTP chunked read handling

---

## Version 26.6.29.3 (2026-06-29 23:34)

﻿* Handle multiple HTTP headers correctly

---

## Version 26.6.29.2 (2026-06-29 23:06)

﻿* Add HTTP chunked transfer encoding support

---

## Version 26.6.29.0 (2026-06-29 18:46)

﻿* Use array callbacks for XML parser handlers

---

## Version 26.6.29.0 (2026-06-29 10:38)

﻿* Fix menu image CSS classes

---

## Version 26.6.28.1 (2026-06-28 15:28)

﻿* Update lib.content.inc

---

## Version 26.6.28.0 (2026-06-28 14:56)

﻿* Refactor flexview and simplify checkbox styling

---

## Version 26.6.27.3 (2026-06-27 19:11)

﻿* Update download.php

---

## Version 26.6.27.2 (2026-06-27 16:36)

﻿* Guard against null inputs in permission handling

---

## Version 26.6.27.1 (2026-06-27 15:14)

﻿* Return affected paths from filemanager ops

---

## Version 26.6.27.0 (2026-06-27 01:59)


---

## Version 26.6.26.1 (2026-06-27 01:45)

﻿* Update lib.mime.inc

---

## Version 26.6.26.1 (2026-06-26 10:50)

﻿* Add email validation, punycode, CSP checking, and proxy trust headers
* Fix CSP cache logic and add exception handler

---

## Version 26.6.25.0 (2026-06-25 08:18)

﻿* UI improvements, deprecated bug fixes

---

## Version 26.6.24.4 (2026-06-24 18:56)

﻿* Disable auto-correct, fix highlighter & CSS

---

## Version 26.6.24.3 (2026-06-24 15:12)

﻿* Update ifc.insert.inc

---

## Version 26.6.24.2 (2026-06-24 12:43)

﻿* Consolidate dev-mode checks in cms_error

---

## Version 26.6.24.1 (2026-06-24 08:39)

﻿* Use content_load instead of load_page

---

## Version 26.6.24.0 (2026-06-24 07:45)

﻿* UI tweaks: buttons, inputs, permissions, mailbox

---

## Version 26.6.23.8 (2026-06-23 19:05)

﻿* Add reference code to mailform

---

## Version 26.6.23.7 (2026-06-23 18:05)

﻿* Replace ifc_param with object; rename loader

---

## Version 26.6.23.6 (2026-06-23 14:28)

﻿* Normalize time handling in friendly_date

---

## Version 26.6.23.5 (2026-06-23 13:28)

﻿* Handle null query in parse_str

---

## Version 26.6.23.4 (2026-06-23 13:14)

﻿* Round and cast captcha image offsets

---

## Version 26.6.23.3 (2026-06-23 11:36)


---

## Version 26.6.23.2 (2026-06-23 11:29)

﻿* fixed proplem in mobile browsers with custom select boxes
* Update lib.flexview.inc

---

## Version 26.6.23.1 (2026-06-23 01:16)


---

## Version 26.6.23.0 (2026-06-23 01:09)

﻿* Avoid undefined $base_id in nocache buffer

---

## Version 26.6.23.0 (2026-06-23 00:25)

﻿* Enable noscript content display and compact CSS

---

## Version 26.6.22.0 (2026-06-22 01:41)

﻿* Add PHP 8 compatibility and null-safe URL handling

---

## Version 26.6.21.8 (2026-06-21 22:28)

﻿* Update mod.mailform.inc

---

## Version 26.6.21.8 (2026-06-21 22:11)

﻿* Update lib.directory.inc

---

## Version 26.6.21.7 (2026-06-21 21:37)


---

## Version 26.6.21.7 (2026-06-21 15:03)

﻿* Update pwnc.inc

---

## Version 26.6.21.6 (2026-06-21 14:45)

﻿* Update lib.content.inc

---

## Version 26.6.21.5 (2026-06-21 14:36)

﻿* Update lib.rss_parser.inc

---

## Version 26.6.21.4 (2026-06-21 14:24)

﻿* Update lib.rss_parser.inc

---

## Version 26.6.21.3 (2026-06-21 14:09)

﻿* Update sys.permission.inc

---

## Version 26.6.21.2 (2026-06-21 13:47)

﻿* Return NULL for empty byte and fix typo

---

## Version 26.6.21.1 (2026-06-21 11:14)


---

## Version 26.6.20.0 (2026-06-20 23:58)

﻿* Replace static password salt with cms_salt

---

## Version 26.6.19.3 (2026-06-19 23:54)

﻿* Refactor user ID generation and cookie handling

---

## Version 26.6.19.2 (2026-06-19 10:17)

﻿* Convert password to UTF-8 and fix JS hashing
* Fix date, hash, math, text, and JS hash bugs

---

## Version 26.6.19.0 (2026-06-19 03:10)

﻿* Fix HTML entity decoding, email regex and utf8 check
* Fix string equality and template asset return

---

## Version 26.6.17.1 (2026-06-17 06:47)

﻿* Use explicit index.php in CMS root URLs

---

## Version 26.6.17.0 (2026-06-17 06:17)


---

## Version 26.6.16.1 (2026-06-17 06:12)

﻿* Fix directory path handling and deletion checks

---

## Version 26.6.16.1 (2026-06-16 22:15)

﻿* Refactor content caching and file locking
* Treat .php URLs as executable when building URLs

---

## Version 26.6.15.2 (2026-06-15 20:02)

﻿* Update ifc.css

---

## Version 26.6.15.1 (2026-06-15 04:11)

﻿* Update ifc.css

---

## Version 26.6.15.0 (2026-06-15 01:22)

﻿* Add move, improve unzip, and cleanup filemanager
* Filemanager: add zip compress, refactor and i18n
* Filemanager: add zip/unzip, unique-name handling
* Filemanager: refactor recent/edited and selection
* Harden DB checks, UI tweaks & filemanager fixes
* Refactor filemanager_delete to handle lists

---

## Version 26.6.9.0 (2026-06-09 05:55)

﻿* Refactor appointment UI, add text delete button

---

## Version 26.6.8.1 (2026-06-08 18:04)

﻿* Fix escaped '|' handling and update examples
* Update .htaccess

---

## Version 26.6.6.1 (2026-06-06 22:13)

﻿* Add CMS flag helpers and migration flow

---

## Version 26.6.6.0 (2026-06-06 18:29)

﻿* update to photoswipe 5.4.4

---

## Version 26.6.4.0 (2026-06-06 18:18)


---

## Version 26.6.4.0 (2026-06-04 16:25)


---

## Version 26.6.3.4 (2026-06-04 16:24)

﻿* Add explicit width/height to textcontrol SVGs

---

## Version 26.6.3.4 (2026-06-03 17:01)

﻿* Use $__delimiter consistently in CONCAT call

---

## Version 26.6.3.3 (2026-06-03 13:21)

﻿* Update ifc.css

---

## Version 26.6.3.1 (2026-06-03 13:13)

﻿* Remove PHP closing tags and tidy whitespace
* Use djb2 for coloring and namespace refs

---

## Version 26.6.2.3 (2026-06-02 15:18)

﻿* Pass NULL and TRUE to cms_url call

---

## Version 26.6.2.2 (2026-06-02 13:37)

﻿* Initialize directory_object and pass to templates

---

## Version 26.6.2.1 (2026-06-02 13:08)

﻿* lil notice fix in ifc.content.inc

---

## Version 26.6.2.0 (2026-06-02 12:03)

﻿* fixed parameter bug in cms_cache_init

---

## Version 26.6.1.2 (2026-06-01 21:59)

﻿* Colorize option/select labels by value

---

## Version 26.6.1.1 (2026-06-01 17:41)

﻿* Fix directory, icon parsing and regex examples

---

## Version 26.6.1.0 (2026-06-01 13:54)

﻿* Refactor directory/interface components and fixes

---

## Version 26.5.31.2 (2026-05-31 10:38)

﻿* Update index.php

---

## Version 26.5.31.1 (2026-05-31 10:16)

﻿* Require MySQLi extension only

---

## Version 26.5.30.5 (2026-05-31 06:45)


---

## Version 26.5.30.4 (2026-05-30 22:03)

﻿* Update lib.update.inc

---

## Version 26.5.30.4 (2026-05-30 21:58)


---

## Version 26.5.30.2 (2026-05-30 09:18)

﻿* Optimized SVG

---

## Version 26.5.30.1 (2026-05-30 08:41)


---

## Version 26.5.30.0 (2026-05-30 08:39)

﻿* Add desktop data; rebuild dirs during update

---

## Version 26.5.29.0 (2026-05-29 13:19)

﻿* Remove !important and fix CSS nesting

---

## Version 26.5.28.2 (2026-05-28 22:12)

﻿* Use IFC tab functions and fix CSS height

---

## Version 26.5.24.2 (2026-05-24 15:39)

﻿* Update lib.ifc.inc

---

## Version 26.5.24.1 (2026-05-24 04:51)

﻿* Add translation parsing and update UTF-8 maps
* Integrated language_selector() where applicable

---

## Version 26.5.22.1 (2026-05-22 12:46)

﻿* Normalize buffer init and strict comparisons in MIME
* Update ifc.css

---

## Version 26.5.21.1 (2026-05-21 01:16)

﻿* Simplify directory-entry checks in filemanager

---

## Version 26.5.21.0 (2026-05-21 00:50)

﻿* Update ifc.css

---

## Version 26.5.20.3 (2026-05-20 23:45)

﻿* Refine access log deduplication check

---

## Version 26.5.20.2 (2026-05-20 06:55)

﻿* Fix LINE-HEIGHT typo and add select styling

---

## Version 26.5.20.1 (2026-05-20 06:20)

﻿* Refactor IFC input handlers and radio labels
* Refactor IFC label parsing and mailbox UI

---

## Version 26.5.18.4 (2026-05-18 16:28)

﻿* Use pagination() helper in interface modules

---

## Version 26.5.18.3 (2026-05-18 15:19)

﻿* Add content info popover and update icons/SVGs
* Update ifc.css

---

## Version 26.5.18.0 (2026-05-18 00:33)

﻿* Add pinned icon and tweak active/related styling

---

## Version 26.5.17.3 (2026-05-17 23:55)

﻿* Scope content cache to user; update folder icons

---

## Version 26.5.17.2 (2026-05-17 22:29)

﻿* Add type='button' and extract onchange handlers

---

## Version 26.5.17.1 (2026-05-17 22:13)

﻿* Refactor content interface state and controls

---

## Version 26.5.17.0 (2026-05-17 19:33)

﻿* Enhance popover structure and styling

---

## Version 26.5.16.3 (2026-05-16 18:29)

﻿* Add datetime inputs, popover and content refactors
* Content UI: href filter, access checks, layout
* Migrate MySQL wrapper to mysqli & refactor pool
* Refactor content UI, URL defaults, caching and CSS
* Refactor parameter checks and add cache init
* Replace $this->mysql with local connections
* Update ifc.css

---

## Version 26.5.11.6 (2026-05-11 17:15)

﻿* Combine bitmask and isset checks in template
* Use SVG no_image background instead of PNG

---

## Version 26.5.11.4 (2026-05-11 15:25)

﻿* Replace empty.svg with empty.png

---

## Version 26.5.11.3 (2026-05-11 15:06)

﻿* Fix event callback iteration and removal
* Use consistent id ifc-loading

---

## Version 26.5.11.1 (2026-05-11 11:51)

﻿* Move header output after JS includes

---

## Version 26.5.11.0 (2026-05-11 11:19)

﻿* Pass object to version_display and set index

---

## Version 26.5.10.1 (2026-05-10 18:45)

﻿* Update sys.data.inc

---

## Version 26.5.10.0 (2026-05-10 17:47)

﻿* Add JavaScript asset support to templates
* Refactor template asset management & errors

---

## Version 26.5.8.2 (2026-05-08 17:20)

﻿* Embed CAPTCHA as data URI; update callers
* Update pwnc.inc

---

## Version 26.5.8.0 (2026-05-08 04:53)

﻿* Run cache cleanup via daemons

---

## Version 26.5.6.0 (2026-05-06 04:50)

﻿* Remove extra space in IMG format placeholder

---

## Version 26.5.5.1 (2026-05-05 15:15)

﻿* Use classList.contains and prevent self-drop

---

## Version 26.5.5.0 (2026-05-05 10:08)

﻿* Use plaintext-only editable and fix selection saving

---

## Version 26.5.4.1 (2026-05-04 01:39)

﻿* Increase note textarea max length to 102400
* Normalize MIME handling and sort mailbox list

---

## Version 26.5.2.6 (2026-05-02 22:43)

﻿* Remove maxlength param from search input

---

## Version 26.5.2.4 (2026-05-02 19:43)

﻿* minor style change

---

## Version 26.5.2.4 (2026-05-02 19:33)

﻿* fixed image size in search backend

---

## Version 26.5.2.3 (2026-05-02 07:25)


---

## Version 26.5.2.2 (2026-05-02 07:13)

﻿* fixed image file name

---

## Version 26.4.30.0 (2026-04-30 00:22)

﻿* Refactor ref handling, remove icons & update CSS

---

## Version 26.4.29.1 (2026-04-29 18:07)

﻿* Use fx_window offsets in fx_offset functions

---

## Version 26.4.29.0 (2026-04-29 00:54)

﻿* fixed bug in directory filesystem removal

---

## Version 26.4.24.1 (2026-04-24 15:34)

﻿* Use .dd-active class for drag/drop styling

---

## Version 26.4.24.0 (2026-04-24 02:49)


---

## Version 26.4.22.1 (2026-04-22 02:13)

﻿* changes style of bracket highlighting

---

## Version 26.4.22.0 (2026-04-22 01:08)

﻿* added active bracket highlighting to code editor

---

## Version 26.4.16.0 (2026-04-16 23:46)

﻿* fixed bug in template.css

---

