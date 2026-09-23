# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content_pool.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content_pool.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## content_pool

The `content_pool` class provides a mechanism for managing reusable content fragments extracted from database-stored documents. It allows operators to define named pools of content that reference specific ranges within content documents, enabling efficient retrieval and caching of frequently accessed content pieces without repeated database queries.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_POOL_PERMISSION_OPERATOR` | `"pool.operator"` | Permission required to perform write operations on the content pool |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Data handler instance for the `#system/content.pool` storage |
| `$operator` | `boolean` | Whether the current user has operator permissions |

### Methods

#### `__construct()`

Initializes the content pool by setting up the data handler, checking operator permissions, creating the content text directory, and establishing a database connection.

**Parameters:** None

**Return Value:** None

**Usage Example:**
```php
$pool = new content_pool();
// Ready to use for content pool operations
```

#### `add($name, $category, $content_index, $range, $type)`

Creates a new content pool entry that references a specific range within a content document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Display name for the pool entry |
| `$category` | `string` | Category grouping for the pool entry |
| `$content_index` | `string` | Index of the source content document |
| `$range` | `mixed` | Range specification for content extraction |
| `$type` | `mixed` | Type specification for content extraction |

**Return Value:** `string|boolean` - The new pool index on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
$newIndex = $pool->add(
    "Homepage Hero",
    "Marketing",
    "homepage_content",
    "hero_section",
    "html"
);
if ($newIndex !== FALSE) {
    echo "Created pool entry: " . $newIndex;
}
```

#### `set($index, $name, $category, $content_index, $range, $type)`

Updates an existing content pool entry with new values. Only provided parameters are updated.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Pool entry identifier to update |
| `$name` | `string\|NULL` | New display name (optional) |
| `$category` | `string\|NULL` | New category (optional) |
| `$content_index` | `string\|NULL` | New content document index (optional) |
| `$range` | `mixed\|NULL` | New range specification (optional) |
| `$type` | `mixed\|NULL` | New type specification (optional) |

**Return Value:** `boolean` - `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
$success = $pool->set(
    "pool_abc123",
    "Updated Hero Section",
    NULL,
    NULL,
    "hero_banner",
    NULL
);
if ($success) {
    echo "Pool entry updated successfully";
}
```

#### `get($index)`

Retrieves all metadata for a specific pool entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Pool entry identifier |

**Return Value:** `array\|NULL` - Pool entry data array or `NULL` if not found

**Usage Example:**
```php
$pool = new content_pool();
$entry = $pool->get("pool_abc123");
if ($entry !== NULL) {
    echo "Name: " . $entry["name"];
    echo "Category: " . $entry["category"];
}
```

#### `get_text($index)`

Retrieves the actual content text for a pool entry, using caching to avoid repeated file reads.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Pool entry identifier |

**Return Value:** `string\|NULL` - The content text or `NULL` if not available

**Usage Example:**
```php
$pool = new content_pool();
$content = $pool->get_text("pool_abc123");
if ($content !== NULL) {
    echo $content; // Outputs the extracted content
}
```

#### `delete($index)`

Removes a content pool entry and its associated content file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Pool entry identifier to delete |

**Return Value:** `boolean` - `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->delete("pool_abc123")) {
    echo "Pool entry deleted successfully";
}
```

#### `synchronize($index)`

Synchronizes a single pool entry by extracting content from its referenced document and writing it to a file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Pool entry identifier to synchronize |

**Return Value:** `boolean` - `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
if ($pool->synchronize("pool_abc123")) {
    echo "Pool entry synchronized with source content";
}
```

#### `synchronize_content($content_index)`

Synchronizes all pool entries that reference a specific content document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `string` | Content document index to synchronize references for |

**Return Value:** `boolean` - `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
// After updating a content document, sync all related pool entries
if ($pool->synchronize_content("homepage_content")) {
    echo "All related pool entries updated";
}
```

#### `synchronize_all()`

Synchronizes all pool entries by batch-processing content documents.

**Parameters:** None

**Return Value:** `boolean` - `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$pool = new content_pool();
// Perform full synchronization of all pool entries
if ($pool->synchronize_all()) {
    echo "All pool entries synchronized";
}
```

## Helper Functions

### `content_pool_get_array($type = NULL)`

Generates a hierarchical array of all content pool entries organized by category, optionally filtered by type.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$type` | `string\|NULL` | `NULL` | Filter by content type |

**Return Value:** `array` - Nested array with categories as keys and pool entries as values

**Usage Example:**
```php
$poolArray = content_pool_get_array("html");
// Returns array like:
// [
//   "Marketing" => ["Homepage Hero" => "pool_abc123"],
//   "Blog" => ["Article Sidebar" => "pool_def456"]
// ]
```

### `content_pool_get_select()`

Generates a flat array of all content pool categories suitable for select dropdowns.

**Parameters:** None

**Return Value:** `array` - Array with category names as both keys and values

**Usage Example:**
```php
$categories = content_pool_get_select();
// Returns array like:
// ["" => "", "Marketing" => "Marketing", "Blog" => "Blog"]
```


<!-- HASH:f60766396da0f674e20a4e7962fba177 -->
