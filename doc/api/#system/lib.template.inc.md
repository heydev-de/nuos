# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.template.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/%23system/lib.template.inc)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Template System Overview

The `lib.template.inc` file provides the core template engine for the NUOS web platform. It handles parsing, rendering, and managing templates that define the structure and presentation of web pages. The template system supports multilingual content, conditional rendering, nested templates, and editing capabilities.

The template engine processes custom XML-like tags (`<CMS:*>`) to generate HTML output. It integrates with other NUOS modules like `image`, `media`, and `download` to handle specialized content types.

---

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for template operators. |

### Template Cache Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_CACHE_SEPARATOR` | `"\x1C"` | ASCII file separator used to separate cached and dynamic content. |

### Option Flags

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_OPTION_NONE` | `0` | No options. |
| `CMS_TEMPLATE_OPTION_HREF` | `1` | Enable href elements. |
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
| `CMS_TEMPLATE_OPTION_ALL` | `CMS_TEMPLATE_OPTION_LAYOUT \| CMS_TEMPLATE_OPTION_EDIT` | All options. |

### Action Types

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_ACTION` | `0` | Action type for element actions. |
| `CMS_TEMPLATE_CONTROL` | `1` | Action type for control actions. |
| `CMS_TEMPLATE_LOCK` | `2` | Lock state for preview mode. |
| `CMS_TEMPLATE_CODE` | `3` | Code field for actions. |
| `CMS_TEMPLATE_IMAGE` | `4` | Image field for actions. |
| `CMS_TEMPLATE_COMMAND` | `5` | Command field for actions. |
| `CMS_TEMPLATE_SWITCH` | `6` | Switch field for control actions. |

### Element Types

| Name | Value | Description |
|------|-------|-------------|
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
| `CMS_TEMPLATE_TYPE_ALL` | `4294967295` | All types. |

### Type Filters

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_TYPE_EDIT` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_PLUGIN \| CMS_TEMPLATE_TYPE_TEXT \| CMS_TEMPLATE_TYPE_VALUE \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_IMAGE \| CMS_TEMPLATE_TYPE_THUMBNAIL \| CMS_TEMPLATE_TYPE_MEDIA \| CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_SWITCH` | Editable element types. |
| `CMS_TEMPLATE_TYPE_SPAN` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_CBLOCK \| CMS_TEMPLATE_TYPE_CALT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE \| CMS_TEMPLATE_TYPE_NOCACHE \| CMS_TEMPLATE_TYPE_CEDIT \| CMS_TEMPLATE_TYPE_CNOEDIT` | Spannable element types. |
| `CMS_TEMPLATE_TYPE_PATH` | `CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE` | Path-extending element types. |

### Command Types

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_COMMAND_NONE` | `0` | No command. |
| `CMS_TEMPLATE_COMMAND_BUFFER` | `1` | Buffer command. |
| `CMS_TEMPLATE_COMMAND_PASTE` | `2` | Paste command. |
| `CMS_TEMPLATE_COMMAND_SWAP` | `4` | Swap command. |
| `CMS_TEMPLATE_COMMAND_KICK1` | `8` | Kick command (primary). |
| `CMS_TEMPLATE_COMMAND_KICK2` | `16` | Kick command (secondary). |
| `CMS_TEMPLATE_COMMAND_DROP1` | `32` | Drop command (primary). |
| `CMS_TEMPLATE_COMMAND_DROP2` | `64` | Drop command (secondary). |
| `CMS_TEMPLATE_COMMAND_RELEASE` | `128` | Release command. |
| `CMS_TEMPLATE_COMMAND_REFERENCE` | `256` | Reference command. |
| `CMS_TEMPLATE_COMMAND_EXPORT` | `512` | Export command. |
| `CMS_TEMPLATE_COMMAND_CLEAR` | `1024` | Clear command. |
| `CMS_TEMPLATE_COMMAND_DRAGDROP1` | `2048` | Drag-and-drop command (primary). |
| `CMS_TEMPLATE_COMMAND_DRAGDROP2` | `4048` | Drag-and-drop command (secondary). |
| `CMS_TEMPLATE_COMMAND_ALL` | `4294967295` | All commands. |

