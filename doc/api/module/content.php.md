# PWNC API Documentation

[← Index](../README.md) | [`module/content.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/content.php)

- **Version:** `26.9.7.10`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## module/content.php

The `content.php` module serves as the central request handler for rendering and editing content within the PWNC Web Platform. It manages both **read mode** (public content delivery) and **write mode** (interactive editing), handling tasks such as:

- Custom 404 responses for static assets
- Directory dereferencing and URL resolution
- Read permission verification
- ETag-based caching and HTTP optimization
- Interactive editing command processing (apply, revert, undo, redo)
- Dynamic generation of JavaScript-powered editing actions
- Output buffering and performance timing

It is invoked via the standard PWNC routing mechanism and integrates deeply with the `content`, `template`, `document`, and `interface` libraries.

---

### Global Variables Used

| Variable | Type | Description |
|---------|------|-------------|
| `$content_display` | string | Display mode (e.g., "directory") |
| `$content_index` | int | Unique identifier of the content item |
| `$content_directory_index` | string | Index of the current directory entry |
| `$content_option` | int | Bitmask of active template options |
| `$content_user` | mixed | User context for static editing parameters |
| `$content_message` | string | Command message (apply, revert, undo, redo) |
| `$range` | string | Template path/range being edited |
| `$type` | string | Type of edit operation (e.g., #buffer, #swap) |
| `$value` | mixed | Value associated with the edit |
| `$left`, `$top` | float | Scroll position preservation |
| `$id` | string | Identifier for DOM elements |

---

### Initialization & Library Loading

#### Anonymous Function Wrapper

```php
(function() { ... })();
```

Wraps all logic in an IIFE to avoid polluting the global scope.

#### Timing Start

```php
$time_start = microtime(TRUE);
```

Records the start time for performance measurement.

#### Library Loading

```php
cms_load("content");
cms_load("template");
```

Loads required libraries. If either fails or the content object is disabled, returns a 500 error.

#### Content Object Instantiation

```php
$content = new content($content_user);
```

Creates a content instance tied to the current user. The `enabled` property determines if the module is active.

---

### Static Editing Parameters

When the user has write access (`$content->writer`):

```php
$content_option = (int)$content_option;
cms_param($content_option, "content_option");
cms_param($content_user, "content_user");
```

Stores editing state in the global parameter manager for persistence across requests.

---

### Directory Dereferencing

Handles navigation through logical directories using `directory://` URIs.

#### Reference Resolution Loop

```php
$data = new data("#system/directory");
```

Loads system directory data.

If no index is provided, selects the first entry and redirects accordingly, respecting language gates and homepage rules.

#### URL Analysis and Dereferencing

Uses `analyze_url()` to parse URLs and resolve schemes like `directory://`, `content://`, or external links. Implements loop detection to prevent infinite redirects.

#### Redirect Handling

Performs appropriate HTTP redirects (301 for permanent, 302 for language gate) after resolving the final target URL.

---

### Buffer Copy Section (`#buffer`)

Handles partial content copying triggered by JavaScript.

#### Conditions

- Valid update test (`test_update`)
- Successful database query for buffer text/template
- Document library loaded

#### Process

```php
$document = new document($text, $template);
$document = $document->extract($range);
$buffer = $document->export();
cms_cache("content." . CMS_USER . ".buffer", $buffer, TRUE);
```

Extracts a portion of content, exports it, caches it permanently, and exits.

---

### Read Permission Verification

```php
if (!cms_permission(CMS_CONTENT_PERMISSION_READER . ".$content_index"))
```

Checks if the current user can read the content. If not, redirects to login with preserved query string.

---

### Status Retrieval

Queries the content status from the database:

```php
mysql_query("SELECT ... FROM " . CMS_DB_CONTENT . " WHERE ... LIMIT 1");
```

If not found, returns 404.

---

### Query String Management

Updates global query parameters when this file is directly accessed:

```php
cms_param($content_index, "content_index");
cms_param($content_directory_index, "content_directory_index");
```

Ensures consistent state tracking.

---

### Read Mode (Public Delivery)

Triggered when the user cannot edit the content.

#### Publication Check

```php
if ((int)$resultrow[CMS_DB_CONTENT_STATUS] !== CMS_CONTENT_STATUS_PUBLICATION)
```

Returns 410 Gone if unpublished.

#### Access Logging

```php
$log = new log();
$log->access("viewed", $content_index);
```

Logs view activity.

#### Output Generation

```php
$output = content_parse($content, $content_index, NULL, CMS_BOT_CHECK, $is_dynamic, $mod_time);
```

Parses and generates HTML output.

#### Caching Headers

If not dynamic:

- Sets `Last-Modified` and `ETag` headers
- Checks `If-None-Match` for 304 Not Modified response

#### Performance Timing

Appends generation time comment:

```php
echo("\n<!-- generated in X ms -->");
```

---

### Write Mode (Editing Interface)

Sets up interactive editing environment with JavaScript support.

#### Header Setup

Includes CSS opacity hiding and localization strings:

```php
$header = jscript("...") . "<script src='asr.js'></script><script src='content.js'></script>";
```

#### Command Processing

Handles messages like `apply`, `revert`, `undo`, `redo` via switch statement.

Redirects back after command execution.

#### Range Writing

For non-command edits:

```php
content_set_range($content, $content_index, $range, $type, $value);
```

Persists changes to the specified range.

---

### Action Configuration

Builds `$action` array defining available editing controls based on permissions and options.

#### Option Select Function

```php
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_SWITCH][CMS_TEMPLATE_CODE] = "tp_ctrl_opt_apply(...);";
```

Sets up dropdown for selecting content options.

#### Command List Initialization

```php
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND] = [];
```

Prepares container for command buttons.

#### Apply/Revert/Undo/Redo Buttons

Conditionally adds buttons based on capability tests:

```php
if ($content->test_apply($content_index)) { ... }
if ($content->test_step_undo($content_index)) { ... }
if ($content->test_step_redo($content_index)) { ... }
```

Each button links to a CMS URL with appropriate parameters.

---

### Interface Integrations

Dynamically includes interface modules if available and permitted.

#### Content Interface

Adds edit functions for href, plugin, text, value, switch types:

```php
if ($content_option & CMS_TEMPLATE_OPTION_HREF) { ... }
if ($content_option & CMS_TEMPLATE_OPTION_TEXT) { ... }
```

Each creates a button linking to `interface.php` with specific configuration.

#### Meta Data, Pool, Analyze, Debug

Adds control commands for metadata management, pool operations, text analysis, and debugging info.

#### Download/Image/Media/Template Editors

Integrates with respective modules if available and permitted.

#### Group/Repeat/Shift Actions

Adds specialized editing actions for grouping, repeating, and shifting content blocks.

---

### Clipboard Operations

When `$content_option` is set:

#### Unique ID Generation

```php
$unique_id = "_" . unique_id();
```

Prevents caching of clipboard operations.

#### Copy/Paste/Swap/Kick/Drop Commands

Generates URLs for clipboard operations:

```php
$action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_BUFFER] = "content_edit_copy(...);";
$action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_PASTE] = "content_edit_paste(...);";
```

#### Drag-and-Drop Support

Adds dragdrop1 and dragdrop2 commands for moving content.

#### Conditional/Alternative/Debug Flags

Sets flags for conditional blocks, alternative blocks, and debug visibility.

---

### Interface/Desktop/Logout Links

Adds top-level navigation commands:

```php
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_007] = "content_load(...);";
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_008] = "load_page(...);";
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_015] = "content_load(...);";
```

---

### Final Output Generation

```php
echo(content_parse($content, $content_index, $action, $header));
```

Renders the full page with editing interface.

#### JavaScript Restoration Script

```html
<script>
tp_flp_restore("<?php echo(q($content_index));?>");
fx_event_listen("window_load", tp_event);
content_edit_restore(left, top, "selector");
</script>
```

Restores scroll position and animates changed values.

#### Performance Timing

Appends final generation time comment.

---

### Usage Example

A typical request to edit content might look like:

```
GET /module/content.php?content_index=123&content_option=7&left=0&top=0
```

This would:
1. Load the content library
2. Instantiate the content object
3. Verify write permissions
4. Set up editing actions for text, value, and switch types
5. Render the editable interface with JavaScript support
6. Include performance timing in the output

The resulting page allows users to interactively edit content ranges, apply changes, and navigate between versions using undo/redo functionality.


<!-- HASH:1dd1ca4eaa859d02f292c2fb0024cd2a -->
