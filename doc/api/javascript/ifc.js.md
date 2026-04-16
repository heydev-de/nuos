# NUOS API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Interface Controller (IFC) Module

The `ifc.js` file provides a comprehensive JavaScript interface controller for the NUOS web platform. It handles form interactions, content manipulation, syntax highlighting, and multi-language support within the platform's integrated development environment.

---

## Command Functions

### `ifc_select_page(index, popup = false)`
Switches the current page to a different interface page.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| index     | number  | Index of the page in the `ifc_page` array to navigate to.                  |
| popup     | boolean | If `true`, loads the page in a new context without resetting the interface. |

**Return Value:** None.

**Mechanism:**
- Constructs a URL using the base path from `ifc_page[0]` and appends the target page from `ifc_page[index]`.
- If `popup` is `false`, resets the interface state before loading the new page.

**Usage:**
- Used for navigating between different interface pages while maintaining or resetting the current state.

---

### `ifc_select_menu(index)`
Executes a menu action defined in the `ifc_menu` array.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| index     | number | Index of the menu action to execute.     |

**Return Value:** None.

**Mechanism:**
- Checks for a confirmation message in `ifc_menu[0][index]` and prompts the user if present.
- Executes the corresponding function from `ifc_menu[1][index]` using `new Function()`.

**Usage:**
- Used for executing menu-driven actions, such as saving, loading, or navigating.

---

### `ifc_submit()`
Submits the primary interface form (`ifc`) programmatically.

**Return Value:** None.

**Mechanism:**
- Memorizes the current scroll position.
- Dispatches a `submit` event on the form to trigger event listeners.
- Submits the form if the event is not canceled.

**Usage:**
- Used to programmatically submit the form after modifications or validations.

---

### `ifc_post(message = "", param = "")`
Posts a message and parameter to the server via the interface form.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| message   | string | Value for the `ifc_message` form field.  |
| param     | string | Value for the `ifc_param` form field.    |

**Return Value:** None.

**Mechanism:**
- Sets the `ifc_message` and `ifc_param` fields in the form.
- Calls `ifc_submit()` to submit the form.

**Usage:**
- Used for sending specific commands or parameters to the server.

---

### `ifc_cancel(offset = 0)`
Cancels the current operation and resets the interface.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| offset    | number | Index offset for resetting form fields.  |

**Return Value:** None.

**Mechanism:**
- Resets the form fields starting from the given offset.
- Posts an `ifc_cancel` message to the server.

**Usage:**
- Used to cancel ongoing operations and return to a clean state.

---

### `ifc_autopost(object, message = "")`
Automatically submits the form when a specified object's value changes.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| message   | string          | Message to post on change.               |

**Return Value:** None.

**Mechanism:**
- Listens for the `change` event on the object and posts the specified message when triggered.

**Usage:**
- Used for auto-saving or auto-submitting forms when specific fields change.

---

### `ifc_response(value)`
Displays a response message in the `ifc-response` element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| value     | string | HTML content to display in the response. |

**Return Value:** None.

**Mechanism:**
- Finds the `ifc-response` element and toggles its visibility to display the provided content.

**Usage:**
- Used to show feedback or responses from server actions.

---

## Value Functions

### `ifc_get(object, index = 0)`
Retrieves the value of a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** Mixed (string, boolean, FileList)
- Returns the value of the form element or `false` if no value is found.

**Mechanism:**
- Handles different input types (checkbox, radio, text, file, etc.) and returns the appropriate value.

**Usage:**
- Used to retrieve values from form elements for processing or validation.

---

### `ifc_get_selection(object)`
Retrieves the currently selected text from a textarea or the document.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object          | DOM object (textarea).                   |

**Return Value:** string
- The selected text.

**Mechanism:**
- For textareas, returns the substring between `selectionStart` and `selectionEnd`.
- For other elements, uses `window.getSelection()`.

**Usage:**
- Used to retrieve selected text for manipulation or formatting.

---

### `ifc_title(object, index = 0)`
Retrieves the title or label of a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** string
- The title or label of the element.

