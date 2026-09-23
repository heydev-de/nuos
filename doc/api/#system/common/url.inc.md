# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/url.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/url.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## translate_url

### Overview
Resolves internal logical URL identifiers (e.g., `content://`, `image://`, `media://`, `download://`) into fully qualified URLs. It also handles standard HTTP(S) URLs and directory-based paths.

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | string | — | The logical or physical URL to resolve. |
| `$param` | array\|NULL | NULL | Additional query parameters to merge. |
| `$language` | string | CMS_LANGUAGE | Language context for directory/content maps. |
| `$omit_cms_param` | bool | FALSE | Whether to omit CMS-specific parameters. |

### Return Values
- **string**: Fully resolved URL.
- **FALSE**: If an error occurs during processing (e.g., image/media/download module fails to load).

### Inner Mechanisms
1. Short-circuits for `javascript:` or `mailto:` schemes.
2. Converts relative paths to absolute using `absolute_path()`.
3. Parses the URL structure via `analyze_url()`.
4. For HTTP(S): determines if external or executable, then builds query string accordingly.
5. For `directory`/`content`: uses language-specific maps to resolve indices.
6. For `image`/`media`: loads respective modules and resolves filenames/URLs.
7. For `download`: loads download module and generates a download link.

### Usage Example
```php
// Resolve a content reference
$url = translate_url("content://about_us");
// Returns something like: https://example.com/modules/content.php?cms_language=en&content_index=about_us
```

---

## analyze_url

### Overview
Parses a URL into its components and enriches it with path information (dirname, basename, filename, extension).

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | string | — | The URL to parse. |

### Return Values
- **array**: Parsed URL components with additional path details.
- **FALSE**: If the URL is invalid.

### Inner Mechanisms
1. Uses PHP's `parse_url()` to break down the URL.
2. Extracts path info using `pathinfo()`.
3. Normalizes directory separators and handles `.` directories.
4. Merges parsed data with default values for missing components.

### Usage Example
```php
$parts = analyze_url("https://example.com/path/to/file.php?query=1#frag");
// Returns array with scheme, host, path, query, fragment, dirname, basename, etc.
```

---

## absolute_path

### Overview
Resolves a relative URL against a base URL to produce an absolute URL.

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$source` | string | — | Base (absolute) URL. |
| `$target` | string | — | Target (possibly relative) URL. |

### Return Values
- **string**: Resolved absolute URL.
- **FALSE**: If either URL is invalid or source isn't absolute.

### Inner Mechanisms
1. Parses both source and target URLs.
2. If target is already absolute, returns it unchanged.
3. Resolves path components using `resolve_path()`.
4. Preserves query and fragment from target.

### Usage Example
```php
$abs = absolute_path("https://example.com/base/", "../other/page");
// Returns: https://example.com/other/page
```

---

## querystring

### Overview
Generates a query string from an array of parameters, optionally merging with another array.

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$param` | array | — | Primary parameters. |
| `$alter` | array\|NULL | NULL | Parameters to merge into `$param`. |

### Return Values
- **string**: Query string starting with `?`.
- **FALSE**: If `$param` is not an array.

### Inner Mechanisms
1. Validates input type.
2. Merges arrays recursively if `$alter` is provided.
3. Delegates to `cms_param()` with flags for query string generation.

### Usage Example
```php
$qs = querystring(["page" => 2], ["sort" => "asc"]);
// Returns: ?page=2&sort=asc
```

---

## u

### Overview
Smart URL generator that appends CMS parameters to a given address or current active URL.

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | string\|array\|NULL | NULL | Base URL or array of parameters. |
| `$param` | array\|NULL | NULL | Additional parameters. |

### Return Values
- **string**: Generated URL with appended parameters.

### Inner Mechanisms
1. If `$address` is an array, treats it as parameters and uses `CMS_ACTIVE_URL`.
2. Ensures `$param` is an array.
3. Falls back to `CMS_ACTIVE_URL` if address is not a string.
4. Appends parameters using `cms_param()`.

### Usage Example
```php
$url = u("/products", ["category" => "books"]);
// Returns: /products?category=books
```

---

## qu

### Overview
URL-encodes the result of `u()` for safe inclusion in JavaScript or HTML contexts.

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | string\|array\|NULL | NULL | Base URL or parameters. |
| `$param` | array\|NULL | NULL | Additional parameters. |

### Return Values
- **string**: URL-encoded URL.

### Inner Mechanisms
1. Calls `u()` to generate the URL.
2. Applies `q()` for encoding.

### Usage Example
```php
$encoded = qu("/search", ["q" => "hello world"]);
// Returns: %2Fsearch%3Fq%3Dhello%20world
```

---

## import_querystring

### Overview
Imports query string parameters from a URL into global variables (`$_GET` and `$GLOBALS`).

### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | string | — | URL containing query string. |

### Return Values
- **TRUE**: On successful import.
- **FALSE**: If URL parsing fails or no query string exists.

### Inner Mechanisms
1. Parses the URL.
2. Extracts and parses the query string.
3. Normalizes values using `cms_utf8_normalize()`.
4. Populates `$_GET` and `$GLOBALS`.

### Usage Example
```php
import_querystring("https://example.com/page?foo=bar&baz=qux");
// Sets $_GET['foo'] = 'bar', $_GET['baz'] = 'qux'
```


<!-- HASH:f7f775d72621cb1538b0983338e81e63 -->
