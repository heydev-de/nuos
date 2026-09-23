# PWNC API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/common.js)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## String

### string_repeat

Repeats a given string a specified number of times. A thin wrapper around the native `String.prototype.repeat()` method.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The string to repeat. |
| `count` | `number` | The number of times to repeat the string. |

**Returns:** `string` — The repeated string.

**Inner mechanism:** Delegates directly to `string.repeat(count)`, which throws a `RangeError` if `count` is negative or infinite.

**Usage context:** Useful for generating padding, indentation, or repeated patterns.

```javascript
string_repeat("ab", 3); // "ababab"
string_repeat("-", 10); // "----------"
```

---

### htmlspecialchars

Converts special HTML characters in a string to their corresponding HTML entities. Mirrors the behavior of PHP's `htmlspecialchars()`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The input string to encode. |

**Returns:** `string` — The encoded string with `"` → `&quot;`, `&` → `&amp;`, `<` → `&lt;`, `>` → `&gt;`.

**Inner mechanism:** Uses a lookup object and a single regex replace to swap all four characters in one pass.

**Usage context:** Safely embedding user-generated content into HTML to prevent XSS.

```javascript
htmlspecialchars("<script>alert('xss')</script>");
// "&lt;script&gt;alert('xss')&lt;/script&gt;"
```

---

### unique_id

Generates a random alphanumeric string of a specified length using characters `0-9`, `A-Z`, and `a-z`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `count` | `number` | The desired length of the generated string. |

**Returns:** `string` — A random string of length `count`.

**Inner mechanism:** Iterates `count` times, each time picking a random character from a 62-character alphabet using `Math.random()`.

**Usage context:** Generating unique DOM element IDs, temporary tokens, or non-cryptographic identifiers.

```javascript
unique_id(8);  // e.g., "aB3xK9mQ"
unique_id(16); // e.g., "Z7n2pR4sT8vW0xY1"
```

---

### strabridge

Truncates a string to a specified length, optionally showing both the beginning and end of the string with an ellipsis in the middle.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `string` | `string` | — | The input string to truncate. |
| `length` | `number` | `50` | The maximum output length including ellipsis. |
| `cut_end` | `boolean` | `false` | If `true`, truncates from the end with a trailing ellipsis. If `false`, shows start + middle ellipsis + end. |

**Returns:** `string` — The truncated string. If the input is shorter than `length`, it is returned unchanged.

**Inner mechanism:**
- If `cut_end` is `true`: slices the first `length - 2` characters and appends `" …"`.
- If `cut_end` is `false`: allocates 65% of the remaining space to the start and 35% to the end, joining them with `" … "`.

**Usage context:** Displaying long text in limited space, such as table cells, tooltips, or preview snippets.

```javascript
strabridge("This is a very long string that needs truncation", 20);
// "This is a very … truncation"

strabridge("This is a very long string that needs truncation", 20, true);
// "This is a very long …"
```

---

### addslashes

Escapes a string by wrapping it in `JSON.stringify()` and stripping the outer quotes. This effectively escapes quotes, backslashes, and control characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The input string to escape. |

**Returns:** `string` — The escaped string with special characters backslash-escaped.

**Inner mechanism:** `JSON.stringify()` produces a quoted, escaped string literal. The `.slice(1, -1)` removes the surrounding double quotes, leaving only the escaped content.

**Usage context:** Preparing strings for embedding in JavaScript string literals or passing to functions that expect escaped input.

```javascript
addslashes("He said \"Hello\" and left.");
// "He said \\\"Hello\\\" and left."
```

---

### strtocolor

Generates a deterministic HSL color from a string, with optional hue differentiation to avoid similar colors for consecutive calls.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `string` | `string` | — | The input string to hash into a color. |
| `lightness` | `number` | `75` | The HSL lightness percentage (0–100). |
| `diff_min` | `number` | `0` | Minimum hue difference from the previous call. If the computed hue is too close to the last one, it is adjusted. |

