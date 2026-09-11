# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.blog.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## blog

The `blog` class provides a complete blogging system within the PWNC Web Platform. It manages blog articles, metadata terms, and custom code snippets associated with specific positions in the blog layout. The class handles database schema verification, permission checks, CRUD operations for articles, metadata linking, and retrieval of custom code blocks.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_BLOG_PERMISSION_OPERATOR` | `"operator"` | Permission level for full administrative access |
| `CMS_BLOG_PERMISSION_WRITER` | `"writer"` | Permission level for creating/editing/deleting articles |
| `CMS_BLOG_PERMISSION_READER` | `"reader"` | Permission level for reading articles |
| `CMS_DB_BLOG_STATUS_INACTIVE` | `0` | Article status: inactive |
| `CMS_DB_BLOG_STATUS_ACTIVE` | `1` | Article status: active |
| `CMS_DB_BLOG_STICKY_OFF` | `0` | Sticky flag: off |
| `CMS_DB_BLOG_STICKY_ON` | `1` | Sticky flag: on |
| `CMS_DB_BLOG_CODE_POSITION_CONTROL` | `0` | Code position: control panel |
| `CMS_DB_BLOG_CODE_POSITION_TEASER` | `1` | Code position: teaser section |
| `CMS_DB_BLOG_CODE_POSITION_BEFORE` | `2` | Code position: before content |
| `CMS_DB_BLOG_CODE_POSITION_AFTER` | `3` | Code position: after content |

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$instance` | string | `""` | Blog instance identifier |
| `$enabled` | boolean | `FALSE` | Whether the blog is properly initialized |
| `$operator` | boolean | `FALSE` | Whether the current user has operator permissions |
| `$writer` | boolean | `FALSE` | Whether the current user has writer permissions |
| `$reader` | boolean | `FALSE` | Whether the current user has reader permissions |

### __construct

Initializes the blog instance by verifying required database tables and setting up permission flags.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$instance` | string | `""` | Blog instance identifier |

**Return Value:** None

**Inner Mechanisms:**
1. Creates a new `mysql` instance
2. Verifies four database tables exist with correct schemas:
   - Main blog table (`CMS_DB_BLOG`)
   - Meta terms table (`CMS_DB_BLOG_META_TERM`)
   - Meta links table (`CMS_DB_BLOG_META_LINK`)
   - Code snippets table (`CMS_DB_BLOG_CODE`)
3. If all tables are verified, sets permission flags based on the current user's permissions for this instance

**Usage Example:**
```php
// Initialize a blog instance named "news"
$blog = new \cms\blog("news");

// Check if the blog is enabled and user has write permissions
if ($blog->enabled && $blog->writer) {
    // User can create/edit articles
}
```

### add

Creates a new blog article with associated metadata.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$title` | string | Required | Article title |
| `$meta` | string | Required | Comma-separated metadata terms |
| `$text` | string | Required | Article content text |
| `$status` | int | `CMS_DB_BLOG_STATUS_ACTIVE` | Article status (active/inactive) |
| `$time` | int\|NULL | `NULL` | Publication timestamp (defaults to current time) |
| `$sticky` | int | `CMS_DB_BLOG_STICKY_OFF` | Sticky flag (on/off) |
| `$test` | boolean | `FALSE` | If TRUE, only tests permissions without creating article |

**Return Value:**
- `int` - The ID of the newly created article on success
- `FALSE` - On failure (disabled blog, insufficient permissions, or database error)

**Inner Mechanisms:**
1. Checks if blog is enabled and user has writer permissions
2. If `$test` is TRUE, returns TRUE immediately (permission check only)
3. Sets timestamp to current time if not provided
4. Inserts article into database with owner set to current user (`CMS_USER`)
5. Links metadata terms to the article via `meta_link()`
6. Returns the new article ID

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Test if user can add articles
if ($blog->test_add()) {
    // Create a new article
    $articleId = $blog->add(
        "My First Article",
        "php, programming, tutorial",
        "This is the content of my article...",
        CMS_DB_BLOG_STATUS_ACTIVE,
        time(),
        CMS_DB_BLOG_STICKY_OFF
    );
    
    if ($articleId !== FALSE) {
        echo "Article created with ID: " . $articleId;
    }
}
```

### test_add

Tests whether the current user has permission to add articles.

**Parameters:** None

**Return Value:**
- `TRUE` - User has permission to add articles
- `FALSE` - User lacks permission or blog is disabled

**Inner Mechanisms:**
Calls `add()` with all parameters set to NULL and `$test` set to TRUE, which performs only permission checks.

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Check permission before showing "Add Article" button
if ($blog->test_add()) {
    echo '<button>Add New Article</button>';
}
```

