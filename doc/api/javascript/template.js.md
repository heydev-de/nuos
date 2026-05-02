# NUOS API Documentation

[← Index](../README.md) | [`javascript/template.js`](https://github.com/heydev-de/nuos/blob/main/nuos/javascript/template.js)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Template System JavaScript Utilities (`template.js`)

This file provides core frontend functionality for the NUOS template editor interface. It handles interactive elements such as dropdown menus, control panels, and element folding states, enabling a visual editing experience for website templates.

---

### Global Variables

| Name                  | Default Value | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `tp_ctrl_opt_value`   | `0`           | Bitmask storing the current state of template control options.              |
| `tp_ctrl_opt_img_url` | `""`          | Base URL path for control option icons (SVG images).                        |

---

### `tp_event()`

**Purpose:**
Initializes event listeners for template editor UI elements. Sets up hover effects, context menu suppression, and tooltip removal.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. **Dropdown Hover Effects:**
   - Targets elements with classes `.tp-dd` and `.tp-dd100`.
   - Adds `mouseover` and `mouseout` listeners to toggle `data-tp-hover` attribute.
   - Uses `fx_event_listen()` (assumed external utility) for cross-browser event binding.

2. **Touch Context Menu Suppression:**
   - Targets buttons within `.tp-edt` containers.
   - Prevents default context menu on touch devices via `pointerType` check.

3. **Tooltip Suppression:**
   - Removes `title` attributes from `.module-settings` elements to prevent native tooltips.

**Usage Context:**
- Called during page initialization to enable interactive template editing features.
- Ensures consistent behavior across desktop and touch devices.

---

### `tp_beforedragstart()`

**Purpose:**
Prepares the UI for drag operations by hiding control elements and removing focus.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Hides the template marker (`#tp-marker`) and control pad (`#tp-ctrl`) using `fx_style()`.
2. Blurs the currently focused element to prevent interference during drag operations.

**Usage Context:**
- Triggered before drag-and-drop operations in the template editor.
- Ensures a clean UI state during element repositioning.

---

### `tp_drop()`

**Purpose:**
Restores the UI after drag operations by re-displaying control elements.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
- Reverts the visibility of `#tp-marker` and `#tp-ctrl` using `fx_style()`.

**Usage Context:**
- Called after drag-and-drop operations complete.
- Restores the template editor's interactive controls.

---

### `tp_ctrl_opt_set(value)`

**Purpose:**
Sets the template control option bitmask to a specific value and triggers an apply action.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | New bitmask value for control options. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Updates `tp_ctrl_opt_value` with the provided `value`.
2. Simulates a click on `#tp-ctrl-opt-apply` to trigger `tp_ctrl_opt_apply()`.

**Usage Context:**
- Used to programmatically set control options (e.g., via UI buttons or keyboard shortcuts).

---

### `tp_ctrl_opt_switch(value)`

**Purpose:**
Toggles a specific bit in the control option bitmask.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | Bitmask value representing the option to toggle. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Checks if the bit is set in `tp_ctrl_opt_value`.
2. Toggles the bit using bitwise operations (`|=`, `&= ~`).
3. Calls `tp_ctrl_opt_img()` to update UI icons.

**Usage Context:**
- Used for interactive toggling of individual control options (e.g., via checkboxes or buttons).

---

### `tp_ctrl_opt_apply(url)`

**Purpose:**
Applies the current control options by navigating to a processed URL.

**Parameters:**

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `url` | String | URL template with placeholders (`%value%`, `%left%`, `%top%`) to replace. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Replaces placeholders in the URL with:
   - `tp_ctrl_opt_value` (current bitmask).
   - Cursor position (`fx_position_left()`, `fx_position_top()`).
2. Navigates to the processed URL using `location.replace()`.

**Usage Context:**
- Triggered by the "Apply" button in the control panel.
- Persists control options to the server for template rendering.

---

### `tp_ctrl_opt_img()`

**Purpose:**
Updates the visual state of control option icons based on the current bitmask.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Defines two parallel arrays:
   - `array1`: Bitmask values for each option.
   - `array2`: Corresponding option names (e.g., `"text"`, `"image"`).
2. Iterates over the 16 options, updating each icon's `src` attribute:
   - Appends `"_disabled"` to the filename if the bit is not set.
   - Uses `fx_change_image()` to update the icon.

**Usage Context:**
- Called whenever `tp_ctrl_opt_value` changes.
- Provides visual feedback for active/inactive options.

---

### `tp_flp(id)`

**Purpose:**
Toggles the folded state of a template element or group of elements.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| `id` | String | ID of the element to fold/unfold (prefixed with `tp-dd-`). |

**Return Values:**
- `false`: Always returns `false` to prevent default event behavior.

**Inner Mechanisms:**
1. **Single Element Toggle:**
   - If no modifier key (Shift/Ctrl/Alt) is pressed, toggles `data-tp-flp-on` for the target element.

2. **Group Toggle:**
   - If a modifier key is pressed, determines the hierarchical relationship between the target and other `.tp-dd100` elements.
   - Folds/unfolds all elements in the same branch (parent/child relationships).
   - Uses `fx_scrollto()` to ensure the target element is visible.

3. **State Persistence:**
   - Calls `tp_flp_store()` to save the current fold state.

**Usage Context:**
- Triggered by user interaction (e.g., clicking a fold/unfold button).
- Supports both individual and hierarchical folding.

---

### `tp_flp_store()`

**Purpose:**
Persists the current fold state of template elements to a cookie.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Iterates over all `.tp-dd100` elements.
2. Constructs a string (`value`) with the IDs of folded elements (e.g., `"/id1/id2/"`).
3. Stores the string in the `cms_tp_flp_value` cookie.

**Usage Context:**
- Called automatically by `tp_flp()`.
- Ensures fold states persist across page reloads.

---

### `tp_flp_restore(content_index)`

**Purpose:**
Restores the fold state of template elements from a cookie.

**Parameters:**

| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `content_index` | String | Current page/content identifier.     |

**Return Values:**
None.

**Inner Mechanisms:**
1. **Page Validation:**
   - Compares `content_index` with the stored `cms_tp_flp_id` cookie.
   - Clears the fold state cookie if the page has changed.

2. **State Restoration:**
   - Retrieves the fold state from `cms_tp_flp_value`.
   - Iterates over `.tp-dd100` elements, toggling `data-tp-flp-on` based on the stored state.
   - Adds `tp-flp-restored` class to the `<html>` element after a delay (50ms) for visual feedback.

**Usage Context:**
- Called during page initialization to restore the user's previous fold state.
- Ensures a consistent editing experience across sessions.


<!-- HASH:09c42a76fc2db97789c3a779176846fb -->
