# NUOS API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## JavaScript Utility Functions for NUOS Web Platform

This file (`common.js`) provides core utility functions for the NUOS web platform, covering string manipulation, text formatting controls, page navigation, form handling, cookie management, and dynamic resource loading. These functions are designed to be lightweight, dependency-free, and reusable across the platform.

---

## **STRING**

### **string_repeat**
Repeats a given string a specified number of times.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| string    | string | The string to repeat.           |
| count     | number | Number of times to repeat.      |

**Return Value:**
- `string`: The repeated string.

**Inner Mechanisms:**
- Uses the native `String.prototype.repeat()` method.

**Usage:**
```javascript
string_repeat("abc", 3); // "abcabcabc"
```

---

### **htmlspecialchars**
Converts special characters to their corresponding HTML entities to prevent XSS attacks.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| string    | string | The input string to escape.     |

**Return Value:**
- `string`: The escaped string.

**Inner Mechanisms:**
- Replaces `&`, `"`, `'`, `<`, and `>` with their HTML entity equivalents using a regex-based lookup.

**Usage:**
```javascript
htmlspecialchars("<script>alert('XSS')</script>");
// "&lt;script&gt;alert(&apos;XSS&apos;)&lt;/script&gt;"
```

---

### **unique_id**
Generates a random alphanumeric string of a specified length.

| Parameter | Type   | Default | Description                     |
|-----------|--------|---------|---------------------------------|
| count     | number |         | Length of the generated string. |

**Return Value:**
- `string`: A random alphanumeric string.

**Inner Mechanisms:**
- Selects random characters from a predefined set (`0-9A-Za-z`) using `Math.random()`.

**Usage:**
```javascript
unique_id(10); // e.g., "aB3x9FgH2Y"
```

---

### **strabridge**
Truncates a string to a specified length, adding an ellipsis (`…`) if necessary. Supports truncation from the start, middle, or end.

| Parameter | Type    | Default | Description                                                                 |
|-----------|---------|---------|-----------------------------------------------------------------------------|
| string    | string  |         | The input string.                                                           |
| length    | number  | 40      | Maximum length of the output string.                                        |
| cut_end   | boolean | false   | If `true`, truncates from the end; otherwise, truncates from the middle.    |

**Return Value:**
- `string`: The truncated string with an ellipsis if needed.

**Inner Mechanisms:**
- If `cut_end` is `true`, truncates from the end.
- Otherwise, splits the string into two parts (65% from the start, 35% from the end) and joins them with an ellipsis.

**Usage:**
```javascript
strabridge("This is a very long string that needs truncation", 20);
// "This is a … truncation"
strabridge("This is a very long string that needs truncation", 20, true);
// "This is a very long …"
```

---

### **addslashes**
Escapes quotes and backslashes in a string to make it safe for use in JavaScript strings.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| string    | string | The input string to escape.     |

**Return Value:**
- `string`: The escaped string.

**Inner Mechanisms:**
- Uses `JSON.stringify()` to escape quotes and backslashes, then removes the surrounding quotes.

**Usage:**
```javascript
addslashes('He said, "Hello!"');
// He said, \"Hello!\"
```

---

## **TEXTCONTROL**

### **textcontrol**
Creates a toolbar for rich text formatting (e.g., bold, italic, links, images) and inserts it into the DOM.

| Parameter   | Type          | Default | Description                                                                 |
|-------------|---------------|---------|-----------------------------------------------------------------------------|
| object      | string        |         | CSS selector for the target input/textarea element.                         |
| image_path  | string        |         | Base path to the directory containing toolbar icons.                        |
| extension   | string        | ""      | Additional HTML to append to the toolbar.                                   |

**Return Value:**
- `void`: Inserts the toolbar into the DOM before the current `<script>` tag.

**Inner Mechanisms:**
- Creates a `<div>` with class `textcontrol`.
- Dynamically generates buttons for formatting options (bold, italic, links, etc.).
- Each button triggers `textcontrol_set()` with the appropriate format.

**Usage:**
```javascript
textcontrol("#myTextarea", "/images/", "<button>Custom</button>");
```

---

### **textcontrol_set**
Applies formatting to the selected text in an input or textarea element based on the specified format.

| Parameter | Type          | Default | Description                                                                 |
|-----------|---------------|---------|-----------------------------------------------------------------------------|
| object    | string        |         | CSS selector for the target element.                                        |
| format    | string        |         | The formatting command (e.g., `+`, `#link`, `#image_left`).                 |
| data      | string        | ""      | Additional data for the format (e.g., URL for links, image paths).          |

**Return Value:**
- `void`: Modifies the target element's value and selection.

**Inner Mechanisms:**
- Handles different formatting commands:
  - **Basic formatting** (bold, italic, etc.): Wraps selected text in `[format]` tags.
  - **Images**: Inserts `[IMG URL]` or `[IMG-> URL]` for left/right alignment.
  - **Links/Email**: Inserts `[URL text]` or `[mailto:email text]`.
  - **Tables**: Inserts table-related tags.
  - **Tokens**: Handles `%%token%%` syntax.
  - **Removal**: Strips formatting from selected text using `textcontrol_remove()`.
- Preserves cursor position and selection after modification.

**Usage:**
```javascript
textcontrol_set("#myTextarea", "+"); // Applies bold formatting
textcontrol_set("#myTextarea", "#link", "https://example.com");
```

---

