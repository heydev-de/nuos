# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.data.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.data.inc)

- **Version:** `26.9.7.12`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The `sys.data.inc` file implements the **Data Class** — a hierarchical key-value storage system for the PWNC Web Platform. It provides persistent data storage in `.dat` files with support for:

- **Hierarchical containers** (nested data structures using `#type` markers)
- **Automatic expiry** of entries via `#expire` timestamps
- **Password-based encryption** of stored values
- **Prefix-based virtual filesystems** (e.g., `image://`, `media://`, `download://`)
- **Deferred saving** to batch write operations and reduce I/O
- **In-memory caching** via `cms_cache`
- **Cursor-based navigation** through data entries

The data file format is a line-based text format where each line represents a key with its properties:
```
key:=prop1=val1;prop2=val2;prop3=val3
```

Keys and values are encoded using `encchr`/`decchr` (custom encoding functions). Special `#type` properties mark container boundaries (`"container"` opens, `"/container"` closes).

---

## Related Functions

### data_sort

Builds a hierarchical tree from the data object, sorts it by a specified property using natural string comparison, then reconstructs the data in sorted order.

| Parameter | Type | Description |
|-----------|------|-------------|
| `&$data` | `data` | Reference to the data object to sort |
| `$property` | `string` | Property name to sort by (e.g., `"#order"`) |
| `$key` | `string\|NULL` | Optional starting key; if empty, starts from root |

**Return:** `void` — modifies `$data->data` in place.

**Inner mechanisms:**
1. **Tree construction:** Iterates through data using cursor navigation (`_move`), building a nested array structure. Container entries (`"#type" == "container"`) push onto a stack; closing containers (`"#type" == "/container"`) pop from the stack.
2. **Sorting:** Uses `uasort` with the `_data_sort` comparator to sort each level of the tree by the specified property's value.
3. **Reconstruction:** Traverses the sorted tree in depth-first order, rebuilding `$data->data` with entries in their new sorted positions.

**Usage example:**
```php
$d = new data("mydata");
$d->set("apple", "item1", "#order");
$d->set("banana", "item2", "#order");
$d->set("cherry", "item3", "#order");
data_sort($d, "#order");
// Data is now sorted: item1, item2, item3
```

---

### _data_sort

Comparison callback for `data_sort`. Compares two array elements by their `#!` (sort key) property using multibyte-safe natural string comparison.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value1` | `array` | First element to compare |
| `$value2` | `array` | Second element to compare |

**Return:** `int` — negative if `$value1 < $value2`, positive if greater, 0 if equal.

**Inner mechanisms:** Uses `utf8_strnatcasecmp` for case-insensitive natural ordering, which handles numeric substrings intelligently (e.g., "item2" sorts before "item10").

---

## data Class

### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `$data` | `array\|NULL` | `[]` | The actual data storage (key → property → value) |
| `$file` | `string\|NULL` | `NULL` | Physical file path for persistence |
| `$buffer` | `array\|NULL` | `NULL` | Temporary buffer for cut/copy/insert operations |
| `$password` | `string\|NULL` | `NULL` | Hashed password for encryption (via `hash64`) |
| `$prefix` | `string\|NULL` | `NULL` | Key prefix for virtual filesystem mapping |
| `$prefix_length` | `int` | `0` | Cached length of `$prefix` for performance |
| `$defer` (static) | `array` | `[]` | Registry of objects pending deferred save |

---

### __construct

Creates a new data instance. If a name is provided, immediately opens the corresponding data file.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$name` | `string\|NULL` | `NULL` | Data file name (without `.dat` extension) or full path |
| `$password` | `string\|NULL` | `NULL` | Password for value encryption |
| `$prefix` | `string\|NULL` | `NULL` | Key prefix for virtual filesystem |

**Return:** `void`

**Inner mechanisms:** If `$name` is non-empty, delegates to `open()`. Otherwise, sets password and prefix directly, creating an in-memory-only data object.

