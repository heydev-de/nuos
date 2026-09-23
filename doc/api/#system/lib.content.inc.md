# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content.inc)

- **Version:** `26.9.23.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# System Content Library

## Overview

The `system/lib.content.inc` file serves as the core engine for the Content Management System (CMS) within the PWNC platform. It provides the logic for managing content lifecycle, including creation, editing, publishing, versioning, and scheduling. It implements a robust permission system based on roles (Writer, Editor, Publisher) and handles content caching to ensure high performance.

## Constants

### Content Types

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_CONTENT_TYPE_ORIGINAL` | 1 | The original source content. |
| `CMS_CONTENT_TYPE_DUPLICATE` | 2 | A duplicate of original content. |
| `CMS_CONTENT_TYPE_COPY` | 3 | A copy of content (often with different permissions). |

### Content Statuses

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_CONTENT_STATUS_DRAFT` | 1 | Content is a draft, not visible to the public. |
| `CMS_CONTENT_STATUS_DOCUMENT` | 2 | Content is authorized and ready for review. |
| `CMS_CONTENT_STATUS_PUBLICATION` | 3 | Content is published and live. |
| `CMS_CONTENT_STATUS_MAIL` | 4 | Reserved for email content. |
| `CMS_CONTENT_STATUS_POOL` | 5 | Reserved for content pools. |

### Content Flags

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_CONTENT_FLAG_NONE` | 0 | No special flags set. |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` | 1 | Exclude from sitemap generation. |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX` | 2 | Set meta robots to `noindex`. |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW` | 4 | Set meta robots to `nofollow`. |
| `CMS_CONTENT_FLAG_ALL` | 4294967295 | All flags enabled. |

### Content Actions

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_CONTENT_ACTION_NONE` | 0 | No action performed. |
| `CMS_CONTENT_ACTION_CREATE` | 1 | Create new content. |
| `CMS_CONTENT_ACTION_UPDATE` | 2 | Update existing content. |
| `CMS_CONTENT_ACTION_AUTHORIZE` | 3 | Authorize content (move to Document status). |
| `CMS_CONTENT_ACTION_PUBLISH` | 5 | Publish content. |
| `CMS_CONTENT_ACTION_WITHDRAW` | 6 | Withdraw published content. |
| `CMS_CONTENT_ACTION_DELETE` | 9 | Delete content. |

### Content Roles

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_CONTENT_ROLE_NONE` | 0 | No role. |
| `CMS_CONTENT_ROLE_WRITER` | 1 | Can create and update drafts. |
| `CMS_CONTENT_ROLE_EDITOR` | 2 | Can authorize and delete documents. |
| `CMS_CONTENT_ROLE_PUBLISHER` | 4 | Can publish content. |
| `CMS_CONTENT_ROLE_WRITER_EDITOR` | 8 | Writer + Editor. |
| `CMS_CONTENT_ROLE_WRITER_PUBLISHER` | 16 | Writer + Publisher. |
| `CMS_CONTENT_ROLE_EDITOR_PUBLISHER` | 32 | Editor + Publisher. |
| `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER` | 64 | All roles combined. |

### Database Columns

| Name | Value | Description |
| :--- | :--- | :--- |
| `CMS_DB_CONTENT` | Prefix + "content" | Main content table. |
| `CMS_DB_CONTENT_INDEX` | "id" | Primary key. |
| `CMS_DB_CONTENT_TYPE` | "type" | Content type (Original, Duplicate, Copy). |
| `CMS_DB_CONTENT_STATUS` | "status" | Current status (Draft, Document, Publication). |
| `CMS_DB_CONTENT_FLAG` | "flag" | Bitmask of flags. |
| `CMS_DB_CONTENT_TEXT` | "text" | The actual content body (MEDIUMTEXT). |
| `CMS_DB_CONTENT_BUFFER_TEXT` | "buffer_text" | Temporary buffer for updates. |
| `CMS_DB_CONTENT_VERSION` | Prefix + "content_version" | Version history table. |

## Functions

### content_get_range

Retrieves a specific range of text from a content item.

**Purpose:** Fetches content data, prioritizing a temporary RAM cache over the database.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$content` | `object` | The content object context. |
| `$index` | `string` | The unique identifier of the content. |
| `$range` | `string` | The specific range or key to retrieve. |
| `$type` | `string` | The type of data to retrieve (e.g., "text"). |

**Return Values:**

*   `mixed`: The retrieved data or `NULL` if not found.

**Inner Mechanisms:**
1.  Checks if the content is enabled.
2.  Checks RAM cache (`cms_cache`) for the content index.
3.  If not in RAM, queries the database for `buffer_text`.
4.  Instantiates a `document` object from the text.
5.  Caches the document object in RAM.
6.  Calls `document->get()` to retrieve the specific range.

**Usage Example:**
```php
// Retrieve the main text body of content index 'my-article'
$text = content_get_range($content, 'my-article', 'text', 'text');
```

### content_set_range

Updates a specific range of text within a content item.

