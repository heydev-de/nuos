# PWNC API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/ifc.js)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## IFC (InterFace Communication) Module

The `ifc.js` file provides a comprehensive JavaScript interface for managing form interactions, input handling, syntax highlighting, and state management in the PWNC Web Platform. It enables seamless communication between frontend components and the backend via form submissions, while offering advanced features like multilingual text management, undo/redo functionality, and real-time syntax highlighting.

---

## Command Functions

Functions for submitting and managing form-based commands.

### `ifc_post(message = "", param = "")`

Submits a command to the backend via the IFC form.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `message` | string   | Command identifier (e.g., `"save"`, `"delete"`).                           |
| `param`   | string   | Additional parameter data to send with the command.                        |

**Mechanism:**
- Sets the `ifc_message` and `ifc_param` hidden form fields.
- Memorizes the current scroll position.
- Dispatches a `submit` event on the IFC form and submits it if not prevented.

**Usage:**
```javascript
// Submit a "save" command with a parameter
ifc_post("save", "user_profile");
```

---

### `ifc_cancel(offset = 0)`

Cancels the current operation and resets form fields.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `offset`  | number | Index offset for form elements to reset.                                   |

**Mechanism:**
- Resets form fields starting from the given offset.
- Submits an `ifc_cancel` command.

**Usage:**
```javascript
// Cancel the current operation and reset the first 3 form fields
ifc_cancel(3);
```

---

### `ifc_autopost(object, message = "")`

Automatically submits a command when a form element changes.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name to monitor for changes.                            |
| `message` | string          | Command to submit on change.                                               |

**Mechanism:**
- Listens for `change` events on the specified object.
- Submits the given command via `ifc_post` when triggered.

**Usage:**
```javascript
// Auto-submit "update_preferences" when a checkbox changes
ifc_autopost("theme_checkbox", "update_preferences");
```

---

### `ifc_response(value)`

Displays a response message in the IFC response area.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `value`   | string | HTML content to display in the response area.                              |

**Mechanism:**
- Finds the `ifc-response` element.
- Temporarily hides it, updates its content, and fades it back in.

**Usage:**
```javascript
// Show a success message
ifc_response("<div class='success'>Settings saved!</div>");
```

---

## Value Functions

Functions for retrieving, setting, and manipulating form field values.

### `ifc_get(object, index = 0)`

Retrieves the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Returns:**
- The element's value, or `false` if unsupported.

**Mechanism:**
- Handles different input types (checkbox, radio, text, file, etc.).
- For radio groups, returns the value of the checked option.

**Usage:**
```javascript
// Get the value of a text input
const username = ifc_get("username_field");
```

---

### `ifc_title(object, index = 0)`

Retrieves the display title/label of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Returns:**
- The element's title/label as a string.

**Mechanism:**
- Extracts titles from labels, button text, or select options.
- Cleans up label content by removing non-essential elements.

**Usage:**
```javascript
// Get the label of a checkbox
const label = ifc_title("agree_checkbox");
```

---

### `ifc_reset(offset = 0)`

Resets all form fields starting from the given offset.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `offset`  | number | Index offset for form elements to reset.                                   |

**Mechanism:**
- Iterates through form elements and clears their values using `ifc_del`.

**Usage:**
```javascript
// Reset all form fields
ifc_reset();
```

---

### `ifc_del(object, index = 0)`

Clears the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Mechanism:**
- Handles different input types (checkbox, radio, text, select, etc.).
- For radio groups, clears all options.

**Usage:**
```javascript
// Clear a text input
ifc_del("search_field");
```

---

### `ifc_set(object, value = "", index = 0)`

Sets the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `value`   | string          | Value to set.                                                              |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Mechanism:**
- Handles different input types (checkbox, radio, text, select, etc.).
- For radio groups, checks the option matching the value.
- Triggers language reload if the element has a `data-l` attribute.

**Usage:**
```javascript
// Set a text input value
ifc_set("username_field", "john_doe");
```

---

### `ifc_copy(source, target)`

Copies the value from one form element to another.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `source`  | object/string   | Source element or its name.                                                |
| `target`  | object/string   | Target element or its name.                                                |

**Mechanism:**
- Retrieves the source value using `ifc_get`.
- Sets the target value using `ifc_set`.

**Usage:**
```javascript
// Copy email from one field to another
ifc_copy("email_input", "email_confirm");
```

---

### `ifc_limit(object, length)`

Limits the length of a form element's value.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `length`  | number          | Maximum allowed length.                                                    |

**Mechanism:**
- Truncates the value if it exceeds the specified length.

**Usage:**
```javascript
// Limit a text input to 100 characters
ifc_limit("comment_field", 100);
```

