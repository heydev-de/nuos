# NUOS API Documentation

[← Index](../README.md) | [`module/sitemap.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Sitemap Module (`module/sitemap.php`)

Generates an XML sitemap for search engines following the **Sitemap Protocol 0.9** (`sitemaps.org`). The module dynamically retrieves all published content entries, resolves their canonical URLs across enabled languages, and outputs a structured XML document with `<url>` entries, `<loc>`, `<lastmod>`, and multilingual `<xhtml:link>` alternates.

---

### **Core Workflow**
1. **Caching**: Checks for a valid cached sitemap (60-second TTL) to avoid redundant database queries.
2. **Language Mapping**: Resolves language-specific canonical URLs for each content entry.
3. **Database Query**: Fetches all published content entries (excluding those flagged for sitemap exclusion or `noindex`).
4. **XML Generation**: Constructs a valid XML sitemap with multilingual support.
5. **Caching & Output**: Caches the generated sitemap permanently and outputs it.

---

### **Constants & Dependencies**
| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| **Dependencies**         |               |                                                                             |
| `nuos.inc`               | Required      | Core NUOS library (loaded via `require`).                                   |
| `content` library        | Loaded        | Provides content management utilities (e.g., status flags, database fields).|
| **Database Fields**      |               |                                                                             |
| `CMS_DB_CONTENT_INDEX`   | Dynamic       | Primary key for content entries.                                           |
| `CMS_DB_CONTENT_TIME`    | Dynamic       | Publication timestamp (UNIX epoch).                                         |
| `CMS_DB_CONTENT_STATUS`  | Dynamic       | Content status field.                                                      |
| `CMS_DB_CONTENT_FLAG`    | Dynamic       | Bitmask field for content flags.                                           |
| **Status/Flag Values**   |               |                                                                             |
| `CMS_CONTENT_STATUS_PUBLICATION` | `1`   | Status value for published content.                                        |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` | Bitmask | Excludes content from sitemap if set.                                      |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX` | Bitmask | Excludes content with `noindex` meta tag.                                  |
| **Language Settings**    |               |                                                                             |
| `CMS_LANGUAGE_ENABLED`   | String/Array  | Comma-separated list of enabled language IDs (e.g., `"0,en,es"`).          |
| `CMS_LANGUAGE_DEFAULT`   | String        | Default language ID (e.g., `"en"`).                                        |

---

### **Key Functions & Logic**

#### **1. Caching Mechanism**
- **Purpose**: Avoids regenerating the sitemap on every request by checking a 60-second cache.
- **Mechanism**:
  - Uses `cms_cache_time($cache_key)` to check if a valid cache exists.
  - If cached and recent (`$cache_time > ($time - 60)`), outputs the cached version via `cms_cache_notouch($cache_key)` and exits.
- **Usage**: Critical for performance; reduces database load for high-traffic sites.

---

#### **2. Language Mapping**
- **Purpose**: Resolves canonical URLs for each content entry in all enabled languages.
- **Mechanism**:
  - Splits `CMS_LANGUAGE_ENABLED` into an array (e.g., `[0, "en", "es"]`).
  - For each language, instantiates a `map` object pointing to `#system/{lang}.directory.content` (e.g., `#system/en.directory.content`).
  - The `map` object (from the `content` library) retrieves the canonical URL for a given content index.
- **Usage**: Enables multilingual sitemaps with `<xhtml:link>` alternates.

---

#### **3. Database Query**
- **Purpose**: Fetches all published content entries eligible for the sitemap.
- **SQL Logic**:
  ```sql
  SELECT index, time
  FROM content_table
  WHERE status = '1'
    AND NOT (flags & (SITEMAP_EXCLUDE | NOINDEX))
  ORDER BY time DESC
  ```
  - **Exclusion Logic**: Skips entries with `SITEMAP_EXCLUDE` or `NOINDEX` flags via bitwise `&` check.
- **Result Processing**: Iterates over results with `mysql_fetch_assoc()`.

---

#### **4. XML Generation**
- **Purpose**: Constructs a valid XML sitemap with multilingual support.
- **Structure**:
  ```xml
  <urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:xhtml="http://www.w3.org/1999/xhtml">
    <url>
      <loc>https://example.com/en/page</loc>
      <lastmod>2023-10-01</lastmod>
      <xhtml:link rel="alternate" hreflang="es" href="https://example.com/es/pagina"/>
      <xhtml:link rel="alternate" hreflang="x-default" href="https://example.com/en/page"/>
    </url>
  </urlset>
  ```
- **Key Logic**:
  - **Primary URL**: Uses the first language’s URL as the `<loc>`.
  - **Alternates**: For multilingual sites, adds `<xhtml:link>` tags for each language (including `x-default` fallback).
  - **Escaping**: Uses `x()` for XML escaping (e.g., `&`, `<`, `>`).

---

#### **5. Caching & Output**
- **Purpose**: Stores the generated sitemap permanently and outputs it.
- **Mechanism**:
  - Caches the XML string via `cms_cache($cache_key, $buffer, TRUE)` (permanent storage).
  - Outputs the buffer with `Content-Type: application/xml`.

---

### **Usage Scenarios**
1. **SEO Optimization**:
   - Deploy as `sitemap.xml` (e.g., via URL rewriting or direct access to `module/sitemap.php`).
   - Ensures search engines discover all published content, including multilingual versions.
2. **Dynamic Content**:
   - Automatically updates when new content is published or existing content is modified.
3. **Performance**:
   - Caching reduces database load; ideal for sites with frequent crawls.

---

### **Error Handling & Edge Cases**
- **No Content**: Outputs an empty `<urlset>` if no eligible content exists.
- **Missing URLs**: Skips entries without canonical URLs in a language (via `continue`).
- **Language Fallback**: Uses the default language (`CMS_LANGUAGE_DEFAULT`) as `x-default` if not explicitly set.

---

### **Dependencies & Integration**
- **`content` Library**: Required for `map` objects and content status/flag definitions.
- **`mysql` Class**: Used for database queries (wraps `mysqli`).
- **`cms_cache`**: Dual-layer caching (RAM + permanent storage).
- **`x()`**: Custom XML escaping to prevent malformed output.


<!-- HASH:5b6d7caa2bd237089bea22a7065dcb12 -->
