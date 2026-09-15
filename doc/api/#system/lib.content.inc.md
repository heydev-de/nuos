# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content.inc)

- **Version:** `26.9.9.7`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.content.inc

## Overview

This file is the core content management library for the PWNC Web Platform. It defines the `content` class, which manages the complete lifecycle of content objects — from creation and editing through publishing, scheduling, versioning, and deletion. It also provides standalone helper functions for content retrieval, parsing, template selection, and directory indexing.

The library implements a role-based permission system (writer, editor, publisher) that governs which actions each user role can perform on content at different statuses (draft, document, publication). It supports scheduled operations (apply, retrieve, publish, withdraw), undo/redo step buffers, content duplication and copying, and extra metadata extraction.

## Constants

### Content Types

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_TYPE_ORIGINAL` | 1 | Original content item |
| `CMS_CONTENT_TYPE_DUPLICATE` | 2 | Duplicate of an original (shares version history) |
| `CMS_CONTENT_TYPE_COPY` | 3 | Independent copy of an original |

### Content Statuses

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_STATUS_DRAFT` | 1 | Draft — being written/edited |
| `CMS_CONTENT_STATUS_DOCUMENT` | 2 | Document — authorized but not yet published |
| `CMS_CONTENT_STATUS_PUBLICATION` | 3 | Published — live on the site |
| `CMS_CONTENT_STATUS_MAIL` | 4 | Reserved for mail content |
| `CMS_CONTENT_STATUS_POOL` | 5 | Reserved for pool content |

### Content Flags

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_FLAG_NONE` | 0 | No flags set |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` | 1 | Exclude from sitemap |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX` | 2 | Set `noindex` meta robots |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW` | 4 | Set `nofollow` meta robots |
| `CMS_CONTENT_FLAG_ALL` | 4294967295 | All flags set |

### Content Actions

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_ACTION_NONE` | 0 | No action |
| `CMS_CONTENT_ACTION_CREATE` | 1 | Create new content |
| `CMS_CONTENT_ACTION_UPDATE` | 2 | Update existing content |
| `CMS_CONTENT_ACTION_AUTHORIZE` | 3 | Authorize (move to document) |
| `CMS_CONTENT_ACTION_DERIVE_DRAFT` | 4 | Derive a draft from published |
| `CMS_CONTENT_ACTION_PUBLISH` | 5 | Publish content |
| `CMS_CONTENT_ACTION_WITHDRAW` | 6 | Withdraw from publication |
| `CMS_CONTENT_ACTION_DUPLICATE` | 7 | Duplicate content |
| `CMS_CONTENT_ACTION_COPY` | 8 | Copy content |
| `CMS_CONTENT_ACTION_DELETE` | 9 | Delete content |
| `CMS_CONTENT_ACTION_RECEIVE` | 10 | Receive sent content |
| `CMS_CONTENT_ACTION_CHANNEL` | 11 | Set channel |
| `CMS_CONTENT_ACTION_FLAG` | 12 | Set flags |
| `CMS_CONTENT_ACTION_EXTRA` | 13 | Set extra metadata |

### Content Roles

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_ROLE_NONE` | 0 | No role |
| `CMS_CONTENT_ROLE_WRITER` | 1 | Writer role |
| `CMS_CONTENT_ROLE_EDITOR` | 2 | Editor role |
| `CMS_CONTENT_ROLE_PUBLISHER` | 4 | Publisher role |
| `CMS_CONTENT_ROLE_ALL` | 7 | All roles (writer + editor + publisher) |
| `CMS_CONTENT_ROLE_WRITER_EDITOR` | 8 | Writer or editor |
| `CMS_CONTENT_ROLE_WRITER_PUBLISHER` | 16 | Writer or publisher |
| `CMS_CONTENT_ROLE_EDITOR_PUBLISHER` | 32 | Editor or publisher |
| `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER` | 64 | Writer, editor, or publisher |

### Schedule Types

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_SCHEDULE_TYPE_APPLY` | 1 | Apply buffered changes |
| `CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE` | 2 | Retrieve a version |
| `CMS_CONTENT_SCHEDULE_TYPE_PUBLISH` | 3 | Publish content |
| `CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW` | 4 | Withdraw content |

### Permission Strings

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_PERMISSION_READER` | `"reader"` | Reader permission level |
| `CMS_CONTENT_PERMISSION_WRITER` | `"writer"` | Writer permission level |
| `CMS_CONTENT_PERMISSION_EDITOR` | `"editor"` | Editor permission level |
| `CMS_CONTENT_PERMISSION_PUBLISHER` | `"publisher"` | Publisher permission level |
| `CMS_CONTENT_PERMISSION_OPERATOR` | `"operator"` | Operator permission level |

### Database Table and Column Constants

