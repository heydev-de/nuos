# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.comment.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Comment Class

The `comment` class provides a comprehensive system for managing user comments within the NUOS platform. It handles comment creation, modification, status management, rating, and deletion, while integrating with spam detection and user permission systems.

---

### Constants

#### Permission Levels
| Name | Value | Description |
|------|-------|-------------|
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"operator"` | Permission to manage (edit/delete) comments |
| `CMS_COMMENT_PERMISSION_WRITER` | `"writer"` | Permission to post comments |
| `CMS_COMMENT_PERMISSION_READER` | `"reader"` | Permission to view and rate comments |

#### Status Values
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment is inactive (not visible) |
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is active (visible) |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `2` | Comment is hidden (visible only to operators) |

#### Database Schema
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_COMMENT` | `CMS_DB_PREFIX . "comment"` | Table name for comments |
| `CMS_DB_COMMENT_INDEX` | `"id"` | Primary key |
| `CMS_DB_COMMENT_INSTANCE` | `"instance"` | Instance identifier (e.g., module or content ID) |
| `CMS_DB_COMMENT_STATUS` | `"status"` | Comment status (active/inactive/hidden) |
| `CMS_DB_COMMENT_TIME` | `"time"` | Timestamp of comment creation |
| `CMS_DB_COMMENT_NAME` | `"name"` | Author name |
| `CMS_DB_COMMENT_EMAIL` | `"email"` | Author email |
| `CMS_DB_COMMENT_URL` | `"url"` | Author URL |
| `CMS_DB_COMMENT_TEXT` | `"text"` | Comment content |
| `CMS_DB_COMMENT_HASH` | `"hash"` | MD5 hash of comment text (for duplicate detection) |
| `CMS_DB_COMMENT_RATING_VALUE` | `"rating_value"` | Sum of ratings (positive/negative) |
| `CMS_DB_COMMENT_RATING_COUNT` | `"rating_count"` | Total number of ratings |
| `CMS_DB_COMMENT_RATING_USERID` | `"rating_userid"` | List of user IDs who rated the comment (prevents duplicate ratings) |
| `CMS_DB_COMMENT_SPAM_PROBABILITY` | `"spam_probability"` | Spam probability score (0-100) |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Instance identifier (e.g., module or content ID) |
| `$enabled` | `FALSE` | Whether the comment system is enabled for this instance |
| `$operator` | `FALSE` | Whether the current user has operator permissions |
| `$writer` | `FALSE` | Whether the current user has writer permissions |
| `$reader` | `FALSE` | Whether the current user has reader permissions |
| `$default_status` | `CMS_DB_COMMENT_STATUS_INACTIVE` | Default status for new comments |
| `$spam_threshold` | `95` | Spam probability threshold (0-100) for rejection |

---

### `__construct($instance = "")`

#### Purpose
Initializes the comment system for a specific instance, verifies database table structure, and checks user permissions.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Instance identifier (e.g., module or content ID) |

#### Return Values
- **`void`**: No explicit return value. Sets object properties (`$enabled`, `$operator`, `$writer`, `$reader`).

#### Inner Mechanisms
1. **Database Verification**: Checks if the `comment` table exists and matches the expected schema. Creates the table if necessary.
2. **Permission Check**: Uses `cms_permission()` to verify the current user's permissions for the given instance.
3. **Spam Filter Integration**: Loads the `category` module (if available) for spam detection.

#### Usage
```php
$comments = new comment("article_123");
if ($comments->enabled) {
    // Comment system is ready for use
}
```

---

### `add($name, $email, $url, $text)`

#### Purpose
Adds a new comment to the database after validating permissions, checking for duplicates, and evaluating spam probability.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Author name |
| `$email` | `string` | Author email |
| `$url` | `string` | Author URL |
| `$text` | `string` | Comment content |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Comment ID on success |
| `FALSE` | If the comment system is disabled or the user lacks permissions |
| `-1` | If the comment is flagged as spam |
| `-2` | If a duplicate comment is detected |

