# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.blog.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Blog Module (`mod.blog.inc`)

The Blog module provides a complete blogging system with article management, tagging, archiving, commenting, and RSS feed generation. It integrates with the PWNC platform's core utilities for database access, URL management, and permission control.

---

### **Global Variables**

| Name | Description |
|------|-------------|
| `$blog_message` | Controls the current view mode (e.g., `add`, `edit`, `_edit`, `code_edit`, `rss`). |
| `$blog_index` | Unique identifier of the current article. |
| `$blog_date` | Date filter for archive view (format: `YYYYMMDD`). |
| `$blog_meta` | Tag filter for meta view. |
| `$blog_page` | Pagination control for overview. |
| `$blog_edit_*` | Form fields for article editing. |
| `$blog_code_edit_*` | Form fields for code template editing. |

---

### **Module Initialization**

1. **Library Loading**
   - Loads `blog` and `comment` libraries. If unavailable, displays an error message and exits.

2. **Template Integration**
   - Merges blog data (`blog_index`) into the active template's query data.

3. **Instance Handling**
   - Uses `CMS_CONTENT_INDEX` as the default instance if not specified in `$GLOBALS["blog"]["instance"]`.

4. **Permission Validation**
   - Checks if the user has at least one of: `reader`, `writer`, or `operator` permissions.

---

### **Core Workflow**

The module processes the `$blog_message` to determine the current action:

| Action | Description |
|--------|-------------|
| `_edit` | Handles article creation, modification, or deletion. |
| `_code_edit` | Handles code template updates. |
| `edit` / `add` | Displays the article editing form. |
| `code_edit` | Displays the code template editing form. |
| `rss` | Generates an RSS feed. |
| (Default) | Displays an article or overview. |

---

### **Key Features**

#### **1. Article Management**
- **Add/Edit Articles**: Form with title, tags, publication status, sticky flag, and content.
- **Delete Articles**: Confirmation dialog before deletion.
- **Tag Suggestions**: Auto-completes tags based on existing articles.

#### **2. Code Templates**
- Customizable HTML snippets for:
  - `CMS_DB_BLOG_CODE_POSITION_BEFORE` (before article)
  - `CMS_DB_BLOG_CODE_POSITION_AFTER` (after article)
  - `CMS_DB_BLOG_CODE_POSITION_TEASER` (in overview)
  - `CMS_DB_BLOG_CODE_POSITION_CONTROL` (in sidebar)

#### **3. Archiving & Tagging**
- **Archive Navigation**: Year/month/day drill-down.
- **Tag Cloud**: Visual representation of tag frequency.

#### **4. RSS Feed**
- Generates a 50-item feed with:
  - Article titles, links, descriptions, and publication dates.
  - Enclosures for images.
  - Caching (60-second delay).

#### **5. Comments**
- Integrates the `comment` module for each article.

---

### **Usage Examples**

#### **1. Displaying a Blog Overview**
```php
// Load the blog module with default settings
cms_application("blog");
```
- **Output**: Renders the blog overview with pagination, archive, and tag cloud.

#### **2. Adding a New Article**
```php
// Navigate to the "add" view
$url = cms_url([
    "blog_message" => "add",
    "blog_date"    => NULL,
    "blog_meta"    => NULL
]);
header("Location: " . $url);
```
- **Output**: Displays a form to create a new article.

#### **3. Generating an RSS Feed**
```php
// Access the RSS feed URL
$rss_url = u(["blog_message" => "rss"]);
echo '<a href="' . x($rss_url) . '">Subscribe to RSS</a>';
```
- **Output**: Generates an XML feed of recent articles.

#### **4. Customizing Code Templates**
```php
// Edit the "before article" template
$url = cms_url([
    "blog_message"            => "code_edit",
    "blog_code_edit_position" => CMS_DB_BLOG_CODE_POSITION_BEFORE
]);
header("Location: " . $url);
```
- **Output**: Displays a form to edit the HTML snippet rendered before each article.

---

### **Database Schema (Implicit)**
The module interacts with the following tables:

| Table | Purpose |
|-------|---------|
| `CMS_DB_BLOG` | Stores articles (title, text, status, time, sticky flag). |
| `CMS_DB_BLOG_META_LINK` | Links articles to tags. |
| `CMS_DB_BLOG_META_TERM` | Stores tag names. |
| `CMS_DB_COMMENT` | Stores comments (linked to articles via `instance`). |

---

### **Permissions**
| Permission | Description |
|------------|-------------|
| `CMS_BLOG_PERMISSION_READER` | Read access to articles. |
| `CMS_BLOG_PERMISSION_WRITER` | Create/edit articles. |
| `CMS_BLOG_PERMISSION_OPERATOR` | Delete articles and manage templates. |

Example:
```php
permission([
    CMS_BLOG_PERMISSION_READER . ".$instance"  => CMS_L_READ,
    CMS_BLOG_PERMISSION_WRITER . ".$instance"  => CMS_L_WRITE,
    CMS_BLOG_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR
]);
```

---

### **Helper Functions**
The module relies on core PWNC utilities:
- `cms_url()`: Generates URLs with CSRF protection.
- `sqlesc()`: Escapes SQL values (recursively for arrays).
- `x()`: Escapes XML/HTML output.
- `u()`: Shortcut for `cms_url()`.
- `cms_cache()`: Caches RSS feeds and other data.

---

### **Error Handling**
- **Database Errors**: Displays `CMS_L_MOD_BLOG_045` (generic error).
- **Missing Articles**: Returns HTTP 410 (Gone) and shows `CMS_L_MOD_BLOG_036` (article not found).
- **Permission Denied**: Silently exits if no permissions are granted.


<!-- HASH:5bc7110c88dd4d0b9ab1195bd8c65db3 -->
