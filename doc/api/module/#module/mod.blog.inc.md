# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.blog.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Blog Module (`mod.blog.inc`)

The Blog module provides a complete blogging system with article management, tagging, archiving, commenting, and RSS feed generation. It integrates with the NUOS platform's core utilities for database access, URL management, and permission control.

---

### **Global Variables**

| Name | Type | Description |
|------|------|-------------|
| `$blog_message` | `string` | Controls the current view mode (e.g., `edit`, `add`, `rss`). |
| `$blog_index` | `string` | Unique identifier for the current blog article. |
| `$blog_date` | `string` | Date filter for articles (format: `YYYYMMDD`). |
| `$blog_meta` | `string` | Tag filter for articles. |
| `$blog_page` | `int` | Pagination page number. |
| `$blog_edit_*` | `mixed` | Form fields for article editing (title, meta, text, etc.). |
| `$blog_code_edit_*` | `mixed` | Form fields for code template editing. |

---

### **Module Initialization**

#### **Purpose**
Initializes the Blog module by loading dependencies, validating permissions, and instantiating the `blog` class.

#### **Mechanisms**
1. **Dependency Loading**: Loads `blog` and `comment` libraries.
2. **Template Integration**: Merges blog data into the global template.
3. **Instance Setup**: Uses `CMS_CONTENT_INDEX` or a custom instance name.
4. **Permission Check**: Validates if the user has read/write/operator access.

#### **Usage**
- Automatically executed when the module is loaded.
- Fails silently if dependencies or permissions are missing.

---

### **Class: `blog`**
*(Assumed from context; not defined in this file.)*

#### **Methods Used in This File**
| Method | Description |
|--------|-------------|
| `blog::__construct($instance)` | Initializes the blog for a given instance. |
| `blog->enabled` | Checks if the blog is enabled. |
| `blog->reader` / `blog->writer` / `blog->operator` | Permission flags. |
| `blog->delete($index)` | Deletes an article. |
| `blog->add($title, $meta, $text, $status, $time, $sticky)` | Creates a new article. |
| `blog->edit($index, $title, $meta, $text, $status, $time, $sticky)` | Updates an existing article. |
| `blog->code_get($position)` | Retrieves code template for a position. |
| `blog->code_set($position, $text)` | Updates code template for a position. |
| `blog->code_parse($position, $replacement)` | Renders a code template with placeholders. |
| `blog->test_delete($index)` | Checks if the user can delete an article. |
| `blog->test_edit($index)` | Checks if the user can edit an article. |
| `blog->test_add()` | Checks if the user can create articles. |
| `blog->test_code_set()` | Checks if the user can edit code templates. |

---

### **Core Logic**

#### **1. Message Handling**
Processes form submissions for article and code template edits.

##### **`_edit` Message**
Handles article creation, updates, and deletions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$blog_edit_message` | `string` | Action to perform (`CMS_L_COMMAND_SAVE`, `CMS_L_COMMAND_DELETE`, etc.). |
| `$blog_edit_*` | `mixed` | Form fields (title, meta, text, status, etc.). |

**Return**: Updates `$success` or `$error` arrays with feedback.

**Mechanisms**:
- **Deletion**: Removes the article and associated comments.
- **Saving**: Validates input, sets publication time, and updates/creates the article.
- **Status Handling**: Supports `ACTIVE`, `INACTIVE`, and scheduled publication.

**Usage**:
- Triggered via form submissions in the `edit` view.

##### **`_code_edit` Message**
Handles updates to code templates (e.g., teaser, article header/footer).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$blog_code_edit_message` | `string` | Action to perform (`CMS_L_COMMAND_SAVE`). |
| `$blog_code_edit_position` | `string` | Template position (e.g., `CMS_DB_BLOG_CODE_POSITION_TEASER`). |
| `$blog_code_edit_text` | `string` | New template content. |

**Return**: Updates `$success` or `$error` arrays with feedback.

**Usage**:
- Triggered via form submissions in the `code_edit` view.

---

#### **2. View Rendering**
Renders different views based on `$blog_message`.

##### **`edit` / `add` View**
Displays a form for creating/editing articles.

**Parameters**:
- `$blog_index`: Article ID (empty for new articles).
- `$blog_edit_*`: Pre-filled form fields (for edits).

**Mechanisms**:
- **Time Selection**: Generates dropdowns for publication scheduling.
- **Tag Suggestions**: Fetches and displays popular tags.
- **Text Editor**: Integrates with `textcontrol` for rich text editing.
- **Permissions**: Shows/hides delete/edit buttons based on user access.

**Usage**:
- Accessed via `blog_message=edit` or `blog_message=add` in the URL.

##### **`code_edit` View**
Displays a form for editing code templates.

**Parameters**:
- `$blog_code_edit_position`: Template position (e.g., `CMS_DB_BLOG_CODE_POSITION_CONTROL`).

**Mechanisms**:
- **Placeholders**: Provides clickable tokens (e.g., `%title%`, `%url%`) for easy insertion.
- **Permissions**: Only shown to users with `test_code_set()` access.

**Usage**:
- Accessed via `blog_message=code_edit` in the URL.

