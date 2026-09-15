# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.template.inc)

- **Version:** `26.9.9.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Template System Library

The `#system/lib.template.inc` file is the core of the PWNC template engine. It provides a comprehensive system for defining, parsing, and rendering templates using custom `<CMS:...>` tags. The template engine supports content management, editing controls, asset management (stylesheets, JavaScript, code), conditional blocks, repeat/shift structures, namespaces, and caching.

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | `"operator"` | Permission level required for template management operations |

### Cache Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_CACHE_SEPARATOR` | `"\x1C"` | ASCII File Separator used to delimit cached template segments |

### Option Flag Constants

These flags are used to determine which editing options are available. They are combined using bitwise OR.

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_OPTION_NONE` | `0` | No options |
| `CMS_TEMPLATE_OPTION_HREF` | `1` | Href editing option |
| `CMS_TEMPLATE_OPTION_PLUGIN` | `2` | Plugin editing option |
| `CMS_TEMPLATE_OPTION_TEXT` | `4` | Text editing option |
| `CMS_TEMPLATE_OPTION_VALUE` | `8` | Value editing option |
| `CMS_TEMPLATE_OPTION_DOWNLOAD` | `16` | Download editing option |
| `CMS_TEMPLATE_OPTION_IMAGE` | `32` | Image editing option |
| `CMS_TEMPLATE_OPTION_THUMBNAIL` | `64` | Thumbnail editing option |
| `CMS_TEMPLATE_OPTION_MEDIA` | `128` | Media editing option |
| `CMS_TEMPLATE_OPTION_TEMPLATE` | `256` | Template editing option |
| `CMS_TEMPLATE_OPTION_GROUP` | `512` | Group editing option |
| `CMS_TEMPLATE_OPTION_REPEAT` | `1024` | Repeat editing option |
| `CMS_TEMPLATE_OPTION_SHIFT` | `2048` | Shift editing option |
| `CMS_TEMPLATE_OPTION_CALT` | `4096` | Conditional alternative editing option |
| `CMS_TEMPLATE_OPTION_CBLOCK` | `8192` | Conditional block editing option |
| `CMS_TEMPLATE_OPTION_DEBUG` | `16384` | Debug mode option |
| `CMS_TEMPLATE_OPTION_SWITCH` | `32768` | Switch editing option |
| `CMS_TEMPLATE_OPTION_LAYOUT` | Composite | Layout-related options (VALUE \| TEMPLATE \| GROUP \| REPEAT \| SHIFT \| CBLOCK \| SWITCH) |
| `CMS_TEMPLATE_OPTION_EDIT` | Composite | Edit-related options (HREF \| PLUGIN \| TEXT \| DOWNLOAD \| IMAGE \| THUMBNAIL \| MEDIA \| CBLOCK) |
| `CMS_TEMPLATE_OPTION_ALL` | Composite | All options (LAYOUT \| EDIT) |

### Action Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_ACTION` | `0` | Action type for element actions |
| `CMS_TEMPLATE_CONTROL` | `1` | Control type for control panel |
| `CMS_TEMPLATE_CODE` | `2` | Code type for code references |
| `CMS_TEMPLATE_IMAGE` | `3` | Image type for image references |
| `CMS_TEMPLATE_COMMAND` | `4` | Command type for command references |
| `CMS_TEMPLATE_SWITCH` | `5` | Switch type for switch references |

### Type Constants

