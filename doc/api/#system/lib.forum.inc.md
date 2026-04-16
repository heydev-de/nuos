# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.forum.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Forum Module (`lib.forum.inc`)

The `lib.forum.inc` file provides a **Forum** class for managing discussion forums within the NUOS platform. It handles forum post creation, editing, moving, deletion, and searching, along with permission-based access control. The module integrates with the platform's database, logging, and notification systems.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_FORUM_PERMISSION_OPERATOR` | `"operator"` | Permission level for forum operators (full control). |
| `CMS_FORUM_PERMISSION_WRITER` | `"writer"` | Permission level for forum writers (create/edit posts). |
| `CMS_FORUM_PERMISSION_READER` | `"reader"` | Permission level for forum readers (view-only). |
| `CMS_DB_FORUM` | `CMS_DB_PREFIX . "forum"` | Base name of the forum database table. |
| `CMS_DB_FORUM_INDEX` | `"id"` | Primary key column for forum posts. |
| `CMS_DB_FORUM_CONTAINER` | `"container"` | Parent post ID (0 for root posts). |
| `CMS_DB_FORUM_USER` | `"user"` | User identifier of the post author. |
| `CMS_DB_FORUM_TITLE` | `"title"` | Title of the forum post. |
| `CMS_DB_FORUM_TEXT` | `"text"` | Content of the forum post. |
| `CMS_DB_FORUM_EMAIL` | `"email"` | Boolean flag for email notifications. |
| `CMS_DB_FORUM_TIME` | `"time"` | Unix timestamp of post creation. |
| `CMS_DB_FORUM_ACCESS` | `"access"` | Counter for post views. |

---

## Utility Function

### `forum_quote($text)`

**Purpose:**
Formats a given text block into a quoted reply format (e.g., for forum responses). Prepends `>` to each line, preserving existing quote levels.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to be quoted. |

**Return Values:**
- `string`: The quoted text, with each line prefixed by `>`.

**Inner Mechanisms:**
1. Splits input into lines using `preg_split`.
2. For lines already starting with `>`, preserves the existing quote level.
3. For other lines, wraps text to 75 characters (via `utf8_wordwrap`) and prepends `>` to each wrapped segment.

**Usage Context:**
- Used when generating quoted replies in forum threads.
- Example:
  ```php
  $quoted = forum_quote("Hello\n> Original quote\nWorld");
  // Output: "> Hello\n>> Original quote\n> World"
  ```

---

## Class: `forum`

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string\|null` | Forum instance identifier (appended to table name). |
| `$table` | `string` | Full database table name for the forum. |
| `$mysql` | `mysql` | Database connection handler. |
| `$operator` | `bool` | `TRUE` if current user has operator permissions. |
| `$writer` | `bool` | `TRUE` if current user has writer permissions. |
| `$reader` | `bool` | `TRUE` if current user has reader permissions. |
| `$enabled` | `bool` | `TRUE` if the forum is initialized and accessible. |

---

### `__construct($instance = NULL)`

**Purpose:**
Initializes a forum instance, creating the database table if necessary, and setting up permissions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string\|null` | Optional forum instance name (e.g., `"support"`). |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
1. Initializes a `mysql` connection.
2. Appends `$instance` to the table name if provided.
3. Checks for MySQL version ≥ 5.7.6 to enable `ngram` full-text parser.
4. Verifies or creates the forum table with columns and full-text indexes.
5. Sets permission flags via `cms_permission()`.

**Usage Context:**
- Called when a forum is first accessed or instantiated.
- Example:
  ```php
  $forum = new forum("support");
  ```

---

### `add($index, $title, $text, $email, $test = FALSE)`

**Purpose:**
Creates a new forum post, optionally as a reply to an existing post (`$index`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Parent post ID (0 for root posts). |
| `$title` | `string` | Post title. |
| `$text` | `string` | Post content. |
| `$email` | `bool` | `TRUE` to enable email notifications for replies. |
| `$test` | `bool` | If `TRUE`, performs a dry run (no database changes). |

**Return Values:**
- `int\|bool`: The new post ID on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates permissions (`$this->writer` or `$this->operator`).
2. Inserts the post into the database with current timestamp.
3. Logs the action via the `log` class.
4. Sends email notifications to the parent post author if enabled.

**Usage Context:**
- Used when a user submits a new post or reply.
- Example:
  ```php
  $post_id = $forum->add(0, "Help", "How do I...", TRUE);
  ```

---

### `test_add($index)`

**Purpose:**
Tests whether a post can be added (dry run).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Parent post ID to test. |

**Return Values:**
- `bool`: `TRUE` if the post can be added, `FALSE` otherwise.

**Usage Context:**
- Used for permission checks before rendering UI forms.

---

### `edit($index, $title, $text, $email, $test = FALSE)`

**Purpose:**
Updates an existing forum post.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to edit. |
| `$title` | `string` | New post title. |
| `$text` | `string` | New post content. |
| `$email` | `bool` | New email notification setting. |
| `$test` | `bool` | If `TRUE`, performs a dry run. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates permissions (`$this->operator` or post ownership for anonymous users).
2. Updates the post in the database.

**Usage Context:**
- Used when a user edits their post.
- Example:
  ```php
  $success = $forum->edit(123, "Updated Title", "New content", FALSE);
  ```

---

### `test_edit($index)`

**Purpose:**
Tests whether a post can be edited (dry run).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to test. |

**Return Values:**
- `bool`: `TRUE` if editable, `FALSE` otherwise.

---

### `move($index, $parent, $test = FALSE)`

**Purpose:**
Moves a post to a new parent (replies follow).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to move. |
| `$parent` | `int` | New parent post ID. |
| `$test` | `bool` | If `TRUE`, performs a dry run. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates operator permissions.
2. Prevents circular references (e.g., moving a post into its own child).
3. Updates the post's `container` field.

**Usage Context:**
- Used by moderators to reorganize threads.

---

### `test_move($index, $parent)`

**Purpose:**
Tests whether a post can be moved (dry run).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to test. |
| `$parent` | `int` | Target parent ID. |

**Return Values:**
- `bool`: `TRUE` if movable, `FALSE` otherwise.

---

### `delete($index, $test = FALSE)`

**Purpose:**
Deletes a forum post and its replies (recursively).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to delete. |
| `$test` | `bool` | If `TRUE`, performs a dry run. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates operator permissions.
2. Uses `mysql->delete()` for recursive deletion.

**Usage Context:**
- Used by moderators to remove posts.

---

### `test_delete($index)`

**Purpose:**
Tests whether a post can be deleted (dry run).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to test. |

**Return Values:**
- `bool`: `TRUE` if deletable, `FALSE` otherwise.

---

### `search($value)`

**Purpose:**
Searches forum posts by keyword, ranking results by relevance.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Search query. |

**Return Values:**
- `resource\|bool`: MySQL result set on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Logs the search action.
2. Sanitizes input for boolean mode (appends `*` to words).
3. Uses `NATURAL LANGUAGE MODE` and `BOOLEAN MODE` for ranking.
4. Returns top 100 results ordered by relevance.

**Usage Context:**
- Used for forum search functionality.
- Example:
  ```php
  $results = $forum->search("error");
  ```

---

### `log_access($index)`

**Purpose:**
Increments the view counter for a post.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to log. |

**Return Values:**
- None.

**Usage Context:**
- Called when a post is viewed to track popularity.


<!-- HASH:da9f553470105c830667327f0e101133 -->
