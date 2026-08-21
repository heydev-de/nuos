# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.template.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template System

The `lib.template.inc` file provides the core template engine for the PWNC Web Platform. It enables the creation, management, and rendering of reusable template components with support for dynamic content, conditional logic, and hierarchical structures. The system is designed to be lightweight, flexible, and efficient, with built-in support for multilingual content, caching, and editing capabilities.

---

## Constants

The following constants define permissions, options, types, and commands used throughout the template system.

### Permission

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | `"operator"` | Permission required to manage templates. |

### Cache

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_CACHE_SEPARATOR` | `"\x1C"` | ASCII file separator used to delimit cached and dynamic content. |

### Option Flags

Option flags control the visibility and behavior of template elements during rendering.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_OPTION_NONE` | `0` | No options enabled. |
| `CMS_TEMPLATE_OPTION_HREF` | `1` | Enable hyperlink elements. |
| `CMS_TEMPLATE_OPTION_PLUGIN` | `2` | Enable plugin elements. |
| `CMS_TEMPLATE_OPTION_TEXT` | `4` | Enable text elements. |
| `CMS_TEMPLATE_OPTION_VALUE` | `8` | Enable value elements. |
| `CMS_TEMPLATE_OPTION_DOWNLOAD` | `16` | Enable download elements. |
| `CMS_TEMPLATE_OPTION_IMAGE` | `32` | Enable image elements. |
| `CMS_TEMPLATE_OPTION_THUMBNAIL` | `64` | Enable thumbnail elements. |
| `CMS_TEMPLATE_OPTION_MEDIA` | `128` | Enable media elements. |
| `CMS_TEMPLATE_OPTION_TEMPLATE` | `256` | Enable template elements. |
| `CMS_TEMPLATE_OPTION_GROUP` | `512` | Enable group elements. |
| `CMS_TEMPLATE_OPTION_REPEAT` | `1024` | Enable repeat elements. |
| `CMS_TEMPLATE_OPTION_SHIFT` | `2048` | Enable shift elements. |
| `CMS_TEMPLATE_OPTION_CALT` | `4096` | Enable conditional alternative elements. |
| `CMS_TEMPLATE_OPTION_CBLOCK` | `8192` | Enable conditional block elements. |
| `CMS_TEMPLATE_OPTION_DEBUG` | `16384` | Enable debug mode. |
| `CMS_TEMPLATE_OPTION_SWITCH` | `32768` | Enable switch elements. |
| `CMS_TEMPLATE_OPTION_LAYOUT` | `CMS_TEMPLATE_OPTION_VALUE \| CMS_TEMPLATE_OPTION_TEMPLATE \| CMS_TEMPLATE_OPTION_GROUP \| CMS_TEMPLATE_OPTION_REPEAT \| CMS_TEMPLATE_OPTION_SHIFT \| CMS_TEMPLATE_OPTION_CBLOCK \| CMS_TEMPLATE_OPTION_SWITCH` | Layout-related options. |
| `CMS_TEMPLATE_OPTION_EDIT` | `CMS_TEMPLATE_OPTION_HREF \| CMS_TEMPLATE_OPTION_PLUGIN \| CMS_TEMPLATE_OPTION_TEXT \| CMS_TEMPLATE_OPTION_DOWNLOAD \| CMS_TEMPLATE_OPTION_IMAGE \| CMS_TEMPLATE_OPTION_THUMBNAIL \| CMS_TEMPLATE_OPTION_MEDIA \| CMS_TEMPLATE_OPTION_CBLOCK` | Edit-related options. |
| `CMS_TEMPLATE_OPTION_ALL` | `CMS_TEMPLATE_OPTION_LAYOUT \| CMS_TEMPLATE_OPTION_EDIT` | All options enabled. |

### Action Types