---

## List Functions

Functions for managing groups of checkboxes.

### `ifc_list_activate(name = "list")`

Checks all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `name`    | string | Base name of the checkbox group.                                            |

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks unchecked checkboxes to activate them.

**Usage:**
```javascript
// Activate all items in a list
ifc_list_activate("items");
```

---

### `ifc_list_invert(name = "list")`

Inverts the checked state of all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `name`    | string | Base name of the checkbox group.                                            |

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks each checkbox to invert its state.

**Usage:**
```javascript
// Invert selection in a list
ifc_list_invert("items");
```

---

### `ifc_list_deactivate(name = "list")`

Unchecks all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `name`    | string | Base name of the checkbox group.                                            |

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks checked checkboxes to deactivate them.

**Usage:**
```javascript
// Deactivate all items in a list
ifc_list_deactivate("items");
```

---

## Textarea Functions

Functions for managing and formatting textarea content.

### `ifc_format(object, index = 0)`

Formats the content of a textarea.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Mechanism:**
- Calls `ifc_clean` with the `format` flag set to `true`.

**Usage:**
```javascript
// Format a textarea's content
ifc_format("content_textarea");
```

---

### `ifc_clean(object, index, format = false)`

Cleans and optionally formats textarea content.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |
| `format`  | boolean         | Whether to apply formatting rules.                                         |

**Mechanism:**
- Removes unwanted characters (carriage returns, zero-width spaces, etc.).
- Replaces hard spaces, line separators, and tabs with standard equivalents.
- Applies formatting rules if enabled (e.g., reducing multiple linebreaks).

**Usage:**
```javascript
// Clean a textarea's content without formatting
ifc_clean("content_textarea");

// Clean and format a textarea's content
ifc_clean("content_textarea", 0, true);
```

---

### `ifc_keydown(event)`

Handles keydown events for textareas to enable advanced editing features.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `event`   | Event  | The keydown event.                                                         |

**Mechanism:**
- Supports special key combinations (e.g., `Alt+F` for formatting, `Ctrl+Z` for undo).
- Handles indentation, line navigation, and multiline selections.
- Prevents default behavior for handled keys.

**Usage:**
```javascript
// Attach keydown handler to a textarea
document.getElementById("content_textarea").addEventListener("keydown", ifc_keydown);
```

---

## Language Functions

Functions for managing multilingual text content.

### `ifc_language_select_all(language)`

Selects all language links for a specific language.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `language`| string | Language code to select.                                                   |

**Mechanism:**
- Clicks all language links matching the specified language.

**Usage:**
```javascript
// Select all German language links
ifc_language_select_all("de");
```

---

### `ifc_language_select(source, target, language)`

Selects a specific language for a target element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `source`  | object/string   | Source element or its name.                                                |
| `target`  | object/string   | Target element or its name.                                                |
| `language`| string          | Language code to select.                                                   |

**Mechanism:**
- Highlights the selected language.
- Loads the language-specific content into the target element.

**Usage:**
```javascript
// Load French content into a target textarea
ifc_language_select("source_textarea", "target_textarea", "fr");
```

---

### `ifc_language_highlight(id, language)`

Highlights language links for a specific language.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `id`      | string | Base ID of the language links.                                             |
| `language`| string | Language code to highlight.                                                |

**Mechanism:**
- Updates the class of language links to reflect the selected language.

**Usage:**
```javascript
// Highlight German language links
ifc_language_highlight("language_links", "de");
```

---

### `ifc_language_load(source, target, language, select = true)`

Loads language-specific content into a target element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `source`  | object/string   | Source element or its name.                                                |
| `target`  | object/string   | Target element or its name.                                                |
| `language`| string          | Language code to load.                                                     |
| `select`  | boolean         | Whether to focus and select the target element.                            |

**Mechanism:**
- Extracts the language-specific content from the source using `ifc_language_get`.
- Updates the target element's value and language property.

**Usage:**
```javascript
// Load Spanish content into a target textarea
ifc_language_load("source_textarea", "target_textarea", "es");
```

---

### `ifc_language_reload(source, target)`

Reloads the current language content into a target element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `source`  | object/string   | Source element or its name.                                                |
| `target`  | object/string   | Target element or its name.                                                |

**Mechanism:**
- Retrieves the target's current language and reloads its content.

**Usage:**
```javascript
// Reload the current language content
ifc_language_reload("source_textarea", "target_textarea");
```

---

### `ifc_language_save(source, target)`

Saves the current language content from a source to a target element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `source`  | object/string   | Source element or its name.                                                |
| `target`  | object/string   | Target element or its name.                                                |

**Mechanism:**
- Updates the target's multilingual content with the source's value.

