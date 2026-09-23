# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.forum.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Class

The `forum` class provides a complete forum system for the PWNC Web Platform. It manages forum posts stored in a database table, supporting hierarchical organization through containers, user permissions, search functionality, and access logging. Each forum instance can be isolated by an optional instance identifier, creating separate tables per instance.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_FORUM_PERMISSION_OPERATOR` | `"operator"` | Permission level for full administrative access |
| `CMS_FORUM_PERMISSION_WRITER` | `"writer"` | Permission level for creating and editing posts |
| `CMS_FORUM_PERMISSION_READER` | `"reader"` | Permission level for read-only access |
| `CMS_DB_FORUM` | `CMS_DB_PREFIX . "forum"` | Base database table name for forum posts |
| `CMS_DB_FORUM_INDEX` | `"id"` | Database column for post ID |
| `CMS_DB_FORUM_CONTAINER` | `"container"` | Database column for parent container/thread ID |
| `CMS_DB_FORUM_USER` | `"user"` | Database column for username |
| `CMS_DB_FORUM_TITLE` | `"title"` | Database column for post title |
| `CMS_DB_FORUM_TEXT` | `"text"` | Database column for post content |
| `CMS_DB_FORUM_EMAIL` | `"email"` | Database column for email notification flag |
| `CMS_DB_FORUM_TIME` | `"time"` | Database column for post timestamp |
| `CMS_DB_FORUM_ACCESS` | `"access"` | Database column for access counter |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string\|NULL` | Instance identifier for table isolation |
| `$table` | `string` | Full database table name |
| `$mysql` | `mysql` | MySQL database connection handler |
| `$operator` | `bool` | Whether current user has operator permissions |
| `$writer` | `bool` | Whether current user has writer permissions |
| `$reader` | `bool` | Whether current user has reader permissions |
| `$enabled` | `bool\|NULL` | Whether the forum is active and available |

### Methods

#### `__construct($instance = NULL)`

Initializes the forum class, sets up the database table, and checks user permissions.

**Parameters:**
- `$instance` (`string\|NULL`) – Optional instance identifier. When provided, appends `_$instance` to the table name for isolation.

**Return Value:** None

**Inner Mechanisms:**
1. Creates a new MySQL connection handler
2. Appends instance identifier to table name if provided
3. Checks MySQL version for n-gram parser support (MySQL 5.7.6+)
4. Verifies/creates the database table with proper schema including indexes and fulltext search capabilities
5. Sets permission flags based on current user's access level
6. Enables the forum only if table verification succeeds

**Usage Example:**
```php
// Create a general forum instance
$forum = new forum();

// Create an instance-specific forum (e.g., for a specific category)
$categoryForum = new forum("general");
```

#### `add($index, $title, $text, $email, $test = FALSE)`

Creates a new forum post.

**Parameters:**
- `$index` (`int`) – Parent container/thread ID. Use 0 for top-level posts.
- `$title` (`string`) – Post title
- `$text` (`string`) – Post content/body
- `$email` (`int`) – Email notification flag (0 or 1)
- `$test` (`bool`) – If TRUE, performs permission check only without inserting data

**Return Value:**
- `int` – ID of the newly created post on success
- `FALSE` – On failure or insufficient permissions

**Inner Mechanisms:**
1. Validates forum is enabled and user has writer permissions
2. Requires either a valid parent index or operator status for top-level posts
3. Inserts the post with current user, timestamp, and sanitized data
4. Logs the creation event
5. Sends email notification to parent post author if requested and email is available
6. Returns the new post ID

**Usage Example:**
```php
$forum = new forum();
$postId = $forum->add(
    0,                    // Top-level post
    "Welcome to the Forum",
    "This is the first post content.",
    1,                    // Enable email notifications
    false                 // Actually create the post
);

if ($postId) {
    echo "Post created with ID: $postId";
}
```

#### `test_add($index)`

Tests whether a post can be added without actually creating it.

**Parameters:**
- `$index` (`int`) – Parent container/thread ID

**Return Value:**
- `TRUE` – If the user has permission to add a post
- `FALSE` – If permissions are insufficient

**Usage Example:**
```php
$forum = new forum();
if ($forum->test_add(0)) {
    echo "User can create top-level posts";
}
```

#### `edit($index, $title, $text, $email, $test = FALSE)`

Edits an existing forum post.

**Parameters:**
- `$index` (`int`) – Post ID to edit
- `$title` (`string`) – New title
- `$text` (`string`) – New content
- `$email` (`int`) – New email notification flag
- `$test` (`bool`) – If TRUE, performs permission check only without updating data

**Return Value:**
- `TRUE` – On success or successful permission test
- `FALSE` – On failure or insufficient permissions

**Inner Mechanisms:**
1. Validates forum is enabled and user has writer permissions
2. Checks that the post exists
3. Verifies ownership (user must be the author) or operator status
4. Updates the post with new values if not in test mode
5. Returns TRUE on success

**Usage Example:**
```php
$forum = new forum();
$success = $forum->edit(
    42,                   // Post ID
    "Updated Title",
    "Updated content here",
    0,                    // Disable email notifications
    false                 // Actually perform the edit
);

if ($success) {
    echo "Post updated successfully";
}
```

#### `test_edit($index)`

Tests whether a post can be edited without actually modifying it.

**Parameters:**
- `$index` (`int`) – Post ID to test

**Return Value:**
- `TRUE` – If the user has permission to edit the post
- `FALSE` – If permissions are insufficient

