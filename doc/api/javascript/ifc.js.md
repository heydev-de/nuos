# NUOS API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.22.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Interface Controller (IFC) - Core JavaScript Module

The `ifc.js` file provides the core client-side interface controller for the NUOS web platform. It handles form interactions, value management, syntax highlighting, undo/redo functionality, and various UI utilities for web applications. This module is designed to work seamlessly with NUOS's PHP backend, enabling dynamic page updates, form submissions, and interactive elements without full page reloads.

---

## Command Functions

### `ifc_select_page(index, popup = false)`
Switches the current page to a different interface page.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| index     | number  | Index of the target page in the `ifc_page` array                           |
| popup     | boolean | If `true`, loads the page in a new context without resetting the interface |

**Return Value:** None

**Mechanism:**
- Constructs a URL using the base page path and appends an `ifc_message` parameter to trigger the page change on the server
- If `popup` is `false`, resets the current interface state before loading the new page
- Uses `load_page()` to perform the actual navigation

**Usage:**
- Used for navigating between different interface pages while maintaining session state
- Can be triggered from menu items or other navigation elements

---

### `ifc_select_menu(index)`
Executes a menu action associated with a specific index.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| index     | number | Index of the menu item to execute    |

**Return Value:** None

**Mechanism:**
- Checks for a confirmation message associated with the menu item
- If present, displays a confirmation dialog before proceeding
- Executes the JavaScript code associated with the menu item using `new Function()`

**Usage:**
- Handles menu item clicks in the interface
- Supports both simple actions and complex operations with confirmation

---

### `ifc_submit()`
Submits the main interface form after memorizing the current scroll position.

**Return Value:** None

**Mechanism:**
- Calls `ifc_memorize_position()` to store scroll coordinates
- Creates and dispatches a submit event on the "ifc" form
- If the event is not prevented, submits the form programmatically

**Usage:**
- Core form submission mechanism that preserves UI state
- Used whenever form data needs to be sent to the server

---

### `ifc_post(message = "", param = "")`
Prepares and submits the interface form with specific message and parameter values.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| message   | string | Value for the `ifc_message` form field           |
| param     | string | Value for the `ifc_param` form field             |

**Return Value:** None

**Mechanism:**
- Sets the specified values on the form's hidden input fields
- Calls `ifc_submit()` to perform the actual submission

**Usage:**
- Used to send specific commands or data to the server
- Commonly used for interface actions that require server processing

---

### `ifc_cancel(offset = 0)`
Cancels the current operation and resets the interface.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| offset    | number | Index offset for form elements to reset          |

**Return Value:** None

**Mechanism:**
- Resets form elements starting from the specified offset
- Posts an "ifc_cancel" message to the server

**Usage:**
- Provides a standardized way to cancel operations
- Used when users need to abort current actions

---

### `ifc_autopost(object, message = "")`
Automatically submits the form when a specified object changes.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to monitor        |
| message   | string          | Message to post when the element changes         |

**Return Value:** None

**Mechanism:**
- Converts string identifiers to DOM objects if necessary
- Adds a change event listener to the object
- Triggers form submission with the specified message when changes occur

**Usage:**
- Creates auto-submitting form elements
- Useful for filters, search boxes, or other elements that should trigger immediate updates

---

### `ifc_response(value)`
Displays a response message in the interface response area.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| value     | string | HTML content to display in the response area     |

**Return Value:** None

**Mechanism:**
- Locates the "ifc-response" element
- Temporarily hides it, then shows it with the new content using a fade-in animation
- Uses `fx_animation_frame()` for smooth transitions

**Usage:**
- Displays server responses or status messages to the user
- Provides visual feedback for user actions

---

## Value Management Functions

### `ifc_get(object, index = 0)`
Retrieves the value from a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to get value from |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** mixed - The element's value or `false` if no value is available

**Mechanism:**
- Handles different input types differently:
  - Checkboxes/radio buttons: returns value if checked
  - Text-based inputs: returns the value property
  - File inputs: returns the files collection or value
  - Radio button groups: returns the value of the checked button

**Usage:**
- Retrieves form field values in a type-aware manner
- Used throughout the interface for reading user input