These constants define the database schema for content tables and their columns.

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_CONTENT` | `CMS_DB_PREFIX . "content"` | Main content table |
| `CMS_DB_CONTENT_INDEX` | `"id"` | Primary key column |
| `CMS_DB_CONTENT_OWNER` | `"owner"` | Owner user column |
| `CMS_DB_CONTENT_TYPE` | `"type"` | Content type column |
| `CMS_DB_CONTENT_STATUS` | `"status"` | Content status column |
| `CMS_DB_CONTENT_FLAG` | `"flag"` | Flags column |
| `CMS_DB_CONTENT_CHANNEL` | `"channel"` | Channel column |
| `CMS_DB_CONTENT_WRITER` | `"writer"` | Writer user column |
| `CMS_DB_CONTENT_WRITER_TIME` | `"writer_time"` | Writer timestamp |
| `CMS_DB_CONTENT_WRITER_COMMENT` | `"writer_comment"` | Writer comment |
| `CMS_DB_CONTENT_EDITOR` | `"editor"` | Editor user column |
| `CMS_DB_CONTENT_EDITOR_TIME` | `"editor_time"` | Editor timestamp |
| `CMS_DB_CONTENT_EDITOR_COMMENT` | `"editor_comment"` | Editor comment |
| `CMS_DB_CONTENT_PUBLISHER` | `"publisher"` | Publisher user column |
| `CMS_DB_CONTENT_PUBLISHER_TIME` | `"publisher_time"` | Publisher timestamp |
| `CMS_DB_CONTENT_PUBLISHER_COMMENT` | `"publisher_comment"` | Publisher comment |
| `CMS_DB_CONTENT_TIME` | `"time"` | Publication time |
| `CMS_DB_CONTENT_TITLE` | `"title"` | Title column |
| `CMS_DB_CONTENT_AUTHOR` | `"author"` | Author column |
| `CMS_DB_CONTENT_DESCRIPTION` | `"description"` | Description column |
| `CMS_DB_CONTENT_KEYWORD` | `"keyword"` | Keyword column |
| `CMS_DB_CONTENT_IMAGE` | `"image"` | Image column |
| `CMS_DB_CONTENT_TEXT` | `"text"` | Published text column |
| `CMS_DB_CONTENT_TEMPLATE` | `"template"` | Template column |
| `CMS_DB_CONTENT_BUFFER_TITLE` | `"buffer_title"` | Buffered title |
| `CMS_DB_CONTENT_BUFFER_AUTHOR` | `"buffer_author"` | Buffered author |
| `CMS_DB_CONTENT_BUFFER_DESCRIPTION` | `"buffer_description"` | Buffered description |
| `CMS_DB_CONTENT_BUFFER_KEYWORD` | `"buffer_keyword"` | Buffered keyword |
| `CMS_DB_CONTENT_BUFFER_IMAGE` | `"buffer_image"` | Buffered image |
| `CMS_DB_CONTENT_BUFFER_TEXT` | `"buffer_text"` | Buffered text |
| `CMS_DB_CONTENT_BUFFER_TEMPLATE` | `"buffer_template"` | Buffered template |
| `CMS_DB_CONTENT_SENDER` | `"sender"` | Sender user column |
| `CMS_DB_CONTENT_SENDER_TIME` | `"sender_time"` | Sender timestamp |
| `CMS_DB_CONTENT_SENDER_COMMENT` | `"sender_comment"` | Sender comment |
| `CMS_DB_CONTENT_EXTRA_VALUE` | `"extra_value"` | Extra metadata value |
| `CMS_DB_CONTENT_EXTRA_TYPE` | `"extra_type"` | Extra metadata type |
| `CMS_DB_CONTENT_EXTRA_COLOR` | `"extra_color"` | Extra metadata color |
| `CMS_DB_CONTENT_VERSION` | `CMS_DB_PREFIX . "content_version"` | Version history table |
| `CMS_DB_CONTENT_VERSION_INDEX` | `"id"` | Version primary key |
| `CMS_DB_CONTENT_VERSION_CONTENT` | `"content"` | Foreign key to content |
| `CMS_DB_CONTENT_VERSION_TIME` | `"time"` | Version timestamp |
| `CMS_DB_CONTENT_VERSION_TITLE` | `"title"` | Version title |
| `CMS_DB_CONTENT_VERSION_AUTHOR` | `"author"` | Version author |
| `CMS_DB_CONTENT_VERSION_DESCRIPTION` | `"description"` | Version description |
| `CMS_DB_CONTENT_VERSION_KEYWORD` | `"keyword"` | Version keyword |
| `CMS_DB_CONTENT_VERSION_IMAGE` | `"image"` | Version image |
| `CMS_DB_CONTENT_VERSION_TEXT` | `"text"` | Version text |
| `CMS_DB_CONTENT_VERSION_TEMPLATE` | `"template"` | Version template |
| `CMS_DB_CONTENT_VERSION_HASH` | `"hash"` | Version hash (unique) |
| `CMS_DB_CONTENT_SCHEDULE` | `CMS_DB_PREFIX . "content_schedule"` | Schedule table |
| `CMS_DB_CONTENT_SCHEDULE_TIME` | `"time"` | Schedule execution time |
| `CMS_DB_CONTENT_SCHEDULE_TYPE` | `"type"` | Schedule type |
| `CMS_DB_CONTENT_SCHEDULE_CONTENT` | `"content"` | Foreign key to content |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_1` | `"value1"` | Schedule value 1 |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_2` | `"value2"` | Schedule value 2 |
| `CMS_DB_CONTENT_SCHEDULE_HASH` | `"hash"` | Schedule hash (primary key) |

## Standalone Functions

### content_get_range

Retrieves a range of text from a content document, using temporary caching for performance.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `&$content` | `content` | The content object instance |
| `$index` | `int` | The content index (ID) |
| `$range` | `mixed` | The range to retrieve (format depends on document class) |
| `$type` | `mixed\|NULL` | Optional type modifier for the range |

**Return Value**

- `mixed` — The retrieved text range, or `FALSE` if content is disabled, or `NULL` if retrieval fails.

**Inner Mechanisms**

1. Checks if the content object is enabled; returns `FALSE` if not.
2. Attempts to retrieve a cached `document` object from `cms_cache("content.$index")`.
3. If not cached, loads the `document` library, queries the database for the buffered text, creates a `document` object, caches it, and returns the range.
4. Returns `NULL` if the document cannot be loaded or the database query fails.

**Usage Example**

```php
$content = new content($user);
$text = content_get_range($content, 42, "1:10", "#text");
// Retrieves characters 1-10 of the buffered text for content ID 42
```

---

### content_set_range

Sets a range of text in a content document, applying various operations (paste, swap, copy, kick, drop, clear, shift) and persisting the result.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `&$content` | `content` | The content object instance |
| `$index` | `int` | The content index (ID) |
| `$range` | `mixed` | The range to modify |
| `$type` | `string` | The operation type (e.g., `"#paste"`, `"#swap"`, `"#copy"`, `"#kick1"`, `"#drop1"`, `"#clear"`, `"shift"`, or a custom type) |
| `$text` | `mixed` | The text or value to apply |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` if content is disabled or the operation fails.

**Inner Mechanisms**

