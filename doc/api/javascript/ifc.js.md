# PWNC API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/ifc.js)

- **Version:** `26.9.21.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## ifc.js — Interface Control Library

The `ifc.js` file is the core client-side interface control library for the PWNC Web Platform. It provides a comprehensive set of functions for managing form interactions, value manipulation, multilingual content editing, syntax highlighting, undo/redo state management, custom select elements, and file upload progress tracking. It is designed to work with the platform's `ifc` form (the main interface form) and integrates with the `fx_*` utility functions for event handling, animations, and styling.

---

## COMMAND

### ifc_command

Executes a JavaScript command stored in a `data-ifc-command` attribute on a DOM element, optionally prompting for confirmation first.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The element containing the `data-ifc-command` attribute with the code to execute. |

**Return value:** `void`

**Inner mechanisms:**
1. Checks for a `data-ifc-confirm` attribute. If present, displays a `confirm()` dialog using the element's text content as the message. If the user cancels, the function returns early.
2. Creates a new `Function` from the `data-ifc-command` attribute value and calls it with the element as `this`.
3. Wraps execution in a `try/catch` to silently swallow errors.

**Usage context:** Used for inline command buttons (e.g., "Delete", "Save") where the action code is embedded in the HTML attribute.

```html
<button data-ifc-command="ifc_post('ifc_delete')" data-ifc-confirm>Delete</button>
```

### ifc_post

Submits the main `ifc` form by setting hidden message and parameter fields, memorizing scroll position, and dispatching a submit event.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `message` | `string` | `""` | The command message to set in the `ifc_message` hidden field. |
| `param` | `string` | `""` | The parameter to set in the `ifc_param` hidden field. |

**Return value:** `void`

**Inner mechanisms:**
1. Sets `ifc_message` and `ifc_param` hidden form fields if non-empty values are provided.
2. Calls `ifc_memorize_position()` to save current scroll position.
3. Dispatches a cancelable `submit` event on the `ifc` form.
4. If no handler prevented the default, calls `form.submit()`.

**Usage context:** The primary method for triggering server-side actions through the interface form.

```javascript
ifc_post("ifc_save", "document_42");
```

### ifc_cancel

Resets form fields and posts a cancel command to the server.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `offset` | `int` | `0` | Starting index in the form's elements collection for reset. |

**Return value:** `void`

**Inner mechanisms:**
1. Calls `ifc_reset(offset)` to clear all form elements from the given offset.
2. Calls `ifc_post("ifc_cancel")` to notify the server of cancellation.

**Usage context:** Used when a user cancels an edit operation, clearing the form and signaling the server.

```javascript
ifc_cancel();
```

### ifc_autopost

Automatically posts the `ifc` form whenever the specified object changes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement|string` | The element to listen on, or its name/id. |
| `message` | `string` | The message to post on change. |

**Return value:** `void`

**Inner mechanisms:**
1. If `object` is not a DOM element, resolves it via `ifc_object()`.
2. Attaches a `change` event listener using `fx_event_listen()` that calls `ifc_post(message)`.

**Usage context:** For auto-saving or auto-submitting when a specific field changes (e.g., a dropdown selection).

```javascript
ifc_autopost("category_selector", "ifc_change_category");
```

### ifc_response

Updates the `ifc-response` element with new content, with a fade-in animation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `string` | The HTML content to display in the response element. |

**Return value:** `void`

**Inner mechanisms:**
1. Retrieves the `ifc-response` element via `ifc_object()`.
2. If not found, returns early.
3. Hides the element by setting `className = "hidden"`.
4. Uses `fx_animation_frame()` to remove the hidden class, creating a fade-in effect.
5. Sets `innerHTML` to the provided value.

**Usage context:** Displaying server response messages (success, error, warnings) in the interface.

```javascript
ifc_response("Record saved successfully.");
```

---

## VALUE

### ifc_get

Retrieves the value from a form element, handling different input types appropriately.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The element or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `string|FileList|boolean` — The element's value, file list, or `false` if no value.

**Inner mechanisms:**
- **checkbox/radio:** Returns the value if checked, otherwise falls through.
- **button, date, hidden, password, reset, select-one, select-multiple, submit, text, textarea:** Returns `object.value`.
- **file:** Returns `object.files` (FileList) if available, otherwise `object.value`.
- **default (radio group by index):** Iterates through indexed elements looking for a checked radio.
- Returns `false` if nothing matched.

**Usage context:** Reading values from form fields in a type-aware manner.

```javascript
var name = ifc_get("user_name");
var files = ifc_get("avatar_upload");
```

### ifc_title