**Purpose:** Modifies content data and persists changes to the database.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$content` | `object` | The content object context. |
| `$index` | `string` | The unique identifier of the content. |
| `$range` | `string` | The specific range or key to update. |
| `$type` | `string` | The action type (e.g., "#paste", "#swap", "#clear"). |
| `$text` | `string` | The new text or value to set. |

**Return Values:**

*   `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1.  Validates content is enabled.
2.  Loads document from cache or DB if not present.
3.  Switches on `$type` to determine the operation:
    *   `#paste`: Injects text from a buffer.
    *   `#swap`, `#dragdrop1`: Swaps text.
    *   `#copy`, `#dragdrop2`: Copies text.
    *   `#kick1`, `#kick2`: Removes text.
    *   `#drop1`, `#drop2`: Drops text (shifts).
    *   `#clear`: Deletes text.
    *   `shift`: Shifts text.
    *   Default: Sets text directly.
4.  Updates the database with the new `buffer_text`.
5.  Refreshes extra fields.

**Usage Example:**
```php
// Replace the first paragraph of content 'my-article' with new text
content_set_range($content, 'my-article', 'text', '#clear', '');
```

### content_parse

Parses and renders a content item using a template.

**Purpose:** Generates the final HTML output for a content page, utilizing a sophisticated caching layer.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$content` | `object` | The content object context. |
| `$index` | `string` | The unique identifier of the content. |
| `$action` | `array` | Array of action parameters (e.g., `[CMS_TEMPLATE_ACTION => 'search']`). |
| `$header` | `array` | Header parameters. |
| `$is_dynamic` | `bool` | Reference to flag if content is dynamic (not cached). |
| `$mod_time` | `int` | Reference to modification time. |

**Return Values:**

*   `string`: The rendered HTML output.

**Inner Mechanisms:**
1.  Checks content enabled status.
2.  Loads `document` and `template` libraries.
3.  Generates a unique hash key based on index, flags, user, language, and request parameters.
4.  **Cache Check:**
    *   Checks if a cache file exists.
    *   Checks if the cache file is newer than the database entry and system files.
    *   If valid, reads from file and returns processed output.
5.  **Cache Miss:**
    *   Instantiates `document` and `template`.
    *   Generates cache file content using `template->create_cache`.
    *   Writes modification time and cache content to the file.
    *   Returns the processed output.

**Usage Example:**
```php
$content = new content($user);
$is_dynamic = false;
$output = content_parse($content, 'my-article', [], [], $is_dynamic, $mod_time);
echo $output;
```

### content_template_export

Exports a specific range of a template associated with content.

**Purpose:** Allows developers to extract template snippets for reuse or preview.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$content` | `object` | The content object context. |
| `$index` | `string` | The unique identifier of the content. |
| `$range` | `string` | The specific range to export (optional). |

**Return Values:**

*   `string` or `bool`: The exported template string or `FALSE` on failure.

**Usage Example:**
```php
$content = new content($user);
$template_snippet = content_template_export($content, 'my-article', 'header');
```

### content_template_select

Retrieves a list of available templates.

**Purpose:** Provides a dropdown list of templates for the CMS interface.

**Return Values:**

*   `array`: Associative array where keys are template names and values are internal keys.

**Usage Example:**
```php
$templates = content_template_select();
// Output: ['Default Template' => 'key1', 'Blog Layout' => 'key2']
```

### content_get_receiver

Determines which users can receive a specific content item.

**Purpose:** Used in content distribution workflows to show a list of potential recipients.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$content` | `object` | The content object context. |
| `$type` | `int` | The content type (e.g., `CMS_CONTENT_TYPE_ORIGINAL`). |
| `$status` | `int` | The content status. |

**Return Values:**

*   `array`: Associative array mapping user keys to `TRUE` if they have permission to receive.

**Inner Mechanisms:**
1.  Checks if the content can be duplicated or copied.
2.  Temporarily modifies `$content` properties (`writer`, `editor`, `publisher`) to simulate the permissions of other users.
3.  Iterates through the permission list.
4.  Checks if the user has the `CMS_CONTENT_ACTION_RECEIVE` permission for the specific type and status.
5.  Restores original `$content` properties.

**Usage Example:**
```php
$content = new content($user);
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_PUBLICATION);
```

## Class: content

### Overview

The `content` class manages the lifecycle of a content item, including CRUD operations, versioning, scheduling, and permission enforcement.

### Properties

| Name | Type | Description |
| :--- | :--- | :--- |
| `$action` | `array` | Permission matrix defining allowed actions based on type, status, and role. |
| `$user` | `string` | The current user identifier. |
| `$writer` | `bool` | Permission flag for writer role. |
| `$editor` | `bool` | Permission flag for editor role. |
| `$publisher` | `bool` | Permission flag for publisher role. |
| `$operator` | `bool` | Permission flag for operator role. |
| `$enabled` | `bool` | Whether the class is initialized and database tables are verified. |

### Methods

#### __construct

Initializes the content manager, verifies database tables, and loads scheduled tasks.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$user` | `string` | The user identifier. Defaults to `CMS_SUPERUSER` if empty. |

