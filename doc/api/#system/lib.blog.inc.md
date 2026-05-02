# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.blog.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Blog Class

The `blog` class provides a comprehensive interface for managing blog articles, metadata, and associated code snippets within the NUOS platform. It handles article creation, modification, deletion, and metadata management (e.g., tags), as well as custom code injection at predefined positions in the blog's frontend.

---

### Constants

#### Permissions
| Name | Value | Description |
|------|-------|-------------|
| `CMS_BLOG_PERMISSION_OPERATOR` | `"operator"` | Full control over the blog instance. |
| `CMS_BLOG_PERMISSION_WRITER` | `"writer"` | Ability to create, edit, and delete articles. |
| `CMS_BLOG_PERMISSION_READER` | `"reader"` | Read-only access to articles. |

#### Status
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_BLOG_STATUS_INACTIVE` | `0` | Article is inactive and not visible. |
| `CMS_DB_BLOG_STATUS_ACTIVE` | `1` | Article is active and visible. |

#### Sticky
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_BLOG_STICKY_OFF` | `0` | Article is not sticky. |
| `CMS_DB_BLOG_STICKY_ON` | `1` | Article is sticky (appears at the top). |

#### Code Position
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_BLOG_CODE_POSITION_CONTROL` | `0` | Control panel code (backend). |
| `CMS_DB_BLOG_CODE_POSITION_TEASER` | `1` | Teaser section code (frontend). |
| `CMS_DB_BLOG_CODE_POSITION_BEFORE` | `2` | Code injected before article content. |
| `CMS_DB_BLOG_CODE_POSITION_AFTER` | `3` | Code injected after article content. |

#### Database Tables and Fields
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_BLOG` | `CMS_DB_PREFIX . "blog"` | Main blog articles table. |
| `CMS_DB_BLOG_INDEX` | `"id"` | Primary key for articles. |
| `CMS_DB_BLOG_INSTANCE` | `"instance"` | Blog instance identifier. |
| `CMS_DB_BLOG_STATUS` | `"status"` | Article status (`CMS_DB_BLOG_STATUS_*`). |
| `CMS_DB_BLOG_TIME` | `"time"` | Publication timestamp. |
| `CMS_DB_BLOG_STICKY` | `"sticky"` | Sticky flag (`CMS_DB_BLOG_STICKY_*`). |
| `CMS_DB_BLOG_OWNER` | `"owner"` | User ID of the article owner. |
| `CMS_DB_BLOG_TITLE` | `"title"` | Article title. |
| `CMS_DB_BLOG_META` | `"meta"` | Comma-separated metadata (tags). |
| `CMS_DB_BLOG_TEXT` | `"text"` | Article content. |
| `CMS_DB_BLOG_META_TERM` | `CMS_DB_PREFIX . "blog_meta_term"` | Metadata terms (tags) table. |
| `CMS_DB_BLOG_META_TERM_INDEX` | `"id"` | Primary key for terms. |
| `CMS_DB_BLOG_META_TERM_TEXT` | `"text"` | Term text (unique per language). |
| `CMS_DB_BLOG_META_TERM_LANGUAGE` | `"language"` | Language identifier for the term. |
| `CMS_DB_BLOG_META_LINK` | `CMS_DB_PREFIX . "blog_meta_link"` | Link table between articles and terms. |
| `CMS_DB_BLOG_META_LINK_ARTICLE` | `"article"` | Article ID. |
| `CMS_DB_BLOG_META_LINK_TERM` | `"term"` | Term ID. |
| `CMS_DB_BLOG_CODE` | `CMS_DB_PREFIX . "blog_code"` | Custom code snippets table. |
| `CMS_DB_BLOG_CODE_INSTANCE` | `"instance"` | Blog instance identifier. |
| `CMS_DB_BLOG_CODE_POSITION` | `"position"` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `CMS_DB_BLOG_CODE_TEXT` | `"text"` | Code snippet content. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Blog instance identifier. |
| `$enabled` | `FALSE` | Whether the blog instance is enabled and tables exist. |
| `$operator` | `FALSE` | Whether the current user has operator permissions. |
| `$writer` | `FALSE` | Whether the current user has writer permissions. |
| `$reader` | `FALSE` | Whether the current user has reader permissions. |

---

### `__construct`

#### Purpose
Initializes a blog instance, verifies database tables, and sets user permissions.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Blog instance identifier. Defaults to `""`. |

#### Return Values
- **None**: Constructor does not return a value.

