# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/misc.inc)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File: `system/common/misc.inc`

This file contains a collection of general-purpose utility functions in the `cms` namespace. These functions provide common operations such as array manipulation, string checks, MIME type resolution, IP anonymization, HTML preview generation, admin notifications, and nested array path access. They are foundational helpers used throughout the PWNC platform.

---

### `each`

**Purpose:** Reimplements PHP's legacy `each()` function (removed in PHP 8.0). Returns the current key/value pair of an array and advances the internal array pointer.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `array` | `array` (by reference) | The array to iterate over. The internal pointer is advanced after each call. |

**Return values:**

| Type | Description |
|------|-------------|
| `array` | An array with keys `0`, `1`, `"key"`, and `"value"` representing the current element. |
| `FALSE` | If the array is empty or the pointer is past the end. |

**Inner mechanisms:** Uses `key()` to get the current key, `current()` to get the value, and `next()` to advance the pointer. Returns `FALSE` when the key is `NULL`.

**Usage example:**
```php
$arr = ["a" => 1, "b" => 2];
while (($item = each($arr)) !== FALSE) {
    echo $item["key"] . " => " . $item["value"] . "\n";
}
// Output: a => 1, b => 2
```

---

### `init`

**Purpose:** Initializes a variable with a default value if it is considered "blank" (unset or empty).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `variable` | `mixed` (by reference) | — | The variable to check and potentially initialize. |
| `default_value` | `mixed` | `NULL` | The value to assign if the variable is blank. |

**Return values:**

| Type | Description |
|------|-------------|
| `mixed` | The variable's value (either the original or the default). |

**Inner mechanisms:** Calls `blank()` to determine if the variable is blank. If so, assigns `$default_value` to it. Returns the variable regardless.

**Usage example:**
```php
$name = NULL;
init($name, "Guest");
echo $name; // Output: Guest
```

---

### `blank`

**Purpose:** Determines whether a variable is "blank" — meaning it is not set, is an empty string, or is an empty value.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `variable` | `mixed` (by reference) | The variable to check. |

**Return values:**

| Type | Description |
|------|-------------|
| `TRUE` | If the variable is not set or is empty. |
| `FALSE` | If the variable has a non-empty value. |

**Inner mechanisms:** Returns `TRUE` immediately if the variable is not set. For scalar types (boolean, integer, double, string), checks if the string representation is `""`. For all other types, falls back to PHP's `empty()`.

**Usage example:**
```php
$val = "";
if (blank($val)) {
    echo "Variable is blank.";
}
```

---

### `yesno`

**Purpose:** Returns a localized "yes" or "no" string based on a boolean value.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `boolean` | `bool` | The boolean value to evaluate. |

**Return values:**

| Type | Description |
|------|-------------|
| `string` | `CMS_L_COMMON_008` (yes) if true, `CMS_L_COMMON_009` (no) if false. |

**Inner mechanisms:** Uses the ternary operator to select between two localized language constants.

**Usage example:**
```php
echo yesno(true);  // Outputs the localized "yes" string
echo yesno(false); // Outputs the localized "no" string
```

---

### `option`

**Purpose:** Conditionally returns a value based on a boolean. Returns the value if the boolean is true; returns `NULL` otherwise.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `boolean` | `bool` | The condition to evaluate. |
| `value` | `mixed` | The value to return if the condition is true. |

**Return values:**

| Type | Description |
|------|-------------|
| `mixed` | `$value` if `$boolean` is true, `NULL` otherwise. |

**Inner mechanisms:** Simple ternary: `$boolean ? $value : NULL`.

**Usage example:**
```php
$label = option($isActive, "Active");
// $label is "Active" if $isActive is true, NULL otherwise
```

---

### `get_mime_type`

**Purpose:** Resolves a MIME type from a file extension or filename. Loads the MIME type mapping from a file on first call and caches it statically.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `filename_or_extension` | `string\|NULL` | `NULL` | A filename (e.g., `"photo.jpg"`) or a bare extension (e.g., `"jpg"`). If `NULL`, returns the full mapping array. |

**Return values:**

