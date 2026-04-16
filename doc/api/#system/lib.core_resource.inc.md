# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.core_resource.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Core Resource Class

The `core_resource` class provides a low-level, file-based recordset management system for structured binary data storage. It enables efficient reading, writing, and searching of fixed-length records with typed fields (strings, UTF-8 strings, and bytes). This class is designed for high-performance scenarios where traditional database systems may be overkill or impractical.

### Class Properties

| Name                | Default/Type       | Description                                                                                     |
|---------------------|--------------------|-------------------------------------------------------------------------------------------------|
| `file`              | `string`           | Path to the binary data file.                                                                  |
| `hfile`             | `resource`         | File handle for the binary data file.                                                          |
| `field_type`        | `array`            | Maps field names to their data types (`string`, `_string`, `byte`).                            |
| `field_offset`      | `array`            | Maps field names to their byte offsets within a record.                                        |
| `field_length`      | `array`            | Maps field names to their byte lengths within a record.                                        |
| `recordset_length`  | `int`              | Total byte length of a single record.                                                          |
| `offset`            | `int`              | Current byte offset within the file (used for record navigation).                              |
| `zero`              | `string`           | A string of null bytes (`\0`) with length equal to `recordset_length` (used for record deletion). |
| `lock`              | `array`            | Stack of file lock states (shared or exclusive) for concurrency control.                       |

---

### `__construct($file, $structure)`

**Purpose:**
Initializes a new `core_resource` instance, creating or opening a binary file and defining its record structure.

**Parameters:**

| Name        | Type     | Description                                                                                     |
|-------------|----------|-------------------------------------------------------------------------------------------------|
| `$file`     | `string` | Path to the binary data file.                                                                  |
| `$structure`| `array`  | Associative array defining the record structure. Keys are field names, values are type definitions. Supported types: `string`, `_string` (UTF-8), `byte`, with optional length in brackets (e.g., `string[10]`). |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
1. Parses the `$structure` array to determine field types, lengths, and offsets.
   - `string`: 1 byte per character (or specified length).
   - `_string`: 4 bytes per character (UTF-8 support, or specified length × 4).
   - `byte`: 1 byte.
2. Calculates the total record length (`recordset_length`) as the sum of all field lengths.
3. Initializes the file:
   - Creates parent directories if they do not exist (`mkpath`).
   - Opens the file in `r+b` (read/write) mode if it exists, or `w+b` (create/read/write) if it does not.
4. Disables write buffering (`stream_set_write_buffer`) for immediate I/O.
5. Registers the `close` method to run on script shutdown.

**Usage Context:**
- Used to initialize a new or existing binary data store with a predefined schema.
- Example:
  ```php
  $resource = new core_resource("/data/users.bin", [
      "id"   => "byte",
      "name" => "string[32]",
      "bio"  => "_string[255]"
  ]);
  ```

---

### `__destruct()`

**Purpose:**
Destructor that ensures the file handle is closed when the object is destroyed.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Calls the `close` method to release the file handle.

**Usage Context:**
- Automatically invoked when the object is garbage-collected or the script ends.

---

### `current()`

**Purpose:**
Returns the current byte offset within the file, adjusted to the start of the nearest record.

**Parameters:**
- None.

**Return Values:**
- `int`: The current byte offset, rounded to the nearest record boundary (or 0 if negative).

**Inner Mechanisms:**
- Returns `max(0, $this->offset)` to ensure the offset is non-negative.

**Usage Context:**
- Used to determine the current position in the file for read/write operations.

---

### `reset()`

**Purpose:**
Resets the internal offset to the start of the file (before the first record).

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Sets `$this->offset` to `-$this->recordset_length`, positioning the cursor before the first record.

**Usage Context:**
- Used to rewind the file for sequential reads from the beginning.

---

### `next($filter = NULL, $reset = FALSE, $limit = NULL)`

**Purpose:**
Advances the internal offset to the next record that matches the given filter criteria.

**Parameters:**

| Name     | Type      | Description                                                                                     |
|----------|-----------|-------------------------------------------------------------------------------------------------|
| `$filter`| `array`   | Associative array of field names and values to match. Special values: `TRUE` (non-empty), `NULL`/`FALSE` (empty). |
| `$reset` | `bool`    | If `TRUE`, resets the offset to the start of the file before searching.                         |
| `$limit` | `int`     | Maximum byte offset to search up to (defaults to file size).                                    |

**Return Values:**
- `bool`: `TRUE` if a matching record is found, `FALSE` otherwise.

**Inner Mechanisms:**
1. If `$reset` is `TRUE`, calls `reset()` to rewind the file.
2. Locks the file for shared access (`LOCK_SH`).
3. Iterates through records until a match is found or the `$limit` is reached:
   - For each record, reads the data and checks if all filter criteria are met.
   - A field matches if:
     - The filter value is `TRUE` and the field is non-empty.
     - The filter value is `NULL`/`FALSE` and the field is empty.
     - The filter value equals the field value (string comparison).
4. Unlocks the file and returns the result.

**Usage Context:**
- Used to sequentially search for records matching specific criteria.
- Example:
  ```php
  $resource->next(["name" => "Alice", "active" => TRUE]);
  ```

---

### `seek($filter, $next = FALSE)`

**Purpose:**
Searches for the next record matching the filter, starting from the current position (or the next record if `$next` is `TRUE`).

**Parameters:**

| Name     | Type    | Description                                                                                     |
|----------|---------|-------------------------------------------------------------------------------------------------|
| `$filter`| `array` | Associative array of field names and values to match (see `next`).                             |
| `$next`  | `bool`  | If `TRUE`, starts searching from the next record (skips the current record).                    |