Retrieves a human-readable title/label for a form element.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The element or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `string` — The element's title text.

**Inner mechanisms:**
- **button, reset, submit:** Returns trimmed `textContent`.
- **checkbox, file, hidden, password, radio, text, textarea:** Finds the closest `LABEL` ancestor, clones it, removes all non-`SPAN` elements and `SPAN` elements with classes, then returns trimmed text.
- **select-one, select-multiple:** Returns the text of the selected option.
- Returns empty string if no match.

**Usage context:** Generating labels for form elements, useful for confirmation dialogs or display purposes.

```javascript
var label = ifc_title("user_email"); // e.g., "Email Address"
```

### ifc_reset

Clears all form elements in the `ifc` form starting from a given offset.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `offset` | `int` | `0` | Starting index in the form's elements collection. |

**Return value:** `void`

**Inner mechanisms:**
1. Iterates through `document.forms["ifc"].elements` starting at `offset`.
2. Calls `ifc_del(object, 0, true)` on each element to clear it.

**Usage context:** Resetting the entire form or a portion of it.

```javascript
ifc_reset(); // Clear all fields
```

### ifc_del

Clears the value of a single form element, handling different input types.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The element or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `void`

**Inner mechanisms:**
- **checkbox, radio:** Sets `value = ""` and `checked = false`.
- **date, file, hidden, password, text, textarea:** Sets `value = ""`.
- **select-one, select-multiple:** Sets `selectedIndex = -1`.
- **default (radio group):** Iterates through indexed elements, clearing value and checked state.
- Calls `ifc_focus(object)` at the end.

**Usage context:** Clearing individual form fields.

```javascript
ifc_del("user_name");
```

### ifc_set

Sets the value of a form element, handling different input types and triggering language reloads.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The element or its name/id. |
| `value` | `string` | `""` | The value to set. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `void`

**Inner mechanisms:**
- **checkbox, date, hidden, password, radio, select-one, select-multiple, text, textarea:** Sets `object.value = value`.
- **file:** Does nothing (security restriction).
- **default (radio group):** Iterates through indexed elements, setting `checked` based on value match.
- If the element has a `data-l` attribute, calls `ifc_language_reload()` to update the corresponding language field.

**Usage context:** Programmatically setting form field values.

```javascript
ifc_set("user_name", "John Doe");
```

### ifc_copy

Copies the value from one form element to another.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `HTMLElement|string` | The source element or its name/id. |
| `target` | `HTMLElement|string` | The target element or its name/id. |

**Return value:** `void`

**Inner mechanisms:**
1. Calls `ifc_get(source)` to retrieve the source value.
2. Calls `ifc_set(target, value)` to set it on the target.

**Usage context:** Duplicating values between fields (e.g., copying a slug from a title).

```javascript
ifc_copy("title", "slug");
```

### ifc_limit