These define the types of template elements. They are used as bit flags.

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_TYPE_NONE` | `0` | No type |
| `CMS_TEMPLATE_TYPE_HEAD` | `1` | Head element (meta, title, base, stylesheet) |
| `CMS_TEMPLATE_TYPE_HREF` | `2` | Hyperlink element |
| `CMS_TEMPLATE_TYPE_PLUGIN` | `4` | Plugin element |
| `CMS_TEMPLATE_TYPE_TEXT` | `8` | Text element |
| `CMS_TEMPLATE_TYPE_VALUE` | `16` | Value display element |
| `CMS_TEMPLATE_TYPE_DOWNLOAD` | `32` | Download link element |
| `CMS_TEMPLATE_TYPE_IMAGE` | `64` | Image element |
| `CMS_TEMPLATE_TYPE_THUMBNAIL` | `128` | Thumbnail element |
| `CMS_TEMPLATE_TYPE_MEDIA` | `256` | Media element |
| `CMS_TEMPLATE_TYPE_TEMPLATE` | `512` | Sub-template element |
| `CMS_TEMPLATE_TYPE_GROUP` | `1024` | Group container element |
| `CMS_TEMPLATE_TYPE_REPEAT` | `2048` | Repeat container element |
| `CMS_TEMPLATE_TYPE_SHIFT` | `4096` | Shift container element |
| `CMS_TEMPLATE_TYPE_MENU` | `8192` | Menu element |
| `CMS_TEMPLATE_TYPE_CBLOCK` | `16384` | Conditional block element |
| `CMS_TEMPLATE_TYPE_CALT` | `32768` | Conditional alternative element |
| `CMS_TEMPLATE_TYPE_BASE` | `65536` | Base path element |
| `CMS_TEMPLATE_TYPE_NAMESPACE` | `131072` | Namespace element |
| `CMS_TEMPLATE_TYPE_NOCACHE` | `262144` | No-cache element |
| `CMS_TEMPLATE_TYPE_CONTROL` | `524288` | Control panel element |
| `CMS_TEMPLATE_TYPE_BACKLINK` | `1048576` | Backlink element |
| `CMS_TEMPLATE_TYPE_DEBUG` | `2097152` | Debug element |
| `CMS_TEMPLATE_TYPE_STYLESHEET` | `4194304` | Stylesheet reference element |
| `CMS_TEMPLATE_TYPE_JAVASCRIPT` | `67108864` | JavaScript reference element |
| `CMS_TEMPLATE_TYPE_SWITCH` | `8388608` | Switch element |
| `CMS_TEMPLATE_TYPE_CEDIT` | `16777216` | Edit block element |
| `CMS_TEMPLATE_TYPE_CNOEDIT` | `33554432` | No-edit block element |
| `CMS_TEMPLATE_TYPE_ALL` | `4294967295` | All types |

### Type Filter Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_TYPE_EDIT` | Composite | Types that are editable (HREF \| PLUGIN \| TEXT \| VALUE \| DOWNLOAD \| IMAGE \| THUMBNAIL \| MEDIA \| TEMPLATE \| GROUP \| REPEAT \| SHIFT \| SWITCH) |
| `CMS_TEMPLATE_TYPE_SPAN` | Composite | Types that are spannable (HREF \| DOWNLOAD \| GROUP \| REPEAT \| SHIFT \| CBLOCK \| CALT \| BASE \| NAMESPACE \| NOCACHE \| CEDIT \| CNOEDIT) |
| `CMS_TEMPLATE_TYPE_PATH` | Composite | Types that extend the path (TEMPLATE \| GROUP \| REPEAT \| SHIFT \| BASE \| NAMESPACE) |

### Command Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_COMMAND_NONE` | `0` | No command |
| `CMS_TEMPLATE_COMMAND_BUFFER` | `1` | Buffer command |
| `CMS_TEMPLATE_COMMAND_PASTE` | `2` | Paste command |
| `CMS_TEMPLATE_COMMAND_SWAP` | `4` | Swap command |
| `CMS_TEMPLATE_COMMAND_KICK1` | `8` | Kick1 command |
| `CMS_TEMPLATE_COMMAND_KICK2` | `16` | Kick2 command |
| `CMS_TEMPLATE_COMMAND_DROP1` | `32` | Drop1 command |
| `CMS_TEMPLATE_COMMAND_DROP2` | `64` | Drop2 command |
| `CMS_TEMPLATE_COMMAND_RELEASE` | `128` | Release command |
| `CMS_TEMPLATE_COMMAND_REFERENCE` | `256` | Reference command |
| `CMS_TEMPLATE_COMMAND_EXPORT` | `512` | Export command |
| `CMS_TEMPLATE_COMMAND_CLEAR` | `1024` | Clear command |
| `CMS_TEMPLATE_COMMAND_DRAGDROP1` | `2048` | Drag-drop move command |
| `CMS_TEMPLATE_COMMAND_DRAGDROP2` | `4096` | Drag-drop duplicate command |
| `CMS_TEMPLATE_COMMAND_ALL` | `4294967295` | All commands |

