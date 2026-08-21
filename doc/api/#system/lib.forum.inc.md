# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.forum.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Module (`lib.forum.inc`)

The **Forum Module** provides a lightweight, hierarchical forum system with permission-based access control, post management, and full-text search capabilities. It integrates with PWNC's core utilities for database operations, user permissions, and logging.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_FORUM_PERMISSION_OPERATOR` | `"operator"` | Permission level for forum administrators (full control). |
| `CMS_FORUM_PERMISSION_WRITER` | `"writer"` | Permission level for users who can create/edit posts. |
| `CMS_FORUM_PERMISSION_READER` | `"reader"` | Permission level for users who can read posts. |
| `CMS_DB_FORUM` | `CMS_DB_PREFIX . "forum"` | Base table name for forum data. |
| `CMS_DB_FORUM_INDEX` | `"id"` | Primary key column (post ID). |
| `CMS_DB_FORUM_CONTAINER` | `"container"` | Parent post ID (0 for root posts). |
| `CMS_DB_FORUM_USER` | `"user"` | Author username. |
| `CMS_DB_FORUM_TITLE` | `"title"` | Post title. |
| `CMS_DB_FORUM_TEXT` | `"text"` | Post content. |
| `CMS_DB_FORUM_EMAIL` | `"email"` | Boolean flag for email notifications. |
| `CMS_DB_FORUM_TIME` | `"time"` | Unix timestamp of post creation. |
| `CMS_DB_FORUM_ACCESS` | `"access"` | Counter for post views. |

---

## Utility Function: `forum_quote()`

### Purpose
Transforms plain text into a quoted format (e.g., for forum replies), prefixing each line with `>` and handling word-wrapping for long lines.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to be quoted. |

### Return Value
`string` – Quoted text with `>` prefixes.

### Inner Mechanisms
1. Splits input into lines using `\n` or `\r\n`.
2. Preserves existing `>` prefixes (e.g., nested quotes).
3. Uses `utf8_wordwrap()` to wrap long lines (multibyte-safe).
4. Joins lines with `\n`.

### Usage Example
```php
$quoted = forum_quote("Hello\nThis is a long line that will be wrapped automatically.");
echo $quoted;
// Output:
// > Hello
// > This is a long line that will be
// > wrapped automatically.
```

---

## Class: `forum`

### Overview
Manages forum posts, permissions, and operations (create, edit, move, delete, search). Supports hierarchical threading and full-text search with relevance scoring.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string|null` | Forum instance identifier (appended to table name). |
| `$table` | `string` | Database table name (e.g., `cms_forum_blog`). |
| `$mysql` | `mysql` | Database connection wrapper. |
| `$operator` | `bool` | `TRUE` if current user has operator permissions. |
| `$writer` | `bool` | `TRUE` if current user can write posts. |
| `$reader` | `bool` | `TRUE` if current user can read posts. |
| `$enabled` | `bool` | `TRUE` if forum is initialized and accessible. |

---

### Constructor: `__construct()`

#### Purpose
Initializes the forum, creates the database table if needed, and sets up permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string|null` | Optional forum instance (e.g., `"blog"`). |

#### Inner Mechanisms
1. Appends `$instance` to the table name (e.g., `cms_forum_blog`).
2. Creates the table with columns for posts, full-text indexes, and an `ngram` parser (if MySQL ≥ 5.7.6).
3. Checks user permissions using `cms_permission()`.

#### Usage Example
```php
$forum = new forum("support"); // Creates/uses table `cms_forum_support`
```

---

### Method: `add()`

#### Purpose
Creates a new forum post. Notifies the parent post's author via email if enabled.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Parent post ID (0 for root posts). |
| `$title` | `string` | Post title. |
| `$text` | `string` | Post content. |
| `$email` | `bool` | `TRUE` to enable email notifications. |
| `$test` | `bool` | `TRUE` to validate without saving. |

