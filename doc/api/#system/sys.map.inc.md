# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.map.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.map.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Map Class

The `map` class provides a high-performance, file-based key-value storage system optimized for fast lookups in both directions (key-to-value and value-to-key). It uses CRC32 hashing for indexing and maintains four internal arrays to enable bidirectional mapping. Data is stored in a custom binary format with atomic write operations to ensure consistency.

### Constants

| Name                | Value | Description                          |
|---------------------|-------|--------------------------------------|
| `CMS_MAP_INDEX_KEY` | `0`   | Index for key-to-index mapping       |
| `CMS_MAP_INDEX_VALUE` | `1` | Index for value-to-index mapping     |
| `CMS_MAP_DATA_KEY`  | `2`   | Index for index-to-key mapping       |
| `CMS_MAP_DATA_VALUE` | `3`  | Index for index-to-value mapping     |

### Properties

| Name  | Default Value | Description                          |
|-------|---------------|--------------------------------------|
| `file` | `NULL`        | Path to the map file                 |
| `data` | Four empty arrays | Internal storage structure for keys, values, and their indices |

---

### `__construct($name = NULL)`

**Purpose:**
Initializes a new `map` instance. Optionally opens a map file if a name is provided.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$name` | `string` | Name of the map file to open (without extension) |

**Return Values:**
- None (constructor)

**Inner Mechanisms:**
- If `$name` is provided, calls `open($name)` to load the map file.
- Initializes the `data` property with four empty arrays if no file is opened.

**Usage Context:**
- Used to create a new map instance, either in-memory or backed by a file.

**Example:**
```php
$map = new \cms\map("user_preferences"); // Opens or creates "user_preferences.map"
```

---

### `open($name)`

**Purpose:**
Opens a map file and loads its contents into memory. Uses temporary caching to avoid repeated file reads.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$name` | `string` | Name of the map file to open (without extension) |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure (though failure cases are not explicitly handled in the code)

**Inner Mechanisms:**
1. Sets the `file` property to the full path of the map file.
2. Checks the temporary cache for existing data. If found, loads it into `data`.
3. If not cached, reads the file line by line, parsing key-value pairs.
4. Populates the four internal arrays:
   - `CMS_MAP_INDEX_KEY`: Maps CRC32 hashes of keys to their indices.
   - `CMS_MAP_INDEX_VALUE`: Maps CRC32 hashes of values to their indices.
   - `CMS_MAP_DATA_KEY`: Maps indices to keys.
   - `CMS_MAP_DATA_VALUE`: Maps indices to values.
5. Caches the loaded data in memory for future use.

**Usage Context:**
- Used to load an existing map file into memory for manipulation.

**Example:**
```php
$map = new \cms\map();
$map->open("product_categories"); // Loads "product_categories.map"
```

---

### `save($name = NULL)`

**Purpose:**
Saves the current map data to a file. Uses atomic write operations to prevent corruption.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$name` | `string` | Optional new name for the map file (without extension) |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. If `$name` is provided, updates the `file` property to the new path.
2. Creates the directory structure if it does not exist.
3. Writes data to a temporary file to ensure atomicity.
4. Uses file locking (`flock`) to prevent concurrent writes.
5. Iterates over `CMS_MAP_DATA_KEY` and writes each key-value pair to the file.
6. Replaces the original file with the temporary file atomically.
7. Updates the temporary cache with the current data.

**Usage Context:**
- Used to persist in-memory changes to disk.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$map->save(); // Saves to "user_preferences.map"
```

---

### `set($key, $value)`

**Purpose:**
Associates a key with a value in the map. Updates the value if the key already exists.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$key`  | `string` | Key to associate                     |
| `$value` | `string` | Value to associate with the key      |

**Return Values:**
- `TRUE` on success

**Inner Mechanisms:**
1. Computes CRC32 hashes for the key and value.
2. If the key already exists:
   - Removes the old value's index from `CMS_MAP_INDEX_VALUE`.
   - Reuses the existing index for the new value.
3. If the key does not exist:
   - Generates a new index by incrementing the highest existing index.
4. Updates all four internal arrays to reflect the new association.

**Usage Context:**
- Used to add or update key-value pairs in the map.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("language", "en_US"); // Sets "language" to "en_US"
$map->set("language", "fr_FR"); // Updates "language" to "fr_FR"
```

---

### `get_value($key)`

**Purpose:**
Retrieves the value associated with a given key.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$key` | `string` | Key to look up                       |

**Return Values:**
- `string`: The associated value if the key exists
- `NULL`: If the key does not exist

**Inner Mechanisms:**
1. Computes the CRC32 hash of the key.
2. Checks `CMS_MAP_INDEX_KEY` for the hash. If found, retrieves the index.
3. Uses the index to retrieve the value from `CMS_MAP_DATA_VALUE`.

**Usage Context:**
- Used to fetch values by their keys.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$theme = $map->get_value("theme"); // Returns "dark"
```

---

### `get_value_list()`

**Purpose:**
Retrieves all values in the map as an indexed array.

**Parameters:**
- None

**Return Values:**
- `array`: An indexed array of all values in the map

**Inner Mechanisms:**
- Returns the values from `CMS_MAP_DATA_VALUE` as an indexed array.

**Usage Context:**
- Used to retrieve all values for iteration or bulk processing.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$map->set("language", "en_US");
$values = $map->get_value_list(); // Returns ["dark", "en_US"]
```

---

### `get_key($value)`

**Purpose:**
Retrieves the key associated with a given value.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value` | `string` | Value to look up                     |

**Return Values:**
- `string`: The associated key if the value exists
- `NULL`: If the value does not exist

**Inner Mechanisms:**
1. Computes the CRC32 hash of the value.
2. Checks `CMS_MAP_INDEX_VALUE` for the hash. If found, retrieves the index.
3. Uses the index to retrieve the key from `CMS_MAP_DATA_KEY`.

**Usage Context:**
- Used to fetch keys by their values (reverse lookup).

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$key = $map->get_key("dark"); // Returns "theme"
```

---

### `del_key($key)`

**Purpose:**
Removes a key and its associated value from the map.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$key` | `string` | Key to remove                        |

**Return Values:**
- None

**Inner Mechanisms:**
1. Computes the CRC32 hash of the key.
2. If the key exists:
   - Retrieves the index and the associated value.
   - Removes the key from `CMS_MAP_INDEX_KEY`.
   - Removes the value from `CMS_MAP_INDEX_VALUE` if no other keys reference it.
   - Removes the key and value from `CMS_MAP_DATA_KEY` and `CMS_MAP_DATA_VALUE`.

**Usage Context:**
- Used to remove entries from the map by key.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$map->del_key("theme"); // Removes the "theme" entry
```

---

### `del_value($value)`

**Purpose:**
Removes all keys associated with a given value from the map.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value` | `string` | Value to remove                      |

**Return Values:**
- None

**Inner Mechanisms:**
1. Computes the CRC32 hash of the value.
2. If the value exists:
   - Retrieves the index and the associated key.
   - Removes the value from `CMS_MAP_INDEX_VALUE`.
   - Removes the key from `CMS_MAP_INDEX_KEY`.
   - Removes the key and value from `CMS_MAP_DATA_KEY` and `CMS_MAP_DATA_VALUE`.

**Usage Context:**
- Used to remove entries from the map by value.

**Example:**
```php
$map = new \cms\map("user_preferences");
$map->set("theme", "dark");
$map->del_value("dark"); // Removes the entry with value "dark"
```


<!-- HASH:f3f39f24287aadad60ab5ab51cd90d76 -->