### Structure Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_STRUCTURE_INDEX` | `0` | Structure field: element index |
| `CMS_TEMPLATE_STRUCTURE_PATH` | `1` | Structure field: element path |
| `CMS_TEMPLATE_STRUCTURE_PARENT` | `2` | Structure field: parent index |
| `CMS_TEMPLATE_STRUCTURE_TYPE` | `3` | Structure field: element type |

### Asset Type Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_ASSET_TYPE_CODE` | `1` | Template code asset |
| `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET` | `2` | Stylesheet asset |
| `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT` | `3` | JavaScript asset |

### JSON Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_JSON_FLAG` | Composite | JSON encoding flags: `JSON_HEX_TAG \| JSON_UNESCAPED_SLASHES \| JSON_UNESCAPED_UNICODE` |

## Related Functions

### template_get_array

Retrieves a hierarchical array of all available templates, organized by category.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `NULL`, `TRUE`, `FALSE` | Filter by page templates. `NULL` = no filter, `TRUE` = only page templates, `FALSE` = only non-page templates |

**Returns:** `array` — A nested array where the first level key is the category, the second level key is the template name (with disambiguation suffixes for duplicates), and the value is the template index/key.

**Inner Mechanisms:** Loads the `#system/template` data store, iterates through all templates, optionally filters by the `page` field, and builds a category → name → index mapping. Duplicate names within a category are disambiguated with ` (1)`, ` (2)`, etc. suffixes. The result is sorted recursively using natural, case-insensitive ordering.

**Usage Example:**
```php
// Get all templates
$templates = template_get_array();
// Result: ["Content" => ["Home" => "home_tpl", "About" => "about_tpl"], ...]

// Get only page templates
$page_templates = template_get_array(TRUE);
```

### template_get_select

Retrieves a flat array of template categories for use in select dropdowns.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `NULL`, `TRUE`, `FALSE` | Filter by page templates |

**Returns:** `array` — An array where both keys and values are category names, suitable for select dropdowns.

**Inner Mechanisms:** Similar to `template_get_array` but only collects unique category names. The result is sorted using natural, case-insensitive ordering.

**Usage Example:**
```php
$categories = template_get_select();
// Result: ["Content" => "Content", "Layout" => "Layout", ...]
```

### template_get_attribute

Extracts attributes from a CMS template tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The full CMS tag string (e.g., `<CMS:text id="content.body"/>`) |
| `$name` | `NULL`, `string` | If `NULL`, returns all attributes as an array. If a string, returns the value of that specific attribute. |

**Returns:** `array` or `string` or `NULL` — If `$name` is `NULL`, returns an associative array of all attributes (lowercased keys). If `$name` is a string, returns the attribute value or `NULL` if not found.

**Inner Mechanisms:** Uses a regular expression to match attribute name-value pairs within the tag string. Supports double-quoted, single-quoted, and unquoted values. When `$name` is `NULL`, all attributes are returned. When `$name` is provided, only that attribute's value is returned. Attribute names are lowercased for case-insensitive matching.

**Usage Example:**
```php
$tag = '<CMS:text id="content.body" default="Hello" edit="off"/>';
$attrs = template_get_attribute($tag);
// Returns: ["id" => "content.body", "default" => "Hello", "edit" => "off"]

$value = template_get_attribute($tag, "default");
// Returns: "Hello"
```

### template_set_attribute

Sets or updates an attribute in a CMS template tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The CMS tag string |
| `$name` | `string` | The attribute name to set |
| `$value` | `string` | The attribute value to set |

**Returns:** `string` — The modified tag string with the attribute set.

**Inner Mechanisms:** First removes any existing attribute with the same name using `template_remove_attribute`, then appends the new attribute to the opening tag using a regex callback that matches the tag name.

**Usage Example:**
```php
$tag = '<CMS:text id="content.body"/>';
$tag = template_set_attribute($tag, "default", "Hello World");
// Result: '<CMS:text id="content.body" default="Hello World"/>'
```

### template_remove_attribute

Removes an attribute from a CMS template tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The CMS tag string |
| `$name` | `NULL`, `string` | If `NULL`, removes all attributes. If a string, removes only that attribute. |

**Returns:** `string` — The modified tag string with the attribute(s) removed.

**Inner Mechanisms:** Uses a regular expression to match and remove the specified attribute (including its value) from the tag string. When `$name` is `NULL`, matches any attribute name.

