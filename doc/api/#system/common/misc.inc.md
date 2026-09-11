# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/misc.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/common/misc.inc

This file contains miscellaneous utility functions for the PWNC Web Platform. These functions provide general-purpose helpers for array manipulation, variable initialization, MIME type handling, IP anonymization, HTML preview generation, output flushing, and admin notifications.

## each

Iterates over an array manually, returning the current key-value pair and advancing the internal pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `array` | array | The array to iterate over (passed by reference) |

**Returns:** `array|FALSE` — An associative array with keys `0`, `1`, `"key"`, and `"value"` containing the current element, or `FALSE` if the array is exhausted.

**Inner mechanism:** Uses PHP's `key()`, `current()`, and `next()` functions to manually traverse the array.

**Usage example:**
```php
$arr = ['a' => 1, 'b' => 2];
while (($item = each($arr)) !== FALSE) {
    echo $item['key'] . ' => ' . $item['value'] . "\n";
}
```

## init

Initializes a variable with a default value if it is considered blank.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `variable` | mixed | — | The variable to check and initialize (passed by reference) |
| `default_value` | mixed | `NULL` | The default value to assign if the variable is blank |

**Returns:** `mixed` — The initialized variable value.

**Inner mechanism:** Calls `blank()` to determine if the variable needs initialization.

**Usage example:**
```php
$name = NULL;
init($name, 'Guest');
echo $name; // Outputs: Guest
```

## blank

Determines whether a variable is considered blank (empty or uninitialized).

| Parameter | Type | Description |
|-----------|------|-------------|
| `variable` | mixed | The variable to check (passed by reference) |

**Returns:** `boolean` — `TRUE` if the variable is blank, `FALSE` otherwise.

**Inner mechanism:** Checks if the variable is unset, then applies type-specific logic: scalars are checked as strings, other types use PHP's `empty()`.

**Usage example:**
```php
$value = '';
if (blank($value)) {
    echo "Value is blank";
}
```

## yesno

Converts a boolean value to a localized yes/no string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `boolean` | boolean | The boolean value to convert |

**Returns:** `string` — `CMS_L_COMMON_008` for true, `CMS_L_COMMON_009` for false.

**Usage example:**
```php
echo yesno(TRUE);  // Outputs localized "Yes"
echo yesno(FALSE); // Outputs localized "No"
```

## option

Returns a value conditionally based on a boolean flag.

| Parameter | Type | Description |
|-----------|------|-------------|
| `boolean` | boolean | The condition flag |
| `value` | mixed | The value to return if the condition is true |

**Returns:** `mixed` — The provided value if true, `NULL` if false.

**Usage example:**
```php
$class = option($isActive, 'active');
// Returns 'active' if $isActive is true, NULL otherwise
```

## get_mime_type

Retrieves the MIME type for a given file extension or filename.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `filename_or_extension` | string | `NULL` | A file extension or filename |

**Returns:** `string|array` — The MIME type string, or the full list if no parameter is given.

**Inner mechanism:** Loads MIME types from a file (`mimetype`) on first call and caches them statically. Extracts extensions from filenames when needed.

**Usage example:**
```php
echo get_mime_type('jpg');        // image/jpeg
echo get_mime_type('photo.png');  // image/png
$all_types = get_mime_type();     // Returns full list
```

## get_mime_list

Generates a list of MIME types mapped to their corresponding icon paths.

**Returns:** `array` — Associative array mapping extensions to icon paths.

**Inner mechanism:** Calls `get_mime_type()` to get all types, then maps each to an icon using `get_mime_icon()`.

**Usage example:**
```php
$icons = get_mime_list();
foreach ($icons as $ext => $icon) {
    echo "$ext: $icon\n";
}
```

## get_mime_icon

Finds the appropriate icon path for a given MIME type.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `type` | string | — | The MIME type |
| `default` | string | `"mimetype/default"` | Fallback icon path |

**Returns:** `string` — The icon path for the MIME type.

**Inner mechanism:** Checks for specific type icons first, then falls back to generic category icons (e.g., `mimetype/image` for `image/jpeg`).

**Usage example:**
```php
$icon = get_mime_icon('image/jpeg');
// Returns 'mimetype/image' if specific icon doesn't exist
```

## in_array_recursive

