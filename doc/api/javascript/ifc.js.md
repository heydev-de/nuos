# PWNC API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/ifc.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC Interface Control JavaScript Library (`ifc.js`)

## Overview

The `ifc.js` file is a core component of the PWNC Web Platform's frontend infrastructure. It provides a comprehensive set of JavaScript functions designed to manage interactive form elements, handle user input, implement syntax highlighting for code editors, manage multilingual content, and support advanced features like undo/redo functionality and file upload progress tracking.

This library operates primarily on HTML forms named `ifc` and extends standard DOM elements with enhanced behaviors. It integrates with other PWNC utilities such as `fx_event_listen`, `fx_animation_frame`, `fx_scroll_container`, and `htmlspecialchars`.

---

## COMMAND

### ifc_post

Submits the main interface form (`ifc`) programmatically, optionally setting message and parameter values before submission.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `message` | `string` | `""`    | Message to set in the `ifc_message` form field |
| `param`   | `string` | `""`    | Parameter to set in the `ifc_param` form field |

**Return Value:** `void`

**Mechanism:** Sets hidden form fields, memorizes scroll position, dispatches a submit event, and submits the form if not prevented.

**Usage Example:**
```javascript
// Submit a command to the server
ifc_post("save_document", "doc_123");
```

---

### ifc_cancel

Resets the form and posts a cancel command.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `offset`  | `number` | `0`     | Starting index for resetting form elements |

**Return Value:** `void`

**Mechanism:** Calls `ifc_reset` to clear form elements, then calls `ifc_post` with `"ifc_cancel"` as the message.

**Usage Example:**
```javascript
// Cancel current operation
ifc_cancel();
```

---

### ifc_autopost

Automatically posts form data when a specified object changes.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element to monitor |
| `message` | `string`         | `""`    | Message to send on change |

**Return Value:** `void`

**Mechanism:** Converts string to object if needed, attaches a change listener that triggers `ifc_post`.

**Usage Example:**
```javascript
// Auto-submit when a dropdown changes
ifc_autopost("language_selector", "change_language");
```

---

### ifc_response

Updates the response display area with new content.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `value`   | `string` | —       | HTML content to display |

**Return Value:** `void`

**Mechanism:** Finds the `ifc-response` element, hides it briefly, updates its content, then shows it again with animation.

**Usage Example:**
```javascript
// Display server response
ifc_response("<p>Operation completed successfully</p>");
```

---

## VALUE

### ifc_get

Retrieves the value from a form element, handling different input types appropriately.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `string\|FileList\|boolean`

**Mechanism:** Handles checkboxes/radios by checking state, returns file lists for file inputs, and handles radio groups through iteration.

**Usage Example:**
```javascript
// Get value from a text input
var value = ifc_get("username");

// Get checked radio value
var choice = ifc_get("gender");
```

---

### ifc_title

Retrieves the display title/label text associated with a form element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `string`

**Mechanism:** For buttons/submits, returns text content. For other inputs, finds associated label and extracts clean text.

**Usage Example:**
```javascript
// Get label text for an input
var label = ifc_title("email");
```

---

### ifc_reset

Clears all form elements starting from a given offset.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `offset`  | `number` | `0`     | Starting index in form elements collection |

**Return Value:** `void`

**Mechanism:** Iterates through form elements from offset, calling `ifc_del` on each.

**Usage Example:**
```javascript
// Clear entire form
ifc_reset();
```

---

### ifc_del

Deletes/clears the value of a specific form element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `void`

**Mechanism:** Handles different input types appropriately (unchecking radios/checkboxes, clearing values, resetting selections), then focuses the element.

**Usage Example:**
```javascript
// Clear a specific input
ifc_del("search_query");
```

---

### ifc_set

Sets the value of a form element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `value`   | `string`         | `""`    | Value to set |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `void`

**Mechanism:** Sets values based on input type, handles radio groups by checking matching value, triggers language reload if `data-l` attribute exists.

**Usage Example:**
```javascript
// Set input value
ifc_set("username", "john_doe");

// Set radio group
ifc_set("gender", "male");
```

---

### ifc_copy

Copies the value from one form element to another.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `source`  | `object\|string` | —       | Source element or name |
| `target`  | `object\|string` | —       | Target element or name |

**Return Value:** `void`

