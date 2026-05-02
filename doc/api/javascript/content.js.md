# NUOS API Documentation

[← Index](../README.md) | [`javascript/content.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Content Editing Module (`content.js`)

This module provides client-side JavaScript functions for managing content editing operations in the NUOS web platform. It handles common content manipulation tasks such as copying, pasting, swapping, deleting, and repositioning content elements. The module integrates with the platform's backend via URL-based commands and provides user confirmation dialogs for destructive actions.

---

### Global Variables

| Name              | Value/Default                     | Description                                                                 |
|-------------------|-----------------------------------|-----------------------------------------------------------------------------|
| `this.name`       | `"content_edit_" + randomNumber`  | Unique identifier for the current content editing context.                  |
| `content_buffer`  | `null`                            | Global buffer storing content ranges for copy/paste operations.            |

---

### Functions

---

#### `content_edit_open(url)`
Opens a content editing interface in the current window.

**Parameters**

| Name  | Type     | Description                          |
|-------|----------|--------------------------------------|
| `url` | `string` | URL of the content editing interface.|

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `load_page(url)` to navigate to the provided URL.

**Usage Context**
- Used to launch content editing interfaces from administrative dashboards or inline editing tools.

---

#### `content_edit_command(url, text = "")`
Executes a content editing command after optional user confirmation.

**Parameters**

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `url`   | `string`  | Command URL with placeholders (`%left%`, `%top%`, `%return%`) for parameters.|
| `text`  | `string`  | Optional confirmation message. If empty, no confirmation is requested.      |

**Return Value**
- `void`

**Inner Mechanisms**
- If `text` is non-empty, displays a confirmation dialog.
- Replaces `%left%` and `%top%` in the URL with the current window's screen coordinates using `fx_position_left()` and `fx_position_top()`.
- Navigates to the resolved URL using `location.replace(url)`.

**Usage Context**
- Core function used by all command-based editing actions (e.g., delete, paste, apply).
- Ensures user confirmation for destructive or high-impact operations.

---

#### `content_edit_copy(url, range)`
Copies a content range to the global buffer and notifies the server.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to notify the server of the copy operation.  |
| `range` | `any`     | Content range or element to be copied.           |

**Return Value**
- `void`

**Inner Mechanisms**
- Sends an asynchronous request to `url` using `asr_send(url)`.
- Stores `range` in `content_buffer` for later paste/swap operations.

**Usage Context**
- Used in content management interfaces to enable copy-paste workflows.

---

#### `content_edit_paste(url)`
Pastes the content from the global buffer to the target location.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the paste command.                |

**Return Value**
- `void`

**Inner Mechanisms**
- Calls `content_edit_command(url, CMS_L_COMMAND_PASTE)` to trigger the paste operation.

**Usage Context**
- Used after `content_edit_copy()` to insert buffered content into a new location.

---

#### `content_edit_swap(url)`
Swaps the content in the target location with the content in the global buffer.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the swap command.                 |

**Return Value**
- `void`

**Inner Mechanisms**
- If `content_buffer` is non-null, calls `content_edit_command(url, CMS_L_MOD_CONTENT_005)`.
- If `content_buffer` is null, displays an alert (`CMS_L_MOD_CONTENT_002`).

**Usage Context**
- Enables reordering of content elements via drag-and-drop or command-based workflows.

---

#### `content_edit_kick1(url)`
Moves a content element forward in the display order by a user-specified number of positions.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL with `%return%` placeholder for the offset.  |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_011`).
- Validates input using regex `/^[0-9]+$/`; displays an error (`CMS_L_MOD_CONTENT_013`) if invalid.
- Replaces `%return%` in `url` with the validated value and calls `content_edit_command(url)`.

**Usage Context**
- Used in content management interfaces to adjust the display order of elements.

---

#### `content_edit_kick2(url)`
Moves a content element backward in the display order by a user-specified number of positions.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL with `%return%` placeholder for the offset.  |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_012`).
- Validates input using regex `/^[0-9]+$/`; displays an error (`CMS_L_MOD_CONTENT_013`) if invalid.
- Replaces `%return%` in `url` with the negated value and calls `content_edit_command(url)`.

**Usage Context**
- Complements `content_edit_kick1()` for bidirectional reordering.

---

#### `content_edit_clear(url)`
Deletes the content at the target location after user confirmation.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the delete command.               |

**Return Value**
- `void`

**Inner Mechanisms**
- Calls `content_edit_command(url, CMS_L_COMMAND_DELETE)` to trigger deletion.

**Usage Context**
- Used to remove content elements from pages or modules.

---

#### `content_edit_repeat(url, value)`
Duplicates a content element a user-specified number of times.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL with `%return%` placeholder for the count.   |
| `value` | `string`  | Default value for the prompt.                    |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_009`), defaulting to `value`.
- Validates input using regex `/^[0-9]+$/`; displays an error (`CMS_L_MOD_CONTENT_013`) if invalid.
- Replaces `%return%` in `url` with the validated value and calls `content_edit_command(url)`.

**Usage Context**
- Used to quickly duplicate content elements (e.g., banners, widgets).

---

#### `content_edit_shift(url, value)`
Shifts a content element's position by a user-specified offset (positive or negative).

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL with `%return%` placeholder for the offset.  |
| `value` | `string`  | Default value for the prompt.                    |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for an integer (`CMS_L_MOD_CONTENT_010`), defaulting to `value`.
- Validates input using regex `/^-?[0-9]+$/`; displays an error (`CMS_L_MOD_CONTENT_013`) if invalid.
- Replaces `%return%` in `url` with the validated value and calls `content_edit_command(url)`.

**Usage Context**
- Used for fine-grained reordering of content elements.

---

#### `content_edit_switch(url, value)`
Toggles a binary state (e.g., visibility, active status) for a content element.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL with `%return%` placeholder for the state.   |
| `value` | `string`  | Current state (`""` or `"1"`).                   |

**Return Value**
- `void`

**Inner Mechanisms**
- Toggles `value` between `""` and `"1"`.
- Replaces `%return%` in `url` with the new value and calls `content_edit_command(url)`.

**Usage Context**
- Used to enable/disable content elements or toggle their properties.

---

#### `content_edit_apply(url)`
Applies pending changes to a content element.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the apply command.                |

**Return Value**
- `void`

**Inner Mechanisms**
- Calls `content_edit_command(url, CMS_L_MOD_CONTENT_003)`.

**Usage Context**
- Used in content editing forms to save changes.

---

#### `content_edit_revert(url)`
Reverts pending changes to a content element.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the revert command.               |

**Return Value**
- `void`

**Inner Mechanisms**
- Calls `content_edit_command(url, CMS_L_MOD_CONTENT_004)`.

**Usage Context**
- Used in content editing forms to discard unsaved changes.

---

#### `content_load(url)`
Navigates the parent window to a new URL.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to navigate to.                              |

**Return Value**
- `void`

**Inner Mechanisms**
- Calls `parent.location.replace(url)` to navigate the parent window.

**Usage Context**
- Used to reload or redirect the main window after content operations.


<!-- HASH:c8df62eae3999252461dbdb49dfc4357 -->