### edit

Modifies an existing blog article.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | int | Required | Article ID to edit |
| `$title` | string | Required | New article title |
| `$meta` | string | Required | New comma-separated metadata terms |
| `$text` | string | Required | New article content |
| `$status` | int\|NULL | `NULL` | New status (if NULL, unchanged) |
| `$time` | int\|NULL | `NULL` | New timestamp (if NULL, unchanged) |
| `$sticky` | int\|NULL | `NULL` | New sticky flag (if NULL, unchanged) |
| `$test` | boolean | `FALSE` | If TRUE, only tests permissions |

**Return Value:**
- `TRUE` - Article successfully updated
- `FALSE` - On failure (disabled blog, insufficient permissions, ownership issues, or database error)

**Inner Mechanisms:**
1. Checks if blog is enabled and user has writer permissions
2. For non-operators, verifies the current user owns the article
3. If `$test` is TRUE, returns TRUE (permission check only)
4. Builds dynamic UPDATE query, only including fields that are not NULL
5. Updates metadata links via `meta_link()`
6. Returns TRUE on success

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Test edit permission
if ($blog->test_edit(123)) {
    // Update article
    $success = $blog->edit(
        123,
        "Updated Title",
        "updated, tags, here",
        "New content for the article",
        CMS_DB_BLOG_STATUS_ACTIVE,
        NULL,  // Keep original timestamp
        CMS_DB_BLOG_STICKY_ON
    );
    
    if ($success) {
        echo "Article updated successfully";
    }
}
```

### test_edit

Tests whether the current user can edit a specific article.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | int | Article ID to test |

**Return Value:**
- `TRUE` - User can edit the article
- `FALSE` - User cannot edit or blog is disabled

**Inner Mechanisms:**
Calls `edit()` with content parameters set to NULL and `$test` set to TRUE.

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Check if user can edit article 456
if ($blog->test_edit(456)) {
    echo '<button>Edit Article</button>';
}
```

### delete

Removes a blog article and its associated metadata.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | int | Required | Article ID to delete |
| `$test` | boolean | `FALSE` | If TRUE, only tests permissions |

**Return Value:**
- `TRUE` - Article successfully deleted
- `FALSE` - On failure (disabled blog, insufficient permissions, ownership issues, or database error)

**Inner Mechanisms:**
1. Checks if blog is enabled and user has writer permissions
2. For non-operators, verifies the current user owns the article
3. If `$test` is TRUE, returns TRUE (permission check only)
4. Deletes the article from the database
5. Cleans up orphaned metadata via `meta_clean()`
6. Returns TRUE on success

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Test delete permission
if ($blog->test_delete(789)) {
    // Delete the article
    if ($blog->delete(789)) {
        echo "Article deleted successfully";
    }
}
```

### test_delete

Tests whether the current user can delete a specific article.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | int | Article ID to test |

**Return Value:**
- `TRUE` - User can delete the article
- `FALSE` - User cannot delete or blog is disabled

**Inner Mechanisms:**
Calls `delete()` with `$test` set to TRUE.

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Check if user can delete article 101
if ($blog->test_delete(101)) {
    echo '<button>Delete Article</button>';
}
```

### meta_link

Links metadata terms to a blog article, creating new terms if they don't exist.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | int | Article ID |
| `$meta` | string | Comma-separated metadata terms |

**Return Value:**
- `TRUE` - Metadata successfully linked
- `FALSE` - On failure (disabled blog, insufficient permissions, or database error)

