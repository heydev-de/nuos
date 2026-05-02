# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview
`misc.inc` provides core utility functions for the NUOS web platform. These functions handle initialization, type conversion, MIME type resolution, recursive array operations, bit manipulation, IP anonymization, and content preview rendering. The file ensures backward compatibility (e.g., `each()` for PHP ≥7.2) and integrates with NUOS’s multibyte-safe, context-aware escaping system.

---

## Functions

### `each`
**Purpose:**
Polyfill for PHP’s deprecated `each()` function. Iterates over an array, returning the current key-value pair and advancing the internal pointer.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$array`| `array&` | Reference to the array to iterate.   |

**Return Values:**
- `array|FALSE`: Associative array with keys `0` (key), `1` (value), `key`, and `value`; or `FALSE` if the end of the array is reached.

**Inner Mechanisms:**
- Uses `key()`, `current()`, and `next()` to replicate `each()` behavior.
- Returns `FALSE` if the internal pointer is invalid (e.g., empty array or end reached).

**Usage Context:**
- Legacy code compatibility. Avoid in new code; use `foreach` instead.

---

### `init`
**Purpose:**
Initializes a variable with a default value if it is unset or empty.

**Parameters:**

| Name              | Type     | Description                          |
|-------------------|----------|--------------------------------------|
| `$variable`       | `mixed&` | Reference to the variable to check.  |
| `$default_value`  | `mixed`  | Default value (default: `""`).       |

**Return Values:**
- `mixed`: The initialized variable (either its original value or `$default_value`).

**Inner Mechanisms:**
- Checks `isset($variable)` and casts to string to test for emptiness (`""`).
- Modifies the variable by reference.

**Usage Context:**
- Default value assignment for optional function parameters or configuration variables.

---

### `blank`
**Purpose:**
Checks if a variable is unset or empty.

**Parameters:**

| Name        | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$variable` | `mixed&` | Reference to the variable to check.  |

**Return Values:**
- `bool`: `TRUE` if the variable is unset or empty (`""`), `FALSE` otherwise.

**Inner Mechanisms:**
- Mirrors the logic of `init()` but returns a boolean.

**Usage Context:**
- Input validation or conditional checks for empty values.

---

### `yesno`
**Purpose:**
Converts a boolean into a localized "Yes"/"No" string.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$boolean` | `bool`   | Boolean value to convert.            |

**Return Values:**
- `string`: Localized string (`CMS_L_COMMON_008` for `TRUE`, `CMS_L_COMMON_009` for `FALSE`).

**Usage Context:**
- Displaying boolean values in user interfaces (e.g., tables, forms).

---

### `option`
**Purpose:**
Returns a value if a condition is `TRUE`, otherwise `NULL`.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$boolean` | `bool`   | Condition to evaluate.               |
| `$value`   | `mixed`  | Value to return if `$boolean` is `TRUE`. |

**Return Values:**
- `mixed|NULL`: `$value` if `$boolean` is `TRUE`, otherwise `NULL`.

**Usage Context:**
- Conditional value assignment (e.g., optional query parameters, UI elements).

---

### `get_mime_type`
**Purpose:**
Resolves a file extension or filename to its MIME type.

**Parameters:**

| Name                     | Type     | Description                          |
|--------------------------|----------|--------------------------------------|
| `$filename_or_extension` | `string` | Filename or extension (default: `NULL`). |

**Return Values:**
- `string|array`:
  - If `$filename_or_extension` is `NULL`: Returns the full MIME type list as an associative array.
  - Otherwise: Returns the MIME type for the given extension or `application/octet-stream` if unknown.

**Inner Mechanisms:**
- Loads MIME types from `CMS_PATH . "mimetype"` (format: `extension type` per line).
- Uses `file_extension()` to extract extensions from filenames.
- Caches the list statically to avoid repeated file reads.

**Usage Context:**
- File uploads, asset management, or HTTP response headers.

---

### `get_mime_list`
**Purpose:**
Generates a list of MIME types with their associated icons.

**Return Values:**
- `array`: Associative array of extensions mapped to their icon paths (e.g., `["png" => "mimetype/image"]`).

**Inner Mechanisms:**
- Iterates over the MIME type list from `get_mime_type()`.
- Filters entries to include only those with valid icons (via `get_mime_icon()`).

**Usage Context:**
- UI components (e.g., file type selectors, asset browsers).

---