### Structure Indices

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_STRUCTURE_INDEX` | `0` | Index of the element in the structure. |
| `CMS_TEMPLATE_STRUCTURE_PATH` | `1` | Path of the element. |
| `CMS_TEMPLATE_STRUCTURE_PARENT` | `2` | Parent index of the element. |
| `CMS_TEMPLATE_STRUCTURE_TYPE` | `3` | Type of the element. |

---

## Utility Functions

### `template_get_array`

**Purpose:**
Retrieves a categorized list of templates from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `NULL\|bool` | Filter for page templates (`TRUE`), modular templates (`FALSE`), or all templates (`NULL`). |

**Return Values:**
- `array`: Associative array of templates grouped by category.

**Inner Mechanisms:**
- Queries the `#system/template` dataset.
- Groups templates by their `category` field.
- Uses localized names if available; falls back to the template index if not.

**Usage:**
- Used to populate template selection lists in the admin interface.

---

### `template_get_select`

**Purpose:**
Retrieves a list of template categories for use in a `<select>` element.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `NULL\|bool` | Filter for page templates (`TRUE`), modular templates (`FALSE`), or all templates (`NULL`). |

**Return Values:**
- `array`: Associative array of categories with empty values.

**Inner Mechanisms:**
- Queries the `#system/template` dataset.
- Collects unique categories.

**Usage:**
- Used to populate category selection dropdowns in the admin interface.

---

### `template_parse_reference`

**Purpose:**
Parses a reference string (e.g., `content://123`, `directory://456`) into a structured array containing the name, description, and URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Reference string to parse. |
| `$analyze` | `bool` | If `TRUE`, returns the analyzed URL components instead of the parsed reference. |

**Return Values:**
- `array\|FALSE`: Associative array with keys `name`, `description`, and `url`, or `FALSE` if the reference is invalid.

**Inner Mechanisms:**
- Uses `analyze_url` to break down the reference.
- Fetches additional data from the `directory` or `content` modules if the reference points to them.

**Usage:**
- Used to resolve references in `href`, `image`, and `download` elements.

---

### `template_read_plugin`

**Purpose:**
Fetches the content of a plugin from a given URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL of the plugin to fetch. |

**Return Values:**
- `string\|FALSE`: The fetched content or `FALSE` on failure.

**Inner Mechanisms:**
- Uses the `http` module to fetch the content.

**Usage:**
- Used to embed external content via the `plugin` element.

---

### `template_preview`

**Purpose:**
Generates a preview of a template for the admin interface.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index_or_code` | `string` | Template index or raw template code. |
| `$is_index` | `bool` | If `TRUE`, treats `$index_or_code` as a template index; otherwise, treats it as raw code. |
| `$document` | `document\|NULL` | Document object to use for the preview. If `NULL`, a new document is created. |

**Return Values:**
- `void`: Outputs the preview directly to the browser.

**Inner Mechanisms:**
- Defines dummy actions for editable elements.
- Wraps the template in a basic HTML structure.
- Uses the `template` class to parse the template.

**Usage:**
- Used in the admin interface to preview templates before applying them.

---

### `template_lock`

**Purpose:**
Generates an overlay that locks the page for preview mode, preventing interactions.

**Parameters:**
- None.

**Return Values:**
- `string`: HTML code for the lock overlay.

**Usage:**
- Used in preview mode to prevent accidental edits.

---

### `template_error`

**Purpose:**
Generates an error message for template-related errors.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$code` | `int` | Error code. |
| `$message` | `string` | Error message. |
| `$path` | `string\|NULL` | Path to the file where the error occurred. |
| `$line` | `int\|NULL` | Line number where the error occurred. |

**Return Values:**
- `string`: HTML code for the error message.

**Inner Mechanisms:**
- Uses `cms_error` to generate the error message.
- Adjusts the line number based on the template context.

**Usage:**
- Used to display errors during template parsing or execution.

---

## `template` Class

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Dataset object for template data. |
| `$operator` | `bool` | Whether the current user has operator permissions. |
| `$tlist` | `array` | Mapping of template element names to their type constants. |
| `$tname` | `array` | Mapping of template type constants to localized names. |
| `$toption` | `array` | Mapping of template type constants to their option flags. |
| `$action` | `array\|NULL` | Actions and commands for editable elements. |
| `$image` | `image\|NULL` | Image module instance. |
| `$media` | `media\|NULL` | Media module instance. |
| `$download` | `download\|NULL` | Download module instance. |
| `$structure_id` | `int\|NULL` | Current structure index during structure parsing. |
| `$parent_id` | `int\|NULL` | Parent index during structure parsing. |
| `$execute_vars` | `array` | Variables for template code execution. |
| `$title` | `string` | Document title. |
| `$description` | `string` | Document description. |
| `$keyword` | `string` | Document keywords. |
| `$header` | `string` | Additional header content. |
| `$query_data` | `array` | Querystring data for URL generation. |

