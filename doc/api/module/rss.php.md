# PWNC API Documentation

[← Index](../README.md) | [`module/rss.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/rss.php)

- **Version:** `26.8.11.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Module (`module/rss.php`)

The RSS module generates RSS 2.0 feeds for content channels in the PWNC Web Platform. It retrieves published content items from a specified channel, formats them into valid RSS XML, and outputs the result with proper caching mechanisms. The module supports customizable limits, sorting orders, and multilingual content.

---

### Dependencies
| Library | Purpose |
|---------|---------|
| `content` | Accesses content items and metadata |
| `directory` | Resolves content categorization and navigation structure |
| `rss` | Provides channel configuration and RSS-specific logic |

---

### Constants & Configuration
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_CONTENT_TIME` | Database field | Last modification timestamp of content |
| `CMS_DB_CONTENT_PUBLISHER_TIME` | Database field | Publication timestamp of content |
| `CMS_CONTENT_STATUS_PUBLICATION` | String | Status value for published content |
| `CMS_L_MOD_RSS_001` | Localization key | Label for "Published" in RSS items |
| `CMS_L_MOD_RSS_002` | Localization key | Label for "Updated" in RSS items |

---

## Class: `rss`

The `rss` class (instantiated as `$rss`) manages RSS channel configurations and metadata. It provides access to channel-specific settings such as titles, descriptions, and links.

### Properties
| Property | Type | Description |
|----------|------|-------------|
| `$data` | `data` object | Stores channel configurations (names, descriptions, links, etc.) |

---

### Workflow Overview

1. **Initialization**
   - Loads required libraries (`content`, `directory`, `rss`).
   - Sets the HTTP response header to `application/rss+xml`.
   - Instantiates the `rss` class to access channel data.

2. **Channel Validation**
   - Exits if no channel is specified (`$rss_channel` is blank).
   - Exits if the specified channel does not exist in `$rss->data`.

3. **Parameter Handling**
   - **Limit**: Number of items to display (default: unlimited).
   - **Order**: Sorting order (`published` or `modified`).

4. **Caching**
   - Generates a cache key based on channel, limit, order, and language.
   - Serves cached output if available and not older than 60 seconds.

5. **RSS Generation**
   - Constructs the RSS XML structure with channel metadata.
   - Queries the database for published content items in the specified channel.
   - Processes each item to generate `<item>` elements with titles, links, descriptions, categories, enclosures, and publication dates.

6. **Output & Caching**
   - Outputs the generated RSS XML.
   - Caches the result permanently for future requests.

---

### Key Functions & Logic

#### `cms_cache_time($cache_key)`
**Purpose**: Retrieves the timestamp of a cached item.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$cache_key` | `string` | Unique identifier for the cached item |

**Return Value**: `int|FALSE` – Timestamp of the cached item or `FALSE` if not found.

**Usage Context**: Determines if the cached RSS feed is still valid.

---

#### `cms_cache_notouch($cache_key)`
**Purpose**: Retrieves cached content without updating its access time.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$cache_key` | `string` | Unique identifier for the cached item |

**Return Value**: `string|FALSE` – Cached content or `FALSE` if not found.

**Usage Context**: Serves cached RSS feeds without modifying cache metadata.

---

#### `translate_url($addr, $param = NULL, $language = NULL, $absolute = FALSE)`
**Purpose**: Resolves logical URLs (e.g., `content://`, `directory://`) into physical URLs.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$addr` | `string` | Logical URL (e.g., `content://123`) |
| `$param` | `array|NULL` | Additional query parameters |
| `$language` | `string|NULL` | Language code for multilingual URLs |
| `$absolute` | `bool` | Whether to generate an absolute URL |

**Return Value**: `string` – Resolved physical URL.

**Usage Context**: Generates links for RSS items and channel metadata.

**Example**:
```php
$url = translate_url("content://42", NULL, CMS_LANGUAGE, TRUE);
// Output: "https://example.com/content/42?lang=en"
```

---

#### `image_process($url, $width, $height)`
**Purpose**: Processes an image URL to generate a resized version.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | Original image URL |
| `$width` | `int` | Maximum width of the processed image |
| `$height` | `int` | Maximum height of the processed image |

**Return Value**: `string` – URL of the processed image.

**Usage Context**: Generates image enclosures for RSS items.

**Example**:
```php
$processed_url = image_process("media://image.jpg", 500, 500);
// Output: "https://example.com/media/image_500x500.jpg"
```

---

#### `friendly_date($timestamp)`
**Purpose**: Converts a Unix timestamp into a human-readable date string.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$timestamp` | `int` | Unix timestamp |

**Return Value**: `string` – Formatted date (e.g., "3 days ago").

**Usage Context**: Appends relative publication dates to RSS item titles.

---

### Usage Example

#### Generating an RSS Feed for a Blog Channel
1. **URL Parameters**:
   - `rss_channel=blog` (specifies the channel)
   - `rss_limit=10` (limits to 10 items)
   - `rss_order=published` (sorts by publication date)

2. **Request**:
   ```http
   GET /module/rss.php?rss_channel=blog&rss_limit=10&rss_order=published
   ```

3. **Output**:
   ```xml
   <?xml version="1.0" encoding="utf-8"?>
   <rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
     <channel>
       <atom:link href="https://example.com/module/rss.php?rss_channel=blog&amp;rss_limit=10&amp;rss_order=published" rel="self" type="application/rss+xml"/>
       <title>Blog</title>
       <link>https://example.com/blog</link>
       <description>Latest blog posts</description>
       <language>en</language>
       <pubDate>Mon, 01 Jan 2023 12:00:00 +0000</pubDate>
       <lastBuildDate>Mon, 01 Jan 2023 12:30:00 +0000</lastBuildDate>
       <generator>PWNC Web Platform</generator>
       <image>
         <url>https://example.com/images/blog_rss.svg</url>
         <title>Blog</title>
         <link>https://example.com/blog</link>
       </image>
       <item>
         <title>Hello World (Published 3 days ago)</title>
         <link>https://example.com/content/42</link>
         <description>&lt;p&gt;Welcome to our blog!&lt;/p&gt;</description>
         <category domain="https://example.com/blog">Technology</category>
         <enclosure url="https://example.com/media/image_500x500.jpg" length="12345" type="image/jpeg"/>
         <guid isPermaLink="false">42</guid>
         <pubDate>Mon, 01 Jan 2023 12:00:00 +0000</pubDate>
       </item>
     </channel>
   </rss>
   ```

---

### Caching Strategy
- **Key**: `"rss.$rss_channel.$rss_limit.$rss_order." . CMS_LANGUAGE`
- **Lifetime**: 60 seconds (short-lived to ensure freshness).
- **Storage**: Dual-layer (RAM and permanent storage).
- **Purpose**: Reduces database queries and improves performance for frequent requests.

---

### Error Handling
- **No Channel**: Exits silently if `$rss_channel` is blank.
- **Invalid Channel**: Exits silently if the channel does not exist in `$rss->data`.
- **Database Errors**: Assumes valid connections; no explicit error handling for query failures.


<!-- HASH:314228dfef57870641a90c5083d01d75 -->