Checks if a value exists anywhere in a nested array structure.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `needle` | mixed | — | The value to search for |
| `haystack` | array | — | The array to search in |
| `strict` | boolean | `FALSE` | Whether to use strict comparison |

**Returns:** `boolean` — `TRUE` if found, `FALSE` otherwise.

**Inner mechanism:** Uses `RecursiveIteratorIterator` to flatten nested arrays for searching.

**Usage example:**
```php
$data = ['level1' => ['level2' => ['target']]];
if (in_array_recursive('target', $data)) {
    echo "Found!";
}
```

## ksort_recursive

Recursively sorts an array by keys.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `array` | array | — | The array to sort (passed by reference) |
| `flag` | integer | `SORT_REGULAR` | Sort flags passed to `ksort()` |

**Returns:** `void`

**Inner mechanism:** Sorts the top-level array, then recursively sorts any array values.

**Usage example:**
```php
$data = [
    'z' => ['y' => 1, 'a' => 2],
    'a' => 3
];
ksort_recursive($data);
// Results in sorted keys at all levels
```

## bitstring

Converts a binary string to its bit representation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `value` | string | — | Binary string to convert |
| `length` | integer | `NULL` | Optional length to truncate/pad to |

**Returns:** `string` — A string of '0's and '1's representing the binary data.

**Inner mechanism:** Iterates through each byte, converting it to 8 binary digits.

**Usage example:**
```php
echo bitstring("\x0F"); // Outputs: 00001111
echo bitstring("\xFF", 4); // Outputs: 1111 (last 4 bits)
```

## set_time_limit

Extends the maximum execution time if needed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | integer | The desired time limit in seconds |

**Returns:** `void`

**Inner mechanism:** Only calls PHP's `set_time_limit()` if the requested time exceeds the current `max_execution_time`.

**Usage example:**
```php
set_time_limit(300); // Extend to 5 minutes if current limit is lower
```

## anonymize_ip

Anonymizes an IP address by zeroing out the last portion.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | string | The IP address to anonymize |

**Returns:** `string` — The anonymized IP address, or `"0.0.0.0"` on failure.

**Inner mechanism:** Uses `inet_pton()`/`inet_ntop()` for binary manipulation, zeroing out the last quarter of the address.

**Usage example:**
```php
echo anonymize_ip('192.168.1.100'); // Outputs: 192.168.0.0
echo anonymize_ip('2001:db8::1');   // Outputs: 2001:db8::
```

## preview

Outputs a complete HTML document for previewing content.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `content_code` | string | — | The HTML content to display in the body |
| `stylesheet` | string | `NULL` | Path to a stylesheet (defaults to common CSS) |
| `body_class` | string | `"preview"` | CSS class for the body element |

**Returns:** `void`

**Inner mechanism:** Generates a full HTML document with meta tags, scripts, and styles, then outputs the content.

**Usage example:**
```php
preview('<h1>Hello World</h1>', NULL, 'my-preview');
// Outputs complete HTML document with the heading
```

## preview_inert

Outputs an HTML document with content rendered inside an inert iframe.

| Parameter | Type | Description |
|-----------|------|-------------|
| `content_code` | string | The HTML content to render in the iframe |

**Returns:** `void`

**Inner mechanism:** Creates an HTML document with a sandboxed iframe using `srcdoc`, with JavaScript to remove the `inert` attribute on load.

**Usage example:**
```php
preview_inert('<p>Safe content</p>');
// Outputs HTML with sandboxed iframe containing the paragraph
```

## force_flush

Forces immediate output flushing with anti-buffering headers.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `size` | integer | `65536` | Size of random padding data |

**Returns:** `void`

**Inner mechanism:** Sends anti-caching headers, ends all output buffers, outputs random padding data, and flushes the buffer.

**Usage example:**
```php
echo "Processing...";
force_flush(); // Ensures "Processing..." is sent immediately
```

## notify_admin

Creates an admin notification message in the system.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | string | The notification message text |

**Returns:** `boolean` — `TRUE` on success, `FALSE` if the core_resource library cannot be loaded.

**Inner mechanism:** Loads the `core_resource` library, creates a new message record with unique IDs and hash, and writes a flag file to signal new notifications.

**Usage example:**
```php
notify_admin('System backup completed successfully');
// Creates a notification in the admin desktop
```>


<!-- HASH:e3be3b74b3dc55328b8ba54a52c133ba -->