**Usage example:**
```php
// Open existing data file
$d = new data("users");

// Create in-memory data with encryption
$d = new data(NULL, "secret123");
$d->set("config", "debug", "value");
```

---

### __destruct

Automatically saves the data object if it's registered for deferred saving.

**Return:** `void`

**Inner mechanisms:** Checks if this object is the "last writer" for its file in the `$defer` registry. If so, performs a forced save. This ensures deferred writes are flushed when the object goes out of scope.

---

### open

Loads data from a `.dat` file into memory, with caching and automatic expiry processing.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$name` | `string` | — | File name or path |
| `$password` | `string\|NULL` | `NULL` | Password for decryption |
| `$prefix` | `string\|NULL` | `NULL` | Key prefix |

**Return:** `bool` — `TRUE` on success.

**Inner mechanisms:**
1. Resolves the file path (appends `.dat` and `CMS_DATA_PATH` if needed).
2. Sets password and prefix.
3. Checks `cms_cache` for a cached copy — returns immediately if found.
4. Opens the file with shared lock (`LOCK_SH`), reads line by line.
5. For each line: splits on `:=` to get key, then splits value on `;` to get property=value pairs.
6. Decodes keys and values using `decchr`.
7. Decrypts values if a password is set.
8. Processes `#expire` timestamps — removes expired entries.
9. Handles container nesting for expiry logic (containers may contain expired children).
10. Caches the loaded data in `cms_cache`.

**Usage example:**
```php
$d = new data();
$d->open("settings");
$debug = $d->get(NULL, "debug"); // Get "debug" property from root
```

---

### save

Persists data to a `.dat` file. Supports deferred saving (batching) and atomic file replacement.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$name` | `string\|NULL` | `NULL` | New file name (changes save target) |
| `$force` | `bool` | `FALSE` | If `TRUE`, writes immediately; if `FALSE`, defers |

**Return:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. If `$name` is provided, resolves the file path and updates `$this->file`. Removes any existing deferral for the old file.
2. Creates the directory path if needed (`mkpath`).
3. **Deferred mode (`$force = FALSE`):** Registers this object in `self::$defer[$this->file]` (last writer wins), caches data, returns `TRUE`.
4. **Forced mode (`$force = TRUE`):** Removes deferral, writes to a temporary file with exclusive lock (`LOCK_EX`), then atomically renames it to the target file.
5. Data is serialized as: `encchr(key):=encchr(prop1)=encchr(val1);encchr(prop2)=encchr(val2)\n`
6. Values are encrypted if a password is set.
7. Empty keys and values are skipped.

**Usage example:**
```php
$d = new data("cache");
$d->set("timestamp", "last_run", time());
$d->save(); // Deferred — will be saved on destruction or defer_apply()

// Or force immediate save:
$d->save(NULL, TRUE);
```

---

### defer_apply (static)

Applies all deferred saves, optionally filtered to a specific file.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$file` | `string\|NULL` | `NULL` | Specific file to flush; if `NULL`, flushes all |

**Return:** `bool` — `TRUE` if all saves succeeded.

**Inner mechanisms:** Iterates through the `$defer` registry. For each registered object, calls `save($file, TRUE)` to force immediate write. Removes entries from the registry as they're processed.

**Usage example:**
```php
// After multiple data objects have been modified:
data::defer_apply(); // Flush all deferred writes
// Or flush a specific file:
data::defer_apply(CMS_DATA_PATH . "logs.dat");
```

---

### defer_relocate (static)

Moves cached data from one file key to another in the cache layer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$from` | `string` | Source cache key |
| `$to` | `string` | Destination cache key |

**Return:** `void`

**Inner mechanisms:** Retrieves the cached data for `$from`. If it exists, stores it under `$to` in cache. If it doesn't exist, deletes any stale data at `$to`. Then deletes the `$from` cache entry.

**Usage example:**
```php
// When renaming a data file:
data::defer_relocate("data." . CMS_DATA_PATH . "old.dat", "data." . CMS_DATA_PATH . "new.dat");
```

---

### defer_discard (static)

Removes a data object from the deferred save registry and clears its cache.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$file` | `string` | File path to discard |

