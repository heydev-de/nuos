# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.comment.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## comment

The `comment` class provides a complete comment management system for the PWNC Web Platform. It handles comment creation, editing, status management, rating, deletion, and spam detection integration. Each instance is tied to a specific content "instance" (e.g., a blog post, product, or page) and enforces role-based permissions for operators, writers, and readers.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"operator"` | Permission level for full administrative control over comments |
| `CMS_COMMENT_PERMISSION_WRITER` | `"writer"` | Permission level for adding and editing comments |
| `CMS_COMMENT_PERMISSION_READER` | `"reader"` | Permission level for viewing and rating comments |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment is pending approval |
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is approved and visible |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `2` | Comment is hidden from public view |

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$instance` | `string` | `""` | Identifier for the content instance this comment system belongs to |
| `$enabled` | `bool` | `FALSE` | Whether the comment system is active and ready for use |
| `$operator` | `bool` | `FALSE` | Whether the current user has operator permissions |
| `$writer` | `bool` | `FALSE` | Whether the current user has writer permissions |
| `$reader` | `bool` | `FALSE` | Whether the current user has reader permissions |
| `$default_status` | `int` | `CMS_DB_COMMENT_STATUS_INACTIVE` | Default status assigned to newly created comments |
| `$spam_threshold` | `int` | `95` | Spam probability threshold above which comments are rejected |

### __construct

Initializes the comment system for a given instance. Verifies the database table exists, creates it if necessary, and checks user permissions.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$instance` | `string` | `""` | Content instance identifier (e.g., `"blog-post-123"`) |

#### Return Values

No explicit return value. Sets internal properties based on database verification and permission checks.

#### Inner Mechanisms

1. Creates a `mysql` instance and verifies the comment table structure exists, creating it with the proper schema if it doesn't.
2. If the table is verified successfully, loads permission flags for the current user against the instance.
3. Sets `$enabled` to `TRUE` only if the table verification passes.

#### Usage Example

```php
$comment = new comment("blog-post-42");
if ($comment->enabled) {
    echo "Comment system ready for blog-post-42";
}
```

### add

Creates a new comment after performing spam checks and duplicate detection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Author's display name |
| `$email` | `string` | Author's email address |
| `$url` | `string` | Author's website URL |
| `$text` | `string` | Comment text content |

#### Return Values

| Type | Description |
|------|-------------|
| `int` | ID of the newly created comment on success |
| `FALSE` | If the system is disabled or user lacks writer permissions |
| `-1` | If the comment is flagged as spam |
| `-2` | If a duplicate comment was found within the last hour |

#### Inner Mechanisms

1. Checks if the system is enabled and the user has writer permissions.
2. Loads the category library and evaluates spam probability using its Bayesian filter.
3. Rejects the comment if spam probability exceeds the threshold.
4. Generates a hash of the comment text and checks for duplicates within the last hour.
5. Inserts the comment into the database with the default status.
6. If the comment is active by default, trains the spam filter as "not spam".
7. Logs the creation event.

#### Usage Example

```php
$comment = new comment("product-100");
$result = $comment->add("Alice", "alice@example.com", "https://alice.dev", "Great product!");
if ($result > 0) {
    echo "Comment created with ID: " . $result;
} elseif ($result === -1) {
    echo "Comment rejected as spam";
} elseif ($result === -2) {
    echo "Duplicate comment detected";
}
```

### edit

Updates an existing comment's details. Only operators can perform this action.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | ID of the comment to edit |
| `$name` | `string` | Updated author name |
| `$email` | `string` | Updated author email |
| `$url` | `string` | Updated author URL |
| `$text` | `string` | Updated comment text |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful update |
| `FALSE` | If disabled, no operator permissions, or database error |

#### Inner Mechanisms

1. Verifies system is enabled and user has operator permissions.
2. Retrieves the current comment's status and text.
3. Updates the comment record with new values and recalculates the text hash.
4. If the comment was previously active, adjusts spam filter training by undoing the old text's "not spam" training and training the new text.

#### Usage Example

```php
$comment = new comment("blog-post-42");
if ($comment->edit(15, "Bob", "bob@example.com", "", "Updated comment text")) {
    echo "Comment updated successfully";
}
```

### status

Changes the status of a comment (e.g., from inactive to active). Only operators can perform this action.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | ID of the comment |
| `$status` | `int` | New status value (`CMS_DB_COMMENT_STATUS_*`) |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful status change or if status is already the same |
| `FALSE` | If disabled, no operator permissions, invalid status, or database error |

#### Inner Mechanisms

1. Validates system status and operator permissions.
2. Rejects `CMS_DB_COMMENT_STATUS_INACTIVE` as an invalid target status.
3. Retrieves the current comment's status and text.
4. If status is unchanged, returns `TRUE` immediately.
5. For transitions to `HIDDEN`: undoes previous "not spam" training if the comment was active.
6. For transitions to `ACTIVE`: trains the comment text as "not spam".
7. Updates the comment's status in the database.

#### Usage Example

```php
$comment = new comment("blog-post-42");
// Approve a pending comment
if ($comment->status(15, CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment approved";
}
```

### rate_good

Increments the positive rating for a comment. Readers can rate comments.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | ID of the comment to rate |
| `$invert` | `bool` | `FALSE` | If `TRUE`, decrements the rating instead |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful rating update |
| `FALSE` | If disabled, no reader permissions, or database error |

#### Inner Mechanisms

1. Checks system status and reader permissions.
2. Constructs a user identifier from the IP hash.
3. Updates the rating value (incrementing or decrementing based on `$invert`), increments the rating count, and appends the user ID to the rating user list.
4. Uses a `NOT LIKE` condition to prevent duplicate ratings from the same user.

#### Usage Example

```php
$comment = new comment("blog-post-42");
if ($comment->rate_good(15)) {
    echo "Positive rating recorded";
}
```

### rate_bad

Decrements the positive rating for a comment (i.e., records a negative rating).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | ID of the comment to rate negatively |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful rating update |
| `FALSE` | If disabled, no reader permissions, or database error |

#### Inner Mechanisms

Delegates to `rate_good()` with `$invert` set to `TRUE`, which decrements the rating value while still incrementing the rating count.

#### Usage Example

```php
$comment = new comment("blog-post-42");
if ($comment->rate_bad(15)) {
    echo "Negative rating recorded";
}
```

### delete

Removes a comment from the database. Only operators can perform this action.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | ID of the comment to delete |
| `$spam` | `bool` | `FALSE` | If `TRUE`, trains the spam filter with this comment's content |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful deletion |
| `FALSE` | If disabled, no operator permissions, or database error |

#### Inner Mechanisms

1. Verifies system status and operator permissions.
2. If `$spam` is `TRUE` and the category library is available:
   - Retrieves the comment's status and text.
   - If the comment was previously active, undoes its "not spam" training.
   - Trains the comment text as "spam" in the Bayesian filter.
3. Deletes the comment record from the database.

#### Usage Example

```php
$comment = new comment("blog-post-42");
// Delete a comment and train it as spam
if ($comment->delete(15, TRUE)) {
    echo "Comment deleted and marked as spam";
}
```


<!-- HASH:a220c55a73bdb7540a98c9f9eef5ec60 -->
