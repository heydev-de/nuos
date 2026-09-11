# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.document.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.document.inc)

- **Version:** `26.9.9.7`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC Document Class

The `document` class is a core component of the PWNC Web Platform, providing a structured way to manage document data with support for templates, references, and hierarchical operations. It serves as a flexible container for content that can be imported, manipulated, and exported in a standardized format.

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DOCUMENT_SEPARATOR` | `"\x1E"` | ASCII record separator used to delimit document entries |
| `CMS_DOCUMENT_TYPE` | `"0"` | Index for the type field in document data arrays |
| `CMS_DOCUMENT_VALUE` | `"1"` | Index for the value field in document data arrays |
| `CMS_DOCUMENT_REFERENCE` | `"2"` | Index for the reference field in document data arrays |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | array | `[]` | Main storage for document elements |
| `$default` | array | `[]` | Default values for document elements |
| `$template_index` | mixed | `NULL` | Index of the active template |
| `$structure` | mixed | `NULL` | Parsed template structure |

## Methods

### `__construct($text = NULL, $template_index = NULL)`

Initializes a new document instance, optionally importing text data and setting a template structure.

**Parameters:**
- `$text` (string|null): Text to import into the document
- `$template_index` (mixed|null): Template index to set as structure

**Usage Example:**
```php
$doc = new document("title:text:Hello World", "main_template");
```

### `import($text)`

Parses and imports text data into the document using the record separator.

**Parameters:**
- `$text` (string): Text data to import

**Inner Mechanisms:**
- Splits text by `CMS_DOCUMENT_SEPARATOR`
- Uses regex to parse each entry into key, type, and value
- Handles `#reference` types by calling `resolve_reference()`

**Usage Example:**
```php
$doc->import("title:text:Hello\x1Ebody:html:<p>Content</p>");
```

### `resolve_reference($id)`

Resolves a reference entry by loading content from the content pool.

**Parameters:**
- `$id` (string): Identifier of the reference to resolve

**Inner Mechanisms:**
- Loads the `content_pool` library
- Retrieves referenced text and parses it
- Merges resolved data with existing entries
- Handles relative key references with `..` prefix

**Usage Example:**
```php
$doc->set("content", "#reference", "page_intro");
$doc->resolve_reference("content");
```

### `set_structure($template_index)`

Sets the document structure based on a template.

**Parameters:**
- `$template_index` (mixed): Template index to use

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$doc->set_structure("article_template");
```

### `update_structure()`

Re-applies the current template structure to the document.

**Return Values:**
- `bool`: Result of `set_structure()` call

**Usage Example:**
```php
$doc->update_structure();
```

### `export($resolve_references = TRUE, $cleanup = TRUE)`

Exports document data as a string.

**Parameters:**
- `$resolve_references` (bool): Whether to resolve references in output
- `$cleanup` (bool): Whether to filter data based on structure

**Return Values:**
- `string`: Exported document data

**Inner Mechanisms:**
- Optionally filters data based on template structure
- Handles three cases: resolved references, unresolved references, and regular values
- Joins entries with `CMS_DOCUMENT_SEPARATOR`

**Usage Example:**
```php
$data = $doc->export(TRUE, TRUE);
```

### `get($id, $type = NULL, $use_default = TRUE)`

Retrieves a value from the document.

**Parameters:**
- `$id` (string): Element identifier
- `$type` (string|null): Expected type filter
- `$use_default` (bool): Whether to fall back to default values

**Return Values:**
- `string|null`: The value if found, `NULL` otherwise

**Inner Mechanisms:**
- Checks data array first, then defaults if enabled
- Validates type compatibility when specified
- Returns `TRUE` immediately for "group" type requests

**Usage Example:**
```php
$title = $doc->get("title", "text");
```

### `get_reference($id, $type = NULL)`

Retrieves the reference status of an element.

**Parameters:**
- `$id` (string): Element identifier
- `$type` (string|null): Expected type filter

**Return Values:**
- `mixed`: Reference value or `NULL` if not found

**Usage Example:**
```php
$ref = $doc->get_reference("content");
```

### `get_parent_template($path)`

Finds the parent template path for a given element path.

**Parameters:**
- `$path` (string): Element path to search from

**Return Values:**
- `string|null`: Parent template path or `NULL` if not found

**Inner Mechanisms:**
- Traverses the structure hierarchy upward
- Returns the first template-type ancestor

**Usage Example:**
```php
$parent = $doc->get_parent_template("section.content");
```

### `get_last_child($id)`

Finds the highest numeric child index for a given parent ID.

**Parameters:**
- `$id` (string): Parent element identifier

**Return Values:**
- `int`: Highest numeric child index

**Inner Mechanisms:**
- Scans data for keys starting with `$id.`
- Extracts and compares numeric segments

**Usage Example:**
```php
$last = $doc->get_last_child("items");
```

### `set($id, $type, $value)`

Sets a value in the document data.

**Parameters:**
- `$id` (string): Element identifier
- `$type` (string): Element type
- `$value` (string): Element value

**Inner Mechanisms:**
- Automatically resolves references when type is `#reference`