**Mechanism:**
- Handles different input types (button, checkbox, select, etc.) and retrieves the associated label or text.

**Usage:**
- Used to display or log the title of form elements.

---

### `ifc_reset(offset = 0)`
Resets all form elements starting from the given offset.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| offset    | number | Index offset for resetting form fields.  |

**Return Value:** None.

**Mechanism:**
- Iterates through form elements and resets their values using `ifc_del()`.

**Usage:**
- Used to clear the form or specific fields.

---

### `ifc_del(object, index, focus = true)`
Clears the value of a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |
| focus     | boolean         | If `true`, focuses the element after clearing. |

**Return Value:** None.

**Mechanism:**
- Handles different input types (checkbox, radio, text, select, etc.) and clears their values.

**Usage:**
- Used to reset individual form elements.

---

### `ifc_set(object, value = "", index = 0)`
Sets the value of a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| value     | string          | Value to set.                            |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** None.

**Mechanism:**
- Handles different input types (checkbox, radio, text, select, etc.) and sets their values.
- Triggers language reload if the element has a `data-l` attribute.

**Usage:**
- Used to update form element values programmatically.

---

### `ifc_copy(source, target)`
Copies the value from one form element to another.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| source    | object/string   | Source object or name.                   |
| target    | object/string   | Target object or name.                   |

**Return Value:** None.

**Mechanism:**
- Retrieves the value from the source using `ifc_get()` and sets it on the target using `ifc_set()`.

**Usage:**
- Used to duplicate values between form elements.

---

### `ifc_select(object, index = 0)`
Selects or focuses a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** None.

**Mechanism:**
- Handles different input types (checkbox, radio, text, textarea) and selects or focuses them.

**Usage:**
- Used to programmatically select or focus form elements.

---

### `ifc_limit(object, limit)`
Limits the length of a form element's value.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| limit     | number          | Maximum allowed length of the value.     |

**Return Value:** None.

**Mechanism:**
- Truncates the value of the object if it exceeds the specified limit.

**Usage:**
- Used to enforce length limits on form fields.

---

## List Functions

### `ifc_list_activate(name = "list")`
Activates all checkboxes in a list.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| name      | string | Name prefix of the list elements.        |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names matching the pattern `name[]` and clicks them if not already checked.

**Usage:**
- Used to select all items in a list.

---

### `ifc_list_invert(name = "list")`
Inverts the selection of all checkboxes in a list.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| name      | string | Name prefix of the list elements.        |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names matching the pattern `name[]` and toggles their checked state.

**Usage:**
- Used to invert the selection of items in a list.

---

### `ifc_list_deactivate(name = "list")`
Deactivates all checkboxes in a list.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| name      | string | Name prefix of the list elements.        |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names matching the pattern `name[]` and clicks them if checked.

**Usage:**
- Used to deselect all items in a list.

---

## Textarea Functions

### `ifc_format(object, index = 0)`
Formats the content of a textarea.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** None.

**Mechanism:**
- Calls `ifc_clean()` with the `format` parameter set to `true`.

**Usage:**
- Used to clean and format textarea content.

---

### `ifc_clean(object, index, format = false)`
Cleans the content of a textarea.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |
| format    | boolean         | If `true`, applies additional formatting. |

**Return Value:** None.

**Mechanism:**
- Removes unwanted characters (carriage returns, zero-width characters, etc.).
- Replaces hard spaces, line separators, and tabs with standard equivalents.
- Applies formatting rules if `format` is `true`.

**Usage:**
- Used to sanitize and normalize textarea content.

---

### `ifc_keydown(event)`
Handles keydown events for textareas to provide advanced editing features.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| event     | object | The keydown event object.                |

**Return Value:** boolean
- Returns `false` to prevent default action for specific keys, otherwise `true`.

**Mechanism:**
- Handles special key combinations (e.g., Alt+F for formatting, Ctrl+Z for undo).
- Manages indentation, line breaks, and selection behavior.

**Usage:**
- Used to enhance textarea editing with custom keybindings and behaviors.

---

## Language Functions

### `ifc_language_separator`
**Constant:** `String.fromCharCode(31)`
- Unit separator character used to delimit language-specific content.

