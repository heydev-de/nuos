# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_resource.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_resource.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## core_resource

The `core_resource` class provides a low-level, file-based record storage mechanism for the PWNC Web Platform. It allows structured binary records to be stored in a flat file, with support for field-level encoding/decoding, record navigation, filtering, locking, and atomic updates.

Each record is defined by a fixed-size structure composed of typed fields (`string`, `_string` for UTF-8, and `byte`). Records are packed into a binary file with no delimiters, relying on fixed offsets and lengths for field access.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$file` | string | `""` | Path to the backing file. |
| `$hfile` | resource\|NULL | `NULL` | File handle opened in read/write binary mode. |
| `$field_type` | array\|NULL | `NULL` | Maps field names to their type (`string`, `_string`, `byte`). |
| `$field_offset` | array\|NULL | `NULL` | Maps field names to their byte offset within a record. |
| `$field_length` | array\|NULL | `NULL` | Maps field names to their byte length within a record. |
| `$recordset_length` | int | `0` | Total size in bytes of one full record. |
| `$offset` | int | `0` | Current read/write position within the file. |
| `$zero` | string | `""` | A zero-filled string of length `$recordset_length`, used for deletion. |
| `$lock` | array | `[]` | Stack tracking active locks to support nested locking. |

---

### __construct

Initializes the resource with a file path and a field structure definition.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$file` | string | Path to the file used for storage. Created if it does not exist. |
| `$structure` | array | Associative array mapping field names to type strings (`string`, `_string`, `byte`, optionally with `[N]` size). |

#### Inner Mechanism

Parses each field type using a regex to determine its type and length. For `string`, the default length is 1 unless specified. For `_string` (UTF-8), the length is multiplied by 4 to accommodate multibyte characters. For `byte`, the length is always 1. Offsets are computed cumulatively. The total record size is stored in `$recordset_length`. If the file exists, it is opened in `r+b` mode; otherwise, the directory is created and the file is opened in `w+b` mode. Read/write buffering is disabled for direct I/O control.

#### Usage Example

```php
$resource = new core_resource("/tmp/data.bin", [
    "id"   => "byte",
    "name" => "_string[32]",
    "flag" => "string[1]"
]);
```

This creates a resource where each record is 37 bytes: 1 byte for `id`, 128 bytes for `name` (32 × 4), and 1 byte for `flag`.

---

### __destruct

Automatically closes the file handle when the object is destroyed.

#### Inner Mechanism

Calls `close()` to ensure the file handle is released.

---

### current

Returns the current record offset.

#### Return Value

- **int**: The current offset, clamped to a minimum of 0.

#### Inner Mechanism

Returns `max(0, $this->offset)` to prevent negative positions.

#### Usage Example

```php
echo $resource->current(); // Outputs current byte offset
```

---

### reset

Resets the internal pointer to before the first record.

#### Inner Mechanism

Sets `$this->offset` to `-$this->recordset_length`, so the next call to `next()` will advance to the beginning of the file.

#### Usage Example

```php
$resource->reset();
$resource->next(); // Now at first record
```

---

### next

Advances the internal pointer to the next matching record.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$filter` | array\|NULL | `NULL` | Optional associative array of field values to match. `TRUE` means "not empty", `FALSE`/`NULL` means "empty". |
| `$reset` | bool | `FALSE` | If true, resets the pointer before searching. |
| `$limit` | int\|NULL | `NULL` | Maximum file size to search within. Defaults to full file size. |

#### Return Value

- **bool**: `TRUE` if a matching record was found, `FALSE` otherwise.

#### Inner Mechanism

If no filter is provided, simply advances the offset by one record length and returns whether more data exists. With a filter, it iterates through records, reading each via `get()`, and checks each field against the filter criteria. Uses shared locks during reads and exclusive locks during writes.

#### Usage Example

```php
// Find next record where 'flag' is not empty
$resource->next(["flag" => TRUE]);

// Find next record where 'id' equals 5
$resource->next(["id" => 5]);
```

---

### seek

Seeks to a specific record matching a filter, optionally starting from the next record.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$filter` | array | — | Field values to match (same semantics as `next()`). |
| `$next` | bool | `FALSE` | If true, starts searching from the record after the current one. |

#### Return Value

- **bool**: `TRUE` if a matching record was found, `FALSE` otherwise.