| Type | Description |
|------|-------------|
| `string` | The MIME type string (e.g., `"image/jpeg"`). |
| `array` | The full extension-to-MIME-type mapping if `$filename_or_extension` is `NULL`. |
| `string` | `"application/octet-stream"` if the extension is not found. |

**Inner mechanisms:** On first call, reads the `mimetype` file from `CMS_PATH` (one extension + type per line, space-separated). If a filename with a dot is passed, extracts the extension via `file_extension()`. Looks up the extension in the cached list, falling back to `"application/octet-stream"`.

**Usage example:**
```php
echo get_mime_type("document.pdf"); // "application/pdf"
echo get_mime_type("jpg");          // "image/jpeg"
$all = get_mime_type();             // Returns full mapping array
```

---

### `get_mime_list`

**Purpose:** Builds a list of file extensions mapped to their corresponding icon paths, using the MIME type mapping.

**Parameters:** None.

**Return values:**

| Type | Description |
|------|-------------|
| `array` | An associative array where keys are file extensions and values are icon paths (or `NULL` if no icon exists). |

**Inner mechanisms:** Calls `get_mime_type()` to get the full mapping, then iterates over each extension/type pair, calling `get_mime_icon()` to resolve the icon path. Only includes entries where an icon is found.

**Usage example:**
```php
$icons = get_mime_list();
// e.g., ["pdf" => "mimetype/application/pdf", "jpg" => "mimetype/image/jpeg", ...]
```

---

### `get_mime_icon`

**Purpose:** Resolves the icon path for a given MIME type. Checks for a specific icon first, then falls back to a category-level icon (e.g., `image/*` → `mimetype/image`).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `type` | `string` | — | The MIME type (e.g., `"image/jpeg"`). |
| `default` | `string\|NULL` | `"mimetype/default"` | The fallback icon path if no specific or category icon is found. |

**Return values:**

| Type | Description |
|------|-------------|
| `string` | The icon path (e.g., `"mimetype/image/jpeg"` or `"mimetype/image"`). |
| `string\|NULL` | The `$default` value if no icon is found. |

**Inner mechanisms:** First checks if an image exists at `mimetype/$type`. If not, extracts the category (the part before `/`) and checks `mimetype/$category`. Returns the default if neither exists.

**Usage example:**
```php
$icon = get_mime_icon("image/png");
// Returns "mimetype/image/png" if it exists, or "mimetype/image" as fallback
```

---

### `in_array_recursive`

**Purpose:** Searches for a value within a nested (multi-dimensional) array.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `needle` | `mixed` | — | The value to search for. |
| `haystack` | `array` | — | The array to search in (can be nested). |
| `strict` | `bool` | `FALSE` | If `TRUE`, uses strict comparison (`===`). |

**Return values:**

| Type | Description |
|------|-------------|
| `TRUE` | If the needle is found at any depth. |
| `FALSE` | If the needle is not found. |

**Inner mechanisms:** Uses `RecursiveIteratorIterator` with `RecursiveArrayIterator` to flatten the nested array traversal. Compares each element using either `==` or `===` depending on `$strict`.

**Usage example:**
```php
$data = ["a" => [1, 2], "b" => [3, [4, 5]]];
var_dump(in_array_recursive(4, $data));        // true
var_dump(in_array_recursive(4, $data, TRUE));   // true (strict)
```

---

### `ksort_recursive`

**Purpose:** Recursively sorts an array and all its sub-arrays by key.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `array` | `array` (by reference) | — | The array to sort. |
| `flag` | `int` | `SORT_REGULAR` | The sort flag passed to `ksort()` (e.g., `SORT_STRING`, `SORT_NUMERIC`). |

**Return values:** None (modifies the array in place).

**Inner mechanisms:** Calls `ksort()` on the top-level array, then iterates over each value. If a value is itself an array, recursively calls `ksort_recursive()` on it.

**Usage example:**
```php
$data = ["b" => ["y" => 2, "x" => 1], "a" => 3];
ksort_recursive($data);
// $data is now ["a" => 3, "b" => ["x" => 1, "y" => 2]]
```

