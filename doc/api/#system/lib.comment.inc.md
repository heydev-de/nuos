# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.comment.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Comment Class

The `comment` class provides a comprehensive system for managing user comments within the PWNC Web Platform. It handles comment creation, modification, status management, rating, and deletion while integrating with spam detection mechanisms. The class enforces permission-based access control and maintains comment state in a MySQL database.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"operator"` | Permission level for full comment management |
| `CMS_COMMENT_PERMISSION_WRITER` | `"writer"` | Permission level for creating comments |
| `CMS_COMMENT_PERMISSION_READER` | `"reader"` | Permission level for reading and rating comments |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment is inactive (not visible) |
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is active (visible) |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `2` | Comment is hidden (visible only to operators) |
| `CMS_DB_COMMENT` | `CMS_DB_PREFIX . "comment"` | Database table name for comments |
| `CMS_DB_COMMENT_INDEX` | `"id"` | Primary key column |
| `CMS_DB_COMMENT_INSTANCE` | `"instance"` | Instance identifier column |
| `CMS_DB_COMMENT_STATUS` | `"status"` | Status column |
| `CMS_DB_COMMENT_TIME` | `"time"` | Timestamp column |
| `CMS_DB_COMMENT_NAME` | `"name"` | Author name column |
| `CMS_DB_COMMENT_EMAIL` | `"email"` | Author email column |
| `CMS_DB_COMMENT_URL` | `"url"` | Author URL column |
| `CMS_DB_COMMENT_TEXT` | `"text"` | Comment text column |
| `CMS_DB_COMMENT_HASH` | `"hash"` | MD5 hash of comment text (binary) |
| `CMS_DB_COMMENT_RATING_VALUE` | `"rating_value"` | Sum of all ratings (positive/negative) |
| `CMS_DB_COMMENT_RATING_COUNT` | `"rating_count"` | Total number of ratings |
| `CMS_DB_COMMENT_RATING_USERID` | `"rating_userid"` | List of user IDs who rated (prevents duplicate ratings) |
| `CMS_DB_COMMENT_SPAM_PROBABILITY` | `"spam_probability"` | Spam probability score (0-100) |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Instance identifier (e.g., `"blog.post.123"`) |
| `$enabled` | `FALSE` | Whether the comment system is enabled for this instance |
| `$operator` | `FALSE` | Whether the current user has operator permissions |
| `$writer` | `FALSE` | Whether the current user has writer permissions |
| `$reader` | `FALSE` | Whether the current user has reader permissions |
| `$default_status` | `CMS_DB_COMMENT_STATUS_INACTIVE` | Default status for new comments |
| `$spam_threshold` | `95` | Spam probability threshold (0-100) for rejection |

---

### Constructor: `__construct`

#### Purpose
Initializes the comment system for a given instance. Verifies the database table structure, sets up permissions, and enables the system if the table exists.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Instance identifier (e.g., `"blog.post.123"`). Defaults to `""`. |

#### Return Values
- **`void`**: No explicit return value. Sets object properties (`$enabled`, `$operator`, `$writer`, `$reader`).

#### Inner Mechanisms
1. Creates a `mysql` object to verify the database table structure.
2. Defines the table schema if it does not exist.
3. Sets permissions based on the current user's role for the given instance.
4. Enables the system if the table is verified.

#### Usage Example
```php
$comment_system = new comment("blog.post.123");
if ($comment_system->enabled) {
    // Comment system is ready for use
}
```

---

### Method: `add`

#### Purpose
Adds a new comment to the database. Validates permissions, checks for spam, and prevents duplicate comments within a 1-hour window.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Author name |
| `$email` | `string` | Author email |
| `$url` | `string` | Author URL (optional) |
| `$text` | `string` | Comment text |

#### Return Values
- **`int`**: Comment ID on success.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.
- **`-1`**: If the comment is flagged as spam.
- **`-2`**: If a duplicate comment is detected within 1 hour.

#### Inner Mechanisms
1. Checks if the system is enabled and the user has writer permissions.
2. Evaluates spam probability using the `category` class (if loaded).
3. Generates a 32-bit hash of the comment text to detect duplicates.
4. Inserts the comment into the database with the default status.
5. Automatically trains the spam filter if the comment is active by default.
6. Logs the action.

#### Usage Example
```php
$comment_id = $comment_system->add(
    "John Doe",
    "john@example.com",
    "https://example.com",
    "This is a great post!"
);
if ($comment_id > 0) {
    echo "Comment added successfully!";
} elseif ($comment_id === -1) {
    echo "Your comment was flagged as spam.";
}
```

---

### Method: `edit`

#### Purpose
Edits an existing comment. Validates operator permissions and updates the comment in the database. Adjusts spam filter training if the comment status changes.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$name` | `string` | Updated author name |
| `$email` | `string` | Updated author email |
| `$url` | `string` | Updated author URL |
| `$text` | `string` | Updated comment text |

