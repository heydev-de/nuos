# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/misc.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## misc.inc

This file contains miscellaneous utility functions for the PWNC Web Platform. These functions provide common operations such as array manipulation, value initialization, MIME type handling, IP anonymization, HTML preview generation, and administrative notifications.

### each

Iterates over an array and returns the current key-value pair.

| Parameter | Type | Description |
|-----------|------|-------------|
| `array` | array | The array to iterate over (passed by reference) |

**Return Value:** `array|FALSE` - An array containing the key and value, or `FALSE` if the array is empty.

**Inner Mechanism:** Uses PHP's internal array pointer functions (`key()`, `current()`, `next()`) to traverse the array.

**Usage Example:**
```php
$arr = ['a' => 1, 'b' => 2];
while (($item = each($arr)) !== false) {
    echo "Key: {$item[0]}, Value: {$item[1]}\n";
}
```

### init

Initializes a variable with a default value if it is blank.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `variable` | mixed | - | The variable to check and initialize (passed by reference) |
| `default_value` | mixed | `NULL` | The default value to assign if the variable is blank |

**Return Value:** `mixed` - The initialized variable.

**Inner Mechanism:** Checks if the variable is blank using the `blank()` function, and if so, assigns the default value.

**Usage Example:**
```php
$name = NULL;
init($name, "Guest");
echo $name; // Outputs: Guest
```

### blank

Determines whether a variable is considered blank.

| Parameter | Type | Description |
|-----------|------|-------------|
| `variable` | mixed | The variable to check (passed by reference) |

**Return Value:** `boolean` - `TRUE` if the variable is blank, `FALSE` otherwise.

**Inner Mechanism:** Returns `TRUE` if the variable is not set or if it's a scalar type with an empty string representation. For other types, uses PHP's `empty()` function.

**Usage Example:**
```php
$value = "";
if (blank($value)) {
    echo "Value is blank";
}
```

### yesno

Converts a boolean value to a localized yes/no string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `boolean` | boolean | The boolean value to convert |

**Return Value:** `string` - A localized string representing "yes" or "no".

**Inner Mechanism:** Returns `CMS_L_COMMON_008` for `TRUE` and `CMS_L_COMMON_009` for `FALSE`.

**Usage Example:**
```php
echo yesno(true);  // Outputs the localized "yes" string
echo yesno(false); // Outputs the localized "no" string
```

### option

Returns a value if a condition is true, otherwise returns NULL.

| Parameter | Type | Description |
|-----------|------|-------------|
| `boolean` | boolean | The condition to evaluate |
| `value` | mixed | The value to return if the condition is true |

**Return Value:** `mixed|NULL` - The provided value if the condition is true, `NULL` otherwise.

**Usage Example:**
```php
$result = option($isAdmin, "Admin privileges granted");
echo $result ?? "No admin privileges";
```

### get_mime_type

Retrieves the MIME type for a given file extension or filename.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `filename_or_extension` | string | `NULL` | A filename or extension to look up |

**Return Value:** `string|array` - The MIME type as a string, or the entire MIME type list if no parameter is provided.

**Inner Mechanism:** Loads MIME types from a file (`mimetype`) on first call and caches them statically. If a filename is provided, extracts the extension before lookup.

**Usage Example:**
```php
$mimeType = get_mime_type("document.pdf");
echo $mimeType; // Outputs: application/pdf

$allTypes = get_mime_type(); // Returns all MIME types
```

### get_mime_list

Generates a list of MIME types mapped to their corresponding icons.

**Return Value:** `array` - An associative array mapping file extensions to icon paths.

**Inner Mechanism:** Calls `get_mime_type()` to get all MIME types, then maps each to an icon using `get_mime_icon()`.

**Usage Example:**
```php
$mimeIcons = get_mime_list();
foreach ($mimeIcons as $ext => $icon) {
    echo "Extension: $ext, Icon: $icon\n";
}
```

### get_mime_icon

Finds an appropriate icon for a given MIME type.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `type` | string | - | The MIME type to find an icon for |
| `default` | string | `"mimetype/default"` | The default icon path if no specific icon is found |

**Return Value:** `string` - The path to the icon image.

**Inner Mechanism:** First checks for an exact MIME type match, then falls back to the main type (before the `/`), and finally returns the default.

**Usage Example:**
```php
$icon = get_mime_icon("image/jpeg");
echo $icon; // Outputs path to jpeg icon or default
```

### in_array_recursive

Searches for a value in a multidimensional array.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `needle` | mixed | - | The value to search for |
| `haystack` | array | - | The array to search in |
| `strict` | boolean | `FALSE` | Whether to use strict comparison |

**Return Value:** `boolean` - `TRUE` if the value is found, `FALSE` otherwise.