---

### `bitstring`

**Purpose:** Converts a binary string into a human-readable string of `0`s and `1`s, representing each byte in 8-bit binary notation.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `value` | `string` | — | The binary string to convert. |
| `length` | `int\|NULL` | `NULL` | If provided, returns only the last `$length` bits. |

**Return values:**

| Type | Description |
|------|-------------|
| `string` | A string of `0` and `1` characters representing the binary content. |

**Inner mechanisms:** Iterates over each byte of the input string, extracts each bit using bitwise AND with `(1 << $_i)` for bit positions 7 down to 0, and appends `"1"` or `"0"` accordingly. If `$length` is specified, truncates the result to the last `$length` characters.

**Usage example:**
```php
echo bitstring("A");        // "01000001"
echo bitstring("A", 4);     // "0001" (last 4 bits)
```

---

### `set_time_limit`

**Purpose:** Sets the maximum execution time for the script, but only if the requested time exceeds the current `max_execution_time` setting.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `time` | `int` | The desired maximum execution time in seconds. |

**Return values:** None.

**Inner mechanisms:** Compares `$time` with `ini_get("max_execution_time")`. If `$time` is greater, calls PHP's native `set_time_limit()`. This prevents lowering the limit below what is already configured.

**Usage example:**
```php
set_time_limit(300); // Extends execution time to 300s if current limit is lower
```

---

### `anonymize_ip`

**Purpose:** Anonymizes an IP address by zeroing out the last quarter of the address (last 8 bits for IPv4, last 32 bits for IPv6).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `address` | `string` | The IP address to anonymize (IPv4 or IPv6). |

**Return values:**

| Type | Description |
|------|-------------|
| `string` | The anonymized IP address. |
| `string` | `"0.0.0.0"` if the input is not a valid IP address. |

**Inner mechanisms:** Uses `inet_pton()` to convert the IP to binary. Calculates the length of one quarter of the binary representation. Keeps the first 3/4 of the address and replaces the last 1/4 with null bytes (`chr(0)`). Converts back with `inet_ntop()`.

**Usage example:**
```php
echo anonymize_ip("192.168.1.100"); // "192.168.1.0"
echo anonymize_ip("10.0.0.5");      // "10.0.0.0"
```

---

### `preview`

**Purpose:** Outputs a complete HTML document for previewing content, including standard PWNC JavaScript and CSS resources.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `content_code` | `string` | — | The HTML content to display in the body. |
| `stylesheet` | `string\|NULL` | `NULL` | Path to a stylesheet. If blank, defaults to `CMS_DATA_URL . "stylesheet.css"`. |
| `body_class` | `string` | `"preview"` | CSS class applied to the `<body>` element. |

**Return values:** None (outputs HTML directly).

**Inner mechanisms:** Constructs a full HTML5 document with charset, title, robots meta tag, base href, script includes (`common.js`, `fx.js`, `defer.js`), a noscript fallback, and a stylesheet link. All dynamic values are escaped using `x()`.

**Usage example:**
```php
preview("<h1>Hello World</h1>", NULL, "my-preview");
// Outputs a full HTML page with the heading in the body
```

---

### `preview_inert`

**Purpose:** Outputs an HTML document that displays content inside a sandboxed `<iframe>` with the `inert` attribute applied, preventing interaction until explicitly enabled.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `content_code` | `string` | The HTML content to embed inside the iframe's `srcdoc`. |

**Return values:** None (outputs HTML directly).

**Inner mechanisms:** Outputs a minimal HTML document with inline CSS for body margin reset and iframe styling. The iframe uses `srcdoc` to embed the content, `sandbox="allow-same-origin"` for security, and an `onload` handler that removes the `inert` attribute from the document element. The content is escaped using `x()`.

**Usage example:**
```php
preview_inert("<p>This content is initially inert.</p>");
// Outputs an HTML page with a sandboxed iframe containing the paragraph
```

---

### `force_flush`

**Purpose:** Forces immediate output flushing by sending anti-buffering headers, closing all output buffers, and emitting a padding script to overcome reverse proxy buffering.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `size` | `int` | `65536` | The size of random padding bytes to emit (to force buffer flush). |

