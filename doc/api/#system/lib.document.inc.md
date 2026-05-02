# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.document.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Document Class

The `document` class in NUOS provides a structured way to manage hierarchical data, typically used for content management. It handles serialization, deserialization, and manipulation of document elements based on a template-defined structure. Documents can contain references to other documents, enabling modular content composition.

---

### Constants

| Name                     | Value       | Description                                                                 |
|--------------------------|-------------|-----------------------------------------------------------------------------|
| `CMS_DOCUMENT_SEPARATOR` | `"\x1E"`    | ASCII record separator used to delimit document elements during serialization. |
| `CMS_DOCUMENT_TYPE`      | `"0"`       | Array index for element type in document data.                              |
| `CMS_DOCUMENT_VALUE`     | `"1"`       | Array index for element value in document data.                             |
| `CMS_DOCUMENT_REFERENCE` | `"2"`       | Array index for reference identifier in document data.                      |

---

### Properties

| Name               | Default Value | Description                                                                 |
|--------------------|---------------|-----------------------------------------------------------------------------|
| `data`             | `[]`          | Associative array storing document elements as `[id => [type, value, reference]]`. |
| `default`          | `[]`          | Default values for document elements, used when no value is set.            |
| `template_index`   | `NULL`        | Identifier of the template used to define the document's structure.         |
| `structure`        | `NULL`        | Parsed template structure defining element hierarchy and types.             |

---

### `__construct`

#### Purpose
Initializes a new document instance, optionally importing data and setting a template structure.

#### Parameters

| Name             | Type     | Description                                                                 |
|------------------|----------|-----------------------------------------------------------------------------|
| `$text`          | `string` | Serialized document data to import. If `NULL`, an empty document is created. |
| `$template_index`| `mixed`  | Template identifier to define the document's structure. If `NULL`, no structure is set. |

#### Return Values
- **None** (Constructor)

#### Inner Mechanisms
- If `$text` is provided, calls `import()` to deserialize the data.
- If `$template_index` is provided, calls `set_structure()` to parse the template.

#### Usage Context
- Used to create a new document from serialized data or an empty document with a defined structure.

---

### `import`

#### Purpose
Deserializes a string into document elements, populating the `data` property.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$text`| `string` | Serialized document data using `CMS_DOCUMENT_SEPARATOR` as delimiter.       |

#### Return Values
- **None**

#### Inner Mechanisms
- Splits the input string by `CMS_DOCUMENT_SEPARATOR`.
- Parses each segment using regex to extract `id`, `type`, and `value`.
- If the type is `#reference`, resolves the reference using `resolve_reference()`.
- Unmatched segments are stored as raw values with `NULL` type.

#### Usage Context
- Used to load document data from storage or transmission formats.

---

### `resolve_reference`

#### Purpose
Resolves a document reference by fetching the referenced content and merging it into the current document.

#### Parameters

| Name | Type     | Description                                                                 |
|------|----------|-----------------------------------------------------------------------------|
| `$id`| `string` | Identifier of the element containing the reference.                         |

#### Return Values
- **None**

#### Inner Mechanisms
- Loads the `content_pool` library to fetch referenced content.
- Splits the referenced content into elements.
- Merges elements into the current document, adjusting keys to maintain hierarchy.
- Existing elements are overwritten only if they are references or marked as such.

#### Usage Context
- Used internally when importing documents containing references (`#reference` type).

---

### `set_structure`

#### Purpose
Parses a template to define the document's structure, enabling validation and hierarchical operations.

#### Parameters

| Name              | Type     | Description                                                                 |
|-------------------|----------|-----------------------------------------------------------------------------|
| `$template_index` | `mixed`  | Identifier of the template to use for structure definition.                 |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `boolean` | `TRUE` if the structure was set successfully, `FALSE` on failure.           |

#### Inner Mechanisms
- Loads the `template` library to parse the template.
- Calls `template->structure()` to generate the structure array.
- Stores the structure and template index in the instance.

#### Usage Context
- Used to enforce a schema on document data, enabling type validation and hierarchical operations.

---