---

### `ifc_get_selection(object)`
Gets the currently selected text from a textarea or the document.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object          | Textarea element to get selection from           |

**Return Value:** string - The selected text

**Mechanism:**
- For textareas: returns the substring between selectionStart and selectionEnd
- For other elements: returns the window's current selection

**Usage:**
- Retrieves selected text for operations like formatting or copying
- Used in text editing functions

---

### `ifc_title(object, index = 0)`
Gets the display title/label for a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element                   |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** string - The element's title or label text

**Mechanism:**
- For buttons: returns the text content
- For checkboxes, radios, text inputs: finds the associated label and returns its text
- For select elements: returns the text of the selected option

**Usage:**
- Retrieves human-readable labels for form elements
- Used for displaying element descriptions or generating reports

---

### `ifc_reset(offset = 0)`
Resets all form elements starting from a specified offset.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| offset    | number | Index offset to start resetting from             |

**Return Value:** None

**Mechanism:**
- Iterates through form elements starting from the offset
- Calls `ifc_del()` on each element to clear its value

**Usage:**
- Clears form data, typically before loading new content
- Used to reset the interface state

---

### `ifc_del(object, index, focus = true)`
Clears the value of a form element and optionally focuses it.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to clear          |
| index     | number          | Index of the element if multiple exist           |
| focus     | boolean         | Whether to focus the element after clearing      |

**Return Value:** None

**Mechanism:**
- Handles different input types differently:
  - Checkboxes/radio buttons: unchecks and clears value
  - Text-based inputs: clears the value
  - Select elements: sets selectedIndex to -1
  - Radio button groups: clears all buttons in the group

**Usage:**
- Clears individual form elements
- Used for resetting specific fields or clearing user input

---

### `ifc_set(object, value = "", index = 0)`
Sets the value of a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to set            |
| value     | string          | Value to set on the element                      |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** None

**Mechanism:**
- Handles different input types differently:
  - Most inputs: sets the value property
  - File inputs: skipped for security reasons
  - Radio button groups: checks the button with matching value
- Triggers language reload if the element has a `data-l` attribute

**Usage:**
- Sets form field values programmatically
- Used for initializing forms or updating values based on user actions

---

### `ifc_copy(source, target)`
Copies the value from one form element to another.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| source    | object/string   | Source element or name                           |
| target    | object/string   | Target element or name                           |

**Return Value:** None

**Mechanism:**
- Gets the value from the source using `ifc_get()`
- Sets the value on the target using `ifc_set()`

**Usage:**
- Copies values between form elements
- Used for duplicating data or synchronizing fields

---

### `ifc_select(object, index = 0)`
Selects or focuses a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to select         |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** None

**Mechanism:**
- For checkboxes/radio buttons: triggers a click event
- For text inputs: focuses and selects the text
- For other elements: simply focuses them

**Usage:**
- Programmatically selects form elements
- Used to direct user attention to specific fields

---

### `ifc_limit(object, limit)`
Limits the length of text in a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to limit          |
| limit     | number          | Maximum allowed length                           |

**Return Value:** None

**Mechanism:**
- Gets the current value using `ifc_get()`
- If the value exceeds the limit, truncates it and sets it back

**Usage:**
- Enforces length limits on text inputs
- Used for fields with specific size constraints

---

## List Management Functions

### `ifc_list_activate(name = "list")`
Checks all checkboxes in a named list.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| name      | string | Base name of the list elements                   |

**Return Value:** None

**Mechanism:**
- Finds all input elements with names starting with the specified name
- Clicks each unchecked checkbox to check it

**Usage:**
- Selects all items in a list
- Used for bulk operations on list items

---

### `ifc_list_invert(name = "list")`
Inverts the checked state of all checkboxes in a named list.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| name      | string | Base name of the list elements                   |

**Return Value:** None

**Mechanism:**
- Finds all input elements with names starting with the specified name
- Clicks each checkbox to invert its state

**Usage:**
- Toggles selection state of list items
- Used for bulk selection operations

---

