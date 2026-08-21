# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/misc.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Miscellaneous Utility Functions

This file provides core utility functions for the PWNC Web Platform, covering array manipulation, type checking, MIME type handling, IP anonymization, content previewing, and administrative notifications. These functions follow the platform's zero-dependency philosophy while ensuring multibyte safety and context-aware escaping.

---

### `each(&$array)`

**Purpose:**
Replicates PHP's deprecated `each()` function, returning the current key-value pair of an array and advancing its internal pointer.

**Parameters:**

| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| $array | array    | Reference to the array to iterate.   |

**Return Values:**
- `array|false`: Associative array with keys `0`, `1`, `key`, and `value` containing the current key-value pair, or `false` if the end of the array is reached.

**Inner Mechanisms:**
- Uses `key()`, `current()`, and `next()` to retrieve and advance the array pointer.
- Returns `false` if the array pointer is invalid (e.g., at the end of the array).

**Usage Context:**
- Used for backward compatibility with legacy code or when manual array iteration is required.

**Example:**
```php
$fruits = ["a" => "apple", "b" => "banana"];
while ($item = each($fruits)) {
    echo "Key: {$item['key']}, Value: {$item['value']}\n";
}
// Output:
// Key: a, Value: apple
// Key: b, Value: banana
```

---

### `init(&$variable, $default_value = NULL)`

**Purpose:**
Initializes a variable with a default value if it is considered "blank" (e.g., `null`, empty string, or unset).

**Parameters:**

| Name            | Type  | Description                                      |
|-----------------|-------|--------------------------------------------------|
| $variable       | mixed | Reference to the variable to initialize.         |
| $default_value  | mixed | Default value to assign if `$variable` is blank. |

**Return Values:**
- `mixed`: The initialized value of `$variable`.

**Inner Mechanisms:**
- Delegates to `blank()` to check if the variable is blank.
- Assigns `$default_value` if the variable is blank.

**Usage Context:**
- Used to ensure variables have sensible defaults before use, reducing boilerplate checks.

**Example:**
```php
$username = "";
init($username, "guest");
echo $username; // Output: guest
```

---

### `blank(&$variable)`

**Purpose:**
Determines if a variable is "blank" (e.g., `null`, unset, empty string, or evaluates to `false` in a boolean context).

**Parameters:**

| Name      | Type  | Description                          |
|-----------|-------|--------------------------------------|
| $variable | mixed | Reference to the variable to check.  |

**Return Values:**
- `bool`: `true` if the variable is blank, `false` otherwise.

**Inner Mechanisms:**
- Checks if the variable is unset or of a scalar type (boolean, integer, double, string) and empty.
- For non-scalar types, delegates to PHP's `empty()`.

**Usage Context:**
- Used for input validation or conditional logic where "blank" values should be treated as invalid.

**Example:**
```php
$value = 0;
if (blank($value)) {
    echo "Value is blank"; // Output: Value is blank
}
```

---

### `yesno($boolean)`

**Purpose:**
Converts a boolean value into a human-readable "Yes" or "No" string using platform localization constants.

**Parameters:**

| Name      | Type    | Description              |
|-----------|---------|--------------------------|
| $boolean  | bool    | Boolean value to convert.|

**Return Values:**
- `string`: `CMS_L_COMMON_008` ("Yes") if `true`, `CMS_L_COMMON_009` ("No") if `false`.

**Usage Context:**
- Used in user interfaces to display boolean values in a readable format.

**Example:**
```php
echo yesno(true); // Output: Yes (or localized equivalent)
```

---

### `option($boolean, $value)`

**Purpose:**
Returns a value if a boolean condition is `true`, otherwise returns `null`.

**Parameters:**

| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| $boolean  | bool    | Condition to evaluate.               |
| $value    | mixed   | Value to return if `$boolean` is true.|

**Return Values:**
- `mixed|null`: `$value` if `$boolean` is `true`, otherwise `null`.

**Usage Context:**
- Used for conditional value assignment, such as in templating or configuration logic.

**Example:**
```php
$isAdmin = true;
$adminBadge = option($isAdmin, "Admin");
echo $adminBadge; // Output: Admin
```

---

### `get_mime_type($filename_or_extension = NULL)`

**Purpose:**
Retrieves the MIME type for a given file extension or filename. Returns the full MIME type list if no argument is provided.

**Parameters:**

| Name                   | Type   | Description                                      |
|------------------------|--------|--------------------------------------------------|
| $filename_or_extension | string | Filename or file extension (e.g., "png" or "file.png"). |