Action types define the structure of the action array used for template editing.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_ACTION` | `0` | Index for action-related data. |
| `CMS_TEMPLATE_CONTROL` | `1` | Index for control-related data. |
| `CMS_TEMPLATE_CODE` | `2` | Index for code-related data. |
| `CMS_TEMPLATE_IMAGE` | `3` | Index for image-related data. |
| `CMS_TEMPLATE_COMMAND` | `4` | Index for command-related data. |
| `CMS_TEMPLATE_SWITCH` | `5` | Index for switch-related data. |

### Element Types

Element types define the different kinds of template elements that can be used.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_TYPE_NONE` | `0` | No type. |
| `CMS_TEMPLATE_TYPE_HEAD` | `1` | Head element. |
| `CMS_TEMPLATE_TYPE_HREF` | `2` | Hyperlink element. |
| `CMS_TEMPLATE_TYPE_PLUGIN` | `4` | Plugin element. |
| `CMS_TEMPLATE_TYPE_TEXT` | `8` | Text element. |
| `CMS_TEMPLATE_TYPE_VALUE` | `16` | Value element. |
| `CMS_TEMPLATE_TYPE_DOWNLOAD` | `32` | Download element. |
| `CMS_TEMPLATE_TYPE_IMAGE` | `64` | Image element. |
| `CMS_TEMPLATE_TYPE_THUMBNAIL` | `128` | Thumbnail element. |
| `CMS_TEMPLATE_TYPE_MEDIA` | `256` | Media element. |
| `CMS_TEMPLATE_TYPE_TEMPLATE` | `512` | Template element. |
| `CMS_TEMPLATE_TYPE_GROUP` | `1024` | Group element. |
| `CMS_TEMPLATE_TYPE_REPEAT` | `2048` | Repeat element. |
| `CMS_TEMPLATE_TYPE_SHIFT` | `4096` | Shift element. |
| `CMS_TEMPLATE_TYPE_MENU` | `8192` | Menu element. |
| `CMS_TEMPLATE_TYPE_CBLOCK` | `16384` | Conditional block element. |
| `CMS_TEMPLATE_TYPE_CALT` | `32768` | Conditional alternative element. |
| `CMS_TEMPLATE_TYPE_BASE` | `65536` | Base element. |
| `CMS_TEMPLATE_TYPE_NAMESPACE` | `131072` | Namespace element. |
| `CMS_TEMPLATE_TYPE_NOCACHE` | `262144` | No-cache element. |
| `CMS_TEMPLATE_TYPE_CONTROL` | `524288` | Control element. |
| `CMS_TEMPLATE_TYPE_BACKLINK` | `1048576` | Backlink element. |
| `CMS_TEMPLATE_TYPE_DEBUG` | `2097152` | Debug element. |
| `CMS_TEMPLATE_TYPE_STYLESHEET` | `4194304` | Stylesheet element. |
| `CMS_TEMPLATE_TYPE_SWITCH` | `8388608` | Switch element. |
| `CMS_TEMPLATE_TYPE_CEDIT` | `16777216` | Editable block element. |
| `CMS_TEMPLATE_TYPE_CNOEDIT` | `33554432` | Non-editable block element. |
| `CMS_TEMPLATE_TYPE_JAVASCRIPT` | `67108864` | JavaScript element. |
| `CMS_TEMPLATE_TYPE_ALL` | `4294967295` | All types enabled. |

### Type Filters

Type filters are used to group related element types for specific use cases.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_TYPE_EDIT` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_PLUGIN \| CMS_TEMPLATE_TYPE_TEXT \| CMS_TEMPLATE_TYPE_VALUE \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_IMAGE \| CMS_TEMPLATE_TYPE_THUMBNAIL \| CMS_TEMPLATE_TYPE_MEDIA \| CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_SWITCH` | Types that can be edited. |
| `CMS_TEMPLATE_TYPE_SPAN` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_CBLOCK \| CMS_TEMPLATE_TYPE_CALT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE \| CMS_TEMPLATE_TYPE_NOCACHE \| CMS_TEMPLATE_TYPE_CEDIT \| CMS_TEMPLATE_TYPE_CNOEDIT` | Types that span multiple elements. |
| `CMS_TEMPLATE_TYPE_PATH` | `CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE` | Types that extend the element path. |

### Command Types

Command types define the actions that can be performed during template editing.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_COMMAND_NONE` | `0` | No command. |
| `CMS_TEMPLATE_COMMAND_BUFFER` | `1` | Buffer command. |
| `CMS_TEMPLATE_COMMAND_PASTE` | `2` | Paste command. |
| `CMS_TEMPLATE_COMMAND_SWAP` | `4` | Swap command. |
| `CMS_TEMPLATE_COMMAND_KICK1` | `8` | Kick (remove) command. |
| `CMS_TEMPLATE_COMMAND_KICK2` | `16` | Kick (remove) command variant. |
| `CMS_TEMPLATE_COMMAND_DROP1` | `32` | Drop command. |
| `CMS_TEMPLATE_COMMAND_DROP2` | `64` | Drop command variant. |
| `CMS_TEMPLATE_COMMAND_RELEASE` | `128` | Release command. |
| `CMS_TEMPLATE_COMMAND_REFERENCE` | `256` | Reference command. |
| `CMS_TEMPLATE_COMMAND_EXPORT` | `512` | Export command. |
| `CMS_TEMPLATE_COMMAND_CLEAR` | `1024` | Clear command. |
| `CMS_TEMPLATE_COMMAND_DRAGDROP1` | `2048` | Drag-and-drop command. |
| `CMS_TEMPLATE_COMMAND_DRAGDROP2` | `4096` | Drag-and-drop command variant. |
| `CMS_TEMPLATE_COMMAND_ALL` | `4294967295` | All commands enabled. |

