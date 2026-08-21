# PWNC API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/common.js)

- **Version:** `26.6.19.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## JavaScript Utility Functions for PWNC Web Platform

This file (`common.js`) provides core utility functions for the PWNC Web Platform, covering string manipulation, text formatting controls, page navigation, form handling, cookie management, and various encoding/decoding operations. These functions are designed to be lightweight, dependency-free, and reusable across the platform.

---

## **STRING**

Functions for string manipulation, encoding, and generation.

---

### **string_repeat(string, count)**
Repeats a given string a specified number of times.

#### **Parameters**
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| string  | string | The string to repeat.                |
| count   | number | Number of times to repeat the string.|

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The repeated string.                 |

#### **Inner Mechanisms**
- Uses the native `String.prototype.repeat()` method.

#### **Usage Example**
```javascript
const repeated = string_repeat("PWNC ", 3);
// Result: "PWNC PWNC PWNC "
```
**Context:** Useful for generating padding, separators, or repeated patterns in UI elements.

---

### **htmlspecialchars(string)**
Escapes special HTML characters to their corresponding HTML entities.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The string to escape.                |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The escaped string.                  |

#### **Mapped Characters**
| Character | Entity   |
|-----------|----------|
| `"`       | `&quot;` |
| `&`       | `&amp;`  |
| `'`       | `&apos;` |
| `<`       | `&lt;`   |
| `>`       | `&gt;`   |

#### **Inner Mechanisms**
- Uses a regular expression to match special characters and replaces them using a lookup object.

#### **Usage Example**
```javascript
const safe = htmlspecialchars('<script>alert("XSS")</script>');
// Result: "&lt;script&gt;alert(&quot;XSS&quot;)&lt;/script&gt;"
```
**Context:** Essential for preventing XSS attacks when rendering user-generated content.

---

### **unique_id(count)**
Generates a random alphanumeric string of a specified length.

#### **Parameters**
| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| count | number | -       | Length of the generated ID.          |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | A random alphanumeric string.        |

#### **Inner Mechanisms**
- Uses a character set of 62 alphanumeric characters (0-9, A-Z, a-z).
- Randomly selects characters using `Math.random()`.

#### **Usage Example**
```javascript
const id = unique_id(10);
// Example Result: "aB3x9YpQ2z"
```
**Context:** Useful for generating unique IDs for DOM elements, temporary files, or session tokens.

---

### **strabridge(string, length = 50, cut_end = false)**
Truncates a string to a specified length, adding an ellipsis (`…`) if truncated. Supports smart truncation (middle or end).

#### **Parameters**
| Name     | Type    | Default | Description                          |
|----------|---------|---------|--------------------------------------|
| string   | string  | -       | The string to truncate.              |
| length   | number  | 50      | Maximum length of the output string. |
| cut_end  | boolean | false   | If `true`, truncates from the end.   |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The truncated string.                |

#### **Inner Mechanisms**
- If `cut_end` is `true`, truncates from the end.
- If `cut_end` is `false`, truncates from the middle, preserving the start (65%) and end (35%) of the string.

#### **Usage Example**
```javascript
const longText = "This is a very long string that needs to be truncated.";
const truncated = strabridge(longText, 20);
// Result: "This is a … truncated."
```
**Context:** Ideal for displaying long strings in UIs (e.g., filenames, descriptions) where space is limited.

---

### **addslashes(string)**
Escapes quotes and backslashes in a string to make it safe for JavaScript string literals.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The string to escape.                |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The escaped string.                  |

#### **Inner Mechanisms**
- Uses `JSON.stringify()` to escape special characters, then removes the surrounding quotes.

#### **Usage Example**
```javascript
const escaped = addslashes('He said, "Hello!"');
// Result: "He said, \\"Hello!\\""
```
**Context:** Useful for safely embedding strings in JavaScript code or HTML attributes.

---