#### Inner Mechanisms
1. **Database Verification**: Uses `mysql->verify_table()` to ensure all required tables (`CMS_DB_BLOG`, `CMS_DB_BLOG_META_TERM`, `CMS_DB_BLOG_META_LINK`, `CMS_DB_BLOG_CODE`) exist with the correct schema.
2. **Permission Assignment**: Checks user permissions for the blog instance using `cms_permission()` and assigns them to `$operator`, `$writer`, and `$reader`.
3. **Enable Flag**: Sets `$enabled` to `TRUE` if all tables exist and are verified.

#### Usage
```php
$blog = new blog("my_blog_instance");
```
- Typically called during blog initialization in a module or controller.

---

### `add`

#### Purpose
Creates a new blog article with the provided metadata and content.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$title` | `string` | Article title. |
| `$meta` | `string` | Comma-separated metadata (tags). |
| `$text` | `string` | Article content. |
| `$status` | `int` | Article status (`CMS_DB_BLOG_STATUS_*`). Defaults to `CMS_DB_BLOG_STATUS_ACTIVE`. |
| `$time` | `int` | Publication timestamp. Defaults to current time if `NULL`. |
| `$sticky` | `int` | Sticky flag (`CMS_DB_BLOG_STICKY_*`). Defaults to `CMS_DB_BLOG_STICKY_OFF`. |
| `$test` | `bool` | If `TRUE`, performs a permission test without creating the article. Defaults to `FALSE`. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Article ID on success. |
| `bool` | `FALSE` on failure (e.g., no permissions, database error). |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$writer` permissions.
2. **Test Mode**: Returns `TRUE` if `$test` is `TRUE`.
3. **Timestamp**: Uses current time if `$time` is `NULL`.
4. **Database Insert**: Inserts the article into `CMS_DB_BLOG` and retrieves the auto-incremented ID.
5. **Metadata Linking**: Calls `meta_link()` to associate the article with its metadata.

#### Usage
```php
$article_id = $blog->add(
    "My Article Title",
    "tag1, tag2, tag3",
    "Article content here...",
    CMS_DB_BLOG_STATUS_ACTIVE,
    time(),
    CMS_DB_BLOG_STICKY_ON
);
```
- Used when creating new articles in the blog's backend or frontend.

---

### `test_add`

#### Purpose
Tests whether the current user has permission to add an article.

#### Parameters
- **None**.

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has writer permissions, `FALSE` otherwise. |

#### Inner Mechanisms
- Calls `add()` with `$test = TRUE` to perform a permission check.

#### Usage
```php
if ($blog->test_add()) {
    // User can add articles
}
```
- Used to conditionally display UI elements (e.g., "Add Article" button).

---

### `edit`

#### Purpose
Updates an existing blog article.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$title` | `string` | New article title. |
| `$meta` | `string` | New comma-separated metadata (tags). |
| `$text` | `string` | New article content. |
| `$status` | `int` | New article status (`CMS_DB_BLOG_STATUS_*`). Defaults to `NULL` (no change). |
| `$time` | `int` | New publication timestamp. Defaults to `NULL` (no change). |
| `$sticky` | `int` | New sticky flag (`CMS_DB_BLOG_STICKY_*`). Defaults to `NULL` (no change). |
| `$test` | `bool` | If `TRUE`, performs a permission test without updating the article. Defaults to `FALSE`. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$writer` permissions.
2. **Ownership Check**: If the user is not an operator, verifies they own the article.
3. **Test Mode**: Returns `TRUE` if `$test` is `TRUE`.
4. **Database Update**: Updates the article in `CMS_DB_BLOG` with the provided fields.
5. **Metadata Linking**: Calls `meta_link()` to update the article's metadata.

#### Usage
```php
$success = $blog->edit(
    123,
    "Updated Title",
    "tag1, tag2",
    "Updated content...",
    CMS_DB_BLOG_STATUS_ACTIVE,
    time(),
    CMS_DB_BLOG_STICKY_OFF
);
```
- Used when editing articles in the blog's backend.

---

### `test_edit`

#### Purpose
Tests whether the current user has permission to edit a specific article.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has permission, `FALSE` otherwise. |

#### Inner Mechanisms
- Calls `edit()` with `$test = TRUE` to perform a permission check.

#### Usage
```php
if ($blog->test_edit(123)) {
    // User can edit article 123
}
```
- Used to conditionally display UI elements (e.g., "Edit" button).

---

### `delete`

#### Purpose
Deletes a blog article.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$test` | `bool` | If `TRUE`, performs a permission test without deleting the article. Defaults to `FALSE`. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$writer` permissions.
2. **Ownership Check**: If the user is not an operator, verifies they own the article.
3. **Test Mode**: Returns `TRUE` if `$test` is `TRUE`.
4. **Database Delete**: Removes the article from `CMS_DB_BLOG`.
5. **Metadata Cleanup**: Calls `meta_clean()` to remove orphaned metadata.

