# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.blog.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Blog Module

## Overview

The `mod.blog.inc` file is the main controller for the **Blog** module in the PWNC Web Platform. It handles all aspects of blog functionality including:

- **Article management**: Creating, editing, deleting, and viewing individual blog posts
- **Overview/listing**: Paginated list of articles with filtering by date and tags
- **RSS feed generation**: XML-based RSS 2.0 feed with caching
- **Code editing**: Inline code snippet management for blog templates
- **Archive navigation**: Year/month/day drill-down archive browser
- **Tag cloud**: Meta tag cloud with weighted font sizes
- **Comment integration**: Embedded comment system per article

The module follows a **state-machine pattern** driven by the `$blog_message` global variable, which determines the current action (e.g., `edit`, `add`, `code_edit`, `rss`). It uses the `blog` and `comment` library classes loaded via `cms_load()`.

## Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$blog_message` | string | Current action/state (e.g., `edit`, `add`, `code_edit`, `rss`) |
| `$blog_index` | string/int | Unique identifier of the current blog article |
| `$blog_date` | string | Date filter for archive navigation (YYYYMMDD format) |
| `$blog_meta` | string | Tag filter for article listing |
| `$blog_page` | int | Current page number for pagination |
| `$blog_edit_*` | mixed | Form field values for article editing |
| `$blog_code_edit_*` | mixed | Form field values for code editing |

## Initialization

### Library Loading

```php
if (! (cms_load("blog") && cms_load("comment")))
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};
```

**Purpose**: Loads the `blog` and `comment` libraries. If either fails to load, the module outputs an "unavailable" message and exits.

**Usage Context**: This is the first step in module initialization. The `blog` library provides the `blog` class with methods like `add()`, `edit()`, `delete()`, `code_set()`, `code_get()`, and `code_parse()`. The `comment` library provides the `comment` class for handling article comments.

### Template Integration

```php
$template = $GLOBALS["blog"]["template"] ?? NULL;
if ($template !== NULL)
    $template->query_data = array_merge($template->query_data, ["blog_index" => $blog_index]);
```

**Purpose**: Merges the current `blog_index` into the template's query data, making it available to the template engine for rendering.

### Instance Configuration

```php
if (isset($GLOBALS["blog"]["instance"]) && nstre($GLOBALS["blog"]["instance"]))
    $instance = $GLOBALS["blog"]["instance"];
else
    $instance = defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : "";
```

**Purpose**: Determines the blog instance identifier. This allows multiple blog instances to coexist on the same platform, each with its own set of articles.

### Blog Object Creation

```php
$blog = new blog($instance);
if (! $blog->enabled)
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};
```

**Purpose**: Instantiates the `blog` class with the determined instance. If the blog is not enabled, the module exits early.

### Permission Validation

```php
if (! ($blog->reader || $blog->writer || $blog->operator)) return;
```

**Purpose**: Checks if the current user has at least reader-level access. If not, the module exits silently without rendering any content.

## Message Handling

### Article Edit/Delete Operations

```php
switch ($blog_message)
{
case "_edit":
    switch ($blog_edit_message)
    {
    case CMS_L_COMMAND_DELETE:
        // Delete article and associated comments
    case CMS_L_COMMAND_CANCEL:
        // Cancel edit operation
    case CMS_L_MOD_BLOG_005:
        // Save and preview
    case CMS_L_COMMAND_SAVE:
        // Save article (add or edit)
    };
    break;
case "_code_edit":
    // Handle code snippet editing
};
```

**Purpose**: Processes form submissions for article management. The `_edit` state handles article creation, modification, and deletion. The `_code_edit` state handles inline code template editing.

**Parameters**:
- `$blog_edit_message`: Command from the form (delete, cancel, save, save+preview)
- `$blog_edit_title`: Article title
- `$blog_edit_meta`: Article tags/meta
- `$blog_edit_text`: Article content
- `$blog_edit_status`: Publication status (active, inactive, scheduled)
- `$blog_edit_time`: Scheduled publication time
- `$blog_edit_sticky`: Sticky flag

**Return Values**: None (outputs HTML directly)

**Inner Mechanisms**:
- Uses `sqlesc()` for SQL escaping
- Calls `$blog->add()` for new articles and `$blog->edit()` for existing ones
- Deletes associated comments when an article is removed
- Sets success/error messages using language constants

