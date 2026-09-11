# PWNC API Documentation

[← Index](../README.md) | [`javascript/move.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/move.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Move Module (`javascript/move.js`)

The `move.js` file implements a drag-and-drop movement system for DOM elements within the PWNC Web Platform. It allows registered elements to be moved freely across the viewport using mouse interactions, with support for auto-scrolling near viewport edges and handling both static and fixed positioning.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `mv_object` | `null` | Currently dragged DOM element |
| `mv_left` | `0` | Initial mouse X position when drag starts |
| `mv_top` | `0` | Initial mouse Y position when drag starts |
| `mv_offset_left` | `0` | Horizontal offset between mouse and element's left edge |
| `mv_offset_top` | `0` | Vertical offset between mouse and element's top edge |
| `mv_fixed` | `false` | Whether the dragged element uses fixed positioning |
| `mv_flag` | `false` | Indicates if the element has started moving |
| `mv_scroll_flag` | `false` | Prevents recursive scroll calls |
| `mv_scroll_x` | `0` | Horizontal scroll delta for auto-scroll |
| `mv_scroll_y` | `0` | Vertical scroll delta for auto-scroll |

### mv_register

Registers a DOM element as movable by disabling default drag behaviors and marking it with an internal flag.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `object` | `HTMLElement\|string` | Element reference or element ID |

#### Return Value

Returns `true` if registration succeeds, `false` otherwise.

#### Usage Example

```javascript
// Register an element by ID
mv_register('myDraggableDiv');

// Register an element directly
var el = document.getElementById('myDraggableDiv');
mv_register(el);
```

### mv_get_object

Finds the topmost movable ancestor of the element currently under the pointer.

#### Return Value

Returns the movable DOM element or `null` if none found.

#### Usage Example

```javascript
// Called internally during mousedown events
var target = mv_get_object();
if (target) {
    console.log('Found movable element:', target.id);
}
```

### mv_move_object

Positions the currently dragged element at the current mouse coordinates, adjusting for offsets and fixed positioning.

#### Usage Example

```javascript
// Called continuously during drag operations
mv_move_object();
```

### mv_scroll

Handles automatic scrolling of the container when the mouse approaches viewport boundaries during a drag operation.

#### Usage Example

```javascript
// Triggered automatically when dragging near edges
mv_scroll();
```

### mv_event

Main event handler that processes mouse events to manage the drag lifecycle.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `event` | `string` | Mouse event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, or `"mouseleave"` |

#### Usage Example

```javascript
// Registered as a global callback
fx_register_callback(mv_event);

// Handles all drag-related mouse events internally
```


<!-- HASH:47dc22be6ceff1629d5572cab1c33d99 -->