**Returns:** `string` — An HSL color string, e.g., `"hsl(120, 75%, 75%)"`.

**Inner mechanism:**
1. Computes a hash of the string using `djb2()` and takes `hash % 360` to get a hue value.
2. If `strtocolor.hue` is set (from a previous call) and `diff_min > 0`, checks the angular difference. If too small, adjusts the hue by the deficit.
3. Stores the current hue on the function object itself (`strtocolor.hue`) for stateful tracking across calls.
4. Returns an HSL string with the computed hue and the given lightness for both saturation and lightness.

**Usage context:** Assigning consistent, visually distinct colors to tags, categories, or user avatars based on their names.

```javascript
strtocolor("Alice");   // "hsl(230, 75%, 75%)"
strtocolor("Bob");     // "hsl(45, 75%, 75%)"
strtocolor("Charlie", 80, 30); // Ensures at least 30° hue difference from previous
```

## Textcontrol

### textcontrol

Creates a rich-text formatting toolbar (a set of clickable buttons) and inserts it into the DOM immediately before the currently executing `<script>` element.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `string` | — | A CSS selector string identifying the target textarea or contenteditable element. |
| `image_path` | `string` | — | Base path for button SVG images. The function appends `"textcontrol/"` to it. |
| `extension` | `string` | `""` | Optional HTML string to append inside the toolbar div (e.g., custom buttons). |

**Returns:** `void` — The toolbar is inserted into the DOM.

**Inner mechanism:**
1. Creates a `<div class="textcontrol">` container.
2. Escapes the `object` selector using `addslashes()` for safe embedding in JavaScript URLs.
3. Defines an array of button definitions: each entry is `[label, image_name, title]`.
4. For each button, creates an `<a>` element with an `href` of `javascript:textcontrol_set(...)` and an `<img>` child pointing to the SVG button image.
5. If `extension` is non-empty, appends it as HTML.
6. Appends a "remove formatting" button.
7. Inserts the entire div before the current script element.

**Usage context:** Adding a formatting toolbar to a textarea or contenteditable element in a CMS editor or comment form.

```html
<script>
textcontrol("#editor", "/assets/", '<button onclick="customFn()">Custom</button>');
</script>
<textarea id="editor"></textarea>
```

---

### textcontrol_set

Applies a formatting tag to the selected text within a textarea or contenteditable element. This is the core function invoked by all toolbar buttons.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `string` | — | A CSS selector for the target element. |
| `format` | `string` | — | The format identifier (e.g., `"+"`, `"/"`, `"#image_left"`, `"#link"`, `"#remove"`). |
| `data` | `string` | `""` | Additional data for the format (e.g., a URL for links, or image metadata). |

**Returns:** `void` — Modifies the target element's content/value and selection.

**Inner mechanism:**
1. Resolves the target element via `document.querySelector(object)`.
2. If the element is disabled, returns early.
3. Determines the current selection:
   - For `<textarea>`: uses `selectionStart`/`selectionEnd`.
   - For contenteditable: uses `window.getSelection()` and `Range` APIs to compute selection boundaries.
4. Based on the `format` string, constructs a `_format` string (the markup to insert) and calculates `start`/`end` offsets for the new cursor position.
5. Handles special cases:
   - **Images** (`#image_left`, `#image_right`): wraps selected text in `[IMG ...]` or `[IMG-> ...]` tags.
   - **Links** (`#link`): creates `[url text]` or `[url]` tags.
   - **Email** (`#email`): creates `[mailto:addr text]` tags.
   - **Tables** (`#table`, `#tablerow`, `#tablecell`): inserts table markup.
   - **Tokens** (`#token`): handles `%%token%%` syntax with comma-separated parameters.
   - **Insert/Replace** (`#insert`, `#replace`, `#replace_all`): replaces or inserts text.
   - **Imbed** (`#imbed`): replaces `%text%` placeholder in template data.
   - **Remove** (`#remove`): calls `textcontrol_remove()` to strip formatting from the selection.
   - **Default**: wraps selection in `[format text]` tags.
