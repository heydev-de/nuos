# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content_pool.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content_pool.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Pool Module

The `content_pool` module provides a system for managing reusable content fragments extracted from larger documents. It allows storing, retrieving, and synchronizing content snippets (called "pools") that reference specific ranges within full documents. This enables efficient content reuse without duplication.

### Related Functions

#### `content_pool_get_array()`
Generates a categorized array of all available content pools for selection interfaces.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `$type`   | string | Optional. If provided, only pools of this type will be included in results. |

**Return Value:**
- `array`: A nested associative array where keys are categories and values are sub-arrays of pool names/keys.

**Mechanism:**
1. Loads pool data from storage
2. Iterates through all pools
3. Skips pools that don't match the specified type (if provided)
4. Organizes pools by category
5. Handles naming collisions by appending incrementing numbers
6. Sorts results naturally and case-insensitively

**Usage Example:**
```php
$pools = content_pool_get_array();
foreach ($pools as $category => $items) {
    echo "<optgroup label='" . x($category) . "'>";
    foreach ($items as $name => $key) {
        echo "<option value='" . x($key) . "'>" . x($name) . "</option>";
    }
    echo "</optgroup>";
}
```

#### `content_pool_get_select()`
Generates a flat array of content pool categories for selection interfaces.

**Return Value:**
- `array`: An associative array where keys and values are category names.

**Mechanism:**
1. Loads pool data from storage
2. Iterates through all pools
3. Collects unique categories
4. Sorts results naturally and case-insensitively

**Usage Example:**
```php
$categories = content_pool_get_select();
echo "<select name='category'>";
foreach ($categories as $value) {
    echo "<option value='" . x($value) . "'>" . x($value) . "</option>";
}
echo "</select>";
```

## Content_Pool Class

Manages the storage, retrieval, and synchronization of content pools.

### Properties

| Name       | Type   | Description                                  |
|------------|--------|----------------------------------------------|
| `$data`    | object | Data handler for pool storage                |
| `$operator`| bool   | Permission flag for operator privileges      |

### Constructor

**`__construct()`**
Initializes the content pool system.

**Mechanism:**
1. Creates data handler for pool storage
2. Checks operator permissions
3. Creates directory structure for content storage
4. Establishes database connection

### Methods

#### `add()`
Creates a new content pool entry.

| Parameter        | Type   | Description                                      |
|------------------|--------|--------------------------------------------------|
| `$name`          | string | Display name for the pool                        |
| `$category`      | string | Category for organizational purposes            |
| `$content_index` | string | Index of the source content document            |
| `$range`         | string | Document range identifier to extract            |
| `$type`          | string | Content type (e.g., "html", "text")             |

**Return Value:**
- `string|bool`: The new pool index on success, FALSE on failure.

**Mechanism:**
1. Validates operator permissions
2. Sanitizes input values
3. Sets default name if empty
4. Creates new pool entry
5. Synchronizes content
6. Saves changes

**Usage Example:**
```php
$pool = new content_pool();
$new_index = $pool->add(
    "Featured Products",
    "Homepage",
    "products_page",
    "featured_section",
    "html"
);
if ($new_index !== FALSE) {
    echo "New pool created with index: " . x($new_index);
}
```

#### `set()`
Updates an existing content pool entry.

| Parameter        | Type   | Description                                      |
|------------------|--------|--------------------------------------------------|
| `$index`         | string | Pool index to update                             |
| `$name`          | string | Optional. New display name                       |
| `$category`      | string | Optional. New category                           |
| `$content_index` | string | Optional. New source content index               |
| `$range`         | string | Optional. New range identifier                   |
| `$type`          | string | Optional. New content type                       |

**Return Value:**
- `bool`: TRUE on success, FALSE on failure.

**Mechanism:**
1. Validates operator permissions
2. Updates specified fields if provided
3. Tracks changes to content references
4. Resynchronizes content if references changed
5. Saves changes

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->set(
    "featured_products",
    "Featured Items",  // new name
    NULL,              // keep category
    "updated_products" // new content index
)) {
    echo "Pool updated successfully";
}
```

#### `get()`
Retrieves all data for a specific pool.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$index`  | string | Pool index to retrieve |

**Return Value:**
- `array`: Associative array of pool data, or NULL if not found.

**Usage Example:**
```php
$pool = new content_pool();
$pool_data = $pool->get("featured_products");
if ($pool_data) {
    echo "Pool Name: " . x($pool_data['name']);
    echo "Source: " . x($pool_data['index']);
}
```

#### `get_text()`
Retrieves the actual content text for a pool.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$index`  | string | Pool index to retrieve |

**Return Value:**
- `string`: The content text, or NULL if not found.

**Mechanism:**
1. Checks temporary cache first
2. Falls back to file storage if not cached
3. Caches retrieved content for future use

**Usage Example:**
```php
$pool = new content_pool();
$content = $pool->get_text("featured_products");
if ($content !== NULL) {
    echo $content; // Output the HTML/text content
}
```

#### `delete()`
Removes a content pool entry.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$index`  | string | Pool index to delete |

**Return Value:**
- `bool`: TRUE on success, FALSE on failure.

**Mechanism:**
1. Validates operator permissions
2. Removes content file if exists
3. Clears temporary cache
4. Removes pool data entry
5. Saves changes

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->delete("old_featured_products")) {
    echo "Pool deleted successfully";
}
```

#### `synchronize()`
Updates a pool's content file based on its source document.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$index`  | string | Pool index to synchronize |

**Return Value:**
- `bool`: TRUE on success, FALSE on failure.

**Mechanism:**
1. Loads required document processing library
2. Retrieves source content from database
3. Processes document to extract specified range
4. Writes extracted content to file
5. Clears temporary cache

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->synchronize("featured_products")) {
    echo "Pool content updated from source";
}
```

#### `synchronize_content()`
Updates all pools that reference a specific content document.

| Parameter        | Type   | Description                          |
|------------------|--------|--------------------------------------|
| `$content_index` | string | Content index to synchronize against |

**Return Value:**
- `bool`: TRUE on success, FALSE on failure.

**Mechanism:**
1. Finds all pools referencing the content
2. Retrieves the source content once
3. Updates all referencing pools
4. Clears temporary caches

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->synchronize_content("products_page")) {
    echo "All pools referencing 'products_page' updated";
}
```

#### `synchronize_all()`
Updates all content pools in the system.

**Return Value:**
- `bool`: TRUE on success, FALSE on failure.

**Mechanism:**
1. Groups pools by their source content
2. Retrieves all source contents in a single query
3. Updates all pools efficiently
4. Clears temporary caches

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->synchronize_all()) {
    echo "All content pools synchronized";
}
```


<!-- HASH:f60766396da0f674e20a4e7962fba177 -->
