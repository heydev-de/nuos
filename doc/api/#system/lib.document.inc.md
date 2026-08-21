# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.document.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.document.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Document Class

The `document` class in the PWNC Web Platform is a structured data container designed to manage hierarchical content with support for templates, references, and dynamic manipulation. It serves as a core component for content management, enabling the import, export, and transformation of structured documents while maintaining type safety and reference integrity.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DOCUMENT_SEPARATOR` | `"\x1E"` | ASCII record separator used to delimit document elements during serialization. |
| `CMS_DOCUMENT_TYPE` | `"0"` | Array index for element type in the internal data structure. |
| `CMS_DOCUMENT_VALUE` | `"1"` | Array index for element value in the internal data structure. |
| `CMS_DOCUMENT_REFERENCE` | `"2"` | Array index for reference identifier in the internal data structure. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$data` | `[]` | Associative array storing document elements as `[id => [type, value, reference]]`. |
| `$default` | `[]` | Associative array storing default values for document elements. |
| `$template_index` | `NULL` | Identifier of the template used to structure the document. |
| `$structure` | `NULL` | Parsed template structure defining the document hierarchy. |

---

### Constructor

#### `__construct($text = NULL, $template_index = NULL)`

**Purpose:**
Initializes a new document instance, optionally importing serialized content and applying a template structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string\|NULL` | Serialized document string to import. If `NULL`, an empty document is created. |
| `$template_index` | `string\|NULL` | Template identifier to structure the document. If `NULL`, no structure is applied. |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
- If `$text` is provided, the `import()` method is called to deserialize the content.
- If `$template_index` is provided, the `set_structure()` method is called to apply the template.

**Usage Context:**
- Used to instantiate a new document, typically when loading content from storage or creating a new document from scratch.

**Example:**
```php
// Create an empty document with a template
$doc = new \cms\document(NULL, "article_template");
```

---

### Methods

#### `import($text)`

**Purpose:**
Deserializes a document string into the internal data structure, parsing element IDs, types, and values.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Serialized document string using `CMS_DOCUMENT_SEPARATOR` as delimiter. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Splits the input string by `CMS_DOCUMENT_SEPARATOR`.
- Parses each segment using regex to extract `id:type:value` triplets.
- If the type is `#reference`, resolves the reference using `resolve_reference()`.
- Unmatched segments are stored as raw values with `NULL` type.

**Usage Context:**
- Used when loading a document from storage or transferring it between systems.

**Example:**
```php
$serialized = "title:text:Hello World\x1Econtent:html:<p>Content</p>";
$doc->import($serialized);
```

---

#### `resolve_reference($id)`

**Purpose:**
Resolves a reference element by fetching its content from the `content_pool` and merging it into the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Element ID whose reference is to be resolved. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Checks if the element exists and is a reference.
- Loads the `content_pool` library and fetches the referenced content.
- Merges the resolved content into the document, preserving hierarchical relationships.
- Handles relative paths (e.g., `..key`) and absolute paths (e.g., `parent.child`).

**Usage Context:**
- Automatically called during `import()` for elements with type `#reference`.
- Used to dynamically load reusable content fragments.

**Example:**
```php
$doc->set("intro", "#reference", "welcome_message");
$doc->resolve_reference("intro"); // Loads "welcome_message" from content_pool
```

---

#### `set_structure($template_index)`

**Purpose:**
Applies a template structure to the document, defining its hierarchy and element types.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$template_index` | `string` | Template identifier to structure the document. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Loads the `template` library and parses the template structure.
- Stores the parsed structure in `$this->structure` and the template identifier in `$this->template_index`.

**Usage Context:**
- Used to enforce a consistent structure on the document, enabling type validation and hierarchical operations.

**Example:**
```php
$doc->set_structure("article_template");
```

---

#### `update_structure()`

**Purpose:**
Reapplies the current template structure to the document, useful after dynamic modifications.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` if no template is set.

**Inner Mechanisms:**
- Calls `set_structure()` with the current `$this->template_index`.

**Usage Context:**
- Used after injecting or modifying elements to ensure the structure remains valid.