6. Replaces the selected text with `_format` and sets the new cursor/selection range.
7. Restores scroll position for textareas and re-focuses the element.

**Usage context:** Called by toolbar button click handlers. Can also be called programmatically to apply formatting.

```javascript
// Apply bold formatting
textcontrol_set("#editor", "+", "");

// Insert a link with selected text as the label
textcontrol_set("#editor", "#link", "https://example.com");

// Remove all formatting from selection
textcontrol_set("#editor", "#remove", "");
```

---

### textcontrol_remove

Parses text containing PWNC markup tags and removes formatting from a specified selection range, returning the cleaned text and adjusted selection boundaries.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | The full text content of the target element. |
| `start` | `number` | The start offset of the selection. |
| `end` | `number` | The end offset of the selection. |

**Returns:** `array` — A 3-element array: `[cleaned_text, new_start, new_end]`.

**Inner mechanism:**
This is a complex state-machine parser that processes the text character by character through multiple modes:

1. **Mode 0 (default):** Collects whitespace, handles table delimiters (`|`), and detects:
   - `%%` — switches to token mode (mode 1).
   - `[` — switches to formatting mode (mode 2), starts collecting a marker.
   - `]` — ends formatting, pops the marker stack, and records a format range.
   - `\` — escape character handling.

2. **Mode 1 (token):** Collects characters until the closing `%%` is found.

3. **Mode 2 (formatting):** Collects the format marker until whitespace or newline is encountered, then pushes the marker onto the stack and increases the buffer depth.

The parser maintains:
- A **buffer stack** (`buffer[]`) for nested formatting levels.
- A **marker stack** (`stack[]`) for format tags.
- A **range list** (`range[]`) recording positions of each format span.
- Table nesting depth tracking.

After parsing, it scans the recorded ranges to find which ones overlap with the selection `[start, end]`:
- If the selection is completely within a single range, returns the inner text of that range.
- If the selection crosses multiple ranges, returns the combined inner text and adjusted boundaries.
- If no ranges overlap, returns the original selection text unchanged.

**Usage context:** Called internally by `textcontrol_set()` when the `#remove` format is applied. Not typically called directly.

```javascript
// Remove formatting from characters 10–25 in the text
var result = textcontrol_remove("Hello [+ bold] world [italic text]", 6, 18);
// result[0] = "bold world text"
// result[1] = 6  (adjusted start)
// result[2] = 18 (adjusted end)
```

## Location

### load_page

Opens a URL in a new browser window with a centered, fixed-size popup configuration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `string` | The URL to open. |
| `target` | `string` | (Optional) The window target name. If omitted, `url` is opened in `_blank` and `target` becomes the URL. |

**Returns:** `void` — Opens a new window. Returns early if the window could not be opened.

**Inner mechanism:**
1. If `target` is `undefined`, swaps arguments: opens `url` in `_blank`.
2. Parses the URL to determine if it's external (different host from `location.host`).
3. Calculates window dimensions (900×600, clamped to screen size) and center position.
4. Calls `window.open()` with a feature string specifying size, position, and UI options.
5. For external URLs, adds `noopener,noreferrer` for security.
6. Sets `first_load = true` on the new window object.
7. Listens for the `load` event and calls `fx_adjust_window()` (defined elsewhere) on first load.

**Usage context:** Opening preview windows, print dialogs, or external links in controlled popups.

```javascript
// Open in a new blank popup
load_page("https://example.com/preview");

// Open in a named window
load_page("https://example.com/print", "print_window");
```

## Form

### limit