### `update_structure`

#### Purpose
Refreshes the document's structure using the current template index.

#### Parameters
- **None**

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `boolean` | `TRUE` if the structure was updated, `FALSE` if no template is set.         |

#### Inner Mechanisms
- Calls `set_structure()` with the current `template_index`.

#### Usage Context
- Used after modifying the document to ensure the structure reflects changes.

---

### `export`

#### Purpose
Serializes the document into a string, optionally resolving references and cleaning up unlisted elements.

#### Parameters

| Name                 | Type      | Description                                                                 |
|----------------------|-----------|-----------------------------------------------------------------------------|
| `$resolve_references`| `boolean` | If `TRUE`, resolves references before exporting. If `FALSE`, exports references as-is. |
| `$cleanup`           | `boolean` | If `TRUE`, removes elements not listed in the structure.                   |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Serialized document data.                                                   |

#### Inner Mechanisms
- If `$cleanup` is `TRUE`, collects element IDs from the structure.
- Iterates over `data`, skipping unlisted or mismatched elements.
- Exports resolved elements or unresolved references based on `$resolve_references`.

#### Usage Context
- Used to save or transmit document data.

---

### `get`

#### Purpose
Retrieves the value of a document element, optionally validating its type and falling back to default values.

#### Parameters

| Name           | Type     | Description                                                                 |
|----------------|----------|-----------------------------------------------------------------------------|
| `$id`          | `string` | Identifier of the element to retrieve.                                      |
| `$type`        | `string` | Expected type of the element. If `NULL`, type is not validated.             |
| `$use_default` | `boolean`| If `TRUE`, falls back to default values if the element is not set.          |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `mixed`  | The element's value, default value, or `NULL` if not found.                 |

#### Inner Mechanisms
- Returns `TRUE` if `$type` is `"group"` (groups are logical containers).
- Checks the element's type if `$type` is provided.
- Falls back to default values if `$use_default` is `TRUE`.

#### Usage Context
- Used to access document data in a type-safe manner.

---

### `get_reference`

#### Purpose
Retrieves the reference identifier of a document element.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$id`  | `string` | Identifier of the element.                                                  |
| `$type`| `string` | Expected type of the element. If `NULL`, type is not validated.             |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `mixed`  | The reference identifier or `NULL` if not found.                            |

#### Inner Mechanisms
- Validates the element's type if `$type` is provided.

#### Usage Context
- Used to check if an element is a reference and retrieve its target.

---

### `get_parent_template`

#### Purpose
Retrieves the path of the nearest parent template for a given element path.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$path` | `string` | Path of the element.                                                        |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Path of the parent template or `NULL` if not found.                        |
| `FALSE`  | If no structure is set.                                                     |

#### Inner Mechanisms
- Traverses the structure to find the element.
- Walks up the parent hierarchy until a template is found.

#### Usage Context
- Used to determine the template context of an element.

---

### `set`

#### Purpose
Sets the value and type of a document element, optionally resolving references.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$id`   | `string` | Identifier of the element.                                                  |
| `$type` | `string` | Type of the element.                                                        |
| `$value`| `mixed`  | Value of the element.                                                       |

#### Return Values
- **None**

#### Inner Mechanisms
- Stores the element in `data` with the given type and value.
- If the type is `#reference`, resolves the reference using `resolve_reference()`.

#### Usage Context
- Used to modify or add elements to the document.

---

### `set_default`

#### Purpose
Sets the default value and type for a document element.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$id`   | `string` | Identifier of the element.                                                  |
| `$type` | `string` | Type of the element.                                                        |
| `$value`| `mixed`  | Default value of the element.                                               |

#### Return Values
- **None**

#### Inner Mechanisms
- Stores the default value in the `default` property.

#### Usage Context
- Used to define fallback values for elements.

---

### `extract`

#### Purpose
Creates a new document containing a subset of elements from the current document, rooted at a given path.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$path` | `string` | Path of the root element to extract.                                        |

#### Return Values

| Type       | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| `document` | A new document containing the extracted elements.                           |
| `FALSE`    | If no structure is set.                                                     |

