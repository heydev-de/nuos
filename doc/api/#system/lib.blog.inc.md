# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.blog.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Blog Class

The `blog` class provides a comprehensive interface for managing blog articles, metadata, and associated code snippets within the PWNC Web Platform. It handles article creation, modification, deletion, and metadata management (e.g., tags/categories), while enforcing permission-based access control. The class also supports custom code injection at predefined positions within the blog's rendering pipeline.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| **Permissions** | | |
| `CMS_BLOG_PERMISSION_OPERATOR` | `"operator"` | Full control over blog instance (edit/delete any article, manage code). |
| `CMS_BLOG_PERMISSION_WRITER` | `"writer"` | Create/edit/delete own articles. |
| `CMS_BLOG_PERMISSION_READER` | `"reader"` | Read-only access to articles. |
| **Status** | | |
| `CMS_DB_BLOG_STATUS_INACTIVE` | `0` | Article is hidden from public view. |
| `CMS_DB_BLOG_STATUS_ACTIVE` | `1` | Article is publicly visible. |
| **Sticky** | | |
| `CMS_DB_BLOG_STICKY_OFF` | `0` | Article appears in chronological order. |
| `CMS_DB_BLOG_STICKY_ON` | `1` | Article is pinned to the top of the list. |
| **Code Position** | | |
| `CMS_DB_BLOG_CODE_POSITION_CONTROL` | `0` | Code executed during blog control logic (e.g., pre-processing). |
| `CMS_DB_BLOG_CODE_POSITION_TEASER` | `1` | Code injected into article teasers (e.g., summaries). |
| `CMS_DB_BLOG_CODE_POSITION_BEFORE` | `2` | Code injected before the full article text. |
| `CMS_DB_BLOG_CODE_POSITION_AFTER` | `3` | Code injected after the full article text. |
| **Database** | | |
| `CMS_DB_BLOG` | `CMS_DB_PREFIX . "blog"` | Main articles table. |
| `CMS_DB_BLOG_INDEX` | `"id"` | Primary key for articles. |
| `CMS_DB_BLOG_INSTANCE` | `"instance"` | Blog instance identifier (e.g., `"news"`). |
| `CMS_DB_BLOG_STATUS` | `"status"` | Article visibility status (`CMS_DB_BLOG_STATUS_*`). |
| `CMS_DB_BLOG_TIME` | `"time"` | Publication timestamp (Unix epoch). |
| `CMS_DB_BLOG_STICKY` | `"sticky"` | Sticky flag (`CMS_DB_BLOG_STICKY_*`). |
| `CMS_DB_BLOG_OWNER` | `"owner"` | User ID of the article author. |
| `CMS_DB_BLOG_TITLE` | `"title"` | Article title (multilingual support via `meta`). |
| `CMS_DB_BLOG_META` | `"meta"` | Comma-separated metadata terms (e.g., `"technology,php"`). |
| `CMS_DB_BLOG_TEXT` | `"text"` | Article content (HTML/Markdown). |
| `CMS_DB_BLOG_META_TERM` | `CMS_DB_PREFIX . "blog_meta_term"` | Metadata terms table. |
| `CMS_DB_BLOG_META_TERM_INDEX` | `"id"` | Primary key for terms. |
| `CMS_DB_BLOG_META_TERM_TEXT` | `"text"` | Term text (e.g., `"technology"`). |
| `CMS_DB_BLOG_META_TERM_LANGUAGE` | `"language"` | Language code (e.g., `"en"`). |
| `CMS_DB_BLOG_META_LINK` | `CMS_DB_PREFIX . "blog_meta_link"` | Junction table linking articles to terms. |
| `CMS_DB_BLOG_META_LINK_ARTICLE` | `"article"` | Foreign key to `CMS_DB_BLOG_INDEX`. |
| `CMS_DB_BLOG_META_LINK_TERM` | `"term"` | Foreign key to `CMS_DB_BLOG_META_TERM_INDEX`. |
| `CMS_DB_BLOG_CODE` | `CMS_DB_PREFIX . "blog_code"` | Custom code snippets table. |
| `CMS_DB_BLOG_CODE_INSTANCE` | `"instance"` | Blog instance identifier. |
| `CMS_DB_BLOG_CODE_POSITION` | `"position"` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `CMS_DB_BLOG_CODE_TEXT` | `"text"` | PHP/HTML/JS code to execute. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Blog instance identifier (e.g., `"news"`). |
| `$enabled` | `FALSE` | `TRUE` if the blog instance is initialized and accessible. |
| `$operator` | `FALSE` | `TRUE` if the current user has operator permissions. |
| `$writer` | `FALSE` | `TRUE` if the current user has writer permissions. |
| `$reader` | `FALSE` | `TRUE` if the current user has reader permissions. |