### Structure Indices

Structure indices define the positions of data within the structure array.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_STRUCTURE_INDEX` | `0` | Index of the element. |
| `CMS_TEMPLATE_STRUCTURE_PATH` | `1` | Path of the element. |
| `CMS_TEMPLATE_STRUCTURE_PARENT` | `2` | Parent index of the element. |
| `CMS_TEMPLATE_STRUCTURE_TYPE` | `3` | Type of the element. |

### Asset Types

Asset types define the different kinds of assets that can be associated with a template.

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_ASSET_TYPE_CODE` | `1` | Template code asset. |
| `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET` | `2` | Stylesheet asset. |
| `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT` | `3` | JavaScript asset. |

---

## Functions

### `template_get_array`

Retrieves a list of templates organized by category.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filter templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array of templates organized by category. |

#### Inner Mechanisms

- Queries the `#system/template` data source.
- Filters templates based on the `page` attribute if `$page` is not `NULL`.
- Organizes templates by their `category` attribute.
- Uses the template's `name` attribute for display, falling back to the template index if the name is empty.
- Ensures unique display names by appending a counter if duplicates exist.
- Sorts the array naturally and case-insensitively.

#### Usage Example

```php
$templates = template_get_array();
foreach ($templates as $category => $items) {
    echo "<h2>" . x($category) . "</h2>";
    echo "<ul>";
    foreach ($items as $name => $index) {
        echo "<li>" . x($name) . "</li>";
    }
    echo "</ul>";
}
```

---

### `template_get_select`

Retrieves a list of template categories for use in a select input.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filter templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array of categories with empty values. |

#### Inner Mechanisms

- Queries the `#system/template` data source.
- Filters templates based on the `page` attribute if `$page` is not `NULL`.
- Extracts the `category` attribute from each template.
- Sorts the array naturally and case-insensitively.

#### Usage Example

```php
$categories = template_get_select();
echo "<select name='template_category'>";
foreach ($categories as $value => $label) {
    echo "<option value='" . x($value) . "'>" . x($label) . "</option>";
}
echo "</select>";
```

---

### `template_get_attribute`

Extracts attributes from an HTML tag.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The HTML tag string. |
| `$name` | `string\|NULL` | The attribute name to extract. If `NULL`, all attributes are returned. |

#### Return Values

| Type | Description |
|------|-------------|
| `array\|string\|NULL` | Associative array of attributes, the value of the specified attribute, or `NULL` if no match is found. |

#### Inner Mechanisms

- Uses a regular expression to match attribute names and values.
- Supports quoted and unquoted attribute values.
- Converts attribute names to lowercase for consistency.
- Decodes HTML entities in attribute values.

#### Usage Example

```php
$tag = '<CMS:text id="content.title" default="Hello" edit="true" />';
$attributes = template_get_attribute($tag);
echo "ID: " . x($attributes['id']) . "<br>";
echo "Default: " . x($attributes['default']);
```

---

### `template_set_attribute`

Sets or updates an attribute in an HTML tag.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The HTML tag string. |
| `$name` | `string` | The attribute name to set. |
| `$value` | `string` | The attribute value to set. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The updated HTML tag string. |

#### Inner Mechanisms

- Removes any existing attribute with the same name.
- Appends the new attribute to the opening tag.

#### Usage Example

```php
$tag = '<CMS:text id="content.title" />';
$updatedTag = template_set_attribute($tag, 'default', 'Hello World');
echo $updatedTag; // Output: <CMS:text id="content.title" default="Hello World" />
```

---

### `template_remove_attribute`

Removes an attribute from an HTML tag.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The HTML tag string. |
| `$name` | `string\|NULL` | The attribute name to remove. If `NULL`, all attributes are removed. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The updated HTML tag string. |

#### Inner Mechanisms

- Uses a regular expression to match and remove the specified attribute.
- Supports quoted and unquoted attribute values.

#### Usage Example

```php
$tag = '<CMS:text id="content.title" default="Hello" edit="true" />';
$updatedTag = template_remove_attribute($tag, 'edit');
echo $updatedTag; // Output: <CMS:text id="content.title" default="Hello" />
```

---

### `template_parse_reference`

Parses a reference string into its components (name, description, and URL).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The reference string to parse. |
| `$analyze` | `bool` | If `TRUE`, returns the analyzed URL components instead of the parsed reference. |

#### Return Values

| Type | Description |
|------|-------------|
| `array\|FALSE` | Associative array with keys `name`, `description`, and `url`, or `FALSE` if the reference is invalid. |