### `ifc_list_deactivate(name = "list")`
Unchecks all checkboxes in a named list.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| name      | string | Base name of the list elements                   |

**Return Value:** None

**Mechanism:**
- Finds all input elements with names starting with the specified name
- Clicks each checked checkbox to uncheck it

**Usage:**
- Clears selection in a list
- Used for bulk deselection operations

---

## Textarea Functions

### `ifc_format(object, index = 0)`
Formats the content of a textarea.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | Textarea element or name                         |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** None

**Mechanism:**
- Calls `ifc_clean()` with the format parameter set to `true`

**Usage:**
- Applies formatting to textarea content
- Used to clean up and standardize text formatting

---

### `ifc_clean(object, index, format = false)`
Cleans and optionally formats textarea content.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | Textarea element or name                         |
| index     | number          | Index of the element if multiple exist           |
| format    | boolean         | Whether to apply additional formatting           |

**Return Value:** None

**Mechanism:**
- Preserves scroll position
- Removes problematic characters (carriage returns, zero-width characters)
- Normalizes whitespace and line breaks
- Applies formatting rules if requested:
  - Replaces multiple spaces or single linebreaks with spaces
  - Reduces multiple linebreaks to two
  - Removes leading/trailing whitespace

**Usage:**
- Cleans up text content in textareas
- Used for normalizing user input or preparing text for processing

---

### `ifc_keydown(event)`
Handles special keyboard events in textareas.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| event     | Event  | The keyboard event                               |

**Return Value:** boolean - Whether to allow the default action

**Mechanism:**
- Handles various keyboard combinations for text editing:
  - Backspace: intelligent line indentation handling
  - Tab: inserts spaces or removes indentation
  - Enter: preserves indentation
  - Home: moves to start of line or indentation
  - Alt+F: formats text
  - Alt+W: cleans text
  - Ctrl+Z/Ctrl+Y: undo/redo operations
- Manages text selection and cursor positioning
- Saves state before making changes

**Usage:**
- Provides advanced text editing capabilities
- Used as an event handler for textarea keydown events

---

## Language Management Functions

### Constants

| Name                     | Value/Default | Description                                      |
|--------------------------|---------------|--------------------------------------------------|
| ifc_language_separator   | String.fromCharCode(31) | Unit separator character used in language strings |
| ifc_language_flag        | false         | Flag to prevent recursive language selection     |

---

### `ifc_language_select_all(language)`
Selects all language links for a specific language.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| language  | string | Language code to select                          |

**Return Value:** boolean - Always returns `false`

**Mechanism:**
- Sets the language flag to prevent recursion
- Finds all language links and triggers their onclick handlers for the specified language
- Resets the language flag when done

**Usage:**
- Changes the interface language globally
- Used when switching languages across the entire application

---

### `ifc_language_select(source, target, language)`
Selects a specific language for a target element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| source    | object/string   | Source element or name                           |
| target    | object/string   | Target element or name                           |
| language  | string          | Language code to select                          |

**Return Value:** boolean - Always returns `false`

**Mechanism:**
- If the target already has the specified language, calls `ifc_language_select_all()`
- Highlights the language selection
- Loads the language content into the target element

**Usage:**
- Changes the language for a specific content element
- Used for multilingual content management

---

### `ifc_language_highlight(id, language)`
Highlights language links for a specific content ID.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| id        | string | Content ID to highlight                          |
| language  | string | Language code to highlight                       |

**Return Value:** None

**Mechanism:**
- Finds all language links for the specified content ID
- Sets their class to "on" for the selected language, "off" for others

**Usage:**
- Visually indicates the current language selection
- Used to show which language is active for a content item

---

### `ifc_language_load(source, target, language, select = true)`
Loads language-specific content from a source to a target.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| source    | object/string   | Source element or name                           |
| target    | object/string   | Target element or name                           |
| language  | string          | Language code to load                            |
| select    | boolean         | Whether to focus the target after loading        |

**Return Value:** None

**Mechanism:**
- Preserves scroll position
- Purges the target's undo/redo state
- Extracts and sets the language-specific content using `ifc_language_get()`
- Optionally focuses the target element

