# PWNC API Documentation

[← Index](../README.md) | [`javascript/template.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/template.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Editor JavaScript Module

This file implements the client-side logic for the PWNC template editor's interactive overlay system. It provides drag-and-drop reordering, inline command buttons, visibility toggling, and option controls for template elements marked with `tp-dd` or `tp-dd100` classes.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `tp_l_title` | `""` | Tooltip text for template elements |
| `tp_l_flip` | `""` | Tooltip text for flip toggle buttons |
| `tp_l_sync` | `""` | Tooltip text indicating synchronized state |
| `tp_ctrl_opt_value` | `0` | Bitmask value for active control options |
| `tp_ctrl_opt_data` | `[]` | Array of option definitions `[type, unused, title, image_on, image_off]` |
| `tp_code` | `{}` | Maps element types to primary command definitions `[action, image, callback]` |
| `tp_command` | `{}` | Maps command names to extended command definitions `[action, param, image, newline]` |
| `tp_dd` | `{}` | Maps action names to handler functions for drag/drop events |

### tp_event()

Initializes the template editor UI by building overlays, registering drag handlers, and setting up event listeners.

**Parameters:** None  
**Returns:** void  

**Mechanisms:**
1. Calls `tp_build()` to hydrate DOM elements
2. If in edit mode (`dd_set_callback` exists), registers the drag/drop callback and adds `tp-touchbar` class
3. Registers all `.tp-dd` and `.tp-dd100` elements with the drag/drop system
4. Adds hover effects via mouseover/mouseout listeners
5. Suppresses context menus on touch devices for buttons
6. Removes tooltips from module settings

**Usage Example:**
```javascript
// Automatically called on page load in edit mode
tp_event();
```

### tp_dd_event(event, source, target)

Handles drag/drop events for template elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `event` | string | Event type: `dblclick`, `beforedragstart`, `dropon`, `dropon_alt`, `drop`, `drop_alt` |
| `source` | HTMLElement | The dragged element |
| `target` | HTMLElement | The drop target element |

**Returns:** void  

**Mechanisms:**
- `dblclick`: Triggers click on the primary edit button
- `beforedragstart`: Hides control overlay and blurs active element
- `dropon`/`dropon_alt`: Executes move/duplicate action from `tp_dd` map
- `drop`/`drop_alt`: Resets control display

**Usage Example:**
```javascript
// Registered automatically via dd_set_callback
tp_dd_event("dropon", draggedElement, targetElement);
```

### tp_ctrl_display(value)

Toggles visibility of the marker and control overlay elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `value` | string\|boolean | CSS display value or `false` to hide |

**Returns:** void  

**Usage Example:**
```javascript
tp_ctrl_display("none");  // Hide controls
tp_ctrl_display(false);   // Hide controls
tp_ctrl_display("block"); // Show controls
```

### tp_dd_beforedragstart()

Hides the control overlay and blurs any focused element when drag begins.

**Parameters:** None  
**Returns:** void  

### tp_dd_drop()

Resets the control overlay visibility after a drop operation.

**Parameters:** None  
**Returns:** void  

### tp_build()

Builds the interactive overlay UI for all template elements with `data-tp-cmd` attribute.

**Parameters:** None  
**Returns:** void  

**Mechanisms:**
1. Iterates over all `.tp-dd`/`.tp-dd100` elements with `data-tp-cmd`
2. Creates a container div with class `tp-edt`
3. For `tp-dd100` elements, adds a flip toggle button
4. Adds primary command button using `tp_code` definition
5. Adds extended command buttons from `tp_command` based on `data-tp-cmd`
6. Repositions activation links for href/download types
7. Adds display name if `data-tp-name` exists
8. Builds option switch buttons from `tp_ctrl_opt_data`

**Usage Example:**
```javascript
// Called automatically during initialization
tp_build();
```

### tp_button(object, action, image, title, id, data_title)

Creates a command button element with an image and click handler.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `object` | HTMLElement | The template element the button operates on |
| `action` | string | Code template with placeholders like `%value%`, `%path%` |
| `image` | array | Image definition `[src, width, height, alt]` |
| `title` | string | Button tooltip text |
| `id` | string\|null | Optional button ID |
| `data_title` | string\|null | Optional data-title attribute |

**Returns:** HTMLButtonElement  

**Mechanisms:**
1. Creates button with type="button"
2. Sets title and optional ID/data-title
3. Creates img element with provided dimensions and alt text
4. Creates click handler that prevents default and executes the action code with placeholders replaced via `tp_action()`

**Usage Example:**
```javascript
const btn = tp_button(
    element,
    "edit('%path%')",
    ["/img/edit.png", 16, 16, "Edit"],
    "Edit element",
    "tp-edt-a-123"
);
```

### tp_action(code, source, target)

Replaces placeholders in action code with actual data values from elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `code` | string | Action code template with placeholders |
| `source` | HTMLElement | Source element providing data |
| `target` | HTMLElement | Target element (defaults to source) |

**Returns:** string - Action code with placeholders replaced  

**Mechanisms:**
- `%index%`: Replaced with `source.dataset.tpIndex`
- `%path%`: Replaced with `source.dataset.tpPath`
- `%type%`: Replaced with `source.dataset.tpType`
- `%reference%`: Replaced with `source.dataset.tpReference`
- `%value%`: Replaced with `source.dataset.tpValue` or `target.dataset.tpPath`
- `%id%`: Replaced with `target.dataset.tpId`

All values are URL-encoded.

**Usage Example:**
```javascript
const action = tp_action("move('%path%', '%id%')", sourceEl, targetEl);
// Result: "move('/content/123', '456')"
```

### tp_ctrl_opt_update()

Updates the visual state of option switch buttons based on the current bitmask value.

**Parameters:** None  
**Returns:** void  

**Mechanisms:**
Iterates over `tp_ctrl_opt_data` and updates each button's image to show on/off state based on whether the corresponding bit is set in `tp_ctrl_opt_value`.

### tp_ctrl_opt_set(value)

Sets the option bitmask value and triggers the apply action.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `value` | number | Bitmask value to set |

**Returns:** void  

**Usage Example:**
```javascript
tp_ctrl_opt_set(5); // Set options 1 and 4
```

### tp_ctrl_opt_switch(value)

Toggles a specific option bit and updates the UI.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `value` | number | Bitmask value to toggle |

**Returns:** void  

**Usage Example:**
```javascript
tp_ctrl_opt_switch(2); // Toggle option 2
```

### tp_ctrl_opt_apply(url)

Navigates to a URL with the current option value and scroll position embedded.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `url` | string | URL template with `%value%`, `%left%`, `%top%` placeholders |

**Returns:** void  

**Mechanisms:**
Replaces placeholders in URL with current option value and viewport scroll position, then navigates using `location.replace()`.

**Usage Example:**
```javascript
tp_ctrl_opt_apply("/admin/template?opt=%value%&x=%left%&y=%top%");
```

### tp_flp(id)

Toggles the "flip" state of template elements, with special behavior for modifier keys.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `id` | string | ID of the element to flip |

**Returns:** void  

**Mechanisms:**
1. If Shift/Ctrl/Alt is pressed: toggles flip state on all `tp-dd100` elements except those containing or contained by the target
2. Otherwise: toggles flip state on the specific element
3. Calls `tp_flp_store()` to persist state

**Usage Example:**
```javascript
tp_flp("123"); // Toggle flip state of element with ID 123
```

### tp_flp_store()

Persists the current flip state to a cookie.

**Parameters:** None  
**Returns:** void  

**Mechanisms:**
Collects all `tp-dd100` elements with `data-tp-flp-on` attribute, extracts their IDs, and stores them as a path-like string in the `cms_tp_flp_value` cookie.

### tp_flp_restore(content_index)

Restores flip state from cookie for the current content index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `content_index` | string | Current content/page identifier |

**Returns:** void  

**Mechanisms:**
1. Adds `tp-flp-restored` class to document element after 50ms
2. Checks if the stored content index matches the current one
3. If mismatch, clears the flip value cookie
4. If match, restores flip state on all `tp-dd100` elements based on stored cookie value

**Usage Example:**
```javascript
tp_flp_restore("page_42"); // Restore flip state for page 42
```


<!-- HASH:d2897da24e1286fbbd9b8c477a00e62d -->