#### Inner Mechanisms

- Supports references in the format `scheme://host/url` (e.g., `content://123`).
- Handles deprecated formats like `directory:123` or `address:/path`.
- Retrieves metadata (name and description) from the appropriate data source based on the scheme:
  - `directory`: Queries the `#system/directory` data source.
  - `content`: Queries the `CMS_DB_CONTENT` table.
  - `address`: Uses the URL as-is.
- Translates the URL using `translate_url`.

#### Usage Example

```php
$reference = 'content://123';
$parsed = template_parse_reference($reference);
if ($parsed) {
    echo "Name: " . x($parsed['name']) . "<br>";
    echo "Description: " . x($parsed['description']) . "<br>";
    echo "URL: " . x($parsed['url']);
}
```

---

### `template_read_plugin`

Fetches the content of a plugin from a given URL.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The URL of the plugin to fetch. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | The fetched content or `FALSE` on failure. |

#### Inner Mechanisms

- Uses the `http` library to fetch the content of the URL.
- Returns `FALSE` if the `http` library is not loaded or the fetch fails.

#### Usage Example

```php
$pluginUrl = 'https://example.com/plugin.php';
$content = template_read_plugin($pluginUrl);
if ($content !== FALSE) {
    echo $content;
}
```

---

### `template_preview`

Generates a preview of a template or template code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index_or_code` | `string` | The template index or raw template code. |
| `$is_index` | `bool` | If `TRUE`, `$index_or_code` is treated as a template index. If `FALSE`, it is treated as raw code. |
| `$document` | `document\|NULL` | The document object to use for rendering. If `NULL`, a new document is created. |
| `$inert` | `bool` | If `TRUE`, wraps the output in an inert preview container. |

#### Return Values

| Type | Description |
|------|-------------|
| `void` | Outputs the preview directly. |

#### Inner Mechanisms

- Defines dummy actions for editable elements to ensure they render correctly in preview mode.
- Creates a new `document` object if none is provided.
- Sets the `CMS_TEMPLATE_PREVIEW` constant to indicate preview mode.
- Renders the template using the `parse` or `parse_code` method of the `template` class.
- Wraps the output in a simple HTML page frame with the necessary stylesheets and scripts.
- Uses `preview_inert` to wrap the output if `$inert` is `TRUE`.

#### Usage Example

```php
// Preview a template by index
template_preview('homepage', TRUE);

// Preview raw template code
$code = '<CMS:text id="content.title" default="Welcome" />';
template_preview($code, FALSE);
```

---

### `template_error`

Generates a custom error message for template-related errors.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$code` | `int` | The error code. |
| `$message` | `string` | The error message. |
| `$path` | `string\|NULL` | The path to the template file. If `NULL`, a default path is used. |
| `$line` | `int\|NULL` | The line number where the error occurred. If `NULL`, the current line is used. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The formatted error message. |

#### Inner Mechanisms

- Constructs a path to the template file if the template index is available.
- Uses `cms_error` to generate the error message.
- Adjusts the line number based on the current template error line.

#### Usage Example

```php
try {
    // Some template-related code
} catch (Exception $e) {
    echo template_error($e->getCode(), $e->getMessage());
}
```

---

## Class: `template`

The `template` class provides methods for managing, parsing, and rendering templates.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Data source for template metadata. |
| `$operator` | `bool` | Indicates if the current user has operator permissions. |
| `$compat_mode` | `bool` | Indicates if compatibility mode is enabled for older templates. |
| `$tlist` | `array` | Maps template element names to their type constants. |
| `$tname` | `array` | Maps template type constants to their localized names. |
| `$toption` | `array` | Maps template type constants to their option flags. |
| `$action` | `array\|NULL` | Action definitions for template editing. |
| `$image` | `image\|NULL` | Instance of the `image` class for image processing. |
| `$media` | `media\|NULL` | Instance of the `media` class for media processing. |
| `$download` | `download\|NULL` | Instance of the `download` class for download processing. |
| `$structure_id` | `int\|NULL` | Current structure index during structure parsing. |
| `$parent_id` | `int\|NULL` | Current parent index during structure parsing. |
| `$execute_vars` | `array` | Variables used during template execution. |
| `$title` | `string` | Document title. |
| `$description` | `string` | Document description. |
| `$keyword` | `string` | Document keywords. |
| `$header` | `string` | Additional header content. |
| `$query_data` | `array` | Query string data. |

---

### Constructor

```php
function __construct()
```

#### Purpose

Initializes the `template` class.

#### Inner Mechanisms

