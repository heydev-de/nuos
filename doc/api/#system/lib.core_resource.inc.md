# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_resource.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_resource.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core_Resource Class

The `core_resource` class provides a low-level, high-performance binary file storage system for structured data. It enables reading, writing, and searching fixed-length records in a binary file, with support for different field types (strings, UTF-8 strings, and bytes). This is particularly useful for caching, session storage, or any scenario requiring fast, direct file-based data access without a database.

### Properties

| Name                | Value/Default | Description                                                                 |
|---------------------|---------------|-----------------------------------------------------------------------------|
| `file`              | `""`          | Path to the binary data file.                                               |
| `hfile`             | `NULL`        | File handle for the binary data file.                                       |
| `field_type`        | `NULL`        | Associative array mapping field names to their types (`string`, `_string`, `byte`). |
| `field_offset`      | `NULL`        | Associative array mapping field names to their byte offset within a record. |
| `field_length`      | `NULL`        | Associative array mapping field names to their byte length within a record. |
| `recordset_length`  | `0`           | Total byte length of a single record.                                       |
| `offset`            | `0`           | Current byte offset within the file (used for iteration).                   |
| `zero`              | `""`          | A string of null bytes (`\0`) with the length of a record (used for deletion). |
| `lock`              | `[]`          | Stack of file lock states (shared or exclusive).                            |

---

### `__construct($file, $structure)`

**Purpose:**
Initializes a new binary data file with a given structure. Creates the file if it does not exist.

**Parameters:**

| Name        | Type         | Description                                                                 |
|-------------|--------------|-----------------------------------------------------------------------------|
| `$file`     | `string`     | Path to the binary data file.                                               |
| `$structure`| `array`      | Associative array defining the record structure. Keys are field names, values are type definitions (e.g., `string[10]`, `_string[255]`, `byte`). |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
- Parses the structure to determine field types, lengths, and offsets.
- Calculates the total record length.
- Opens the file in read-write mode (`r+b` or `w+b`).
- Disables read/write buffering for direct I/O.
- Registers a shutdown function to ensure the file is closed properly.

**Usage Context:**
- Used to initialize a new binary storage file or open an existing one.
- Typically called once per file during application initialization.

