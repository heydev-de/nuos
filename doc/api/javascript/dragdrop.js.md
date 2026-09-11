# PWNC API Documentation

[← Index](../README.md) | [`javascript/dragdrop.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/dragdrop.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## dragdrop.js

The `dragdrop.js` file implements a lightweight, dependency-free drag-and-drop system for the PWNC Web Platform. It provides core functionality to register draggable and droppable elements, track mouse interactions, manage visual feedback during dragging, and trigger user-defined callbacks for various drag-and-drop events.

This module relies on global utility functions such as `fx_pointer_object`, `fx_mouse_x`, `fx_mouse_y`, `fx_mouse_window_x`, `fx_mouse_window_y`, `fx_window_left`, `fx_window_top`, `fx_window_width`, `fx_window_height`, `fx_keyboard_key`, `fx_mouse_key`, `fx_style`, `fx_move`, `fx_scroll_container`, `fx_update_window_position`, `fx_animation_frame`, `fx_noscroll`, `fx_pointer_block`, and `fx_register_callback`. These are assumed to be defined elsewhere in the platform's JavaScript framework.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `dd_object` | `null` | Currently dragged source element. |
| `dd_vehicle` | `null` | Visual clone element that follows the cursor during drag. |
| `dd_touched` | `null` | Currently hovered potential drop target. |
| `dd_callback` | `null` | User-defined callback function for drag events. |
| `dd_left` | `0` | Initial X coordinate of the drag start. |
| `dd_top` | `0` | Initial Y coordinate of the drag start. |
| `dd_nofx` | `false` | Flag indicating whether visual effects should be skipped for the current target. |
| `dd_flag` | `false` | Modifier key flag (Shift/Ctrl/Alt) active during drag start. |
| `dd_scroll_flag` | `false` | Indicates if auto-scroll is currently active. |
| `dd_scroll_x` | `0` | Horizontal scroll delta for auto-scroll. |
| `dd_scroll_y` | `0` | Vertical scroll delta for auto-scroll. |

---

## dd_register

Registers an element as a draggable or droppable object by attaching metadata and disabling text selection.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `object` | `string\|object` | Element ID or DOM object to register. |
| `type` | `number` | Bitmask type identifier for the object (used for matching with `accept`). |
| `accept` | `number` | Bitmask of types this object can interact with. |
| `fixed` | `boolean` | If `true`, the object cannot be dragged (only acts as a drop target). |
| `nofx` | `boolean` | If `true`, disables visual effects (e.g., highlight) on drop targets. |

### Return Value

- **Type:** `boolean`
- **Description:** Returns `true` if registration was successful, `false` if the object could not be resolved.

### Inner Mechanism

1. Resolves the input to a DOM object if a string ID is provided.
2. Sets custom properties (`dd_enabled`, `dd_type`, `dd_accept`, `dd_fixed`, `dd_nofx`) on the element.
3. Disables native drag and text selection behaviors.
4. Sets a descriptive `title` attribute based on the object's role.

### Usage Example

```html
<div id="item1">Drag me</div>
<script>
dd_register("item1", 1, 2, false, false);
</script>
```

This registers the element with ID `item1` as a draggable item of type `1` that accepts drops from type `2`.

---

## dd_set_callback

Sets a global callback function to handle drag-and-drop events.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `callback` | `function` | Function to call on drag events. Receives `(event, source, target)`. |

### Return Value

- **Type:** `void`

### Inner Mechanism

Stores the provided function reference in the global `dd_callback` variable. This function is invoked throughout the drag lifecycle with contextual parameters.

### Usage Example

```javascript
function my_callback(event, source, target) {
    console.log(event, source, target);
}
dd_set_callback(my_callback);
```

---

## dd_get_object

Traverses up the DOM tree from the element under the pointer to find the nearest registered drag/drop object.

### Parameters

- None

### Return Value

- **Type:** `object\|null`
- **Description:** The nearest registered ancestor element, or `null` if none found or if it's the same as the currently dragged object.

### Inner Mechanism

1. Gets the element under the pointer via `fx_pointer_object()`.
2. Walks up the DOM using `parentElement`.
3. Returns the first element with `dd_enabled === true`, excluding the current `dd_object`.

### Usage Example

Used internally by `dd_event` to determine which element is being interacted with.

---

## dd_move_vehicle

Positions the drag visual clone (vehicle) near the mouse cursor.

### Parameters

- None

### Return Value

- **Type:** `void`

### Inner Mechanism

Calls `fx_move` to set the vehicle's position to `(fx_mouse_window_x + 10, fx_mouse_window_y + 5)`.

### Usage Example

Called automatically during drag operations to keep the visual clone following the cursor.

---

## dd_scroll

Handles auto-scrolling of the container when the cursor approaches the viewport edge during a drag.

### Parameters

- None

### Return Value

- **Type:** `void`

### Inner Mechanism

1. Checks if scrolling is needed based on `dd_scroll_x` and `dd_scroll_y`.
2. Scrolls the container using `fx_scroll_container.scrollBy`.
3. Forces a reflow to detect position changes.
4. Updates the vehicle position if scrolling occurred.
5. Schedules itself via `fx_animation_frame` for continuous scrolling.

### Usage Example

Automatically triggered during drag operations when the cursor nears the edge of the scrollable area.

---

## dd_event

Main event handler for mouse interactions, managing the full drag-and-drop lifecycle.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `event` | `string` | Mouse event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, `"mouseleave"`, `"dblclick"`. |

### Return Value

- **Type:** `void`

### Inner Mechanism

#### `mousedown`
- Ignores if another drag is active or non-left click.
- Identifies the source object via `dd_get_object`.
- Stores initial coordinates and modifier key state.
- Disables page scrolling.

#### `mousemove`
- Ignores if no source or source is fixed.
- On first significant movement (>10px), creates the drag vehicle (visual clone).
- Manages auto-scrolling near viewport edges.
- Tracks potential drop targets and applies visual feedback.
- Fires appropriate callbacks (`dragstart`, `dragover`, `drag`).

#### `mouseup`
- Fires callbacks based on final position:
  - `activate` or `select` if not moved far.
  - `dropon` or `dropon_alt` if over a valid target.
  - `drop` or `drop_alt` otherwise.

#### `mouseleave`
- Cleans up all drag state:
  - Removes vehicle element.
  - Clears touched target.
  - Re-enables scrolling and pointer events.

#### `dblclick`
- Fires `dblclick` callback if left-click on a registered object.

### Usage Example

Registered automatically via `fx_register_callback(dd_event)` at the end of the file. No manual invocation required.

---

## Event Callback Values

When using `dd_set_callback`, the following event strings may be received:

| Event | Description |
|-------|-------------|
| `activate` | Clicked without dragging. |
| `dblclick` | Double-clicked a registered object. |
| `select` | Clicked with modifier key held. |
| `beforedragstart` | Drag is about to begin. |
| `dragstart` | Drag has started. |
| `dragover` | Dragging over a valid target. |
| `drag` | Dragging over an invalid area. |
| `dropon` | Released over a valid target. |
| `dropon_alt` | Released over a valid target with modifier key. |
| `drop` | Released over an invalid area. |
| `drop_alt` | Released over an invalid area with modifier key. |


<!-- HASH:60e105817f617386a4c51aa71005d91b -->
