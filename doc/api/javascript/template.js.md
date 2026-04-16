# NUOS API Documentation

[← Index](../README.md) | [`javascript/template.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Template Control JavaScript (template.js)

This file provides core frontend functionality for the NUOS template editor interface. It handles interactive elements like dropdown menus, control panels, and template block folding (collapsing/expanding). The code establishes event listeners, visual state management, and persistent user preferences for the template editing workflow.

---

### Global Variables

| Name                  | Default/Value | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `tp_ctrl_opt_value`   | `0`           | Bitmask storing the current state of template control options.              |
| `tp_ctrl_opt_img_url` | `""`          | Base URL path for control option button images.                             |

---

### `tp_event()`

**Purpose:**
Initializes event listeners for template editor UI elements. Sets up hover effects, touchscreen context menu suppression, and tooltip text clearing.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. **Dropdown Hover Effects:**
   - Queries all elements with classes `.tp-dd` or `.tp-dd100`.
   - Attaches `mouseover` and `mouseout` listeners to toggle `data-tp-hover` attribute.
   - Uses `e.stopPropagation()` to prevent event bubbling.

2. **Touchscreen Context Menu Suppression:**
   - Targets buttons within `.tp-edt` containers.
   - Prevents default context menu on touch input (`e.pointerType === "touch"`).

3. **Tooltip Suppression:**
   - Clears `title` attributes for all `.module-settings` elements to suppress native tooltips.

**Usage Context:**
- Called during page initialization to set up interactive behaviors.
- Ensures consistent UX across desktop and touch devices.

---

### `tp_beforedragstart()`

**Purpose:**
Prepares the UI for drag operations by hiding the control pad and removing focus from active elements.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Hides the template marker (`#tp-marker`) and control pad (`#tp-ctrl`) using `fx_style()`.
2. Blurs the currently focused element to prevent interference during drag operations.

**Usage Context:**
- Invoked before drag-and-drop operations in the template editor.
- Ensures a clean state for visual feedback during dragging.

---

### `tp_drop()`

**Purpose:**
Restores the control pad visibility after a drag operation completes.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
- Reverts the display state of `#tp-marker` and `#tp-ctrl` to their default values using `fx_style()`.

**Usage Context:**
- Called after drag-and-drop operations to reinstate the control pad.

---

### `tp_ctrl_opt_set(value)`

**Purpose:**
Sets the template control option bitmask to a specific value and triggers the apply action.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | New bitmask value for control options. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Updates `tp_ctrl_opt_value` with the provided `value`.
2. Simulates a click on the `#tp-ctrl-opt-apply` button to apply changes.

**Usage Context:**
- Used to programmatically set control options (e.g., from a button click).

---

### `tp_ctrl_opt_switch(value)`

**Purpose:**
Toggles a specific bit in the control option bitmask.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | Bitmask value to toggle (e.g., `4` for "text" option). |

**Return Values:**
None.

**Inner Mechanisms:**
1. Checks if the bit is set in `tp_ctrl_opt_value` using bitwise AND (`&`).
2. Toggles the bit using bitwise OR (`|=`) or AND with NOT (`&= ~`).
3. Calls `tp_ctrl_opt_img()` to update the UI.

**Usage Context:**
- Used to toggle individual control options (e.g., enabling/disabling "text" mode).

---

### `tp_ctrl_opt_apply(url)`

**Purpose:**
Applies the current control options by navigating to a URL with the options embedded.

**Parameters:**

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `url` | String | URL template with placeholders (`%value%`, `%left%`, `%top%`) to replace. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Replaces placeholders in the URL:
   - `%value%` with `tp_ctrl_opt_value`.
   - `%left%` with the left position from `fx_position_left()`.
   - `%top%` with the top position from `fx_position_top()`.
2. Navigates to the updated URL using `location.replace()`.

**Usage Context:**
- Called when the user confirms control option changes.
- Persists options by reloading the page with the new state.

---

### `tp_ctrl_opt_img()`

**Purpose:**
Updates the visual state of control option buttons based on the current bitmask.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Defines two parallel arrays:
   - `array1`: Bitmask values for each option (e.g., `4` for "text").
   - `array2`: Corresponding option names (e.g., `"text"`).
2. Iterates over the 16 options, updating each button's image:
   - Constructs the image URL using `tp_ctrl_opt_img_url` and the option name.
   - Appends `"_disabled"` to the filename if the bit is not set.
   - Uses `fx_change_image()` to update the button's first child element.

**Usage Context:**
- Called whenever `tp_ctrl_opt_value` changes to reflect the UI state.

---

### `tp_flp(id)`

**Purpose:**
Toggles the folded/collapsed state of a template block or group of blocks.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| `id` | String | ID of the template block to toggle.  |

**Return Values:**
- `false`: Always returns `false` to prevent default behavior.

**Inner Mechanisms:**
1. **Single Block Toggle:**
   - If no modifier keys (Shift/Ctrl/Alt) are pressed, toggles the `data-tp-flp-on` attribute for the block with ID `tp-dd-{id}`.

2. **Group Toggle (Modifier Keys):**
   - If a modifier key is pressed, identifies all blocks (`.tp-dd100`) and determines their relationship to the target block:
     - **Parent/Child Relationship:** Blocks are toggled if they are ancestors or descendants of the target.
   - Updates the `data-tp-flp-on` attribute for all affected blocks.
   - Scrolls to the target block using `fx_scrollto()`.

3. **Persistence:**
   - Calls `tp_flp_store()` to save the folded state.

**Usage Context:**
- Triggered by user interaction (e.g., clicking a fold button).
- Supports both single-block and hierarchical folding.

---

### `tp_flp_store()`

**Purpose:**
Persists the current folded state of template blocks to a cookie.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Queries all blocks with class `.tp-dd100`.
2. Constructs a string (`value`) in the format `/{id1}/{id2}/...` for blocks with `data-tp-flp-on`.
3. Stores the string in the `cms_tp_flp_value` cookie.

**Usage Context:**
- Called after folding/unfolding blocks to save user preferences.

---

### `tp_flp_restore(content_index)`

**Purpose:**
Restores the folded state of template blocks from a cookie.

**Parameters:**

| Name            | Type   | Description                                      |
|-----------------|--------|--------------------------------------------------|
| `content_index` | String | Current page/content identifier.                 |

**Return Values:**
None.

**Inner Mechanisms:**
1. **Page Check:**
   - Compares the current `content_index` with the value stored in `cms_tp_flp_id`.
   - Deletes the `cms_tp_flp_value` cookie if the page has changed.

2. **State Restoration:**
   - Retrieves the folded state from `cms_tp_flp_value`.
   - Queries all blocks (`.tp-dd100`) and updates their `data-tp-flp-on` attribute based on the cookie value.
   - Adds the `tp-flp-restored` class to the document root after a 50ms delay for visual feedback.

**Usage Context:**
- Called during page load to restore the user's previous folding preferences.


<!-- HASH:026f26f3626adc8f45a3a7afeeb96a25 -->
