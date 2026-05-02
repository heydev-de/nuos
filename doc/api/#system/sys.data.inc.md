# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.data.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Data Management System (`sys.data.inc`)

The `sys.data.inc` file provides a hierarchical data storage and manipulation system for the NUOS platform. It includes a standalone function for sorting hierarchical data and a `data` class that handles structured data storage, retrieval, and manipulation in a tree-like format. The system supports encryption, prefix-based key management, and temporary caching.

---

## Functions

### `data_sort(&$data, $property, $key = NULL)`

Sorts hierarchical data in-place based on a specified property.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$data`    | `data`   | Reference to a `data` object containing hierarchical data to be sorted.     |
| `$property`| `string` | Property name used as the sorting key.                                     |
| `$key`     | `string` | Optional. Starting key for sorting (defaults to root).                     |

**Return Value:**
- `void`: Modifies the `$data` object in-place.

**Inner Mechanisms:**
1. **Tree Construction:** Builds a hierarchical array representation of the data using a stack-based approach to track container nesting.
2. **Tree Sorting:** Recursively sorts each container using `uasort` and the `_data_sort` comparator.
3. **Data Reconstruction:** Rebuilds the `data` object from the sorted array while preserving the original structure.

**Usage Context:**
- Used to sort hierarchical data (e.g., navigation menus, content trees) by a specific property (e.g., title, date).
- Typically called after loading data from a `.dat` file.

---

### `_data_sort($value1, $value2)`

Comparator function for hierarchical sorting.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | `array`| First value to compare.         |
| `$value2` | `array`| Second value to compare.        |

**Return Value:**
- `int`: Comparison result (`-1`, `0`, or `1`) based on natural case-insensitive string comparison.

**Inner Mechanisms:**
- Uses `utf8_strnatcasecmp` for multibyte-safe natural sorting.
- Skips comparison if either value lacks the sorting property (`#!`).

**Usage Context:**
- Internal use by `data_sort` for sorting containers.

---

## `data` Class

The `data` class provides a hierarchical data storage system with support for containers, encryption, and prefix-based key management.

### Properties

| Name              | Default | Description                                                                 |
|-------------------|---------|-----------------------------------------------------------------------------|
| `$file`           | `NULL`  | Path to the `.dat` file storing the data.                                   |
| `$data`           | `NULL`  | Associative array holding the hierarchical data.                            |
| `$buffer`         | `NULL`  | Temporary buffer for cut/copy/paste operations.                             |
| `$password`       | `NULL`  | Encryption key for securing data.                                           |
| `$prefix`         | `NULL`  | Prefix applied to keys (e.g., `image://`).                                  |
| `$prefix_length`  | `0`     | Length of the prefix (optimization for key processing).                     |

---

### Constructor

#### `__construct($name = NULL, $password = NULL, $prefix = NULL)`

Initializes a `data` object and optionally opens a data file.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | Optional. Name/path of the `.dat` file to open.                            |
| `$password` | `string` | Optional. Password for data encryption.                                    |
| `$prefix`   | `string` | Optional. Prefix for keys (e.g., `image://`).                              |

**Inner Mechanisms:**
- Calls `open()` if `$name` is provided.
- Sets password and prefix if provided.

**Usage Context:**
- Initialize a `data` object for file-based or in-memory data operations.

---

### Methods

#### `open($name, $password = NULL, $prefix = NULL)`

Loads data from a `.dat` file into the object.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | Name/path of the `.dat` file.                                              |
| `$password` | `string` | Optional. Password for decrypting data.                                    |
| `$prefix`   | `string` | Optional. Prefix for keys.                                                 |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. **Caching:** Checks temporary cache (`cms_cache`) for the file.
2. **File Parsing:** Reads the file line-by-line, parsing keys and values.
   - Keys are decrypted using `decchr`.
   - Values are decrypted using `decrypt` if a password is set.
3. **Expiration:** Skips expired entries (based on `#expire` property).
4. **Caching:** Stores loaded data in temporary cache.

**Usage Context:**
- Load data from a `.dat` file for manipulation.

---

#### `save($name = NULL)`