**Return:** `void`

**Inner mechanisms:** Unsets the entry in `self::$defer` and calls `cms_cache_delete` to remove the cached data.

**Usage example:**
```php
// Discard changes to a temporary data file:
data::defer_discard(CMS_DATA_PATH . "temp.dat");
```

---

### set

Sets a value in the data store, with prefix removal on the key.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | `mixed` | `NULL` | Value to set (array replaces entire dataset) |
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name within the key |

**Return:** `mixed` — result of `_set`

**Inner mechanisms:** Removes the configured prefix from `$key`, then delegates to `_set`.

**Usage example:**
```php
$d = new data("config");
$d->set("dark_mode", "theme", "enabled"); // Sets $data["theme"]["dark_mode"] = "enabled"
$d->set(["a" => 1, "b" => 2], "newkey");  // Replaces entire entry for "newkey"
```

---

### _set

Internal setter that handles the actual data assignment logic.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | `mixed` | `NULL` | Value to set |
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |

**Return:** `mixed` — `TRUE` on success, or result of `_del` if value is empty.

**Inner mechanisms:**
- If `$value` is not an array and is empty (`stre`), calls `_del` to remove the entry.
- If both `$key` and `$property` are empty: replaces entire `$this->data` with `(array)$value`.
- If only `$property` is empty: sets `$this->data[$key]` to `(array)$value`.
- Otherwise: sets `$this->data[$key][$property]` to `(string)$value`.

---

### get

Retrieves a value from the data store, with prefix removal on the key.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |

**Return:** `mixed` — the value, sub-array, or entire dataset.

**Inner mechanisms:** Removes prefix from `$key`, delegates to `_get`.

**Usage example:**
```php
$d = new data("users");
$email = $d->get("user123", "email"); // Get email property of user123
$all = $d->get(); // Get entire dataset
$user = $d->get("user123"); // Get all properties of user123
```

---

### _get

Internal getter implementing the retrieval logic.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |

**Return:** `mixed` — value, sub-array, or `NULL`.

**Inner mechanisms:**
- Both empty: returns entire `$this->data`.
- `$property` empty, `$key` set: returns `$this->data[$key]` if exists.
- Both set: returns `$this->data[$key][$property]` if exists.
- Otherwise: returns `NULL`.

---

### has

Checks if a key/property exists in the data store.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |

**Return:** `bool`

**Inner mechanisms:** Removes prefix, delegates to `_has`.

**Usage example:**
```php
if ($d->has("user123", "email")) {
    echo "Email exists: " . $d->get("user123", "email");
}
```

---

### _has

Internal existence checker.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |

**Return:** `bool`

**Inner mechanisms:**
- Both empty: returns `$this->data !== NULL`.
- `$property` empty, `$key` set: returns `count($this->data[$key]) > 0`.
- Both set: returns `nstre($this->data[$key][$property])` (not empty).
- Otherwise: returns `FALSE`.

---

### del

Deletes a key or property from the data store.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |
| `$recursive` | `bool` | `TRUE` | If `TRUE`, deletes container and all children |

**Return:** `bool` — `TRUE` on success.

**Inner mechanisms:** Removes prefix, delegates to `_del`.

**Usage example:**
```php
$d->del("user123", "email"); // Delete just the email property
$d->del("user123"); // Delete entire user entry (including children if container)
```

---

### _del

Internal deletion logic with container-aware recursive deletion.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Data key |
| `$property` | `string\|NULL` | `NULL` | Property name |
| `$recursive` | `bool` | `TRUE` | Recursive deletion flag |

**Return:** `bool`

