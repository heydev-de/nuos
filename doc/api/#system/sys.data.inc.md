# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.data.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.data.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Data Class and Related Functions

The `data` class in the PWNC Web Platform provides a structured way to manage hierarchical data stored in `.dat` files. It supports CRUD operations, hierarchical navigation, sorting, and prefix-based key management. The class is optimized for performance with file-based locking, buffering, and caching mechanisms.

### Key Features
- **Hierarchical Data Management**: Supports nested containers with parent-child relationships.
- **File-Based Storage**: Data is persisted in `.dat` files with atomic write operations.
- **Prefix Handling**: Keys can be prefixed (e.g., `image://`, `media://`) for logical grouping.
- **Temporary Caching**: Uses `cms_cache()` to avoid repeated file reads.
- **Encryption**: Optional password-based encryption for sensitive data.

---

## Functions

### `data_sort(&$data, $property, $key = NULL)`
Sorts hierarchical data in-place by a specified property.

#### Parameters
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$data`    | `data`   | **Reference** to a `data` object.                                           |
| `$property`| `string` | Property name to sort by (e.g., `"name"`).                                 |
| `$key`     | `string` | Optional starting key for partial sorting. Defaults to root (`0`).         |

#### Return Values
- **`void`**: Modifies the `$data` object in-place.

#### Inner Mechanisms
1. **Tree Construction**: Builds a hierarchical array from the `data` object, tracking container relationships.
2. **Sorting**: Uses `uasort()` with `_data_sort()` to sort nodes recursively.
3. **Reconstruction**: Rebuilds the `data` object from the sorted tree.

#### Usage Example
```php
$data = new data("products");
data_sort($data, "price"); // Sorts all products by price
```

---

### `_data_sort($value1, $value2)`
Helper function for `data_sort()` to compare two nodes by their sort property.

#### Parameters
| Name      | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | `array`| First node (with `"#!"` key).   |
| `$value2` | `array`| Second node (with `"#!"` key).  |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `int`    | `-1` if `$value1` < `$value2`, `1` if `$value1` > `$value2`, `0` if equal.  |

---

## Class: `data`

### Properties
| Name              | Default | Description                                                                 |
|-------------------|---------|-----------------------------------------------------------------------------|
| `$data`           | `[]`    | In-memory data store.                                                       |
| `$file`           | `NULL`  | Path to the `.dat` file.                                                    |
| `$buffer`         | `NULL`  | Temporary buffer for cut/copy/paste operations.                             |
| `$password`       | `NULL`  | Hashed password for encryption.                                             |
| `$prefix`         | `NULL`  | Key prefix (e.g., `image://`).                                              |
| `$prefix_length`  | `0`     | Length of the prefix (for removal).                                         |

---

### Constructor: `__construct($name = NULL, $password = NULL, $prefix = NULL)`
Initializes a `data` object and optionally opens a file.

#### Parameters
| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | File name (without `.dat` extension) or full path.                         |
| `$password` | `string` | Password for encryption.                                                    |
| `$prefix`   | `string` | Key prefix (overrides default for specific files).                         |

#### Usage Example
```php
$data = new data("settings", "secret123", "config://");
```

---

### `open($name, $password = NULL, $prefix = NULL)`
Loads data from a `.dat` file into memory.

#### Parameters
| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | File name or path.                                                          |
| `$password` | `string` | Password for decryption.                                                    |
| `$prefix`   | `string` | Key prefix.                                                                 |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` on success, `FALSE` otherwise. |

#### Inner Mechanisms
1. **Caching**: Checks `cms_cache()` for a cached version of the file.
2. **File Reading**: Parses the file line-by-line, decrypting values if a password is set.
3. **Expiration**: Skips expired entries (based on `#expire` property).

#### Usage Example
```php
$data->open("users", "password123");
```

---

### `save($name = NULL)`
Saves in-memory data to a `.dat` file.

#### Parameters
| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$name` | `string` | Optional new file name/path.    |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` on success, `FALSE` otherwise. |

#### Inner Mechanisms
1. **Atomic Writes**: Uses a temporary file to avoid corruption.
2. **Encryption**: Encrypts values if a password is set.
3. **Caching**: Updates `cms_cache()` with the new data.

#### Usage Example
```php
$data->set("admin", "role", "superuser");
$data->save();
```

---

### `set($value = NULL, $key = NULL, $property = NULL)`
Sets a value in the data store.