**Usage:**
```javascript
// Save content from a source to a target
ifc_language_save("source_textarea", "target_textarea");
```

---

### `ifc_language_get(text, language)`

Extracts language-specific content from a multilingual string.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `text`    | string | Multilingual string.                                                       |
| `language`| string | Language code to extract.                                                  |

**Returns:**
- The extracted language-specific content.

**Mechanism:**
- Uses a unit separator (`\x1F`) to parse multilingual strings.

**Usage:**
```javascript
// Extract French content from a multilingual string
const frenchContent = ifc_language_get(multilingualText, "fr");
```

---

### `ifc_language_set(text, value, language)`

Updates language-specific content in a multilingual string.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `text`    | string | Original multilingual string.                                              |
| `value`   | string | New content for the specified language.                                    |
| `language`| string | Language code to update.                                                   |

**Returns:**
- The updated multilingual string.

**Mechanism:**
- Uses a unit separator (`\x1F`) to update multilingual strings.

**Usage:**
```javascript
// Update French content in a multilingual string
const updatedText = ifc_language_set(multilingualText, "Bonjour", "fr");
```

---

## Common Functions

Utility functions for DOM manipulation and event handling.

### `ifc_object(name, index = 0, window = this)`

Retrieves a DOM element by name or ID.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `name`    | string          | Element name or ID.                                                        |
| `index`   | number          | Index of the element if multiple exist.                                    |
| `window`  | Window          | Window context to search within.                                           |

**Returns:**
- The DOM element, or `null` if not found.

**Mechanism:**
- Searches by name first, then by ID if no index is specified.

**Usage:**
```javascript
// Get the first element with name "username"
const usernameField = ifc_object("username");

// Get the element with ID "submit_button"
const submitButton = ifc_object("submit_button", -1);
```

---

### `ifc_focus(object, index = 0)`

Focuses a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `index`   | number          | Index of the element if multiple exist.                                    |

**Mechanism:**
- Skips hidden or disabled elements.

**Usage:**
```javascript
// Focus a text input
ifc_focus("username_field");
```

---

### `ifc_autofocus()`

Automatically focuses the first visible and eligible form element.

**Mechanism:**
- Searches for visible `input[type=text]`, `textarea`, or `contenteditable` elements.
- Focuses the first one found, optionally selecting its content.

**Usage:**
```javascript
// Focus the first eligible field on page load
window.addEventListener("load", ifc_autofocus);
```

---

### `ifc_scroll(object, top, left)`

Scrolls a textarea to specific coordinates.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |
| `top`     | number          | Vertical scroll position.                                                  |
| `left`    | number          | Horizontal scroll position.                                                |

**Mechanism:**
- Uses a timeout to ensure the scroll is applied after rendering.

**Usage:**
```javascript
// Scroll a textarea to the top-left corner
ifc_scroll("content_textarea", 0, 0);
```

---

### `ifc_memorize_position()`

Memorizes the current scroll position in hidden form fields.

**Mechanism:**
- Stores scroll positions in `ifc_left` and `ifc_top` fields.

**Usage:**
```javascript
// Memorize scroll position before submitting a form
ifc_memorize_position();
```

---

## Download Function

### `ifc_download(url)`

Triggers a file download from a given URL.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `url`     | string | URL of the file to download.                                               |

**Mechanism:**
- Creates a hidden anchor element and triggers a click event.

**Usage:**
```javascript
// Download a file
ifc_download("/downloads/report.pdf");
```

---

## Custom Select Function

### `ifc_custom_select(object)`

Enhances a custom select element with standard select behavior.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `object`  | object/string   | DOM element or its name.                                                   |

**Mechanism:**
- Defines properties (`type`, `value`, `selectedIndex`, `options`) to mimic a native select.
- Handles change events and click interactions.

**Usage:**
```javascript
// Enhance a custom select element
ifc_custom_select("custom_select");
```

---

## Syntax Highlighting

Functions for real-time syntax highlighting in textareas or contenteditable elements.

### `ifc_highlight_init()`

Initializes the syntax highlighting system.

**Mechanism:**
- Preprocesses regex patterns for faster matching.

**Usage:**
```javascript
// Initialize syntax highlighting (called automatically)
ifc_highlight_init();
```

---

### `ifc_highlight(object, mode = 0, no_insert = false, bounce = false)`

Applies syntax highlighting to a textarea or contenteditable element.

| Parameter  | Type            | Description                                                                 |
|------------|-----------------|-----------------------------------------------------------------------------|
| `object`   | object          | DOM element to highlight.                                                   |
| `mode`     | number          | Initial highlighting mode (e.g., `0` for HTML).                            |
| `no_insert`| boolean/number  | Flags to skip certain patterns (e.g., PHP, tokens).                        |
| `bounce`   | boolean         | Internal flag for debouncing.                                              |