**Return Values:**
- `string|array`: MIME type (e.g., "image/png") or associative array of all MIME types if `$filename_or_extension` is `null`.
- Defaults to "application/octet-stream" for unknown extensions.

**Inner Mechanisms:**
- Loads MIME types from `CMS_PATH . "mimetype"` on first call (static cache).
- Extracts the file extension if a filename is provided.

**Usage Context:**
- Used for file uploads, downloads, or content-type header generation.

**Example:**
```php
echo get_mime_type("image.jpg"); // Output: image/jpeg
```

---

### `get_mime_list()`

**Purpose:**
Generates a list of file extensions with their associated MIME type icons.

**Return Values:**
- `array`: Associative array where keys are file extensions and values are MIME type icon paths.

**Inner Mechanisms:**
- Delegates to `get_mime_type()` to retrieve the full MIME type list.
- Uses `get_mime_icon()` to resolve icon paths for each MIME type.

**Usage Context:**
- Used in file browsers or asset managers to display file type icons.

**Example:**
```php
$mimeList = get_mime_list();
print_r($mimeList["pdf"]); // Output: mimetype/application_pdf (or similar)
```

---

### `get_mime_icon($type, $default = "mimetype/default")`

**Purpose:**
Resolves the icon path for a given MIME type, with fallback to a default icon.

**Parameters:**

| Name      | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| $type     | string | MIME type (e.g., "image/png").                   |
| $default  | string | Default icon path if no match is found.          |

**Return Values:**
- `string`: Path to the MIME type icon.

**Inner Mechanisms:**
- Checks for an exact MIME type match (e.g., "mimetype/image_png").
- Falls back to the primary type (e.g., "mimetype/image") if no exact match exists.
- Uses `image_exists()` to verify icon availability.

**Usage Context:**
- Used in conjunction with `get_mime_list()` to display file type icons.

**Example:**
```php
echo get_mime_icon("application/pdf"); // Output: mimetype/application_pdf
```

---

### `in_array_recursive($needle, $haystack, $strict = FALSE)`

**Purpose:**
Checks if a value exists in a nested array (recursively).

**Parameters:**

| Name      | Type    | Description                                      |
|-----------|---------|--------------------------------------------------|
| $needle   | mixed   | Value to search for.                             |
| $haystack | array   | Array to search in.                              |
| $strict   | bool    | Whether to use strict comparison (`===`).        |

**Return Values:**
- `bool`: `true` if the value is found, `false` otherwise.

**Inner Mechanisms:**
- Uses `RecursiveIteratorIterator` and `RecursiveArrayIterator` to traverse nested arrays.
- Supports both loose (`==`) and strict (`===`) comparison.

**Usage Context:**
- Used for deep array searches, such as in configuration validation or data filtering.

**Example:**
```php
$data = ["a" => ["b" => ["c" => 1]]];
echo in_array_recursive(1, $data) ? "Found" : "Not found"; // Output: Found
```

---

### `ksort_recursive(&$array, $flag = SORT_REGULAR)`

**Purpose:**
Recursively sorts an array by keys while preserving nested array structures.

**Parameters:**

| Name    | Type   | Description                                      |
|---------|--------|--------------------------------------------------|
| $array  | array  | Reference to the array to sort.                  |
| $flag   | int    | Sorting flags (e.g., `SORT_REGULAR`, `SORT_STRING`). |

**Return Values:**
- `void`: Modifies the array in-place.

**Inner Mechanisms:**
- Uses `ksort()` to sort the top-level array.
- Recursively applies `ksort_recursive()` to nested arrays.

**Usage Context:**
- Used for organizing configuration arrays or structured data before serialization.

**Example:**
```php
$data = ["b" => ["d" => 2, "c" => 1], "a" => 0];
ksort_recursive($data);
print_r($data);
// Output: Array ( [a] => 0 [b] => Array ( [c] => 1 [d] => 2 ) )
```

---

### `bitstring($value, $length = NULL)`

**Purpose:**
Converts a binary string into its binary representation (e.g., "A" → "01000001").

**Parameters:**

| Name    | Type   | Description                                      |
|---------|--------|--------------------------------------------------|
| $value  | string | Binary string to convert.                        |
| $length | int    | Optional length to truncate the output.          |

**Return Values:**
- `string`: Binary representation of the input string.

**Inner Mechanisms:**
- Iterates over each byte in the input string.
- Converts each byte to its 8-bit binary representation.
- Truncates the result if `$length` is provided.

**Usage Context:**
- Used for debugging binary data or low-level bit manipulation.

**Example:**
```php
echo bitstring("A"); // Output: 01000001
```

---

### `set_time_limit($time)`

**Purpose:**
Sets the script execution time limit if the requested time exceeds the current limit.

