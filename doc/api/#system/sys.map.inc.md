# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.map.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.map.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## map

The `map` class provides a persistent key-value storage mechanism backed by flat files. It maintains bidirectional indexing between keys and values, allowing efficient lookups in both directions. Data is stored in a simple line-based format where each key-value pair occupies two consecutive lines in a `.map` file. The class supports deferred saving with versioning to prevent unnecessary disk writes, and integrates with the PWNC caching system for improved performance.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `$file` | string\|NULL | Full path to the map file on disk |
| `$data` | array | Internal data structure containing four indexed arrays for bidirectional key-value mapping |
| `$version` | array (static) | Global version tracking for deferred saves across instances |
| `$_version` | array | Instance-level version tracking for deferred saves |

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAP_INDEX_KEY` | 0 | Index for key-to-index lookup array |
| `CMS_MAP_INDEX_VALUE` | 1 | Index for value-to-index lookup array |
| `CMS_MAP_DATA_KEY` | 2 | Index for index-to-key data array |
| `CMS_MAP_DATA_VALUE` | 3 | Index for index-to-value data array |

### __construct

#### __construct($name = NULL)

Initializes a new map instance, optionally opening an existing map file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string\|NULL | Map name (without extension) to open immediately |

**Returns:** void

**Mechanism:** If a name is provided, calls `open()` to load the map data from cache or file.

**Usage:**
```php
// Create empty map
$m = new map();

// Open existing map named "redirects"
$m = new map("redirects");
```

### __destruct

#### __destruct()

Automatically saves any modified maps when the object is destroyed, but only if the version hasn't changed since the last modification.

**Mechanism:** Iterates through deferred versions and calls `save()` with force flag for unchanged versions.

**Usage:**
```php
$m = new map("config");
$m->set("debug", "true");
// Destructor automatically saves when $m goes out of scope
```

### open

#### open($name)

Loads map data from cache or file into memory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | Map name (without extension) |

**Returns:** bool — Always returns TRUE

**Mechanism:**
1. Sets the file path using `CMS_DATA_PATH`
2. Attempts to retrieve data from cache via `cms_cache()`
3. If not cached, reads the file line-by-line, building four internal arrays:
   - Key-to-index mapping (using CRC32 hashes)
   - Value-to-index mapping (using CRC32 hashes)
   - Index-to-key data
   - Index-to-value data
4. Stores loaded data in cache

**Usage:**
```php
$m = new map();
$m->open("translations");
// Now $m contains all key-value pairs from translations.map
```

### save

#### save($name = NULL, $force = FALSE)

Persists map data to disk with support for deferred saving.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string\|NULL | Map name to save (defaults to current file) |
| `$force` | bool | If TRUE, immediately writes to disk; if FALSE, defers save |

**Returns:** bool — TRUE on success, FALSE on failure

**Mechanism:**
- **Deferred mode ($force=FALSE):** Increments version counter, updates cache, returns TRUE if file is writable
- **Forced mode ($force=TRUE):** 
  1. Removes version tracking
  2. Writes data to temporary file with exclusive lock
  3. Atomically replaces original file via `rename()`
  4. Updates cache

**Usage:**
```php
$m = new map("settings");
$m->set("theme", "dark");
$m->save(); // Defer save

// Later, force immediate save
$m->save(NULL, TRUE);
```

### set

#### set($key, $value)

Adds or updates a key-value pair in the map.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Key to set |
| `$value` | string | Value to associate with key |

**Returns:** bool — Always returns TRUE

**Mechanism:**
1. Computes CRC32 hashes for key and value
2. If key exists, removes old value's index entry
3. If key doesn't exist, assigns next available index
4. Updates all four internal arrays

**Usage:**
```php
$m = new map("routes");
$m->set("/home", "index.php");
$m->set("/about", "about.php");
```

### get_value

#### get_value($key)

Retrieves the value associated with a given key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Key to look up |

**Returns:** string\|NULL — The associated value, or NULL if not found

**Mechanism:** Computes CRC32 of key, looks up index in key-to-index array, then retrieves value from index-to-value array.

**Usage:**
```php
$m = new map("config");
$debug = $m->get_value("debug_mode"); // Returns "true" or NULL
```

### get_value_list

#### get_value_list()

Returns all values in the map.

**Returns:** array — Array of all values

**Mechanism:** Returns `array_values()` of the index-to-value data array.

**Usage:**
```php
$m = new map("users");
$all_users = $m->get_value_list();
```

### get_key

#### get_key($value)

Retrieves the key associated with a given value (reverse lookup).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Value to look up |

**Returns:** string\|NULL — The associated key, or NULL if not found

**Mechanism:** Computes CRC32 of value, looks up index in value-to-index array, then retrieves key from index-to-key array.

**Usage:**
```php
$m = new map("redirects");
$source = $m->get_key("/new-location"); // Find which URL redirects here
```

### del_key

#### del_key($key)

Removes a key-value pair by key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Key to remove |

**Returns:** void

**Mechanism:**
1. Computes CRC32 of key
2. Retrieves associated index and value
3. Removes entries from all four internal arrays
4. Cleans up value-to-index entry if it points to the same index

**Usage:**
```php
$m = new map("temp_data");
$m->del_key("session_123");
```

### del_value

#### del_value($value)

Removes a key-value pair by value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Value to remove |

**Returns:** void

**Mechanism:**
1. Computes CRC32 of value
2. Retrieves associated index and key
3. Removes entries from all four internal arrays
4. Cleans up key-to-index entry

**Usage:**
```php
$m = new map("file_aliases");
$m->del_value("/old/path/file.txt");
```


<!-- HASH:2ffcf7aa1e4276ebb8bd79ea9b06723b -->