**Usage Example**:
```php
// When a user submits the article edit form:
// $blog_message = "_edit"
// $blog_edit_message = CMS_L_COMMAND_SAVE
// $blog_edit_title = "My New Article"
// $blog_edit_text = "Article content here..."
// The module will call $blog->add() or $blog->edit() accordingly
```

## Display Logic

### Error and Success Messages

```php
if (count($error))
    echo("<div class=\"response-error\">" . implode("<br>", $error) . "</div>");
if (nstre($success))
    echo("<div class=\"response-success\">" . $success . "</div>");
```

**Purpose**: Displays any error or success messages generated during form processing.

### Article Editing Form

**Purpose**: Renders the article editing interface with:
- Title input field
- Tag input with suggestions from existing tags
- Publication options (immediate, scheduled, draft)
- Sticky post checkbox
- Rich text editor with image/link/token insertion buttons
- Save, cancel, and save+preview buttons

**Key Features**:
- Tag suggestions are retrieved from the database and displayed as clickable links
- The text editor supports external image, link, and token insertion via interface modules
- Scheduled publication uses HTML5 `datetime-local` input
- JavaScript automatically switches to "scheduled" mode when a date is selected

### Code Editing Form

**Purpose**: Provides an interface for editing inline code snippets that appear in blog templates.

**Parameters**:
- `$blog_code_edit_position`: Position identifier (before, after, teaser, control)
- `$blog_code_edit_text`: Code content

**Inner Mechanisms**:
- Retrieves existing code via `$blog->code_get()`
- Saves code via `$blog->code_set()`
- Provides placeholder tokens for dynamic content insertion

### RSS Feed Generation

**Purpose**: Generates an RSS 2.0 XML feed of the latest 50 published articles.

**Key Features**:
- Uses 60-second caching to reduce database load
- Includes article titles, links, descriptions, and publication dates
- Supports media enclosures for images embedded in articles
- Sets appropriate HTTP headers (`Content-Type: application/rss+xml`)

**Inner Mechanisms**:
- Clears all output buffers before sending headers
- Checks cache using `cms_cache_time()` and serves cached content if fresh
- Builds XML manually with proper escaping via `x()`
- Processes article text to extract first paragraph and first image
- Caches the final XML output permanently

### Article Display

**Purpose**: Renders a single blog article with full content, metadata, and comments.

**Key Features**:
- Displays article title, publication date, and author
- Parses article text with `parse_text()` for formatting
- Shows code snippets before and after the article content
- Integrates the comment system with proper instance identification
- Provides edit/delete commands based on user permissions

**Inner Mechanisms**:
- Queries the database for the specific article by index
- Sets HTTP 410 (Gone) status for non-existent articles
- Uses `insert()` for template hooks (top_article, bottom_article)
- Passes replacement variables to code parsing for dynamic content

### Article Overview/Listing

**Purpose**: Displays a paginated list of blog articles with filtering capabilities.

**Key Features**:
- 10 articles per page
- Filtering by date (year/month/day) and tags
- Comment count display (if comment module is available)
- Sticky articles appear at the top
- Scheduled articles are marked differently
- Pagination controls with proper URL generation

**Inner Mechanisms**:
- Calculates total article count for pagination
- Builds complex SQL queries with JOINs for meta tags and comments
- Uses `pagination()` helper for page navigation
- Applies different CSS classes based on article status and stickiness

### Archive Navigation

**Purpose**: Provides a hierarchical archive browser allowing users to drill down by year, month, and day.

**Key Features**:
- Year-level view showing all years with article counts
- Month-level view (when year is selected) showing months
- Day-level calendar view (when month is selected) showing individual days
- Active selections are highlighted
- Calendar view shows weekday headers and proper day alignment

**Inner Mechanisms**:
- Uses SQL `YEAR()`, `MONTH()`, `DAY()` functions on `FROM_UNIXTIME()` for date extraction
- Generates calendar using `mktime()` for date calculations
- Applies different CSS classes for active/inactive selections

### Tag Cloud

**Purpose**: Displays a weighted tag cloud based on article metadata.

**Key Features**:
- Font size varies based on tag frequency (more articles = larger font)
- Active tag is highlighted
- Tags are sorted alphabetically
- Links allow filtering articles by tag

