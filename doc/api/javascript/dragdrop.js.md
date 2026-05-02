# NUOS API Documentation

[← Index](../README.md) | [`javascript/dragdrop.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.24.1`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Drag & Drop Module (`dragdrop.js`)

This module provides a lightweight, framework-agnostic drag-and-drop system for DOM elements. It enables interactive dragging of elements, visual feedback during drag operations, and customizable drop targets with type-based acceptance filtering. The module integrates with NUOS's core frontend utilities (`fx_*`) for pointer management, styling, and animation.

---

### Global Variables

| Name            | Default | Description                                                                                     |
|-----------------|---------|-------------------------------------------------------------------------------------------------|
| `dd_object`     | `null`  | Currently dragged source element.                                                               |
| `dd_vehicle`    | `null`  | Visual clone of the dragged element (floating overlay).                                         |
| `dd_touched`    | `null`  | Current potential drop target under the pointer.                                                |
| `dd_callback`   | `null`  | User-defined callback function for drag-and-drop events.                                        |
| `dd_left`       | `0`     | Initial X-coordinate of the drag start (relative to element).                                   |
| `dd_top`        | `0`     | Initial Y-coordinate of the drag start (relative to element).                                   |
| `dd_nofx`       | `false` | Flag to disable visual feedback (CSS classes) on drop targets.                                  |
| `dd_flag`       | `false` | Modifier flag (Shift/Ctrl/Alt) active during drag start.                                        |
| `dd_scroll_flag`| `false` | Flag indicating whether auto-scrolling at page borders is active.                               |
| `dd_scroll_x`   | `0`     | Horizontal scroll delta for auto-scrolling.                                                     |
| `dd_scroll_y`   | `0`     | Vertical scroll delta for auto-scrolling.                                                       |

---

### `dd_register(object, type, accept, fixed = false, nofx = false)`

Registers a DOM element for drag-and-drop operations.

#### Parameters

| Name     | Type            | Description                                                                                     |
|----------|-----------------|-------------------------------------------------------------------------------------------------|
| `object` | `string|object` | DOM element ID or direct DOM object to register.                                                |
| `type`   | `number`        | Bitmask representing the drag type (e.g., `0x01` for "file", `0x02` for "text").                |
| `accept` | `number`        | Bitmask representing acceptable drop types (e.g., `0x03` accepts both `0x01` and `0x02`).       |
| `fixed`  | `boolean`       | If `true`, the element cannot be dragged (acts only as a drop target).                          |
| `nofx`   | `boolean`       | If `true`, disables visual feedback (CSS class toggling) on this element.                       |

#### Return Value
- `boolean`: `true` if registration succeeded, `false` if the element was invalid.

#### Inner Mechanisms
1. Resolves the DOM element from an ID if necessary.
2. Attaches drag-and-drop metadata (`dd_enabled`, `dd_type`, `dd_accept`, `dd_fixed`, `dd_nofx`) to the element.
3. Disables native browser drag-and-drop (`ondragstart`, `onselectstart`).
4. Sets a tooltip (`title`) based on the element's role (drag source, drop target, or both).

#### Usage
```javascript
// Register a draggable element (type 0x01, accepts 0x02)
dd_register("my-draggable-element", 0x01, 0x02);

// Register a drop target (type 0x00, accepts 0x01, fixed)
dd_register("my-drop-target", 0x00, 0x01, true);
```

---

### `dd_set_callback(callback)`

Sets the user-defined callback function for drag-and-drop events.

#### Parameters

| Name       | Type       | Description                                                                                     |
|------------|------------|-------------------------------------------------------------------------------------------------|
| `callback` | `function` | Function to handle drag-and-drop events. See below for event types.                            |

#### Callback Signature
```javascript
function callback(event, source, target) { ... }
```
- **`event`**: Event type (string). Possible values:
  - `"activate"`, `"dblclick"`, `"select"`: Non-drag interactions.
  - `"beforedragstart"`, `"dragstart"`: Drag initiation.
  - `"dragover"`, `"drag"`: Drag movement (over a target or elsewhere).
  - `"dropon"`, `"dropon_alt"`: Drop on a valid target (normal/alternate).
  - `"drop"`, `"drop_alt"`: Drop outside a valid target (normal/alternate).
- **`source`**: Dragged element (`dd_object`).
- **`target`**: Drop target (`dd_touched`), or `null` if no valid target.

#### Usage
```javascript
dd_set_callback((event, source, target) => {
    if (event === "dropon") {
        console.log("Dropped", source.id, "on", target.id);
    }
});
```

---

### `dd_get_object()`

Retrieves the topmost registered drag-and-drop element under the pointer.

#### Return Value
- `object|null`: The DOM element, or `null` if none is found or the pointer is over the dragged source.

#### Inner Mechanisms
1. Uses `fx_pointer_object()` to get the element under the pointer.
2. Traverses up the DOM tree to find the nearest ancestor with `dd_enabled = true`.
3. Excludes the currently dragged source (`dd_object`).

#### Usage
```javascript
const target = dd_get_object();
if (target) console.log("Target:", target.id);
```

---

### `dd_move_vehicle()`

Updates the position of the drag vehicle (visual clone) to follow the pointer.

#### Inner Mechanisms
- Uses `fx_move()` to position the vehicle at `(fx_mouse_window_x + 10, fx_mouse_window_y + 5)`.

#### Usage
Called internally during drag operations. No direct external usage.

---

### `dd_scroll()`

Handles auto-scrolling at page borders during drag operations.

#### Inner Mechanisms
1. Checks if scrolling is necessary (`dd_scroll_x` or `dd_scroll_y` non-zero).
2. Scrolls the container by the delta values using `scrollBy({ behavior: "instant" })`.
3. Forces a reflow to ensure the vehicle's position updates correctly.
4. Recursively schedules itself via `fx_animation_frame(dd_scroll, 25)` if scrolling is active.

#### Usage
Called internally during drag operations. No direct external usage.

---

### `dd_event(event)`

Core event handler for mouse interactions. Registered via `fx_register_callback()`.

#### Parameters

| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `event` | `string` | Mouse event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, `"mouseleave"`, or `"dblclick"`.  |

#### Inner Mechanisms
- **`"mousedown"`**:
  - Validates left mouse button and selects the source element (`dd_object`).
  - Stores initial coordinates and modifier flags (`dd_flag`).
  - Disables page scrolling via `fx_noscroll()`.
- **`"mousemove"`**:
  - Initiates drag if the pointer moves >10px from the start.
  - Creates a vehicle (clone of the source) and appends it to the body.
  - Handles auto-scrolling at page borders.
  - Updates the cursor and visual feedback based on the target under the pointer.
  - Triggers `"dragover"` or `"drag"` callbacks.
- **`"mouseup"`**:
  - Triggers `"select"`/`"activate"` (if no drag occurred) or `"dropon"`/`"drop"` callbacks.
  - Cleans up the vehicle and resets state.
- **`"mouseleave"`**:
  - Resets all drag-and-drop state (source, vehicle, touched element).
  - Re-enables scrolling and pointer responsiveness.
- **`"dblclick"`**:
  - Triggers the `"dblclick"` callback if a valid target is under the pointer.

#### Usage
Handled internally by the NUOS event system. No direct external usage.

---

### Integration Notes
1. **Dependencies**:
   - Requires `fx_*` utilities (pointer management, styling, animation).
   - Assumes `fx_register_callback()` is available for event binding.
2. **Styling**:
   - Drop targets receive the `dd-active` class during drag-over (unless `dd_nofx = true`).
   - The vehicle uses the `dd-vehicle` class for styling.
3. **Performance**:
   - Uses `pointerEvents: "none"` on the vehicle to avoid interference.
   - Clones the source element without `id` attributes or scripts to prevent side effects.
4. **Type Matching**:
   - Drop targets accept sources if `(target.dd_accept & source.dd_type) !== 0`.


<!-- HASH:85ffb4ccd643e02be03bcfeb7e7e9b61 -->