Saves the current data to a `.dat` file.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$name`   | `string` | Optional. Name/path of the `.dat` file. Overrides `$this->file` if set.    |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. **Path Creation:** Ensures the directory exists using `mkpath`.
2. **File Writing:** Writes each key-value pair to the file.
   - Keys are encrypted using `encchr`.
   - Values are encrypted using `encrypt` if a password is set.
3. **Caching:** Updates the temporary cache.

**Usage Context:**
- Persist changes to a `.dat` file.

---

#### `set($value = NULL, $key = NULL, $property = NULL)`

Sets a value in the data hierarchy.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$value`   | `mixed`  | Value to set. If `NULL` or empty, the property is deleted.                 |
| `$key`     | `string` | Optional. Key of the dataset. If `NULL`, replaces all data.                |
| `$property`| `string` | Optional. Property name. If `NULL`, replaces the entire dataset at `$key`. |

**Return Value:**
- `void`: Modifies the data in-place.

**Inner Mechanisms:**
- Delegates to `_set` after removing the prefix from `$key`.

**Usage Context:**
- Set a property value, replace a dataset, or replace all data.

---

#### `_set($value = NULL, $key = NULL, $property = NULL)`

Internal method for setting values.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$value`   | `mixed`  | Value to set.                                                              |
| `$key`     | `string` | Key of the dataset.                                                        |
| `$property`| `string` | Property name.                                                             |

**Inner Mechanisms:**
- Handles three cases:
  1. Replace all data (`$key` and `$property` are `NULL`).
  2. Replace a dataset (`$property` is `NULL`).
  3. Set a property value.

**Usage Context:**
- Internal use by `set`.

---

#### `get($key = NULL, $property = NULL)`

Retrieves a value from the data hierarchy.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Optional. Key of the dataset. If `NULL`, returns all data.                 |
| `$property`| `string` | Optional. Property name. If `NULL`, returns the entire dataset at `$key`.  |

**Return Value:**
- `mixed`: The requested value, dataset, or `NULL` if not found.

**Inner Mechanisms:**
- Delegates to `_get` after removing the prefix from `$key`.

**Usage Context:**
- Retrieve a property value, dataset, or all data.

---

#### `_get($key = NULL, $property = NULL)`

Internal method for retrieving values.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key of the dataset.                                                        |
| `$property`| `string` | Property name.                                                             |

**Return Value:**
- `mixed`: The requested value, dataset, or `NULL` if not found.

**Inner Mechanisms:**
- Handles three cases:
  1. Return all data (`$key` and `$property` are `NULL`).
  2. Return a dataset (`$property` is `NULL`).
  3. Return a property value.

**Usage Context:**
- Internal use by `get`.

---

#### `has($key = NULL, $property = NULL)`

Checks if a key or property exists.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Optional. Key of the dataset. If `NULL`, checks if data exists.            |
| `$property`| `string` | Optional. Property name. If `NULL`, checks if the dataset exists.          |

**Return Value:**
- `bool`: `TRUE` if the key/property exists, `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to `_has` after removing the prefix from `$key`.

**Usage Context:**
- Check for the existence of a key, dataset, or property.

---

#### `_has($key = NULL, $property = NULL)`

Internal method for checking existence.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key of the dataset.                                                        |
| `$property`| `string` | Property name.                                                             |

**Return Value:**
- `bool`: `TRUE` if the key/property exists, `FALSE` otherwise.

**Inner Mechanisms:**
- Handles three cases:
  1. Check if data exists (`$key` and `$property` are `NULL`).
  2. Check if a dataset exists (`$property` is `NULL`).
  3. Check if a property exists and is non-empty.

**Usage Context:**
- Internal use by `has`.

---

#### `del($key = NULL, $property = NULL, $recursive = TRUE)`

Deletes a key, property, or dataset.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Optional. Key of the dataset. If `NULL`, deletes all data.                 |
| `$property`| `string` | Optional. Property name. If `NULL`, deletes the entire dataset at `$key`.  |
| `$recursive`| `bool`  | If `TRUE`, deletes all child containers recursively.                       |

**Return Value:**
- `bool`: `TRUE` on success.

**Inner Mechanisms:**
- Delegates to `_del` after removing the prefix from `$key`.

**Usage Context:**
- Delete a property, dataset, or all data.

---

#### `_del($key = NULL, $property = NULL, $recursive = TRUE)`

Internal method for deletion.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key of the dataset.                                                        |
| `$property`| `string` | Property name.                                                             |
| `$recursive`| `bool`  | If `TRUE`, deletes all child containers recursively.                       |