#### Usage
```php
$success = $blog->delete(123);
```
- Used when deleting articles in the blog's backend.

---

### `test_delete`

#### Purpose
Tests whether the current user has permission to delete a specific article.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has permission, `FALSE` otherwise. |

#### Inner Mechanisms
- Calls `delete()` with `$test = TRUE` to perform a permission check.

#### Usage
```php
if ($blog->test_delete(123)) {
    // User can delete article 123
}
```
- Used to conditionally display UI elements (e.g., "Delete" button).

---

### `meta_link`

#### Purpose
Associates an article with metadata terms (tags) and updates the metadata tables.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$meta` | `string` | Comma-separated metadata (tags). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$writer` permissions.
2. **Cleanup**: Removes existing metadata links for the article.
3. **Normalization**: Splits `$meta` into an array, trims whitespace, filters empty values, converts to lowercase, and removes duplicates.
4. **Term Insertion**: Inserts new terms into `CMS_DB_BLOG_META_TERM` (ignoring duplicates).
5. **Link Creation**: Creates links between the article and its terms in `CMS_DB_BLOG_META_LINK`.

#### Usage
```php
$success = $blog->meta_link(123, "tag1, tag2, tag3");
```
- Automatically called by `add()` and `edit()` to manage metadata.

---

### `meta_clean`

#### Purpose
Removes orphaned metadata terms and links (e.g., terms with no associated articles).

#### Parameters
- **None**.

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$writer` permissions.
2. **Link Cleanup**: Deletes links in `CMS_DB_BLOG_META_LINK` where the associated article no longer exists.
3. **Term Cleanup**: Deletes terms in `CMS_DB_BLOG_META_TERM` with no associated links.

#### Usage
```php
$blog->meta_clean();
```
- Automatically called by `delete()` to maintain database integrity.

---

### `code_set`

#### Purpose
Stores or updates a custom code snippet for a specific position in the blog.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `$text` | `string` | Code snippet content. |
| `$test` | `bool` | If `TRUE`, performs a permission test without updating the code. Defaults to `FALSE`. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled` and `$operator` permissions.
2. **Test Mode**: Returns `TRUE` if `$test` is `TRUE`.
3. **Database Replace**: Uses `REPLACE` to insert or update the code snippet in `CMS_DB_BLOG_CODE`.

#### Usage
```php
$success = $blog->code_set(
    CMS_DB_BLOG_CODE_POSITION_TEASER,
    "<div>Custom teaser code</div>"
);
```
- Used to inject custom HTML/JS/CSS into specific blog sections.

---

### `test_code_set`

#### Purpose
Tests whether the current user has permission to set custom code.

#### Parameters
- **None**.

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has operator permissions, `FALSE` otherwise. |

#### Inner Mechanisms
- Calls `code_set()` with `$test = TRUE` to perform a permission check.

#### Usage
```php
if ($blog->test_code_set()) {
    // User can set custom code
}
```
- Used to conditionally display UI elements (e.g., "Custom Code" editor).

---

### `code_get`

#### Purpose
Retrieves a custom code snippet for a specific position.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Code snippet content. |
| `bool` | `FALSE` on failure (e.g., no permissions, no code found). |

#### Inner Mechanisms
1. **Permission Check**: Verifies `$enabled`.
2. **Database Query**: Retrieves the code snippet from `CMS_DB_BLOG_CODE`.

#### Usage
```php
$code = $blog->code_get(CMS_DB_BLOG_CODE_POSITION_TEASER);
```
- Used to fetch custom code for rendering in the blog's frontend.

---

### `code_parse`

#### Purpose
Parses a custom code snippet, replacing placeholders with provided values.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (`CMS_DB_BLOG_CODE_POSITION_*`). |
| `$replacement` | `array` | Associative array of placeholder-value pairs (e.g., `["%TITLE%" => "My Title"]`). Defaults to `NULL`. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Parsed code snippet. |
| `string` | Empty string if no code exists. |
| `bool` | `FALSE` on failure. |

#### Inner Mechanisms
1. **Code Retrieval**: Fetches the code snippet using `code_get()` and loads it with `l()`.
2. **Placeholder Replacement**: Replaces placeholders (e.g., `%TITLE%`) with values from `$replacement`. Placeholders can be wrapped in optional brackets (e.g., `[%TITLE%]`).
3. **XML Escaping**: Escapes replacement values with `x()` for safe output.

#### Usage
```php
$parsed_code = $blog->code_parse(
    CMS_DB_BLOG_CODE_POSITION_TEASER,
    ["%TITLE%" => "My Article", "%AUTHOR%" => "John Doe"]
);
```
- Used to dynamically render custom code with contextual values.


<!-- HASH:e929afc30a1fc9c6148515fd224a2023 -->
