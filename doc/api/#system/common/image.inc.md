# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/image.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Image Handling Utilities

This file provides core image handling functionality for the NUOS platform, including image processing, resizing, format conversion, and HTML generation. It integrates with the platform's caching, URL resolution, and daemon systems to deliver optimized images efficiently.

---

## `image()`

Generates an HTML `<img>` tag with support for responsive images, deferred loading, and format conversion.

### Parameters

| Name      | Type    | Default | Description                                                                                     |
|-----------|---------|---------|-------------------------------------------------------------------------------------------------|
| `$url`    | string  | -       | Image URL or path. Can be local or remote.                                                     |
| `$width`  | int     | `NULL`  | Target width in pixels. If `NULL`, original width is used.                                      |
| `$height` | int     | `NULL`  | Target height in pixels. If `NULL`, original height is used.                                    |
| `$alt`    | string  | `NULL`  | Alternative text for the image.                                                                |
| `$style`  | string  | `NULL`  | Inline CSS styles for the image.                                                               |
| `$defer`  | bool    | `FALSE` | If `TRUE`, enables deferred loading (lazy loading) using a placeholder SVG and JavaScript.     |
| `$preview`| bool    | `TRUE`  | If `TRUE`, enables preview generation and background processing for large images.              |

### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | HTML `<img>` tag with attributes for responsive images, deferred loading, and accessibility.   |

### Inner Mechanisms

1. **Image Processing**:
   - Calls `image_process()` to generate a cached, resized version of the image.
   - Adjusts dimensions to maintain aspect ratio if processing fails.

2. **Deferred Loading**:
   - If `$defer` is `TRUE`, generates a placeholder SVG and uses `data-defer-src` for lazy loading.
   - Wraps the `<img>` tag in a `<noscript>` fallback for non-JavaScript environments.

3. **Responsive Images**:
   - Generates a `srcset` attribute with multiple resolutions for high-DPI displays.
   - Uses a predefined list of resolutions (`160` to `7680` pixels) and filters out redundant sizes.

4. **Escaping**:
   - Uses `x()` for XML escaping of URLs and attributes.

### Usage

- **Basic Usage**:
  ```php
  echo image("path/to/image.jpg", 300, 200, "Example Image");
  ```
  Generates an `<img>` tag with a 300x200 image.

- **Deferred Loading**:
  ```php
  echo image("path/to/image.jpg", 300, 200, "Example Image", NULL, TRUE);
  ```
  Enables lazy loading for performance optimization.

- **Responsive Images**:
  ```php
  echo image("path/to/image.jpg", 800, 600);
  ```
  Automatically generates a `srcset` for high-DPI displays.

---

## `image_exists()`

Checks if a local image file exists in any of the supported formats.

### Parameters

| Name    | Type   | Default | Description                     |
|---------|--------|---------|---------------------------------|
| `$value`| string | -       | Image filename without extension.|

### Return Value

| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| bool    | `TRUE` if the image exists in any supported format (`.webp`, `.jpg`, `.png`, `.gif`, `.svg`).  |

### Inner Mechanisms

- Checks for the existence of the file in the `CMS_IMAGES_PATH` directory with any supported extension.

### Usage

```php
if (image_exists("logo")) {
    echo "Logo exists!";
}
```

---

## `image_quality()`

Calculates the optimal image quality for a given resolution and format using quadratic Bézier interpolation.

### Parameters

| Name        | Type   | Default | Description                     |
|-------------|--------|---------|---------------------------------|
| `$width`    | int    | -       | Image width in pixels.          |
| `$height`   | int    | -       | Image height in pixels.         |
| `$extension`| string | -       | Image format (`"webp"` or `"jpg"`).|

### Return Value

| Type | Description                                                                                     |
|------|-------------------------------------------------------------------------------------------------|
| int  | Quality value between `0` and `100`.                                                            |

### Inner Mechanisms

1. **Quality Ranges**:
   - Defines minimum, mid, and maximum quality values for each format (`webp` and `jpg`).

2. **Resolution Ranges**:
   - Uses predefined resolution thresholds (`$rmin`, `$rmid`, `$rmax`) to determine quality.

3. **Bézier Interpolation**:
   - Computes a smooth transition between quality values based on the image's diagonal resolution.
   - Uses a control point to ensure a non-linear transition.

### Usage

```php
$quality = image_quality(1920, 1080, "webp");
```

---

## `image_process()`

Processes an image (resizing, format conversion, caching) and returns its URL.

### Parameters