**Return Values:**

*   `void`

**Inner Mechanisms:**
1.  Sets up the permission matrix (`$action`).
2.  Connects to MySQL and verifies existence of `content`, `content_version`, and `content_schedule` tables.
3.  Checks for scheduled tasks (Apply, Retrieve, Publish, Withdraw) and executes them if the time has arrived.
4.  Loads user permissions based on the `cms_permission` function.

**Usage Example:**
```php
// Initialize for a specific user
$content = new content('john_doe');
```

#### create

Creates a new content item in draft status.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$title` | `string` | The title of the content. |
| `$template` | `string` | The template name to use. |
| `$comment` | `string` | Comment for the writer. |
| `$test` | `bool` | If `TRUE`, performs validation without saving. |

**Return Values:**

*   `int` or `bool`: The new content index on success, `FALSE` on failure.

**Usage Example:**
```php
$content = new content('john_doe');
$index = $content->create("My New Article", "blog_template", "Initial draft");
```

#### publish

Publishes content immediately or schedules it for a future date.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$index` | `string` | The content index. |
| `$comment` | `string` | Comment for the publisher. |
| `$time_publish` | `int` | Unix timestamp for scheduled publication (0 for immediate). |
| `$time_withdraw` | `int` | Unix timestamp for scheduled withdrawal (0 for never). |
| `$directory_index` | `string` | The directory key to link to. |
| `$directory_action` | `string` | Action: "replace", "insert", "append". |
| `$directory_title` | `string` | Custom title for the directory entry. |
| `$test` | `bool` | If `TRUE`, performs validation without saving. |

**Return Values:**

*   `int` or `bool`: Directory index on success, `TRUE` on immediate publish, `FALSE` on failure.

**Inner Mechanisms:**
1.  Checks permissions.
2.  If `$time_publish > time()`: Schedules the event in `content_schedule` table.
3.  If immediate:
    *   Updates status to `PUBLICATION`.
    *   Links content to the directory (if specified).
    *   Optionally schedules a withdrawal.

**Usage Example:**
```php
$content = new content('john_doe');
// Publish immediately
$content->publish('article_123', 'Published now', 0, 0, 'news', 'append', 'Latest News');
```

#### action

Checks if the current user has permission to perform a specific action on a content item.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$type` | `int` | Content type. |
| `$status` | `int` | Content status. |
| `$action` | `int` | Action to perform. |

**Return Values:**

*   `bool`: `TRUE` if permitted, `FALSE` otherwise.

**Inner Mechanisms:**
1.  Looks up the role bitmask from the `$action` matrix.
2.  Checks roles in order of precedence: `writer` -> `editor` -> `publisher`.
3.  Uses bitwise AND (`&`) to check if the user's role flags match the required role flags.

**Usage Example:**
```php
$content = new content('john_doe');
// Check if user can publish the current content
if ($content->action($content->type, $content->status, CMS_CONTENT_ACTION_PUBLISH)) {
    echo "You can publish this.";
}
```

#### version_retrieve

Retrieves a specific version of content and applies it to the active content.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$version_index` | `int` | The ID of the version in the `content_version` table. |
| `$time` | `int` | Timestamp for scheduled retrieval. |
| `$apply` | `bool` | If `TRUE`, applies the version immediately. |
| `$test` | `bool` | If `TRUE`, performs validation without saving. |

**Return Values:**

*   `bool`: `TRUE` on success.

**Usage Example:**
```php
$content = new content('john_doe');
// Retrieve version 42 immediately
$content->version_retrieve(42, 0, TRUE);
```

#### step_store, step_undo, step_redo

Manages a 10-step undo/redo buffer for content editing.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$index` | `string` | The content index. |
| `$test` | `bool` | If `TRUE`, performs validation without saving. |

**Return Values:**

*   `bool`: `TRUE` on success.

**Inner Mechanisms:**
1.  Stores the current `buffer_text` in a circular buffer (RAM cache) based on a position index.
2.  `step_undo`: Restores the previous state from the buffer.
3.  `step_redo`: Restores the next state from the buffer.

**Usage Example:**
```php
$content = new content('john_doe');
$content->step_store('article_123'); // Save current state
// ... make edits ...
$content->step_undo('article_123'); // Undo last edit
```

#### delete

Permanently deletes a content item and its associated versions and schedules.

**Parameters:**

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `$index` | `string` | The content index. |
| `$ignore_directory` | `bool` | If `TRUE`, does not unlink from directory. |
| `$override_owner` | `bool` | If `TRUE`, allows deletion by non-owners. |
| `$test` | `bool` | If `TRUE`, performs validation without saving. |

**Return Values:**

*   `bool`: `TRUE` on success.

**Usage Example:**
```php
$content = new content('john_doe');
$content->delete('article_123', FALSE, FALSE, TRUE); // Test deletion
```


<!-- HASH:bd8ed61501fbf3708833e20b838b31d5 -->
