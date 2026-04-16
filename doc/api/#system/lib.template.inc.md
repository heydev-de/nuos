# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.template.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Template System

The `lib.template.inc` file provides the core template engine for the NUOS web platform, enabling dynamic content rendering, conditional logic, and structural organization of web pages. It includes utility functions for template management and a `template` class that handles parsing, caching, and exporting of templates.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for template operators. |
| `CMS_TEMPLATE_CACHE_SEPARATOR` | `"\x1C"` | ASCII file separator used for template caching. |
| `CMS_TEMPLATE_OPTION_*` | Bitmask values | Option flags for template elements (e.g., `CMS_TEMPLATE_OPTION_HREF`, `CMS_TEMPLATE_OPTION_IMAGE`). |
| `CMS_TEMPLATE_OPTION_LAYOUT` | Combined bitmask | Combines layout-related options. |
| `CMS_TEMPLATE_OPTION_EDIT` | Combined bitmask | Combines edit-related options. |
| `CMS_TEMPLATE_OPTION_ALL` | Combined bitmask | Combines all options. |
| `CMS_TEMPLATE_ACTION` | `0` | Index for action-related data. |
| `CMS_TEMPLATE_CONTROL` | `1` | Index for control-related data. |
| `CMS_TEMPLATE_LOCK` | `2` | Index for lock-related data. |
| `CMS_TEMPLATE_CODE` | `3` | Index for code-related data. |
| `CMS_TEMPLATE_IMAGE` | `4` | Index for image-related data. |
| `CMS_TEMPLATE_COMMAND` | `5` | Index for command-related data. |
| `CMS_TEMPLATE_SWITCH` | `6` | Index for switch-related data. |
| `CMS_TEMPLATE_TYPE_*` | Bitmask values | Type flags for template elements (e.g., `CMS_TEMPLATE_TYPE_HREF`, `CMS_TEMPLATE_TYPE_IMAGE`). |
| `CMS_TEMPLATE_TYPE_EDIT` | Combined bitmask | Combines editable element types. |
| `CMS_TEMPLATE_TYPE_SPAN` | Combined bitmask | Combines spannable element types. |
| `CMS_TEMPLATE_TYPE_PATH` | Combined bitmask | Combines path-extending element types. |
| `CMS_TEMPLATE_TYPE_ALL` | `4294967295` | All type flags combined. |
| `CMS_TEMPLATE_COMMAND_*` | Bitmask values | Command flags for template actions (e.g., `CMS_TEMPLATE_COMMAND_BUFFER`, `CMS_TEMPLATE_COMMAND_CLEAR`). |
| `CMS_TEMPLATE_COMMAND_ALL` | `4294967295` | All command flags combined. |
| `CMS_TEMPLATE_STRUCTURE_INDEX` | `0` | Index for element identifier in structure. |
| `CMS_TEMPLATE_STRUCTURE_PATH` | `1` | Index for element path in structure. |
| `CMS_TEMPLATE_STRUCTURE_PARENT` | `2` | Index for parent identifier in structure. |
| `CMS_TEMPLATE_STRUCTURE_TYPE` | `3` | Index for element type in structure. |

---

## Utility Functions

### `template_get_array`

Retrieves a categorized list of templates for selection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filters templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array of templates grouped by category. |

#### Inner Mechanisms

- Queries the `#system/template` dataset.
- Groups templates by their `category` field.
- Localizes template names using `l()`.
- Handles duplicate names by appending a counter.

#### Usage

- Used in administrative interfaces to populate template selection dropdowns.

---

### `template_get_select`

Retrieves a list of template categories for selection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filters templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array of template categories. |

#### Inner Mechanisms

- Queries the `#system/template` dataset.
- Extracts unique categories from templates.

#### Usage

- Used in administrative interfaces to populate category selection dropdowns.

---

### `template_parse_reference`