**Mechanism:** Uses `ifc_get` to retrieve source value and `ifc_set` to apply it to target.

**Usage Example:**
```javascript
// Copy email to confirmation field
ifc_copy("email", "confirm_email");
```

---

### ifc_limit

Truncates the value of a form element to a maximum length.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `length`  | `number`         | —       | Maximum allowed length |

**Return Value:** `void`

**Mechanism:** Gets current value, truncates if too long, sets truncated value back.

**Usage Example:**
```javascript
// Limit title to 100 characters
ifc_limit("title", 100);
```

---

## LIST

### ifc_list_activate

Checks all checkboxes in a named list.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `name`    | `string` | `"list"` | Base name of checkbox group |

**Return Value:** `void`

**Mechanism:** Selects all checkboxes whose names start with the given prefix and end with `]`, then clicks unchecked ones.

**Usage Example:**
```javascript
// Select all items in a list
ifc_list_activate("users");
```

---

### ifc_list_invert

Inverts the checked state of all checkboxes in a named list.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `name`    | `string` | `"list"` | Base name of checkbox group |

**Return Value:** `void`

**Mechanism:** Clicks every checkbox in the group to toggle their states.

**Usage Example:**
```javascript
// Invert selection
ifc_list_invert("files");
```

---

### ifc_list_deactivate

Unchecks all checked checkboxes in a named list.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `name`    | `string` | `"list"` | Base name of checkbox group |

**Return Value:** `void`

**Mechanism:** Clicks only checked checkboxes to uncheck them.

**Usage Example:**
```javascript
// Deselect all items
ifc_list_deactivate("permissions");
```

---

## TEXTAREA

### ifc_format

Cleans and formats textarea content with additional formatting rules.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of textarea |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `void`

**Mechanism:** Calls `ifc_clean` with `format=true` to apply both cleaning and formatting transformations.

**Usage Example:**
```javascript
// Format code in editor
ifc_format("code_editor");
```

---

### ifc_clean

Cleans textarea content by normalizing whitespace and special characters.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of textarea |
| `index`   | `number`         | `0`     | Index for elements with same name |
| `format`  | `boolean`        | `false` | Whether to apply additional formatting |

**Return Value:** `void`

**Mechanism:** Preserves scroll position, removes carriage returns and zero-width characters, replaces hard spaces with regular spaces, normalizes line breaks, handles tabs, removes trailing whitespace, optionally applies formatting rules, then restores scroll position.

**Usage Example:**
```javascript
// Clean up pasted text
ifc_clean("description");
```

---

### ifc_keydown

Handles keydown events in textareas and contenteditable elements with advanced editing features.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `event`   | `Event`  | —       | Keyboard event object |

**Return Value:** `boolean`

**Mechanism:** Implements smart backspace (handles indentation), tab insertion/removal, enter with auto-indentation, home key behavior, and keyboard shortcuts for formatting, undo/redo, and highlighting.

**Usage Example:**
```html
<textarea onkeydown="return ifc_keydown(event)">
```

---

## LANGUAGE

### ifc_language_select_all

Selects all language links matching a specific language.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `language`| `string` | —       | Language code to select |

**Return Value:** `boolean`

**Mechanism:** Iterates through document links, finds those with class starting with `language-`, extracts language from ID, and triggers click on matching ones.

**Usage Example:**
```javascript
// Select all English content
ifc_language_select_all("en");
```

---

### ifc_language_select

Selects a language for a target element, loading content if needed.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `source`  | `object\|string` | —       | Source element or name |
| `target`  | `object\|string` | —       | Target element or name |
| `language`| `string`         | —       | Language code to select |

**Return Value:** `boolean`

**Mechanism:** If target already has the language, either returns false or selects all. Otherwise highlights the language link, loads content, and returns false.

**Usage Example:**
```javascript
// Switch editor to French
ifc_language_select("source_field", "target_field", "fr");
```

---

### ifc_language_highlight

Highlights language selector links for a given element ID.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `id`      | `string` | —       | Element ID prefix |
| `language`| `string` | —       | Active language code |

**Return Value:** `void`

**Mechanism:** Iterates through document links, finds those with IDs starting with the given prefix, and sets their class to `language-on` or `language-off`.

