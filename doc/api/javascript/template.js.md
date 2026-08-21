# PWNC API Documentation

[← Index](../README.md) | [`javascript/template.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/template.js)

- **Version:** `26.7.5.5`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Control System (`template.js`)

Core frontend module for PWNC's visual template editor. Provides interactive UI controls for real-time template manipulation, including drag-and-drop, state management, and visual toggles. Designed for zero-dependency operation with direct DOM manipulation.

---

### Global Variables

| Name                  | Default/Value | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `tp_ctrl_opt_value`   | `0`           | Bitmask storing current control panel option states (16 flags).             |
| `tp_ctrl_opt_img_url` | `""`          | Base URL path for control panel button icons (SVG assets).                  |

---

### `tp_event()`
Initializes event listeners for template editor UI elements.

#### Purpose
Attaches mouse interaction handlers to dropdown menus and suppresses unwanted behaviors (context menus on touch, tooltips in settings).

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
1. **Dropdown Hover States**: Adds `data-tp-hover` attribute on `mouseover`, removes on `mouseout` for `.tp-dd` and `.tp-dd100` elements.
2. **Touch Context Menu Suppression**: Prevents default context menu on touch devices for `.tp-edt > BUTTON`.
3. **Tooltip Removal**: Clears `title` attributes in `.module-settings` to prevent native tooltips.

#### Usage Context
Called during page initialization to enable interactive template editing features.

#### Example
```javascript
// Initialize template editor events on page load
document.addEventListener("DOMContentLoaded", tp_event);
```

---

### `tp_beforedragstart()`
Prepares UI for drag operations by hiding control elements.

#### Purpose
Ensures a clean drag state by hiding the control pad and removing focus from active elements.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
1. Hides `#tp-marker` and `#tp-ctrl` elements using `fx_style()`.
2. Blurs the currently focused element.

#### Usage Context
Triggered before drag operations (e.g., moving template elements).

#### Example
```javascript
// Attach to dragstart event
document.addEventListener("dragstart", tp_beforedragstart);
```

---

### `tp_drop()`
Restores UI after drag operations.

#### Purpose
Re-enables the control pad and marker after a drag operation completes.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
Resets `display` property of `#tp-marker` and `#tp-ctrl` to their default values.

#### Usage Context
Triggered after drag operations (e.g., `drop` or `dragend` events).

#### Example
```javascript
document.addEventListener("drop", tp_drop);
```

---

### `tp_ctrl_opt_set(value)`
Sets the control panel option bitmask to a specific value.

#### Purpose
Directly assigns a new value to `tp_ctrl_opt_value` and triggers the apply action.

#### Parameters

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `value` | `number` | New bitmask value for control options. |

#### Return Value
None.

#### Inner Mechanisms
1. Updates `tp_ctrl_opt_value`.
2. Simulates a click on `#tp-ctrl-opt-apply` to trigger `tp_ctrl_opt_apply()`.

#### Usage Context
Used to set control panel options programmatically (e.g., from keyboard shortcuts).

#### Example
```javascript
// Set control options to "text" and "href" (bits 4 and 1)
tp_ctrl_opt_set(4 | 1);
```

---

### `tp_ctrl_opt_switch(value)`
Toggles a specific bit in the control panel option bitmask.

#### Purpose
Flips a single option flag in `tp_ctrl_opt_value` (on/off) and updates UI icons.

#### Parameters

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `value` | `number` | Bitmask value of the option to toggle. |

#### Return Value
None.

#### Inner Mechanisms
1. Uses bitwise XOR to toggle the specified bit.
2. Calls `tp_ctrl_opt_img()` to update button icons.

#### Usage Context
Triggered by user interaction with control panel buttons.

#### Example
```javascript
// Toggle the "image" option (bit 32)
tp_ctrl_opt_switch(32);
```

---

### `tp_ctrl_opt_apply(url)`
Applies control panel options by navigating to a processed URL.

#### Purpose
Replaces the current URL with a version incorporating the current control options and cursor position.

#### Parameters

| Name  | Type     | Description                                                                 |
|-------|----------|-----------------------------------------------------------------------------|
| `url` | `string` | Template URL with placeholders (`%value%`, `%left%`, `%top%`) to replace. |

#### Return Value
None.

#### Inner Mechanisms
1. Replaces placeholders in `url` with:
   - `tp_ctrl_opt_value` → `%value%`
   - Cursor X position → `%left%`
   - Cursor Y position → `%top%`
2. Navigates to the processed URL using `location.replace()`.

#### Usage Context
Called when the user confirms control panel changes (e.g., via "Apply" button).

#### Example
```javascript
// Apply options to a template URL
tp_ctrl_opt_apply("https://pwnc.it/template?id=123&options=%value%&x=%left%&y=%top%");
```

---

### `tp_ctrl_opt_img()`
Updates control panel button icons based on the current option bitmask.

#### Purpose
Dynamically changes button icons to reflect enabled/disabled states.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
1. Maps bitmask values to option names (e.g., `4` → `"text"`).
2. Updates `src` of each button's child `<img>` using `fx_change_image()`:
   - Appends `"_disabled"` to the filename if the option is off.

#### Usage Context
Called after `tp_ctrl_opt_value` changes to provide visual feedback.

#### Example
```javascript
// Initialize button icons (assumes tp_ctrl_opt_img_url is set)
tp_ctrl_opt_img_url = "/assets/";
tp_ctrl_opt_img();
```

---

### `tp_flp(id)`
Toggles the "flip" state of a template dropdown section.

#### Purpose
Expands/collapses a dropdown section (`.tp-dd100`) and manages sibling sections based on modifier keys.

#### Parameters

| Name | Type     | Description                          |
|------|----------|--------------------------------------|
| `id` | `string` | ID suffix of the dropdown element (prefixed with `tp-dd-`). |

#### Return Value
`false` (to prevent default event behavior).

#### Inner Mechanisms
1. **Modifier Key Logic**:
   - **Shift/Ctrl/Alt**: Toggles all sibling sections (expands if parent/child relationship exists).
   - **No Modifier**: Toggles only the specified section.
2. **State Management**:
   - Sets/removes `data-tp-flp-on` attribute.
   - Calls `tp_flp_store()` to persist state.
3. **Scrolling**: Uses `fx_scrollto()` to focus the section.

#### Usage Context
Triggered by user clicks on dropdown headers.

#### Example
```javascript
// Toggle section with ID "tp-dd-content"
tp_flp("content");
```

---

### `tp_flp_store()`
Persists the current flip state of all dropdown sections to a cookie.

#### Purpose
Saves the expanded/collapsed state of `.tp-dd100` elements for session continuity.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
1. Iterates through all `.tp-dd100` elements.
2. Builds a string of IDs (e.g., `"/content/header/"`) for expanded sections.
3. Stores the string in `cms_tp_flp_value` cookie.

#### Usage Context
Called automatically by `tp_flp()` after state changes.

#### Example
```javascript
// Manually save state (e.g., before page unload)
window.addEventListener("beforeunload", tp_flp_store);
```

---

### `tp_flp_restore(content_index)`
Restores the flip state of dropdown sections from a cookie.

#### Purpose
Reapplies the saved flip state for a specific page (identified by `content_index`).

#### Parameters

| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `content_index` | `string` | Unique identifier for the current page. |

#### Return Value
None.

#### Inner Mechanisms
1. **Page Validation**:
   - Compares `content_index` with `cms_tp_flp_id` cookie.
   - Clears state if the page has changed.
2. **State Restoration**:
   - Parses `cms_tp_flp_value` cookie to get expanded section IDs.
   - Sets/removes `data-tp-flp-on` attributes.
3. **Visual Feedback**:
   - Adds `tp-flp-restored` class to `<html>` after a delay (50ms).

#### Usage Context
Called during page initialization to restore UI state.

#### Example
```javascript
// Restore state for page "home"
document.addEventListener("DOMContentLoaded", () => tp_flp_restore("home"));
```


<!-- HASH:551fd93b254d3ff837065795326b546a -->
