# PWNC API Documentation

[← Index](../README.md) | [`module/sitemap.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/sitemap.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Sitemap Module

The `sitemap.php` module generates an XML sitemap for the PWNC Web Platform. It dynamically retrieves published content from the database, constructs URLs for each language variant, and outputs a standards-compliant sitemap following the [Sitemap Protocol](https://www.sitemaps.org/protocol.html). The module supports multilingual sites by including `hreflang` annotations and `x-default` fallbacks.

### Key Features:
- **Caching**: Uses a dual-layer cache (`cms_cache`) to store the generated sitemap for 60 seconds, reducing database load.
- **Multilingual Support**: Generates alternate links for each enabled language and includes an `x-default` fallback.
- **Content Filtering**: Only includes content that is published and not excluded from sitemaps or marked with `noindex`.

---

### Configuration Constants

| Constant | Description |
|---------|-------------|
| `CMS_LANGUAGE_ENABLED` | Comma-separated list of enabled languages (e.g., `en,de`). If falsy, defaults to `[0]` (single language). |
| `CMS_LANGUAGE_DEFAULT` | Default language code used for `x-default` fallback link. |
| `CMS_DB_CONTENT` | Database table name for content records. |
| `CMS_DB_CONTENT_INDEX` | Column name for the content index/identifier. |
| `CMS_DB_CONTENT_TIME` | Column name for the content last-modified timestamp. |
| `CMS_DB_CONTENT_STATUS` | Column name for the content status field. |
| `CMS_CONTENT_STATUS_PUBLICATION` | Status value indicating published content. |
| `CMS_DB_CONTENT_FLAG` | Column name for content flags (bitmask). |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` | Flag bit indicating content should be excluded from sitemap. |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX` | Flag bit indicating content should not be indexed by search engines. |

---

### Execution Flow

1. **Initialization**: Loads required libraries and sets the XML content-type header.
2. **Cache Check**: Attempts to serve a cached version of the sitemap if it was generated within the last 60 seconds.
3. **Language Mapping**: Builds a mapping of language codes to directory content objects for URL resolution.
4. **Database Query**: Retrieves all published, non-excluded content ordered by modification time.
5. **URL Generation**: For each content item, resolves its URL per language and constructs `<url>` entries with optional `<xhtml:link>` alternates.
6. **Output & Caching**: Outputs the final XML and caches it permanently for future requests.

---

### Usage Example

This module is typically accessed directly via HTTP at a URL like `https://example.com/module/sitemap.php`. No direct function calls are needed; it runs as a standalone endpoint.

```php
// Access the sitemap via browser or crawler:
// GET https://example.com/module/sitemap.php
//
// Response:
// <?xml version="1.0" encoding="utf-8"?>
// <urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:xhtml="http://www.w3.org/1999/xhtml">
//   <url>
//     <loc>https://example.com/en/article-title</loc>
//     <lastmod>2025-04-01</lastmod>
//     <xhtml:link rel="alternate" href="https://example.com/en/article-title" hreflang="en"/>
//     <xhtml:link rel="alternate" href="https://example.com/de/artikel-titel" hreflang="de"/>
//     <xhtml:link rel="alternate" href="https://example.com/en/article-title" hreflang="x-default"/>
//   </url>
// </urlset>
```

---

### Internal Mechanisms

#### Cache Handling
```php
$cache_key = "sitemap";
$cache_time = cms_cache_time($cache_key);
$time = time();
if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) {
    echo(cms_cache_notouch($cache_key));
    exit();
}
```
- Checks if a cached sitemap exists and was generated within the last 60 seconds.
- If so, serves the cached version immediately without querying the database.

#### Language Mapping
```php
$language = stre(CMS_LANGUAGE_ENABLED) ? [0] : explode(",", CMS_LANGUAGE_ENABLED);
foreach ($language AS $value) {
    $_value = ($value !== 0) ? "$value." : "";
    $map[$value] = new map("#system/" . $_value . "directory.content");
}
```
- Creates a `map` object for each language to resolve content indices into URLs.
- Uses `#system/{lang}.directory.content` as the logical identifier for URL mappings.

#### Database Query
```sql
SELECT {CMS_DB_CONTENT_INDEX}, {CMS_DB_CONTENT_TIME}
FROM {CMS_DB_CONTENT}
WHERE {CMS_DB_CONTENT_STATUS} = '{CMS_CONTENT_STATUS_PUBLICATION}'
  AND NOT {CMS_DB_CONTENT_FLAG} & ({CMS_CONTENT_FLAG_SITEMAP_EXCLUDE} | {CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX})
ORDER BY {CMS_DB_CONTENT_TIME} DESC
```
- Selects only published content that is not flagged for exclusion or noindex.
- Orders results by most recently modified first.

#### URL Construction
For each content item:
- Resolves its URL using the language-specific `map` object.
- Outputs a `<url>` block with `<loc>` and `<lastmod>`.
- If multilingual, adds `<xhtml:link>` elements for all language variants.
- Adds an `x-default` link pointing to the default language version.

#### Final Output
```php
$buffer .= "</urlset>";
cms_cache($cache_key, $buffer, TRUE);
echo($buffer);
```
- Appends closing tag, caches the full sitemap permanently, and outputs it.

---

### Notes
- The `x()` function is used for XML escaping of URLs and dates.
- The `map` class is assumed to provide a `get_value($index)` method that returns the canonical URL for a given content index.
- This module assumes a properly configured PWNC environment with database access and language settings defined.


<!-- HASH:f2bfe6aaaeb38aecee97d02545c3314c -->