### `ifc_language_flag`
**Variable:** `false`
- Flag to indicate if a language selection is in progress.

---

### `ifc_language_select_all(language)`
Selects all language links for a specific language.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| language  | string | Language code to select.                 |

**Return Value:** boolean
- Returns `false` to prevent default action.

**Mechanism:**
- Iterates through all links with class `language-*` and triggers their `onclick` event if they match the specified language.

**Usage:**
- Used to switch all language-specific content to a new language.

---

### `ifc_language_select(source, target, language)`
Selects a specific language for a target element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| source    | object/string   | Source object or name.                   |
| target    | object/string   | Target object or name.                   |
| language  | string          | Language code to select.                 |

**Return Value:** boolean
- Returns `false` to prevent default action.

**Mechanism:**
- Highlights the selected language and loads the corresponding content into the target element.

**Usage:**
- Used to switch the language of a specific form element.

---

### `ifc_language_highlight(id, language)`
Highlights the selected language for a group of language links.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| id        | string | ID prefix of the language links.         |
| language  | string | Language code to highlight.              |

**Return Value:** None.

**Mechanism:**
- Iterates through all links with IDs matching the pattern `id:language` and updates their class to reflect the selected language.

**Usage:**
- Used to visually indicate the currently selected language.

---

### `ifc_language_load(source, target, language, select = true)`
Loads language-specific content into a target element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| source    | object/string   | Source object or name.                   |
| target    | object/string   | Target object or name.                   |
| language  | string          | Language code to load.                   |
| select    | boolean         | If `true`, focuses the target element.   |

**Return Value:** None.

**Mechanism:**
- Retrieves the language-specific content from the source using `ifc_language_get()` and sets it on the target.
- Preserves scroll position and focuses the target if `select` is `true`.

**Usage:**
- Used to load language-specific content into form elements.

---

### `ifc_language_reload(source, target)`
Reloads the current language content into a target element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| source    | object/string   | Source object or name.                   |
| target    | object/string   | Target object or name.                   |

**Return Value:** None.

**Mechanism:**
- Retrieves the current language from the target and reloads the content using `ifc_language_load()`.

**Usage:**
- Used to refresh language-specific content.

---

### `ifc_language_save(source, target)`
Saves the current language content from a source to a target.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| source    | object/string   | Source object or name.                   |
| target    | object/string   | Target object or name.                   |

**Return Value:** None.

**Mechanism:**
- Retrieves the current language from the source and saves the content to the target using `ifc_language_set()`.

**Usage:**
- Used to update the target with the source's language-specific content.

---

### `ifc_language_get(text, language)`
Retrieves language-specific content from a multi-language string.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| text      | string | Multi-language string.                   |
| language  | string | Language code to retrieve.               |

**Return Value:** string
- The language-specific content.

**Mechanism:**
- Parses the multi-language string to extract the content for the specified language.

**Usage:**
- Used to extract language-specific content from a combined string.

---

### `ifc_language_set(text, value, language)`
Sets language-specific content in a multi-language string.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| text      | string | Original multi-language string.          |
| value     | string | Content to set for the specified language. |
| language  | string | Language code to set.                    |

**Return Value:** string
- The updated multi-language string.

**Mechanism:**
- Updates or appends the language-specific content in the multi-language string.

**Usage:**
- Used to update or add language-specific content to a combined string.

---

## Common Functions

### `ifc_object(name, index = 0, window = this)`
Retrieves a DOM object by name or ID.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| name      | string          | Name or ID of the object.                |
| index     | number          | Index of the object if multiple exist.   |
| window    | object          | Window context to search within.         |

**Return Value:** object/NULL
- The DOM object or `null` if not found.

**Mechanism:**
- Searches for the object by name (using `getElementsByName`) or ID (using `getElementById`).

**Usage:**
- Used to retrieve DOM objects for manipulation.

---

### `ifc_focus(object, index = 0)`
Focuses a form element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| index     | number          | Index of the object if multiple exist.   |

**Return Value:** None.

**Mechanism:**
- Focuses the object if it is not disabled or hidden.