Truncates the value of a form element to a specified maximum length.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement|string` | The element or its name/id. |
| `length` | `int` | Maximum number of characters. |

**Return value:** `void`

**Inner mechanisms:**
1. Retrieves the current value via `ifc_get()`.
2. If the value's length exceeds `length`, truncates it using `substring(0, length)` and sets it back via `ifc_set()`.

**Usage context:** Enforcing character limits on text inputs.

```javascript
ifc_limit("description", 255);
```

---

## LIST

### ifc_list_activate

Checks all checkboxes in a named list that are currently unchecked.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `string` | `"list"` | The base name prefix for the checkbox group. |

**Return value:** `void`

**Inner mechanisms:**
1. Selects all `INPUT` elements whose `name` starts with `name + "["` and ends with `"]"`.
2. Iterates through them, calling `.click()` on any that are not checked.

**Usage context:** "Select all" functionality for list-based checkboxes.

```javascript
ifc_list_activate("users");
```

### ifc_list_invert

Toggles the checked state of all checkboxes in a named list.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `string` | `"list"` | The base name prefix for the checkbox group. |

**Return value:** `void`

**Inner mechanisms:**
1. Selects all matching checkboxes.
2. Calls `.click()` on each, toggling their state.

**Usage context:** "Invert selection" functionality.

```javascript
ifc_list_invert("files");
```

### ifc_list_deactivate

Unchecks all checkboxes in a named list that are currently checked.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `string` | `"list"` | The base name prefix for the checkbox group. |

**Return value:** `void`

**Inner mechanisms:**
1. Selects all matching checkboxes.
2. Calls `.click()` on any that are checked.

**Usage context:** "Select none" functionality.

```javascript
ifc_list_deactivate("permissions");
```

---

## TEXTAREA

### ifc_format

Cleans and formats textarea content with additional formatting rules (collapsing whitespace, reducing line breaks).

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The textarea or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `void`

**Inner mechanisms:**
1. Calls `ifc_clean(object, index, true)` with `format = true`.

**Usage context:** Formatting text content (e.g., via Alt+F shortcut) to normalize whitespace and structure.

```javascript
ifc_format("content_editor");
```

### ifc_clean

Cleans textarea content by normalizing whitespace, removing problematic characters, and optionally applying formatting rules.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The textarea or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |
| `format` | `boolean` | `false` | Whether to apply additional formatting (collapse spaces, reduce line breaks). |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves the object if needed.
2. Saves current scroll position.
3. Retrieves the current value via `ifc_get()`.
4. Applies a series of regex replacements:
   - Removes carriage returns and zero-width characters (`\r`, `\u200C`, `\u200D`, `\uFEFF`).
   - Replaces hard spaces with normal spaces.
   - Replaces line/paragraph separators with `\n`.
   - Replaces tabs with 4 spaces.
   - Removes whitespace before line breaks.
   - If `format` is true: collapses multiple spaces/single line breaks into one space, reduces 3+ line breaks to 2.
   - Removes leading and trailing whitespace.
5. Sets the cleaned value via `ifc_set()`.
6. Restores scroll position via `_ifc_scroll()`.

**Usage context:** Cleaning up pasted or edited text content (e.g., via Alt+W shortcut).

```javascript
ifc_clean("article_body");
```

### ifc_keydown

Handles keydown events in textareas and contenteditable elements, providing custom behavior for Backspace, Tab, Enter, Home, and various keyboard shortcuts.

| Parameter | Type | Description |
|-----------|------|-------------|
| `event` | `KeyboardEvent` | The keydown event. |

**Return value:** `boolean` — `true` to allow default behavior, `false` to prevent it.

**Inner mechanisms:**
1. Builds a composite key string including modifier prefixes (e.g., `[Alt]`, `[Control]`, `[Shift]`).
2. Determines selection state:
   - For `HTMLTextAreaElement`: uses `selectionStart`/`selectionEnd`.
   - For contenteditable: uses `window.getSelection()` and `Range` to compute selection boundaries.
3. Handles specific keys:
   - **Modifier keys (Alt, Control, Meta, Shift):** Returns `true` (allow default).
   - **Backspace:** Intelligently deletes indentation or word content based on cursor position and line structure.
   - **Tab / Shift+Tab:** Indents or dedents selected lines (4 spaces).
   - **Enter:** Inserts a newline with current line's indentation.
   - **Home:** Moves cursor to start of line content (after indentation) or start of line.
   - **Alt+F:** Calls `ifc_format()`.
   - **Alt+W:** Calls `ifc_clean()`.
   - **Ctrl+Y / Ctrl+Shift+Z:** Calls `ifc_state_redo()`.
   - **Ctrl+Z:** Calls `ifc_state_undo()`.
   - **Arrow keys, End, PageUp, PageDown:** Returns `true` (allow default).
   - **Default:** Returns `true`.
4. Saves state via `ifc_state_save()`.
5. If not returning early, modifies the text value and updates selection.
6. Deletes `ifc_highlight_value` to trigger re-highlighting.

**Usage context:** Attached as a `keydown` event handler on textareas and contenteditable elements to provide IDE-like editing features.

```html
<textarea onkeydown="return ifc_keydown(event)"></textarea>
```

---

## LANGUAGE

### ifc_language_separator

| Name | Value | Description |
|------|-------|-------------|
| `ifc_language_separator` | `String.fromCharCode(31)` (Unit Separator) | The character used to separate language-specific content blocks within a single field value. |

### ifc_language_flag

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `ifc_language_flag` | `boolean` | `false` | Flag to prevent recursive language selection during bulk operations. |

### ifc_language_select_all

Selects a specific language across all language links on the page.

| Parameter | Type | Description |
|-----------|------|-------------|
| `language` | `string` | The language code to select (e.g., `"en"`, `"de"`). |

**Return value:** `boolean` — Always `false` (prevents default link behavior).

**Inner mechanisms:**
1. Sets `ifc_language_flag = true` to prevent recursion.
2. Iterates through all `document.links`.
3. For links with class starting with `"language-"`, extracts the language from the `id` (format: `prefix:language`).
4. If the language matches, triggers `onclick` on the link.
5. Resets the flag and returns `false`.

**Usage context:** "Select all" for a specific language in a multilingual editor.

```javascript
ifc_language_select_all("en");
```

### ifc_language_select

Selects a language for a target field, loading the appropriate content.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `HTMLElement|string` | The source element or its name/id. |
| `target` | `HTMLElement|string` | The target element or its name/id. |
| `language` | `string` | The language code to select. |

**Return value:** `boolean` — Always `false`.

**Inner mechanisms:**
1. Resolves `target` if needed.
2. If the target's current language matches, either returns early (if flag is set) or calls `ifc_language_select_all()`.
3. Otherwise, calls `ifc_language_highlight()` to update link classes, then `ifc_language_load()` to load the content.

**Usage context:** Switching between language tabs in a multilingual form field.

```javascript
ifc_language_select("title_source", "title_target", "de");
```

### ifc_language_highlight

Highlights the active language link and dims others.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `string` | The base ID prefix for language links. |
| `language` | `string` | The active language code. |

**Return value:** `void`

**Inner mechanisms:**
1. Iterates through all `document.links`.
2. For links whose `id` starts with `id + ":"`, sets the class to `"language-on"` if the language matches, or `"language-off"` otherwise.

**Usage context:** Visual feedback when switching languages.

```javascript
ifc_language_highlight("title", "fr");
```

### ifc_language_load

Loads content for a specific language into a target field.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `source` | `HTMLElement|string` | — | The source element or its name/id. |
| `target` | `HTMLElement|string` | — | The target element or its name/id. |
| `language` | `string` | — | The language code to load. |
| `select` | `boolean` | `true` | Whether to select/focus the target after loading. |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves `source` and `target` if needed.
2. Saves scroll position.
3. Purges undo/redo state via `ifc_state_purge()`.
4. Sets `ifc_state_nosave` flag to prevent state saving during programmatic value change.
5. Sets `target.value` to the language-specific content extracted via `ifc_language_get()`.
6. Sets `target.language` to the new language.
7. If not in bulk-select mode, optionally selects and focuses the target, restoring scroll position.

**Usage context:** Loading the correct language content when a user switches language tabs.

```javascript
ifc_language_load("content_source", "content_target", "es");
```

### ifc_language_reload

Reloads the current language content for a target field, typically after the source value changes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `HTMLElement|string` | The source element or its name/id. |
| `target` | `HTMLElement|string` | The target element or its name/id. |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves `source` and `target` if needed.
2. Gets the current language from `target.language` (or empty string if null).
3. Calls `ifc_language_load()` with the current language.

**Usage context:** Called after `ifc_set()` when a field has a `data-l` attribute, to sync the language-specific content.

```javascript
ifc_language_reload("title_source", "title_target");
```

### ifc_language_save

Saves the current target value back into the source's language-specific storage.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `HTMLElement|string` | The source element or its name/id. |
| `target` | `HTMLElement|string` | The target element or its name/id. |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves `source` and `target` if needed.
2. Gets the current language from `source.language` (or empty string if null).
3. Sets `target.value` to the result of `ifc_language_set()`, which merges the target's current value into the source's language-specific content.

**Usage context:** Saving edited content back to the multilingual storage structure.

```javascript
ifc_language_save("content_source", "content_target");
```

### ifc_language_get

Extracts the content for a specific language from a composite text string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | The composite text containing all language versions. |
| `language` | `string` | The language code to extract. |

**Return value:** `string` — The content for the specified language, or empty string if not found.

**Inner mechanisms:**
1. If `language` is empty, returns the text before the first separator.
2. Searches for `separator + language + ":"` in the text.
3. If found, extracts the content between that marker and the next separator (or end of string).
4. Returns empty string if the language is not found.

**Usage context:** Internal function used by `ifc_language_load()` to extract language-specific content.

```javascript
var content = ifc_language_get("Hello\x1fen:Hello\x1fes:Hola", "es"); // "Hola"
```

### ifc_language_set

Sets or updates the content for a specific language within a composite text string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | The composite text containing all language versions. |
| `value` | `string` | The new content for the specified language. |
| `language` | `string` | The language code to update. |

**Return value:** `string` — The updated composite text.

**Inner mechanisms:**
1. If `language` is empty, replaces the text before the first separator with `value`.
2. If `value` is non-empty, prepends `separator + language + ":"`.
3. Searches for the existing language block. If found, replaces it. If not found, appends the new block.
4. Returns the updated composite text.

**Usage context:** Internal function used by `ifc_language_save()` to merge language-specific content.

```javascript
var updated = ifc_language_set("Hello\x1fen:Hello", "Hola", "es"); // "Hello\x1fes:Hola"
```

---

## COMMON

### ifc_object

Retrieves a DOM element by name or ID, with optional index for name-based lookups.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `string` | — | The element's name or ID. |
| `index` | `int` | `0` | Index for name-based lookups (use `-1` to skip name lookup). |
| `window` | `Window` | `this` | The window context to search in. |

**Return value:** `HTMLElement|null` — The found element, or `null` if not found.

**Inner mechanisms:**
1. If `index >= 0`, searches by name using `getElementsByName()`.
2. If found at the given index, returns it.
3. Falls back to `getElementById()`.
4. Returns `null` if nothing is found.

**Usage context:** The primary element resolution function used throughout the library.

```javascript
var field = ifc_object("user_name");
var item = ifc_object("checkbox_group", 2);
```

### ifc_focus

Sets focus on a form element, skipping disabled and hidden elements.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The element or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups. |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves the object if needed.
2. Returns early if the element is disabled or of type `hidden`.
3. Calls `object.focus()`.

**Usage context:** Focusing form fields after operations like clearing or setting values.

```javascript
ifc_focus("user_email");
```

### ifc_autofocus

Automatically focuses the first visible editable element on the page.

**Return value:** `void`

**Inner mechanisms:**
1. Queries for `CODE[contenteditable]`, `INPUT[type=file]`, `INPUT[type=text]`, and `TEXTAREA` elements.
2. Iterates through them, checking if each is within the viewport using `getBoundingClientRect()`.
3. For the first visible element:
   - If it's a text input with value length ≤ 20, selects all text.
   - Calls `focus({ preventScroll: true })`.
4. Returns after focusing the first match.

**Usage context:** Called on page load to automatically focus the primary input field.

```javascript
window.addEventListener("load", ifc_autofocus);
```

### ifc_scroll

Restores scroll position on a textarea element after a short delay.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement|string` | The textarea or its name/id. |
| `top` | `int` | The scroll top position to restore. |
| `left` | `int` | The scroll left position to restore. |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves the object if needed.
2. Returns early if the element is not a textarea.
3. Uses `setTimeout()` with a 10ms delay to call `_ifc_scroll()`, allowing the browser to process value changes first.

