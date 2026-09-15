# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.plist.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.plist.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## plist

The `plist` class provides a lightweight, file-based persistent list storage mechanism. It stores records of fixed length in a binary file, each prefixed with a CRC32 hash for integrity verification. The class supports adding, retrieving, and removing records with optional deduplication, using file locking for concurrency safety.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$hfile` | `NULL` | File handle resource for the underlying `.lst` file |
| `$record_length` | `NULL` | Maximum length (in bytes) of each stored record's value |
| `$buffer_length` | `65536` | Buffer size (in bytes) used during bulk read/write operations |

### __construct

Initializes a new plist instance by opening (or creating) a `.lst` file.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | string | — | Logical name or full path of the list file. If it doesn't end with `.lst`, it's resolved under `CMS_DATA_PATH` |
| `$record_length` | int | `500` | Maximum byte length of each record's value |

#### Usage Example

```php
$list = new plist("mylist", 256);
// Opens or creates CMS_DATA_PATH/mylist.lst with 256-byte records
```

### add

Appends a new record to the list, optionally removing any existing identical records first.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | The value to store (truncated/padded to `$record_length`) |
| `$remove_existing` | bool | `FALSE` | If true, removes all existing records matching both hash and value before appending |

#### Return Values

- `TRUE` on success
- `FALSE` if the file handle is invalid

#### Inner Mechanisms

1. Truncates `$value` to `$record_length` bytes and pads it with null bytes to ensure fixed size
2. Computes a CRC32 hash of the padded value and packs it as a 4-byte big-endian integer
3. If `$remove_existing` is true, scans backwards through the file to find and remove matching records
4. Appends the hash + value to the end of the file

#### Usage Example

```php
$list = new plist("visitors");
$list->add("192.168.1.1", true); // Add IP, remove duplicates
```

### get

Retrieves a range of records from the list, optionally removing them after reading.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$offset` | int | `0` | Starting position from the end of the list (0 = last record) |
| `$length` | int | `1` | Number of records to retrieve |
| `$remove` | bool | `FALSE` | If true, removes retrieved records from the file |

#### Return Values

- Array of strings (null-byte trimmed) on success
- `FALSE` if the file handle is invalid

#### Inner Mechanisms

1. Calculates the starting position from the end of the file based on `$offset`
2. Reads records backwards, collecting values into an array
3. If `$remove` is true, calls `remove()` to delete the retrieved records

#### Usage Example

```php
$list = new plist("log");
$recent = $list->get(0, 10); // Get last 10 records
```

### length

Returns the total number of records currently stored in the list.

#### Return Values

- Integer count of records on success
- `FALSE` if the file handle is invalid

#### Usage Example

```php
$list = new plist("queue");
echo $list->length(); // Outputs current record count
```

### remove

Removes a range of records from the list, shifting subsequent records to fill the gap.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$offset` | int | — | Position from the end of the list (0 = last record) |
| `$length` | int | `1` | Number of records to remove |

#### Return Values

- `TRUE` on success
- `FALSE` if the file handle is invalid

#### Inner Mechanisms

1. Calculates the absolute position from the end of the file
2. Shifts all records after the target range forward to overwrite removed records
3. Truncates the file to remove the gap

#### Usage Example

```php
$list = new plist("temp");
$list->remove(0, 5); // Remove last 5 records
```

### __destruct

Closes the file handle when the plist object is destroyed.

#### Usage Example

```php
$list = new plist("data");
$list->add("item");
unset($list); // Triggers __destruct, closing the file
```


<!-- HASH:8700d6b9b81a5fc7f481c4bb03a09905 -->