### **textcontrol_remove**
Removes formatting tags from a selected range of text in a formatted string.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| text      | string | The input text with formatting. |
| start     | number | Start index of the selection.   |
| end       | number | End index of the selection.     |

**Return Value:**
- `array`: A tuple containing:
  - `string`: The unformatted text.
  - `number`: New start index of the selection.
  - `number`: New end index of the selection.

**Inner Mechanisms:**
- Parses the input text to identify and remove formatting tags (e.g., `[bold]`, `[IMG]`).
- Handles nested formatting and tables.
- Returns the cleaned text and adjusted selection indices.

**Usage:**
```javascript
textcontrol_remove("[bold]Hello[/bold]", 0, 13);
// ["Hello", 0, 5]
```

---

## **LOCATION**

### **load_page**
Opens a URL in a new window or tab, centered on the screen.

| Parameter | Type   | Default | Description                                                                 |
|-----------|--------|---------|-----------------------------------------------------------------------------|
| url       | string |         | The URL to open.                                                            |
| target    | string | "_blank"| The target window name. If omitted, `url` is treated as the target.         |

**Return Value:**
- `void`: Opens the URL in a new window.

**Inner Mechanisms:**
- Detects external URLs (different host) and adds `noopener,noreferrer` for security.
- Centers the window on the screen and sets dimensions to 900x600 (or screen size if smaller).
- Listens for the `load` event to adjust the window size using `fx_adjust_window()`.

**Usage:**
```javascript
load_page("https://example.com");
load_page("https://example.com", "myWindow");
```

---

## **FORM**

### **limit**
Truncates the value of an input or textarea element to a specified length, preserving the cursor position.

| Parameter | Type          | Description                     |
|-----------|---------------|---------------------------------|
| object    | HTMLInputElement or HTMLTextAreaElement | The input element. |
| limit     | number        | Maximum allowed length.         |

**Return Value:**
- `void`: Modifies the element's value if it exceeds the limit.

**Inner Mechanisms:**
- Uses `slice()` to truncate the value.
- Restores the cursor position using `setSelectionRange()`.

**Usage:**
```javascript
limit(document.querySelector("#myInput"), 100);
```

---

## **COOKIE**

### **getcookie**
Retrieves the value of a cookie by name.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| name      | string | The name of the cookie.         |

**Return Value:**
- `string`: The decoded cookie value, or an empty string if not found.

**Inner Mechanisms:**
- Uses a regex to match the cookie name and value.
- Decodes the value using `decodeURIComponent()`.

**Usage:**
```javascript
getcookie("session_id");
```

---

### **setcookie**
Sets a cookie with a name, value, and optional expiration.

| Parameter | Type          | Default | Description                                                                 |
|-----------|---------------|---------|-----------------------------------------------------------------------------|
| name      | string        |         | The cookie name.                                                            |
| value     | string        |         | The cookie value.                                                           |
| expires   | Date or string| null    | Expiration date or duration (e.g., `"1d"`). If `null`, the cookie is session-only. |

**Return Value:**
- `void`: Sets the cookie in `document.cookie`.

**Inner Mechanisms:**
- Encodes the value using `encodeURIComponent()`.
- Sets `path=/` and `samesite=Strict` by default.
- Adds `secure` flag for HTTPS connections.

**Usage:**
```javascript
setcookie("session_id", "abc123", new Date(Date.now() + 86400000)); // Expires in 1 day
setcookie("theme", "dark", "7d");
```

---

### **delcookie**
Deletes a cookie by setting its expiration to a past date.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| name      | string | The name of the cookie to delete. |

**Return Value:**
- `void`: Removes the cookie.

**Inner Mechanisms:**
- Sets the cookie's expiration to `Thu, 01 Jan 1970 00:00:00 GMT`.

**Usage:**
```javascript
delcookie("session_id");
```

---

## **VARIOUS**

### **load_script**
Dynamically loads a JavaScript file if it hasn't already been loaded.

| Parameter | Type     | Default | Description                                                                 |
|-----------|----------|---------|-----------------------------------------------------------------------------|
| url       | string   |         | The URL of the script to load.                                              |
| onload    | function |         | Callback function to execute when the script loads.                         |

**Return Value:**
- `void`: Appends the script to the `<head>` of the document.

**Inner Mechanisms:**
- Checks if the script is already loaded using `document.querySelector()`.
- Sets `async=true` for non-blocking loading.
- Attaches the `onload` callback if provided.

**Usage:**
```javascript
load_script("/js/library.js", () => console.log("Loaded!"));
```

---

### **load_css**
Dynamically loads a CSS file if it hasn't already been loaded.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| url       | string | The URL of the CSS file.        |

**Return Value:**
- `void`: Appends the CSS link to the `<head>` of the document.

**Inner Mechanisms:**
- Uses `rel="preload"` initially to prioritize loading.
- Switches to `rel="stylesheet"` after loading to apply the styles.

**Usage:**
```javascript
load_css("/css/theme.css");
```

---

### **document_write**
Inserts HTML content into the DOM before the current `<script>` tag.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| output    | string | The HTML content to insert.     |

**Return Value:**
- `void`: Inserts the HTML into the DOM.

**Inner Mechanisms:**
- Uses `insertAdjacentHTML("beforebegin", output)` to insert content.

**Usage:**
```javascript
document_write("<div>Hello, world!</div>");
```


<!-- HASH:80048ee088058ff133f1aee5b028f9bb -->
