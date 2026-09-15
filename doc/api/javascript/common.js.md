# PWNC API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/common.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## String Utilities

### string_repeat
Repeats a given string a specified number of times.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | The string to repeat |
| count | number | Number of repetitions |

**Returns:** `string` – The repeated string.

**Mechanism:** Wraps the native `String.prototype.repeat()` method.

**Usage:**
```javascript
string_repeat("ab", 3); // "ababab"
```

---

### htmlspecialchars
Converts special HTML characters to their corresponding entities.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | Input string to escape |

**Returns:** `string` – Escaped string with HTML entities.

**Mechanism:** Uses a lookup map and regex replacement for `"`, `&`, `'`, `<`, `>`.

**Usage:**
```javascript
htmlspecialchars("<script>alert('xss')</script>");
// "&lt;script&gt;alert(&apos;xss&apos;)&lt;/script&gt;"
```

---

### unique_id
Generates a random alphanumeric string of a given length.

| Parameter | Type | Description |
|-----------|------|-------------|
| count | number | Length of the generated ID |

**Returns:** `string` – Random alphanumeric string.

**Mechanism:** Selects random characters from a 62-character pool (`0-9A-Za-z`).

**Usage:**
```javascript
unique_id(8); // e.g., "aB3xK9mQ"
```

---

### strabridge
Truncates a string to a specified length, optionally showing beginning and end with an ellipsis in the middle.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| string | string | — | Input string |
| length | number | 50 | Maximum output length |
| cut_end | boolean | false | If true, cuts from the end instead of bridging |

**Returns:** `string` – Truncated string.

**Mechanism:** If `cut_end` is true, slices the string and appends `" …"`. Otherwise, splits the string into start and end portions with `" … "` in between.

**Usage:**
```javascript
strabridge("This is a very long string that needs truncation", 20);
// "This is a … truncation"

strabridge("Long string here", 10, true);
// "Long str …"
```

---

### addslashes
Escapes a string using JSON encoding to simulate PHP-style `addslashes`.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | Input string |

**Returns:** `string` – Escaped string.

**Mechanism:** Uses `JSON.stringify()` and strips surrounding quotes.

**Usage:**
```javascript
addslashes("It's a test"); // "It\\'s a test"
```

---

### strtocolor
Generates a consistent HSL color from a string, ensuring visual distinction between consecutive calls.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| string | string | — | Input string |
| lightness | number | 75 | Lightness percentage |
| diff_min | number | 0 | Minimum hue difference from previous call |

**Returns:** `string` – HSL color string.

**Mechanism:** Uses `djb2` hash to derive a hue. Maintains state via `strtocolor.hue` to ensure minimum hue difference between consecutive calls.

**Usage:**
```javascript
strtocolor("category1"); // "hsl(120, 75%, 75%)"
strtocolor("category2"); // Different hue, at least diff_min apart
```

## Text Control

### textcontrol
Creates a WYSIWYG-style formatting toolbar for textareas or contenteditable elements.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| object | string | — | CSS selector for the target element |
| image_path | string | — | Base path for button images |
| extension | string | "" | Additional HTML to append to the toolbar |

**Returns:** `void`

**Mechanism:** Dynamically builds a `<div>` with anchor buttons, each linked to `textcontrol_set()` with specific formatting commands. Inserts the toolbar before the calling `<script>` tag.

**Usage:**
```html
<script>
textcontrol("#mytextarea", "/assets/images/", '<button>Custom</button>');
</script>
<textarea id="mytextarea"></textarea>
```

---

### textcontrol_set
Applies BBCode-style formatting to selected text in a textarea or contenteditable element.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| object | string | — | CSS selector for the target element |
| format | string | — | Formatting command (e.g., `+`, `/`, `#link`) |
| data | string | "" | Additional data for the format |

**Returns:** `void`

**Mechanism:** Handles selection detection for both textareas and contenteditable elements. Applies formatting based on the `format` parameter, adjusting cursor position after insertion.

**Usage:**
```javascript
textcontrol_set("#mytextarea", "+", ""); // Wrap selection in bold tags
textcontrol_set("#mytextarea", "#link", "https://example.com"); // Insert link
```

---

### textcontrol_remove
Removes formatting from selected text by parsing and reconstructing the content.

| Parameter | Type | Description |
|-----------|------|-------------|
| text | string | Full text content |
| start | number | Selection start position |
| end | number | Selection end position |

**Returns:** `array` – `[cleaned_text, new_start, new_end]`

**Mechanism:** Implements a state machine parser that tracks formatting markers (`[`, `]`, `%`, `|`) and reconstructs text without formatting. Handles nested structures like tables and tokens.

**Usage:**
```javascript
var result = textcontrol_remove("[+bold text]", 0, 12);
// ["bold text", 0, 10]
```

## Location

### load_page
Opens a URL in a new popup window with standardized dimensions and positioning.

