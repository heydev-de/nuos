# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.plist.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.plist.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## PLIST Class

The `plist` class provides a persistent, file-based list storage mechanism optimized for high-performance, fixed-length record operations. It is designed for scenarios requiring frequent additions, retrievals, and removals of records from the end of a list (LIFO behavior), with optional deduplication. The class ensures thread-safe operations via file locking and minimizes I/O overhead through buffered reads/writes.

### Properties

| Name              | Value/Default | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$hfile`          | `NULL`        | File handle for the underlying `.lst` file.                                 |
| `$record_length`  | `NULL`        | Maximum length of each record in bytes.                                     |
| `$buffer_length`  | `65536`       | Buffer size for read/write operations (64KB).                               |

---

### `__construct`

#### Purpose
Initializes a new `plist` instance, creating the underlying `.lst` file if it does not exist. The file is opened in binary mode with unbuffered I/O for performance.

#### Parameters

| Name              | Type      | Description                                                                 |
|-------------------|-----------|-----------------------------------------------------------------------------|
| `$name`           | `string`  | Base name of the list file. If no `.lst` extension is provided, it is appended automatically. The file is stored in `CMS_DATA_PATH`. |
| `$record_length`  | `int`     | (Optional) Maximum record length in bytes. Default: `500`.                  |

#### Return Values
- **None**: Constructor initializes the object.

#### Inner Mechanisms
1. **File Path Resolution**: Appends `.lst` to `$name` if missing and prepends `CMS_DATA_PATH`.
2. **File Creation**: Creates parent directories if they do not exist (`mkpath`).
3. **File Handling**: Opens the file in `c+b` mode (create, read/write, binary) with unbuffered I/O (`stream_set_read_buffer`, `stream_set_write_buffer`).
4. **Record Length**: Stores `$record_length` for later use in record padding/truncation.

#### Usage Context
- Use when persistent, high-performance list storage is needed (e.g., task queues, logs, or temporary data).
- Ideal for environments where database overhead is undesirable.

#### Example
```php
$plist = new \cms\plist("user_tasks", 256);
// Creates/opens CMS_DATA_PATH/user_tasks.lst with 256-byte records.
```

---

### `add`

#### Purpose
Appends a new record to the end of the list. Optionally removes existing duplicates of the same value.

#### Parameters

| Name                | Type      | Description                                                                 |
|---------------------|-----------|-----------------------------------------------------------------------------|
| `$value`            | `string`  | The record value to add. Truncated to `$record_length` if longer.          |
| `$remove_existing`  | `bool`    | (Optional) If `TRUE`, removes all existing records with the same value. Default: `FALSE`. |

#### Return Values
- `bool`: `TRUE` on success, `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`) on the file.
2. **Value Preparation**: Truncates/pads `$value` to `$record_length` with null bytes.
3. **Hashing**: Computes a CRC32 hash of the value for efficient duplicate detection.
4. **Duplicate Removal**: If `$remove_existing` is `TRUE`, scans the file backward for matching hashes/values and shifts subsequent records to overwrite duplicates.
5. **Appending**: Writes the hash and value to the end of the file.
6. **Unlocking**: Releases the lock (`LOCK_UN`).

#### Usage Context
- Use for adding records to a persistent queue or log.
- Enable `$remove_existing` to enforce uniqueness (e.g., deduplicated task queues).

#### Example
```php
$plist->add("Process user #12345", TRUE);
// Adds the task and removes any existing identical tasks.
```

---

### `get`

#### Purpose
Retrieves records from the end of the list (LIFO order) starting at a specified offset. Optionally removes retrieved records.

#### Parameters

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$offset`   | `int`     | (Optional) Zero-based offset from the end of the list. Default: `0` (newest record). |
| `$length`   | `int`     | (Optional) Number of records to retrieve. If `0`, retrieves all records. Default: `1`. |
| `$remove`   | `bool`    | (Optional) If `TRUE`, removes retrieved records. Default: `FALSE`.          |

#### Return Values
- `array|bool`: Array of retrieved records (trimmed of null bytes) or `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`).
2. **Offset Calculation**: Converts `$offset` to a file position (e.g., `0` = last record).
3. **Reading**: Reads records backward from the calculated position, stopping at `$length` or the start of the file.
4. **Removal**: Calls `remove` if `$remove` is `TRUE`.
5. **Unlocking**: Releases the lock.

#### Usage Context
- Use for processing records in LIFO order (e.g., task queues, logs).
- Set `$remove` to `TRUE` for destructive reads (e.g., queue processing).

#### Example
```php
$tasks = $plist->get(0, 5, TRUE);
// Retrieves and removes the 5 newest tasks.
```

---

### `length`

#### Purpose
Returns the number of records in the list.

#### Parameters
- **None**

#### Return Values
- `int|bool`: Number of records or `FALSE` if the file handle is invalid.

#### Inner Mechanisms
- Divides the file size by the record length (including the 4-byte hash) to compute the count.

#### Usage Context
- Use to monitor list size (e.g., queue depth).

#### Example
```php
$count = $plist->length();
echo "Pending tasks: $count";
```

---

### `remove`

#### Purpose
Removes records from the end of the list (LIFO order) starting at a specified offset.

#### Parameters

| Name      | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$offset` | `int`     | Zero-based offset from the end of the list.                                |
| `$length` | `int`     | (Optional) Number of records to remove. If `0`, removes all records from `$offset` to the end. Default: `1`. |

#### Return Values
- `bool`: `TRUE` on success, `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`).
2. **Offset Calculation**: Converts `$offset` to a file position.
3. **Shifting**: Reads records after the removal range and shifts them backward to overwrite removed records.
4. **Truncation**: Reduces the file size to exclude removed records.
5. **Unlocking**: Releases the lock.

#### Usage Context
- Use for manual list pruning (e.g., removing stale tasks).

#### Example
```php
$plist->remove(10, 5);
// Removes 5 records starting from the 10th newest.
```

---

### `__destruct`

#### Purpose
Closes the file handle when the object is destroyed.

#### Parameters
- **None**

#### Return Values
- **None**

#### Inner Mechanisms
- Calls `fclose` on `$hfile` if it is valid.

#### Usage Context
- Automatically invoked when the object is garbage-collected or script execution ends.


<!-- HASH:8700d6b9b81a5fc7f481c4bb03a09905 -->