1. Checks if content is enabled; returns `FALSE` if not.
2. Retrieves the document from cache or database (loading buffered text and template).
3. Switches on `$type` to perform the appropriate document operation:
   - `"#paste"`: Injects a new document from a clipboard buffer.
   - `"#swap"`/`"#dragdrop1"`: Swaps the range with `$text`.
   - `"#copy"`/`"#dragdrop2"`: Copies `$text` into the range.
   - `"#kick1"`/`"#kick2"`: Kicks the range by `$text`.
   - `"#drop1"`: Drops the range by 1 position.
   - `"#drop2"`: Drops the range by -1 position.
   - `"#clear"`: Deletes the range.
   - `"shift"`: Shifts the range and sets a "shift" marker.
   - Default: Sets the range with the given type and text.
4. Resets the document's default value.
5. Caches the modified document.
6. Calls `$content->update()` to persist the exported text to the database.

**Usage Example**

```php
$content = new content($user);
content_set_range($content, 42, "1:10", "#copy", "Hello World");
// Copies "Hello World" into characters 1-10 of content ID 42's buffer
```

---

### content_get_directory_index

Finds the first directory entry that links to a specified content index.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `int` | The content index to search for |

**Return Value**

- `mixed` — The directory key (string) of the first matching entry, or `0` if none found.

**Inner Mechanisms**

1. Loads the `#system/directory` data store.
2. Iterates through all directory entries.
3. Skips container entries.
4. Uses a regex to match `content://` URLs that reference the given `$content_index`.
5. Returns the key of the first match, or `0` if no match is found.

**Usage Example**

```php
$dirKey = content_get_directory_index(42);
// Returns the directory key for the first entry linking to content ID 42
```

---

### content_parse

Parses and renders content for display, with permanent file-based caching for performance.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `&$content` | `content` | The content object instance |
| `$index` | `int` | The content index (ID) |
| `$action` | `array\|NULL` | Optional action array (may contain `CMS_TEMPLATE_ACTION`) |
| `$header` | `mixed\|NULL` | Optional header data |
| `&$is_dynamic` | `bool` | Reference flag indicating if output is dynamic |
| `&$mod_time` | `int` | Reference for modification time (default `0`) |

**Return Value**

- `mixed` — The parsed output text, or `FALSE` if content is disabled or parsing fails.

**Inner Mechanisms**

