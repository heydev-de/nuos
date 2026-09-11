# PWNC API Documentation

[← Index](../README.md) | [`javascript/content.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/content.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## content.js

The `content.js` file is a JavaScript module that provides a set of utility functions for managing content editing operations within the PWNC Web Platform. These functions handle tasks such as loading pages, executing commands with confirmation prompts, prompting users for input with validation, copying and pasting content, and restoring view state after navigation.

The module relies on several global variables and external functions:

| Variable | Description |
|----------|-------------|
| `this.name` | A unique identifier for the current editing session, generated randomly if not already set. |
| `content_buffer` | Stores a range object used for copy/paste operations. |

### content_load

#### Description
Navigates the parent window to a specified URL by replacing the current location.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to navigate to. |

#### Return Value
None.

#### Inner Mechanism
Uses `parent.location.replace(url)` to change the parent frame's URL without adding a new entry to the browser history.

#### Usage Example
```javascript
content_load("/admin/pages/edit/123");
```
This navigates the parent window to the edit page for content item 123.

---

### content_edit_open

#### Description
Opens a content editing interface by loading a specific page.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL of the editing interface. |

#### Return Value
None.

#### Inner Mechanism
Calls the global `load_page(url)` function to load the editing interface.

#### Usage Example
```javascript
content_edit_open("/admin/content/edit?id=456");
```
Loads the content editor for item 456.

---

### content_edit_command

#### Description
Executes a command at a given URL, optionally prompting the user for confirmation. It also supports placeholder substitution for positioning values.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `url` | `string` | — | The command URL, potentially containing `%left%` and `%top%` placeholders. |
| `text` | `string` | `""` | Confirmation message shown to the user before proceeding. |

#### Return Value
None.

#### Inner Mechanism
If `text` is non-empty, it shows a confirmation dialog. If confirmed, it replaces `%left%` and `%top%` placeholders in the URL with the results of `fx_position_left()` and `fx_position_top()`, then navigates using `location.replace()`.

#### Usage Example
```javascript
content_edit_command("/admin/content/delete?id=789", "Are you sure you want to delete this item?");
```
Prompts the user for confirmation and, if confirmed, deletes the item.

---

### content_edit_prompt

#### Description
Prompts the user for input, validates it against a regular expression, and then executes a command with the validated value substituted into the URL.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `url` | `string` | — | The command URL, potentially containing `%return%` placeholder. |
| `text` | `string` | — | Prompt message displayed to the user. |
| `value` | `string` | — | Default value shown in the prompt. |
| `pattern` | `RegExp` | — | Regular expression used to validate the user's input. |
| `negate` | `boolean` | `false` | If true, negates the returned value (e.g., converts to negative number). |

#### Return Value
None.

#### Inner Mechanism
Displays a prompt dialog. If the user cancels, it returns early. If the input doesn't match the pattern, it alerts an error message (`CMS_L_MOD_CONTENT_013`). Otherwise, it calls `content_edit_command()` with the `%return%` placeholder replaced by the validated value (or its negation).

#### Usage Example
```javascript
content_edit_prompt("/admin/content/move?pos=%return%", "Enter position:", 1, /^[0-9]+$/);
```
Prompts the user for a numeric position and moves the content accordingly.

---

### content_edit_copy

#### Description
Sends a copy request to the server and stores a range object in the content buffer for later use.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to send the copy request to. |
| `range` | `object` | A range object representing the selected content. |

#### Return Value
None.

#### Inner Mechanism
Calls `asr_send(url)` to send the copy request asynchronously, and stores the `range` in the global `content_buffer` variable.

#### Usage Example
```javascript
content_edit_copy("/admin/content/copy?id=101", selectedRange);
```
Copies the selected content and stores the range for potential paste operations.

---

### content_edit_paste

#### Description
Executes a paste command after confirming with the user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to execute the paste command at. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_command()` with a confirmation message (`CMS_L_COMMAND_PASTE`).

#### Usage Example
```javascript
content_edit_paste("/admin/content/paste?id=101");
```
Prompts the user to confirm pasting and executes the paste command.

---

### content_edit_swap

#### Description
Executes a swap command if there is content in the buffer; otherwise, alerts the user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to execute the swap command at. |

#### Return Value
None.

#### Inner Mechanism
Checks if `content_buffer` is set. If so, calls `content_edit_command()` with a confirmation message (`CMS_L_MOD_CONTENT_005`). Otherwise, alerts the user (`CMS_L_MOD_CONTENT_002`).

#### Usage Example
```javascript
content_edit_swap("/admin/content/swap?id=101");
```
Swaps the current content with the buffered content, if available.

---

### content_edit_kick1

#### Description
Prompts the user for a numeric value and executes a command with that value.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The command URL, potentially containing `%return%` placeholder. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_prompt()` with a prompt message (`CMS_L_MOD_CONTENT_011`), default value `1`, and a numeric-only validation pattern.

#### Usage Example
```javascript
content_edit_kick1("/admin/content/kick?by=%return%");
```
Prompts the user for a number and kicks content by that amount.

---

### content_edit_kick2

#### Description
Similar to `content_edit_kick1`, but negates the user-provided value.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The command URL, potentially containing `%return%` placeholder. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_prompt()` with a prompt message (`CMS_L_MOD_CONTENT_012`), default value `1`, a numeric-only validation pattern, and `negate=true`.

#### Usage Example
```javascript
content_edit_kick2("/admin/content/kick?by=%return%");
```
Prompts the user for a number and kicks content by the negative of that amount.

---

### content_edit_clear

#### Description
Executes a clear/delete command after confirming with the user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to execute the clear command at. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_command()` with a confirmation message (`CMS_L_COMMAND_DELETE`).

#### Usage Example
```javascript
content_edit_clear("/admin/content/clear?id=101");
```
Prompts the user to confirm clearing and executes the command.

---

### content_edit_repeat

#### Description
Prompts the user for a numeric value and executes a repeat command with that value.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The command URL, potentially containing `%return%` placeholder. |
| `value` | `string` | Default value shown in the prompt. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_prompt()` with a prompt message (`CMS_L_MOD_CONTENT_009`), the provided default value, and a numeric-only validation pattern.

#### Usage Example
```javascript
content_edit_repeat("/admin/content/repeat?times=%return%", 3);
```
Prompts the user for a number and repeats the content that many times.

---

### content_edit_shift

#### Description
Prompts the user for a numeric value (including negative numbers) and executes a shift command.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The command URL, potentially containing `%return%` placeholder. |
| `value` | `string` | Default value shown in the prompt. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_prompt()` with a prompt message (`CMS_L_MOD_CONTENT_010`), the provided default value, and a pattern allowing optional negative numbers.

#### Usage Example
```javascript
content_edit_shift("/admin/content/shift?by=%return%", 0);
```
Prompts the user for a number (positive or negative) and shifts the content accordingly.

---

### content_edit_switch

#### Description
Toggles a boolean-like value in the command URL.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The command URL, potentially containing `%return%` placeholder. |
| `value` | `string` | Current value; if empty, sets to "1", otherwise sets to empty. |

#### Return Value
None.

#### Inner Mechanism
Replaces `%return%` in the URL with `"1"` if `value` is empty, or with an empty string otherwise. Then calls `content_edit_command()`.

#### Usage Example
```javascript
content_edit_switch("/admin/content/toggle?state=%return%", "");
```
Toggles the state to "1" if currently empty.

---

### content_edit_apply

#### Description
Executes an apply command after confirming with the user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to execute the apply command at. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_command()` with a confirmation message (`CMS_L_MOD_CONTENT_003`).

#### Usage Example
```javascript
content_edit_apply("/admin/content/apply?id=101");
```
Prompts the user to confirm applying changes and executes the command.

---

### content_edit_revert

#### Description
Executes a revert command after confirming with the user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `url` | `string` | The URL to execute the revert command at. |

#### Return Value
None.

#### Inner Mechanism
Calls `content_edit_command()` with a confirmation message (`CMS_L_MOD_CONTENT_004`).

#### Usage Example
```javascript
content_edit_revert("/admin/content/revert?id=101");
```
Prompts the user to confirm reverting changes and executes the command.

---

### content_edit_restore

#### Description
Restores visual styles and scroll position after a page navigation, optionally highlighting edited elements.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `left` | `number` | Horizontal scroll position as a percentage (0–100). |
| `top` | `number` | Vertical scroll position as a percentage (0–100). |
| `selector` | `string` | CSS selector for elements to highlight with the `tp-edited` class. |

#### Return Value
None.

#### Inner Mechanism
Defines a callback function `func` that:
- Resets opacity and pointer-events on the document element.
- Highlights elements matching the `selector` by adding the `tp-edited` class.

If the current URL contains a hash (`#`), it listens for the `pageshow` event and calls `func` directly. Otherwise, it:
- Saves the current `scroll-behavior` style.
- Sets `scroll-behavior` to `auto`.
- Listens for `pageshow` to:
  - Update window size.
  - Scroll to the calculated position based on `left` and `top` percentages.
  - Restore the original `scroll-behavior`.
  - Call `func`.

#### Usage Example
```javascript
content_edit_restore(50, 25, ".content-item");
```
After navigation, scrolls to 50% horizontally and 25% vertically, and highlights all `.content-item` elements.


<!-- HASH:03656ca21ad13ad3fdfceea2d5b13da8 -->