#### Inner Mechanism

Saves the current offset, optionally advances by one record, then calls `next()` with the filter. If not found, resets and searches from the beginning up to the saved offset.

#### Usage Example

```php
// Find the next record where 'id' is 3, starting from current position
$resource->seek(["id" => 3], TRUE);
```

---

### get

Reads the current record or a specific field from it.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$key` | string\|NULL | `NULL` | Field name to retrieve. If `NULL`, returns all fields as an array. |
| `$raw` | bool | `FALSE` | If true, returns raw binary data without decoding. |

#### Return Value

- **mixed**: Decoded field value, raw binary string, or associative array of all fields. Returns `NULL` on read failure.

#### Inner Mechanism

Reads `$recordset_length` bytes from the current offset. If a key is specified, extracts the relevant portion using precomputed offsets and lengths, then applies `field_decode()` unless `$raw` is true. Otherwise, decodes all fields into an associative array.

#### Usage Example

```php
// Get all fields of current record
$record = $resource->get();

// Get only the 'name' field
$name = $resource->get("name");

// Get raw binary data for 'id'
$rawId = $resource->get("id", TRUE);
```

---

### set

Writes a new record at the current offset, preserving unspecified fields.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | array | Associative array of field values to write. Unspecified fields retain their current values. |

#### Return Value

- **bool**: `TRUE` on success, `FALSE` if the current record could not be read.

#### Inner Mechanism

Reads the current record in raw mode, then constructs a new binary record by encoding each field. Unspecified fields use the existing raw data. Writes the assembled record back to the file using an exclusive lock.

#### Usage Example

```php
$resource->set([
    "id"   => 1,
    "name" => "Alice",
    "flag" => "Y"
]);
```

---

### del

Deletes the current record by overwriting it with zero bytes.

#### Return Value

- **bool**: Always `TRUE`.

#### Inner Mechanism

Writes a string of null bytes (`$this->zero`) at the current offset using an exclusive lock.

#### Usage Example

```php
$resource->del(); // Clears current record
```

---

### field_encode

Encodes a PHP value into its binary representation for storage.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$key` | string | Field name determining the encoding type. |
| `$value` | mixed | Value to encode. |

#### Return Value

- **string**: Binary representation of the value.

#### Inner Mechanism

For `string` and `_string` types, returns the value as-is. For `byte` type, converts the integer to a single character using `chr()`.

#### Usage Example

```php
$encoded = $resource->field_encode("id", 65); // Returns "A"
```

---

### field_decode

Decodes a binary field value back into a PHP value.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$key` | string | Field name determining the decoding type. |
| `$value` | string | Raw binary data to decode. |

#### Return Value

- **mixed**: Decoded value. Strings are trimmed of null bytes; bytes are converted to integers.

#### Inner Mechanism

For `string` and `_string` types, strips trailing null bytes using `rtrim()`. For `byte` type, returns `NULL` if empty, otherwise converts the character to its ordinal value using `ord()`.

#### Usage Example

```php
$decoded = $resource->field_decode("id", "A"); // Returns 65
```

---

### lock

Acquires a shared or exclusive lock on the file.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$exclusive` | bool | `FALSE` | If true, requests an exclusive (write) lock. Otherwise, a shared (read) lock. |

#### Inner Mechanism

Maintains a stack (`$this->lock`) to track nested lock requests. Only acquires a new lock if the current top of the stack differs from the requested lock type. Uses `flock()` with `LOCK_EX` or `LOCK_SH`.

#### Usage Example

```php
$resource->lock(TRUE);  // Exclusive lock
$resource->set($data);
$resource->unlock();
```

---

### unlock

Releases the most recent lock.

#### Inner Mechanism

Pops the last lock state from the stack. If the stack becomes empty, releases the lock entirely. If transitioning from exclusive to shared, downgrades to a shared lock.

#### Usage Example

```php
$resource->lock();
$data = $resource->get();
$resource->unlock();
```

---

### close

Closes the file handle if still open.

#### Inner Mechanism

Checks if `$this->hfile` is a valid resource and closes it with `fclose()`.

#### Usage Example

```php
$resource->close(); // Explicitly close file
```


<!-- HASH:5e2b3fb966d0cd2421d62fea1db18fa8 -->