**Parameters:**

| Name  | Type  | Description                          |
|-------|-------|--------------------------------------|
| $time | int   | Desired execution time in seconds.   |

**Return Values:**
- `void`

**Inner Mechanisms:**
- Compares `$time` with `ini_get("max_execution_time")`.
- Calls PHP's `set_time_limit()` only if necessary.

**Usage Context:**
- Used in long-running scripts (e.g., batch processing) to prevent timeouts.

**Example:**
```php
set_time_limit(300); // Extend execution time to 5 minutes
```

---

### `anonymize_ip($address)`

**Purpose:**
Anonymizes an IP address by zeroing out the last octet (IPv4) or last 80 bits (IPv6).

**Parameters:**

| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| $address  | string | IP address (IPv4 or IPv6).           |

**Return Values:**
- `string`: Anonymized IP address (e.g., "192.168.1.0" or "2001:db8::").

**Inner Mechanisms:**
- Uses `inet_pton()` to convert the IP address to binary format.
- Truncates the last portion of the binary string and pads with zeros.
- Uses `inet_ntop()` to convert back to a human-readable format.

**Usage Context:**
- Used for privacy compliance (e.g., GDPR) when logging IP addresses.

**Example:**
```php
echo anonymize_ip("192.168.1.42"); // Output: 192.168.1.0
```

---

### `preview($content_code, $stylesheet = NULL, $body_class = "preview")`

**Purpose:**
Renders HTML content in a standalone preview window with platform assets (CSS/JS) and a base URL.

**Parameters:**

| Name           | Type   | Description                                      |
|----------------|--------|--------------------------------------------------|
| $content_code  | string | HTML content to preview.                         |
| $stylesheet    | string | Optional custom stylesheet URL.                  |
| $body_class    | string | CSS class for the `<body>` element.              |

**Return Values:**
- `void`: Outputs HTML directly to the browser.

**Inner Mechanisms:**
- Injects platform JavaScript files (`common.js`, `fx.js`, `defer.js`).
- Sets a `<base>` tag to ensure relative URLs resolve correctly.
- Uses `x()` for XML escaping and `CMS_ROOT_URL` for the base URL.

**Usage Context:**
- Used for WYSIWYG previews, template testing, or content previews.

**Example:**
```php
preview("<h1>Hello, World!</h1>");
// Renders a full HTML page with the heading.
```

---

### `preview_inert($content_code)`

**Purpose:**
Renders HTML content in an inert `<iframe>` with a sandboxed environment to prevent script execution.

**Parameters:**

| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| $content_code | string | HTML content to preview.             |

**Return Values:**
- `void`: Outputs HTML directly to the browser.

**Inner Mechanisms:**
- Uses `srcdoc` to embed the content in an `<iframe>`.
- Applies the `sandbox` attribute to restrict script execution.
- Sets the `inert` attribute on the iframe's document to prevent interaction.

**Usage Context:**
- Used for secure previews of untrusted content (e.g., user-generated HTML).

**Example:**
```php
preview_inert("<script>alert('Blocked');</script><p>Safe content</p>");
// Renders the paragraph without executing the script.
```

---

### `force_flush($size = 65536)`

**Purpose:**
Forces output buffering to flush, ensuring real-time updates in long-running scripts.

**Parameters:**

| Name  | Type  | Description                                      |
|-------|-------|--------------------------------------------------|
| $size | int   | Size of the dummy output to generate (in bytes). |

**Return Values:**
- `void`

**Inner Mechanisms:**
- Disables caching and compression headers if not already sent.
- Ends all output buffers.
- Generates a dummy `<script>` tag with random bytes to force a flush.

**Usage Context:**
- Used in streaming or long-polling scripts to ensure timely output delivery.

**Example:**
```php
echo "Starting...";
force_flush();
sleep(2);
echo "Done.";
// Output appears immediately, not after 2 seconds.
```

---

### `notify_admin($string)`

**Purpose:**
Sends an administrative notification via the platform's messaging system.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| $string | string | Notification message.                |

**Return Values:**
- `bool`: `true` if the notification was sent, `false` if the messaging system failed to load.

**Inner Mechanisms:**
- Loads the `core_resource` library to access the messaging system.
- Generates a unique thread ID, timestamp, and hash for the message.
- Writes a flag file to trigger a notification in the admin interface.

**Usage Context:**
- Used for error reporting, alerts, or system events requiring admin attention.

**Example:**
```php
notify_admin("Database connection failed");
// Sends a notification to the admin dashboard.
```


<!-- HASH:e3be3b74b3dc55328b8ba54a52c133ba -->