Enforces a maximum character length on a form input or textarea, preserving the cursor position.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLInputElement|HTMLTextAreaElement` | The form element to limit. |
| `limit` | `number` | The maximum allowed character count. |

**Returns:** `void` — Truncates the element's value if it exceeds the limit.

**Inner mechanism:**
1. If the current value length is within the limit, returns immediately.
2. Records the current cursor position (`selectionStart`/`selectionEnd`).
3. Truncates the value to `limit` characters using `slice(0, limit)`.
4. Restores the cursor position (clamped to the new length).

**Usage context:** Real-time input validation on text fields with a maximum length, such as title fields or comment boxes.

```html
<textarea id="comment" oninput="limit(this, 500)"></textarea>
```

## Cookie

### getcookie

Reads the value of a named cookie from `document.cookie`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `string` | The cookie name to look up. |

**Returns:** `string` — The decoded cookie value, or an empty string if not found.

**Inner mechanism:** Constructs a regex `(?:^|; *)name=([^;]*)` to match the cookie in the cookie string, then `decodeURIComponent()`s the captured value.

**Usage context:** Retrieving user preferences, session tokens, or any stored cookie value.

```javascript
var theme = getcookie("theme"); // "dark" or ""
```

---

### setcookie

Sets a cookie with the given name, value, and optional expiration, with security best practices.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `string` | — | The cookie name. |
| `value` | `string` | — | The cookie value (will be URL-encoded). |
| `expires` | `Date\|string\|null` | `null` | Expiration: a `Date` object, a date string, or `null` for a session cookie. |

**Returns:** `void` — Writes to `document.cookie`.

**Inner mechanism:**
1. Constructs the cookie string: `name=value; path=/; samesite=Strict`.
2. If the protocol is HTTPS, appends `; secure`.
3. If `expires` is a `Date`, appends `; expires=` + UTC string.
4. If `expires` is a truthy non-Date value, appends it directly as the expires string.
5. Assigns the full cookie string to `document.cookie`.

**Usage context:** Storing user preferences, session identifiers, or any client-side state.

```javascript
// Set a session cookie
setcookie("theme", "dark");

