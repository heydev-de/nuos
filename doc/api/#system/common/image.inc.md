# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/image.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## image.inc

This file provides the core image handling infrastructure for the PWNC Web Platform. It includes functions for rendering responsive images with multiple resolution support, processing and caching image transformations, and a static `image_processor` class that handles low-level operations such as format detection, quality computation, and remote file synchronization.

---

## image

### image($url, $width, $height, $alt, $attribute, $defer, $preview)

Renders an HTML `<img>` tag with optional responsive `srcset`, lazy-loading via `data-defer-src`, and automatic fallback to a placeholder image on errors.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$url` | string | — | Logical or physical URL/path of the source image. |
| `$width` | int\|NULL | NULL | Target display width in pixels. |
| `$height` | int\|NULL | NULL | Target display height in pixels. |
| `$alt` | string\|NULL | NULL | Alt text for accessibility. |
| `$attribute` | array\|string\|TRUE\|NULL | NULL | HTML attributes; string becomes inline style; `TRUE` returns data array instead of HTML. |
| `$defer` | bool | FALSE | If true, generates a noscript fallback + deferred JS-loaded image. |
| `$preview` | bool\|NULL | TRUE | Generate preview placeholders during processing. `NULL` disables all processing. |

**Return Value:**  
- When `$attribute === TRUE`: returns an associative array containing `url`, `path`, `width`, `height`, and `alt`.
- Otherwise: returns a string of HTML markup.

**Inner Mechanisms:**
1. Resolves the source image path using `image_path()` or local cache lookup.
2. Falls back to `no_image.svg` if the image cannot be resolved.
3. Uses `image_processor::resolution()` to compute target dimensions.
4. Calls `image_process()` to generate resized versions and build a `srcset`.
5. Builds final HTML output with proper escaping via `x()`.

**Usage Example:**
```php
echo image("/images/photo.jpg", 800, 600, "A scenic view");
// Outputs: <img src="/data/image/cache/.../photo-800x600.jpg" width="800" height="600" alt="A scenic view">
```

---

## image_process

### image_process($url, $width, $height, $pref_type, $ignore_cache, $preview)

Processes one or more target resolutions of an image, generating cached derivative files. Supports chained downscaling for efficiency.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$url` | string | — | Source image URL. |
| `$width` | int\|array\|NULL | NULL | Target width(s). Array allows multiple sizes. |
| `$height` | int\|array\|NULL | NULL | Target height(s). Array allows multiple sizes. |
| `$pref_type` | string\|NULL | NULL | Preferred output format extension (e.g., 'webp'). |
| `$ignore_cache` | bool | FALSE | Skip checking existing cached files. |
| `$preview` | bool | TRUE | Generate preview-sized intermediate files. |

**Return Value:**  
- If single resolution requested: returns a string URL to the processed image.
- If multiple resolutions: returns an associative array mapping URLs to widths.

**Inner Mechanisms:**
1. Validates and resolves the source image path.
2. Skips processing for GIF/SVG formats.
3. Loads the source image using GD library functions.
4. Iterates over each target resolution, creating scaled derivatives.
5. Encodes output in WebP/JPEG/PNG based on alpha channel support.
6. Optionally creates daemon tasks for background processing.

**Usage Example:**
```php
$urls = image_process("/images/banner.png", [320, 768, 1280], NULL, "webp");
// Returns: ["/data/image/cache/.../banner-320x...webp" => 320, ...]
```

---

## image_data

### image_data($url)

Convenience wrapper around `image()` that returns metadata about an image without rendering HTML.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | string | Image URL. |

**Return Value:**  
Associative array with keys: `url`, `path`, `width`, `height`, `alt`.

**Usage Example:**
```php
$data = image_data("/images/logo.svg");
echo $data["width"]; // e.g., 200
```

---

## image_exists

### image_exists($value)

Checks whether a local image file exists in either SVG or PNG format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Relative path without extension. |

**Return Value:**  
Boolean indicating existence.

**Usage Example:**
```php
if (image_exists("icons/user")) {
    echo image("icons/user");
}
```

---

## image_path

### image_path($url)

Resolves a logical URL to a physical filesystem path within the CMS root.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | string | URL starting with `CMS_ROOT_URL`. |

**Return Value:**  
Physical path string if valid and within root, otherwise `FALSE`.

**Usage Example:**
```php
$path = image_path(CMS_ROOT_URL . "/images/test.png");
// Returns: "/var/www/data/images/test.png"
```

---

## getimagesize

### getimagesize($file, &$imageinfo)