Parses and resolves template references (e.g., `content://`, `directory://`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Reference string to parse. |
| `$analyze` | `bool` | If `TRUE`, returns analyzed URL components instead of resolved data. |

#### Return Values

| Type | Description |
|------|-------------|
| `array\|FALSE` | Resolved reference data (`name`, `description`, `url`) or `FALSE` if invalid. |

#### Inner Mechanisms

- Handles deprecated formats (e.g., `address:`).
- Uses `analyze_url()` to parse the reference.
- Resolves references based on the scheme (`directory`, `content`, `address`).
- Retrieves metadata from the respective datasets.

#### Usage

- Used to resolve references in `href`, `image`, and `download` elements.

---

### `template_read_plugin`

Fetches content from a remote URL for use in `plugin` elements.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL to fetch content from. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Fetched content or `FALSE` on failure. |

#### Inner Mechanisms

- Uses the `http` library to fetch data.
- Returns `FALSE` if the library is unavailable or the request fails.

#### Usage

- Used to embed external content in `plugin` elements.

---

### `template_preview`

Generates a preview of a template for editing.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index_or_code` | `string` | Template index or raw template code. |
| `$is_index` | `bool` | If `TRUE`, treats `$index_or_code` as a template index. |
| `$document` | `document\|NULL` | Document object to use for preview. Creates a new one if `NULL`. |

#### Inner Mechanisms

- Defines dummy actions for editable elements.
- Uses the `template` class to parse the template.
- Embeds the template in a simple HTML frame.
- Displays the preview with a lock overlay if in preview mode.

#### Usage

- Used in the administrative interface to preview templates before saving.

---

### `template_lock`

Generates an overlay to lock the document in preview mode.

#### Return Values

| Type | Description |
|------|-------------|
| `string` | HTML anchor tag with styles to lock the document. |

#### Usage

- Used to prevent interaction with the document during preview.

---

### `template_error`

Generates an error message for template-related issues.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$code` | `int` | Error code. |
| `$message` | `string` | Error message. |
| `$path` | `string\|NULL` | File path where the error occurred. |
| `$line` | `int\|NULL` | Line number where the error occurred. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Formatted error message. |

#### Inner Mechanisms

- Uses `cms_error()` to generate the error message.
- Adjusts the line number based on the template context.

#### Usage

- Used to display errors during template parsing or execution.

---

## `template` Class

The `template` class provides methods for managing, parsing, and exporting templates.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Dataset object for template data. |
| `$operator` | `bool` | Indicates if the current user has operator permissions. |
| `$tlist` | `array` | Maps template element names to their type flags. |
| `$tname` | `array` | Maps type flags to localized names. |
| `$toption` | `array` | Maps type flags to their option flags. |
| `$action` | `array\|NULL` | Actions and controls for editable elements. |
| `$image` | `image\|NULL` | Image library instance. |
| `$media` | `media\|NULL` | Media library instance. |
| `$download` | `download\|NULL` | Download library instance. |
| `$structure_id` | `int\|NULL` | Current structure index. |
| `$parent_id` | `int\|NULL` | Current parent index in structure. |
| `$execute_vars` | `array` | Variables for template execution. |
| `$title` | `string` | Document title. |
| `$description` | `string` | Document description. |
| `$keyword` | `string` | Document keywords. |
| `$header` | `string` | Additional header content. |
| `$query_data` | `array` | Querystring data for URL generation. |

---

### `__construct`

Initializes the template class.

#### Inner Mechanisms

- Initializes the `#system/template` dataset.
- Sets operator permissions using `cms_permission()`.
- Creates necessary directories for template storage.

---

### `add`

Adds a new template to the dataset.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Template name. |
| `$category` | `string\|NULL` | Template category. |
| `$page` | `bool\|NULL` | If `TRUE`, marks the template as a page template. |
| `$code` | `string\|NULL` | Template code. |
| `$stylesheet` | `string\|NULL` | Template stylesheet. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Template index on success, `FALSE` on failure. |

#### Inner Mechanisms

- Validates operator permissions.
- Sets a default name if none is provided.
- Inserts the template into the dataset.
- Saves the template code and stylesheet if provided.

#### Usage

- Used in administrative interfaces to create new templates.

---

### `set`

Updates an existing template's metadata.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$name` | `string\|NULL` | New template name. |
| `$category` | `string\|NULL` | New template category. |
| `$page` | `bool\|NULL` | New page status. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Validates operator permissions.
- Updates the template's metadata in the dataset.
- Saves the dataset.

#### Usage

- Used in administrative interfaces to update template metadata.

---

### `get`

Retrieves a template's metadata.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Template metadata. |

#### Inner Mechanisms

- Retrieves data from the `#system/template` dataset.

#### Usage

- Used to display template metadata in administrative interfaces.

---

### `set_code`

Saves template code to files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | Template code. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Validates operator permissions.
- Splits the code into language-specific files.
- Writes each language version to a separate file.
- Updates the temporary cache.

#### Usage

- Used to save template code during editing.

---

### `get_code`

Retrieves template code from files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. `NULL` for active language, `FALSE` for all languages. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Template code. |

#### Inner Mechanisms

- Retrieves code from the temporary cache or files.
- Falls back to the default language if the specified language is unavailable.

#### Usage

- Used to load template code for parsing or editing.

---

### `set_stylesheet`

Saves template stylesheet to files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | Stylesheet code. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Validates operator permissions.
- Splits the stylesheet into language-specific files.
- Writes each language version to a separate file.

#### Usage

- Used to save template stylesheets during editing.

---

### `get_stylesheet`

Retrieves template stylesheet from files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. `NULL` for active language, `FALSE` for all languages. |
| `$check` | `bool` | If `TRUE`, checks for file existence instead of reading content. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|bool` | Stylesheet code or file path if `$check` is `TRUE`. `FALSE` if the file does not exist. |

#### Inner Mechanisms

- Retrieves stylesheet from files.
- Falls back to the default language if the specified language is unavailable.

#### Usage

- Used to load stylesheets for linking in parsed templates.

---

### `delete`

Deletes a template and its associated files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms

- Validates operator permissions.
- Deletes all language-specific template and stylesheet files.
- Removes the template from the dataset.

#### Usage

- Used in administrative interfaces to delete templates.

---

### `parse`

Parses a template and renders it into a document.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to populate. |
| `$index` | `string` | Template index. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and controls for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$cache` | `bool` | If `TRUE`, enables caching. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Rendered HTML output. |

#### Inner Mechanisms

- Links the template's stylesheet.
- Sets up actions, meta data, and header content.
- Calls `_parse()` to process the template.

#### Usage

- Used to render templates for display or editing.

---

### `parse_code`

Parses raw template code and renders it into a document.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to populate. |
| `$template` | `string` | Template code. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and controls for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Rendered HTML output. |

#### Inner Mechanisms

- Sets up actions, meta data, and header content.
- Calls `_parse()` to process the template code.

#### Usage

- Used to render raw template code for display or editing.

---

### `_parse`

Core method for parsing template code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to populate. |
| `$template` | `string` | Template code. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$cache` | `bool\|int` | Caching mode. `TRUE` for caching, `FALSE` for no caching, `-1` to finish a nocache span. |
| `$template_index` | `string\|NULL` | Template index for caching. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Rendered HTML output. |

#### Inner Mechanisms

- Tokenizes and caches template code.
- Processes PHP code blocks and template tags.
- Handles element attributes, IDs, and values.
- Manages conditional blocks, groups, repeats, and shifts.
- Supports editable elements with drag-and-drop functionality.
- Generates debug information if enabled.
- Injects meta data, stylesheets, and scripts into the document head.

#### Usage

- Internally called by `parse()` and `parse_code()` to render templates.

---

### `export`

Exports a template and its content for external use.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to export. |
| `$index` | `string` | Template index. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |
| `$return_stylesheet` | `bool` | If `TRUE`, returns stylesheet data alongside the template code. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|array` | Exported template code. If `$return_stylesheet` is `TRUE`, returns an array with `code` and `stylesheet` keys. |

#### Inner Mechanisms

- Processes template tags and attributes.
- Handles conditional blocks, groups, repeats, and shifts.
- Resolves template references and includes subtemplates.
- Combines stylesheets of all employed templates if `$return_stylesheet` is `TRUE`.

#### Usage

- Used to export templates and their content for backup or migration.

---

### `structure`

Generates a structural representation of a template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to analyze. |
| `$index` | `string` | Template index. |
| `$base_id` | `string\|NULL` | Base ID for element indexing. |

#### Return Values

| Type | Description |
|------|-------------|
| `array\|FALSE` | Associative array representing the template structure or `FALSE` on failure. |

#### Inner Mechanisms

- Processes template tags and attributes.
- Tracks parent-child relationships between elements.
- Returns an array with element indices, paths, parents, and types.

#### Usage

- Used to analyze template structures for administrative purposes.

---

### `create_cache`

Creates a cacheable version of a parsed template.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to populate. |
| `$index` | `string` | Template index. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and controls for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `&$is_dynamic` | `bool` | Output parameter indicating if the template contains dynamic content. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array with `cache` and `output` keys. `cache` contains the cacheable template, and `output` contains the rendered HTML. |

#### Inner Mechanisms

- Calls `parse()` to render the template.
- Splits the output into cached and dynamic parts using `CMS_TEMPLATE_CACHE_SEPARATOR`.
- Returns both the cacheable template and the rendered output.

#### Usage

- Used to generate cacheable templates for performance optimization.

---

### `process_cache`

Processes a cached template to render dynamic content.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$document` | `document` | Document object to populate. |
| `$template` | `string` | Cached template. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Actions and controls for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `&$is_dynamic` | `bool` | Output parameter indicating if the template contains dynamic content. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Rendered HTML output. |

#### Inner Mechanisms

- Splits the cached template into static and dynamic parts.
- Processes dynamic parts using `_parse()`.
- Combines static and dynamic parts into the final output.

#### Usage

- Used to render cached templates with dynamic content.

---

### `execute`

Executes PHP code within a template context.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$cms_template_code` | `string` | PHP code to execute. |
| `$cms_template_document` | `document` | Document object. |
| `$cms_template_base_id` | `string` | Base ID for element indexing. |
| `$cms_template_path_id` | `string` | Path ID for element indexing. |
| `$cms_template_temp_id` | `string` | Temporary ID for variable storage. |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Return value of the executed code. |

#### Inner Mechanisms

- Temporarily replaces the error handler with `template_error()`.
- Imports variables from `$execute_vars`.
- Executes the code within the current namespace.
- Stores defined variables back into `$execute_vars`.
- Restores the original error handler.

#### Usage

- Internally called by `_parse()` to execute PHP code blocks in templates.

---

### `attribute`

Generates HTML attributes from an associative array.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | Associative array of attributes. |
| `$alter` | `array\|NULL` | Additional attributes to merge. |
| `$skip` | `string\|array\|NULL` | Attributes to skip. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | HTML attribute string or `FALSE` on failure. |

#### Inner Mechanisms

- Merges `$alter` into `$param` if provided.
- Skips specified attributes and internal keys (e.g., `_id`).
- Escapes attribute values using `x()`.

#### Usage

- Used to generate HTML attributes for template elements.


<!-- HASH:5b3582b2ef8df170682cdf98ee248dec -->
