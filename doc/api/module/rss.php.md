# NUOS API Documentation

[← Index](../README.md) | [`module/rss.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## RSS Feed Generator Module

This module generates an RSS 2.0 feed for content channels in the NUOS platform. It retrieves published content items from a specified channel, formats them into valid RSS XML, and outputs the result with proper caching mechanisms.

---

### Dependencies and Initialization

#### Required Libraries
| Library      | Purpose                                                                 |
|--------------|-------------------------------------------------------------------------|
| `content`    | Provides access to content items and their metadata                     |
| `directory`  | Enables navigation through content hierarchy for category assignment   |
| `rss`        | Core RSS feed generation logic and channel management                   |

#### Initial Setup
1. Sets the HTTP response header to `application/rss+xml` with UTF-8 encoding
2. Instantiates the `rss` class for channel data management

---

### Core Functionality

#### Channel Validation
```php
if (empty($rss_channel)) return;
if (! $rss->data->get($rss_channel)) return;
```
- **Purpose**: Ensures a valid channel is specified before proceeding
- **Parameters**:
  - `$rss_channel`: String identifier for the content channel
- **Behavior**:
  - Terminates execution if no channel is specified or if the channel is invalid
- **Usage Context**: Must be set before including this module

---

#### Configuration Parameters

| Parameter      | Type    | Default | Description                                                                 |
|----------------|---------|---------|-----------------------------------------------------------------------------|
| `$rss_limit`   | integer | NULL    | Maximum number of items to include in the feed (minimum 1)                 |
| `$rss_order`   | string  | NULL    | Sorting order: `"published"` (default) or `"modified"`                     |

---

#### Caching Mechanism
```php
$cache_key = "rss.$rss_channel.$rss_limit.$rss_order." . CMS_LANGUAGE;
$cache_time = cms_cache_time($cache_key);
if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) {
    echo(cms_cache_notouch($cache_key));
    exit();
}
```
- **Purpose**: Implements 60-second cache invalidation to reduce database load
- **Cache Key Components**:
  - Channel identifier
  - Item limit
  - Sorting order
  - Current language
- **Behavior**:
  - Serves cached content if available and fresh
  - Proceeds with generation if cache is stale or missing

---

### RSS Feed Structure

#### Channel Metadata
| Element           | Source                                                                 |
|-------------------|------------------------------------------------------------------------|
| `<title>`         | Channel name from `rss` data store                                    |
| `<link>`          | Channel link URL                                                      |
| `<description>`   | Channel description                                                   |
| `<language>`      | Current CMS language (CMS_LANGUAGE)                                  |
| `<pubDate>`       | Most recent publication date from content items                       |
| `<lastBuildDate>` | Current server time                                                   |
| `<category>`      | Channel category (if specified)                                       |
| `<generator>`     | CMS identifier (CMS_IDENTIFIER)                                       |
| `<image>`         | Channel image (falls back to default RSS icon)                        |

#### Item Generation
```php
while ($resultrow = mysql_fetch_assoc($result)) {
    // Item processing...
}
```
- **Query Parameters**:
  - Selects content items with `CMS_CONTENT_STATUS_PUBLICATION` status
  - Filters by channel membership
  - Orders by specified sort field (published/modified time)
  - Limits to `$rss_limit` items if specified

#### Item Elements
| Element           | Content                                                                 |
|-------------------|-------------------------------------------------------------------------|
| `<title>`         | Content title with modification/publishing indicator for recent items  |
| `<link>`          | Translated content URL (`content://` protocol)                         |
| `<description>`   | Parsed content description (HTML allowed)                              |
| `<category>`      | Hierarchical categories from directory structure                       |
| `<enclosure>`     | Image attachment with dimensions, size, and MIME type                  |
| `<guid>`          | Content index as permanent identifier                                  |
| `<pubDate>`       | Publication date in RFC 2822 format                                    |

---

### Key Helper Functions

#### `translate_url()`
- **Purpose**: Resolves logical content URLs to physical paths
- **Usage**:
  ```php
  translate_url("content://$index", NULL, CMS_LANGUAGE, TRUE)
  ```
- **Parameters**:
  | Parameter | Type    | Description                          |
  |-----------|---------|--------------------------------------|
  | Address   | string  | Logical URL (e.g., `content://123`)  |
  | Params    | array   | Additional URL parameters            |
  | Language  | string  | Target language                      |
  | Absolute  | boolean | Force absolute URL generation        |

#### `image_process()`
- **Purpose**: Generates optimized image URLs with specified dimensions
- **Parameters**:
  | Parameter | Type    | Description               |
  |-----------|---------|---------------------------|
  | URL       | string  | Source image URL          |
  | Width     | integer | Maximum width (500px)     |
  | Height    | integer | Maximum height (500px)    |

#### `x()`
- **Purpose**: XML-escapes content for RSS output
- **Escapes**: `&`, `"`, `'`, `<`, `>`

---

### Output and Caching
```php
cms_cache($cache_key, $buffer, TRUE);
echo($buffer);
```
- **Final Steps**:
  1. Stores generated XML in permanent cache
  2. Outputs the complete RSS feed
- **Cache Lifetime**: Permanent until invalidated by content changes

---

### Usage Scenarios

#### Basic Feed Generation
```php
// In calling script:
$rss_channel = "news";
$rss_limit = 10;
include("module/rss.php");
```

#### Advanced Configuration
```php
$rss_channel = "blog";
$rss_limit = 5;
$rss_order = "modified";  // Show recently modified items first
include("module/rss.php");
```

#### Integration with Frontend
- Access via URL: `/module/rss.php?rss_channel=news`
- Can be linked in HTML:
  ```html
  <link rel="alternate" type="application/rss+xml"
        title="News Feed"
        href="/module/rss.php?rss_channel=news" />
  ```

---

### Error Handling
- Silently exits if:
  - No channel is specified
  - Specified channel doesn't exist
- Gracefully handles:
  - Missing images (no enclosure)
  - Empty categories
  - Remote image fetching failures

---

### Performance Considerations
1. **Database Optimization**:
   - Single query for channel metadata
   - Single query for content items with LIMIT clause
2. **Caching**:
   - 60-second memory cache
   - Permanent storage cache
3. **Image Processing**:
   - Dimension limits prevent oversized images
   - Local file size checks avoid remote requests when possible


<!-- HASH:f6629f84211891a3df67f9d4589271b4 -->
