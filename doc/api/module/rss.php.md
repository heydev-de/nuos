# PWNC API Documentation

[← Index](../README.md) | [`module/rss.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/rss.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## rss.php Module

The `rss.php` module generates and serves an RSS 2.0 feed for a specified content channel. It retrieves published content from the database, formats it according to RSS specifications, and caches the output for performance.

### Overview

This module:
- Loads required libraries (`content`, `directory`, `rss`)
- Sets the appropriate `Content-Type` header for RSS XML
- Validates the requested RSS channel
- Queries the database for published content items
- Builds a complete RSS 2.0 XML document with channel metadata and item entries
- Caches the generated feed for 60 seconds to reduce database load
- Outputs the final XML feed

### Key Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `$rss_channel` | `NULL` | The content channel identifier for which to generate the RSS feed |
| `$rss_limit` | `NULL` | Maximum number of items to include in the feed |
| `$rss_order` | `NULL` | Sorting order for items (`"published"` or `"modified"`) |
| `$cache_key` | Dynamic | Unique cache key based on channel, limit, order, and language |
| `$cache_time` | Dynamic | Timestamp of last cache update |
| `$time` | `time()` | Current Unix timestamp |

### Constants Used

| Constant | Description |
|----------|-------------|
| `CMS_DB_CONTENT` | Database table name for content |
| `CMS_DB_CONTENT_INDEX` | Column name for content index |
| `CMS_DB_CONTENT_TIME` | Column name for content modification time |
| `CMS_DB_CONTENT_PUBLISHER_TIME` | Column name for content publication time |
| `CMS_DB_CONTENT_TITLE` | Column name for content title |
| `CMS_DB_CONTENT_DESCRIPTION` | Column name for content description |
| `CMS_DB_CONTENT_IMAGE` | Column name for content image |
| `CMS_DB_CONTENT_STATUS` | Column name for content status |
| `CMS_DB_CONTENT_CHANNEL` | Column name for content channel |
| `CMS_CONTENT_STATUS_PUBLICATION` | Status value indicating published content |
| `CMS_LANGUAGE` | Current language code |
| `CMS_IDENTIFIER` | Platform identifier string |
| `CMS_IMAGES_URL` | Base URL for images |
| `CMS_ROOT_URL` | Root URL of the site |
| `CMS_L_MOD_RSS_001` | Language string for "Updated" |
| `CMS_L_MOD_RSS_002` | Language string for "Published" |

### Execution Flow

1. **Library Loading**: Loads `content`, `directory`, and `rss` libraries
2. **Header Setting**: Sends `Content-Type: application/rss+xml; charset=utf-8`
3. **Channel Validation**: Checks if `$rss_channel` is valid
4. **Parameter Processing**: Processes `$rss_limit` and `$rss_order` parameters
5. **Cache Check**: Returns cached content if available and fresh (within 60 seconds)
6. **XML Generation**: Builds the RSS XML structure
7. **Database Queries**: Retrieves channel metadata and content items
8. **Item Processing**: Formats each content item as an RSS `<item>`
9. **Caching**: Stores the generated feed in cache
10. **Output**: Echoes the final XML feed

### RSS Feed Structure

The generated feed includes:
- Channel metadata (title, link, description, language, dates, image)
- Multiple content items with titles, links, descriptions, categories, enclosures, and GUIDs

### Usage Example

To request an RSS feed for a channel named "news":

```
GET /module/rss.php?rss_channel=news&rss_limit=10&rss_order=published
```

This would return an RSS 2.0 feed containing the 10 most recently published items from the "news" channel.

### Caching Mechanism

The module uses a dual-layer caching system:
- Cache key includes channel name, item limit, sort order, and language
- Cache is considered fresh if updated within the last 60 seconds
- Fresh cache is returned immediately without database queries
- Generated feeds are cached permanently after creation

### Item Processing Details

Each content item in the feed includes:
- **Title**: Content title with optional "Updated" or "Published" timestamp suffix
- **Link**: Translated URL to the content item
- **Description**: Parsed content description
- **Categories**: Derived from directory structure hierarchy
- **Enclosure**: Optional media enclosure with URL, length, and MIME type
- **GUID**: Permanent identifier for the item
- **Publication Date**: RFC 2822 formatted publication timestamp

### Directory-Based Category Generation

The module traverses the directory structure to determine category paths for each content item:
- Uses a stack-based approach to track container hierarchy
- Matches content items to their parent directories
- Generates category entries with domain URLs and human-readable names


<!-- HASH:314228dfef57870641a90c5083d01d75 -->
