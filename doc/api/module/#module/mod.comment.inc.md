# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.comment.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Comment Module (`mod.comment.inc`)

The **Comment Module** provides a complete commenting system for PWNC-based websites. It handles:
- **Comment submission** (with optional CAPTCHA)
- **Comment moderation** (approval, editing, deletion, spam marking)
- **Comment rating** (upvote/downvote)
- **Pagination** and **display ordering**
- **Email notifications** for new comments and moderation actions
- **Spam detection** (via probability threshold)

The module integrates with the PWNC permission system and supports anonymous users (with optional email verification).

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$comment_message` | `string` | Action trigger (e.g., `"add"`, `"edit"`, `"delete"`). |
| `$comment_index` | `string` | Unique identifier for a comment. |
| `$comment_page` | `int` | Current pagination page. |
| `$comment_add_*` | `string` | Form fields for adding a comment (name, email, URL, text, CAPTCHA). |
| `$comment_edit_*` | `string` | Form fields for editing a comment (name, email, URL, text). |

---

### Class: `comment`

The `comment` class encapsulates all comment-related operations, including database interactions, permission checks, and spam detection.

#### Properties

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Content instance identifier (e.g., blog post ID). |
| `$enabled` | `bool` | Whether the comment system is enabled for this instance. |
| `$reader` | `bool` | Whether the current user can read comments. |
| `$writer` | `bool` | Whether the current user can post comments. |
| `$operator` | `bool` | Whether the current user can moderate comments. |
| `$default_status` | `int` | Default status for new comments (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_INACTIVE`). |
| `$spam_threshold` | `int` | Spam probability threshold (0–100). Comments exceeding this are flagged as spam. |

---

### Methods

---

#### `comment::__construct(string $instance)`

**Purpose:**
Initializes the comment system for a specific content instance.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Content instance identifier (e.g., `"blog/123"`). |

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Sets the `$instance` property.
2. Checks if the comment system is enabled for this instance.
3. Validates user permissions (`reader`, `writer`, `operator`) via `permission()`.

**Usage Example:**
```php
$comment = new comment("blog/123");
if ($comment->enabled) {
    // Render comment section
}
```

---

#### `comment::add(string $name, string $email, string $url, string $text): int|bool`

**Purpose:**
Adds a new comment to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Commenter's name. |
| `$email` | `string` | Commenter's email (validated). |
| `$url` | `string` | Commenter's homepage URL (optional). |
| `$text` | `string` | Comment text. |

**Return Values:**
- `int`: Comment index on success.
- `-1`: Comment flagged as spam.
- `-2`: Duplicate comment detected.
- `false`: Database error.

**Inner Mechanisms:**
1. Validates input (non-empty fields, valid email).
2. Checks for duplicate comments (same name/email/text within 5 minutes).
3. Calculates spam probability (via Akismet or similar).
4. Inserts the comment into the database with the default status.
5. Updates the commenter's rating history.

**Usage Example:**
```php
$index = $comment->add(
    "John Doe",
    "john@example.com",
    "https://example.com",
    "This is a great post!"
);
if ($index === -1) {
    echo "Your comment was flagged as spam.";
}
```

---

#### `comment::edit(string $index, string $name, string $email, string $url, string $text): bool`

**Purpose:**
Updates an existing comment.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$name` | `string` | Updated name. |
| `$email` | `string` | Updated email. |
| `$url` | `string` | Updated URL. |
| `$text` | `string` | Updated text. |

**Return Values:**
- `true`: Success.
- `false`: Failure (invalid index or database error).

**Inner Mechanisms:**
1. Validates the comment index.
2. Updates the comment in the database.
3. Logs the edit action.

**Usage Example:**
```php
if ($comment->edit("123", "Jane Doe", "jane@example.com", "", "Updated text")) {
    echo "Comment updated successfully.";
}
```

---

#### `comment::delete(string $index, bool $spam = false): bool`

**Purpose:**
Deletes a comment (or marks it as spam).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$spam` | `bool` | If `true`, marks the comment as spam (default: `false`). |