**Example:**
```php
$doc->inject("body", $new_content);
$doc->update_structure(); // Revalidate structure
```

---

#### `export($resolve_references = TRUE, $cleanup = TRUE)`

**Purpose:**
Serializes the document into a string, optionally resolving references and cleaning up unstructured elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$resolve_references` | `bool` | If `TRUE`, resolves references before exporting. If `FALSE`, exports references as-is. |
| `$cleanup` | `bool` | If `TRUE`, removes elements not defined in the template structure. |

**Return Values:**
- `string`: Serialized document string.

**Inner Mechanisms:**
- Iterates over `$this->data` and constructs a string using `CMS_DOCUMENT_SEPARATOR`.
- Skips resolved references if `$resolve_references` is `FALSE`.
- Filters out elements not matching the template structure if `$cleanup` is `TRUE`.

**Usage Context:**
- Used to save the document to storage or transfer it between systems.

**Example:**
```php
$serialized = $doc->export(TRUE, TRUE); // Resolve references and clean up
```

---

#### `get($id, $type = NULL, $use_default = TRUE)`

**Purpose:**
Retrieves the value of a document element, optionally validating its type and falling back to defaults.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Element ID to retrieve. |
| `$type` | `string\|NULL` | Expected type of the element. If `NULL`, type is not validated. |
| `$use_default` | `bool` | If `TRUE`, falls back to default values if the element is not found. |

**Return Values:**
- `string\|NULL`: Element value or `NULL` if not found.

**Inner Mechanisms:**
- Checks if the element exists and matches the expected type.
- Falls back to `$this->default` if `$use_default` is `TRUE` and the element is not found.

**Usage Context:**
- Used to access document content in templates or application logic.

**Example:**
```php
$title = $doc->get("title", "text"); // Returns the title as text
```

---

#### `get_reference($id, $type = NULL)`

**Purpose:**
Retrieves the reference identifier of a document element, if it exists.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Element ID to check. |
| `$type` | `string\|NULL` | Expected type of the element. If `NULL`, type is not validated. |

**Return Values:**
- `string\|NULL`: Reference identifier or `NULL` if not a reference.

**Inner Mechanisms:**
- Checks if the element exists and matches the expected type.
- Returns the reference identifier stored in `CMS_DOCUMENT_REFERENCE`.

**Usage Context:**
- Used to inspect references before resolving them.

**Example:**
```php
$ref = $doc->get_reference("intro"); // Returns "welcome_message" if intro is a reference
```

---

#### `get_parent_template($path)`

**Purpose:**
Retrieves the path of the nearest parent template for a given element path.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to inspect. |

**Return Values:**
- `string\|NULL`: Path of the parent template or `NULL` if not found.

**Inner Mechanisms:**
- Traverses the template structure upwards from the given path until a template element is found.

**Usage Context:**
- Used to determine the template context of an element for dynamic operations.

**Example:**
```php
$parent_template = $doc->get_parent_template("body.paragraph");
```

---

#### `set($id, $type, $value)`

**Purpose:**
Sets the value and type of a document element, optionally resolving references.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Element ID to set. |
| `$type` | `string` | Type of the element (e.g., `text`, `html`, `#reference`). |
| `$value` | `string` | Value of the element. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Stores the element in `$this->data` with the given type and value.
- If the type is `#reference`, calls `resolve_reference()` to load the referenced content.

**Usage Context:**
- Used to populate or modify document content.

**Example:**
```php
$doc->set("title", "text", "Hello World");
```

---

#### `set_default($id, $type, $value)`

**Purpose:**
Sets a default value for a document element, used as a fallback in `get()`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Element ID to set. |
| `$type` | `string` | Type of the element. |
| `$value` | `string` | Default value of the element. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Stores the default value in `$this->default`.

**Usage Context:**
- Used to define fallback values for optional elements.

**Example:**
```php
$doc->set_default("subtitle", "text", "Default Subtitle");
```

---

#### `extract($path)`

**Purpose:**
Extracts a subdocument from the given path, preserving hierarchical relationships.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to extract. |