- Initializes the `$data` property with the `#system/template` data source.
- Sets the `$operator` property based on the `CMS_TEMPLATE_PERMISSION_OPERATOR` permission.
- Determines if compatibility mode is enabled based on the system setting.
- Creates necessary directories for template assets.

#### Usage Example

```php
$template = new template();
```

---

### `add`

```php
function add($name, $category = NULL, $page = NULL, $code = NULL, $stylesheet = NULL, $javascript = NULL)
```

#### Purpose

Adds a new template to the system.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | The name of the template. |
| `$category` | `string\|NULL` | The category of the template. |
| `$page` | `bool\|NULL` | If `TRUE`, the template is a page template. If `FALSE`, it is a modular template. |
| `$code` | `string\|NULL` | The template code. |
| `$stylesheet` | `string\|NULL` | The stylesheet code. |
| `$javascript` | `string\|NULL` | The JavaScript code. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | The index of the new template or `FALSE` on failure. |

#### Inner Mechanisms

- Checks if the user has operator permissions.
- Sets a default name if none is provided.
- Inserts the template metadata into the data source.
- Saves the template code, stylesheet, and JavaScript assets.
- Returns the index of the new template.

#### Usage Example

```php
$template = new template();
$index = $template->add(
    'Homepage',
    'Layout',
    TRUE,
    '<CMS:text id="content.title" default="Welcome" />',
    'body { font-family: Arial; }',
    'console.log("Template loaded");'
);
if ($index !== FALSE) {
    echo "Template added with index: " . x($index);
}
```

---

### `set`

```php
function set($index, $name = NULL, $category = NULL, $page = NULL)
```

#### Purpose

Updates the metadata of an existing template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template to update. |
| `$name` | `string\|NULL` | The new name of the template. |
| `$category` | `string\|NULL` | The new category of the template. |
| `$page` | `bool\|NULL` | The new page status of the template. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Checks if the user has operator permissions.
- Sets a default name if none is provided.
- Updates the template metadata in the data source.
- Saves the changes to the data source.

#### Usage Example

```php
$template = new template();
$success = $template->set('homepage', 'New Homepage', 'Layout', TRUE);
if ($success) {
    echo "Template updated successfully.";
}
```

---

### `get`

```php
function get($index)
```

#### Purpose

Retrieves the metadata of a template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | The template metadata. |

#### Inner Mechanisms

- Retrieves the template metadata from the data source.

#### Usage Example

```php
$template = new template();
$metadata = $template->get('homepage');
echo "Template Name: " . x($metadata['name']);
```

---

### `asset_path`

```php
private function asset_path($type)
```

#### Purpose

Generates the file path for a template asset based on its type.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | The asset type (e.g., `CMS_TEMPLATE_ASSET_TYPE_CODE`). |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | The file path format or `FALSE` if the type is invalid. |

#### Inner Mechanisms

- Returns the appropriate file path format based on the asset type.

#### Usage Example

```php
$template = new template();
$path = $template->asset_path(CMS_TEMPLATE_ASSET_TYPE_CODE);
echo "Code asset path: " . x($path);
```

---

### `set_asset`

```php
private function set_asset($index, &$text, $type)
```

#### Purpose

Saves a template asset (code, stylesheet, or JavaScript) to disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$text` | `string` | The asset content. |
| `$type` | `int` | The asset type (e.g., `CMS_TEMPLATE_ASSET_TYPE_CODE`). |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Checks if the user has operator permissions.
- Retrieves the asset path using `asset_path`.
- Processes multilingual content if applicable.
- Writes the asset content to the appropriate file(s).
- Deletes the file if the content is empty.

#### Usage Example

```php
$template = new template();
$success = $template->set_asset('homepage', '<CMS:text id="content.title" />', CMS_TEMPLATE_ASSET_TYPE_CODE);
if ($success) {
    echo "Asset saved successfully.";
}
```

---

### `get_asset`

```php
private function get_asset($index, $language, $check, $type)
```

#### Purpose

Retrieves a template asset (code, stylesheet, or JavaScript) from disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$language` | `string\|NULL\|FALSE` | The language code. If `FALSE`, retrieves all languages. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks if the file exists. If `FALSE`, reads the file content. |
| `$type` | `int` | The asset type (e.g., `CMS_TEMPLATE_ASSET_TYPE_CODE`). |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|array\|FALSE` | The asset content, file path, or `FALSE` on failure. |

#### Inner Mechanisms

- Retrieves the asset path using `asset_path`.
- Handles multilingual content if applicable.
- Reads the asset content from the appropriate file(s).
- Falls back to the default language if the specified language is not available.

#### Usage Example

```php
$template = new template();
$code = $template->get_asset('homepage', NULL, FALSE, CMS_TEMPLATE_ASSET_TYPE_CODE);
echo "Template Code: " . x($code);
```

---

### `set_code`

```php
function set_code($index, $text)
```

#### Purpose

Saves the template code to disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$text` | `string` | The template code. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Uses `set_asset` to save the template code.
- Invalidates the template cache by touching the cache file.