// Set a cookie that expires in 30 days
setcookie("last_visit", "2024-01-15", new Date(Date.now() + 30 * 86400000));
```

---

### delcookie

Deletes a cookie by setting its expiration to a past date.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `string` | The cookie name to delete. |

**Returns:** `void` — Overwrites the cookie with an expired timestamp.

**Inner mechanism:** Sets `document.cookie` with the same name, an empty value, `path=/`, `samesite=Strict`, and an expiration date of January 1, 1970.

**Usage context:** Logging out users, clearing preferences, or resetting state.

```javascript
delcookie("session_token");
```

## Clipboard

### clipboard_write

Writes text to the system clipboard, using the modern Clipboard API when available and falling back to `document.execCommand("copy")` for older browsers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | The text to copy to the clipboard. |

**Returns:** `boolean` — `true` if the Clipboard API was used (async, no guarantee of success); `true`/`false` from `execCommand` in the fallback path.

**Inner mechanism:**
1. If `window.isSecureContext` is `true` and `navigator.clipboard` exists, calls `navigator.clipboard.writeText(text)` and returns `true` immediately (the operation is asynchronous).
2. **Fallback:** Creates a hidden `<textarea>`, sets its value, selects the text, calls `document.execCommand("copy")`, removes the textarea, and returns the result.

**Usage context:** "Copy to clipboard" buttons for code snippets, share links, or configuration values.

```javascript
// Copy a code snippet
clipboard_write("SELECT * FROM users WHERE id = 1;");
```

## Various

### str2utf8bin

Converts a string to its UTF-8 binary representation as a JavaScript string of raw byte characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The input string to convert. |

**Returns:** `string` — A string where each character represents a single UTF-8 byte (code points 0–255).

**Inner mechanism:** Uses `TextEncoder` to encode the string to UTF-8 bytes, then `String.fromCodePoint()` to convert each byte value into a character.

**Usage context:** Preparing strings for binary operations, hashing, or encoding to binary formats.

```javascript
str2utf8bin("Hello"); // "Hello" (ASCII is unchanged)
str2utf8bin("café");  // "cafÃ©" (UTF-8 bytes as characters)
```

---

### djb2

Computes the djb2 hash of a string, a simple non-cryptographic hash function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The input string to hash. |

**Returns:** `number` — A 32-bit unsigned integer hash value.

**Inner mechanism:**
1. Encodes the string to UTF-8 bytes using `TextEncoder`.
2. Initializes `hash = 5381`.
3. For each byte: `hash = ((hash << 5) + hash + byte) & 0xFFFFFFFF` (equivalent to `hash * 33 + byte`).
4. Returns the final hash as a 32-bit unsigned integer.

**Usage context:** Used by `strtocolor()` to generate deterministic color values. Also useful for hash tables, bloom filters, or any non-cryptographic hashing need.

```javascript
djb2("hello"); // 0x45d6c7e5 (some 32-bit integer)
djb2("world"); // different value
```

---

### crc32

Computes the CRC32 checksum of a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `string` | `string` | The input string to checksum. |

**Returns:** `number` — A 32-bit unsigned integer CRC32 checksum.

**Inner mechanism:**
1. Encodes the string to UTF-8 bytes using `TextEncoder`.
2. Initializes `crc = 0xFFFFFFFF`.
3. For each byte: XORs it into the CRC, then performs 8 rounds of bit-shifting and conditional XOR with the polynomial `0xEDB88320`.
4. Returns `(crc ^ 0xFFFFFFFF) >>> 0` (final XOR and unsigned conversion).

**Usage context:** Data integrity checks, generating deterministic identifiers, or comparing string equality with a compact hash.

```javascript
crc32("hello"); // 0x3610a686 (some 32-bit integer)
crc32("world"); // different value
```

---

### load_script

Dynamically loads and executes a JavaScript file, preventing duplicate loads.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `string` | The URL of the JavaScript file to load. |
| `onload` | `function` | (Optional) Callback function executed when the script finishes loading. |

**Returns:** `void` — Appends a `<script>` element to `<head>`.

**Inner mechanism:**
1. Checks if a `<script>` with the same `src` already exists in the document. If so, returns early.
2. Creates a new `<script>` element.
3. If `onload` is a function, attaches it as a load event listener via `fx_event_listen()`.
4. Sets `src` and `async = true`.
5. Appends the script to `document.head`, triggering the download and execution.

**Usage context:** Lazy-loading modules, loading third-party libraries on demand, or conditional script loading.

```javascript
load_script("/assets/analytics.js", function() {
    console.log("Analytics loaded");
});
```

---

### load_css

Dynamically loads a CSS stylesheet with a preload-then-apply pattern for non-blocking loading.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `string` | The URL of the CSS file to load. |

**Returns:** `void` — Appends a `<link>` element to `<head>`.

**Inner mechanism:**
1. Checks if a `<link>` with the same `href` already exists. If so, returns early.
2. Creates a `<link>` element with:
   - `rel = "preload"` (initially)
   - `as = "style"`
   - `href = url`
3. On `onload`, switches `rel` to `"stylesheet"` to apply the styles, and nulls out the `onload` handler to prevent double-firing.
4. Appends the link to `document.head`.

**Usage context:** Loading theme stylesheets, component CSS, or any non-critical styles without blocking page rendering.

```javascript
load_css("/assets/theme-dark.css");
```

---

### document_write

Inserts HTML content into the DOM immediately before the currently executing `<script>` element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `output` | `string` | The HTML string to insert. |

**Returns:** `void` — Inserts HTML via `insertAdjacentHTML`.

**Inner mechanism:**
1. Gets a reference to `document.currentScript` (the script element currently being executed).
2. If a script element exists, calls `script.insertAdjacentHTML("beforebegin", output)` to insert the HTML just before it.

**Usage context:** Server-side rendered pages where inline scripts need to inject HTML content at their position in the document flow.

```html
<script>
document_write("<div class='notice'>Welcome!</div>");
</script>
<!-- The div appears here, before the script -->
```


<!-- HASH:cfb3fc3df51ff9da370050766eafda66 -->
