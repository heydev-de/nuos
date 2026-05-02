# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.map.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Map Class Overview

The `map` class in the NUOS web platform provides a lightweight, file-based key-value storage system optimized for fast lookups using CRC32 hashing. It is designed for scenarios requiring persistent, high-performance mapping between keys and values, such as URL routing, configuration management, or data indexing.

The class maintains four internal arrays:
- **Index by Key**: Maps CRC32 hashes of keys to their internal indices.
- **Index by Value**: Maps CRC32 hashes of values to their internal indices.
- **Data by Key**: Stores the actual keys indexed by their internal indices.
- **Data by Value**: Stores the actual values indexed by their internal indices.

This dual-indexing approach enables efficient bidirectional lookups (key → value and value → key).

---

### Constants

| Name                | Value | Description                                                                 |
|---------------------|-------|-----------------------------------------------------------------------------|
| `CMS_MAP_INDEX_KEY` | `0`   | Array index for the key-to-index mapping (`$data[CMS_MAP_INDEX_KEY]`).     |
| `CMS_MAP_INDEX_VALUE` | `1` | Array index for the value-to-index mapping (`$data[CMS_MAP_INDEX_VALUE]`). |
| `CMS_MAP_DATA_KEY`  | `2`   | Array index for the index-to-key data (`$data[CMS_MAP_DATA_KEY]`).         |
| `CMS_MAP_DATA_VALUE` | `3`  | Array index for the index-to-value data (`$data[CMS_MAP_DATA_VALUE]`).     |

---

### Properties

| Name   | Default Value                                                                 | Description                                                                 |
|--------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `file` | `NULL`                                                                        | Path to the map file on disk.                                               |
| `data` | `[CMS_MAP_INDEX_KEY => [], CMS_MAP_INDEX_VALUE => [], CMS_MAP_DATA_KEY => [], CMS_MAP_DATA_VALUE => []]` | Internal storage structure holding all key-value pairs and their indices. |

---

### `__construct($name = NULL)`

#### Purpose
Initializes a new `map` instance. Optionally opens a map file if a name is provided.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$name` | `string` | (Optional) Name of the map file (without `.map` extension) to open.        |

#### Return Values
- **None**: Constructor does not return a value.

#### Inner Mechanisms
- If `$name` is provided, the constructor calls `open($name)` to load the map from disk.
- If no name is provided, the map is initialized as empty.

#### Usage Context
- Used to create a new `map` object, optionally loading an existing map file.
- Example:
  ```php
  $map = new \cms\map("url_routes");
  ```

---

### `open($name)`

#### Purpose
Opens and loads a map file from disk into memory. Uses temporary caching to avoid repeated disk I/O.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$name` | `string` | Name of the map file (without `.map` extension) to open.                   |

#### Return Values
- **`TRUE`**: Map was successfully loaded (either from cache or disk).
- **`FALSE`**: (Implicit) If file operations fail, the method returns `TRUE` but leaves the map empty.

#### Inner Mechanisms
1. **Cache Check**: Attempts to retrieve the map from the temporary cache using the key `"map." . $this->file`.
2. **File Load**: If not in cache, reads the map file line by line:
   - Each key-value pair is stored on two consecutive lines.
   - Keys and values are hashed using `crc32()` and stored in the appropriate index arrays.
   - The actual key and value are stored in the data arrays using an auto-incremented index.
3. **Caching**: The loaded map is stored in the temporary cache for future use.

#### Usage Context
- Used to load an existing map file into memory for manipulation.
- Example:
  ```php
  $map->open("user_preferences");
  ```

---

### `save($name = NULL)`

#### Purpose
Saves the current map data to a file on disk. Updates the temporary cache to reflect the saved state.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$name` | `string` | (Optional) Name of the map file (without `.map` extension) to save to. If provided, updates the `file` property. |

#### Return Values
- **`TRUE`**: Map was successfully saved.
- **`FALSE`**: Failed to create the directory or open the file for writing.

#### Inner Mechanisms
1. **File Path**: If `$name` is provided, updates the `file` property to point to the new map file.
2. **Directory Creation**: Uses `mkpath()` to ensure the target directory exists.
3. **File Writing**: Opens the file in write mode (`"wb"`) and locks it exclusively (`LOCK_EX`).
4. **Data Serialization**: Iterates over the `CMS_MAP_DATA_KEY` array, writing each non-empty key and its corresponding value on two consecutive lines.
5. **Caching**: Updates the temporary cache with the current map data.

#### Usage Context
- Used to persist in-memory map changes to disk.
- Example:
  ```php
  $map->set("home", "/index.php");
  $map->save("url_routes");
  ```

---

### `set($key, $value)`

#### Purpose
Adds or updates a key-value pair in the map. Maintains both key and value indices for bidirectional lookups.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$key`  | `string` | The key to set.                                                             |
| `$value` | `string` | The value to associate with the key.                                        |