**Return values:** None.

**Inner mechanisms:** If headers haven't been sent, sends `Cache-Control: no-transform`, `Content-Encoding: none`, and `X-Accel-Buffering: no` headers. Then closes all output buffer levels with `ob_end_flush()`. Emits a `<script type="text/plain">` block containing random bytes (with null bytes and `<` replaced by `_`) to force the output buffer to flush. Calls `flush()`.

**Usage example:**
```php
echo "Processing step 1...\n";
force_flush();
// Ensures the output is sent to the browser immediately
```

---

### `notify_admin`

**Purpose:** Creates an admin notification message using the `core_resource` system, writing a message to the desktop inbox and creating a notification flag file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `string` | `string` | The notification message text. |

**Return values:**

| Type | Description |
|------|-------------|
| `TRUE` | If the notification was successfully created. |
| `FALSE` | If the `core_resource` library could not be loaded. |

**Inner mechanisms:** Loads the `core_resource` library. Creates a `core_resource` instance pointing to `CMS_DATA_PATH . "#desktop/ims.core"` with a defined schema. Generates a unique thread ID and timestamp. Prepends a localized prefix (`CMS_L_COMMON_001`) to the message. Computes a hash using `hash32()`. Seeks to an empty owner record, then sets the message fields (id, thread, time, owner, receiver, sender, status, text, hash). Finally, writes a flag file at `CMS_DATA_PATH . "#desktop/admin/ims.flag"` to signal a new notification.

**Usage example:**
```php
notify_admin("Database backup completed successfully.");
// Creates a notification in the admin's desktop inbox
```

---

### `array_get_path`

**Purpose:** Retrieves a value from a nested array using a delimited path string (e.g., `"user/profile/name"`).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `array` | `array` | — | The source array. |
| `path` | `string\|array` | — | A path string (e.g., `"a/b/c"`) or an array of path strings to try in order. |
| `delimiter` | `string` | `"/"` | The delimiter used to split the path into keys. |

**Return values:**

| Type | Description |
|------|-------------|
| `mixed` | The value found at the path. |
| `NULL` | If the path does not exist or the array structure doesn't match. |

**Inner mechanisms:** If `$path` is an array, iterates over each path string and returns the first non-`NULL` result. If `$path` is blank, returns the entire array. Otherwise, splits the path by the delimiter and traverses the array level by level, returning `NULL` if any key is missing or the current value is not an array.

**Usage example:**
```php
$data = ["user" => ["profile" => ["name" => "Alice"]]];
echo array_get_path($data, "user/profile/name"); // "Alice"
echo array_get_path($data, ["missing/path", "user/profile/name"]); // "Alice"
```

---

### `array_set_path`

**Purpose:** Sets a value in a nested array at a specified path, creating intermediate arrays as needed.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `array` | `array` (by reference) | — | The target array (modified in place). |
| `path` | `string` | — | A path string (e.g., `"a/b/c"`) specifying where to set the value. |
| `value` | `mixed` | — | The value to set at the path. |
| `delimiter` | `string` | `"/"` | The delimiter used to split the path into keys. |

**Return values:**

| Type | Description |
|------|-------------|
| `TRUE` | If the value was successfully set. |
| `FALSE` | If an intermediate key exists but is not an array (cannot traverse further). |

**Inner mechanisms:** If `$path` is blank, replaces the entire array with `$value` and returns `TRUE`. Otherwise, splits the path into keys, pops the last key as the target, and traverses the array using references. Creates empty arrays for missing intermediate keys. If an intermediate key exists but is not an array, returns `FALSE`. Sets the final key to `$value`.

**Usage example:**
```php
$data = [];
array_set_path($data, "user/profile/name", "Alice");
// $data is now ["user" => ["profile" => ["name" => "Alice"]]]

$data = ["user" => "existing_string"];
array_set_path($data, "user/profile", "Bob"); // Returns FALSE (can't traverse into a string)
```


<!-- HASH:0077cec066eefebf1baddc2f0c573c0a -->