**Usage Example:**
```php
$tag = '<CMS:text id="content.body" default="Hello"/>';
$tag = template_remove_attribute($tag, "default");
// Result: '<CMS:text id="content.body"/>'
```

### template_parse_reference

Parses a reference value (directory://, content://, address://) and resolves it to a structured array.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The reference string to parse |
| `$analyze` | `bool` | If `TRUE`, returns the raw analyzed URL array instead of the processed result |

**Returns:** `array` or `FALSE` — An array with keys `name`, `description`, and `url`, or `FALSE` if the reference is invalid.

**Inner Mechanisms:** First handles deprecated reference formats (`directory:`, `content:`, `address:`). Then uses `analyze_url()` to parse the reference. Based on the scheme (`directory`, `content`, `address`), it retrieves the name and description from the appropriate data source (directory data store, content database, or none for address). The URL is resolved using `translate_url()`.

**Usage Example:**
```php
$ref = template_parse_reference("content://123");
// Returns: ["name" => "Page Title", "description" => "Page description", "url" => "https://example.com/page"]
```

### template_read_plugin

Reads plugin content from a remote URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The URL to fetch plugin content from |

**Returns:** `string` or `FALSE` — The fetched content, or `FALSE` on failure.

**Inner Mechanisms:** Loads the HTTP library, opens the URL using `http_fopen()`, checks the HTTP status code (rejects 300+), and fetches the data using `http_fetch_data()`.

**Usage Example:**
```php
$content = template_read_plugin("https://example.com/plugin.php");
if ($content !== FALSE) {
    echo $content;
}
```

### template_preview

Generates a preview of a template, either by index or by raw code.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index_or_code` | `string` | Template index (if `$is_index` is `TRUE`) or raw template code |
| `$is_index` | `bool` | Whether `$index_or_code` is a template index |
| `$document` | `document` or `NULL` | Document object to use; if `NULL`, a new one is created |
| `$inert` | `bool` | If `TRUE`, wraps output in an inert preview container |

**Returns:** `void` — Outputs the preview directly.

**Inner Mechanisms:** Defines dummy action data for all template types, creates a document if not provided, sets the `CMS_TEMPLATE_PREVIEW` constant, and either parses a template by index or embeds raw code in a simple HTML page frame. The output is optionally wrapped using `preview_inert()`.

**Usage Example:**
```php
// Preview a template by index
template_preview("home_page", TRUE);

// Preview raw template code
template_preview('<CMS:text id="content.body"/>', FALSE);
```

### template_error

Handles template-related errors by formatting and forwarding them to the CMS error handler.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$code` | `int` | Error type code |
| `$message` | `string` | Error message |
| `$path` | `string` or `NULL` | File path (unused, always set to "embedded PHP script") |
| `$line` | `int` or `NULL` | Line number offset |

**Returns:** `mixed` — The result of `cms_error()`.

**Inner Mechanisms:** Sets the path to "embedded PHP script", looks up the template name from the `#system/template` data store using the global `template_error_index`, constructs a language-specific file path, and calls `cms_error()` with the adjusted line number.

**Usage Example:**
```php
// Called internally by the template engine when PHP eval errors occur
template_error(E_ERROR, "Undefined variable: foo", NULL, 5);
```

## template Class

The main template engine class that handles parsing, rendering, exporting, and structuring of templates.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Data store instance for `#system/template` |
| `$operator` | `bool` | Whether the current user has operator permission |
| `$compat_mode` | `bool` | Compatibility mode flag for older template versions |
| `$tlist` | `array` | Token-to-type mapping (e.g., `"head"` → `CMS_TEMPLATE_TYPE_HEAD`) |
| `$tname` | `array` | Type-to-name mapping for display labels |
| `$toption` | `array` | Type-to-option flag mapping |
| `$tcompat` | `array` | Type compatibility mapping (e.g., VALUE ↔ SWITCH, IMAGE ↔ THUMBNAIL) |
| `$olist` | `array` | Option list for editing controls |
| `$clist` | `array` | Command list for editing controls |
| `$action` | `array` | Action definitions for template elements |
| `$image` | `image` | Image extension instance |
| `$media` | `media` | Media extension instance |
| `$download` | `download` | Download extension instance |
| `$structure_id` | `int` | Current structure index counter |
| `$parent_id` | `int` | Current parent index in structure |
| `$execute_vars` | `array` | Variables for PHP code execution context |
| `$title` | `string` | Document title |
| `$description` | `string` | Document description |
| `$keyword` | `string` | Document keywords |
| `$header` | `string` | Additional header content |
| `$query_data` | `array` | Querystring data for URL generation |

### __construct

Initializes the template engine.

**Parameters:** None

**Returns:** `void`

**Inner Mechanisms:** Creates a `data` instance for the `#system/template` store, checks operator permission, determines compatibility mode from system settings, and creates necessary directories for template assets.

**Usage Example:**
```php
$template = new template();
```

### add

Creates a new template entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Template name (language key) |
| `$category` | `string` or `NULL` | Category for grouping |
| `$page` | `bool` or `NULL` | Whether this is a page template |
| `$code` | `string` or `NULL` | Template code |
| `$stylesheet` | `string` or `NULL` | Stylesheet content |
| `$javascript` | `string` or `NULL` | JavaScript content |

**Returns:** `string` or `FALSE` — The new template index on success, `FALSE` on failure.

**Inner Mechanisms:** Checks operator permission, sets a default name if none provided, inserts a new record into the data store, then sets the code, stylesheet, and JavaScript assets. Saves the data store.

**Usage Example:**
```php
$template = new template();
$index = $template->add("My Template", "Content", TRUE, '<CMS:text id="content.body"/>');
```

### set

Updates an existing template entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index to update |
| `$name` | `string` or `NULL` | New template name |
| `$category` | `string` or `NULL` | New category |
| `$page` | `bool` or `NULL` | New page flag |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Checks operator permission, sets a default name if needed, updates the dataset, and saves.

**Usage Example:**
```php
$template = new template();
$template->set("home_tpl", "Homepage", "Content", TRUE);
```

### get

Retrieves a template entry by index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |

**Returns:** `mixed` — The template data record.

**Inner Mechanisms:** Delegates to the data store's `get()` method.

**Usage Example:**
```php
$template = new template();
$data = $template->get("home_tpl");
```

### asset_path (private)

Returns the file path pattern for a given asset type.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Asset type constant (`CMS_TEMPLATE_ASSET_TYPE_CODE`, `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET`, or `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT`) |

**Returns:** `string` or `FALSE` — The path pattern with `%s` placeholders, or `FALSE` for unknown types.

**Inner Mechanisms:** Uses a switch statement to map asset types to path patterns.

**Usage Example:**
```php
// Internal use only
$path = $this->asset_path(CMS_TEMPLATE_ASSET_TYPE_CODE);
// Returns: "#template/%s%s.htm"
```

### set_asset (private)

Writes asset content to files for all languages.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$text` | `string` or `array` | Asset content (can be a language-keyed array) |
| `$type` | `int` | Asset type constant |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Checks operator permission, gets the path pattern, iterates through language variants using `language_get_array()`, writes or deletes files as appropriate, and returns success/failure status.

**Usage Example:**
```php
// Internal use only
$this->set_asset("home_tpl", $code, CMS_TEMPLATE_ASSET_TYPE_CODE);
```

### get_asset (private)

Reads asset content from files.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$language` | `string` or `FALSE` | Language code, `FALSE` for all languages, `NULL` for active language |
| `$check` | `bool` | If `TRUE`, checks file existence instead of reading content |
| `$type` | `int` | Asset type constant |

**Returns:** `string` or `bool` — The asset content, or `FALSE` if file doesn't exist (when `$check` is `TRUE`).

**Inner Mechanisms:** Gets the path pattern, handles multi-language retrieval when `$language` is `FALSE`, falls back to the default language when the active language file is missing, and either checks existence or reads content.

**Usage Example:**
```php
// Internal use only
$code = $this->get_asset("home_tpl", "en", FALSE, CMS_TEMPLATE_ASSET_TYPE_CODE);
```

### set_code

Sets the template code for a given index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$text` | `string` or `array` | Template code content |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Delegates to `set_asset()` with `CMS_TEMPLATE_ASSET_TYPE_CODE`, then invalidates the content cache by touching the template data file.

**Usage Example:**
```php
$template = new template();
$template->set_code("home_tpl", '<CMS:text id="content.body"/>');
```

### get_code

Retrieves the template code for a given index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$language` | `string` or `FALSE` or `NULL` | Language code, `FALSE` for all languages, `NULL` for active language |

**Returns:** `string` — The template code.

**Inner Mechanisms:** For multi-language retrieval (`$language === FALSE`), delegates to `get_asset()`. For single language, first checks the temporary cache (`cms_cache`), then reads from file and caches the result.

**Usage Example:**
```php
$template = new template();
$code = $template->get_code("home_tpl");
```

### set_stylesheet

Sets the stylesheet for a given template index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$text` | `string` or `array` | Stylesheet content |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Delegates to `set_asset()` with `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET`.

**Usage Example:**
```php
$template = new template();
$template->set_stylesheet("home_tpl", ".my-class { color: red; }");
```

### get_stylesheet

Retrieves the stylesheet for a given template index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$language` | `string` or `FALSE` or `NULL` | Language code |
| `$check` | `bool` | If `TRUE`, checks file existence |

**Returns:** `string` or `bool` — The stylesheet content or file path.

**Inner Mechanisms:** Delegates to `get_asset()` with `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET`.

**Usage Example:**
```php
$template = new template();
$css = $template->get_stylesheet("home_tpl");
```

### set_javascript

Sets the JavaScript for a given template index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$text` | `string` or `array` | JavaScript content |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Delegates to `set_asset()` with `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT`.

**Usage Example:**
```php
$template = new template();
$template->set_javascript("home_tpl", "console.log('loaded');");
```

### get_javascript

Retrieves the JavaScript for a given template index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index |
| `$language` | `string` or `FALSE` or `NULL` | Language code |
| `$check` | `bool` | If `TRUE`, checks file existence |

**Returns:** `string` or `bool` — The JavaScript content or file path.

**Inner Mechanisms:** Delegates to `get_asset()` with `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT`.

**Usage Example:**
```php
$template = new template();
$js = $template->get_javascript("home_tpl");
```

### delete

Deletes a template and all its associated assets.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index to delete |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:** Checks operator permission, iterates through all enabled languages, deletes code, stylesheet, and JavaScript files, clears temporary cache entries, removes the data record, and saves.

**Usage Example:**
```php
$template = new template();
$template->delete("old_template");
```

### parse

Parses a template by index and returns the rendered output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$index` | `string` | Template index |
| `$title` | `string` or `NULL` | Page title |
| `$description` | `string` or `NULL` | Page description |
| `$keyword` | `string` or `NULL` | Page keywords |
| `$action` | `array` or `NULL` | Action definitions |
| `$header` | `string` or `NULL` | Additional header content |
| `$base_id` | `string` or `NULL` | Base ID for path resolution |
| `$cache` | `bool` | Whether to enable caching |

**Returns:** `string` or `FALSE` — The parsed output, or `FALSE` on error.

**Inner Mechanisms:** Links the template's stylesheet and JavaScript, sets metadata properties, and delegates to `_parse()` with the template code retrieved via `get_code()`.

**Usage Example:**
```php
$template = new template();
$output = $template->parse($document, "home_tpl", "My Page", "Page description");
```

### parse_code

Parses raw template code and returns the rendered output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$template` | `string` | Raw template code |
| `$title` | `string` or `NULL` | Page title |
| `$description` | `string` or `NULL` | Page description |
| `$keyword` | `string` or `NULL` | Page keywords |
| `$action` | `array` or `NULL` | Action definitions |
| `$header` | `string` or `NULL` | Additional header content |
| `$base_id` | `string` or `NULL` | Base ID for path resolution |

**Returns:** `string` or `FALSE` — The parsed output, or `FALSE` on error.

**Inner Mechanisms:** Sets metadata properties and delegates to `_parse()` with the provided template code.

**Usage Example:**
```php
$template = new template();
$output = $template->parse_code($document, '<CMS:text id="content.body"/>', "My Page");
```

### _parse (private)

The core template parsing engine that processes template code, handles all CMS tags, PHP code blocks, and generates output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$template` | `string` | Template source code |
| `$base_id` | `string` or `NULL` | Base ID for path resolution |
| `$cache` | `bool` or `int` | Caching mode: `TRUE` = cache, `FALSE` = no cache, `-1` = finish nocache span |
| `$template_index` | `string` or `NULL` | Template index for caching |

**Returns:** `string` or `FALSE` — The parsed output, or `FALSE` on error.

**Inner Mechanisms:** This is the heart of the template engine. It uses static variables to maintain state across recursive calls (template cache, nesting depth, preview/debug flags, base URL, stylesheet, title wrapper, header insert position). The method:

1. Initializes extensions (image, media, download) on first call
2. Defines scripting constants (`CMS_TEMPLATE`, `CMS_TEMPLATE_EDIT`, `CMS_TEMPLATE_OPTION`)
3. Tokenizes the template using `parse_token()` and caches the result
4. Parses PHP code blocks using `token_get_all()` and `eval()`
5. Processes `<CMS:...>` start tags by matching against the token list (`$tlist`)
6. Handles each element type with specific logic (head, control, menu, cblock, calt, group, base, namespace, repeat, shift, template, text, value, switch, href, image, thumbnail, media, download, plugin, nocache, cedit, cnoedit)
7. Processes closing tags for spannable elements
8. Manages stacks for groups, repeats, shifts, namespaces, bases, conditional blocks, and debug info
9. On the base level, inserts meta data (charset, title, description, keywords, generator, base URL, canonical/alternate links, stylesheets, JavaScript libraries, editing scripts)

**Usage Example:**
```php
// Internal use - called by parse() and parse_code()
$output = $this->_parse($document, $template_code, $base_id, $cache, $template_index);
```

### export

Exports a template with resolved IDs and asset references, suitable for content export.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$index` | `string` | Template index |
| `$base_id` | `string` or `NULL` | Base ID for path resolution |
| `$return_asset` | `bool` | If `TRUE`, returns combined assets instead of parsed output |

**Returns:** `string` or `array` or `FALSE` — The exported template code, an array with `code`, `stylesheet`, and `javascript` keys (when `$return_asset` is `TRUE`), or `FALSE` on error.

**Inner Mechanisms:** Similar to `_parse()` but instead of rendering output, it resolves all element IDs to concrete paths, replaces repeat/shift elements with namespace wrappers, recursively exports sub-templates, and optionally combines all stylesheet and JavaScript assets from employed templates.

**Usage Example:**
```php
$template = new template();
$exported = $template->export($document, "home_tpl");
// Returns resolved template code with concrete IDs
```

### structure

Analyzes a template and returns its structural information.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$index` | `string` | Template index |
| `$base_id` | `string` or `NULL` | Base ID for path resolution |

**Returns:** `array` or `FALSE` — An array of structure entries, or `FALSE` on error.

**Inner Mechanisms:** Iterates through the template code, identifies all editable and path-extending elements, and builds a structure array where each entry contains the element's index, path, parent ID, and type. Uses stacks to track parent-child relationships through groups, bases, namespaces, repeats, and shifts.

**Usage Example:**
```php
$template = new template();
$structure = $template->structure($document, "home_tpl");
// Returns: [1 => ["index" => "content.body", "path" => "content.body", "parent" => 0, "type" => "text"], ...]
```

### create_cache

Creates a cached version of a parsed template, separating static and dynamic content.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$index` | `string` | Template index |
| `$title` | `string` or `NULL` | Page title |
| `$description` | `string` or `NULL` | Page description |
| `$keyword` | `string` or `NULL` | Page keywords |
| `$action` | `array` or `NULL` | Action definitions |
| `$header` | `string` or `NULL` | Additional header content |
| `$is_dynamic` | `bool` | Set to `TRUE` if the template contains dynamic (nocache) content |

**Returns:** `array` — An array with `cache` and `output` keys.

**Inner Mechanisms:** Calls `parse()` with caching enabled, then splits the result by the cache separator. Segments are categorized as cached (static), base ID, or uncached (dynamic). The `cache` key contains the full cached version, while `output` contains only the static portions.

**Usage Example:**
```php
$template = new template();
$is_dynamic = FALSE;
$result = $template->create_cache($document, "home_tpl", "My Page", NULL, NULL, NULL, NULL, $is_dynamic);
// $result["cache"] contains the full cached template
// $result["output"] contains only static content
// $is_dynamic is TRUE if nocache blocks were found
```

### process_cache

Processes a cached template, re-parsing only the dynamic (uncached) segments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object (passed by reference) |
| `$template` | `string` | Cached template string |
| `$title` | `string` or `NULL` | Page title |
| `$description` | `string` or `NULL` | Page description |
| `$keyword` | `string` or `NULL` | Page keywords |
| `$action` | `array` or `NULL` | Action definitions |
| `$header` | `string` or `NULL` | Additional header content |
| `$is_dynamic` | `bool` | Set to `TRUE` if dynamic content was processed |

**Returns:** `string` — The fully processed output.

**Inner Mechanisms:** Splits the cached template by the cache separator, processes each segment: cached segments are output directly, base ID segments are stored, and uncached segments are re-parsed using `_parse()` with the nocache flag (`-1`).

**Usage Example:**
```php
$template = new template();
$is_dynamic = FALSE;
$output = $template->process_cache($document, $cached_template, "My Page", NULL, NULL, NULL, NULL, $is_dynamic);
```

### execute

Executes embedded PHP code within a template, with proper error handling and variable scoping.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$cms_template_code` | `string` | PHP code to execute |
| `$cms_template_document` | `document` | Document object |
| `$cms_template_base_id` | `string` | Base ID |
| `$cms_template_path_id` | `string` | Path ID |
| `$cms_template_temp_id` | `string` | Temporary ID for variable storage |

**Returns:** `mixed` — The return value of the executed code.

**Inner Mechanisms:** Temporarily replaces the error handler with `template_error()`, imports previously stored variables from `$execute_vars`, executes the code using `eval()` within the `cms` namespace, stores the resulting variables for future calls, and restores the error handler.

**Usage Example:**
```php
// Internal use - called by _parse() when processing PHP code blocks
$result = $this->execute("return $document->get('content.body');", $document, "content", "content.body", "temp123");
```

### attribute

Generates HTML attribute strings from parameter arrays, with filtering and transformation.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | Parameter array (attribute name → value) |
| `$alter` | `array` or `NULL` | Additional parameters to merge |
| `$skip` | `array` or `string` or `NULL` | Attribute names to skip |
| `$return_array` | `bool` | If `TRUE`, returns an array instead of a string |

**Returns:** `string` or `array` or `FALSE` — The attribute string(s), or `FALSE` if `$param` is not an array.

**Inner Mechanisms:** Merges alter parameters, normalizes the skip list, filters out NULL values, empty keys, underscore-prefixed keys (unless a non-underscore version exists), and skipped keys. Strips leading underscores from remaining keys. Returns either a space-prefixed attribute string or an associative array.

**Usage Example:**
```php
$template = new template();
$attrs = $template->attribute(
    ["class" => "my-class", "style" => "color:red", "_id" => "123"],
    ["title" => "My Element"],
    ["style"]
);
// Returns: ' class="my-class" title="My Element" id="123"'
```

### parse_id

Parses a CMS element ID string, resolving relative and absolute references.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` or `NULL` | The ID string to parse |
| `$id_base` | `string` | Base ID for relative resolution |
| `$id_path` | `string` | Current path ID |
| `$id_auto` | `int` | Automatic index counter |
| `$id_data_new` | `string` | Output: resolved data index |
| `$id_path_new` | `string` | Output: resolved path index |
| `$id_elem_new` | `string` | Output: element index |

**Returns:** `bool` — `TRUE` if the ID was explicitly valid, `FALSE` if auto-generated.

**Inner Mechanisms:** Processes relative automatic indexes (`+1`, `-2`), validates the ID format, and resolves it based on prefix modifiers:
- `.` prefix: relative to base ID
- `..` prefix: absolute path
- No prefix: relative to current path

If no valid ID is given, generates one using the automatic index.

**Usage Example:**
```php
$template = new template();
$valid = $template->parse_id(
    "+1",           // ID string
    "content",      // Base ID
    "content.",     // Path ID
    0,              // Auto index
    $id_data,       // Output: data index
    $id_path,       // Output: path index
    $id_elem        // Output: element index
);
// $id_data = "content.1", $id_path = "content.1", $id_elem = "1"
```


<!-- HASH:723d9f4fcfd255a2e78a43cc621b47ff -->