**Usage:**
- Loads translated content into an element
- Used for displaying multilingual content

---

### `ifc_language_reload(source, target)`
Reloads the current language content for a target.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| source    | object/string   | Source element or name                           |
| target    | object/string   | Target element or name                           |

**Return Value:** None

**Mechanism:**
- Gets the current language from the target
- Calls `ifc_language_load()` with the current language

**Usage:**
- Refreshes language content without changing language
- Used when source content has been updated

---

### `ifc_language_save(source, target)`
Saves content from a source to a target's language-specific storage.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| source    | object/string   | Source element or name                           |
| target    | object/string   | Target element or name                           |

**Return Value:** None

**Mechanism:**
- Gets the current language from the source
- Updates the target's value with the source's content for the specific language

**Usage:**
- Saves translated content back to the storage element
- Used when editing multilingual content

---

### `ifc_language_get(text, language)`
Extracts language-specific content from a multilingual string.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| text      | string | Multilingual text string                         |
| language  | string | Language code to extract                         |

**Return Value:** string - The language-specific content

**Mechanism:**
- If language is empty, returns the default content (before any language markers)
- Otherwise, extracts content between the specified language marker and the next marker

**Usage:**
- Retrieves specific language content from a multilingual string
- Used for displaying translated content

---

### `ifc_language_set(text, value, language)`
Sets language-specific content in a multilingual string.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| text      | string | Original multilingual text string                |
| value     | string | Content to set for the specified language        |
| language  | string | Language code to set                             |

**Return Value:** string - The updated multilingual string

**Mechanism:**
- If language is empty, replaces the default content
- Otherwise, either updates existing content for the language or appends new content

**Usage:**
- Updates or adds language-specific content to a multilingual string
- Used when saving translated content

---

## Common Utility Functions

### `ifc_object(name, index = 0, window = this)`
Gets a DOM element by name or ID.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| name      | string          | Element name or ID                               |
| index     | number          | Index of the element if multiple exist           |
| window    | Window          | Window context to search in                      |

**Return Value:** object/HTMLElement - The found element or `null`

**Mechanism:**
- If index is non-negative, gets elements by name and returns the one at the specified index
- Otherwise, gets the element by ID
- Returns `null` if no element is found

**Usage:**
- Retrieves DOM elements in a flexible way
- Used throughout the interface for element access

---

### `ifc_focus(object, index = 0)`
Focuses a form element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | DOM element or name of element to focus          |
| index     | number          | Index of the element if multiple exist           |

**Return Value:** None

**Mechanism:**
- Skips disabled or hidden elements
- Calls the element's focus method

**Usage:**
- Sets focus on form elements programmatically
- Used to direct user attention to specific fields

---

### `ifc_autofocus()`
Automatically focuses the first visible, editable element.

**Return Value:** None

**Mechanism:**
- Finds all contenteditable elements, file inputs, text inputs, and textareas
- Determines which elements are visible in the viewport
- Focuses the first visible element, optionally selecting its content

**Usage:**
- Automatically focuses the appropriate element when a page loads
- Used to improve user experience by reducing the need for manual focusing

---

### `ifc_scroll(object, top, left)`
Restores scroll position in a textarea.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | Textarea element or name                         |
| top       | number          | Vertical scroll position                         |
| left      | number          | Horizontal scroll position                       |

**Return Value:** None

**Mechanism:**
- Uses a timeout to defer the scroll operation
- Calls `_ifc_scroll()` to perform the actual scrolling

**Usage:**
- Restores scroll position after content changes
- Used to maintain user position in textareas

---

### `_ifc_scroll(object, top, left)`
Internal function to set scroll position in a textarea.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Textarea element                                 |
| top       | number | Vertical scroll position                         |
| left      | number | Horizontal scroll position                       |

**Return Value:** None

**Mechanism:**
- Sets the scrollTop and scrollLeft properties directly

**Usage:**
- Internal implementation of scroll position restoration
- Used by `ifc_scroll()`

---

### `ifc_memorize_position()`
Stores the current scroll position in hidden form fields.

**Return Value:** None