**Mechanism:**
- Debounces rapid changes to avoid performance issues.
- Uses a state machine to parse and highlight text based on predefined patterns.
- Supports nested languages (e.g., PHP inside HTML, CSS inside HTML).

**Usage:**
```javascript
// Highlight a textarea's content as HTML
ifc_highlight(document.getElementById("html_editor"));
```

---

### `ifc_highlight_bracket(object)`

Highlights matching brackets in a highlighted element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `object`  | object | DOM element containing highlighted content.                                |

**Mechanism:**
- Finds the nearest bracket to the cursor and highlights its pair.

**Usage:**
```javascript
// Highlight matching brackets in a code editor
document.addEventListener("selectionchange", () => {
    ifc_highlight_bracket(document.getElementById("code_editor"));
});
```

---

### `ifc_contenteditable_init(id)`

Initializes a contenteditable element to behave like a textarea.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `id`      | string | ID of the contenteditable element.                                         |

**Mechanism:**
- Defines properties (`type`, `value`, `select`) to mimic a textarea.
- Adds a placeholder element and event listeners for bracket highlighting.

**Usage:**
```javascript
// Initialize a contenteditable element
ifc_contenteditable_init("code_editor");
```

---

## Undo/Redo Functions

Functions for managing undo and redo states in textareas or contenteditable elements.

### `ifc_state_save(object)`

Saves the current state of an element for undo/redo.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `object`  | object | DOM element to save.                                                       |

**Mechanism:**
- Stores the element's value and cursor position.
- Limits the undo stack to 100 states.

**Usage:**
```javascript
// Save the state of a textarea
ifc_state_save(document.getElementById("content_textarea"));
```

---

### `ifc_state_undo(object, redo = false)`

Undoes or redoes the last change.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| `object`  | object  | DOM element to undo/redo.                                                  |
| `redo`    | boolean | Whether to redo instead of undo.                                           |

**Mechanism:**
- Restores the previous value and cursor position from the undo/redo stack.

**Usage:**
```javascript
// Undo the last change
ifc_state_undo(document.getElementById("content_textarea"));

// Redo the last undone change
ifc_state_redo(document.getElementById("content_textarea"));
```

---

### `ifc_state_redo(object)`

Alias for `ifc_state_undo(object, true)`.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `object`  | object | DOM element to redo.                                                       |

**Usage:**
```javascript
// Redo the last undone change
ifc_state_redo(document.getElementById("content_textarea"));
```

---

### `ifc_state_purge(object)`

Clears the undo/redo history for an element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `object`  | object | DOM element to purge.                                                      |

**Usage:**
```javascript
// Clear undo/redo history
ifc_state_purge(document.getElementById("content_textarea"));
```

---

## File Upload Progress

### `ifc_show_upload_progress(form)`

Enhances a form to display upload progress for file inputs.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `form`    | object | Form element to enhance.                                                   |

**Mechanism:**
- Adds progress bars for file inputs.
- Overrides form submission to use `XMLHttpRequest` for progress tracking.

**Usage:**
```javascript
// Enhance a form to show upload progress
ifc_show_upload_progress(document.getElementById("upload_form"));
```

---

## Constants

| Name                     | Value               | Description                                                                 |
|--------------------------|---------------------|-----------------------------------------------------------------------------|
| `ifc_language_separator` | `\x1F` (unit sep.)  | Separator used in multilingual strings.                                    |
| `ifc_language_flag`      | `false`             | Flag to prevent recursive language selection.                              |

## Syntax Highlighting Patterns

The `ifc_highlight_detect` array defines patterns for syntax highlighting in various languages (HTML, CSS, JavaScript, PHP, etc.). Each pattern is an array with the following structure:

| Index | Type     | Description                                                                 |
|-------|----------|-----------------------------------------------------------------------------|
| 0     | RegExp   | Regex pattern to match.                                                     |
| 1     | number   | State transition (positive to enter, negative to exit, zero to keep).      |
| 2     | string   | CSS class name for the matched text.                                        |
| 3     | number   | Flags to control pattern behavior (e.g., skip PHP, skip tokens).            |
| 4     | number   | Internal cache key (auto-assigned by `ifc_highlight_init`).                 |

**Example Patterns:**
```javascript
// PHP detection
ifc_highlight_php = [ /<\?(?:php\s|=)/i, 30, "php", 1 ];

// HTML comment
[ /<!--/, 4, "html-comment" ]
```


<!-- HASH:82b0b3f9ee57a3b518bd77d6947636a6 -->