**Usage:**
- Used to programmatically focus form elements.

---

### `ifc_autofocus()`
Automatically focuses the first visible and editable form element.

**Return Value:** None.

**Mechanism:**
- Selects all editable elements (input, textarea, contenteditable) and focuses the first visible one.

**Usage:**
- Used to set initial focus on page load.

---

### `ifc_scroll(object, top, left)`
Scrolls a textarea to a specific position.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |
| top       | number          | Vertical scroll position.                |
| left      | number          | Horizontal scroll position.              |

**Return Value:** None.

**Mechanism:**
- Uses a timeout to scroll the textarea to the specified position.

**Usage:**
- Used to restore scroll position after content updates.

---

### `_ifc_scroll(object, top, left)`
Internal function to scroll a textarea to a specific position.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea).                   |
| top       | number | Vertical scroll position.                |
| left      | number | Horizontal scroll position.              |

**Return Value:** None.

**Mechanism:**
- Directly sets the `scrollTop` and `scrollLeft` properties of the textarea.

**Usage:**
- Used internally by `ifc_scroll()`.

---

### `ifc_memorize_position()`
Memorizes the current scroll position of the main scroll container.

**Return Value:** None.

**Mechanism:**
- Stores the current scroll position in hidden form fields (`ifc_left` and `ifc_top`).

**Usage:**
- Used to preserve scroll position across page reloads.

---

## Loading Animation Functions

### `ifc_loading_event(event)`
Displays a loading animation during page unload.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| event     | string | Event type (e.g., `window_unload`).      |

**Return Value:** None.

**Mechanism:**
- Creates and appends a loading animation element if the event is `window_unload`.

**Usage:**
- Used to indicate loading during page transitions.

---

### `_ifc_loading_event()`
Hides the loading animation.

**Return Value:** None.

**Mechanism:**
- Hides the `ifc-loading` element.

**Usage:**
- Used to remove the loading animation after completion.

---

## Download Function

### `ifc_download(url)`
Triggers a file download from a given URL.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| url       | string | URL of the file to download.             |

**Return Value:** None.

**Mechanism:**
- Creates a hidden anchor element, sets its `href` to the URL, and triggers a click event.

**Usage:**
- Used to initiate file downloads programmatically.

---

## Custom Select Function

### `ifc_custom_select(object)`
Enhances a custom select element with additional properties and behaviors.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object/string   | DOM object or name of the object.        |

**Return Value:** None.

**Mechanism:**
- Defines custom properties (`type`, `value`, `selectedIndex`, `options`) for the select element.
- Adds event listeners for change events and click interactions.
- Supports custom styling via the `data-size` attribute.

**Usage:**
- Used to create accessible and customizable select elements.

---

## Syntax Highlighting Functions

### `ifc_highlight_php`
**Constant:** `[ /<\?(?:php\s|=)/i, 30, "php", 1 ]`
- Regex and state information for PHP code detection.

### `ifc_highlight_token`
**Constant:** `[ /(?:^|[^\\])(%%(?:%(?!%)|[^%\s])*)/, 50, "token", 2 ]`
- Regex and state information for token detection.

### `ifc_highlight_detect`
**Array:** Syntax highlighting rules for different languages and states.
- Contains rules for HTML, CSS, JavaScript, PHP, and custom format detection.

---

### `ifc_highlight_init()`
Initializes the syntax highlighting detection rules.

**Return Value:** None.

**Mechanism:**
- Caches regex patterns to optimize performance.

**Usage:**
- Called automatically on script load.

---

### `ifc_highlight(object, mode = 0, no_insert = false, bounce = false)`
Applies syntax highlighting to a textarea or contenteditable element.

| Parameter | Type            | Description                              |
|-----------|-----------------|------------------------------------------|
| object    | object          | DOM object to highlight.                 |
| mode      | number          | Initial highlighting mode (default: 0).  |
| no_insert | boolean/number  | Flags to skip specific insertions.       |
| bounce    | boolean         | If `true`, applies debouncing.           |

**Return Value:** None.