| Name                  | Type    | Default | Description                                                                                     |
|-----------------------|---------|---------|-------------------------------------------------------------------------------------------------|
| `$url`                | string  | -       | Image URL or path.                                                                             |
| `$width`              | int     | `NULL`  | Target width in pixels. If `NULL`, original width is used.                                      |
| `$height`             | int     | `NULL`  | Target height in pixels. If `NULL`, original height is used.                                    |
| `$preference_override`| string  | `NULL`  | Overrides the default format preference (`"webp"`, `"jpg"`, `"png"`).                          |
| `$ignore_cache`       | bool    | `FALSE` | If `TRUE`, forces regeneration of the cached image.                                            |
| `$preview`            | bool    | `TRUE`  | If `TRUE`, enables preview generation and background processing for large images.              |

### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | URL of the processed image.                                                                    |

### Inner Mechanisms

1. **URL Analysis**:
   - Uses `analyze_url()` to parse the image URL and validate its scheme and extension.

2. **Local vs. Remote**:
   - Checks if the image is local or remote.
   - For remote images, retrieves a local copy and updates it hourly if modified.

3. **Caching**:
   - Generates a unique cache path using a hash of the URL.
   - Checks for existing cached images in declining format priority.

4. **Image Processing**:
   - Uses the GD library for resizing and format conversion.
   - Falls back to a CGI script if GD is unavailable.
   - Supports `jpg`, `png`, `webp`, and `gif` formats.
   - Handles transparency and alpha channels for `png` and `webp`.

5. **Preview Generation**:
   - If `$preview` is `TRUE`, generates a low-resolution preview and delegates full processing to a background daemon.

6. **Daemon Integration**:
   - Uses `cms_daemon()` to offload heavy processing to a background task.

### Usage

- **Basic Resizing**:
  ```php
  $url = image_process("path/to/image.jpg", 800, 600);
  ```

- **Format Conversion**:
  ```php
  $url = image_process("path/to/image.png", 800, 600, "webp");
  ```

- **Force Regeneration**:
  ```php
  $url = image_process("path/to/image.jpg", 800, 600, NULL, TRUE);
  ```

---

## `image_path()`

Converts a URL to a local filesystem path if the image is local.

### Parameters

| Name  | Type   | Default | Description                     |
|-------|--------|---------|---------------------------------|
| `$url`| string | -       | Image URL.                      |

### Return Value

| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| string  | Local filesystem path if the image is local and exists.                                        |
| bool    | `FALSE` if the image is remote or does not exist.                                              |

### Inner Mechanisms

- Checks if the URL starts with `CMS_ROOT_URL` and converts it to a local path.

### Usage

```php
$path = image_path("https://example.com/images/logo.png");
if ($path !== FALSE) {
    echo "Local path: $path";
}
```

---

## `image_type()`

Determines the type of an image file.

### Parameters

| Name   | Type   | Default | Description                     |
|--------|--------|---------|---------------------------------|
| `$path`| string | -       | Local filesystem path to the image.|

### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | Image type (`"gif"`, `"jpg"`, `"png"`, `"webp"`, `"svg"`).                                     |
| bool   | `FALSE` if the file does not exist or the type cannot be determined.                           |

### Inner Mechanisms

- Uses `exif_imagetype()` or `getimagesize()` to determine the image type.
- Special handling for SVG files.

### Usage

```php
$type = image_type("/path/to/image.jpg");
if ($type === "jpg") {
    echo "JPEG image detected.";
}
```

---

## `getimagesize()`

Wrapper for PHP's `getimagesize()` with support for SVG and preview sizes.

### Parameters

| Name         | Type   | Default | Description                                                                                     |
|--------------|--------|---------|-------------------------------------------------------------------------------------------------|
| `$filename`  | string | -       | Image URL or local path.                                                                       |
| `&$imageinfo`| array  | `NULL`  | Reference to an array for additional image information.                                        |
| `$preview`   | mixed  | `NULL`  | If not `NULL`, returns cached preview dimensions.                                              |

### Return Value

| Type  | Description                                                                                     |
|-------|-------------------------------------------------------------------------------------------------|
| array | Image dimensions as `[width, height]`.                                                         |
| bool  | `FALSE` if the image cannot be processed.                                                      |

### Inner Mechanisms

1. **Preview Handling**:
   - Checks the cache for preview dimensions if `$preview` is not `NULL`.

2. **SVG Support**:
   - Uses `SimpleXML` to parse SVG files and extract dimensions from `width`, `height`, or `viewBox` attributes.

3. **Fallback**:
   - Returns `[300, 250]` if the image cannot be processed.

### Usage

```php
$size = getimagesize("path/to/image.svg");
if ($size !== FALSE) {
    list($width, $height) = $size;
    echo "Width: $width, Height: $height";
}
```


<!-- HASH:773bbfcb3d41142452aefb7a901cf69b -->