**Usage Example:**
```php
$forum = new forum();
if ($forum->test_edit(42)) {
    echo "User can edit post #42";
}
```

#### `move($index, $parent, $test = FALSE)`

Moves a post to a different container/thread.

**Parameters:**
- `$index` (`int`) – Post ID to move
- `$parent` (`int`) – New parent container/thread ID
- `$test` (`bool`) – If TRUE, performs validation only without moving

**Return Value:**
- `TRUE` – On success or successful validation test
- `FALSE` – On failure or insufficient permissions

**Inner Mechanisms:**
1. Validates forum is enabled and user has operator permissions
2. Ensures the post and parent are different
3. Verifies the parent isn't already the current container
4. Checks that moving won't create a circular reference (parent isn't a child of the post)
5. Updates the container field if not in test mode

**Usage Example:**
```php
$forum = new forum();
$success = $forum->move(42, 10, false);  // Move post 42 to thread 10

if ($success) {
    echo "Post moved successfully";
}
```

#### `test_move($index, $parent)`

Tests whether a post can be moved without actually moving it.

**Parameters:**
- `$index` (`int`) – Post ID to test
- `$parent` (`int`) – Proposed new parent container/thread ID

**Return Value:**
- `TRUE` – If the move operation is valid
- `FALSE` – If validation fails

**Usage Example:**
```php
$forum = new forum();
if ($forum->test_move(42, 10)) {
    echo "Post can be moved to thread 10";
}
```

#### `delete($index, $test = FALSE)`

Deletes a forum post.

**Parameters:**
- `$index` (`int`) – Post ID to delete
- `$test` (`bool`) – If TRUE, performs permission check only without deleting

**Return Value:**
- `TRUE` – On success or successful permission test
- `FALSE` – On failure or insufficient permissions

**Inner Mechanisms:**
1. Validates forum is enabled and user has operator permissions
2. Deletes the post using the MySQL handler's delete method
3. Returns TRUE on success

**Usage Example:**
```php
$forum = new forum();
$success = $forum->delete(42, false);  // Delete post 42

if ($success) {
    echo "Post deleted successfully";
}
```

#### `test_delete($index)`

Tests whether a post can be deleted without actually deleting it.

**Parameters:**
- `$index` (`int`) – Post ID to test

**Return Value:**
- `TRUE` – If the user has permission to delete the post
- `FALSE` – If permissions are insufficient

**Usage Example:**
```php
$forum = new forum();
if ($forum->test_delete(42)) {
    echo "User can delete post #42";
}
```

#### `search($value)`

Searches forum posts using full-text search.

**Parameters:**
- `$value` (`string`) – Search query string

**Return Value:**
- `resource` – MySQL result set on success
- `FALSE` – If forum is disabled

**Inner Mechanisms:**
1. Logs the search activity
2. Sanitizes input for boolean mode search (removes special characters)
3. Prepares search terms with wildcards for partial matching
4. Constructs a complex query combining:
   - Natural language mode search on title (weighted ×2) and text
   - Boolean mode search on title (weighted ×2) and text
5. Filters to only include posts with containers > 0 (replies)
6. Orders results by relevance score
7. Limits to 100 results

**Usage Example:**
```php
$forum = new forum();
$results = $forum->search("php programming");

if ($results) {
    while ($row = mysql_fetch_assoc($results)) {
        echo "Found: " . $row['title'] . " (relevance: " . $row['relevance'] . ")\n";
    }
}
```

#### `reply_count($index)`

Counts replies for one or more posts.

**Parameters:**
- `$index` (`int\|array`) – Single post ID or array of post IDs

**Return Value:**
- `array` – Associative array mapping post IDs to reply counts
- `FALSE` – If forum is disabled or query fails

**Inner Mechanisms:**
1. Converts single ID to array if needed
2. Queries the database counting posts grouped by container
3. Builds an associative array with all requested IDs, defaulting to 0 for those with no replies
4. Returns the complete mapping

**Usage Example:**
```php
$forum = new forum();
$counts = $forum->reply_count([1, 2, 3]);

foreach ($counts as $postId => $replyCount) {
    echo "Post $postId has $replyCount replies\n";
}
```

#### `log_access($index)`

Increments the access counter for a specific post.

**Parameters:**
- `$index` (`int`) – Post ID to log access for

**Return Value:**
- `NULL` – Always returns nothing (void)

**Inner Mechanisms:**
1. Checks if forum is enabled
2. Executes an UPDATE query incrementing the access counter
3. No return value

**Usage Example:**
```php
$forum = new forum();
$forum->log_access(42);  // Increment view count for post 42
```

### Helper Function

#### `forum_quote($text)`

Formats text as a quoted reply for forum posts.

**Parameters:**
- `$text` (`string`) – Original text to quote

**Return Value:**
- `string` – Formatted quoted text with `>` prefixes

**Inner Mechanisms:**
1. Splits text into lines
2. For each line:
   - If already quoted (starts with `> `), prefixes with another `>`
   - Otherwise, wraps the line using multibyte-safe word wrapping and prefixes each wrapped line with `> `
3. Joins all lines with newlines

**Usage Example:**
```php
$original = "This is a long paragraph that might need wrapping when quoted in a forum reply.";
$quoted = forum_quote($original);
echo $quoted;
// Output:
// > This is a long paragraph that might need wrapping when quoted in a
// > forum reply.
```


<!-- HASH:9376720ba5976dca1b331a99de2c2f47 -->