---

### `__construct`

**Purpose:**
Initializes the template engine.

**Parameters:**
- None.

**Inner Mechanisms:**
- Initializes the `#system/template` dataset.
- Checks operator permissions.
- Creates necessary directories for template storage.

---

### `add`

**Purpose:**
Adds a new template to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Name of the template. |
| `$category` | `string\|NULL` | Category of the template. |
| `$page` | `bool\|NULL` | Whether the template is a page template. |
| `$code` | `string\|NULL` | Template code. |
| `$stylesheet` | `string\|NULL` | Stylesheet code. |

**Return Values:**
- `string\|FALSE`: The index of the new template or `FALSE` on failure.

**Inner Mechanisms:**
- Validates operator permissions.
- Sets a default name if none is provided.
- Inserts the template into the dataset and saves the code and stylesheet.

**Usage:**
- Used in the admin interface to create new templates.

---

### `set`

**Purpose:**
Updates an existing template's metadata.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template to update. |
| `$name` | `string\|NULL` | New name of the template. |
| `$category` | `string\|NULL` | New category of the template. |
| `$page` | `bool\|NULL` | Whether the template is a page template. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates operator permissions.
- Updates the template's metadata in the dataset.

**Usage:**
- Used in the admin interface to edit template properties.

---

### `get`

**Purpose:**
Retrieves a template's metadata.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template. |

**Return Values:**
- `array`: Associative array of template metadata.

**Usage:**
- Used to fetch template details for display or editing.

---

### `set_code`

**Purpose:**
Saves the code of a template for all enabled languages.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template. |
| `$text` | `string` | Template code. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Splits the code into language-specific versions using `language_get_array`.
- Saves each version to a separate file.
- Updates the temporary cache.

**Usage:**
- Used when saving template code in the admin interface.

---

### `get_code`

**Purpose:**
Retrieves the code of a template for a specific language.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `NULL`, uses the current language. If `FALSE`, returns a multilingual string. |

**Return Values:**
- `string`: Template code.

**Inner Mechanisms:**
- Checks the temporary cache first.
- Falls back to reading the code from the file system.
- If no code exists for the requested language, falls back to the default language.

**Usage:**
- Used during template parsing to retrieve the code.

---

### `set_stylesheet`

**Purpose:**
Saves the stylesheet of a template for all enabled languages.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template. |
| `$text` | `string` | Stylesheet code. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Splits the stylesheet into language-specific versions using `language_get_array`.
- Saves each version to a separate file.

**Usage:**
- Used when saving template stylesheets in the admin interface.

---

### `get_stylesheet`

**Purpose:**
Retrieves the stylesheet of a template for a specific language.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `NULL`, uses the current language. If `FALSE`, returns a multilingual string. |
| `$check` | `bool` | If `TRUE`, returns the file path if the file exists, or `FALSE` otherwise. |

**Return Values:**
- `string\|FALSE`: Stylesheet code or file path, or `FALSE` if the file does not exist.

**Inner Mechanisms:**
- Reads the stylesheet from the file system.
- Falls back to the default language if no stylesheet exists for the requested language.

**Usage:**
- Used during template parsing to link the stylesheet.

---

### `delete`

**Purpose:**
Deletes a template and its associated files.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the template to delete. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Deletes all language-specific code and stylesheet files.
- Removes the template from the dataset.

**Usage:**
- Used in the admin interface to delete templates.

---

### `parse`

**Purpose:**
Parses a template and generates the output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for parsing. |
| `$index` | `string` | Index of the template to parse. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and commands for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$cache` | `bool` | Whether to enable caching. |

**Return Values:**
- `string`: Generated HTML output.

**Inner Mechanisms:**
- Links the template's stylesheet.
- Sets up meta data and actions.
- Calls `_parse` to process the template code.

**Usage:**
- Used to render templates for display.

---

### `parse_code`

**Purpose:**
Parses raw template code and generates the output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for parsing. |
| `$template` | `string` | Template code to parse. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and commands for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |

**Return Values:**
- `string`: Generated HTML output.

**Inner Mechanisms:**
- Sets up meta data and actions.
- Calls `_parse` to process the template code.

**Usage:**
- Used to parse raw template code, e.g., for previews or dynamic templates.

---

### `_parse`

**Purpose:**
Core template parsing function. Processes template code and generates HTML output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for parsing. |
| `$template` | `string` | Template code to parse. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$cache` | `bool\|int` | Caching mode: `TRUE` for caching, `FALSE` for no caching, `-1` to finish a no-cache span. |
| `$template_index` | `string\|NULL` | Index of the template (for caching). |