**Inner Mechanism:** Uses PHP's `RecursiveIteratorIterator` to traverse all elements in a multidimensional array.

**Usage Example:**
```php
$arr = [1, [2, [3, 4]], 5];
if (in_array_recursive(3, $arr)) {
    echo "Found 3 in the array";
}
```

### ksort_recursive

Recursively sorts an array by keys.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `array` | array | - | The array to sort (passed by reference) |
| `flag` | integer | `SORT_REGULAR` | Sort flags passed to `ksort()` |

**Return Value:** `void` - The array is modified in place.

**Inner Mechanism:** Sorts the top-level array by keys, then recursively sorts any sub-arrays.

**Usage Example:**
```php
$data = [
    'z' => ['c' => 3, 'a' => 1],
    'a' => ['b' => 2, 'a' => 1]
];
ksort_recursive($data);
print_r($data);
```

### bitstring

Converts a binary string to its bit representation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `value` | string | - | The binary string to convert |
| `length` | integer | `NULL` | Optional length to truncate the result to |

**Return Value:** `string` - A string of '0's and '1's representing the binary data.

**Inner Mechanism:** Iterates through each byte of the input string and converts it to 8 binary digits.

**Usage Example:**
```php
$bits = bitstring("A");
echo $bits; // Outputs: 01000001

$truncated = bitstring("ABC", 8);
echo $truncated; // Outputs: 01000010 (last 8 bits)
```

### set_time_limit

Sets the maximum execution time for a script.

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | integer | The time limit in seconds |

**Return Value:** `void`

**Inner Mechanism:** Only calls PHP's native `set_time_limit()` if the specified time exceeds the current `max_execution_time` setting.

**Usage Example:**
```php
set_time_limit(300); // Set 5-minute execution limit
```

### anonymize_ip

Anonymizes an IP address by zeroing out part of it.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | string | The IP address to anonymize |

**Return Value:** `string` - The anonymized IP address.

**Inner Mechanism:** Converts the IP to binary form, calculates a portion to preserve (1/4 of the address), and replaces the rest with zeros.

**Usage Example:**
```php
$anonymized = anonymize_ip("192.168.1.100");
echo $anonymized; // Outputs: 192.168.0.0
```

### preview

Generates an HTML preview page with the given content.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `content_code` | string | - | The HTML content to display |
| `stylesheet` | string | `NULL` | Path to a stylesheet (defaults to CMS_DATA_URL) |
| `body_class` | string | `"preview"` | CSS class for the body element |

**Return Value:** `void` - Outputs HTML directly.

**Inner Mechanism:** Outputs a complete HTML document with proper headers, scripts, and styling for previewing content.

**Usage Example:**
```php
preview("<h1>Hello World</h1>", NULL, "my-preview");
```

### preview_inert

Generates an HTML preview page using an iframe with inert content.

| Parameter | Type | Description |
|-----------|------|-------------|
| `content_code` | string | The HTML content to display in the iframe |

**Return Value:** `void` - Outputs HTML directly.

**Inner Mechanism:** Creates an HTML document with a sandboxed iframe that loads the content via `srcdoc`, making it inert until explicitly activated.

**Usage Example:**
```php
preview_inert("<p>This content is inert</p>");
```

### force_flush

Forces output flushing and sends a chunk of random data.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `size` | integer | `65536` | Size of random data to send |

**Return Value:** `void`

**Inner Mechanism:** Sets appropriate headers to prevent buffering, ends all output buffers, sends random data wrapped in a script tag, and flushes the output.

**Usage Example:**
```php
force_flush(); // Force immediate output
```

### notify_admin

Sends a notification to the admin via the internal messaging system.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | string | The notification message |

**Return Value:** `boolean` - `TRUE` on success, `FALSE` if the core_resource library cannot be loaded.

**Inner Mechanism:** Creates a new message in the internal messaging system with the provided text, assigns it to the admin user, and creates a notification flag file.

**Usage Example:**
```php
notify_admin("System backup completed successfully");
```

### array_get_path

Retrieves a value from a multidimensional array using a path notation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `array` | array | - | The array to search in |
| `path` | string | - | Dot or slash-separated path to the desired value |
| `delimiter` | string | `"/"` | The delimiter used in the path |

**Return Value:** `mixed|NULL` - The value at the specified path, or `NULL` if not found.

**Inner Mechanism:** Splits the path by the delimiter and traverses the array step by step, returning `NULL` if any part of the path doesn't exist.

**Usage Example:**
```php
$data = ['user' => ['profile' => ['name' => 'John']]];
$name = array_get_path($data, "user/profile/name");
echo $name; // Outputs: John


<!-- HASH:423e1753673da07a5569749789c2848b -->