**Inner mechanisms:**
- If `$this->data` is `NULL`, returns `FALSE`.
- If `$property` is empty:
  - If `$key` is also empty: clears entire dataset.
  - If `$key` exists:
    - If `$recursive`: uses `_copy` with "del" action to remove the key and all its children.
    - If not recursive: removes just the key, then if it was a container, walks forward deleting all children until the matching `/container` marker.
- If `$property` is set: unsets just that property from the key's entry.

---

### set_buffer

Populates the internal buffer with an array of values for subsequent insert/append operations.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | `array` | Array of values to buffer |

**Return:** `bool` — `TRUE` on success, `FALSE` if not an array.

**Inner mechanisms:** Clears any existing buffer, then assigns each value a unique ID key.

**Usage example:**
```php
$d->set_buffer(["item1", "item2", "item3"]);
$d->insert("existing_key"); // Inserts buffered items after "existing_key"
```

---

### cut

Copies a key and its children to the buffer, then deletes them from the data store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Key to cut |

**Return:** `mixed` — result of `apply_prefix(_cut($key))`

**Inner mechanisms:** Removes prefix, delegates to `_cut` which calls `_copy` with "cut" action.

**Usage example:**
```php
$d->cut("section1"); // Moves section1 to buffer, removes from data
$d->insert("section2"); // Inserts cut data after section2
```

---

### _cut

Internal cut operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Key to cut |

**Return:** `bool`

**Inner mechanisms:** Calls `_copy($key, "cut")`.

---

### insert

Inserts buffered data after a specified key.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Key after which to insert |

**Return:** `mixed` — prefixed key of first inserted item, or `TRUE`/`FALSE`.

**Inner mechanisms:** Removes prefix, delegates to `_insert`, then applies prefix to the result.

**Usage example:**
```php
$d->set_buffer([["name" => "new1"], ["name" => "new2"]]);
$d->insert("item5"); // Inserts new1, new2 after item5
```

---

### _insert

Internal insert logic that rebuilds the data array with buffered items inserted at the correct position.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Key after which to insert |

**Return:** `mixed` — key of first inserted item, or `TRUE`/`FALSE`.

**Inner mechanisms:**
1. If buffer is `NULL`, returns `FALSE`.
2. If `$key` is empty, defaults to `0`.
3. Iterates through data using cursor navigation, building a new array.
4. When the target key is found, inserts all buffered items.
5. Replaces `$this->data` with the new array.
6. Returns the key of the first inserted item (or `TRUE` if none).

---

### append

Appends buffered data to the end of a container or after a specified key.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Key after which to append |

**Return:** `mixed` — result of `apply_prefix(_append($key))`

**Inner mechanisms:** Removes prefix, delegates to `_append` which calls `_copy` with "append" action.

---

### _append

Internal append operation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string\|NULL` | `NULL` | Key after which to append |

**Return:** `bool`

**Inner mechanisms:** Calls `_copy($key, "append")`.

---

### copy

Copies a key and its children to the buffer without deleting from the data store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Key to copy |
| `$action` | `string\|NULL` | Action type: `"cut"`, `"del"`, `"append"`, or `NULL` (copy) |

**Return:** `mixed` — result of `apply_prefix(_copy($key, $action))`

**Inner mechanisms:** Removes prefix, delegates to `_copy`.

**Usage example:**
```php
$d->copy("template"); // Copies template to buffer
$d->insert("new_section"); // Inserts copy after new_section
```

---

### _copy

Core copy/cut/delete/append engine that traverses container hierarchies.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | `string` | — | Starting key |
| `$action` | `string\|NULL` | `NULL` | Action: `"cut"`, `"del"`, `"append"`, or `NULL` (copy) |

**Return:** `bool` — `TRUE` on success.

**Inner mechanisms:**
1. If action is `"append"` and buffer is `NULL`, returns `FALSE`. If key is empty, delegates to `_insert(0)`.
2. Clears buffer unless action is `"append"` or `"del"`.
3. If key doesn't exist, returns `FALSE`.
4. Positions cursor at the key using `_move("to", $key)`.
5. Iterates through data, tracking container depth:
   - `"container"` increments depth, `"/container"` decrements.
6. For each entry:
   - **Default (copy):** Adds to buffer with unique ID.
   - **Cut:** Adds to buffer with original key, then deletes from data.
   - **Del:** Deletes from data.
   - **Append:** Adds to buffer with original key.
7. Continues until depth returns to 0 (all children processed).
8. If action is `"append"`, calls `_insert` to place buffered items.

---

### seek

Searches for a key matching all specified property conditions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$condition` | `array` | Associative array of property => value pairs to match |