#### Usage Example

```php
$template = new template();
$success = $template->set_code('homepage', '<CMS:text id="content.title" default="Welcome" />');
if ($success) {
    echo "Template code saved successfully.";
}
```

---

### `get_code`

```php
function get_code($index, $language = NULL)
```

#### Purpose

Retrieves the template code from disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$language` | `string\|NULL\|FALSE` | The language code. If `FALSE`, retrieves all languages. If `NULL`, uses the current language. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The template code. |

#### Inner Mechanisms

- Uses `get_asset` to retrieve the template code.
- Implements temporary caching for performance.

#### Usage Example

```php
$template = new template();
$code = $template->get_code('homepage');
echo "Template Code: " . x($code);
```

---

### `set_stylesheet`

```php
function set_stylesheet($index, $text)
```

#### Purpose

Saves the template stylesheet to disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$text` | `string` | The stylesheet code. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Uses `set_asset` to save the stylesheet.

#### Usage Example

```php
$template = new template();
$success = $template->set_stylesheet('homepage', 'body { font-family: Arial; }');
if ($success) {
    echo "Stylesheet saved successfully.";
}
```

---

### `get_stylesheet`

```php
function get_stylesheet($index, $language = NULL, $check = FALSE)
```

#### Purpose

Retrieves the template stylesheet from disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$language` | `string\|NULL\|FALSE` | The language code. If `FALSE`, retrieves all languages. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks if the file exists. If `FALSE`, reads the file content. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | The stylesheet content, file path, or `FALSE` on failure. |

#### Inner Mechanisms

- Uses `get_asset` to retrieve the stylesheet.

#### Usage Example

```php
$template = new template();
$stylesheet = $template->get_stylesheet('homepage', NULL, FALSE);
echo "Stylesheet: " . x($stylesheet);
```

---

### `set_javascript`

```php
function set_javascript($index, $text)
```

#### Purpose

Saves the template JavaScript to disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$text` | `string` | The JavaScript code. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Uses `set_asset` to save the JavaScript.

#### Usage Example

```php
$template = new template();
$success = $template->set_javascript('homepage', 'console.log("Template loaded");');
if ($success) {
    echo "JavaScript saved successfully.";
}
```

---

### `get_javascript`

```php
function get_javascript($index, $language = NULL, $check = FALSE)
```

#### Purpose

Retrieves the template JavaScript from disk.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template. |
| `$language` | `string\|NULL\|FALSE` | The language code. If `FALSE`, retrieves all languages. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks if the file exists. If `FALSE`, reads the file content. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | The JavaScript content, file path, or `FALSE` on failure. |

#### Inner Mechanisms

- Uses `get_asset` to retrieve the JavaScript.

#### Usage Example

```php
$template = new template();
$javascript = $template->get_javascript('homepage', NULL, FALSE);
echo "JavaScript: " . x($javascript);
```

---

### `delete`

```php
function delete($index)
```

#### Purpose

Deletes a template and its associated assets.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The index of the template to delete. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Checks if the user has operator permissions.
- Deletes the template code, stylesheet, and JavaScript files for all languages.
- Removes the template metadata from the data source.
- Invalidates the template cache.

#### Usage Example

```php
$template = new template();
$success = $template->delete('homepage');
if ($success) {
    echo "Template deleted successfully.";
}
```

---

### `parse`

```php
function parse(&$document, $index, $title = NULL, $description = NULL, $keyword = NULL, $action = NULL, $header = NULL, $base_id = NULL, $cache = FALSE)
```

#### Purpose

Parses a template and renders it into HTML.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$index` | `string` | The index of the template to parse. |
| `$title` | `string\|NULL` | The document title. |
| `$description` | `string\|NULL` | The document description. |
| `$keyword` | `string\|NULL` | The document keywords. |
| `$action` | `array\|NULL` | Action definitions for template editing. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | The base ID for element paths. |
| `$cache` | `bool` | If `TRUE`, enables caching. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The rendered HTML. |

#### Inner Mechanisms

- Links the stylesheet and JavaScript of the template.
- Sets the document metadata (title, description, keywords, and header).
- Calls `_parse` to render the template.

#### Usage Example

```php
$template = new template();
$document = new document();
$html = $template->parse($document, 'homepage', 'Home', 'Welcome to our website', 'home, welcome');
echo $html;
```

---

### `parse_code`

```php
function parse_code(&$document, $template, $title = NULL, $description = NULL, $keyword = NULL, $action = NULL, $header = NULL, $base_id = NULL)
```

#### Purpose

Parses raw template code and renders it into HTML.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$template` | `string` | The raw template code. |
| `$title` | `string\|NULL` | The document title. |
| `$description` | `string\|NULL` | The document description. |
| `$keyword` | `string\|NULL` | The document keywords. |
| `$action` | `array\|NULL` | Action definitions for template editing. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | The base ID for element paths. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The rendered HTML. |