**Inner Mechanisms:**
- Handles three cases:
  1. Delete all data (`$key` and `$property` are `NULL`).
  2. Delete a dataset (`$property` is `NULL`).
     - If `$recursive` is `TRUE`, uses `_copy` to delete child containers.
  3. Delete a property.

**Usage Context:**
- Internal use by `del`.

---

#### `set_buffer($array)`

Sets the internal buffer for cut/copy/paste operations.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| `$array`  | `array` | Array of values to store in the buffer.                                    |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Assigns a unique ID to each value in the buffer.

**Usage Context:**
- Prepare data for cut/copy/paste operations.

---

#### `cut($key)`

Cuts a dataset and its children to the buffer.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset to cut.                                                 |

**Return Value:**
- `mixed`: Key with prefix applied on success, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `_cut`, which calls `_copy` with the `cut` action.

**Usage Context:**
- Remove a dataset and store it in the buffer for later insertion.

---

#### `_cut($key)`

Internal method for cutting data.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset to cut.                                                 |

**Return Value:**
- `bool`: `TRUE` on success.

**Usage Context:**
- Internal use by `cut`.

---

#### `insert($key = NULL)`

Inserts buffered data at a specified position.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Optional. Key after which to insert the buffered data.                     |

**Return Value:**
- `mixed`: Key of the first inserted item with prefix applied, or `TRUE` on success.

**Inner Mechanisms:**
- Delegates to `_insert` and applies the prefix to the returned key.

**Usage Context:**
- Insert previously cut/copied data into the hierarchy.

---

#### `_insert($key = NULL)`

Internal method for inserting data.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key after which to insert the buffered data.                               |

**Return Value:**
- `mixed`: Key of the first inserted item or `TRUE` on success.

**Inner Mechanisms:**
- Rebuilds the data array, inserting buffered items at the specified position.

**Usage Context:**
- Internal use by `insert`.

---

#### `append($key = NULL)`

Appends buffered data to a container.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Optional. Key of the container to append to.                               |

**Return Value:**
- `mixed`: Key of the first appended item with prefix applied, or `TRUE` on success.

**Inner Mechanisms:**
- Delegates to `_append`, which calls `_copy` with the `append` action.

**Usage Context:**
- Append previously cut/copied data to a container.

---

#### `_append($key = NULL)`

Internal method for appending data.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the container to append to.                                         |

**Return Value:**
- `mixed`: Key of the first appended item or `TRUE` on success.

**Usage Context:**
- Internal use by `append`.

---

#### `copy($key, $action = NULL)`

Copies a dataset and its children to the buffer.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset to copy.                                                |
| `$action` | `string` | Optional. Action to perform (`append`, `del`, or `NULL` for copy).         |

**Return Value:**
- `mixed`: Key with prefix applied on success, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `_copy` and applies the prefix to the returned key.

**Usage Context:**
- Copy a dataset to the buffer for later insertion or deletion.

---

#### `_copy($key, $action = NULL)`

Internal method for copying data.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset to copy.                                                |
| `$action` | `string` | Action to perform (`append`, `del`, `cut`, or `NULL` for copy).            |

**Return Value:**
- `bool`: `TRUE` on success.

**Inner Mechanisms:**
- Iterates through the dataset and its children, storing them in the buffer.
- Handles different actions:
  - `append`: Appends buffered data to the dataset.
  - `del`: Deletes the dataset.
  - `cut`: Copies and deletes the dataset.

**Usage Context:**
- Internal use by `copy`, `cut`, and `append`.

---

#### `seek($condition)`

Searches for a dataset matching specified conditions.

| Parameter  | Type    | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `$condition`| `array`| Associative array of property-value pairs to match.                        |

**Return Value:**
- `mixed`: Key of the matching dataset with prefix applied, or `FALSE` if not found.

**Inner Mechanisms:**
- Delegates to `_seek` after removing prefixes from condition keys.

**Usage Context:**
- Find a dataset based on property values (e.g., `#type = "container"`).

---

#### `_seek($condition)`

Internal method for searching data.

| Parameter  | Type    | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `$condition`| `array`| Associative array of property-value pairs to match.                        |

**Return Value:**
- `mixed`: Key of the matching dataset or `FALSE` if not found.

**Inner Mechanisms:**
- Iterates through the data, checking each dataset against the conditions.

**Usage Context:**
- Internal use by `seek`.

---

