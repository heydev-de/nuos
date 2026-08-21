# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/image.inc)

- **Version:** `26.8.5.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Handling Utilities (`image.inc`)

This file provides core image handling functionality for the PWNC Web Platform, including:
- Dynamic image resizing and format conversion
- Multi-resolution image generation (for responsive design)
- SVG support
- Remote image caching
- Image processing daemon integration

The utilities work with both local and remote images, automatically caching processed versions for performance.

---

## Functions

### `image()`

Generates an HTML `<img>` tag with optional responsive image support and deferred loading.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string | - | Image URL (local or remote) |
| `$width` | int\|null | NULL | Target width (0 = auto) |
| `$height` | int\|null | NULL | Target height (0 = auto) |
| `$alt` | string\|null | NULL | Alternative text |
| `$attribute` | array\|string | NULL | HTML attributes (array) or style (string) |
| `$defer` | bool | FALSE | Enable deferred loading |
| `$preview` | bool\|null | TRUE | Preview mode (NULL = no processing) |

#### Return Value
- **string**: HTML `<img>` tag with appropriate attributes

#### Inner Mechanisms
1. **URL Processing**: Resolves short format URLs (e.g., `logo` → `logo.svg`/`logo.png`)
2. **Remote Caching**: Downloads and caches remote images locally
3. **Responsive Generation**: Creates multiple resolutions for `srcset` if needed
4. **Deferred Loading**: Generates placeholder SVG for lazy loading
5. **Error Handling**: Falls back to `no_image.svg` on failure

#### Usage Example
```php
// Basic usage with responsive images
echo image("products/phone", 400, 0, "Smartphone");

// Deferred loading with custom attributes
echo image("hero/banner", 1200, 0, "", ["class" => "hero-image"], TRUE);
```

---

### `image_process()`

Processes an image to generate multiple resolutions or convert formats.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string | - | Image URL |
| `$width` | int\|array | NULL | Target width(s) |
| `$height` | int\|array | NULL | Target height(s) |
| `$pref_type` | int\|null | NULL | Preferred output format (IMAGETYPE_*) |
| `$ignore_cache` | bool | FALSE | Bypass cache |
| `$preview` | bool | TRUE | Enable preview mode |

#### Return Value
- **string\|array**: Processed image URL(s) or original URL on failure

#### Inner Mechanisms
1. **Format Detection**: Determines optimal output format (WebP preferred)
2. **Progressive Downscaling**: Processes images in multiple passes for quality
3. **Daemon Integration**: Queues background processing for large images
4. **Cache Management**: Stores processed images in `CMS_DATA_PATH/image/cache/`

#### Usage Example
```php
// Generate multiple resolutions for responsive design
$responsive_images = image_process(
    "gallery/photo1",
    [300, 600, 1200],  // widths
    NULL,              // auto height
    IMAGETYPE_WEBP     // force WebP
);
```

---

### `image_exists()`

Checks if an image exists in supported formats (SVG/PNG).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | string | Image basename (without extension) |

#### Return Value
- **bool**: TRUE if image exists in any supported format

#### Usage Example
```php
if (image_exists("logo")) {
    echo image("logo", 200, 0);
}
```

---

### `image_path()`

Resolves a URL to a local filesystem path.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | string | Image URL |

#### Return Value
- **string\|false**: Local path or FALSE if not found

#### Inner Mechanisms
- Caches results to avoid repeated filesystem checks
- Validates paths stay within `CMS_ROOT_PATH`

#### Usage Example
```php
$path = image_path(CMS_IMAGES_URL . "icons/check.svg");
if ($path) {
    $size = filesize($path);
}
```

---

### `getimagesize()`

Extended version of PHP's `getimagesize()` with SVG support and caching.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$file` | string | Image path/URL |
| `$imageinfo` | array | Output parameter for additional info |

#### Return Value
- **array\|false**: Image size info or FALSE on failure

#### Inner Mechanisms
1. **SVG Support**: Parses SVG dimensions from attributes/viewBox
2. **Alpha/Animation Detection**: Identifies transparency and animation
3. **Caching**: Stores results to avoid repeated processing

#### Usage Example
```php
$size = getimagesize("diagram.svg", $info);
if ($size) {
    echo "Width: {$size[0]}px, Height: {$size[1]}px";
    if ($info["alpha"]) {
        echo " (has transparency)";
    }
}
```

---

## Class: `image_processor`