### `get_mime_icon`
**Purpose:**
Resolves a MIME type to its icon path.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$type`    | `string` | MIME type (e.g., `image/png`).       |
| `$default` | `string` | Fallback icon path (default: `"mimetype/default"`). |

**Return Values:**
- `string`: Path to the icon (e.g., `"mimetype/image"`), or `$default` if no icon exists.

**Inner Mechanisms:**
- Checks for icons in the following order:
  1. `mimetype/{type}` (e.g., `mimetype/image/png`).
  2. `mimetype/{primary_type}` (e.g., `mimetype/image`).
- Uses `image_exists()` to validate paths.

**Usage Context:**
- Displaying file type icons in UIs.

---

### `in_array_recursive`
**Purpose:**
Recursively checks if a value exists in a nested array.

**Parameters:**

| Name        | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$needle`   | `mixed`  | Value to search for.                 |
| `$haystack` | `array`  | Array to search.                     |
| `$strict`   | `bool`   | Whether to use strict comparison (default: `FALSE`). |

**Return Values:**
- `bool`: `TRUE` if `$needle` is found, `FALSE` otherwise.

**Inner Mechanisms:**
- Uses `RecursiveIteratorIterator` and `RecursiveArrayIterator` to traverse nested arrays.
- Supports both loose (`==`) and strict (`===`) comparison.

**Usage Context:**
- Searching deeply nested configuration arrays or data structures.

---

### `ksort_recursive`
**Purpose:**
Recursively sorts an array by keys.

**Parameters:**

| Name     | Type     | Description                          |
|----------|----------|--------------------------------------|
| `$array` | `array&` | Reference to the array to sort.      |

**Inner Mechanisms:**
- Applies `ksort()` to the top-level array and recursively to all nested arrays.

**Usage Context:**
- Normalizing associative arrays for consistent output (e.g., JSON responses).

---

### `bitstring`
**Purpose:**
Converts a string into its binary representation (bitstring).

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `string` | Input string.                        |
| `$length` | `int`    | Truncate to this many bits (default: `NULL`). |

**Return Values:**
- `string`: Binary representation (e.g., `"01000001"` for `"A"`), truncated to `$length` if specified.

**Inner Mechanisms:**
- Iterates over each byte in the string, converting it to 8 bits.
- Uses bitwise operations (`&`, `pow(2, $_i)`) to check each bit.

**Usage Context:**
- Debugging binary data or bitmask operations.

---

### `set_time_limit`
**Purpose:**
Extends the script execution time limit if safe mode is disabled.

**Parameters:**

| Name   | Type  | Description                          |
|--------|-------|--------------------------------------|
| `$time`| `int` | Desired time limit in seconds.       |

**Inner Mechanisms:**
- Checks `safe_mode` and `max_execution_time` INI settings.
- Calls PHP’s `set_time_limit()` only if the requested time exceeds the current limit.

**Usage Context:**
- Long-running scripts (e.g., batch processing, file imports).

---

### `anonymize_ip`
**Purpose:**
Anonymizes an IPv4 or IPv6 address by zeroing the last octet/hextet.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$address` | `string` | IP address (e.g., `"192.168.1.1"`).  |

**Return Values:**
- `string`: Anonymized IP (e.g., `"192.168.1.0"` for IPv4, `"2001:db8::"` for IPv6), or `"0.0.0.0"` on failure.

**Inner Mechanisms:**
- Uses `inet_pton()` to convert the IP to binary format.
- Zeroes the last 1/4 of the binary string and converts back to text with `inet_ntop()`.

**Usage Context:**
- Privacy compliance (e.g., GDPR, logs, analytics).

---

### `preview`
**Purpose:**
Renders a standalone HTML preview of content with NUOS’s base styles and scripts.

**Parameters:**

| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$content_code` | `string` | HTML content to preview.             |
| `$stylesheet`   | `string` | Custom stylesheet URL (default: `NULL`). |
| `$body_class`   | `string` | CSS class for the `<body>` (default: `"preview"`). |

**Inner Mechanisms:**
- Outputs a full HTML5 document with:
  - UTF-8 charset.
  - `noindex, nofollow` meta tag.
  - Base URL set to `CMS_ROOT_URL`.
  - NUOS core scripts (`common.js`, `fx.js`, `defer.js`).
  - Fallback styles for `<noscript>`.
  - Custom or default stylesheet.
- Escapes dynamic values with `x()` (XML escaping).

**Usage Context:**
- Previewing content snippets (e.g., emails, reports, or CMS content).

---

### `force_flush`
**Purpose:**
Forces output buffer flushing to the client, optionally padding with random data.

**Parameters:**

| Name   | Type  | Description                          |
|--------|-------|--------------------------------------|
| `$size`| `int` | Size of random padding in bytes (default: `65536`). |

**Inner Mechanisms:**
- Outputs a `<script type="text/plain">` block with random bytes to bypass compression.
- Ends all output buffers with `ob_end_flush()` and calls `flush()`.

**Usage Context:**
- Streaming large responses (e.g., file downloads, real-time updates).


<!-- HASH:9277724f1c7bb57dce558010ec5d45ea -->