#### `move($target = "current", $key = NULL)`

Moves the internal pointer to a specified position.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$target` | `string` | Target position (`current`, `first`, `last`, `prev`, `next`, `to`, `parent`).|
| `$key`    | `string` | Optional. Key for `to` or `parent` targets.                                |

**Return Value:**
- `mixed`: Key of the new position with prefix applied, or `NULL` on failure.

**Inner Mechanisms:**
- Delegates to `_move` after removing the prefix from `$key`.

**Usage Context:**
- Navigate the data hierarchy programmatically.

---

#### `_move($target = "current", $key = NULL)`

Internal method for moving the pointer.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$target` | `string` | Target position.                                                           |
| `$key`    | `string` | Key for `to` or `parent` targets.                                          |

**Return Value:**
- `mixed`: Key of the new position or `NULL` on failure.

**Inner Mechanisms:**
- Uses PHP array functions (`reset`, `end`, `next`, `prev`) to navigate.
- For `parent`, tracks container nesting to find the parent key.

**Usage Context:**
- Internal use by `move`.

---

#### `is_container($key)`

Checks if a dataset is a container.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset.                                                        |

**Return Value:**
- `bool`: `TRUE` if the dataset is a container, `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to `_is_container` after removing the prefix from `$key`.

**Usage Context:**
- Determine if a dataset can contain child datasets.

---

#### `_is_container($key)`

Internal method for checking container status.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the dataset.                                                        |

**Return Value:**
- `bool`: `TRUE` if the dataset is a container, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks if the `#type` property equals `container`.

**Usage Context:**
- Internal use by `is_container`.

---

#### `is_child($key, $parent)`

Checks if a dataset is a child of another dataset.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the child dataset.                                                  |
| `$parent` | `string` | Key of the parent dataset.                                                 |

**Return Value:**
- `bool`: `TRUE` if `$key` is a child of `$parent`, `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to `_is_child` after removing prefixes from `$key` and `$parent`.

**Usage Context:**
- Verify hierarchical relationships (e.g., for navigation or permission checks).

---

#### `_is_child($key, $parent)`

Internal method for checking child status.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the child dataset.                                                  |
| `$parent` | `string` | Key of the parent dataset.                                                 |

**Return Value:**
- `bool`: `TRUE` if `$key` is a child of `$parent`, `FALSE` otherwise.

**Inner Mechanisms:**
- Iterates through the data, tracking container nesting to determine the relationship.

**Usage Context:**
- Internal use by `is_child`.

---

#### `has_children($key)`

Checks if a container has child datasets.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the container.                                                      |

**Return Value:**
- `bool`: `TRUE` if the container has children, `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to `_has_children` after removing the prefix from `$key`.

**Usage Context:**
- Determine if a container is non-empty (e.g., for UI rendering).

---

#### `_has_children($key)`

Internal method for checking children.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key of the container.                                                      |

**Return Value:**
- `bool`: `TRUE` if the container has children, `FALSE` otherwise.

**Inner Mechanisms:**
- Moves to the container and checks if the next dataset is a closing container (`/container`).

**Usage Context:**
- Internal use by `has_children`.

---

#### `set_password($value = NULL)`

Sets the encryption password.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$value`  | `string` | Optional. Password for encryption. If `NULL`, disables encryption.         |

**Inner Mechanisms:**
- Hashes the password using `hash64` for security.

**Usage Context:**
- Enable or disable data encryption.

---

#### `set_prefix($value = NULL)`

Sets the key prefix.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$value`  | `string` | Optional. Prefix for keys. If `NULL`, uses a default based on the file.    |

**Inner Mechanisms:**
- Uses static defaults for system files (e.g., `image://` for `image.dat`).

**Usage Context:**
- Configure prefix-based key management.

---

#### `apply_prefix($key)`

Applies the prefix to a key.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key to prefix.                                                             |

**Return Value:**
- `string`: Prefixed key.

**Usage Context:**
- Convert internal keys to prefixed keys for external use.

---

#### `remove_prefix($key)`

Removes the prefix from a key.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key to process.                                                            |

**Return Value:**
- `string`: Key without prefix.

**Inner Mechanisms:**
- Uses `strncmp` for efficient prefix removal.

**Usage Context:**
- Convert prefixed keys to internal keys for processing.


<!-- HASH:fd68a932299bcb779014b57835591fb6 -->
