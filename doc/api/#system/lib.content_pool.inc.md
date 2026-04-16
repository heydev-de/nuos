# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.content_pool.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Content Pool Module (`lib.content_pool.inc`)

The **Content Pool** module provides a centralized system for managing reusable content fragments extracted from NUOS documents. It allows storing, retrieving, and synchronizing content snippets (e.g., text blocks, HTML, or structured data) referenced by document ranges and types. This module is essential for content reuse, dynamic updates, and performance optimization.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_POOL_PERMISSION_OPERATOR` | `"pool.operator"` | Permission identifier required to modify content pool entries. |

---

## Functions

### `content_pool_get_array`

**Purpose:**
Generates a categorized, human-readable associative array of all content pool entries, optionally filtered by type. Used for UI selection lists (e.g., dropdowns).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `string` \| `NULL` | Optional. If provided, only entries of this type are included. If `NULL`, all entries are returned. |

**Return Values:**
- `array`: A nested associative array structured as:
  ```php
  [
      "Category Name" => [
          "Display Name" => "content_index",
          ...
      ],
      ...
  ]
  ```
  - Empty string (`""`) as a key represents uncategorized entries.
  - Display names are deduplicated (e.g., `"Name (1)"`, `"Name (2)"`).

**Inner Mechanisms:**
1. Loads content pool data using the `data` class.
2. Iterates through all entries, skipping those that do not match the provided `$type`.
3. Groups entries by category, using empty string for uncategorized items.
4. Generates display names from the `name` field or falls back to the entry key.
5. Ensures unique display names by appending incrementing numbers if duplicates exist.
6. Sorts categories and entries alphabetically.

**Usage Context:**
- Used in administrative interfaces to populate selection lists (e.g., content pool picker).
- Example:
  ```php
  $pool_array = content_pool_get_array("html");
  // Renders a dropdown of all HTML content pool entries.
  ```

---

### `content_pool_get_select`

**Purpose:**
Generates a flat associative array of all unique categories in the content pool. Used for UI category filters.

**Parameters:**
None.

**Return Values:**
- `array`: An associative array where keys and values are category names, sorted alphabetically.
  ```php
  [
      "" => "",
      "Category 1" => "Category 1",
      "Category 2" => "Category 2",
      ...
  ]
  ```

**Inner Mechanisms:**
1. Loads content pool data using the `data` class.
2. Iterates through all entries, collecting unique category names.
3. Sorts the resulting array alphabetically.

**Usage Context:**
- Used in administrative interfaces to populate category filters (e.g., dropdowns).
- Example:
  ```php
  $categories = content_pool_get_select();
  // Renders a dropdown of all available categories.
  ```

---

## Class: `content_pool`

Manages the lifecycle of content pool entries, including creation, modification, deletion, and synchronization with source documents.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Instance of the `data` class for managing content pool metadata. |
| `$operator` | `bool` | Indicates whether the current user has operator permissions (`CMS_CONTENT_POOL_PERMISSION_OPERATOR`). |
| `$mysql` | `mysql` \| `NULL` | Database connection instance for content synchronization. Lazy-loaded. |

---

### `__construct`

**Purpose:**
Initializes the `content_pool` instance, loads metadata, checks permissions, and ensures the storage directory exists.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Initializes the `data` instance for `#system/content.pool`.
2. Checks operator permissions using `cms_permission()`.
3. Creates the storage directory (`#content/pool`) if it does not exist.

**Usage Context:**
- Instantiated when content pool operations are required.
- Example:
  ```php
  $pool = new content_pool();
  ```

---

### `add`

**Purpose:**
Adds a new content pool entry and synchronizes it with the source document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Display name of the entry. |
| `$category` | `string` | Category of the entry. |
| `$content_index` | `string` | Index of the source document in the content table. |
| `$range` | `string` | Document range identifier (e.g., `"header"`, `"body.section1"`). |
| `$type` | `string` | Content type (e.g., `"html"`, `"text"`, `"json"`). |

**Return Values:**
- `string` \| `FALSE`: The index of the newly created entry on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates operator permissions.
2. Trims and sanitizes `$name` and `$category`.
3. Falls back to a default name (`CMS_L_CONTENT_POOL_001`) if `$name` is empty.
4. Buffers the new entry data and attempts insertion.
5. Synchronizes the entry with the source document using `synchronize()`.
6. Saves the metadata if synchronization succeeds.

**Usage Context:**
- Used in administrative interfaces to create new content pool entries.
- Example:
  ```php
  $index = $pool->add("Footer", "Layout", "page_home", "footer", "html");
  ```