| Parameter | Type | Description |
|-----------|------|-------------|
| url | string | URL to open |
| target | string | Window target/name (optional) |

**Returns:** `void`

**Mechanism:** Centers a 900x600 window on screen. Detects external URLs and adds `noopener,noreferrer` for security. Listens for load events to adjust window size.

**Usage:**
```javascript
load_page("https://example.com", "popup");
```

## Form

### limit
Enforces a maximum character limit on a text input or textarea.

| Parameter | Type | Description |
|-----------|------|-------------|
| object | HTMLInputElement/HTMLTextAreaElement | Target form element |
| limit | number | Maximum allowed characters |

**Returns:** `void`

**Mechanism:** Truncates the value if it exceeds the limit while preserving the cursor position.

**Usage:**
```javascript
limit(document.getElementById("username"), 20);
```

## Cookie

### getcookie
Retrieves the value of a named cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| name | string | Cookie name |

**Returns:** `string` – Decoded cookie value, or empty string if not found.

**Mechanism:** Uses regex to parse `document.cookie`.

**Usage:**
```javascript
var theme = getcookie("theme"); // "dark"
```

---

### setcookie
Sets a cookie with optional expiration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| name | string | — | Cookie name |
| value | string | — | Cookie value |
| expires | Date/null | null | Expiration date |

**Returns:** `void`

**Mechanism:** Encodes value, sets `path=/`, `samesite=Strict`, and conditionally adds `secure` flag for HTTPS.

**Usage:**
```javascript
setcookie("theme", "dark", new Date("2025-12-31"));
```

---

### delcookie
Deletes a cookie by setting its expiration to the past.

| Parameter | Type | Description |
|-----------|------|-------------|
| name | string | Cookie name |

**Returns:** `void`

**Mechanism:** Sets cookie with immediate expiration date.

**Usage:**
```javascript
delcookie("session_id");
```

## Clipboard

### clipboard_write
Writes text to the clipboard using the modern API with a fallback.

| Parameter | Type | Description |
|-----------|------|-------------|
| text | string | Text to copy |

**Returns:** `boolean` – Success status.

**Mechanism:** Uses `navigator.clipboard.writeText()` in secure contexts. Falls back to creating a hidden `<textarea>` and using `document.execCommand("copy")`.

**Usage:**
```javascript
if (clipboard_write("Copied text!")) {
    console.log("Copied!");
}
```

## Various

### str2utf8bin
Encodes a string into a binary representation of its UTF-8 bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | Input string |

**Returns:** `string` – Binary string representation.

**Mechanism:** Uses `TextEncoder` to get UTF-8 bytes, then converts to a binary string via `String.fromCodePoint`.

**Usage:**
```javascript
str2utf8bin("hello"); // "h\u0000e\u0000l\u0000l\u0000o\u0000"
```

---

### djb2
Computes a 32-bit hash of a string using the djb2 algorithm.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | Input string |

**Returns:** `number` – 32-bit hash value.

**Mechanism:** Encodes string to UTF-8 bytes, then applies the djb2 hash formula: `hash = ((hash << 5) + hash + byte) & 0xFFFFFFFF`.

**Usage:**
```javascript
djb2("test"); // 220747932
```

---

### crc32
Computes a CRC32 checksum of a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| string | string | Input string |

**Returns:** `number` – Unsigned 32-bit CRC32 checksum.

**Mechanism:** Encodes string to UTF-8 bytes, then applies the standard CRC32 algorithm with polynomial `0xEDB88320`.

**Usage:**
```javascript
crc32("test"); // 3632233996
```

---

### load_script
Dynamically loads a JavaScript file if not already loaded.

| Parameter | Type | Description |
|-----------|------|-------------|
| url | string | Script URL |
| onload | function | Callback when script loads |

**Returns:** `void`

**Mechanism:** Checks for existing script tags with the same `src`. Creates a new `<script>` element with `async=true` and appends to `<head>`.

**Usage:**
```javascript
load_script("/js/plugin.js", function() {
    console.log("Plugin loaded");
});
```

---

### load_css
Dynamically loads a CSS file with preload optimization.

| Parameter | Type | Description |
|-----------|------|-------------|
| url | string | Stylesheet URL |

**Returns:** `void`

**Mechanism:** Checks for existing `<link>` tags. Creates a preload `<link>` that switches to `stylesheet` on load.

**Usage:**
```javascript
load_css("/css/theme.css");
```

---

### document_write
Inserts HTML content before the currently executing script tag.

| Parameter | Type | Description |
|-----------|------|-------------|
| output | string | HTML string to insert |

**Returns:** `void`

**Mechanism:** Uses `document.currentScript` to find the executing script and inserts HTML before it using `insertAdjacentHTML`.

**Usage:**
```javascript
document_write("<div>Dynamic content</div>");
```


<!-- HASH:11996177d14a9520ddf8b0a7a3dafad4 -->
