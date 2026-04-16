# NUOS API Documentation

[← Index](../README.md) | [`javascript/dragdrop.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Drag & Drop Module (`dragdrop.js`)

Core drag-and-drop (DnD) utility for NUOS. Implements a lightweight, framework-free DnD system with visual feedback, keyboard modifiers, and custom event callbacks. Works with any DOM element and supports type-based acceptance filtering.

---

### Global Variables

| Name            | Default | Description                                                                 |
|-----------------|---------|-----------------------------------------------------------------------------|
| `dd_object`     | `null`  | Currently dragged source element.                                           |
| `dd_vehicle`    | `null`  | Floating clone of the dragged element (visual representation).              |
| `dd_touched`    | `null`  | Current potential drop target under the mouse.                              |
| `dd_callback`   | `null`  | User-defined callback function for DnD events.                              |
| `dd_left`       | `0`     | Initial mouse X position when drag started.                                 |
| `dd_top`        | `0`     | Initial mouse Y position when drag started.                                 |
| `dd_nofx`       | `false` | Flag to suppress visual feedback (outline) on the current target.          |
| `dd_flag`       | `false` | Modifier flag (Shift/Ctrl/Alt) active during drag.                          |
| `dd_scroll_flag`| `false` | Flag indicating auto-scroll is active.                                      |
| `dd_scroll_x`   | `0`     | Horizontal scroll delta for auto-scroll.                                    |
| `dd_scroll_y`   | `0`     | Vertical scroll delta for auto-scroll.                                      |

---

### `dd_register(object, type, accept, fixed = false, nofx = false)`

Registers a DOM element for drag-and-drop participation.

#### Parameters

| Name     | Type            | Description                                                                 |
|----------|-----------------|-----------------------------------------------------------------------------|
| `object` | `string|object` | DOM element ID or direct reference.                                         |
| `type`   | `number`        | Bitmask representing the drag type(s) this element can emit.                |
| `accept` | `number`        | Bitmask representing the drag type(s) this element can accept as a target.  |
| `fixed`  | `boolean`       | If `true`, element cannot be dragged (acts only as a drop target).          |
| `nofx`   | `boolean`       | If `true`, suppresses visual feedback (outline) when this element is hovered.|

#### Return Value

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `boolean` | `true` if registration succeeded, `false` if the element was invalid.       |

#### Inner Mechanisms

1. Resolves `object` to a DOM element if passed as an ID.
2. Sets custom properties (`dd_enabled`, `dd_type`, `dd_accept`, `dd_fixed`, `dd_nofx`) on the element.
3. Disables native `dragstart` and `selectstart` events to prevent browser interference.
4. Sets a tooltip (`title`) based on the element’s role (drag, drop, or both).

#### Usage

```javascript
// Register a draggable item (type 1, accepts nothing)
dd_register("item1", 1, 0);

// Register a drop target (type 0, accepts type 1)
dd_register("target1", 0, 1);
```

---

### `dd_set_callback(callback)`

Sets the user-defined callback function for DnD events.

#### Parameters

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `callback` | `function` | Function to handle DnD events. See below for event signatures.              |

#### Callback Signature

```javascript
function callback(event, source, target) { ... }
```

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `event`   | `string` | Event type (see list below).                                                |
| `source`  | `object` | The dragged element (`dd_object`).                                          |
| `target`  | `object` | The potential drop target (`dd_touched`), or `null` if none.               |

#### Event Types

| Event              | Trigger Condition                                                                 |
|--------------------|-----------------------------------------------------------------------------------|
| `"activate"`       | Element clicked without dragging (no modifier).                                   |
| `"select"`         | Element clicked without dragging (with modifier).                                 |
| `"dblclick"`       | Element double-clicked.                                                           |
| `"beforedragstart"`| Before drag starts (mouse moved >10px from initial position).                    |
| `"dragstart"`      | Drag operation starts (vehicle created).                                          |
| `"dragover"`       | Mouse over a valid drop target.                                                   |
| `"drag"`           | Mouse over an invalid area or no target.                                          |
| `"dropon"`         | Dropped on a valid target (no modifier).                                          |
| `"dropon_alt"`     | Dropped on a valid target (with modifier).                                        |
| `"drop"`           | Dropped on an invalid area (no modifier).                                         |
| `"drop_alt"`       | Dropped on an invalid area (with modifier).                                       |

#### Usage

```javascript
function my_callback(event, source, target) {
    if (event === "dropon") {
        target.appendChild(source);
    }
}
dd_set_callback(my_callback);
```

---

### `dd_get_object()`

Retrieves the topmost registered DnD element under the mouse pointer.

#### Return Value

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `object` | The topmost registered element under the mouse, or `null` if none.          |
| `null`   | If the element is the currently dragged source (`dd_object`).               |

#### Inner Mechanisms

1. Uses `fx_pointer_object()` to get the element under the mouse.
2. Traverses up the DOM tree until a registered element (`dd_enabled`) is found.
3. Excludes the currently dragged source (`dd_object`).

#### Usage

```javascript
var target = dd_get_object();
if (target) { ... }
```

---

### `dd_move_vehicle()`

Updates the position of the drag vehicle (visual clone) to follow the mouse.

#### Inner Mechanisms

- Uses `fx_move()` to position the vehicle 10px right and 5px down from the mouse.
- Ensures the vehicle remains visible and follows the cursor smoothly.

---

### `dd_scroll()`

Handles auto-scrolling when the mouse is near the edge of the viewport.

#### Inner Mechanisms

1. Checks if scrolling is needed (mouse near viewport edges).
2. Uses `fx_scroll_container.scrollBy()` to scroll the page.
3. Forces a reflow to ensure the vehicle’s position is recalculated.
4. Updates the vehicle’s position if scrolling occurred.
5. Uses `fx_animation_frame()` to repeat every 25ms while scrolling is active.

#### Usage

- Automatically triggered during drag when the mouse is near the viewport edge.

---

### `dd_event(event)`

Core event handler for mouse interactions. Registered with `fx_register_callback()`.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `event` | `string` | Mouse event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, `"mouseleave"`, `"dblclick"`. |

#### Inner Mechanisms

- **`"mousedown"`**:
  - Checks if a valid source is clicked (left mouse button only).
  - Stores initial mouse position and modifier state.
  - Disables page scrolling (`fx_noscroll()`).

- **`"mousemove"`**:
  - Starts drag if mouse moves >10px from initial position.
  - Creates a vehicle (clone of the dragged element) and appends it to the body.
  - Handles auto-scrolling (`dd_scroll()`).
  - Updates visual feedback (cursor, outline) based on the current target.
  - Triggers `"dragover"` or `"drag"` callbacks.

- **`"mouseup"`**:
  - Triggers `"activate"`, `"select"`, `"dropon"`, `"dropon_alt"`, `"drop"`, or `"drop_alt"` callbacks based on modifier and target.
  - Falls through to `"mouseleave"` to clean up.

- **`"mouseleave"`**:
  - Cleans up the drag operation (removes vehicle, resets cursor, reenables scrolling).
  - Resets all global DnD state variables.

- **`"dblclick"`**:
  - Triggers `"dblclick"` callback if a registered element is double-clicked.

#### Usage

- Automatically managed by the NUOS event system. No direct invocation needed.


<!-- HASH:cc2ce6b175878c2674582d9e136e6ca6 -->