**Inner Mechanisms**:
- Calculates min/max tag counts for font size scaling
- Uses linear interpolation for font size calculation
- Applies `utf8_ucwords()` for proper capitalization

### Control Panel

**Purpose**: Renders the blog control section with archive navigation, tag cloud, code editing, and RSS feed link.

**Key Features**:
- Archive navigation on the left
- Tag cloud in the middle
- Code editing button (if permissions allow)
- RSS feed link
- Permission summary at the bottom

**Inner Mechanisms**:
- Uses `insert()` for template hooks (top_control, bottom_control)
- Calls `permission()` to display user permission levels
- Integrates with the comment system for instance identification

## Usage Examples

### Basic Blog Display

```php
// In a template file:
$blog_message = "";  // Default: show overview
$blog_index = "";    // No specific article
$blog_date = "";     // No date filter
$blog_meta = "";     // No tag filter
$blog_page = 0;      // First page

// Include the module:
include("module/blog/mod.blog.inc");
```

### Viewing a Specific Article

```php
$blog_message = "";
$blog_index = "123";  // Article ID
// Module will display the full article with comments
```

### Editing an Article

```php
$blog_message = "edit";
$blog_index = "123";  // Article to edit
// Module will display the editing form pre-filled with article data
```

### Creating a New Article

```php
$blog_message = "add";
$blog_index = "";  // Empty for new article
// Module will display an empty editing form
```

### RSS Feed Access

```php
$blog_message = "rss";
// Module will output RSS XML with appropriate headers
```

## Language Constants

The module uses numerous language constants for internationalization:

| Constant | Purpose |
|----------|---------|
| `CMS_L_MOD_BLOG_001` | Pagination text |
| `CMS_L_MOD_BLOG_002` | "Add article" heading |
| `CMS_L_MOD_BLOG_003` | "Title" label |
| `CMS_L_MOD_BLOG_004` | "Tags" label |
| `CMS_L_MOD_BLOG_005` | "Save and preview" button |
| `CMS_L_MOD_BLOG_006` | Success message for deletion |
| `CMS_L_MOD_BLOG_007` | Success message for edit |
| `CMS_L_MOD_BLOG_008` | Success message for add |
| `CMS_L_MOD_BLOG_009` | "Edit" button |
| `CMS_L_MOD_BLOG_010` | "Read more" link |
| `CMS_L_MOD_BLOG_011` | "Overview" button |
| `CMS_L_MOD_BLOG_012` | Error message for deletion |
| `CMS_L_MOD_BLOG_013` | Error message for edit |
| `CMS_L_MOD_BLOG_014` | Error message for add |
| `CMS_L_MOD_BLOG_015` | Archive tooltip format |
| `CMS_L_MOD_BLOG_016` | "Edit code" heading |
| `CMS_L_MOD_BLOG_017` | Success message for code save |
| `CMS_L_MOD_BLOG_018` | Error message for code save |
| `CMS_L_MOD_BLOG_019` | "Archive" heading |
| `CMS_L_MOD_BLOG_020` | "Tags" heading |
| `CMS_L_MOD_BLOG_021` | "Clear filter" link |
| `CMS_L_MOD_BLOG_022` | "Publication options" legend |
| `CMS_L_MOD_BLOG_023` | "Publish immediately" option |
| `CMS_L_MOD_BLOG_024` | "Draft" option |
| `CMS_L_MOD_BLOG_025` | "Publication date" aria-label |
| `CMS_L_MOD_BLOG_029` | Delete confirmation message |
| `CMS_L_MOD_BLOG_030` | "RSS feed" link |
| `CMS_L_MOD_BLOG_031` | Default RSS title |
| `CMS_L_MOD_BLOG_032` | "Placeholders" label |
| `CMS_L_MOD_BLOG_033` | Author format string |
| `CMS_L_MOD_BLOG_034` | "%author%" placeholder |
| `CMS_L_MOD_BLOG_035` | "%user%" placeholder |
| `CMS_L_MOD_BLOG_036` | "Article not found" message |
| `CMS_L_MOD_BLOG_037` | "Sticky" checkbox label |
| `CMS_L_MOD_BLOG_038` | Comment count format |
| `CMS_L_MOD_BLOG_040` | "Content" label |
| `CMS_L_MOD_BLOG_041` | "Code" label |
| `CMS_L_MOD_BLOG_042` | "Scheduled" aria-label |
| `CMS_L_MOD_BLOG_043` | "Article not found" heading |
| `CMS_L_MOD_BLOG_044` | Error heading |
| `CMS_L_MOD_BLOG_045` | Error description |