#### Inner Mechanisms
1. **Permission Check**: Validates `$this->writer` and `$this->enabled`.
2. **Spam Detection**: Uses the `category` module to evaluate spam probability. Rejects comments exceeding `$this->spam_threshold`.
3. **Duplicate Check**: Searches for identical comments (same hash) posted within the last hour.
4. **Database Insertion**: Stores the comment with a default status (`$this->default_status`).
5. **Spam Training**: If the comment is active by default, trains the spam filter to recognize it as non-spam.
6. **Logging**: Records the action in the system log.

#### Usage
```php
$comment_id = $comments->add(
    "John Doe",
    "john@example.com",
    "https://example.com",
    "This is a great article!"
);
if ($comment_id > 0) {
    echo "Comment posted successfully!";
}
```

---

### `edit($index, $name, $email, $url, $text)`

#### Purpose
Updates an existing comment's metadata and content. Requires operator permissions.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$name` | `string` | Updated author name |
| `$email` | `string` | Updated author email |
| `$url` | `string` | Updated author URL |
| `$text` | `string` | Updated comment content |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure (disabled system, lack of permissions, or database error) |

#### Inner Mechanisms
1. **Permission Check**: Validates `$this->operator` and `$this->enabled`.
2. **Status Check**: Retrieves the current comment status and text.
3. **Spam Training Correction**: If the comment was previously active, retrains the spam filter to reflect the updated text.
4. **Database Update**: Updates the comment record with new values.

#### Usage
```php
if ($comments->edit(42, "Jane Doe", "jane@example.com", "", "Updated comment text")) {
    echo "Comment updated successfully!";
}
```

---

### `status($index, $status)`

#### Purpose
Changes the status of a comment (e.g., from inactive to active). Requires operator permissions.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$status` | `int` | New status (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_HIDDEN`) |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success or if the status is already set |
| `FALSE` | On failure (disabled system, lack of permissions, or database error) |

#### Inner Mechanisms
1. **Permission Check**: Validates `$this->operator` and `$this->enabled`.
2. **Status Validation**: Rejects invalid status values (e.g., `CMS_DB_COMMENT_STATUS_INACTIVE`).
3. **Spam Training**:
   - If hiding an active comment, retrains the spam filter to undo its "non-spam" status.
   - If activating a comment, trains the spam filter to recognize it as non-spam.
4. **Database Update**: Updates the comment's status.

#### Usage
```php
if ($comments->status(42, CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment is now visible!";
}
```

---

### `rate_good($index, $invert = FALSE)`

#### Purpose
Records a positive (or negative, if `$invert = TRUE`) rating for a comment. Prevents duplicate ratings from the same user.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$invert` | `bool` | If `TRUE`, records a negative rating |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure (disabled system, lack of permissions, or duplicate rating) |

#### Inner Mechanisms
1. **Permission Check**: Validates `$this->reader` and `$this->enabled`.
2. **User ID Tracking**: Uses `CMS_IPHASH` to generate a unique user identifier and checks for prior ratings.
3. **Database Update**: Increments the rating value and count, and appends the user ID to the `rating_userid` field.

#### Usage
```php
if ($comments->rate_good(42)) {
    echo "Thanks for your feedback!";
}
```

---

### `rate_bad($index)`

#### Purpose
Records a negative rating for a comment. Wrapper for `rate_good($index, TRUE)`.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure |

#### Usage
```php
$comments->rate_bad(42);
```

---

### `delete($index, $spam = FALSE)`

#### Purpose
Deletes a comment from the database. Optionally trains the spam filter if the comment is marked as spam.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$spam` | `bool` | If `TRUE`, trains the spam filter to recognize the comment as spam |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure (disabled system, lack of permissions, or database error) |

#### Inner Mechanisms
1. **Permission Check**: Validates `$this->operator` and `$this->enabled`.
2. **Spam Training**:
   - If `$spam = TRUE` and the comment was active, retrains the spam filter to undo its "non-spam" status.
   - Trains the spam filter to recognize the comment as spam.
3. **Database Deletion**: Removes the comment record.

#### Usage
```php
if ($comments->delete(42, TRUE)) {
    echo "Comment deleted and marked as spam!";
}
```


<!-- HASH:7fb838068b28ccf3f9de2a364d6e7771 -->