**Return Values:**
- `bool`: `TRUE` if a matching record is found, `FALSE` otherwise.

**Inner Mechanisms:**
1. Saves the current offset.
2. If `$next` is `TRUE`, advances the offset by one record.
3. Locks the file and searches for a matching record using `next`.
4. If no match is found, rewinds to the saved offset and searches again from the start.
5. Unlocks the file and returns the result.

**Usage Context:**
- Used to find the next matching record without resetting the file position.
- Example:
  ```php
  $resource->seek(["status" => "active"]);
  ```

---

### `get($key = NULL, $raw = FALSE)`

**Purpose:**
Retrieves the current record or a specific field from it.

**Parameters:**

| Name   | Type     | Description                                                                                     |
|--------|----------|-------------------------------------------------------------------------------------------------|
| `$key` | `string` | Field name to retrieve. If `NULL`, returns the entire record as an associative array.          |
| `$raw` | `bool`   | If `TRUE`, returns the raw binary data; otherwise, decodes the data based on the field type.   |

**Return Values:**
- `array`|`string`|`int`|`NULL`:
  - If `$key` is `NULL`: Associative array of field names and values.
  - If `$key` is specified: The value of the field (type depends on `$raw` and field type).
  - `NULL` if the field does not exist or the read fails.

**Inner Mechanisms:**
1. Locks the file for shared access.
2. Reads the record at the current offset.
3. If `$key` is specified:
   - Extracts the field data using `substr` and decodes it (unless `$raw` is `TRUE`).
4. If `$key` is `NULL`:
   - Decodes all fields and returns them as an associative array.
5. Unlocks the file and returns the result.

**Usage Context:**
- Used to read data from the current record.
- Example:
  ```php
  $record = $resource->get(); // Get entire record
  $name = $resource->get("name"); // Get specific field
  ```

---

### `set($value)`

**Purpose:**
Updates the current record with the provided data.

**Parameters:**

| Name    | Type    | Description                                                                                     |
|---------|---------|-------------------------------------------------------------------------------------------------|
| `$value`| `array` | Associative array of field names and values to write.                                          |

**Return Values:**
- `bool`: `TRUE` on success.

**Inner Mechanisms:**
1. Retrieves the current record (or an empty record if none exists).
2. Merges the provided `$value` with the existing record data.
3. Encodes each field based on its type and packs it into a binary string.
4. Locks the file for exclusive access (`LOCK_EX`).
5. Writes the binary data to the current offset.
6. Unlocks the file and returns `TRUE`.

**Usage Context:**
- Used to update or create a record at the current offset.
- Example:
  ```php
  $resource->set(["name" => "Alice", "age" => 30]);
  ```

---

### `del()`

**Purpose:**
Deletes the current record by overwriting it with null bytes.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success.

**Inner Mechanisms:**
1. Locks the file for exclusive access.
2. Writes the `zero` string (null bytes) to the current offset.
3. Unlocks the file and returns `TRUE`.

**Usage Context:**
- Used to mark a record as deleted (logical deletion).
- Example:
  ```php
  $resource->del();
  ```

---

### `field_encode($key, $value)`

**Purpose:**
Encodes a field value into its binary representation based on the field type.

**Parameters:**

| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                                    |
| `$value`| `mixed`  | Value to encode.                                                                               |

**Return Values:**
- `string`: The encoded binary data.

**Inner Mechanisms:**
- `string`/`_string`: Returns the value as-is.
- `byte`: Converts the value to a single byte using `chr`.

**Usage Context:**
- Internal method used by `set` to prepare data for writing.

---

### `field_decode($key, $value)`

**Purpose:**
Decodes a binary field value into its PHP representation based on the field type.

**Parameters:**

| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                                    |
| `$value`| `string` | Binary data to decode.                                                                         |

**Return Values:**
- `string`|`int`: The decoded value.

**Inner Mechanisms:**
- `string`/`_string`: Trims trailing null bytes using `rtrim`.
- `byte`: Converts the byte to an integer using `ord`.

**Usage Context:**
- Internal method used by `get` to process raw data.

---

### `lock($exclusive = FALSE)`

**Purpose:**
Acquires a shared or exclusive lock on the file.

**Parameters:**

| Name         | Type   | Description                                                                                     |
|--------------|--------|-------------------------------------------------------------------------------------------------|
| `$exclusive` | `bool` | If `TRUE`, acquires an exclusive lock (`LOCK_EX`); otherwise, acquires a shared lock (`LOCK_SH`). |

**Return Values:**
- None.

**Inner Mechanisms:**
1. Checks the current lock state (stored in the `lock` stack).
2. If `$exclusive` is `TRUE` and the last lock is not exclusive, acquires an exclusive lock.
3. If `$exclusive` is `FALSE` and no lock is held, acquires a shared lock.
4. Pushes the new lock state onto the `lock` stack.

**Usage Context:**
- Internal method used to manage file locks for concurrency control.

---

### `unlock()`

**Purpose:**
Releases the most recent lock on the file.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Pops the current lock state from the `lock` stack.
2. If no locks remain, releases the file lock (`LOCK_UN`).
3. If the released lock was exclusive and a shared lock remains, downgrades to a shared lock.

**Usage Context:**
- Internal method used to manage file locks for concurrency control.

---

### `close()`

**Purpose:**
Closes the file handle if it is open.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Checks if the file handle is a valid resource and closes it using `fclose`.

**Usage Context:**
- Called automatically on object destruction or script shutdown.
- Can be called manually to release resources early.


<!-- HASH:dcc70dee7de143efedefe46d5eb2a279 -->