#### Return Values
- **`TRUE`**: The key-value pair was successfully set.

#### Inner Mechanisms
1. **Hashing**: Computes CRC32 hashes for the key and value.
2. **Key Exists Check**:
   - If the key already exists, retrieves its index and removes the old value from the value index.
   - If the key does not exist, generates a new index by incrementing the highest existing index.
3. **Index Update**: Updates the key and value indices with the new or existing index.
4. **Data Storage**: Stores the key and value in the data arrays using the index.

#### Usage Context
- Used to populate or update the map with key-value pairs.
- Example:
  ```php
  $map->set("contact", "/contact.php");
  ```

---

### `get_value($key)`

#### Purpose
Retrieves the value associated with a given key.

#### Parameters

| Name  | Type     | Description                                                                 |
|-------|----------|-----------------------------------------------------------------------------|
| `$key` | `string` | The key whose value should be retrieved.                                    |

#### Return Values
- **`string`**: The value associated with the key.
- **`NULL`**: The key does not exist in the map.

#### Inner Mechanisms
1. **Hashing**: Computes the CRC32 hash of the key.
2. **Index Lookup**: Checks if the key exists in the key index.
3. **Value Retrieval**: If the key exists, retrieves the value from the data array using the index.

#### Usage Context
- Used to fetch values by their keys, e.g., resolving URLs from route names.
- Example:
  ```php
  $url = $map->get_value("contact"); // Returns "/contact.php"
  ```

---

### `get_value_list()`

#### Purpose
Retrieves all values in the map as a flat array.

#### Parameters
- **None**

#### Return Values
- **`array`**: A numerically indexed array of all values in the map.

#### Inner Mechanisms
- Returns the values from `CMS_MAP_DATA_VALUE` using `array_values()` to reindex the array numerically.

#### Usage Context
- Used to retrieve all values for iteration or bulk processing.
- Example:
  ```php
  $urls = $map->get_value_list();
  ```

---

### `get_key($value)`

#### Purpose
Retrieves the key associated with a given value.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | The value whose key should be retrieved.                                    |

#### Return Values
- **`string`**: The key associated with the value.
- **`NULL`**: The value does not exist in the map.

#### Inner Mechanisms
1. **Hashing**: Computes the CRC32 hash of the value.
2. **Index Lookup**: Checks if the value exists in the value index.
3. **Key Retrieval**: If the value exists, retrieves the key from the data array using the index.

#### Usage Context
- Used for reverse lookups, e.g., finding a route name by its URL.
- Example:
  ```php
  $name = $map->get_key("/contact.php"); // Returns "contact"
  ```

---

### `del_key($key)`

#### Purpose
Removes a key-value pair from the map by its key.

#### Parameters

| Name  | Type     | Description                                                                 |
|-------|----------|-----------------------------------------------------------------------------|
| `$key` | `string` | The key to remove.                                                          |

#### Return Values
- **None**

#### Inner Mechanisms
1. **Hashing**: Computes the CRC32 hash of the key.
2. **Existence Check**: If the key exists, retrieves its index and the CRC32 hash of its value.
3. **Index Cleanup**: Removes the key and value from all four internal arrays.

#### Usage Context
- Used to remove obsolete or invalid key-value pairs.
- Example:
  ```php
  $map->del_key("old_route");
  ```

---

### `del_value($value)`

#### Purpose
Removes a key-value pair from the map by its value.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | The value to remove.                                                        |

#### Return Values
- **None**

#### Inner Mechanisms
1. **Hashing**: Computes the CRC32 hash of the value.
2. **Existence Check**: If the value exists, retrieves its index and the CRC32 hash of its key.
3. **Index Cleanup**: Removes the key and value from all four internal arrays.

#### Usage Context
- Used for cleanup when a value is no longer valid.
- Example:
  ```php
  $map->del_value("/deprecated.php");
  ```


<!-- HASH:8e2d81d7a79345d27b29b511bebb67d6 -->