**Return Values:**
- `string`: Generated HTML output.

**Inner Mechanisms:**
- Tokenizes the template code and PHP blocks.
- Processes each `<CMS:*>` tag and its attributes.
- Handles nested templates, conditional blocks, and editable elements.
- Generates debug information if enabled.
- Manages caching and dynamic content.

**Usage:**
- Called by `parse` and `parse_code` to process template code.

---

### `export`

**Purpose:**
Exports a template and its content as a standalone template file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for export. |
| `$index` | `string` | Index of the template to export. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$return_stylesheet` | `bool` | If `TRUE`, returns the combined stylesheet of all used templates. |

**Return Values:**
- `string\|array`: Exported template code, or an associative array with keys `code` and `stylesheet` if `$return_stylesheet` is `TRUE`.

**Inner Mechanisms:**
- Processes the template code to resolve references and conditionals.
- Combines stylesheets of all used templates if requested.

**Usage:**
- Used to export templates for sharing or backup.

---

### `structure`

**Purpose:**
Generates a structure map of a template, showing the hierarchy of elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for structure generation. |
| `$index` | `string` | Index of the template. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |

**Return Values:**
- `array\|FALSE`: Associative array representing the structure, or `FALSE` if the template is empty.

**Inner Mechanisms:**
- Processes the template code to build a hierarchical structure of elements.
- Tracks parent-child relationships.

**Usage:**
- Used in the admin interface to visualize template structure.

---

### `create_cache`

**Purpose:**
Generates a cache version of a template, separating static and dynamic content.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for caching. |
| `$index` | `string` | Index of the template. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and commands for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `&$is_dynamic` | `bool` | Output parameter indicating whether dynamic content was found. |

**Return Values:**
- `array`: Associative array with keys `cache` (cached content) and `output` (processed output).

**Inner Mechanisms:**
- Uses `parse` to generate the output.
- Separates static and dynamic content using `CMS_TEMPLATE_CACHE_SEPARATOR`.

**Usage:**
- Used to optimize performance by caching static content.

---

### `process_cache`

**Purpose:**
Processes a cached template, injecting dynamic content.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to use for processing. |
| `$template` | `string` | Cached template content. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and commands for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `&$is_dynamic` | `bool` | Output parameter indicating whether dynamic content was found. |

**Return Values:**
- `string`: Processed HTML output.

**Inner Mechanisms:**
- Splits the cached content into static and dynamic parts.
- Processes dynamic parts using `_parse`.

**Usage:**
- Used to render cached templates with dynamic content.

---

### `execute`

**Purpose:**
Executes PHP code embedded in a template.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$cms_template_code` | `string` | PHP code to execute. |
| `$cms_template_document` | `document` | Document object. |
| `$cms_template_base_id` | `string` | Base ID for element indexing. |
| `$cms_template_path_id` | `string` | Path ID for element indexing. |
| `$cms_template_temp_id` | `string` | Temporary ID for variable isolation. |

**Return Values:**
- `mixed`: Return value of the executed code.

**Inner Mechanisms:**
- Temporarily replaces the error handler to catch template errors.
- Extracts and stores variables to isolate execution contexts.

**Usage:**
- Called by `_parse` to execute embedded PHP code.

---

### `attribute`

**Purpose:**
Generates an HTML attribute string from an associative array of attributes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | Associative array of attributes. |
| `$alter` | `array\|NULL` | Additional attributes to merge. |
| `$skip` | `string\|array\|NULL` | Attributes to skip. |

**Return Values:**
- `string`: HTML attribute string.

**Inner Mechanisms:**
- Merges additional attributes.
- Skips specified attributes and internal fields.
- Escapes attribute values.

**Usage:**
- Used to generate HTML attributes for template elements.


<!-- HASH:ca472aee323b050863ed93fb816dde07 -->