#### Return Values
- **`TRUE`**: On success.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.

#### Inner Mechanisms
1. Checks if the system is enabled and the user has operator permissions.
2. Retrieves the current comment status and text.
3. Updates the comment in the database.
4. Adjusts spam filter training if the comment was previously active.

#### Usage Example
```php
if ($comment_system->edit(42, "Jane Doe", "jane@example.com", "", "Updated comment text.")) {
    echo "Comment updated successfully!";
}
```

---

### Method: `status`

#### Purpose
Changes the status of a comment (e.g., from inactive to active). Validates operator permissions and adjusts spam filter training accordingly.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$status` | `int` | New status (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_HIDDEN`) |

#### Return Values
- **`TRUE`**: On success or if the status is already set.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.

#### Inner Mechanisms
1. Checks if the system is enabled and the user has operator permissions.
2. Validates the new status (cannot be `CMS_DB_COMMENT_STATUS_INACTIVE`).
3. Retrieves the current comment status and text.
4. Updates the status in the database.
5. Adjusts spam filter training:
   - If hiding an active comment, undoes previous "nospam" training.
   - If activating a comment, trains it as "nospam".

#### Usage Example
```php
if ($comment_system->status(42, CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment is now visible!";
}
```

---

### Method: `rate_good`

#### Purpose
Increments the positive rating of a comment. Prevents duplicate ratings from the same user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$invert` | `bool` | If `TRUE`, decrements the rating (used for `rate_bad`). Defaults to `FALSE`. |

#### Return Values
- **`TRUE`**: On success.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.

#### Inner Mechanisms
1. Checks if the system is enabled and the user has reader permissions.
2. Updates the comment's rating value and count.
3. Records the user's IP hash to prevent duplicate ratings.

#### Usage Example
```php
if ($comment_system->rate_good(42)) {
    echo "Thanks for your feedback!";
}
```

---

### Method: `rate_bad`

#### Purpose
Decrements the rating of a comment (calls `rate_good` with `$invert = TRUE`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |

#### Return Values
- **`TRUE`**: On success.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.

#### Usage Example
```php
if ($comment_system->rate_bad(42)) {
    echo "Your feedback has been recorded.";
}
```

---

### Method: `delete`

#### Purpose
Deletes a comment from the database. Optionally trains the spam filter if the comment is marked as spam.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$spam` | `bool` | If `TRUE`, trains the spam filter. Defaults to `FALSE`. |

#### Return Values
- **`TRUE`**: On success.
- **`FALSE`**: If the system is disabled, user lacks permissions, or database error occurs.

#### Inner Mechanisms
1. Checks if the system is enabled and the user has operator permissions.
2. If `$spam` is `TRUE`, retrieves the comment text and trains the spam filter:
   - Undoes previous "nospam" training if the comment was active.
   - Trains the comment as spam.
3. Deletes the comment from the database.

#### Usage Example
```php
if ($comment_system->delete(42, TRUE)) {
    echo "Comment deleted and marked as spam.";
}
```


<!-- HASH:a220c55a73bdb7540a98c9f9eef5ec60 -->