Core image processing functionality with static methods.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$width_max` | int\|null | Maximum processing width |
| `$height_max` | int\|null | Maximum processing height |
| `$pref_type` | int\|null | Default output format preference |
| `$format` | array | Supported image formats |
| `$ext_to_type` | array | Extension → IMAGETYPE mapping |
| `$type_to_ext` | array | IMAGETYPE → extension mapping |

---

### `resolution()`

Calculates target resolutions while maintaining aspect ratio.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$source_width` | int | Original width |
| `$source_height` | int | Original height |
| `$target_width` | int\|array\|null | Target width(s) |
| `$target_height` | int\|array\|null | Target height(s) |

#### Return Value
- **array**: Sorted list of `[width, height]` pairs

#### Usage Example
```php
$resolutions = image_processor::resolution(1920, 1080, [300, 600, 1200]);
// Returns: [[1200, 675], [600, 338], [300, 169]]
```

---

### `alpha_anim()`

Detects alpha transparency and animation in images.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$path` | string | Image path |
| `$type` | int | IMAGETYPE constant |

#### Return Value
- **array\|false**: `[has_alpha, is_animated]` or FALSE on failure

#### Usage Example
```php
$info = image_processor::alpha_anim("animation.webp", IMAGETYPE_WEBP);
if ($info[1]) {
    echo "This is an animated WebP!";
}
```

---

### `load()`

Loads an image into memory with error suppression.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$path` | string | Image path |
| `$type` | int | IMAGETYPE constant |

#### Return Value
- **resource\|false**: GD image resource or FALSE on failure

#### Usage Example
```php
$image = image_processor::load("photo.jpg", IMAGETYPE_JPEG);
if ($image) {
    // Process image...
    imagedestroy($image);
}
```

---

### `determine_format()`

Determines the optimal output format based on support and transparency.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pref_type` | int\|null | NULL | Preferred format |
| `$alpha` | bool | FALSE | Requires transparency |

#### Return Value
- **int\|false**: IMAGETYPE constant or FALSE if no supported format

#### Usage Example
```php
$format = image_processor::determine_format(IMAGETYPE_WEBP, TRUE);
if ($format === IMAGETYPE_PNG) {
    echo "Falling back to PNG for transparency";
}
```

---

### `compute_quality()`

Calculates optimal quality setting based on image dimensions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$width` | int | Image width |
| `$height` | int | Image height |
| `$type` | int | IMAGETYPE constant |

#### Return Value
- **int**: Quality percentage (0-100)

#### Usage Example
```php
$quality = image_processor::compute_quality(1920, 1080, IMAGETYPE_JPEG);
// Returns: 85 (for 1080p JPEG)
```

---

### `cache_path()`

Generates a cache path for processed images.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string | - | Image URL |
| `$width` | int\|null | NULL | Target width |
| `$height` | int\|null | NULL | Target height |
| `$ext` | string\|null | NULL | File extension |

#### Return Value
- **string\|false**: Cache path or FALSE on failure

#### Usage Example
```php
$path = image_processor::cache_path("https://example.com/photo.jpg", 800, 600);
// Returns: "/path/to/cms/data/image/cache/ab/cd/photo-800x600-12345678.jpg"
```

---

### `cache_url()`

Converts a cache path to a URL.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$path` | string | Cache path |

#### Return Value
- **string\|false**: Cache URL or FALSE on failure

#### Usage Example
```php
$url = image_processor::cache_url("/path/to/cms/data/image/cache/ab/cd/photo.jpg");
// Returns: "https://example.com/data/image/cache/ab/cd/photo.jpg"
```

---

### `cache_remote()`

Caches a remote image locally with TTL management.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string | - | Remote image URL |
| `$path` | string | - | Local cache path |
| `$ttl` | int | 600 | Cache TTL in seconds |

#### Return Value
- **bool\|null**: TRUE (updated), FALSE (error), NULL (unchanged)

#### Usage Example
```php
$result = image_processor::cache_remote(
    "https://example.com/photo.jpg",
    "/path/to/cache/photo.jpg"
);
if ($result === TRUE) {
    echo "Image cached successfully";
}
```

---

### `contain()`

Calculates dimensions that fit within constraints while maintaining aspect ratio.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$width` | int | Original width |
| `$height` | int | Original height |
| `$width_max` | int | Maximum width |
| `$height_max` | int | Maximum height |

#### Return Value
- **array**: `[width, height]` of contained dimensions

#### Usage Example
```php
$dimensions = image_processor::contain(1920, 1080, 800, 600);
// Returns: [800, 450]
```


<!-- HASH:5b3dda26cd41924d17f6989e79303c71 -->