**Usage context:** Restoring scroll position after programmatic value changes.

```javascript
ifc_scroll("content_editor", 100, 0);
```

### _ifc_scroll

Internal helper that directly sets scroll position on an element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The target element. |
| `top` | `int` | The scroll top position. |
| `left` | `int` | The scroll left position. |

**Return value:** `void`

**Inner mechanisms:**
1. Sets `object.scrollTop = top` and `object.scrollLeft = left`.

**Usage context:** Called by `ifc_scroll()` after a delay.

### ifc_memorize_position

Saves the current scroll position of the `fx_scroll_container` into hidden form fields.

**Return value:** `void`

**Inner mechanisms:**
1. Calls `ifc_set("ifc_left", ...)` and `ifc_set("ifc_top", ...)` with rounded scroll positions from `fx_scroll_container`.

**Usage context:** Called by `ifc_post()` to preserve scroll position across form submissions.

```javascript
ifc_memorize_position();
```

---

## DOWNLOAD

### ifc_download

Triggers a file download by creating a temporary anchor element and clicking it.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `string` | The URL of the file to download. |

**Return value:** `void`

**Inner mechanisms:**
1. Creates an `<a>` element with the given `href`.
2. Sets `display: none` and appends it to the document body.
3. Calls `object.click()` to trigger the download.
4. After 1 second, removes the element and hides the loading indicator via `fx_visible("ifc-loading", false)`.