**Return:** `mixed` — prefixed key of first match, or `FALSE`.

**Inner mechanisms:** Removes prefix from all condition values, delegates to `_seek`, then applies prefix.

**Usage example:**
```php
$key = $d->seek(["#type" => "user", "status" => "active"]);
if ($key !== FALSE) {
    echo "Found active user at key: " . $key;
}
```

---

### _seek

Internal search implementation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$condition` | `array` | Property => value pairs to match |

**Return:** `string\|FALSE` — key of first matching entry, or `FALSE`.

**Inner mechanisms:**
1. Moves cursor to first entry.
2. Iterates through all entries using `_move("next")`.
3. For each entry, checks all conditions: if any property doesn't match, skips.
4. Returns the first key where all conditions match.
5. Returns `FALSE` if no match found.

---

### move

Moves the internal cursor to a specified position.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$target` | `string` | `"current"` | Target position: `"current"`, `"first"`, `"last"`, `"prev"`, `"next"`, `"to"`, `"parent"` |
| `$key` | `string\|NULL` | `NULL` | Key for `"to"` and `"parent"` targets |

**Return:** `mixed` — prefixed key at new cursor position, or `NULL`.

**Inner mechanisms:** Removes prefix from `$key`, delegates to `_move`, then applies prefix.

**Usage example:**
```php
$d->move("first"); // Move to first entry
$key = $d->move("next"); // Move to next entry
$d->move("to", "target_key"); // Move to specific key
$parent = $d->move("parent", "child_key"); // Find parent of child_key
```

---

### _move

Internal cursor navigation engine.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$target` | `string` | `"current"` | Navigation target |
| `$key` | `string\|NULL` | `NULL` | Key for `"to"` and `"parent"` |

**Return:** `string\|NULL` — key at new position, or `NULL`.

**Inner mechanisms:**
- **`"current"`:** Returns current key without moving.
- **`"first"`:** Resets to first element.
- **`"last"`:** Moves to last element.
- **`"prev"`:** Returns current key, then moves backward.
- **`"next"`:** Returns current key, then moves forward.
- **`"to"`:** Searches forward for the specified key.
- **`"parent"`:** Tracks container nesting depth to find the parent container of the specified key.

---

### is_container

Checks if a key represents a container entry.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Key to check |

**Return:** `bool`

**Inner mechanisms:** Removes prefix, delegates to `_is_container`.

---

### _is_container

Internal container check.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Key to check |

**Return:** `bool`

**Inner mechanisms:** Returns `TRUE` if key is empty (root is always a container) or if `#type` property equals `"container"`.

---

### is_child

Checks if a key is a descendant of a parent key (within container hierarchy).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Child key to check |
| `$parent` | `string` | Parent key to check against |

**Return:** `bool`

**Inner mechanisms:** Removes prefixes, delegates to `_is_child`.

---

### _is_child

Internal child relationship check using depth tracking.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Child key |
| `$parent` | `string` | Parent key |

**Return:** `bool`

**Inner mechanisms:**
1. Returns `FALSE` if key is empty.
2. Returns `TRUE` if key equals parent.
3. Returns `FALSE` if parent is not a container.
4. Iterates through data tracking container depth:
   - When parent is found, starts tracking depth.
   - `"container"` increments depth, `"/container"` decrements.
   - Returns `TRUE` when child key is found at depth > 0.
   - Returns `FALSE` when depth returns to 0 without finding child.