#### Parameters
| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$value`    | `mixed`  | Value to set. If `NULL` or empty, deletes the key/property.                 |
| `$key`      | `string` | Key (prefix is automatically removed).                                     |
| `$property` | `string` | Property name. If `NULL`, sets the entire key or data store.                |

#### Return Values
- **`void`**: Modifies the data store in-place.

#### Usage Example
```php
$data->set("John Doe", "user1", "name"); // Sets user1's name
$data->set(["name" => "Jane"], "user2"); // Sets user2's data
```

---

### `get($key = NULL, $property = NULL)`
Retrieves a value from the data store.

#### Parameters
| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$key`      | `string` | Key (prefix is automatically removed).                                     |
| `$property` | `string` | Property name. If `NULL`, returns the entire key or data store.             |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `mixed`  | The requested value, or `NULL` if not found.                                |

#### Usage Example
```php
$name = $data->get("user1", "name"); // Returns "John Doe"
$allUsers = $data->get(); // Returns all data
```

---

### `has($key = NULL, $property = NULL)`
Checks if a key or property exists.

#### Parameters
| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$key`      | `string` | Key (prefix is automatically removed).                                     |
| `$property` | `string` | Property name. If `NULL`, checks if the key exists.                         |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` if exists, `FALSE` otherwise. |

#### Usage Example
```php
if ($data->has("user1", "name")) { /* ... */ }
```

---

### `del($key = NULL, $property = NULL, $recursive = TRUE)`
Deletes a key or property.

#### Parameters
| Name         | Type     | Description                                                                 |
|--------------|----------|-----------------------------------------------------------------------------|
| `$key`       | `string` | Key (prefix is automatically removed).                                     |
| `$property`  | `string` | Property name. If `NULL`, deletes the entire key.                           |
| `$recursive` | `bool`   | If `TRUE`, deletes all children of a container.                             |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` on success, `FALSE` otherwise. |

#### Usage Example
```php
$data->del("user1", "name"); // Deletes user1's name
$data->del("user1"); // Deletes user1 and all properties
```

---

### `cut($key)`
Cuts a key (and its children) into the buffer.

#### Parameters
| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$key`| `string` | Key (prefix is automatically removed). |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` on success, `FALSE` otherwise. |

#### Usage Example
```php
$data->cut("category1"); // Moves category1 to buffer
```

---

### `insert($key = NULL)`
Inserts buffered data at a specified key.

#### Parameters
| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$key`| `string` | Insertion point (prefix is automatically removed). Defaults to root. |

#### Return Values
| Type     | Description                     |
|----------|---------------------------------|
| `string` | Key of the first inserted item, or `TRUE` on success. |

#### Usage Example
```php
$data->insert("parent_category"); // Inserts buffered data under parent_category
```

---

### `move($target = "current", $key = NULL)`
Moves the internal pointer to a specific position.

#### Parameters
| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$target`| `string` | Target position: `"current"`, `"first"`, `"last"`, `"prev"`, `"next"`, `"to"`, `"parent"`. |
| `$key`   | `string` | Key for `"to"` or `"parent"` targets.                                      |

#### Return Values
| Type     | Description                     |
|----------|---------------------------------|
| `string` | Key at the new position, or `NULL` if invalid. |

#### Usage Example
```php
$firstKey = $data->move("first"); // Moves to first key
$parentKey = $data->move("parent", "child1"); // Moves to parent of child1
```

---

### `is_container($key)`
Checks if a key is a container.

#### Parameters
| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$key`| `string` | Key (prefix is automatically removed). |

#### Return Values
| Type    | Description                     |
|---------|---------------------------------|
| `bool`  | `TRUE` if the key is a container, `FALSE` otherwise. |

#### Usage Example
```php
if ($data->is_container("category1")) { /* ... */ }
```

---

### `set_password($value = NULL)`
Sets the encryption password.

#### Parameters
| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Password (hashed internally).   |

#### Usage Example
```php
$data->set_password("secure123");
```

---

### `set_prefix($value = NULL)`
Sets the key prefix.

#### Parameters
| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Prefix (e.g., `image://`). If `NULL`, uses a default based on the file.     |

#### Usage Example
```php
$data->set_prefix("media://");
```

---

### `apply_prefix($key)`
Applies the prefix to a key.

#### Parameters
| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$key`| `string` | Key to prefix.                  |

#### Return Values
| Type     | Description                     |
|----------|---------------------------------|
| `string` | Prefixed key.                   |

#### Usage Example
```php
$prefixedKey = $data->apply_prefix("image1"); // Returns "image://image1"
```

---

### `remove_prefix($key)`
Removes the prefix from a key.

#### Parameters
| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$key`| `string` | Key to unprefix.                |

#### Return Values
| Type     | Description                     |
|----------|---------------------------------|
| `string` | Unprefixed key.                 |

#### Usage Example
```php
$unprefixedKey = $data->remove_prefix("image://image1"); // Returns "image1"
```


<!-- HASH:0388ff2a51bc44846ff93a2410f9026f -->