**Usage context:** Initiating file downloads from JavaScript without navigating away.

```javascript
ifc_download("/api/export?format=csv");
```

---

## CUSTOM SELECT

### ifc_custom_select

Enhances a custom select element (built with checkboxes and labels) to behave like a native `<select>` element.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement|string` | — | The container element or its name/id. |
| `index` | `int` | `0` | Index for name-based lookups (use `-1` to skip name lookup). |

**Return value:** `void`

**Inner mechanisms:**
1. Resolves the object if needed.
2. Defines custom properties on the object:
   - **`type`:** Returns `"select-multiple"` if checkboxes exist, otherwise `"select-one"`.
   - **`value`:** Getter returns the checked input's value; setter checks matching inputs and calls `change()`.
   - **`selectedIndex`:** Getter returns the index of the checked input; setter checks the input at the given index and calls `change()`.
   - **`options`:** Returns an array of option objects with `index`, `value`, `selected`, and `text` properties.
3. Stores the initial value in `object._value`.
4. Defines a `change()` function that dispatches a `change` event and executes any `data-onchange` code.
5. Attaches `change` listeners to all input elements.
6. For checkbox-based selects, attaches click listeners to labels to handle label clicks (toggling selection).
7. Sets the `--size` CSS variable if a `data-size` attribute is present.
8. Scrolls the checked item into view on initialization.

**Usage context:** Initializing custom select dropdowns that use HTML checkboxes and labels for styling flexibility.

```html
<div id="my_select" data-onchange="ifc_post('ifc_change')">
  <label><input type="checkbox" value="a"> Option A</label>
  <label><input type="checkbox" value="b"> Option B</label>