**Mechanism:**
- Parses the content of the object and applies syntax highlighting based on the current mode.
- Uses debouncing to optimize performance.
- Preserves cursor position and scroll state.

**Usage:**
- Used to highlight code in textareas or contenteditable elements.

---

### `ifc_save_selection(context)`
Saves the current selection in a contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| context   | object | DOM object (contenteditable).            |

**Return Value:** function
- A function to restore the selection.

**Mechanism:**
- Saves the current selection range and returns a function to restore it.

**Usage:**
- Used internally by `ifc_highlight()` to preserve cursor position.

---

### `ifc_get_position_in_context(context, position)`
Retrieves the DOM node and offset for a given text position in a contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| context   | object | DOM object (contenteditable).            |
| position  | number | Text position to locate.                 |

**Return Value:** object
- An object with `node` and `position` properties.

**Mechanism:**
- Uses a `TreeWalker` to locate the node and offset corresponding to the text position.

**Usage:**
- Used internally by `ifc_save_selection()` and `ifc_state_set_range()`.

---

### `ifc_contenteditable_init(id)`
Initializes a contenteditable element to behave like a textarea.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| id        | string | ID of the contenteditable element.       |

**Return Value:** None.

**Mechanism:**
- Defines custom properties (`type`, `value`, `select`) for the element.
- Adds a placeholder element for styling.

**Usage:**
- Used to enhance contenteditable elements with textarea-like behavior.

---

## Undo/Redo Functions

### `ifc_state_save(object)`
Saves the current state of a textarea or contenteditable element for undo/redo.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |

**Return Value:** None.

**Mechanism:**
- Stores the current value and selection range in the `ifc_state_undo` array.
- Limits the undo stack to 100 states.

**Usage:**
- Used to enable undo/redo functionality.

---

### `ifc_state_undo(object, redo = false)`
Undoes or redoes the last action on a textarea or contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |
| redo      | boolean | If `true`, performs a redo.              |

**Return Value:** None.

**Mechanism:**
- Moves the current state between the undo and redo stacks.
- Restores the previous value and selection range.

**Usage:**
- Used to implement undo/redo functionality.

---

### `ifc_state_redo(object)`
Redoes the last undone action on a textarea or contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |

**Return Value:** None.

**Mechanism:**
- Calls `ifc_state_undo()` with `redo` set to `true`.

**Usage:**
- Used to redo previously undone actions.

---

### `ifc_state_get_range(object)`
Retrieves the current selection range of a textarea or contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |

**Return Value:** array
- An array with `[start, end]` positions.

**Mechanism:**
- For textareas, uses `selectionStart` and `selectionEnd`.
- For contenteditable elements, calculates the range using `TreeWalker`.

**Usage:**
- Used internally by `ifc_state_save()` and `ifc_state_undo()`.

---

### `ifc_state_set_range(object, start, end)`
Sets the selection range of a textarea or contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |
| start     | number | Start position of the selection.         |
| end       | number | End position of the selection.           |

**Return Value:** None.

**Mechanism:**
- For textareas, sets `selectionStart` and `selectionEnd`.
- For contenteditable elements, uses `ifc_get_position_in_context()` to set the range.

**Usage:**
- Used internally by `ifc_state_undo()`.

---

### `ifc_state_purge(object)`
Purges the undo/redo state of a textarea or contenteditable element.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| object    | object | DOM object (textarea or contenteditable). |

**Return Value:** None.

**Mechanism:**
- Removes all undo/redo state properties from the object.

**Usage:**
- Used to clear the undo/redo history.

---

## File Upload Progress Function

### `ifc_show_upload_progress(form)`
Displays upload progress for file inputs in a form.

| Parameter | Type   | Description                              |
|-----------|--------|------------------------------------------|
| form      | object | The form element containing file inputs. |

**Return Value:** None.

**Mechanism:**
- Listens for the `submit` event on the form.
- Overrides the default submission to display progress using `XMLHttpRequest`.
- Shows a progress bar for each file input.

**Usage:**
- Used to provide visual feedback during file uploads.


<!-- HASH:f116da22f377da48a56368ef5b79db81 -->