**Usage Example:**
```php
$doc->set("title", "text", "New Title");
```

### `set_default($id, $type, $value)`

Sets a default value for an element.

**Parameters:**
- `$id` (string): Element identifier
- `$type` (string): Element type
- `$value` (mixed): Default value

**Inner Mechanisms:**
- For "repeat" types, only increases values (never decreases)

**Usage Example:**
```php
$doc->set_default("items", "repeat", 5);
```

### `extract($path)`

Extracts a portion of the document as a new document instance.

**Parameters:**
- `$path` (string): Path of the element to extract

**Return Values:**
- `document`: New document containing extracted elements

**Inner Mechanisms:**
- Creates a new document instance
- Copies the initial element and its children
- Handles relative path references with `..` prefix

**Usage Example:**
```php
$excerpt = $doc->extract("section.content");
```

### `inject($path, $document)`

Injects elements from another document into this document.

**Parameters:**
- `$path` (string): Target path for injection
- `$document` (document): Source document to inject from

**Inner Mechanisms:**
- Maps source element keys to target paths
- Clears existing data at target path
- Injects elements and recomputes structure
- Fits unassigned elements into available slots

**Usage Example:**
```php
$doc->inject("section.content", $excerpt);
```

### `copy($path_source, $path_target)`

Copies elements from one path to another.

**Parameters:**
- `$path_source` (string): Source path
- `$path_target` (string): Target path

**Usage Example:**
```php
$doc->copy("section.content", "section.sidebar");
```

### `swap($path_source, $path_target)`

Swaps elements between two paths.

**Parameters:**
- `$path_source` (string): First path
- `$path_target` (string): Second path

**Usage Example:**
```php
$doc->swap("section.content", "section.sidebar");
```

### `kick($path, $value)`

Moves elements within the document structure.

**Parameters:**
- `$path` (string): Path of the element to move
- `$value` (int): Number of positions to move (negative for reverse)

**Inner Mechanisms:**
- Calculates element depth and relevant parent
- Pre-seeds repeat blocks when moving forward
- Extracts and re-injects elements at target positions

**Usage Example:**
```php
$doc->kick("items.2", 1); // Move item 2 one position forward
```

### `drop($path, $value)`

Removes an element and shifts others to fill the gap.

**Parameters:**
- `$path` (string): Path of the element to remove
- `$value` (int): Direction of shift (positive or negative)

**Inner Mechanisms:**
- Removes the initial element
- Shifts compatible elements to fill the gap
- Maintains structural integrity

**Usage Example:**
```php
$doc->drop("items.2", 1); // Remove item 2 and shift others
```

### `shift($id, $value)`

Shifts elements within a shift container.

**Parameters:**
- `$id` (string): Container element identifier
- `$value` (int): Number of positions to shift

**Inner Mechanisms:**
- Identifies container and child elements
- Finds compatible target positions
- Extracts and re-injects elements

**Usage Example:**
```php
$doc->shift("carousel", 1); // Shift carousel items forward
```

### `del($path)`

Deletes an element and its children from the document.

**Parameters:**
- `$path` (string): Path of the element to delete

**Inner Mechanisms:**
- Removes the initial element data
- Recursively removes child element data for templates and groups

**Usage Example:**
```php
$doc->del("section.content");
```

### `cleanup()`

Removes data elements not present in the current structure.

**Inner Mechanisms:**
- Builds a list of valid element IDs and types from structure
- Filters data array to only include valid elements

**Usage Example:**
```php
$doc->cleanup();
```


<!-- HASH:060b16720e9cad4341651321248ec766 -->