</div>
<script>
  ifc_custom_select("my_select");
</script>
```

---

## SYNTAX HIGHLIGHTING

### ifc_highlight_php

| Name | Value | Description |
|------|-------|-------------|
| `ifc_highlight_php` | `[ /<\?(?:php\s|=)/i, 30, "php", 1 ]` | Detection rule for entering PHP mode. |

### ifc_highlight_token

| Name | Value | Description |
|------|-------|-------------|
| `ifc_highlight_token` | `[ /(?:^|[^\\])(%%(?:%(?!%)|[^%\s])*)/, 50, "token", 2 ]` | Detection rule for token placeholders. |

### ifc_highlight_detect

| Name | Type | Description |
|------|------|-------------|
| `ifc_highlight_detect` | `Array` | Array of detection rule sets, indexed by state number. Each rule set is an array of rules. |

**Rule format:** Each rule is an array: `[regex, nextState, className, skipType, cacheKey]`

| Index | State | Description |
|-------|-------|-------------|
| 0 | html | HTML root context |
| 1 | html-tag | Inside an HTML tag |
| 2 | html-string (double) | Inside a double-quoted HTML attribute |
| 3 | html-string (single) | Inside a single-quoted HTML attribute |
| 4 | html-comment | Inside an HTML comment |
| 10 | style | Inside a `<style>` block |
| 11 | style-string (double) | Inside a double-quoted CSS string |
| 12 | style-string (single) | Inside a single-quoted CSS string |
| 13 | style-comment | Inside a CSS multiline comment |
| 20 | script | Inside a `<script>` block |
| 21 | script-string (double) | Inside a double-quoted JS string |
| 22 | script-string (single) | Inside a single-quoted JS string |
| 23 | script-comment (multiline) | Inside a JS multiline comment |
| 24 | script-comment (single) | Inside a JS single-line comment |
| 25 | script-regex | Inside a JS regex literal |
| 30 | php | Inside PHP code |
| 31 | php-string (double) | Inside a double-quoted PHP string |
| 32 | php-string (single) | Inside a single-quoted PHP string |
| 33 | php-comment (multiline) | Inside a PHP multiline comment |
| 34 | php-comment (single) | Inside a PHP single-line comment |
| 40 | format | Format placeholder context |
| 41 | format-type | Format type specifier |
| 50 | token | Token placeholder context |

### ifc_highlight_init

Initializes the syntax highlighting detection rules by assigning cache keys to each rule.

**Return value:** `void`

**Inner mechanisms:**
1. Creates a `Map` to track unique regex patterns.
2. Iterates through all detection rule sets and their rules.
3. For each rule's regex, converts it to a string and checks the map.
4. Assigns a unique integer cache key (stored at index 4 of each rule) to avoid redundant regex executions.

**Usage context:** Called once at script load time to prepare the highlighting engine.

```javascript
ifc_highlight_init(); // Called automatically
```

### ifc_highlight

Performs syntax highlighting on a textarea or contenteditable element, producing HTML with `<span>` elements for styled tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The textarea or contenteditable element to highlight. |
| `mode` | `int` | (Optional) Starting state/mode for highlighting. |
| `no_insert` | `boolean` | (Optional) Whether to skip certain state transitions. |
| `bounce` | `boolean` | (Internal) Used for debouncing. |

**Return value:** `void`

**Inner mechanisms:**
1. **Change detection:** Compares current value with stored `ifc_highlight_value`. If unchanged, returns early.
2. **Debouncing:** If not in bounce mode, schedules a delayed call (250ms) and returns. If the delay hasn't elapsed, reschedules.
3. **State machine:** Uses a stack-based state machine:
   - Starts in the given `mode` (default 0 = HTML).
   - For each position in the text, finds the best matching rule in the current state.
   - Caches regex results to avoid redundant searches.
   - On match:
     - **Positive next state:** Opens a `<span>` with the class and pushes the new state.
     - **Negative next state:** Closes multiple `</span>` tags and pops states.
     - **Zero next state:** Outputs the matched text with a class span, handling bracket matching.
4. **Bracket matching:** In states 10, 20, and 30, tracks bracket pairs and assigns IDs for later highlighting.
5. **Line numbers:** If `white-space` is `pre`, adds line number spans.
6. **Layout:** Updates the sibling element's `minHeight` based on line count.
7. **DOM update:** Hides the original element, sets `innerHTML` on it, and restores display.
8. **Cursor restoration:** Calls the restore function from `ifc_save_selection()`.
9. Logs timing to console.

**Usage context:** Called on input/change events in code editors to provide real-time syntax highlighting.

```javascript
ifc_highlight(document.getElementById("code_editor"));
```

### ifc_highlight_bracket

Highlights matching bracket pairs when the cursor is near a bracket.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The highlighted content container. |

**Return value:** `void`

**Inner mechanisms:**
1. Removes `active` class from all previously highlighted brackets.
2. Gets the current selection.
3. If the selection's focus node is within the object:
   - If it's a text node, finds the parent span with a bracket ID.
   - Adjusts the node based on cursor offset.
4. Walks backward through sibling/parent elements, tracking bracket depth.
5. When an unmatched opening bracket is found, adds `active` class to both the opening and closing bracket spans.

**Usage context:** Called on `selectionchange` events to provide bracket pair highlighting.

```javascript
document.addEventListener("selectionchange", () => ifc_highlight_bracket(editor));
```

### ifc_save_selection

Saves the current text selection within a contenteditable element and returns a restore function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `context` | `HTMLElement` | The contenteditable element to save selection within. |

**Return value:** `function` — A restore function that re-applies the saved selection.

**Inner mechanisms:**
1. Gets the current selection.
2. If no selection or selection is outside the context, returns a no-op function.
3. Determines selection direction (forward or backward).
4. Clones the range and calculates absolute start/end positions within the context.
5. Returns a closure that:
   - Gets the active element.
   - Converts saved positions back to node/offset pairs using `ifc_get_position_in_context()`.
   - Creates a new range and selection.
   - Applies direction if reversed.
   - Focuses the element.

**Usage context:** Called before syntax highlighting to preserve cursor position across DOM updates.

```javascript
var restore = ifc_save_selection(editor);
// ... perform highlighting ...
restore();
```

### ifc_get_position_in_context

Converts an absolute character position within a contenteditable element to a DOM node and offset.

| Parameter | Type | Description |
|-----------|------|-------------|
| `context` | `HTMLElement` | The contenteditable element. |
| `position` | `int` | The absolute character position. |

**Return value:** `Object` — `{ node: TextNode|null, position: int }`

**Inner mechanisms:**
1. Creates a `TreeWalker` that iterates over text nodes.
2. For each text node, if the position is beyond its length, subtracts the length and continues.
3. When the position falls within a text node, returns that node and the remaining position.
4. Returns `{ node: null, position: position }` if the position is beyond all text.

**Usage context:** Used by `ifc_save_selection()` restore function and `ifc_state_set_range()` to convert positions to DOM ranges.

```javascript
var pos = ifc_get_position_in_context(editor, 42);
```

### ifc_contenteditable_init

Initializes a contenteditable element to behave like a textarea, with value, type, and select properties.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `string` | The ID of the contenteditable element. |

**Return value:** `void`

**Inner mechanisms:**
1. Gets the element by ID.
2. Creates a placeholder `div` after the element.
3. Defines `type` as a read-only property with value `"textarea"`.
4. Defines `value` as a property:
   - **Getter:** Returns `textContent`.
   - **Setter:** Compares with current value, saves state, sets `textContent`, and dispatches a `change` event.
5. Defines `select` as a function that selects all content using `Range` and `Selection`.
6. Attaches a `selectionchange` listener for bracket highlighting.

**Usage context:** Converting a `<div contenteditable>` into a textarea-like element for use with the rest of the `ifc` system.

```html
<div id="editor" contenteditable="true"></div>
<script>
  ifc_contenteditable_init("editor");