**Usage Example:**
```javascript
// Highlight English as active
ifc_language_highlight("title", "en");
```

---

### ifc_language_load

Loads content for a specific language into a target element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `source`  | `object\|string` | —       | Source element or name |
| `target`  | `object\|string` | —       | Target element or name |
| `language`| `string`         | —       | Language code to load |
| `select`  | `boolean`        | `true`  | Whether to select/focus the target |

**Return Value:** `void`

**Mechanism:** Preserves scroll position, purges undo state, sets target value from source using `ifc_language_get`, sets language property, optionally selects and focuses.

**Usage Example:**
```javascript
// Load German content
ifc_language_load("source", "target", "de");
```

---

### ifc_language_reload

Reloads content for the current language of a target element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `source`  | `object\|string` | —       | Source element or name |
| `target`  | `object\|string` | —       | Target element or name |

**Return Value:** `void`

**Mechanism:** Gets current language from target, calls `ifc_language_load` with that language.

**Usage Example:**
```javascript
// Reload current language content
ifc_language_reload("source", "target");
```

---

### ifc_language_save

Saves content from source to target, preserving language-specific data.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `source`  | `object\|string` | —       | Source element or name |
| `target`  | `object\|string` | —       | Target element or name |

**Return Value:** `void`

**Mechanism:** Gets language from source, uses `ifc_language_set` to merge source value into target value.

**Usage Example:**
```javascript
// Save content preserving language data
ifc_language_save("editor", "storage");
```

---

### ifc_language_get

Extracts content for a specific language from a multilingual string.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `text`    | `string` | —       | Multilingual text with separators |
| `language`| `string` | —       | Language code to extract |

**Return Value:** `string`

**Mechanism:** Uses `ifc_language_separator` (ASCII 31) to parse language segments. Returns default content if language is empty, or specific language content if found.

**Usage Example:**
```javascript
// Extract English content
var en_text = ifc_language_get(multilingual_string, "en");
```

---

### ifc_language_set

Sets content for a specific language in a multilingual string.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `text`    | `string` | —       | Original multilingual text |
| `value`   | `string` | —       | New content for the language |
| `language`| `string` | —       | Language code to update |

**Return Value:** `string`

**Mechanism:** Inserts or replaces language-specific content in the multilingual string, preserving other language segments.

**Usage Example:**
```javascript
// Update French content
var updated = ifc_language_set(original, "Bonjour", "fr");
```

---

## COMMON

### ifc_object

Retrieves a DOM element by name or ID.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `name`    | `string` | —       | Element name or ID |
| `index`   | `number` | `0`     | Index for elements with same name |
| `window`  | `Window` | `this`  | Window context to search in |

**Return Value:** `HTMLElement\|null`

**Mechanism:** First tries to find by name using `getElementsByName`, then falls back to `getElementById`.

**Usage Example:**
```javascript
// Get element by name
var field = ifc_object("username");

// Get element by ID
var panel = ifc_object("settings_panel");
```

---

### ifc_focus

Focuses a form element, skipping disabled and hidden elements.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of element |
| `index`   | `number`         | `0`     | Index for elements with same name |

**Return Value:** `void`

**Mechanism:** Converts string to object if needed, checks if element is disabled or hidden, then focuses it.

**Usage Example:**
```javascript
// Focus search field
ifc_focus("search");
```

---

### ifc_autofocus

Automatically focuses the first visible editable element on the page.

**Return Value:** `void`

**Mechanism:** Queries for CODE, file inputs, text inputs, and textareas, checks visibility using bounding rectangles, and focuses the first visible one.

**Usage Example:**
```javascript
// Auto-focus first input on page load
window.addEventListener("load", ifc_autofocus);
```

---

### ifc_scroll

Restores scroll position of a textarea after a delay.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of textarea |
| `top`     | `number`         | —       | Vertical scroll position |
| `left`    | `number`         | —       | Horizontal scroll position |

**Return Value:** `void`

**Mechanism:** Converts string to object if needed, checks if it's a textarea, then uses setTimeout to restore scroll position.

**Usage Example:**
```javascript
// Restore scroll position after content update
ifc_scroll("editor", 100, 0);
```

---

### _ifc_scroll

Internal function to directly set scroll position.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Textarea DOM element |
| `top`     | `number` | —       | Vertical scroll position |
| `left`    | `number` | —       | Horizontal scroll position |