---

### `set`

**Purpose:**
Updates an existing content pool entry and resynchronizes it if the reference (content index, range, or type) changes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the content pool entry to update. |
| `$name` | `string` | New display name. |
| `$category` | `string` | New category. |
| `$content_index` | `string` | New source document index. |
| `$range` | `string` | New document range. |
| `$type` | `string` | New content type. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates operator permissions.
2. Trims and sanitizes `$name` and `$category`.
3. Falls back to a default name if `$name` is empty.
4. Backs up current reference values (`index`, `range`, `type`).
5. Updates the entry with new values.
6. Resynchronizes the entry if any reference value has changed.
7. Saves the metadata.

**Usage Context:**
- Used in administrative interfaces to edit content pool entries.
- Example:
  ```php
  $success = $pool->set("footer_1", "Updated Footer", "Layout", "page_home", "footer", "html");
  ```

---

### `get`

**Purpose:**
Retrieves metadata for a content pool entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the content pool entry. |

**Return Values:**
- `array` \| `NULL`: Associative array of entry metadata (e.g., `name`, `category`, `index`, `range`, `type`), or `NULL` if the entry does not exist.

**Inner Mechanisms:**
- Delegates to the `data` class's `get()` method.

**Usage Context:**
- Used to display or process entry metadata.
- Example:
  ```php
  $metadata = $pool->get("footer_1");
  ```

---

### `get_text`

**Purpose:**
Retrieves the actual content of a content pool entry, with caching for performance.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the content pool entry. |

**Return Values:**
- `string` \| `NULL`: The content of the entry, or `NULL` if the entry does not exist or has no content.

**Inner Mechanisms:**
1. Checks the temporary cache (`cms_cache`) for the content.
2. If not cached, reads the content from the file system (`#content/pool/$index`).
3. Caches the content for future requests.

**Usage Context:**
- Used to render content pool entries in frontend or backend contexts.
- Example:
  ```php
  $content = $pool->get_text("footer_1");
  echo $content; // Outputs the HTML/text content.
  ```

---

### `delete`

**Purpose:**
Deletes a content pool entry and its associated content file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the content pool entry to delete. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates operator permissions.
2. Deletes the associated content file if it exists.
3. Removes the entry from the temporary cache.
4. Deletes the entry from the metadata store and saves changes.

**Usage Context:**
- Used in administrative interfaces to remove content pool entries.
- Example:
  ```php
  $success = $pool->delete("footer_1");
  ```

---

### `synchronize`

**Purpose:**
Synchronizes a content pool entry with its source document by extracting the specified range and type, then writing the result to the content file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Index of the content pool entry to synchronize. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Loads the `document` library.
2. Establishes a database connection if not already present.
3. Retrieves the source document from the database using the entry's `index`.
4. Processes the document to extract the specified range and type.
5. Writes the extracted content to the entry's file.
6. Clears the entry from the temporary cache.

**Usage Context:**
- Called automatically during `add` and `set` operations if reference values change.
- Can be called manually to force synchronization.
- Example:
  ```php
  $success = $pool->synchronize("footer_1");
  ```

---

### `synchronize_content`

**Purpose:**
Synchronizes all content pool entries that reference a specific source document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `string` | Index of the source document to synchronize. |

**Return Values:**
- `void`: No explicit return value. Errors are silently ignored.

**Inner Mechanisms:**
1. Loads the `document` library.
2. Generates a list of all content pool entries referencing the specified `$content_index`.
3. Retrieves the source document from the database.
4. Processes each entry to extract the specified range and type.
5. Writes the extracted content to each entry's file.
6. Clears each entry from the temporary cache.

**Usage Context:**
- Called when a source document is updated to ensure all dependent content pool entries are synchronized.
- Example:
  ```php
  $pool->synchronize_content("page_home");
  ```

---

### `synchronize_all`

**Purpose:**
Synchronizes all content pool entries with their respective source documents.

**Parameters:**
None.

**Return Values:**
- `void`: No explicit return value. Errors are silently ignored.

**Inner Mechanisms:**
1. Loads the `document` library.
2. Generates a list of all unique source document indices referenced by content pool entries.
3. Retrieves all referenced documents from the database in a single query.
4. Processes each document to extract the specified range and type for all dependent entries.
5. Writes the extracted content to each entry's file.
6. Clears each entry from the temporary cache.

**Usage Context:**
- Used for bulk synchronization, e.g., during system maintenance or initial setup.
- Example:
  ```php
  $pool->synchronize_all();
  ```


<!-- HASH:0041274b438356dfafe3766a24218fed -->