#### Return Value
- `int` – New post ID on success.
- `bool` – `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions (`$writer` or `$operator`).
2. Inserts post into the database with current timestamp.
3. Logs the action via `log()->access()`.
4. Sends email notifications if `$email=TRUE` and the parent post has notifications enabled.

#### Usage Example
```php
$post_id = $forum->add(0, "Welcome", "Hello world!", TRUE);
// Creates a root post with email notifications disabled.
```

---

### Method: `test_add()`

#### Purpose
Validates if a post can be added without saving it.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Parent post ID. |

#### Return Value
`bool` – `TRUE` if the post can be added.

#### Usage Example
```php
if ($forum->test_add(0)) {
    echo "You can create a root post.";
}
```

---

### Method: `edit()`

#### Purpose
Updates an existing post. Restricts edits to the post author (unless operator).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to edit. |
| `$title` | `string` | New title. |
| `$text` | `string` | New content. |
| `$email` | `bool` | New email notification setting. |
| `$test` | `bool` | `TRUE` to validate without saving. |

#### Return Value
`bool` – `TRUE` on success.

#### Inner Mechanisms
1. Checks if the user is the post author or an operator.
2. Allows anonymous users to edit their own posts (if `CMS_USER="anonymous"`).

#### Usage Example
```php
$forum->edit(123, "Updated Title", "New content", FALSE);
```

---

### Method: `test_edit()`

#### Purpose
Validates if a post can be edited without saving changes.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID. |

#### Return Value
`bool` – `TRUE` if editable.

#### Usage Example
```php
if ($forum->test_edit(123)) {
    echo "Post 123 can be edited.";
}
```

---

### Method: `move()`

#### Purpose
Moves a post to a new parent. Prevents circular references.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to move. |
| `$parent` | `int` | New parent post ID. |
| `$test` | `bool` | `TRUE` to validate without moving. |

#### Return Value
`bool` – `TRUE` on success.

#### Inner Mechanisms
1. Validates operator permissions.
2. Checks for circular references using `mysql->is_child()`.
3. Updates the `container` field.

#### Usage Example
```php
$forum->move(123, 456); // Moves post 123 under post 456
```

---

### Method: `test_move()`

#### Purpose
Validates if a post can be moved without executing the move.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID. |
| `$parent` | `int` | Target parent ID. |

#### Return Value
`bool` – `TRUE` if movable.

#### Usage Example
```php
if ($forum->test_move(123, 456)) {
    echo "Post 123 can be moved under 456.";
}
```

---

### Method: `delete()`

#### Purpose
Deletes a post and its children (via `mysql->delete()`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to delete. |
| `$test` | `bool` | `TRUE` to validate without deleting. |

#### Return Value
`bool` – `TRUE` on success.

#### Usage Example
```php
$forum->delete(123);
```

---

### Method: `test_delete()`

#### Purpose
Validates if a post can be deleted without executing the deletion.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID. |

#### Return Value
`bool` – `TRUE` if deletable.

#### Usage Example
```php
if ($forum->test_delete(123)) {
    echo "Post 123 can be deleted.";
}
```

---

### Method: `search()`

#### Purpose
Performs a full-text search across post titles and content, returning results sorted by relevance.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Search query. |

#### Return Value
- `resource` – MySQL result set with columns: `*`, `relevance`.
- `bool` – `FALSE` on failure.

#### Inner Mechanisms
1. Uses `NATURAL LANGUAGE MODE` and `BOOLEAN MODE` for scoring.
2. Weights title matches higher than content matches.
3. Logs the search via `log()->access()`.

#### Usage Example
```php
$result = $forum->search("installation guide");
while ($row = mysql_fetch_assoc($result)) {
    echo "Post {$row['id']}: {$row['title']} (Relevance: {$row['relevance']})";
}
```

---

### Method: `reply_count()`

#### Purpose
Counts replies for one or more posts.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int|array` | Post ID(s) to count replies for. |

#### Return Value
`array` – Associative array of `[post_id => count]`.

#### Usage Example
```php
$counts = $forum->reply_count([123, 456]);
echo "Post 123 has {$counts[123]} replies.";
```

---

### Method: `log_access()`

#### Purpose
Increments the view counter for a post.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID. |

#### Usage Example
```php
$forum->log_access(123); // Increments view count for post 123
```


<!-- HASH:9376720ba5976dca1b331a99de2c2f47 -->