---

### has_children

Checks if a container key has any child entries.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Container key to check |

**Return:** `bool`

**Inner mechanisms:** Removes prefix, delegates to `_has_children`.

---

### _has_children

Internal children check.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Container key |

**Return:** `bool`

**Inner mechanisms:**
1. Returns `FALSE` if key is empty.
2. Returns `FALSE` if key is not a container.
3. Moves cursor to the key, then to the next entry.
4. Returns `TRUE` if the next entry's `#type` is not `"/container"` (i.e., there are children).

---

### set_password

Sets the encryption password for the data object.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | `string\|NULL` | `NULL` | Password string |

**Return:** `void`

**Inner mechanisms:** If value is non-empty, stores `hash64($value)` (a hash of the password). If empty, sets to `NULL` (disables encryption).

**Usage example:**
```php
$d = new data("secure_data");
$d->set_password("my_secret");
$d->set("api_key", "config", "value"); // Will be encrypted on save
```

---

### set_prefix

Configures the key prefix for virtual filesystem mapping.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | `string\|NULL` | `NULL` | Prefix string |

**Return:** `void`

**Inner mechanisms:**
1. If `$value` is non-empty, sets `$this->prefix` directly.
2. If `$value` is empty and `$this->file` matches a known system file, sets a predefined prefix:
   - `#system/image.dat` → `"image://"`
   - `#system/media.dat` → `"media://"`
   - `#system/download.dat` → `"download://"`
3. Updates `$this->prefix_length` for performance.

**Usage example:**
```php
$d = new data("image");
// Automatically gets prefix "image://" if file is #system/image.dat
$d->set("logo", "meta", "logo.png");
$key = $d->get("logo", "meta"); // Returns "image://logo"
```

---

### apply_prefix

Prepends the configured prefix to a key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string\|mixed` | Key to prefix |

**Return:** `string\|mixed` — prefixed key, or original value if not a string.

**Inner mechanisms:** If `$key` is a string, returns `$this->prefix . $key`. Otherwise returns `$key` unchanged.

---

### remove_prefix

Strips the configured prefix from a key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string\|mixed` | Key to process |

**Return:** `string\|mixed` — key without prefix, or original value if not a string.

**Inner mechanisms:** If prefix length is 0, returns key unchanged. If key starts with the prefix, returns the substring after the prefix. Otherwise returns key unchanged.

---

## Typical Usage Patterns

### Basic CRUD Operations
```php
$d = new data("users");
$d->set("john@example.com", "user_001", "email");
$d->set("active", "user_001", "status");

$email = $d->get("user_001", "email");
$d->del("user_001", "status");
$d->save();
```

### Container Hierarchy
```php
$d = new data("menu");
$d->set("container", "section1", "#type");
$d->set("item1", "section1", "#type");
$d->set("value1", "item1", "label");
$d->set("/container", "section1_end", "#type");
$d->save();
```

### Encrypted Data
```php
$d = new data("secrets", "password123");
$d->set("api_key_12345", "service", "key");
$d->save();
// On reload, values are automatically decrypted
```

### Deferred Saving
```php
$d1 = new data("cache1");
$d1->set("value", "key", "prop");
$d1->save(); // Deferred

$d2 = new data("cache2");
$d2->set("value", "key", "prop");
$d2->save(); // Deferred

data::defer_apply(); // Flush all at once
```

### Buffer Operations (Cut/Copy/Paste)
```php
$d = new data("content");
$d->copy("template_section"); // Copy to buffer
$d->insert("target_section"); // Insert copy after target

$d->cut("old_section"); // Cut to buffer
$d->append("new_parent"); // Append cut data to new parent
```


<!-- HASH:2edb0eb419270964a34d8807d0d5cbf4 -->