#### Inner Mechanisms

- Sets the document metadata (title, description, keywords, and header).
- Calls `_parse` to render the template code.

#### Usage Example

```php
$template = new template();
$document = new document();
$code = '<CMS:text id="content.title" default="Welcome" />';
$html = $template->parse_code($document, $code, 'Home');
echo $html;
```

---

### `_parse`

```php
private function _parse(&$document, $template, $base_id = NULL, $cache = FALSE, $template_index = NULL)
```

#### Purpose

Internal method for parsing and rendering template code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$template` | `string` | The template code. |
| `$base_id` | `string\|NULL` | The base ID for element paths. |
| `$cache` | `bool\|int` | If `TRUE`, enables caching. If `-1`, finishes a preprocessed no-cache span. |
| `$template_index` | `string\|NULL` | The index of the template. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The rendered HTML. |

#### Inner Mechanisms

- Tokenizes the template code for PHP processing.
- Processes template elements (e.g., `<CMS:text>`, `<CMS:group>`) and their attributes.
- Handles conditional logic (e.g., `<CMS:cblock>`, `<CMS:calt>`).
- Manages hierarchical structures (e.g., `<CMS:group>`, `<CMS:repeat>`).
- Supports dynamic content insertion and editing controls.
- Implements caching and no-cache spans.
- Injects metadata into the HTML head.

#### Usage Example

```php
$template = new template();
$document = new document();
$html = $template->_parse($document, '<CMS:text id="content.title" default="Welcome" />');
echo $html;
```

---

### `export`

```php
function export(&$document, $index, $base_id = NULL, $return_asset = FALSE)
```

#### Purpose

Exports a template and its content into a standalone format.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$index` | `string` | The index of the template to export. |
| `$base_id` | `string\|NULL` | The base ID for element paths. |
| `$return_asset` | `bool` | If `TRUE`, returns the combined stylesheet and JavaScript assets. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|array` | The exported template code, or an array of assets if `$return_asset` is `TRUE`. |

#### Inner Mechanisms

- Processes the template to resolve dynamic content and references.
- Replaces relative paths with absolute references.
- Combines stylesheets and JavaScript assets if `$return_asset` is `TRUE`.

#### Usage Example

```php
$template = new template();
$document = new document();
$exported = $template->export($document, 'homepage');
echo "Exported Template: " . x($exported);
```

---

### `structure`

```php
function structure(&$document, $index, $base_id = NULL)
```

#### Purpose

Generates a hierarchical structure of the template elements.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$index` | `string` | The index of the template. |
| `$base_id` | `string\|NULL` | The base ID for element paths. |

#### Return Values

| Type | Description |
|------|-------------|
| `array\|FALSE` | The hierarchical structure of the template or `FALSE` on failure. |

#### Inner Mechanisms

- Parses the template to identify editable and path-extending elements.
- Builds a hierarchical structure with parent-child relationships.
- Returns an array where each element contains its index, path, parent index, and type.

#### Usage Example

```php
$template = new template();
$document = new document();
$structure = $template->structure($document, 'homepage');
print_r($structure);
```

---

### `create_cache`

```php
function create_cache(&$document, $index, $title = NULL, $description = NULL, $keyword = NULL, $action = NULL, $header = NULL, &$is_dynamic = FALSE)
```

#### Purpose

