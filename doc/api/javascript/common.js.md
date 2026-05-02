# NUOS API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/nuos/blob/main/nuos/javascript/common.js)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## JavaScript Utility Functions for NUOS Web Platform

This file (`common.js`) provides core JavaScript utility functions for the NUOS web platform, covering string manipulation, text formatting controls, navigation, form handling, cookie management, and dynamic resource loading. These functions are designed to be lightweight, dependency-free, and reusable across the platform.

---

## **STRING**

Functions for string manipulation and generation.

---

### **string_repeat(string, count)**
Repeats a given string a specified number of times.

#### **Parameters**
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| string  | string | The string to repeat.                |
| count   | number | The number of times to repeat.       |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The repeated string.                 |

#### **Inner Mechanisms**
- Uses the native `String.prototype.repeat()` method.

#### **Usage**
- Generating padding, separators, or repeated patterns in dynamic content.

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

#### **Inner Mechanisms**
- Replaces `&`, `"`, `'`, `<`, and `>` with `&amp;`, `&quot;`, `&apos;`, `&lt;`, and `&gt;` respectively.
- Uses a regex-based replacement with a lookup object.

#### **Usage**
- Preventing XSS attacks when inserting user-generated content into HTML.
- Safely rendering dynamic content in templates.

---

### **unique_id(count)**
Generates a random alphanumeric string of a given length.

#### **Parameters**
| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| count | number | Length of the generated string.      |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | A random alphanumeric string.        |

#### **Inner Mechanisms**
- Uses a character set of 62 characters (0-9, A-Z, a-z).
- Selects characters randomly using `Math.random()`.

#### **Usage**
- Generating unique IDs for DOM elements, temporary keys, or nonces.

---

### **strabridge(string, length = 40, cut_end = false)**
Truncates a string to a specified length, adding an ellipsis (`…`) if truncated.

#### **Parameters**
| Name     | Type    | Default | Description                                      |
|----------|---------|---------|--------------------------------------------------|
| string   | string  | —       | The string to truncate.                          |
| length   | number  | 40      | Maximum length of the output string.             |
| cut_end  | boolean | false   | If `true`, truncates from the end; otherwise, centers the ellipsis. |

#### **Return Value**
| Type   | Description                                      |
|--------|--------------------------------------------------|
| string | The truncated string with ellipsis if applicable. |

#### **Inner Mechanisms**
- If `cut_end` is `true`, truncates from the start and appends `…`.
- Otherwise, splits the truncation: 65% from the start, 35% from the end, with `…` in the middle.

#### **Usage**
- Displaying long strings in limited UI space (e.g., previews, lists, tables).

---

### **addslashes(string)**
Escapes quotes and backslashes in a string for use in JavaScript strings.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The string to escape.                |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The escaped string.                  |

#### **Inner Mechanisms**
- Uses `JSON.stringify()` to escape quotes and backslashes, then slices off the surrounding quotes.

#### **Usage**
- Safely embedding strings in JavaScript code or inline event handlers.

---

---

## **TEXTCONTROL**

Functions for creating and manipulating a rich text formatting toolbar and applying formatting to text inputs.

---

### **textcontrol(object, image_path, extension = "")**
Creates a formatting toolbar for a text input or textarea.

#### **Parameters**
| Name        | Type   | Description                                      |
|-------------|--------|--------------------------------------------------|
| object      | string | CSS selector for the target input/textarea.      |
| image_path  | string | Base path to the toolbar icon images.            |
| extension   | string | Optional HTML string to append to the toolbar.   |

#### **Return Value**
None (DOM manipulation).

#### **Inner Mechanisms**
- Creates a `<div class="textcontrol">` and populates it with buttons for formatting (bold, italic, etc.).
- Each button triggers `textcontrol_set()` with the appropriate format.
- Inserts the toolbar before the current `<script>` tag.

#### **Usage**
- Enabling rich text formatting in user-editable content areas (e.g., comments, messages, CMS content).

---

### **textcontrol_set(object, format, data = "")**
Applies formatting to the selected text in a target input/textarea.

#### **Parameters**
| Name    | Type   | Description                                      |
|---------|--------|--------------------------------------------------|
| object  | string | CSS selector for the target input/textarea.      |
| format  | string | Formatting command (e.g., `+`, `#image_left`).   |
| data    | string | Optional data for the format (e.g., URL, email). |

#### **Return Value**
None (modifies the target input).

#### **Inner Mechanisms**
- Handles both `<textarea>` and `contenteditable` elements.
- Retrieves the current selection and applies the appropriate formatting syntax (e.g., `[bold text]`).
- Supports special formats: images, links, emails, tables, and tokens.
- Preserves cursor position and scroll state.

