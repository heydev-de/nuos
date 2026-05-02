# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.comment.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Comment Module (`mod.comment.inc`)

This file implements the **Comment Module** for the NUOS web platform, providing functionality for user-generated comments on content. It handles:
- **Comment display** (paginated, with ratings)
- **Comment submission** (with spam detection and CAPTCHA)
- **Comment moderation** (edit, enable/disable, delete, mark as spam)
- **Email notifications** (for new comments and approvals)
- **User permissions** (reader, writer, operator)

The module integrates with the NUOS core utilities for database operations, URL generation, and security.

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

The `comment` class (defined in `comment.lib.php`) encapsulates all comment-related operations. This file initializes an instance and delegates actions to it.

#### Key Properties

| Property | Type | Description |
|----------|------|-------------|
| `$instance` | `string` | Content identifier (e.g., `CMS_CONTENT_INDEX`). |
| `$enabled` | `bool` | Whether the comment module is enabled for this instance. |
| `$reader` | `bool` | User has read permission. |
| `$writer` | `bool` | User has write permission. |
| `$operator` | `bool` | User has moderation permission. |
| `$default_status` | `int` | Default status for new comments (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_INACTIVE`). |
| `$spam_threshold` | `int` | Spam probability threshold (0–100). |

---

### Workflow

#### 1. **Initialization**
- Loads the `comment` library and checks if the module is enabled.
- Validates user permissions (`reader`, `writer`, or `operator`).
- Sets default configuration (status, spam threshold, CAPTCHA).

#### 2. **Action Handling**
Processes user actions via `$comment_message`:
- **`add`**: Validates and submits a new comment.
- **`rate_good`/`rate_bad`**: Updates comment ratings.
- **`_edit`/`edit`**: Edits an existing comment.
- **`enable`/`disable`/`delete`/`spam`**: Moderation actions.

#### 3. **Display**
- Renders comments (paginated, with ratings and timestamps).
- Shows forms for adding/editing comments (with CAPTCHA if enabled).
- Displays success/error messages.

---

### Functions/Methods

#### `comment->add()`
**Purpose**: Submits a new comment to the database.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Commenter's name. |
| `$email` | `string` | Commenter's email (validated). |
| `$url` | `string` | Commenter's homepage URL (optional). |
| `$text` | `string` | Comment text. |

**Return Values**:
| Value | Type | Description |
|-------|------|-------------|
| `string` | Comment index (success). |
| `-1` | Spam detected. |
| `-2` | Duplicate comment. |
| `FALSE` | Database error. |

**Inner Mechanisms**:
- Validates input (non-empty fields, valid email).
- Checks for spam using `$spam_threshold`.
- Prevents duplicate submissions.
- Sends email notifications if configured.

**Usage**:
```php
$index = $comment->add("User", "user@example.com", "https://example.com", "Comment text");
if ($index === -1) { /* Handle spam */ }
```

---

#### `comment->edit()`
**Purpose**: Updates an existing comment.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment identifier. |
| `$name` | `string` | Updated name. |
| `$email` | `string` | Updated email. |
| `$url` | `string` | Updated URL. |
| `$text` | `string` | Updated text. |

**Return Values**:
| Value | Type | Description |
|-------|------|-------------|
| `TRUE` | Success. |
| `FALSE` | Failure. |

**Usage**:
```php
if ($comment->edit($index, "New Name", "new@example.com", "", "Updated text")) {
    echo "Comment updated!";
}
```

---

#### `comment->status()`
**Purpose**: Changes a comment's status (active/hidden).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment identifier. |
| `$status` | `int` | `CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_HIDDEN`. |

**Return Values**:
| Value | Type | Description |
|-------|------|-------------|
| `TRUE` | Success. |
| `FALSE` | Failure. |

**Usage**:
```php
$comment->status($index, CMS_DB_COMMENT_STATUS_ACTIVE);
```

---

#### `comment->delete()`
**Purpose**: Deletes a comment (or marks it as spam).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment identifier. |
| `$spam` | `bool` | If `TRUE`, marks as spam (default: `FALSE`). |

**Return Values**:
| Value | Type | Description |
|-------|------|-------------|
| `TRUE` | Success. |
| `FALSE` | Failure. |

**Usage**:
```php
$comment->delete($index, TRUE); // Mark as spam
```

---

#### `comment->rate_good()` / `comment->rate_bad()`
**Purpose**: Updates a comment's rating.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment identifier. |

**Return Values**:
| Value | Type | Description |
|-------|------|-------------|
| `TRUE` | Success. |
| `FALSE` | Failure. |

**Inner Mechanisms**:
- Tracks user IPs to prevent duplicate votes.
- Updates `rating_value` and `rating_count` in the database.

**Usage**:
```php
$comment->rate_good($index);
```

---

### Key Utilities

#### `pagination()`
**Purpose**: Renders pagination controls.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | Base URL with `%page%` placeholder. |
| `$page` | `int` | Current page. |
| `$count` | `int` | Total pages. |
| `$label` | `string` | Accessibility label. |
| `$class` | `string` | CSS class. |

**Usage**:
```php
pagination(u(["comment_page" => "%page%"]), $page, $_count, "Comments", "comment-pagination");
```

---

#### `smtp_send()`
**Purpose**: Sends email notifications (requires `smtp` library).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$to` | `string` | Recipient email. |
| `$subject` | `string` | Email subject. |
| `$body` | `string` | Email body. |

**Usage**:
```php
smtp_send($email, "New Comment", "A new comment requires approval.");
```

---

### Security Considerations
1. **Input Validation**:
   - All form inputs are stripped of extra spaces (`stripspaces()`).
   - Emails are validated with `verify_email()`.
   - URLs are normalized to start with `https://`.

2. **Escaping**:
   - Database values: `sqlesc()`.
   - HTML output: `x()` (XML escaping).
   - URLs: `u()` and `cms_url()`.

3. **CSRF Protection**:
   - Handled by `cms_param()` and `cms_url()`.

4. **Spam Prevention**:
   - CAPTCHA (if enabled).
   - Spam probability threshold (`$spam_threshold`).

---

### Typical Usage Scenarios

#### 1. **Displaying Comments**
```php
// Initialize the module
$comment = new comment(CMS_CONTENT_INDEX);
if ($comment->enabled && $comment->reader) {
    // Render comments
    $comment->display();
}
```

#### 2. **Adding a Comment**
```php
if ($comment->writer) {
    $index = $comment->add(
        $_POST["comment_add_name"],
        $_POST["comment_add_email"],
        $_POST["comment_add_url"],
        $_POST["comment_add_text"]
    );
    if ($index === -1) {
        echo "Spam detected!";
    }
}
```

#### 3. **Moderating Comments**
```php
if ($comment->operator) {
    $comment->status($index, CMS_DB_COMMENT_STATUS_ACTIVE); // Approve
    $comment->delete($index); // Delete
}
```

---

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is visible. |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment requires approval. |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `-1` | Comment is hidden. |
| `CMS_COMMENT_PERMISSION_READER` | `"comment.reader"` | Read permission key. |
| `CMS_COMMENT_PERMISSION_WRITER` | `"comment.writer"` | Write permission key. |
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"comment.operator"` | Moderation permission key. |

---

### Error Messages

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_COMMENT_001` | "Name is required." |
| `CMS_L_MOD_COMMENT_002` | "Invalid email address." |
| `CMS_L_MOD_COMMENT_004` | "Comment text is required." |
| `CMS_L_MOD_COMMENT_022` | "Failed to add comment." |
| `CMS_L_MOD_COMMENT_024` | "Action failed." |
| `CMS_L_MOD_COMMENT_038` | "Comment marked as spam." |
| `CMS_L_MOD_COMMENT_040` | "Duplicate comment detected." |
| `CMS_L_MOD_COMMENT_045` | "Invalid CAPTCHA code." |


<!-- HASH:83e1b193edadcd6c60d59dbc8c397a2f -->
