# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.plist.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## PLIST Class Overview

The `plist` class provides a persistent, file-based list storage mechanism optimized for high-performance read/write operations. It implements a fixed-record-length binary file format with CRC32 hashing for data integrity and efficient duplicate detection. This class is particularly suited for:

- **Queue management** (FIFO/LIFO operations)
- **Persistent data storage** with minimal memory footprint
- **High-frequency logging** or event tracking
- **Caching systems** requiring disk persistence

Key characteristics:
- **Fixed record length** ensures predictable performance
- **Binary storage** minimizes disk I/O overhead
- **File locking** guarantees thread-safe operations
- **Zero dependencies** on external libraries
- **Memory-efficient** processing via buffered I/O

---

## Properties

| Name              | Default Value | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$hfile`          | `NULL`        | File handle resource for the underlying `.lst` file                        |
| `$record_length`  | `NULL`        | Maximum length (bytes) of each stored record (set during construction)     |
| `$buffer_length`  | `65536`       | I/O buffer size (bytes) for bulk operations (64KB default)                 |

---

## Constructor

### `plist::__construct()`

**Purpose:**
Initializes a new persistent list instance, creating the underlying storage file if necessary.

**Parameters:**

| Name              | Type     | Description                                                                 |
|-------------------|----------|-----------------------------------------------------------------------------|
| `$name`           | `string` | Base name/path for the list file. Automatically appends `.lst` if missing.  |
| `$record_length`  | `int`    | (Optional) Maximum record length in bytes. Default: `500`.                 |

**Inner Mechanisms:**
1. **File Resolution:**
   - Appends `.lst` extension if not present
   - Prefixes with `CMS_DATA_PATH` if no absolute path is provided
2. **File Creation:**
   - Creates parent directories via `mkpath()` if they don't exist
   - Opens file in `c+b` mode (create/read/write/binary)
3. **Initialization:**
   - Stores file handle and record length for subsequent operations

**Usage Context:**
```php
// Basic usage
$queue = new \cms\plist('user_actions');

// Custom record length
$log = new \cms\plist('system_events', 1024);
```

---

## Methods

### `plist::add()`

**Purpose:**
Appends a new record to the list, optionally removing existing duplicates.

**Parameters:**

| Name                | Type      | Description                                                                 |
|---------------------|-----------|-----------------------------------------------------------------------------|
| `$value`            | `string`  | Data to store (truncated to `$record_length` if longer)                    |
| `$remove_existing`  | `bool`    | (Optional) If `TRUE`, removes all existing instances of `$value`. Default: `FALSE`. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` if file handle is invalid

**Inner Mechanisms:**
1. **Data Preparation:**
   - Truncates/pads input to `$record_length` with null bytes
   - Generates CRC32 hash of the prepared value
2. **Duplicate Handling (if enabled):**
   - Scans file backward from end to beginning
   - Compares both hash and value for matches
   - Removes all matches via buffered block shifting
3. **Appending:**
   - Writes hash (4 bytes) + value at file end
   - Maintains exclusive file lock during operation

**Usage Context:**
```php
// Simple append
$queue->add('user_login:12345');

// Append with deduplication
$queue->add('cache_invalidate:homepage', TRUE);
```

---

### `plist::get()`

**Purpose:**
Retrieves records from the list in reverse chronological order (newest first).

**Parameters:**

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$offset`  | `int`    | (Optional) Number of records to skip from the end. Default: `0`.            |
| `$length`  | `int`    | (Optional) Maximum records to return. `0` = all remaining. Default: `1`.    |
| `$remove`  | `bool`   | (Optional) If `TRUE`, removes returned records. Default: `FALSE`.           |

**Return Values:**
- `array|bool`: Array of records (newest first) or `FALSE` on failure

**Inner Mechanisms:**
1. **Position Calculation:**
   - Converts offset to absolute file position (newest = end of file)
   - Handles negative offsets by clamping to valid range
2. **Buffered Reading:**
   - Reads records backward from calculated position
   - Strips null padding from retrieved values
3. **Removal (if enabled):**
   - Delegates to `remove()` with calculated offset/length

**Usage Context:**
```php
// Get newest record
$latest = $queue->get();

// Get next 5 records (without removal)
$batch = $queue->get(0, 5);

// Get and remove oldest 10 records
$oldest = $queue->get(0, 10, TRUE);
```

---

### `plist::length()`

**Purpose:**
Returns the current number of records in the list.

**Return Values:**
- `int|bool`: Record count or `FALSE` if file handle is invalid

**Inner Mechanisms:**
- Divides file size by record length (including 4-byte hash prefix)
- Uses integer division for accurate count

**Usage Context:**
```php
if ($queue->length() > 1000) {
    // Process batch when queue exceeds threshold
}
```

---

### `plist::remove()`

**Purpose:**
Deletes records from the list by position (newest first).

**Parameters:**

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$offset`  | `int`    | Number of records to skip from the end (0 = newest record)                  |
| `$length`  | `int`    | (Optional) Number of records to remove. `0` = all remaining. Default: `1`.  |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` if file handle is invalid

**Inner Mechanisms:**
1. **Position Calculation:**
   - Converts offset to absolute file position
   - Clamps values to valid range
2. **Buffered Removal:**
   - Shifts subsequent records forward to overwrite target range
   - Uses `$buffer_length` for efficient bulk operations
3. **Truncation:**
   - Reduces file size to new logical end

**Usage Context:**
```php
// Remove newest record
$queue->remove(0);

// Remove oldest 100 records
$queue->remove(0, 100);
```

---

### `plist::__destruct()`

**Purpose:**
Automatically closes the file handle when the object is destroyed.

**Inner Mechanisms:**
- Checks for valid file handle before closing
- Ensures proper resource cleanup

**Usage Context:**
- Implicitly called during object destruction (e.g., `unset()` or script end)
- Manual invocation is unnecessary


<!-- HASH:bbb739dbbfff0ab56cdca0663584ec6f -->
