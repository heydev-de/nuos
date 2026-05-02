# NUOS API Documentation

[← Index](../README.md) | [`module/rss.php`](https://github.com/heydev-de/nuos/blob/main/nuos/module/rss.php)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## RSS Feed Generator Module

This module generates an RSS 2.0 feed for content channels in the NUOS platform. It retrieves published content items from a specified channel, formats them according to the RSS specification, and outputs the result with proper XML headers. The module supports caching to improve performance and reduce database load.

---

### Dependencies and Initialization

#### Required Libraries
| Library      | Purpose                                                                 |
|--------------|-------------------------------------------------------------------------|
| `content`    | Provides access to content items and their metadata                     |
| `directory`  | Used for resolving content categorization and navigation structure      |
| `rss`        | Core RSS feed generation logic and channel management                   |

#### Initial Setup
- Sets the HTTP `Content-Type` header to `application/rss+xml; charset=utf-8`
- Instantiates the `rss` class to access channel data
- Validates the requested channel (`$rss_channel`) and exits if invalid or missing

---

### Parameters

| Parameter      | Type     | Default | Description                                                                                     |
|----------------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$rss_channel` | string   | (none)  | **Required.** Identifier of the content channel to generate the feed for                        |
| `$rss_limit`   | int      | NULL    | Maximum number of items to include in the feed. If NULL, all items are included                |
| `$rss_order`   | string   | NULL    | Sorting order of items: `"published"` (default) or `"modified"`                                |

---

### Caching Mechanism

#### Cache Key
```
"rss.$rss_channel.$rss_limit.$rss_order." . CMS_LANGUAGE
```

#### Cache Logic
- Checks if a valid cache entry exists and is less than 60 seconds old
- If valid, outputs the cached content and exits
- If invalid or missing, generates fresh content and caches it permanently

---

### Core Functions and Logic

#### Feed Structure Generation
The module constructs an RSS 2.0-compliant XML document with the following elements:

| Element            | Source / Logic                                                                                     |
|--------------------|----------------------------------------------------------------------------------------------------|
| `<title>`          | Channel name from `rss->data`                                                                      |
| `<link>`           | Channel link from `rss->data`                                                                      |
| `<description>`    | Channel description from `rss->data`                                                               |
| `<language>`       | Current language (`CMS_LANGUAGE`)                                                                  |
| `<pubDate>`        | Most recent publication date of content in the channel                                             |
| `<lastBuildDate>`  | Current server time (RFC 2822 format)                                                              |
| `<category>`       | Channel category from `rss->data` (if set)                                                         |
| `<generator>`      | NUOS platform identifier (`CMS_IDENTIFIER`)                                                        |
| `<image>`          | Channel image (fallback to default RSS icon)                                                       |
| `<item>`           | Repeated for each content item in the channel                                                      |

---

### `rss()` Class Usage

#### `rss->data->get($channel, $field)`
**Purpose:**
Retrieves metadata for a given RSS channel.

**Parameters:**
| Name      | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `$channel`| string | Channel identifier                               |
| `$field`  | string | Field name: `"name"`, `"link"`, `"description"`, `"category"`, `"image"` |

**Return Value:**
- `string`: The localized value of the field
- `FALSE`: If the channel or field does not exist

**Usage Context:**
Used to populate channel-level metadata in the RSS feed.

---

### Database Queries

#### Content Item Retrieval
```sql
SELECT index, time, publisher_time, title, description, image
FROM content_table
WHERE status = 'publication'
  AND channel LIKE '%/channel_id/%'
ORDER BY order_field DESC
[LIMIT limit]
```

- **`order_field`**: Determined by `$rss_order` (`CMS_DB_CONTENT_PUBLISHER_TIME` or `CMS_DB_CONTENT_TIME`)
- **`channel_id`**: Escaped using `sqlesc($rss_channel)`
- **`limit`**: Applied only if `$rss_limit` is set

---

### Item-Level Processing

#### Title Formatting
- If the item was **published** (not modified), uses `CMS_L_MOD_RSS_001` ("Published on")
- If the item was **modified**, uses `CMS_L_MOD_RSS_002` ("Updated on")
- Appends a friendly date if the item is less than 3 days old

#### Link Generation
- Uses `translate_url("content://$index", ...)` to generate a public URL for the content item

#### Description Parsing
- Uses `parse_text()` to convert stored markup into display-ready HTML

#### Category Resolution
- Traverses the `directory` structure to find all containers that reference the content item
- Generates `<category>` elements with `domain` attributes pointing to the container URL

#### Enclosure Support
- If the content item has an associated image:
  - Processes the image to a maximum of 500x500 pixels using `image_process()`
  - Determines MIME type and file size (local or remote)
  - Adds an `<enclosure>` element if both are available

#### GUID
- Uses the content item's internal index as a permanent, non-URL identifier

#### Publication Date
- Always uses the publisher time (`CMS_DB_CONTENT_PUBLISHER_TIME`) in RFC 2822 format

---

### Utility Functions Used

| Function          | Purpose                                                                                     |
|-------------------|---------------------------------------------------------------------------------------------|
| `l($string)`      | Localizes a string using the current language                                               |
| `x($string)`      | Escapes XML special characters (`"`, `'`, `&`, `<`, `>`)                                    |
| `u($params)`      | Generates a URL with the given parameters                                                   |
| `sqlesc($value)`  | Escapes a value for safe use in SQL queries (recursive for arrays)                          |
| `translate_url()` | Resolves logical URLs (e.g., `content://`, `image://`) to physical URLs                     |
| `image_process()` | Resizes and processes images                                                                |
| `image_path()`    | Converts a URL to a local filesystem path                                                   |
| `get_mime_type()` | Determines the MIME type of a file                                                          |
| `get_headers()`   | Fetches HTTP headers for remote resources                                                   |
| `friendly_date()` | Converts a timestamp to a human-readable relative date (e.g., "2 hours ago")                |
| `parse_text()`    | Converts stored text markup (e.g., Markdown) to HTML                                        |

---

### Usage Scenarios

#### 1. Public RSS Feed
- Accessed via a URL like `/rss?rss_channel=news&rss_limit=10`
- Used by feed readers (e.g., Feedly, Inoreader) to syndicate content

#### 2. Dynamic Channel Feeds
- Supports multiple channels (e.g., "news", "blog", "events")
- Each channel can have its own metadata (title, description, image)

#### 3. Performance Optimization
- Caching reduces database load for frequently accessed feeds
- Cache is invalidated after 60 seconds to ensure freshness

#### 4. Integration with Frontend
- Can be linked from `<head>` using:
  ```html
  <link rel="alternate" type="application/rss+xml" title="News" href="/rss?rss_channel=news" />
  ```

---

### Example Output
```xml
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
  <channel>
    <atom:link href="https://example.com/rss?rss_channel=news" rel="self" type="application/rss+xml"/>
    <title>Company News</title>
    <link>https://example.com/news</link>
    <description>Latest updates from our company</description>
    <language>en</language>
    <pubDate>Mon, 01 Jan 2024 12:00:00 +0000</pubDate>
    <lastBuildDate>Mon, 01 Jan 2024 12:30:00 +0000</lastBuildDate>
    <generator>NUOS Web Platform</generator>
    <image>
      <url>https://example.com/images/news-rss.png</url>
      <title>Company News</title>
      <link>https://example.com/news</link>
    </image>
    <item>
      <title>New Product Launch (Published on Jan 1, 2024)</title>
      <link>https://example.com/news/2024/01/new-product</link>
      <description>&lt;p&gt;We are excited to announce our new product...&lt;/p&gt;</description>
      <category domain="https://example.com/products">Products</category>
      <enclosure url="https://example.com/images/product.jpg" length="12345" type="image/jpeg"/>
      <guid isPermaLink="false">news/2024/01/new-product</guid>
      <pubDate>Mon, 01 Jan 2024 12:00:00 +0000</pubDate>
    </item>
  </channel>
</rss>
```


<!-- HASH:8102b96746affb500a16edf0fa30a203 -->