</script>
```

---

## UNDO / REDO

### ifc_state_save

Saves the current value and cursor position to the undo stack.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The textarea or contenteditable element. |

**Return value:** `void`

**Inner mechanisms:**
1. Returns early if `ifc_state_nosave` flag is set.
2. Initializes undo/redo stacks and position if not present.
3. Gets the current selection range via `ifc_state_get_range()`.
4. If the position change is ≤ 1 character, returns early (no significant change).
5. If the new value matches the last undo entry, returns early (duplicate).
6. Pushes `[value, range]` to the undo stack, keeping only the last 100 entries.
7. Clears the redo stack.

**Usage context:** Called on every keystroke or input event to build the undo history.

```javascript
textarea.addEventListener("input", () => ifc_state_save(textarea));
```

### ifc_state_undo

Reverts to the previous state in the undo/redo stack.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | `HTMLElement` | — | The element to undo on. |
| `redo` | `boolean` | `false` | If `true`, performs a redo instead of undo. |

**Return value:** `void`

**Inner mechanisms:**
1. Determines which stack to pop from (`undo` or `redo`) and which to push to.
2. Returns early if the source stack is empty.
3. Pushes the current state to the target stack.
4. Pops the state from the source stack.
5. Sets `ifc_state_nosave` flag, restores the value, clears the flag.
6. Deletes `ifc_highlight_value` to trigger re-highlighting.
7. Restores the cursor position via `ifc_state_set_range()`.

**Usage context:** Called on Ctrl+Z (undo) or Ctrl+Y/Ctrl+Shift+Z (redo).

```javascript
ifc_state_undo(textarea); // Undo
ifc_state_undo(textarea, true); // Redo
```

### ifc_state_redo

Performs a redo operation by calling `ifc_state_undo` with `redo = true`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The element to redo on. |

**Return value:** `void`

**Inner mechanisms:**
1. Calls `ifc_state_undo(object, true)`.

**Usage context:** Called on Ctrl+Y or Ctrl+Shift+Z.

```javascript
ifc_state_redo(textarea);
```

### ifc_state_get_range

Gets the current selection range (start and end positions) within an element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The textarea or contenteditable element. |

**Return value:** `Array` — `[start, end]` character positions.

**Inner mechanisms:**
- **HTMLTextAreaElement:** Uses `selectionStart` and `selectionEnd`.
- **Contenteditable:** Uses `window.getSelection()` and `Range` to compute absolute positions within the element.

**Usage context:** Used by `ifc_state_save()` and `ifc_state_undo()` to track cursor position.

```javascript
var range = ifc_state_get_range(textarea); // [10, 25]
```

### ifc_state_set_range

Sets the selection range within an element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The textarea or contenteditable element. |
| `start` | `int` | The start position. |
| `end` | `int` | The end position. |

**Return value:** `void`

**Inner mechanisms:**
- **HTMLTextAreaElement:** Sets `selectionStart` and `selectionEnd`.
- **Contenteditable:** Uses `ifc_get_position_in_context()` to convert positions to DOM nodes, creates a `Range`, and applies it to the selection.

**Usage context:** Used by `ifc_state_undo()` to restore cursor position after undo/redo.

```javascript
ifc_state_set_range(textarea, 10, 25);
```

### ifc_state_purge

Clears all undo/redo state from an element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` | The element to purge state from. |