1. Checks if content is enabled and loads `document` and `template` libraries.
2. Defines `CMS_CONTENT_INDEX` and `CMS_CONTENT_DIRECTORY_INDEX` constants if not already set.
3. Queries the database for content fields (flag, time, title, description, keyword, text, template).
4. Sets `$mod_time` from the database result.
5. Defines `CMS_CONTENT_META_ROBOTS` based on flag bits.
6. Retrieves or creates a cached `document` object.
7. If no action is specified (i.e., not a template action):
   - Generates a cache key from a hash of index, flag, user, language, URL, and serialized request.
   - Creates a cache directory path.
   - Opens a cache file with file locking.
   - If the cache is valid (modification time matches, directory and template data files haven't changed), reads and returns the cached output.
   - Otherwise, preprocesses via `$template->create_cache()`, writes the cache, and returns the output.
8. If an action is specified, processes completely via `$template->parse()` and sets `$is_dynamic = TRUE`.

**Usage Example**

```php
$content = new content($user);
$isDynamic = FALSE;
$modTime = 0;
$output = content_parse($content, 42, NULL, NULL, $isDynamic, $modTime);
// Parses and renders content ID 42 for display
```

---

### content_template_export

Exports a content document's template structure, optionally for a specific range.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `&$content` | `content` | The content object instance |
| `$index` | `int` | The content index (ID) |
| `$range` | `mixed\|NULL` | Optional range to export; if `NULL`, exports the full template |

**Return Value**

- `mixed` — The exported template structure, or `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled and loads `document` and `template` libraries.
2. Queries the database for buffered text and template.
3. Retrieves or creates a cached `document` object.
4. Instantiates a `template` object and calls `$template->export()` with the document, template (or `NULL` if range is specified), range, and `TRUE` for the export flag.

**Usage Example**

```php
$content = new content($user);
$export = content_template_export($content, 42);
// Exports the full template structure for content ID 42
```

---

### content_template_select

Builds a sorted list of available page templates for selection in a UI.

**Parameters**

None.

**Return Value**

- `array\|FALSE` — An associative array mapping display names to template keys, sorted naturally. Returns `FALSE` if templates are not available.

**Inner Mechanisms**

1. Checks if templates are available via `cms_available("template")`.
2. Loads the `#system/template` data store.
3. Iterates through all templates, skipping those without a `page` property.
4. Builds display names, prefixing with category if present.
5. Handles duplicate names by appending `(1)`, `(2)`, etc.
6. Sorts the array naturally (case-insensitive) and returns it.

**Usage Example**

```php
$templates = content_template_select();
// Returns ['Default' => 'default', 'Blog/Post' => 'blog_post', ...]
```

---

### content_get_receiver

Determines which users can receive a content item for duplication or copying, based on their permissions.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `&$content` | `content` | The content object instance |
| `$type` | `int` | The content type (original, duplicate, or copy) |
| `$status` | `int` | The content status |

**Return Value**

- `array\|FALSE` — An array with three sub-arrays (for original, duplicate, and copy types), each mapping user keys to `TRUE` if the user can receive. Returns `FALSE` if content is disabled.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Initializes an array with three type keys.
3. Tests if the current user can duplicate or copy the content.
4. Backs up current writer/editor/publisher permissions.
5. Iterates through all users in the `#system/permission` data store.
6. For each user (excluding the current user), temporarily sets their permissions and checks if they can receive the content for each type.
7. Restores original permissions.
8. Returns the array of permitted receivers.

**Usage Example**

```php
$content = new content($user);
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT);
// Returns list of users who can receive this content for duplication/copying
```

## content Class

The `content` class manages the full lifecycle of content objects in the PWNC platform. It handles database schema verification, scheduled operations, permission-based access control, and all content state transitions.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$action` | `array` | `NULL` | Permission matrix mapping type → status → action → required role |
| `$user` | `string` | `NULL` | The current user identifier |
| `$writer` | `bool` | `FALSE` | Whether the user has writer permissions |
| `$editor` | `bool` | `FALSE` | Whether the user has editor permissions |
| `$publisher` | `bool` | `FALSE` | Whether the user has publisher permissions |
| `$operator` | `bool` | `FALSE` | Whether the user has operator permissions |
| `$enabled` | `bool` | `FALSE` | Whether the content system is enabled (tables verified) |

### __construct

Initializes the content system: verifies database tables, processes scheduled operations, and sets up user permissions.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string\|NULL` | The user identifier; if empty, defaults to `CMS_SUPERUSER` |

**Return Value**

None (constructor).

**Inner Mechanisms**

1. Defines the `$action` permission matrix as a 3D array: type → status → action → required role bitmask. This matrix defines which roles can perform which actions on which content types at which statuses.
2. Creates a `mysql` instance and verifies three database tables:
   - `CMS_DB_CONTENT` — main content table with all columns and an index on owner/type/status.
   - `CMS_DB_CONTENT_VERSION` — version history table with a unique hash column.
   - `CMS_DB_CONTENT_SCHEDULE` — schedule table with a primary key hash.
3. If all tables are verified, sets `$enabled = TRUE`.
4. Processes pending scheduled operations:
   - Sets all role flags to `TRUE` (system-level processing).
   - Queries the schedule table for entries with time ≤ current time.
   - For each entry, retrieves the content owner, sets `$this->user`, and dispatches to the appropriate method based on schedule type:
     - `APPLY` → `$this->apply($index)`
     - `RETRIEVE` → `$this->version_retrieve($value1, 0, TRUE)`
     - `PUBLISH` → `$this->publish(...)` with directory parameters
     - `WITHDRAW` → `$this->withdraw(...)` with directory index
   - Deletes each processed schedule entry.
5. Sets up user permissions:
   - If `$user` is empty, uses `CMS_SUPERUSER`.
   - Sets `$this->user` to the resolved user.
   - Builds a permission prefix string.
   - Queries `cms_permission()` for writer, editor, publisher, and operator permissions.

**Usage Example**

```php
$content = new content("john_doe");
// Creates a content manager for user "john_doe" with appropriate permissions
```

---

### test_create

Tests whether the current user can create a new content item.

**Parameters**

None.

**Return Value**

- `bool` — `TRUE` if creation is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->create("", NULL, NULL, TRUE)` with `$test = TRUE`, which checks permissions without actually creating the content.

**Usage Example**

```php
if ($content->test_create()) {
    // User can create new content
}
```

---

### create

Creates a new content item in draft status.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$title` | `string` | — | The content title |
| `$template` | `string\|NULL` | `NULL` | Optional template name |
| `$comment` | `string\|NULL` | `NULL` | Optional writer comment |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission without creating |

**Return Value**

- `int\|FALSE` — The new content index (ID) on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled and if the user has `CREATE` permission for original/draft content.
2. If `$test` is `TRUE`, returns `TRUE` immediately.
3. Processes the title through language functions if it's a language key.
4. Gets the default RSS channel if the RSS library is loaded.
5. Builds and executes an `INSERT` query with:
   - Owner set to the current user.
   - Type set to `CMS_CONTENT_TYPE_ORIGINAL`.
   - Status set to `CMS_CONTENT_STATUS_DRAFT`.
   - Writer set to `CMS_SUPERUSER`.
   - Timestamps set to current time.
   - Title, author, and buffer fields populated.
   - Template included if provided.
6. On success, clears the step buffer and returns the new content ID via `mysql_insert_id()`.

**Usage Example**

```php
$index = $content->create("My New Article", "blog_post", "Initial draft");
// Creates a new draft content item with the given title and template
```

---

### test_update

Tests whether the current user can update a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if update is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->update($index, NULL, NULL, NULL, NULL, NULL, NULL, NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_update(42)) {
    // User can update content ID 42
}
```

---

### update

Updates an existing content item's buffered fields.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$title` | `string\|NULL` | `NULL` | New title (or `NULL` to skip) |
| `$description` | `string\|NULL` | `NULL` | New description |
| `$keyword` | `string\|NULL` | `NULL` | New keyword |
| `$image` | `string\|NULL` | `NULL` | New image |
| `$text` | `string\|NULL` | `NULL` | New buffered text |
| `$comment` | `string\|NULL` | `NULL` | Writer comment |
| `$template` | `string\|NULL` | `NULL` | New template |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Checks for external writer permission via `cms_permission("content.writer.$index")`.
3. Queries the database for the content's owner, type, status, and writer.
4. Validates that either:
   - The user is the owner AND has `UPDATE` permission for the content's type/status, OR
   - An external writer permission exists AND the content is in publication status.
5. If `$test` is `TRUE`, returns `TRUE`.
6. Processes the title through language functions if it's a language key.
7. Determines the new status (publication stays publication, otherwise draft).
8. Sets the writer to the external writer if applicable, otherwise the current user.
9. Stores the current step for undo/redo.
10. Builds and executes an `UPDATE` query setting only the non-`NULL` fields.
11. Calls `refresh_extra()` to update extra metadata.
12. Returns `TRUE` on success.

**Usage Example**

```php
$content->update(42, "Updated Title", "New description", NULL, NULL, $newText);
// Updates the title, description, and text of content ID 42
```

---

### test_copy

Tests whether the current user can copy a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if copy is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->copy($index, FALSE, TRUE)` with `$duplicate = FALSE` and `$test = TRUE`.

**Usage Example**

```php
if ($content->test_copy(42)) {
    // User can copy content ID 42
}
```

---

### copy

Creates a copy or duplicate of an existing content item.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The source content index (ID) |
| `$duplicate` | `bool` | `FALSE` | If `TRUE`, creates a duplicate (shares version history); if `FALSE`, creates an independent copy |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `int\|FALSE` — The new content index (ID) on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Determines the action (`DUPLICATE` or `COPY`) and type (`DUPLICATE` or `COPY`) based on `$duplicate`.
3. Queries the source content from the database.
4. Validates that the user is the owner and has the appropriate action permission.
5. If `$test` is `TRUE`, returns `TRUE`.
6. Determines the new status:
   - For duplicates: always `DRAFT`.
   - For copies: if source is `PUBLICATION`, new status is `DOCUMENT`; otherwise, same as source.
7. Builds and executes an `INSERT` query copying all fields from the source, with:
   - Owner set to the current user.
   - Type set to the determined type.
   - Status set to the determined status.
   - Sender fields populated.
8. If duplicating, also copies version history from the source, generating new SHA-256 hashes.
9. Clears the step buffer for the new content.
10. Returns the new content ID.

**Usage Example**

```php
$newIndex = $content->copy(42, TRUE);
// Creates a duplicate of content ID 42 (shares version history, starts as draft)
```

---

### test_duplicate

Tests whether the current user can duplicate a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if duplication is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->copy($index, TRUE, TRUE)` with `$duplicate = TRUE` and `$test = TRUE`.

**Usage Example**

```php
if ($content->test_duplicate(42)) {
    // User can duplicate content ID 42
}
```

---

### duplicate

Creates a duplicate of an existing content item (shares version history).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The source content index (ID) |

**Return Value**

- `int\|FALSE` — The new content index (ID) on success, `FALSE` on failure.

**Inner Mechanisms**

Calls `$this->copy($index, TRUE)` with `$duplicate = TRUE`.

**Usage Example**

```php
$newIndex = $content->duplicate(42);
// Creates a duplicate of content ID 42
```

---

### test_authorize

Tests whether the current user can authorize a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if authorization is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->authorize($index, NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_authorize(42)) {
    // User can authorize content ID 42
}
```

---

### authorize

Authorizes a content item, moving it from draft to document status.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$comment` | `string\|NULL` | `NULL` | Optional editor comment |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that the user is the owner and has `AUTHORIZE` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Executes an `UPDATE` query setting:
   - Status to `CMS_CONTENT_STATUS_DOCUMENT`.
   - Editor to `CMS_NAME`.
   - Editor time to current time.
   - Editor comment if provided.
6. Clears the step buffer.
7. Returns `TRUE` on success.

**Usage Example**

```php
$content->authorize(42, "Ready for review");
// Authorizes content ID 42, moving it to document status
```

---

### test_derive_draft

Tests whether the current user can derive a draft from a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if derivation is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->derive_draft($index, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_derive_draft(42)) {
    // User can derive a draft from content ID 42
}
```

---

### derive_draft

Derives a draft from a published content item, allowing further editing.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that the user is the owner and has `DERIVE_DRAFT` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Executes an `UPDATE` query setting the status to `CMS_CONTENT_STATUS_DRAFT`.
6. Clears the step buffer.
7. Returns `TRUE` on success.

**Usage Example**

```php
$content->derive_draft(42);
// Derives a draft from published content ID 42 for further editing
```

---

### test_publish

Tests whether the current user can publish a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if publishing is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->publish($index, NULL, 0, 0, NULL, NULL, NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_publish(42)) {
    // User can publish content ID 42
}
```

---

### publish

Publishes a content item, either immediately or on a schedule, with optional directory integration.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$comment` | `string\|NULL` | `NULL` | Optional publisher comment |
| `$time_publish` | `int` | `0` | Scheduled publish time (0 = immediate) |
| `$time_withdraw` | `int` | `0` | Scheduled withdraw time (0 = no auto-withdraw) |
| `$directory_index` | `mixed\|NULL` | `NULL` | Directory key for linking |
| `$directory_action` | `string\|NULL` | `NULL` | Directory action: `"replace"`, `"insert"`, `"append"` |
| `$directory_title` | `string\|NULL` | `NULL` | Title for new directory entries |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `mixed` — The directory index on success (if directory action was performed), `TRUE` on success (no directory action), or `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, status, title, and description.
3. Validates that the user is the owner and has `PUBLISH` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Gets the current time.
6. **Scheduled publication** (if `$time_publish > $time`):
   - If a directory action is specified, loads the `directory` library and creates a placeholder entry:
     - `"insert"`: Inserts a new directory entry before the given key.
     - `"append"`: Appends a new directory entry after the given key.
   - Saves the directory.
   - Adds schedule entries for publish (and optionally withdraw).
   - Returns the directory index.
7. **Immediate publication**:
   - Executes an `UPDATE` query setting:
     - Status to `CMS_CONTENT_STATUS_PUBLICATION`.
     - Publisher to `CMS_NAME`.
     - Publisher time to current time.
     - Publisher comment if provided.
   - Clears the step buffer.
   - If a directory action is specified, loads the `directory` library and:
     - `"replace"`: Updates an existing directory entry's name, description, and URL.
     - `"insert"`: Inserts a new directory entry with `content://$index` URL.
     - `"append"`: Appends a new directory entry with `content://$index` URL.
   - Saves the directory.
   - Adds a withdraw schedule if `$time_withdraw > $time`.
   - Returns the directory index (or `TRUE` if no directory action).

**Usage Example**

```php
$dirIndex = $content->publish(42, "Published", 0, 0, "main_menu", "replace", "My Article");
// Immediately publishes content ID 42 and links it in the directory
```

---

### test_apply

Tests whether the current user can apply buffered changes to a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if application is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->apply($index, 0, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_apply(42)) {
    // User can apply buffered changes to content ID 42
}
```

---

### apply

Applies buffered changes to a content item, either immediately or on a schedule.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$time` | `int` | `0` | Scheduled apply time (0 = immediate) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Calls `$this->test_update($index)` to verify update permission.
2. If `$test` is `TRUE`:
   - Queries whether the writer time differs from the publication time (indicating pending changes).
   - Returns `TRUE` if changes exist, `FALSE` otherwise.
3. **Scheduled application** (if `$time > time()`):
   - Adds a schedule entry for `CMS_CONTENT_SCHEDULE_TYPE_APPLY`.
4. **Immediate application**:
   - Deletes any existing apply schedule.
   - Stores a version via `_version_store()`.
   - Executes an `UPDATE` query copying all buffer fields to the live fields:
     - `time` = `writer_time`
     - `title` = `buffer_title`
     - `author` = `buffer_author`
     - `description` = `buffer_description`
     - `keyword` = `buffer_keyword`
     - `image` = `buffer_image`
     - `text` = `buffer_text`
     - `template` = `buffer_template`
   - If the `content_pool` library is loaded, synchronizes the content pool.
   - Returns `TRUE` on success.

**Usage Example**

```php
$content->apply(42);
// Applies buffered changes to content ID 42 immediately
```

---

### test_revert

Tests whether the current user can revert a content item to its last applied state.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if revert is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->revert($index, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_revert(42)) {
    // User can revert content ID 42
}
```

---

### revert

Reverts a content item's buffered state to match its currently published state.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Calls `$this->test_update($index)` to verify update permission.
2. If `$test` is `TRUE`, returns `TRUE`.
3. Stores the current step for undo/redo.
4. Executes an `UPDATE` query copying all live fields to the buffer fields:
   - `writer_time` = `time`
   - `buffer_title` = `title`
   - `buffer_author` = `author`
   - `buffer_description` = `description`
   - `buffer_keyword` = `keyword`
   - `buffer_image` = `image`
   - `buffer_text` = `text`
   - `buffer_template` = `template`
5. Calls `refresh_extra()` to update extra metadata.
6. Returns `TRUE` on success.

**Usage Example**

```php
$content->revert(42);
// Reverts the buffer of content ID 42 to match its published state
```

---

### test_version_store

Tests whether the current user can store a version of a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if version storage is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->version_store($index, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_version_store(42)) {
    // User can store a version of content ID 42
}
```

---

### version_store

Stores the current buffered state of a content item as a new version.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Calls `$this->test_update($index)` to verify update permission.
2. If `$test` is `TRUE`, returns `TRUE`.
3. Calls `_version_store($index)` to insert a version record.
4. Returns `TRUE` on success.

**Usage Example**

```php
$content->version_store(42);
// Stores the current state of content ID 42 as a new version
```

---

### _version_store

Internal method that inserts a version record into the version table.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$version_index` | `int` | The content index (ID) to version |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

Executes an `INSERT IGNORE` query that:
1. Selects the content's writer_time, buffer fields, and index from the content table.
2. Generates a SHA-256 hash from the concatenation of index and all buffer fields.
3. Inserts the record into the version table.
4. Uses `INSERT IGNORE` to prevent duplicate versions (same hash = same content).

**Usage Example**

```php
// Called internally by version_store()
$content->_version_store(42);
```

---

### test_version_retrieve

Tests whether the current user can retrieve a version of a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$version_index` | `int` | The version index (ID) |

**Return Value**

- `bool` — `TRUE` if version retrieval is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->version_retrieve($version_index, 0, FALSE, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_version_retrieve(100)) {
    // User can retrieve version ID 100
}
```

---

### version_retrieve

Retrieves a stored version of a content item, either immediately or on a schedule, into either the active or buffered state.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$version_index` | `int` | — | The version index (ID) |
| `$time` | `int` | `0` | Scheduled retrieve time (0 = immediate) |
| `$apply` | `bool` | `FALSE` | If `TRUE`, retrieves into active state; if `FALSE`, retrieves into buffered state |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the version table for the version record.
3. Calls `$this->test_update()` on the associated content index to verify permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. **Scheduled retrieval** (if `$time > time()`):
   - Adds a schedule entry for `CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE`.
6. **Immediate retrieval**:
   - Deletes any existing retrieve schedule.
   - Stores the current step for undo/redo.
   - If `$apply` is `TRUE`:
     - Executes an `UPDATE` query setting live fields from the version record, with `time` set to current time.
   - If `$apply` is `FALSE`:
     - Executes an `UPDATE` query setting buffer fields from the version record, with `writer_time` set to the version's time.
   - Calls `refresh_extra()` if retrieving into buffer.
   - Returns `TRUE` on success.

**Usage Example**

```php
$content->version_retrieve(100, 0, TRUE);
// Immediately retrieves version ID 100 into the active state of its content
```

---

### schedule_add

Adds a scheduled operation to the schedule table.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$time` | `int` | — | Execution time (Unix timestamp) |
| `$type` | `int` | — | Schedule type (apply, retrieve, publish, withdraw) |
| `$content_index` | `int` | — | The content index (ID) |
| `$value1` | `mixed\|NULL` | `NULL` | Schedule value 1 (e.g., directory index) |
| `$value2` | `mixed\|NULL` | `NULL` | Schedule value 2 (e.g., directory title) |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

Executes a `REPLACE` query that:
1. Computes a hash from the concatenation of type, content index, and values.
2. Inserts or replaces the schedule record with the computed hash as the primary key.
3. This ensures that duplicate schedules for the same content/type/values are replaced rather than duplicated.

**Usage Example**

```php
$content->schedule_add(time() + 3600, CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 42);
// Schedules content ID 42 to be published in 1 hour
```

---

### schedule_delete

Deletes a scheduled operation from the schedule table.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$type` | `int` | — | Schedule type |
| `$content_index` | `int` | — | The content index (ID) |
| `$value1` | `mixed\|NULL` | `NULL` | Schedule value 1 |
| `$value2` | `mixed\|NULL` | `NULL` | Schedule value 2 |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

Computes the same hash as `schedule_add()` and calls `_schedule_delete()` with it.

**Usage Example**

```php
$content->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 42);
// Deletes the scheduled publish operation for content ID 42
```

---

### _schedule_delete

Internal method that deletes a schedule entry by its hash.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$hash` | `string` | The schedule hash (primary key) |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

Executes a `DELETE` query on the schedule table where the hash matches.

**Usage Example**

```php
// Called internally by schedule_delete()
$content->_schedule_delete("abc123hash");
```

---

### test_withdraw

Tests whether the current user can withdraw a content item from publication.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if withdrawal is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->withdraw($index, NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_withdraw(42)) {
    // User can withdraw content ID 42
}
```

---

### withdraw

Withdraws a content item from publication, moving it to document status and removing directory links.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$directory_index` | `mixed\|NULL` | `NULL` | Specific directory entry to unlink (or `NULL` for all) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that the user is the owner and has `WITHDRAW` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Calls `_withdraw()` to remove directory links.
6. Executes an `UPDATE` query setting the status to `CMS_CONTENT_STATUS_DOCUMENT`.
7. Returns `TRUE` on success.

**Usage Example**

```php
$content->withdraw(42);
// Withdraws content ID 42 from publication, moving it to document status
```

---

### _withdraw

Internal method that removes directory links to a content item.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$directory_index` | `mixed\|NULL` | `NULL` | Specific directory entry to unlink (or `NULL` for all) |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Loads the `directory` library.
2. Queries the schedule table for scheduled publish entries to build a list of protected directory indexes.
3. Iterates through the directory data:
   - For container entries, checks if the URL links to the specified content.
   - If `$directory_index` is `NULL` or matches the current key, releases the URL (sets it to empty).
   - Otherwise, increments a remainder count.
   - Tracks container depth and removes empty containers that are not protected by schedules.
4. Saves the directory.
5. Returns `TRUE` if the directory was saved and no remaining links exist.

**Usage Example**

```php
// Called internally by withdraw() and delete()
$content->_withdraw(42);
```

---

### test_send

Tests whether the current user can send a content item to another user.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if sending is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->send($index, $this->user, NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_send(42)) {
    // User can send content ID 42
}
```

---

### send

Sends a content item to another user, transferring ownership.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$receiver` | `string` | — | The receiving user identifier |
| `$comment` | `string\|NULL` | `NULL` | Optional sender comment |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, status, and writer.
3. Validates that the user is the owner.
4. Temporarily sets the user's permissions to the receiver's permissions.
5. Checks if the receiver has `RECEIVE` permission for the content's type/status.
6. Restores the original permissions.
7. If `$test` is `TRUE`, returns `TRUE` if the receiver has permission.
8. Executes an `UPDATE` query setting:
   - Owner to the receiver.
   - Sender to `CMS_NAME`.
   - Sender time to current time.
   - Sender comment if provided.
9. Clears the step buffer.
10. Returns `TRUE` on success.

**Usage Example**

```php
$content->send(42, "jane_doe", "Please review this");
// Sends content ID 42 to user "jane_doe"
```

---

### test_delete

Tests whether the current user can delete a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if deletion is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->delete($index, FALSE, FALSE, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_delete(42)) {
    // User can delete content ID 42
}
```

---

### delete

Deletes a content item and all associated data (versions, schedules, directory links).

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$ignore_directory` | `bool` | `FALSE` | If `TRUE`, skips directory link removal |
| `$override_owner` | `bool` | `FALSE` | If `TRUE`, bypasses owner check |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that either the user is the owner (with `DELETE` permission) or `$override_owner` is `TRUE`.
4. If `$test` is `TRUE`, returns `TRUE`.
5. If `$ignore_directory` is `FALSE`, calls `_withdraw()` to remove directory links.
6. Executes three `DELETE` queries:
   - Deletes the content record from the main table.
   - Deletes all version records from the version table.
   - Deletes all schedule records from the schedule table.
7. Clears the step buffer.
8. Returns `TRUE` on success.

**Usage Example**

```php
$content->delete(42);
// Deletes content ID 42 and all associated data
```

---

### test_flag_set

Tests whether the current user can set flags on a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if flag setting is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->flag_set($index, CMS_CONTENT_FLAG_NONE, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_flag_set(42)) {
    // User can set flags on content ID 42
}
```

---

### flag_set

Sets flags on a content item.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$flag` | `int` | `CMS_CONTENT_FLAG_NONE` | The flag value to set |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that the user is the owner and has `FLAG` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Executes an `UPDATE` query setting the flag column.
6. Returns `TRUE` on success.

**Usage Example**

```php
$content->flag_set(42, CMS_CONTENT_FLAG_SITEMAP_EXCLUDE);
// Sets the sitemap exclude flag on content ID 42
```

---

### test_channel_set

Tests whether the current user can set the channel on a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if channel setting is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->channel_set($index, "", TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_channel_set(42)) {
    // User can set the channel on content ID 42
}
```

---

### channel_set

Sets the RSS channel for a content item.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$channel` | `string` | `""` | The channel name |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Queries the content's owner, type, and status.
3. Validates that the user is the owner and has `CHANNEL` permission.
4. If `$test` is `TRUE`, returns `TRUE`.
5. Executes an `UPDATE` query setting the channel column.
6. Returns `TRUE` on success.

**Usage Example**

```php
$content->channel_set(42, "news");
// Sets the RSS channel to "news" for content ID 42
```

---

### step_store

Stores the current buffered text state for undo/redo functionality.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Retrieves step metadata from cache: offset, count, and position.
3. Queries the current buffered text from the database.
4. Computes the storage position: `($_position = ($offset + $position) % 10)`.
5. Stores the current buffered text in cache at `content.$index.step.$_position`.
6. Updates the step metadata:
   - If position ≥ 9, increments offset (mod 10) and resets position.
   - Otherwise, increments position and count.
7. Saves the updated metadata to cache.
8. Returns `TRUE` on success.

This implements a circular buffer of 10 steps for undo/redo.

**Usage Example**

```php
// Called internally before updates to enable undo
$content->step_store(42);
```

---

### test_step_undo

Tests whether the current user can undo a step.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if undo is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->step_undo($index, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_step_undo(42)) {
    // User can undo a step on content ID 42
}
```

---

### step_undo

Undoes the last buffered text change, restoring the previous state.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Retrieves step metadata from cache.
3. Validates that position ≥ 1 (there is something to undo).
4. Calls `$this->test_update($index)` to verify update permission.
5. If `$test` is `TRUE`, returns `TRUE`.
6. Stores the current state at the current position (for potential redo).
7. Decrements the position.
8. Retrieves the previous state from cache.
9. Executes an `UPDATE` query setting the buffered text and writer time.
10. Updates the step metadata.
11. Calls `refresh_extra()`.
12. Returns `TRUE` on success.

**Usage Example**

```php
$content->step_undo(42);
// Undoes the last change to content ID 42's buffer
```

---

### step_undo_depth

Returns the number of undoable steps available.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `int` — The number of undoable steps (0 if none or disabled).

**Inner Mechanisms**

1. Checks if content is enabled.
2. Retrieves step metadata from cache.
3. Returns the position value (number of steps that can be undone).

**Usage Example**

```php
$depth = $content->step_undo_depth(42);
// Returns the number of undoable steps for content ID 42
```

---

### test_step_redo

Tests whether the current user can redo a step.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if redo is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->step_redo($index, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_step_redo(42)) {
    // User can redo a step on content ID 42
}
```

---

### step_redo

Redoes the last undone buffered text change.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` otherwise.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Retrieves step metadata from cache.
3. Validates that position < count (there is something to redo).
4. Calls `$this->test_update($index)` to verify update permission.
5. If `$test` is `TRUE`, returns `TRUE`.
6. Increments the position.
7. Retrieves the next state from cache.
8. Executes an `UPDATE` query setting the buffered text and writer time.
9. Updates the step metadata.
10. Calls `refresh_extra()`.
11. Returns `TRUE` on success.

**Usage Example**

```php
$content->step_redo(42);
// Redoes the last undone change to content ID 42's buffer
```

---

### step_redo_depth

Returns the number of redoable steps available.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `int` — The number of redoable steps (0 if none or disabled).

**Inner Mechanisms**

1. Checks if content is enabled.
2. Retrieves step metadata from cache.
3. Returns `count - position` (number of steps that can be redone).

**Usage Example**

```php
$depth = $content->step_redo_depth(42);
// Returns the number of redoable steps for content ID 42
```

---

### step_clear

Clears all undo/redo step data for a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` if disabled.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Clears the step metadata cache entry.
3. Clears all 10 step cache entries (indices 0-9).
4. Returns `TRUE`.

**Usage Example**

```php
$content->step_clear(42);
// Clears all undo/redo history for content ID 42
```

---

### action

Checks whether the current user has permission to perform a specific action on content of a given type and status.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | The content type (original, duplicate, copy) |
| `$status` | `int` | The content status (draft, document, publication) |
| `$action` | `int` | The action to check (create, update, publish, etc.) |

**Return Value**

- `bool` — `TRUE` if the user has permission, `FALSE` otherwise.

**Inner Mechanisms**

1. Looks up the required role bitmask from the `$action` matrix: `$this->action[$type][$status][$action]`.
2. Checks the user's role flags against the required role using bitwise operations:
   - If the user is a writer:
     - Checks `CMS_CONTENT_ROLE_WRITER` bit.
     - If also an editor, checks `CMS_CONTENT_ROLE_WRITER_EDITOR` and `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER` bits.
     - Checks `CMS_CONTENT_ROLE_WRITER_PUBLISHER` bit.
   - If the user is an editor:
     - Checks `CMS_CONTENT_ROLE_EDITOR` bit.
     - Checks `CMS_CONTENT_ROLE_EDITOR_PUBLISHER` bit.
   - If the user is a publisher:
     - Checks `CMS_CONTENT_ROLE_PUBLISHER` bit.
3. Returns `TRUE` if any role check passes, `FALSE` otherwise.

**Usage Example**

```php
if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_PUBLISH)) {
    // User can publish this content
}
```

---

### refresh_extra

Extracts and stores extra metadata from a content document's data structure.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool\|NULL` — `TRUE` on success, `FALSE` on failure, `NULL` if extra metadata is not enabled.

**Inner Mechanisms**

1. Checks if content is enabled.
2. Creates a `system` instance and retrieves the `content.extra_id` system value.
3. If the extra ID is empty, returns `NULL` (feature not enabled).
4. Loads the `document` library.
5. Queries the buffered text from the database.
6. Creates a `document` object from the buffered text.
7. Extracts the type and value from the document's data at the extra ID key.
8. Executes an `UPDATE` query setting the `extra_type` and `extra_value` columns.
9. Returns `TRUE` on success.

**Usage Example**

```php
// Called internally after content updates
$content->refresh_extra(42);
```

---

### test_set_extra

Tests whether the current user can set extra metadata on a content item.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The content index (ID) |

**Return Value**

- `bool` — `TRUE` if setting extra metadata is permitted, `FALSE` otherwise.

**Inner Mechanisms**

Calls `$this->set_extra($index, NULL, "value", NULL, TRUE)` with `$test = TRUE`.

**Usage Example**

```php
if ($content->test_set_extra(42)) {
    // User can set extra metadata on content ID 42
}
```

---

### set_extra

Sets extra metadata (type, value, color) on a content item.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | The content index (ID) |
| `$value` | `mixed\|NULL` | `NULL` | The extra value |
| `$type` | `string` | `"value"` | The extra type |
| `$color` | `mixed\|NULL` | `NULL` | The extra color |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests permission |

**Return Value**

- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**

1. Calls `$this->test_update($index)` to verify update permission.
2. If `$test` is `TRUE`, returns `TRUE`.
3. Executes an `UPDATE` query setting the `extra_type`, `extra_value`, and `extra_color` columns.
4. Returns `TRUE` on success.

**Usage Example**

```php
$content->set_extra(42, "featured", "category", "#ff0000");
// Sets extra metadata on content ID 42
```


<!-- HASH:0c731c58aede70fcca59dbe45b46e1ff -->