### **strtocolor(string, lightness = 75, diff_min = 0)**
Generates a deterministic HSL color from a string, ensuring adjacent colors are visually distinct.

#### **Parameters**
| Name      | Type   | Default | Description                          |
|-----------|--------|---------|--------------------------------------|
| string    | string | -       | The input string.                    |
| lightness | number | 75      | Lightness percentage (0-100).        |
| diff_min  | number | 0       | Minimum hue difference from previous.|

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | An HSL color string.                 |

#### **Inner Mechanisms**
- Uses the `djb2` hash function to generate a hue value from the string.
- Adjusts the hue to ensure a minimum difference from the last generated color (stored in `strtocolor.hue`).
- Returns an HSL string with the calculated hue and specified lightness.

#### **Usage Example**
```javascript
const color1 = strtocolor("user1");
// Result: "hsl(123, 75%, 75%)"
const color2 = strtocolor("user2", 50, 30);
// Ensures color2 is at least 30° different from color1.
```
**Context:** Useful for generating consistent, visually distinct colors for tags, avatars, or categories.

---

## **TEXTCONTROL**

Functions for creating and managing a rich text formatting toolbar.

---

### **textcontrol(object, image_path, extension = "")**
Creates a rich text formatting toolbar for a text input or textarea.

#### **Parameters**
| Name        | Type   | Default | Description                          |
|-------------|--------|---------|--------------------------------------|
| object      | string | -       | CSS selector for the target element. |
| image_path  | string | -       | Path to the toolbar icon images.     |
| extension   | string | ""      | Additional HTML to append to the toolbar. |

#### **Return Value**
None. The toolbar is inserted before the current `<script>` tag.

#### **Toolbar Buttons**
| Button       | Format       | Description               |
|--------------|--------------|---------------------------|
| `+`          | bold         | Bold text.                |
| `/`          | italic       | Italic text.              |
| `_`          | underline    | Underlined text.          |
| `<`          | big          | Large text.               |
| `>`          | small        | Small text.               |
| `<-`         | left         | Left-aligned text.        |
| `<->`        | center       | Center-aligned text.      |
| `->`         | right        | Right-aligned text.       |
| `#image_left`| image_left   | Left-aligned image.       |
| `#image_right`| image_right | Right-aligned image.      |
| `#link`      | link         | Hyperlink.                |
| `#email`     | email        | Email link.               |
| `#remove`    | remove       | Remove formatting.        |

#### **Inner Mechanisms**
- Creates a `<div>` with class `textcontrol`.
- Dynamically generates buttons for each formatting option.
- Each button triggers `textcontrol_set()` with the appropriate format.

#### **Usage Example**
```javascript
// In HTML:
// <textarea id="content"></textarea>
// <script>textcontrol("#content", "/images/");</script>
```
**Context:** Provides a lightweight, customizable rich text editor for user inputs (e.g., comments, articles).

---

### **textcontrol_set(object, format, data = "")**
Applies or removes formatting to the selected text in a text input or textarea.

#### **Parameters**
| Name   | Type   | Default | Description                          |
|--------|--------|---------|--------------------------------------|
| object | string | -       | CSS selector for the target element. |
| format | string | -       | The format to apply (e.g., `bold`).  |
| data   | string | ""      | Additional data (e.g., URL for links). |

#### **Return Value**
None. Modifies the target element's value or selection.

#### **Supported Formats**
| Format          | Description                          | Data Usage                     |
|-----------------|--------------------------------------|--------------------------------|
| `bold`          | Bold text.                           | -                              |
| `italic`        | Italic text.                         | -                              |
| `underline`     | Underlined text.                     | -                              |
| `big`           | Large text.                          | -                              |
| `small`         | Small text.                          | -                              |
| `left`          | Left-aligned text.                   | -                              |
| `center`        | Center-aligned text.                 | -                              |
| `right`         | Right-aligned text.                  | -                              |
| `#image_left`   | Left-aligned image.                  | Image URL.                     |
| `#image_right`  | Right-aligned image.                 | Image URL.                     |
| `#link`         | Hyperlink.                           | URL.                           |
| `#email`        | Email link.                          | Email address.                 |
| `#remove`       | Remove formatting.                   | -                              |