**Return Values:**
- `\cms\document\|FALSE`: Extracted subdocument or `FALSE` if the path is invalid.

**Inner Mechanisms:**
- Creates a new `document` instance and copies the element and its children.
- Adjusts paths to be relative to the extracted element (e.g., `..key` for siblings).

**Usage Context:**
- Used to isolate a portion of the document for manipulation or transfer.

**Example:**
```php
$excerpt = $doc->extract("body.paragraph");
```

---

#### `inject($path, $document)`

**Purpose:**
Injects a subdocument into the given path, merging it with the existing structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path to inject the subdocument. |
| `$document` | `\cms\document` | Subdocument to inject. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Clears the target path and injects the subdocument elements.
- Recomputes the structure to accommodate the new elements.
- Fits unassigned elements into compatible positions in the structure.

**Usage Context:**
- Used to merge documents or insert dynamic content.

**Example:**
```php
$new_content = new \cms\document("title:text:New Title");
$doc->inject("body", $new_content);
```

---

#### `copy($path_source, $path_target)`

**Purpose:**
Copies an element and its children from `$path_source` to `$path_target`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path_source` | `string` | Path of the element to copy. |
| `$path_target` | `string` | Path to copy the element to. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Uses `extract()` to isolate the source element and `inject()` to place it at the target path.

**Usage Context:**
- Used to duplicate content within a document.

**Example:**
```php
$doc->copy("body.paragraph1", "body.paragraph2");
```

---

#### `swap($path_source, $path_target)`

**Purpose:**
Swaps two elements and their children between `$path_source` and `$path_target`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path_source` | `string` | Path of the first element. |
| `$path_target` | `string` | Path of the second element. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Uses `extract()` to isolate both elements and `inject()` to swap their positions.

**Usage Context:**
- Used to reorder content within a document.

**Example:**
```php
$doc->swap("body.paragraph1", "body.paragraph2");
```

---

#### `kick($path, $value)`

**Purpose:**
Moves an element and its children by `$value` positions within its parent container.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to move. |
| `$value` | `int` | Number of positions to move (positive or negative). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Identifies the parent container and level of the element.
- Finds the nth compatible position in the specified direction and moves the element.

**Usage Context:**
- Used to reorder elements within a group or template.

**Example:**
```php
$doc->kick("body.paragraph1", 1); // Moves paragraph1 down by 1 position
```

---

#### `drop($path, $value)`

**Purpose:**
Removes an element and shifts subsequent elements to fill the gap, then reinserts the element at a new position.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to drop. |
| `$value` | `int` | Direction and magnitude of the drop (positive or negative). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Removes the element and shifts subsequent elements to fill the gap.
- Reinserts the element at the nth compatible position in the specified direction.

**Usage Context:**
- Used to reorder elements while maintaining structural integrity.

**Example:**
```php
$doc->drop("body.paragraph1", -1); // Drops paragraph1 up by 1 position
```

---

#### `shift($id, $value)`

**Purpose:**
Shifts all first-level children of a `shift` container by `$value` positions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | ID of the `shift` container. |
| `$value` | `int` | Number of positions to shift (positive or negative). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Identifies the `shift` container and its first-level children.
- Moves each child to the nth compatible position in the specified direction.

**Usage Context:**
- Used to reorder dynamic content within a `shift` container.

**Example:**
```php
$doc->shift("slideshow", 1); // Shifts slideshow children down by 1 position
```

---

#### `del($path)`

**Purpose:**
Deletes an element and its children from the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to delete. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Removes the element and its children from `$this->data`.

**Usage Context:**
- Used to remove content from the document.

**Example:**
```php
$doc->del("body.paragraph1");
```

---

#### `cleanup()`

**Purpose:**
Removes elements from the document that do not match the template structure.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Collects element IDs and types from the template structure.
- Filters `$this->data` to retain only elements matching the structure.

**Usage Context:**
- Used to enforce structural integrity before exporting or saving the document.

**Example:**
```php
$doc->cleanup(); // Removes unstructured elements
```


<!-- HASH:0b082a458dfe583738b1d0f4b355089a -->