---

### `__construct($instance = "")`

Initializes a blog instance, verifies database tables, and sets user permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Blog instance identifier (e.g., `"news"`). |

#### Return Values
- **`void`**: Initializes object properties and verifies database structure.

#### Inner Mechanisms
1. **Database Verification**: Checks if required tables (`CMS_DB_BLOG`, `CMS_DB_BLOG_META_TERM`, `CMS_DB_BLOG_META_LINK`, `CMS_DB_BLOG_CODE`) exist and creates them if missing.
2. **Permission Assignment**: Uses `cms_permission()` to check user roles (`operator`, `writer`, `reader`) for the given instance.
3. **State Initialization**: Sets `$enabled` to `TRUE` only if all tables are verified.

#### Usage Example
```php
$blog = new \cms\blog("news");
if ($blog->enabled) {
    echo "Blog instance 'news' is ready.";
}
```

---

### `add($title, $meta, $text, $status = CMS_DB_BLOG_STATUS_ACTIVE, $time = NULL, $sticky = CMS_DB_BLOG_STICKY_OFF, $test = FALSE)`

Creates a new blog article.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$title` | `string` | Article title. |
| `$meta` | `string` | Comma-separated metadata terms (e.g., `"technology,php"`). |
| `$text` | `string` | Article content (HTML/Markdown). |
| `$status` | `int` | Visibility status (`CMS_DB_BLOG_STATUS_*`). Default: `CMS_DB_BLOG_STATUS_ACTIVE`. |
| `$time` | `int\|NULL` | Publication timestamp (Unix epoch). Default: `time()`. |
| `$sticky` | `int` | Sticky flag (`CMS_DB_BLOG_STICKY_*`). Default: `CMS_DB_BLOG_STICKY_OFF`. |
| `$test` | `bool` | If `TRUE`, performs a permission check without creating the article. |

#### Return Values
- **`int\|FALSE`**: Article ID on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks writer permissions or the blog is disabled.
2. **Timestamp Handling**: Uses current time if `$time` is `NULL`.
3. **Database Insertion**: Adds the article to `CMS_DB_BLOG` and links metadata terms via `meta_link()`.

#### Usage Example
```php
$articleId = $blog->add(
    "New PHP Features in 2026",
    "php,technology",
    "<p>PHP 8.4 introduces...</p>",
    CMS_DB_BLOG_STATUS_ACTIVE,
    time(),
    CMS_DB_BLOG_STICKY_ON
);
if ($articleId) {
    echo "Article created with ID: $articleId";
}
```

---

### `test_add()`

Tests if the current user can create articles (permission check).

#### Return Values
- **`bool`**: `TRUE` if the user has writer permissions, `FALSE` otherwise.

#### Usage Example
```php
if ($blog->test_add()) {
    echo "You can create articles.";
}
```

---

### `edit($index, $title, $meta, $text, $status = NULL, $time = NULL, $sticky = NULL, $test = FALSE)`

Modifies an existing article.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$title` | `string` | New title. |
| `$meta` | `string` | New metadata terms. |
| `$text` | `string` | New content. |
| `$status` | `int\|NULL` | New visibility status. If `NULL`, unchanged. |
| `$time` | `int\|NULL` | New timestamp. If `NULL`, unchanged. |
| `$sticky` | `int\|NULL` | New sticky flag. If `NULL`, unchanged. |
| `$test` | `bool` | If `TRUE`, performs a permission check without updating. |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Ownership Check**: Non-operators can only edit their own articles.
2. **Dynamic SQL**: Omits unchanged fields (`status`, `time`, `sticky`) from the `UPDATE` query.
3. **Metadata Update**: Re-links metadata terms via `meta_link()`.