#### **Inner Mechanisms**
- Handles both `<textarea>` and `contenteditable` elements.
- Preserves cursor position and selection after formatting.
- Uses a custom markup syntax (e.g., `[bold text]`) for formatting.

#### **Usage Example**
```javascript
// Apply bold formatting to selected text in a textarea.
textcontrol_set("#content", "bold");
```
**Context:** Core function for the `textcontrol` toolbar, enabling rich text editing.

---

### **textcontrol_remove(text, start, end)**
Removes formatting from a selected range of text while preserving the raw content.

#### **Parameters**
| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| text  | string | The input text with formatting.      |
| start | number | Start index of the selection.        |
| end   | number | End index of the selection.          |

#### **Return Value**
| Type  | Description                          |
|-------|--------------------------------------|
| array | `[cleaned_text, new_start, new_end]` |

#### **Inner Mechanisms**
- Parses the text to identify formatting markers (e.g., `[bold]`).
- Removes formatting while preserving the raw text.
- Adjusts the selection range to account for removed markers.

#### **Usage Example**
```javascript
const [cleaned, start, end] = textcontrol_remove("[bold Hello] world", 0, 12);
// Result: ["Hello world", 0, 5]
```
**Context:** Used by `textcontrol_set()` to remove formatting when the `#remove` button is clicked.

---

## **LOCATION**

Functions for page navigation and window management.

---

### **load_page(url, target)**
Opens a URL in a new window or tab, centered on the screen.

#### **Parameters**
| Name   | Type   | Default   | Description                          |
|--------|--------|-----------|--------------------------------------|
| url    | string | -         | The URL to open.                     |
| target | string | `_blank`  | The target window name.              |

#### **Return Value**
None. Opens a new window.

#### **Inner Mechanisms**
- Detects external URLs (different host) and adds `noopener` and `noreferrer` for security.
- Centers the new window on the screen.
- Adjusts window size to fit the screen.

#### **Usage Example**
```javascript
load_page("https://pwnc.it", "PWNC_Window");
```
**Context:** Useful for opening external links or pop-up windows (e.g., login forms, help pages).

---

## **FORM**

Functions for form input handling.

---

### **limit(object, limit)**
Limits the length of a text input or textarea to a specified maximum.

#### **Parameters**
| Name  | Type          | Description                          |
|-------|---------------|--------------------------------------|
| object| HTMLInputElement | The input or textarea element.   |
| limit | number        | Maximum allowed length.              |

#### **Return Value**
None. Modifies the element's value if it exceeds the limit.

#### **Inner Mechanisms**
- Truncates the value if it exceeds the limit.
- Preserves the cursor position.

#### **Usage Example**
```javascript
// In HTML:
// <input type="text" id="username" oninput="limit(this, 20)">
```
**Context:** Enforces input length limits (e.g., usernames, titles) without requiring server-side validation.

---

## **COOKIE**

Functions for managing browser cookies.

---

### **getcookie(name)**
Retrieves the value of a cookie by name.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The cookie name.                     |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The cookie value, or empty string.   |

#### **Inner Mechanisms**
- Uses a regular expression to parse `document.cookie`.

#### **Usage Example**
```javascript
const theme = getcookie("theme");
// Result: "dark" (or "" if not set)
```
**Context:** Retrieves user preferences or session data stored in cookies.

---

### **setcookie(name, value, expires = null)**
Sets a cookie with a name, value, and optional expiration.

#### **Parameters**
| Name    | Type            | Default | Description                          |
|---------|-----------------|---------|--------------------------------------|
| name    | string          | -       | The cookie name.                     |
| value   | string          | -       | The cookie value.                    |
| expires | Date or boolean | null    | Expiration date or `true` for session cookie. |