**Inner Mechanisms:**
1. Checks if blog is enabled and user has writer permissions
2. Removes existing metadata links for the article
3. Normalizes metadata terms:
   - Splits by comma
   - Strips whitespace
   - Filters out empty values
   - Converts to lowercase
   - Removes duplicates
4. Inserts new terms into the meta terms table (using INSERT IGNORE)
5. Retrieves term IDs
6. Creates links between article and terms (using INSERT IGNORE)

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Link metadata to article 123
$success = $blog->meta_link(123, "php, web development, tutorial");

if ($success) {
    echo "Metadata linked successfully";
}
```

### meta_clean

Removes orphaned metadata entries that are no longer linked to any articles.

**Parameters:** None

**Return Value:**
- `TRUE` - Cleanup completed
- `FALSE` - If blog is disabled or user lacks writer permissions

**Inner Mechanisms:**
1. Checks if blog is enabled and user has writer permissions
2. Deletes metadata links where the associated article no longer exists
3. Deletes metadata terms that are no longer linked to any articles

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Clean up orphaned metadata after bulk deletions
$blog->meta_clean();
```

### code_set

Sets custom code for a specific position in the blog instance.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$position` | int | Required | Code position (use CMS_DB_BLOG_CODE_POSITION_* constants) |
| `$text` | string | Required | Code content to store |
| `$test` | boolean | `FALSE` | If TRUE, only tests permissions |

**Return Value:**
- `TRUE` - Code successfully stored
- `FALSE` - On failure (disabled blog, insufficient permissions, or database error)

**Inner Mechanisms:**
1. Checks if blog is enabled and user has operator permissions
2. If `$test` is TRUE, returns TRUE (permission check only)
3. Uses REPLACE query to insert or update code for the given instance and position

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Test permission
if ($blog->test_code_set()) {
    // Set code for the "before content" position
    $blog->code_set(
        CMS_DB_BLOG_CODE_POSITION_BEFORE,
        '<div class="custom-banner">Advertisement</div>'
    );
}
```

### test_code_set

Tests whether the current user can set custom code.

**Parameters:** None

**Return Value:**
- `TRUE` - User can set code
- `FALSE` - User cannot or blog is disabled

**Inner Mechanisms:**
Calls `code_set()` with NULL parameters and `$test` set to TRUE.

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Check if user can manage custom code
if ($blog->test_code_set()) {
    echo '<button>Manage Custom Code</button>';
}
```

### code_get

Retrieves custom code for a specific position.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$position` | int | Code position (use CMS_DB_BLOG_CODE_POSITION_* constants) |

**Return Value:**
- `string` - The stored code content
- `FALSE` - On error or if blog is disabled

**Inner Mechanisms:**
1. Checks if blog is enabled
2. Queries the code table for the given instance and position
3. Returns the code text or FALSE on error

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Get code for teaser position
$teaserCode = $blog->code_get(CMS_DB_BLOG_CODE_POSITION_TEASER);

if ($teaserCode !== FALSE) {
    echo $teaserCode;
}
```

### code_parse

Retrieves and processes custom code for a specific position, optionally replacing placeholders.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$position` | int | Required | Code position (use CMS_DB_BLOG_CODE_POSITION_* constants) |
| `$replacement` | array\|NULL | `NULL` | Associative array of placeholder replacements |

**Return Value:**
- `string` - Processed code content (empty string if no code exists)
- `FALSE` - If blog is disabled

**Inner Mechanisms:**
1. Checks if blog is enabled
2. Retrieves code via `code_get()`
3. Applies `l()` function (likely for localization or processing)
4. Returns empty string if code is empty
5. If `$replacement` is an array, uses `replace_placeholder()` to substitute placeholders
6. Otherwise returns the raw code

**Usage Example:**
```php
$blog = new \cms\blog("news");

// Get and process code with placeholder replacements
$processedCode = $blog->code_parse(
    CMS_DB_BLOG_CODE_POSITION_AFTER,
    ['{article_id}' => 123, '{title}' => 'My Article']
);

echo $processedCode;
```


<!-- HASH:a2ef5920c1b8821d0e1e3230189668a7 -->