**Return value:** `void`

**Inner mechanisms:**
1. Deletes `ifc_state_undo`, `ifc_state_redo`, `ifc_state_position`, and `ifc_state_nosave` properties from the object.

**Usage context:** Called when switching languages or loading new content to prevent undoing across different content states.

```javascript
ifc_state_purge(textarea);
```

---

## FILE UPLOAD PROGRESS

### ifc_show_upload_progress

Attaches a submit handler to a form that intercepts file uploads and displays progress bars using XMLHttpRequest.

| Parameter | Type | Description |
|-----------|------|-------------|
| `form` | `HTMLFormElement` | The form element to enhance. |

**Return value:** `void`

**Inner mechanisms:**
1. Checks for `INPUT[type=file]` elements in the form. Returns early if none found.
2. Attaches a `submit` event listener (non-capturing) to the form.
3. On submit:
   - Prevents multiple simultaneous uploads via `form.uploading` flag.
   - Creates `<progress>` elements before each file input that has files.
   - Sets `form.uploading = true`.
   - Creates an `XMLHttpRequest` to handle the upload.
   - **Progress tracking:** Updates progress bars based on `e.loaded / e.total`. Shows an alternative animation if no progress for 250ms.
   - **Load handler:** Creates a Blob from the response and navigates to it (avoids `document.write` conflicts).
   - **Error handler:** Removes progress elements and resets the uploading flag.
   - Sends the form data via `XMLHttpRequest.send(new FormData(form))`.
   - Triggers global loading animation via `ifc_loading_event("window_unload")`.
   - Starts the timeout check for progress animation.

**Usage context:** Enhancing forms with file inputs to show upload progress instead of a full page reload.

```javascript
ifc_show_upload_progress(document.getElementById("upload_form"));
```


<!-- HASH:1c905059360df00afac622372680e6a8 -->