**Return Value:** `void`

**Mechanism:** Directly sets `scrollTop` and `scrollLeft` properties.

**Usage Example:**
```javascript
// Used internally by ifc_scroll
_ifc_scroll(textarea_element, 50, 0);
```

---

### ifc_memorize_position

Stores current scroll position in hidden form fields.

**Return Value:** `void`

**Mechanism:** Gets scroll position from `fx_scroll_container` and stores rounded values in `ifc_left` and `ifc_top` form fields.

**Usage Example:**
```javascript
// Save scroll position before form submission
ifc_memorize_position();
```

---

## DOWNLOAD

### ifc_download

Triggers a file download by creating a temporary link.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `url`     | `string` | —       | URL of file to download |

**Return Value:** `void`

**Mechanism:** Creates an anchor element, sets href, appends to body, clicks it, then removes it after 1 second. Also hides loading indicator.

**Usage Example:**
```javascript
// Download generated report
ifc_download("/reports/sales_2023.pdf");
```

---

## CUSTOM SELECT

### ifc_custom_select

Enhances a custom select element with standard select-like properties and behaviors.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object\|string` | —       | DOM element or name of custom select |

**Return Value:** `void`

**Mechanism:** Defines `type`, `value`, `selectedIndex`, and `options` properties on the element, sets up change listeners, handles label clicks, and manages visual scrolling.

**Usage Example:**
```javascript
// Initialize custom select
ifc_custom_select("country_selector");
```

---

## SYNTAX HIGHLIGHTING

### ifc_highlight_init

Initializes syntax highlighting by creating a cache of regex patterns.

**Return Value:** `void`

**Mechanism:** Iterates through `ifc_highlight_detect` array, creates unique keys for regex patterns, and stores them in the array for caching during highlighting.

**Usage Example:**
```javascript
// Called automatically on script load
ifc_highlight_init();
```

---

### ifc_highlight

Performs syntax highlighting on a textarea or contenteditable element.

| Parameter | Type             | Default | Description |
|-----------|------------------|---------|-------------|
| `object`  | `object`         | —       | DOM element to highlight |
| `mode`    | `number`         | `0`     | Initial highlighting mode |
| `no_insert`| `boolean`       | `false` | Skip certain insertions |
| `bounce`  | `boolean`        | —       | Internal debouncing flag |

**Return Value:** `void`

**Mechanism:** Implements a state machine-based highlighter with debouncing, bracket matching, line numbers, and cursor preservation. Supports HTML, CSS, JavaScript, PHP, and custom format tokens.

**Usage Example:**
```javascript
// Highlight code editor content
ifc_highlight(code_editor_element);
```

---

### ifc_highlight_bracket

Highlights matching brackets in syntax-highlighted content.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Highlighted content container |

**Return Value:** `void`

**Mechanism:** Removes previous active highlights, gets current selection, finds bracket spans, and highlights matching pairs by traversing the DOM.

**Usage Example:**
```javascript
// Called on selection change
document.addEventListener("selectionchange", () => ifc_highlight_bracket(editor));
```

---

### ifc_save_selection

Saves and restores cursor position in contenteditable elements.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `context` | `object` | —       | Content container element |

**Return Value:** `function`

**Mechanism:** Captures current selection state, calculates relative positions, and returns a restore function that recreates the selection after content updates.

**Usage Example:**
```javascript
// Save selection before updating content
var restore = ifc_save_selection(editor);
// ... update content ...
restore(); // Restore cursor position
```

---

### ifc_get_position_in_context

Calculates the position of a character within a contenteditable context.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `context` | `object` | —       | Content container element |
| `position`| `number` | —       | Character position to find |

**Return Value:** `object`

**Mechanism:** Uses TreeWalker to traverse text nodes, subtracting lengths until finding the correct node and offset.

**Usage Example:**
```javascript
// Find position in editor
var pos = ifc_get_position_in_context(editor, 150);
```

---

### ifc_contenteditable_init

Initializes a contenteditable element as a textarea replacement.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `id`      | `string` | —       | ID of element to initialize |

**Return Value:** `void`

**Mechanism:** Creates placeholder element, defines `type`, `value`, and `select` properties, and sets up bracket highlighting on selection changes.

**Usage Example:**
```javascript
// Initialize contenteditable as textarea
ifc_contenteditable_init("editor");
```

---

## UNDO / REDO

### ifc_state_save

Saves the current state of an element for undo functionality.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to save state for |

**Return Value:** `void`

**Mechanism:** Initializes undo/redo stacks if needed, checks for significant changes (position or value), and pushes current state to undo stack.

**Usage Example:**
```javascript
// Save state on input
editor.addEventListener("input", () => ifc_state_save(editor));
```

---

### ifc_state_undo

Reverts to the previous state of an element.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to undo |
| `redo`    | `boolean`| `false` | Whether this is a redo operation |

**Return Value:** `void`

**Mechanism:** Moves current state to opposite stack, pops from source stack, restores value and selection range.

**Usage Example:**
```javascript
// Undo last change
ifc_state_undo(editor);
```

---

### ifc_state_redo

Redoes the last undone state of an element.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to redo |

**Return Value:** `void`

**Mechanism:** Calls `ifc_state_undo` with `redo=true`.

**Usage Example:**
```javascript
// Redo last undone change
ifc_state_redo(editor);
```

---

### ifc_state_get_range

Gets the current selection range of an element.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to get range from |

**Return Value:** `array`

**Mechanism:** For textareas, uses `selectionStart`/`selectionEnd`. For contenteditable, uses `window.getSelection` and range calculations.

**Usage Example:**
```javascript
// Get current selection range
var range = ifc_state_get_range(editor);
```

---

### ifc_state_set_range

Sets the selection range of an element.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to set range on |
| `start`   | `number` | —       | Start position |
| `end`     | `number` | —       | End position |

**Return Value:** `void`

**Mechanism:** For textareas, sets `selectionStart`/`selectionEnd`. For contenteditable, uses `ifc_get_position_in_context` to find nodes and sets range.

**Usage Example:**
```javascript
// Restore selection range
ifc_state_set_range(editor, 10, 20);
```

---

### ifc_state_purge

Clears all undo/redo state from an element.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `object`  | `object` | —       | Element to purge state from |

**Return Value:** `void`

**Mechanism:** Deletes all `ifc_state_*` properties from the element.

**Usage Example:**
```javascript
// Clear undo history
ifc_state_purge(editor);
```

---

## FILE UPLOAD PROGRESS

### ifc_show_upload_progress

Sets up progress tracking for file uploads in a form.

| Parameter | Type     | Default | Description |
|-----------|----------|---------|-------------|
| `form`    | `object` | —       | Form element to track |

**Return Value:** `void`

**Mechanism:** Checks for file inputs, intercepts form submission, creates progress bars, uses XMLHttpRequest for upload with progress events, handles timeouts, and displays response.

**Usage Example:**
```javascript
// Enable upload progress for form
ifc_show_upload_progress(document.getElementById("upload_form"));
```

---

## Constants and Configuration

### Syntax Highlighting Configuration

| Constant | Description |
|----------|-------------|
| `ifc_highlight_php` | PHP detection pattern |
| `ifc_highlight_token` | Token detection pattern |
| `ifc_highlight_detect` | Array of highlighting state definitions |
| `ifc_language_separator` | ASCII 31 character used to separate language segments |
| `ifc_language_flag` | Boolean flag to prevent recursive language operations |

### Highlighting States

| State | Description |
|-------|-------------|
| 0     | Root/HTML context |
| 1     | HTML tag context |
| 2     | HTML double-quoted string |
| 3     | HTML single-quoted string |
| 4     | HTML comment |
| 8     | Return to root context |
| 10    | CSS/Style context |
| 11    | CSS double-quoted string |
| 12    | CSS single-quoted string |
| 13    | CSS comment |
| 20    | JavaScript context |
| 21    | JS double-quoted string |
| 22    | JS single-quoted string |
| 23    | JS multiline comment |
| 24    | JS single-line comment |
| 25    | JS regex context |
| 30    | PHP context |
| 31    | PHP double-quoted string |
| 32    | PHP single-quoted string |
| 33    | PHP multiline comment |
| 34    | PHP single-line comment |
| 40    | Format context |
| 41    | Format type context |
| 50    | Token context |


<!-- HASH:0ffe80c3b3996aae2b6aedcb61e85099 -->