Wrapper around PHP’s native `getimagesize()` with added caching and SVG support.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$file` | string | File path or URL. |
| `$imageinfo` | mixed | Passed by reference to native function. |

**Return Value:**  
Standard `getimagesize()` result array, or `FALSE` on failure.

**Inner Mechanisms:**
1. Resolves local paths using `image_path()`.
2. Caches results using `cms_cache()`.
3. Parses SVG dimensions from XML attributes or viewBox.
4. Detects alpha channels and animation flags for PNG/WebP.

**Usage Example:**
```php
$size = getimagesize("/images/photo.jpg");
list($w, $h) = $size;
```

---

## image_processor

Static utility class providing core image processing capabilities including resolution calculation, format selection, quality tuning, and caching logic.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$width_max` | int\|NULL | Maximum allowed output width. |
| `$height_max` | int\|NULL | Maximum allowed output height. |
| `$pref_type` | int\|NULL | Preferred GD image type constant. |
| `$format` | array | Map of supported formats and their descriptions. |
| `$ext_to_type` | array | Extension-to-GD-type mapping. |
| `$type_to_ext` | array | GD-type-to-extension mapping. |

---

### resolution($source_width, $source_height, $target_width, $target_height)

Computes a list of target resolutions constrained by system limits and aspect ratio preservation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source_width` | int | Original image width. |
| `$source_height` | int | Original image height. |
| `$target_width` | int\|array\|NULL | Desired width(s). |
| `$target_height` | int\|array\|NULL | Desired height(s). |

**Return Value:**  
Array of `[width, height]` pairs sorted by descending area.

**Usage Example:**
```php
$sizes = image_processor::resolution(1920, 1080, 800, NULL);
// Returns: [[800, 450]]
```

---

### alpha_anim($path, $type)

Detects transparency (alpha) and animation status in PNG/WebP images.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | Filesystem path to image. |
| `$type` | int | GD image type constant. |

**Return Value:**  
Array `[alpha, anim]` booleans, or `FALSE` if unsupported.

**Usage Example:**
```php
list($hasAlpha, $isAnimated) = image_processor::alpha_anim("/tmp/test.png", IMAGETYPE_PNG);
```

---

### load($path, $type)

Loads an image resource from disk using the appropriate GD function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | Filesystem path. |
| `$type` | int | GD image type constant. |

**Return Value:**  
GD image resource or `FALSE` on failure.

**Usage Example:**
```php
$img = image_processor::load("/images/photo.jpg", IMAGETYPE_JPEG);
```

---

### determine_format($pref_type, $alpha)

Selects the best output format based on preference and alpha channel requirement.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$pref_type` | int\|NULL | Preferred GD type. |
| `$alpha` | bool | Whether transparency is needed. |

**Return Value:**  
GD image type constant or `FALSE`.

**Usage Example:**
```php
$fmt = image_processor::determine_format(IMAGETYPE_WEBP, TRUE);
// Returns: IMAGETYPE_WEBP
```

---

### compute_quality($width, $height, $type)

Calculates compression quality dynamically based on image size using quadratic Bézier interpolation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$width` | int | Output width. |
| `$height` | int | Output height. |
| `$type` | int | GD image type constant. |

**Return Value:**  
Integer quality value (0–100).

**Usage Example:**
```php
$quality = image_processor::compute_quality(1280, 720, IMAGETYPE_JPEG);
// Returns: ~87
```

---

### cache_path($url, $width, $height, $ext)

Generates a deterministic cache file path for a given image URL and dimensions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | string | Source image URL. |
| `$width` | int\|NULL | Target width. |
| `$height` | int\|NULL | Target height. |
| `$ext` | string\|NULL | Output file extension. |

**Return Value:**  
Cache file path string or `FALSE` if invalid.

**Usage Example:**
```php
$path = image_processor::cache_path("/images/photo.jpg", 800, 600, "webp");
// Returns: "/data/image/cache/ab/cd/photo-800x600-efgh1234.webp"
```

---

### cache_url($path)

Converts a cache file path into a publicly accessible URL.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | Absolute cache file path. |

**Return Value:**  
Publicly accessible URL string or `FALSE`.

**Usage Example:**
```php
$url = image_processor::cache_url("/data/image/cache/photo.webp");
// Returns: "/data/image/cache/photo.webp"
```

---

### cache_remote($url, $path, $ttl)

Downloads and caches a remote image, respecting HTTP status codes and modification times.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | string | Remote image URL. |
| `$path` | string | Local cache file path. |
| `$ttl` | int | Cooldown period in seconds. |

**Return Value:**  
- `TRUE`: Successfully downloaded/updated.
- `FALSE`: Error occurred.
- `NULL`: Still within cooldown or unchanged.

**Usage Example:**
```php
$result = image_processor::cache_remote("https://example.com/image.jpg", "/data/cache/image.jpg");
```

---

### contain($width, $height, $width_max, $height_max)

Scales an image to fit within specified bounds while preserving aspect ratio.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$width` | int | Current width. |
| `$height` | int | Current height. |
| `$width_max` | int | Maximum allowed width. |
| `$height_max` | int | Maximum allowed height. |

**Return Value:**  
Array `[new_width, new_height]`.

**Usage Example:**
```php
list($w, $h) = image_processor::contain(1920, 1080, 800, 600);
// Returns: [800, 450]
```


<!-- HASH:bb2cf25c4387c40385d7a760d35af604 -->