## Database Schema References

The module interacts with several database tables through constants:

- `CMS_DB_BLOG`: Main articles table
- `CMS_DB_BLOG_INSTANCE`: Instance identifier column
- `CMS_DB_BLOG_INDEX`: Article ID column
- `CMS_DB_BLOG_STATUS`: Publication status column
- `CMS_DB_BLOG_TIME`: Publication timestamp column
- `CMS_DB_BLOG_STICKY`: Sticky flag column
- `CMS_DB_BLOG_TITLE`: Article title column
- `CMS_DB_BLOG_META`: Tags/meta column
- `CMS_DB_BLOG_TEXT`: Article content column
- `CMS_DB_BLOG_OWNER`: Author ID column
- `CMS_DB_BLOG_META_LINK`: Tag-to-article linking table
- `CMS_DB_BLOG_META_TERM`: Tag definitions table
- `CMS_DB_BLOG_META_TERM_TEXT`: Tag text column
- `CMS_DB_BLOG_META_TERM_INDEX`: Tag ID column
- `CMS_DB_BLOG_META_LINK_ARTICLE`: Link to article ID
- `CMS_DB_BLOG_META_LINK_TERM`: Link to tag ID
- `CMS_DB_BLOG_CODE_POSITION_*`: Code position constants
- `CMS_DB_BLOG_STATUS_ACTIVE`: Active status value
- `CMS_DB_BLOG_STATUS_INACTIVE`: Inactive status value
- `CMS_DB_BLOG_STICKY_ON`: Sticky enabled value
- `CMS_DB_BLOG_STICKY_OFF`: Sticky disabled value
- `CMS_DB_COMMENT`: Comments table
- `CMS_DB_COMMENT_INSTANCE`: Comment instance identifier
- `CMS_DB_COMMENT_STATUS`: Comment status column
- `CMS_DB_COMMENT_STATUS_ACTIVE`: Active comment status
- `CMS_DB_COMMENT_INDEX`: Comment ID column

## Helper Functions Used

- `sqlesc()`: SQL string escaping
- `x()`: XML/HTML escaping
- `cms_url()`: URL generation with parameter merging
- `cms_param()`: State parameter management
- `cms_cache()`: Caching mechanism
- `cms_cache_time()`: Cache timestamp checking
- `cms_cache_notouch()`: Retrieve cached content without updating timestamp
- `cms_load()`: Library loading
- `cms_available()`: Module availability checking
- `cms_permission()`: Permission checking
- `parse_text()`: Text parsing/formatting
- `remove_format()`: Strip formatting from text
- `stripspaces()`: Remove extra whitespace
- `first_paragraph()`: Extract first paragraph
- `first_words()`: Extract first words
- `friendly_date()`: Human-readable date formatting
- `get_first_image()`: Extract first image from text
- `translate_url()`: Resolve logical URLs to physical
- `image_process()`: Process/resize images
- `image_path()`: Get filesystem path for images
- `get_mime_type()`: Determine MIME type
- `pagination()`: Generate pagination controls
- `insert()`: Template hook insertion
- `month()`: Month name from number
- `weekday()`: Weekday name from number
- `permission_get_name()`: Get user display name
- `permission()`: Display permission summary
- `image()`: Render image HTML
- `jscript()`: Generate JavaScript
- `q()`: JavaScript string encoding
- `qr()`: URL-encoded JavaScript string
- `qx()`: XML+URL encoded JavaScript string
- `u()`: Smart URL generation
- `r()`: Raw URL encoding
- `stre()`: Check if string is empty
- `nstre()`: Check if string is not empty
- `streq()`: String equality check
- `nstreq()`: String inequality check
- `utf8_substr()`: Multibyte-safe substring
- `utf8_ucwords()`: Multibyte-safe uppercase words


<!-- HASH:a777ad4a470a2906e66a2a985e3131b0 -->