**Example:**
```php
$structure = [
    "id"    => "byte",
    "name"  => "string[32]",
    "email" => "_string[255]"
];
$resource = new \cms\core_resource("/data/users.bin", $structure);
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
- Calls the `close()` method.

**Usage Context:**
- Automatically invoked when the object is garbage-collected or explicitly unset.

---

### `current()`

**Purpose:**
Returns the current byte offset within the file, adjusted to the start of the current record.

**Parameters:**
- None.

**Return Values:**
- `int`: The current byte offset (always non-negative).

**Inner Mechanisms:**
- Returns `max(0, $this->offset)` to ensure the offset is never negative.

**Usage Context:**
- Used to check the current position during iteration.

**Example:**
```php
$offset = $resource->current();
```

---

### `reset()`

**Purpose:**
Resets the internal offset to the start of the file (before the first record).

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Sets `$this->offset` to `-$this->recordset_length`, so the next call to `next()` starts at the first record.

**Usage Context:**
- Used to restart iteration from the beginning of the file.

**Example:**
```php
$resource->reset();
while ($resource->next()) {
    $record = $resource->get();
    // Process record
}
```

---

### `next($filter = NULL, $reset = FALSE, $limit = NULL)`

**Purpose:**
Advances the internal offset to the next record, optionally filtering records by field values.

**Parameters:**

| Name     | Type          | Description                                                                 |
|----------|---------------|-----------------------------------------------------------------------------|
| `$filter`| `array\|NULL` | Associative array of field names and values to match. Use `TRUE` to match non-empty fields, `FALSE` or `NULL` to match empty fields. |
| `$reset` | `bool`        | If `TRUE`, resets the offset to the start of the file before iterating.     |
| `$limit` | `int\|NULL`   | Maximum byte offset to read up to (defaults to the file size).              |

**Return Values:**
- `bool`: `TRUE` if a matching record is found, `FALSE` otherwise.

**Inner Mechanisms:**
- If no filter is provided, advances the offset by one record and returns `TRUE` if within bounds.
- If a filter is provided, iterates through records until a match is found or the limit is reached.
- Uses file locking to ensure thread safety during iteration.

**Usage Context:**
- Used to iterate through records with or without filtering.
- Essential for searching or processing records sequentially.

**Example:**
```php
// Find the first record where "name" is "John" and "email" is non-empty
$resource->reset();
if ($resource->next(["name" => "John", "email" => TRUE])) {
    $record = $resource->get();
    echo "Found: " . $record["email"];
}
```

---

### `seek($filter, $next = FALSE)`

**Purpose:**
Searches for a record matching the filter, starting from the current position or the next record.

**Parameters:**

| Name     | Type      | Description                                                                 |
|----------|-----------|-----------------------------------------------------------------------------|
| `$filter`| `array`   | Associative array of field names and values to match.                       |
| `$next`  | `bool`    | If `TRUE`, starts searching from the next record (skips the current one).   |

**Return Values:**
- `bool`: `TRUE` if a matching record is found, `FALSE` otherwise.

**Inner Mechanisms:**
- Saves the current offset, then uses `next()` to search forward.
- If no match is found, resets to the saved offset and searches from the start of the file.

**Usage Context:**
- Used to locate a specific record without resetting the iteration state.

**Example:**
```php
// Find the next record where "id" is 5
if ($resource->seek(["id" => chr(5)])) {
    $record = $resource->get();
    echo "Found record with ID 5";
}
```

---

### `get($key = NULL, $raw = FALSE)`

**Purpose:**
Retrieves the current record or a specific field from the current record.

**Parameters:**

| Name   | Type      | Description                                                                 |
|--------|-----------|-----------------------------------------------------------------------------|
| `$key` | `string\|NULL` | Field name to retrieve. If `NULL`, returns the entire record as an array.   |
| `$raw` | `bool`    | If `TRUE`, returns the raw binary value without decoding.                  |

**Return Values:**
- `array\|string\|int\|NULL`: The requested field or record, or `NULL` if the field does not exist or the end of the file is reached.

**Inner Mechanisms:**
- Reads the current record from the file.
- Decodes field values based on their type (e.g., trims null bytes for strings, converts bytes to integers).
- Uses file locking to ensure thread safety.

**Usage Context:**
- Used to access data after positioning the offset with `next()` or `seek()`.

**Example:**
```php
$resource->reset();
while ($resource->next()) {
    $record = $resource->get();
    echo "Name: " . $record["name"] . "\n";
}
```

---

### `set($value)`

**Purpose:**
Updates the current record with new values.

**Parameters:**

| Name    | Type    | Description                                                                 |
|---------|---------|-----------------------------------------------------------------------------|
| `$value`| `array` | Associative array of field names and values to update.                     |

**Return Values:**
- `bool`: `TRUE` if the record was updated successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Retrieves the current record in raw form.
- Encodes new values and packs them into binary format.
- Writes the updated record back to the file at the current offset.
- Uses exclusive file locking to prevent concurrent writes.

**Usage Context:**
- Used to modify records after positioning the offset.

**Example:**
```php
$resource->reset();
if ($resource->next(["name" => "John"])) {
    $resource->set(["name" => "Johnny", "email" => "johnny@example.com"]);
}
```

---

### `del()`

**Purpose:**
Deletes the current record by overwriting it with null bytes.

**Parameters:**
- None.

**Return Values:**
- `bool`: Always `TRUE`.

**Inner Mechanisms:**
- Writes the `zero` string (a record-length sequence of null bytes) to the current offset.
- Uses exclusive file locking.

**Usage Context:**
- Used to mark records as deleted (logical deletion).

**Example:**
```php
$resource->reset();
if ($resource->next(["name" => "John"])) {
    $resource->del();
}
```

---

### `field_encode($key, $value)`

**Purpose:**
Encodes a field value into its binary representation based on the field type.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                 |
| `$value`| `mixed`  | Value to encode.                                                            |

**Return Values:**
- `string`: The encoded binary value.

**Inner Mechanisms:**
- For `string` and `_string` fields, returns the value as-is.
- For `byte` fields, converts the value to a single character using `chr()`.

**Usage Context:**
- Used internally by `set()` to prepare values for writing.

---

### `field_decode($key, $value)`

**Purpose:**
Decodes a binary field value into its PHP representation based on the field type.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                 |
| `$value`| `string` | Binary value to decode.                                                    |

**Return Values:**
- `string\|int\|NULL`: The decoded value.

**Inner Mechanisms:**
- For `string` and `_string` fields, trims trailing null bytes.
- For `byte` fields, converts the character to its ASCII value using `ord()` or returns `NULL` for empty values.

**Usage Context:**
- Used internally by `get()` to process raw data.

---

### `lock($exclusive = FALSE)`

**Purpose:**
Acquires a shared or exclusive lock on the file.

**Parameters:**

| Name         | Type   | Description                                                                 |
|--------------|--------|-----------------------------------------------------------------------------|
| `$exclusive` | `bool` | If `TRUE`, acquires an exclusive lock (`LOCK_EX`). Otherwise, acquires a shared lock (`LOCK_SH`). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Maintains a stack of lock states to support nested locking.
- Only acquires a new lock if the current state does not already satisfy the request.

**Usage Context:**
- Used internally to ensure thread-safe file access.

---

### `unlock()`

**Purpose:**
Releases the most recent lock on the file.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Pops the current lock state from the stack.
- Downgrades or releases locks as needed based on the remaining stack.

**Usage Context:**
- Used internally to manage lock state.

---

### `close()`

**Purpose:**
Closes the file handle if it is open.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Checks if the file handle is a valid resource before closing it.

**Usage Context:**
- Called automatically by the destructor or shutdown function.
- Can be called manually to release resources early.


<!-- HASH:5e2b3fb966d0cd2421d62fea1db18fa8 -->