#### **Usage**
- Applying formatting from the `textcontrol` toolbar.
- Programmatic formatting of user input.

---

### **textcontrol_remove(text, start, end)**
Removes formatting from a selected range in formatted text.

#### **Parameters**
| Name  | Type   | Description                                      |
|-------|--------|--------------------------------------------------|
| text  | string | The formatted text.                              |
| start | number | Start index of the selection.                    |
| end   | number | End index of the selection.                      |

#### **Return Value**
| Type  | Description                                      |
|-------|--------------------------------------------------|
| array | `[cleaned_text, new_start, new_end]` — the cleaned text and adjusted selection indices. |

#### **Inner Mechanisms**
- Parses the text to identify and remove formatting markers (e.g., `[bold]`, `[IMG]`).
- Handles nested formatting, tables, and tokens.
- Returns the cleaned text and adjusted selection indices.

#### **Usage**
- Removing formatting from a selected range in `textcontrol_set()`.

---

---

## **LOCATION**

Functions for page navigation and window management.

---

### **load_page(url, target)**
Opens a URL in a new window or tab, centered on the screen.

#### **Parameters**
| Name   | Type   | Description                                      |
|--------|--------|--------------------------------------------------|
| url    | string | The URL to open.                                 |
| target | string | Optional. Target window name or `_blank`.        |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Detects external URLs (different host) and adds `noopener,noreferrer`.
- Centers the new window on the screen.
- Listens for the `load` event to adjust window size if needed.

#### **Usage**
- Opening external or internal links in a controlled popup window.

---

---

## **FORM**

Functions for form input control.

---

### **limit(object, limit)**
Enforces a maximum character limit on a text input or textarea.

#### **Parameters**
| Name   | Type          | Description                                      |
|--------|---------------|--------------------------------------------------|
| object | HTMLInputElement or HTMLTextAreaElement | The input element. |
| limit  | number        | Maximum allowed characters.                      |

#### **Return Value**
None (modifies the input value).

#### **Inner Mechanisms**
- Truncates the value if it exceeds the limit.
- Preserves the current selection.

#### **Usage**
- Enforcing character limits in forms (e.g., comments, titles).

---

---

## **COOKIE**

Functions for managing browser cookies.

---

### **getcookie(name)**
Retrieves the value of a cookie by name.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The name of the cookie.              |

#### **Return Value**
| Type   | Description                          |
|--------|--------------------------------------|
| string | The cookie value, or empty string.   |

#### **Inner Mechanisms**
- Uses regex to parse `document.cookie`.

#### **Usage**
- Reading user preferences or session data.

---

### **setcookie(name, value, expires = null)**
Sets a cookie with a given name, value, and optional expiration.

#### **Parameters**
| Name     | Type            | Description                                      |
|----------|-----------------|--------------------------------------------------|
| name     | string          | The cookie name.                                 |
| value    | string          | The cookie value.                                |
| expires  | Date or boolean | Optional. Expiration date or `true` for session. |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Sets `path=/` and `samesite=Strict`.
- Adds `secure` flag if on HTTPS.
- Handles expiration in UTC string format.

#### **Usage**
- Storing user preferences, session tokens, or tracking data.

---

### **delcookie(name)**
Deletes a cookie by name.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The name of the cookie to delete.    |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Sets the cookie with an expired date.

#### **Usage**
- Logging out users or clearing session data.

---

---

## **VARIOUS**

Miscellaneous utility functions.

---

### **load_script(url, onload)**
Dynamically loads a JavaScript file.

#### **Parameters**
| Name   | Type     | Description                                      |
|--------|----------|--------------------------------------------------|
| url    | string   | The script URL.                                  |
| onload | function | Optional. Callback when the script loads.        |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Checks if the script is already loaded.
- Appends a `<script>` tag to `<head>` with `async=true`.
- Attaches the `onload` callback if provided.

#### **Usage**
- Lazy-loading third-party libraries or modules.

---

### **load_css(url)**
Dynamically loads a CSS file.

#### **Parameters**
| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| url  | string | The stylesheet URL.                  |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Checks if the stylesheet is already loaded.
- Uses `<link rel="preload">` and switches to `stylesheet` on load.

#### **Usage**
- Loading theme or component-specific styles on demand.

---

### **document_write(output)**
Inserts HTML content before the current `<script>` tag.

#### **Parameters**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| output | string | The HTML string to insert.           |

#### **Return Value**
None.

#### **Inner Mechanisms**
- Uses `insertAdjacentHTML("beforebegin")` on the current script.

#### **Usage**
- Inline dynamic content insertion during page load.


<!-- HASH:445b03608a7695f23c1ea9f68f7f792a -->