**Mechanism:**
- Gets the current scroll position from `fx_scroll_container`
- Stores the values in "ifc_left" and "ifc_top" form fields

**Usage:**
- Preserves scroll position across page reloads
- Used before form submissions to maintain UI state

---

## Loading Animation Functions

### `ifc_loading_event(event)`
Shows a loading animation for specific events.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| event     | string | Event name                                       |

**Return Value:** None

**Mechanism:**
- Only triggers for "window_unload" event
- Creates and appends a loading div if one doesn't exist

**Usage:**
- Displays loading indicators during page transitions
- Used to provide visual feedback during navigation

---

### `_ifc_loading_event()`
Hides the loading animation.

**Return Value:** None

**Mechanism:**
- Calls `fx_visible()` to hide the "ifc-loading" element

**Usage:**
- Hides the loading indicator when operations complete
- Used internally by other functions

---

## Download Function

### `ifc_download(url)`
Initiates a file download from a specified URL.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| url       | string | URL of the file to download                      |

**Return Value:** None

**Mechanism:**
- Creates an invisible anchor element
- Sets its href to the download URL
- Triggers a click event on the anchor
- Removes the anchor after a delay
- Hides the loading animation

**Usage:**
- Initiates file downloads programmatically
- Used for download links or buttons

---

## Custom Select Function

### `ifc_custom_select(object)`
Enhances a custom select element with standard select functionality.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object/string   | Custom select element or name                    |

**Return Value:** None

**Mechanism:**
- Defines properties to mimic standard select element behavior:
  - type: "select-one" or "select-multiple"
  - value: gets/sets the selected value
  - selectedIndex: gets/sets the selected index
  - options: gets an array of option objects
- Adds event listeners for change events
- Handles click events on labels for radio button groups
- Sets CSS custom properties for styling

**Usage:**
- Enhances custom-styled select elements with standard functionality
- Used for creating accessible, custom-styled form controls

---

## Syntax Highlighting Functions

### Constants and Variables

| Name                     | Type   | Description                                      |
|--------------------------|--------|--------------------------------------------------|
| ifc_highlight_php        | array  | Regex and metadata for PHP detection             |
| ifc_highlight_token      | array  | Regex and metadata for token detection           |
| ifc_highlight_detect     | array  | Array of syntax detection rules for different states |

---

### `ifc_highlight_init()`
Initializes the syntax highlighting system.

**Return Value:** None

**Mechanism:**
- Creates a mapping of regex patterns to unique identifiers
- Updates the detection rules with these identifiers for caching

**Usage:**
- Initializes the syntax highlighting system at load time
- Called automatically when the script loads

---

### `ifc_highlight(object, mode = 0, no_insert = false, bounce)`
Applies syntax highlighting to a contenteditable element.

| Parameter | Type            | Description                                      |
|-----------|-----------------|--------------------------------------------------|
| object    | object          | Contenteditable element to highlight             |
| mode      | number          | Initial highlighting mode                        |
| no_insert | boolean/number  | Flags for skipping certain elements              |
| bounce    | boolean         | Whether this is a debounced call                 |

**Return Value:** None

**Mechanism:**
- Implements debouncing to prevent excessive highlighting
- Uses a state machine to parse the content according to language rules
- Applies CSS classes to different syntax elements
- Handles bracket matching for programming languages
- Preserves cursor position and scroll position
- Supports multiple languages: HTML, CSS, JavaScript, PHP, and custom format syntax

**Usage:**
- Applies syntax highlighting to code editors
- Used for displaying formatted code in the interface

---

### `ifc_highlight_bracket(object)`
Highlights matching brackets in a highlighted element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Contenteditable element with highlighted content |

**Return Value:** None

**Mechanism:**
- Finds the current cursor position
- Locates the nearest bracket
- Highlights matching opening/closing brackets

**Usage:**
- Provides visual feedback for bracket matching
- Used in code editors to help with code structure

---

### `ifc_save_selection(context)`
Saves the current text selection in a contenteditable element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| context   | object | Contenteditable element                          |

**Return Value:** function - A function to restore the selection

**Mechanism:**
- Captures the current selection range
- Calculates the character positions of the selection
- Returns a function that can restore the selection