##### **`rss` View**
Generates an RSS 2.0 feed for the blog.

**Mechanisms**:
- **Caching**: Uses `cms_cache()` to store the feed for 60 seconds.
- **Query**: Fetches the 50 most recent articles.
- **Enclosures**: Includes the first image from each article as an enclosure.
- **Headers**: Sends `Content-Type: application/rss+xml`.

**Usage**:
- Accessed via `blog_message=rss` in the URL.

##### **Article View**
Displays a single article with comments.

**Parameters**:
- `$blog_index`: Article ID.

**Mechanisms**:
- **Permissions**: Shows edit/delete buttons if allowed.
- **Meta Data**: Updates the template's title, description, and keywords.
- **Comments**: Embeds the `comment` module for the article.
- **Code Templates**: Renders templates for `BEFORE` and `AFTER` positions.

**Usage**:
- Accessed via `blog_index` in the URL.

##### **Overview View**
Displays a paginated list of articles with filtering by date/tag.

**Mechanisms**:
- **Pagination**: Uses `pagination()` to split results into pages.
- **Filtering**: Supports date (`blog_date`) and tag (`blog_meta`) filters.
- **Teasers**: Shows the first paragraph of each article.
- **Code Templates**: Renders templates for the `TEASER` position.

**Usage**:
- Default view when no `blog_message` or `blog_index` is specified.

---

#### **3. Control Panel**
Renders the blog's sidebar with navigation and tools.

##### **Archive Navigation**
Displays a hierarchical calendar for filtering articles by year/month/day.

**Mechanisms**:
- **Query**: Groups articles by year/month/day.
- **Styling**: Highlights active filters.

##### **Tag Cloud**
Displays a weighted list of tags for filtering articles.

**Mechanisms**:
- **Query**: Groups articles by tag and counts occurrences.
- **Styling**: Font size scales with tag popularity.

##### **Code Template**
Renders the `CONTROL` template (e.g., for RSS links or custom HTML).

##### **RSS Feed Link**
Provides a link to the blog's RSS feed.

---

### **Helper Functions**

#### **`select($array, $selected, $name, $numeric = FALSE)`**
*(Assumed from context; not defined in this file.)*

**Purpose**: Generates an HTML `<select>` dropdown.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | `array` | Key-value pairs for options. |
| `$selected` | `mixed` | Currently selected value. |
| `$name` | `string` | Name attribute for the `<select>`. |
| `$numeric` | `bool` | If `TRUE`, uses array values as both keys and values. |

**Usage**:
- Used for time selection in the `edit` view.

---

#### **`pagination($url, $page, $count, $label, $class)`**
*(Assumed from context; not defined in this file.)*

**Purpose**: Generates pagination links.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | `string` | Base URL with `\x1B%page%` placeholder. |
| `$page` | `int` | Current page number. |
| `$count` | `int` | Total number of pages. |
| `$label` | `string` | Label for the pagination (e.g., "Page"). |
| `$class` | `string` | CSS class for the pagination container. |

**Usage**:
- Used in the `overview` view to paginate articles.

---

#### **`insert($position)`**
*(Assumed from context; not defined in this file.)*

**Purpose**: Inserts content at predefined positions (e.g., `top_article`, `bottom_overview`).

**Usage**:
- Used to inject custom content into the blog layout.

---

### **Constants**

| Constant | Description |
|----------|-------------|
| `CMS_DB_BLOG_*` | Database column names (e.g., `CMS_DB_BLOG_TITLE`). |
| `CMS_DB_BLOG_STATUS_ACTIVE` | Article is published. |
| `CMS_DB_BLOG_STATUS_INACTIVE` | Article is unpublished. |
| `CMS_DB_BLOG_STICKY_ON` | Article is sticky (appears first). |
| `CMS_DB_BLOG_CODE_POSITION_*` | Positions for code templates (e.g., `TEASER`, `BEFORE`, `AFTER`, `CONTROL`). |
| `CMS_L_*` | Localized strings (e.g., `CMS_L_MOD_BLOG_001` for "Page"). |

---

### **Usage Examples**

#### **1. Displaying a Blog**
```php
// Load the blog module (typically via URL routing)
cms_application("blog");
```

#### **2. Creating an Article**
```php
// Navigate to the "add" view
$url = cms_url(["blog_message" => "add"]);
header("Location: $url");
```

#### **3. Editing an Article**
```php
// Navigate to the "edit" view for article ID "123"
$url = cms_url(["blog_message" => "edit", "blog_index" => "123"]);
header("Location: $url");
```

#### **4. Subscribing to RSS**
```php
// Link to the RSS feed
$rss_url = u(["blog_message" => "rss"]);
echo("<a href=\"$rss_url\">Subscribe</a>");
```

#### **5. Filtering Articles by Tag**
```php
// Filter articles by the "news" tag
$url = cms_url(["blog_meta" => "news"]);
header("Location: $url");
```

#### **6. Filtering Articles by Date**
```php
// Filter articles from January 2023
$url = cms_url(["blog_date" => "202301"]);
header("Location: $url");
```


<!-- HASH:a1e98bf56ab792eb95ab10539176286a -->