Creates a cached version of a parsed template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$index` | `string` | The index of the template. |
| `$title` | `string\|NULL` | The document title. |
| `$description` | `string\|NULL` | The document description. |
| `$keyword` | `string\|NULL` | The document keywords. |
| `$action` | `array\|NULL` | Action definitions for template editing. |
| `$header` | `string\|NULL` | Additional header content. |
| `$is_dynamic` | `bool` | Output parameter indicating if the template contains dynamic content. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | An array with keys `cache` (cached content) and `output` (rendered HTML). |

#### Inner Mechanisms

- Parses the template with caching enabled.
- Splits the output into cached and dynamic sections.
- Returns both the cached content and the fully rendered output.

#### Usage Example

```php
$template = new template();
$document = new document();
$result = $template->create_cache($document, 'homepage', 'Home', 'Welcome', 'home, welcome');
echo "Cached Content: " . x($result['cache']);
echo "Rendered Output: " . $result['output'];
```

---

### `process_cache`

```php
function process_cache(&$document, $template, $title = NULL, $description = NULL, $keyword = NULL, $action = NULL, $header = NULL, &$is_dynamic = FALSE)
```

#### Purpose

Processes a cached template to render dynamic content.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | The document object containing the content data. |
| `$template` | `string` | The cached template content. |
| `$title` | `string\|NULL` | The document title. |
| `$description` | `string\|NULL` | The document description. |
| `$keyword` | `string\|NULL` | The document keywords. |
| `$action` | `array\|NULL` | Action definitions for template editing. |
| `$header` | `string\|NULL` | Additional header content. |
| `$is_dynamic` | `bool` | Output parameter indicating if the template contains dynamic content. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The rendered HTML. |

#### Inner Mechanisms

- Splits the cached template into static and dynamic sections.
- Processes the dynamic sections using `_parse`.
- Combines the static and dynamic content into the final output.

#### Usage Example

```php
$template = new template();
$document = new document();
$cached = 'static content' . CMS_TEMPLATE_CACHE_SEPARATOR . 'base.id' . CMS_TEMPLATE_CACHE_SEPARATOR . 'dynamic content';
$html = $template->process_cache($document, $cached, 'Home');
echo $html;
```

---

### `execute`

```php
function execute($cms_template_code, $cms_template_document, $cms_template_base_id, $cms_template_path_id, $cms_template_temp_id)
```

#### Purpose

Executes PHP code embedded in a template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$cms_template_code` | `string` | The PHP code to execute. |
| `$cms_template_document` | `document` | The document object. |
| `$cms_template_base_id` | `string` | The base ID for element paths. |
| `$cms_template_path_id` | `string` | The current path ID. |
| `$cms_template_temp_id` | `string` | A temporary ID for variable storage. |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | The result of the executed code. |

#### Inner Mechanisms

- Temporarily replaces the error handler to catch template-related errors.
- Extracts variables from the `$execute_vars` property.
- Executes the PHP code in the current namespace.
- Stores the defined variables back into `$execute_vars`.
- Restores the original error handler.

#### Usage Example

```php
$template = new template();
$document = new document();
$result = $template->execute('return 2 + 2;', $document, 'base.id', 'path.id', 'temp.id');
echo "Result: " . x($result);
```

---

### `attribute`

```php
function attribute($param, $alter = NULL, $skip = NULL, $return_array = FALSE)
```

#### Purpose

Generates HTML attributes from an associative array.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | The associative array of attributes. |
| `$alter` | `array\|NULL` | Additional attributes to merge. |
| `$skip` | `string\|array\|NULL` | Attributes to skip. |
| `$return_array` | `bool` | If `TRUE`, returns an associative array. If `FALSE`, returns a string. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|array\|FALSE` | The generated attributes as a string or array, or `FALSE` on failure. |

#### Inner Mechanisms

- Merges the `$param` and `$alter` arrays.
- Skips specified attributes and internal keys (e.g., `id`, `idref`).
- Generates a string of HTML attributes or an associative array.

#### Usage Example

```php
$template = new template();
$attributes = ['class' => 'button', 'style' => 'color: red;'];
$htmlAttr = $template->attribute($attributes);
echo "<div$htmlAttr>Click Me</div>";
```

---

### `parse_id`

```php
function parse_id($string, $id_base, $id_path, $id_auto, &$id_data_new = NULL, &$id_path_new = NULL, &$id_elem_new = NULL)
```

#### Purpose

Parses an element ID into its components.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The ID string to parse. |
| `$id_base` | `string` | The base ID for relative paths. |
| `$id_path` | `string` | The current path ID. |
| `$id_auto` | `int` | The current automatic index. |
| `$id_data_new` | `string` | Output parameter for the new data ID. |
| `$id_path_new` | `string` | Output parameter for the new path ID. |
| `$id_elem_new` | `string` | Output parameter for the new element ID. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the ID is valid, `FALSE` otherwise. |

#### Inner Mechanisms

- Processes relative automatic indexes (e.g., `+1`, `-1`).
- Handles absolute, relative, and base-relative paths.
- Validates the ID format.

#### Usage Example

```php
$template = new template();
$valid = $template->parse_id('content.title', 'base.', 'path.', 1, $dataId, $pathId, $elemId);
if ($valid) {
    echo "Data ID: " . x($dataId) . "<br>";
    echo "Path ID: " . x($pathId) . "<br>";
    echo "Element ID: " . x($elemId);
}
```


<!-- HASH:29763db42db5d94f00e439129f99d411 -->