**Usage:**
- Preserves cursor position during content changes
- Used by syntax highlighting to maintain user position

---

### `ifc_get_position_in_context(context, position)`
Gets the DOM position corresponding to a character position.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| context   | object | Contenteditable element                          |
| position  | number | Character position                               |

**Return Value:** object - An object with node and position properties

**Mechanism:**
- Uses a TreeWalker to find the text node at the specified position
- Returns the node and character offset within that node

**Usage:**
- Converts character positions to DOM positions
- Used for cursor positioning in contenteditable elements

---

### `ifc_contenteditable_init(id)`
Initializes a contenteditable element with textarea-like properties.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| id        | string | ID of the contenteditable element                |

**Return Value:** None

**Mechanism:**
- Adds a placeholder element
- Defines properties to mimic textarea behavior:
  - type: always "textarea"
  - value: gets/sets textContent
  - select: selects all content
- Adds event listeners for bracket highlighting

**Usage:**
- Enhances contenteditable elements with textarea-like functionality
- Used for creating rich text editors

---

## Undo/Redo Functions

### `ifc_state_save(object)`
Saves the current state of an editable element for undo/redo.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Editable element (textarea or contenteditable)   |

**Return Value:** None

**Mechanism:**
- Skips if the element has the "ifc_state_nosave" property
- Initializes state arrays if they don't exist
- Gets the current selection range
- Only saves if the content or position has changed significantly
- Limits the undo stack to 100 items

**Usage:**
- Saves state before content changes
- Used to enable undo functionality

---

### `ifc_state_undo(object, redo = false)`
Undoes or redoes the last change to an editable element.

| Parameter | Type    | Description                                      |
|-----------|---------|--------------------------------------------------|
| object    | object  | Editable element                                 |
| redo      | boolean | Whether to perform a redo instead of undo        |

**Return Value:** None

**Mechanism:**
- Moves the current state between undo and redo stacks
- Restores the content and selection range
- Prevents recursive state saving during restoration

**Usage:**
- Implements undo/redo functionality
- Used for Ctrl+Z/Ctrl+Y keyboard shortcuts

---

### `ifc_state_redo(object)`
Redoes the last undone change to an editable element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Editable element                                 |

**Return Value:** None

**Mechanism:**
- Calls `ifc_state_undo()` with redo set to `true`

**Usage:**
- Implements redo functionality
- Used for Ctrl+Y keyboard shortcut

---

### `ifc_state_get_range(object)`
Gets the current selection range in an editable element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Editable element                                 |

**Return Value:** array - [start, end] character positions

**Mechanism:**
- For textareas: returns selectionStart and selectionEnd
- For contenteditable elements: calculates character positions from DOM ranges

**Usage:**
- Captures selection state for undo/redo
- Used internally by state management functions

---

### `ifc_state_set_range(object, start, end)`
Sets the selection range in an editable element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Editable element                                 |
| start     | number | Start character position                         |
| end       | number | End character position                           |

**Return Value:** None

**Mechanism:**
- For textareas: sets selectionStart and selectionEnd
- For contenteditable elements: converts character positions to DOM positions and sets the range

**Usage:**
- Restores selection state for undo/redo
- Used internally by state management functions

---

### `ifc_state_purge(object)`
Clears the undo/redo state for an editable element.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| object    | object | Editable element                                 |

**Return Value:** None

**Mechanism:**
- Removes all state-related properties from the element

**Usage:**
- Clears undo history when content is reloaded
- Used to prevent undo/redo across content changes

---

## File Upload Progress Function

### `ifc_show_upload_progress(form)`
Enhances a form with file upload progress indicators.

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| form      | object | Form element to enhance                          |

**Return Value:** None

**Mechanism:**
- Checks for file inputs in the form
- Adds progress elements next to active file inputs
- Overrides form submission to use XMLHttpRequest
- Displays upload progress and handles timeouts
- Redirects to the response page when complete

**Usage:**
- Provides visual feedback during file uploads
- Used for forms that handle file uploads


<!-- HASH:f7ff5fcb4e54c5f52154b58def36b089 -->