#### Usage Example
```php
if ($blog->edit(
    42,
    "Updated PHP Features",
    "php,technology,updates",
    "<p>PHP 8.4 now includes...</p>"
)) {
    echo "Article updated.";
}
```

---

### `test_edit($index)`

Tests if the current user can edit a specific article.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### Return Values
- **`bool`**: `TRUE` if the user has permissions, `FALSE` otherwise.

#### Usage Example
```php
if ($blog->test_edit(42)) {
    echo "You can edit this article.";
}
```

---

### `delete($index, $test = FALSE)`

Deletes an article and its associated metadata links.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$test` | `bool` | If `TRUE`, performs a permission check without deleting. |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Ownership Check**: Non-operators can only delete their own articles.
2. **Metadata Cleanup**: Removes orphaned metadata terms via `meta_clean()`.

#### Usage Example
```php
if ($blog->delete(42)) {
    echo "Article deleted.";
}
```

---

### `test_delete($index)`

Tests if the current user can delete a specific article.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### Return Values
- **`bool`**: `TRUE` if the user has permissions, `FALSE` otherwise.

#### Usage Example
```php
if ($blog->test_delete(42)) {
    echo "You can delete this article.";
}
```

---

### `meta_link($index, $meta)`

Links an article to metadata terms (e.g., tags/categories).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$meta` | `string` | Comma-separated terms (e.g., `"php,technology"`). |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Normalization**: Splits `$meta` into terms, trims whitespace, converts to lowercase, and removes duplicates.
2. **Term Insertion**: Adds new terms to `CMS_DB_BLOG_META_TERM` (ignores duplicates).
3. **Linkage**: Creates entries in `CMS_DB_BLOG_META_LINK` to associate the article with terms.

#### Usage Example
```php
$blog->meta_link(42, "php,technology,web");
```

---

### `meta_clean()`

Removes orphaned metadata terms and links (e.g., terms with no associated articles).

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Link Cleanup**: Deletes entries in `CMS_DB_BLOG_META_LINK` where the article no longer exists.
2. **Term Cleanup**: Deletes terms in `CMS_DB_BLOG_META_TERM` with no remaining links.

#### Usage Example
```php
$blog->meta_clean(); // Run periodically to maintain database hygiene.
```

---

### `code_set($position, $text, $test = FALSE)`

Stores custom code for a specific position in the blog's rendering pipeline.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `$text` | `string` | PHP/HTML/JS code to execute. |
| `$test` | `bool` | If `TRUE`, performs a permission check without saving. |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. **Permission Check**: Requires operator permissions.
2. **Database Update**: Uses `REPLACE` to insert or update the code snippet.

#### Usage Example
```php
$blog->code_set(
    CMS_DB_BLOG_CODE_POSITION_BEFORE,
    "<div class='alert'>Sponsored Content</div>"
);
```

---

### `test_code_set()`

Tests if the current user can set custom code.

#### Return Values
- **`bool`**: `TRUE` if the user has operator permissions, `FALSE` otherwise.

#### Usage Example
```php
if ($blog->test_code_set()) {
    echo "You can set custom code.";
}
```

---

### `code_get($position)`

Retrieves custom code for a specific position.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |

#### Return Values
- **`string\|FALSE`**: The stored code, or `FALSE` on failure.

#### Usage Example
```php
$code = $blog->code_get(CMS_DB_BLOG_CODE_POSITION_AFTER);
if ($code) {
    echo "After-article code: $code";
}
```

---

### `code_parse($position, $replacement = NULL)`

Retrieves and processes custom code, optionally replacing placeholders.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `$replacement` | `array\|NULL` | Key-value pairs for placeholder replacement (e.g., `["%NAME%" => "Alice"]`). |

#### Return Values
- **`string\|FALSE`**: Processed code, or an empty string if no code exists.

#### Inner Mechanisms
1. **Code Retrieval**: Fetches code via `code_get()`.
2. **Placeholder Replacement**: Uses `replace_placeholder()` if `$replacement` is provided.

#### Usage Example
```php
$code = $blog->code_parse(
    CMS_DB_BLOG_CODE_POSITION_TEASER,
    ["%TITLE%" => "New Article"]
);
echo $code; // e.g., "<h2>%TITLE%</h2>" → "<h2>New Article</h2>"
```


<!-- HASH:a2ef5920c1b8821d0e1e3230189668a7 -->