#### **Return Value**
None. Sets the cookie via `document.cookie`.

#### **Inner Mechanisms**
- Encodes the value using `encodeURIComponent`.
- Sets `path=/` and `samesite=Strict` for security.
- Adds `secure` flag if the page is served over HTTPS.

#### **Usage Example**
```javascript
setcookie("theme", "dark", new Date(Date.now() + 86400e3)); // Expires in 1 day
```
**Context:** Stores user preferences or session data.

---

### **delcookie(name)**
Deletes a cookie by name.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The cookie name.                     |

#### **Return Value**
None. Sets the cookie's expiration to a past date.

#### **Usage Example**
```javascript
delcookie("theme");
```
**Context:** Removes cookies (e.g., on logout).

---

## **VARIOUS**

Miscellaneous utility functions.

---

### **str2utf8bin(string)**
Converts a string to its UTF-8 binary representation.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string.                    |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The UTF-8 binary string.             |

#### **Inner Mechanisms**
- Uses `TextEncoder` to convert the string to a UTF-8 byte array.
- Converts each byte to its corresponding Unicode code point.

#### **Usage Example**
```javascript
const binary = str2utf8bin("PWNC");
// Result: "PWNC" (as UTF-8 bytes)
```
**Context:** Useful for binary data processing or encoding.

---

### **djb2(string)**
Generates a 32-bit hash of a string using the DJB2 algorithm.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string.                    |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| number | The 32-bit hash value.               |

#### **Inner Mechanisms**
- Converts the string to a UTF-8 byte array.
- Applies the DJB2 hashing algorithm.

#### **Usage Example**
```javascript
const hash = djb2("PWNC");
// Result: 210713324 (example)
```
**Context:** Used by `strtocolor()` for deterministic color generation.

---

### **crc32(string)**
Generates a 32-bit CRC hash of a string.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string.                    |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| number | The 32-bit CRC value.                |

#### **Inner Mechanisms**
- Converts the string to a UTF-8 byte array.
- Applies the CRC32 algorithm.

#### **Usage Example**
```javascript
const crc = crc32("PWNC");
// Result: 3058687904 (example)
```
**Context:** Useful for checksums or data integrity checks.

---

### **load_script(url, onload)**
Dynamically loads a JavaScript file.

#### **Parameters**
| Name   | Type     | Default | Description                          |
|--------|----------|---------|--------------------------------------|
| url    | string   | -       | The script URL.                      |
| onload | function | -       | Callback function on load.           |

#### **Return Value**
None. Appends a `<script>` tag to the document head.

#### **Inner Mechanisms**
- Checks if the script is already loaded.
- Sets `async=true` for non-blocking loading.

#### **Usage Example**
```javascript
load_script("/js/module.js", () => console.log("Loaded!"));
```
**Context:** Loads external scripts on demand (e.g., for modular applications).

---

### **load_css(url)**
Dynamically loads a CSS file.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| url  | string | The CSS file URL.                    |

#### **Return Value**
None. Appends a `<link>` tag to the document head.

#### **Inner Mechanisms**
- Checks if the CSS is already loaded.
- Uses `preload` for performance, then switches to `stylesheet`.

#### **Usage Example**
```javascript
load_css("/css/theme.css");
```
**Context:** Loads stylesheets dynamically (e.g., for theme switching).

---

### **document_write(output)**
Inserts HTML content before the current `<script>` tag.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| output | string | The HTML content to insert.          |

#### **Return Value**
None. Inserts HTML using `insertAdjacentHTML`.

#### **Usage Example**
```javascript
document_write("<div>Hello, PWNC!</div>");
```
**Context:** Replaces `document.write()` for modern, non-blocking DOM manipulation.


<!-- HASH:454a495f4fd2f18026e7312a40ce3d47 -->