#### Inner Mechanisms
- Finds the root element in the structure.
- Copies the root element and its descendants into the new document, adjusting keys to maintain relative paths.

#### Usage Context
- Used to isolate a portion of the document for manipulation or injection.

---

### `inject`

#### Purpose
Merges elements from another document into the current document at a specified path.

#### Parameters

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `$path`    | `string`   | Path where the elements will be injected.                                   |
| `$document`| `document` | Document containing the elements to inject.                                |

#### Return Values
- **None**

#### Inner Mechanisms
- Extracts the target path to clear it.
- Merges elements from the source document, adjusting keys to fit the target hierarchy.
- Recomputes the structure and fits unassigned elements into compatible positions.

#### Usage Context
- Used to combine documents or move elements between documents.

---

### `copy`

#### Purpose
Copies elements from one path to another within the document.

#### Parameters

| Name            | Type     | Description                                                                 |
|-----------------|----------|-----------------------------------------------------------------------------|
| `$path_source`  | `string` | Path of the source elements.                                                |
| `$path_target`  | `string` | Path where the elements will be copied.                                     |

#### Return Values
- **None**

#### Inner Mechanisms
- Uses `extract()` to get the source elements.
- Uses `inject()` to place them at the target path.

#### Usage Context
- Used to duplicate elements within the document.

---

### `swap`

#### Purpose
Swaps elements between two paths within the document.

#### Parameters

| Name            | Type     | Description                                                                 |
|-----------------|----------|-----------------------------------------------------------------------------|
| `$path_source`  | `string` | Path of the first set of elements.                                          |
| `$path_target`  | `string` | Path of the second set of elements.                                         |

#### Return Values
- **None**

#### Inner Mechanisms
- Uses `extract()` to get elements from both paths.
- Uses `inject()` to place them at the opposite paths.

#### Usage Context
- Used to reorder or exchange elements within the document.

---

### `kick`

#### Purpose
Moves an element and its siblings by a specified number of positions within the same hierarchical level.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$path` | `string` | Path of the element to move.                                                |
| `$value`| `integer`| Number of positions to move (positive or negative).                         |

#### Return Values
- **None**

#### Inner Mechanisms
- Finds the element and its relevant parent (template or group).
- Extracts the element and its siblings, then re-injects them at the target positions.

#### Usage Context
- Used to reorder elements within a container.

---

### `drop`

#### Purpose
Removes an element and shifts its siblings to fill the gap, maintaining the same hierarchical level.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$path` | `string` | Path of the element to remove.                                              |
| `$value`| `integer`| Direction of the shift (`1` for forward, `-1` for backward).                |

#### Return Values
- **None**

#### Inner Mechanisms
- Removes the element and shifts siblings to fill the gap, re-injecting them at the appropriate positions.

#### Usage Context
- Used to remove elements while preserving the order of remaining elements.

---

### `shift`

#### Purpose
Moves an element within a `shift` container by a specified number of positions.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$id`   | `string` | Identifier of the `shift` container.                                        |
| `$value`| `integer`| Number of positions to move (positive or negative).                         |

#### Return Values
- **None**

#### Inner Mechanisms
- Collects first-level children of the `shift` container.
- Extracts and re-injects elements at the target positions.

#### Usage Context
- Used to reorder elements within a `shift` container.

---

### `del`

#### Purpose
Removes an element and its descendants from the document.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$path` | `string` | Path of the element to remove.                                              |

#### Return Values
- **None**

#### Inner Mechanisms
- Finds the element in the structure and removes it and its descendants from `data`.

#### Usage Context
- Used to delete elements from the document.

---

### `cleanup`

#### Purpose
Removes elements from `data` that are not listed in the structure or have mismatched types.

#### Parameters
- **None**

#### Return Values
- **None**

#### Inner Mechanisms
- Collects element IDs and types from the structure.
- Filters `data` to retain only elements with matching IDs and types.

#### Usage Context
- Used to enforce schema compliance and remove redundant data.


<!-- HASH:1ac8b04d718d46513538269f80e1afb9 -->