**Return Values:**
- `true`: Success.
- `false`: Failure (invalid index or database error).

**Inner Mechanisms:**
1. Validates the comment index.
2. Deletes the comment or updates its spam status.
3. Logs the action.

**Usage Example:**
```php
if ($comment->delete("123", true)) {
    echo "Comment marked as spam and deleted.";
}
```

---

#### `comment::status(string $index, int $status): bool`

**Purpose:**
Updates a comment's status (active, inactive, hidden).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$status` | `int` | New status (`CMS_DB_COMMENT_STATUS_ACTIVE`, `CMS_DB_COMMENT_STATUS_INACTIVE`, or `CMS_DB_COMMENT_STATUS_HIDDEN`). |

**Return Values:**
- `true`: Success.
- `false`: Failure (invalid index or database error).

**Inner Mechanisms:**
1. Validates the comment index.
2. Updates the status in the database.

**Usage Example:**
```php
if ($comment->status("123", CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment approved.";
}
```

---

#### `comment::rate_good(string $index): bool`

**Purpose:**
Increments a comment's upvote count.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |

**Return Values:**
- `true`: Success.
- `false`: Failure (invalid index or user already voted).

**Inner Mechanisms:**
1. Checks if the user has already voted.
2. Updates the comment's rating value and count.
3. Records the user's vote in the database.

**Usage Example:**
```php
if ($comment->rate_good("123")) {
    echo "Upvoted!";
}
```

---

#### `comment::rate_bad(string $index): bool`

**Purpose:**
Increments a comment's downvote count.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |

**Return Values:**
- `true`: Success.
- `false`: Failure (invalid index or user already voted).

**Inner Mechanisms:**
1. Checks if the user has already voted.
2. Updates the comment's rating value and count.
3. Records the user's vote in the database.

**Usage Example:**
```php
if ($comment->rate_bad("123")) {
    echo "Downvoted!";
}
```

---

### Module Workflow

1. **Initialization:**
   - Loads the comment library.
   - Validates permissions (`reader`, `writer`, `operator`).
   - Sets default configuration (status, spam threshold, CAPTCHA).

2. **Action Handling:**
   - Processes form submissions (`add`, `edit`, `delete`, `rate`, etc.).
   - Validates input and checks for errors.
   - Updates the database and sends notifications.

3. **Display:**
   - Renders comments with pagination.
   - Shows rating bars and moderation controls (if permitted).
   - Displays forms for adding/editing comments.

---

### Usage Example: Rendering a Comment Section

```php
// Initialize the comment system for a blog post
$comment = new comment("blog/123");
if ($comment->enabled) {
    // Process actions (e.g., add, delete)
    include "module/#module/mod.comment.inc";

    // Display the comment section
    echo "<section id='comment'>";
    if ($comment->reader) {
        // Show comments and forms
    }
    echo "</section>";
}
```

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is visible. |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment requires approval. |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `-1` | Comment is hidden. |
| `CMS_COMMENT_PERMISSION_READER` | `"comment.reader"` | Permission to read comments. |
| `CMS_COMMENT_PERMISSION_WRITER` | `"comment.writer"` | Permission to post comments. |
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"comment.operator"` | Permission to moderate comments. |

---

### Error Messages

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_COMMENT_001` | "Name is required." |
| `CMS_L_MOD_COMMENT_002` | "Invalid email address." |
| `CMS_L_MOD_COMMENT_004` | "Comment text is required." |
| `CMS_L_MOD_COMMENT_022` | "Failed to add comment." |
| `CMS_L_MOD_COMMENT_038` | "Comment flagged as spam." |
| `CMS_L_MOD_COMMENT_040` | "Duplicate comment detected." |
| `CMS_L_MOD_COMMENT_045` | "CAPTCHA verification failed." |


<!-- HASH:cb54642679f9844441a81cf38505bdd0 -->
