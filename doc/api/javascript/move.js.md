# NUOS API Documentation

[← Index](../README.md) | [`javascript/move.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview
`move.js` provides drag-and-drop functionality for DOM elements within the NUOS web platform. It allows users to move registered elements by clicking and dragging them with the mouse. The module handles event registration, position calculation, scroll adjustments at page borders, and cursor management.

---

## Global Variables

| Name            | Value/Default | Description                                                                                     |
|-----------------|---------------|-------------------------------------------------------------------------------------------------|
| `mv_object`     | `null`        | Currently selected DOM element being moved.                                                     |
| `mv_left`       | `0`           | Initial X-coordinate of the mouse when dragging started.                                        |
| `mv_top`        | `0`           | Initial Y-coordinate of the mouse when dragging started.                                        |
| `mv_offset_left`| `0`           | Horizontal offset between the mouse position and the element's left edge.                       |
| `mv_offset_top` | `0`           | Vertical offset between the mouse position and the element's top edge.                          |
| `mv_fixed`      | `false`       | Flag indicating if the element is positioned as `fixed` (relative to viewport).                 |
| `mv_flag`       | `false`       | Flag indicating if a drag operation is actively in progress.                                    |
| `mv_scroll_flag`| `false`       | Flag indicating if automatic scrolling at page borders is active.                               |
| `mv_scroll_x`   | `0`           | Horizontal scroll speed (pixels per frame) when near page borders.                              |
| `mv_scroll_y`   | `0`           | Vertical scroll speed (pixels per frame) when near page borders.                                |

---

## Functions

### `mv_register(object)`
Registers a DOM element for drag-and-drop functionality.

#### Parameters
| Name    | Type               | Description                                                                                     |
|---------|--------------------|-------------------------------------------------------------------------------------------------|
| `object`| `string` or `object`| DOM element ID or the element itself. If a string, it is resolved via `document.getElementById`.|

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `boolean`| `true` if registration succeeded; `false` if the element is invalid or not found.               |

#### Inner Mechanisms
1. Resolves the input to a DOM element if a string ID is provided.
2. Prevents default drag-and-drop and text selection behaviors via `ondragstart` and `onselectstart` event handlers.
3. Marks the element with a custom property `mv_enabled = true`.
4. Sets a default title (`"✥ Move"`) if none exists.

#### Usage
- Call this function during initialization to enable drag-and-drop for an element.
- Example:
  ```javascript
  mv_register("my-draggable-element");
  ```

---

### `mv_get_object()`
Retrieves the nearest registered draggable element under the current mouse pointer.

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `object`| The nearest registered draggable element, or `null` if none is found or the current `mv_object` is the same. |

#### Inner Mechanisms
1. Uses `fx_pointer_object()` to get the element under the mouse.
2. Traverses up the DOM tree via `parentElement` to find the nearest ancestor with `mv_enabled = true`.
3. Skips the current `mv_object` to avoid self-selection.

#### Usage
- Internally used to determine which element to move when a mouse down event occurs.

---

### `mv_move_object()`
Updates the position of the currently selected element (`mv_object`) based on mouse coordinates.

#### Inner Mechanisms
1. Calculates the new position using `fx_mouse_x`, `fx_mouse_y`, and the stored offsets (`mv_offset_left`, `mv_offset_top`).
2. Adjusts for `fixed` positioning by subtracting the window scroll position (`fx_window_left`, `fx_window_top`).
3. Applies the new position using `fx_move()`.

#### Usage
- Called during mouse move events to update the element's position in real-time.

---

### `mv_scroll()`
Handles automatic scrolling of the page when the mouse is near the edges during a drag operation.

#### Inner Mechanisms
1. Sets `mv_scroll_flag` to `true` to indicate scrolling is active.
2. Stores the current window scroll position.
3. Uses `fx_scroll_container.scrollBy()` to scroll by `mv_scroll_x` and `mv_scroll_y` pixels.
4. Temporarily hides the dragged element to check if scrolling occurred (via `fx_window_left`/`fx_window_top` changes).
5. If scrolling occurred, updates the element's position via `mv_move_object()`.
6. Uses `fx_animation_frame()` to repeat the scroll operation every 25ms until the mouse moves away from the edge.

#### Usage
- Automatically triggered when the mouse is within 100 pixels of the page borders during a drag operation.

---

### `mv_event(event)`
Handles mouse events (`mousedown`, `mousemove`, `mouseup`, `mouseleave`) for drag-and-drop operations.

#### Parameters
| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `event` | `string` | Event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, or `"mouseleave"`.                      |

#### Inner Mechanisms
##### `mousedown`
1. Exits if another element is already being dragged or if the left mouse button is not pressed.
2. Retrieves the target element via `mv_get_object()`.
3. Stores initial mouse coordinates and calculates offsets for positioning.
4. Adjusts offsets for `fixed` elements.
5. Sets `mv_object` and disables page scrolling via `fx_noscroll()`.

##### `mousemove`
1. Exits if no element is being dragged.
2. If `mv_flag` is `true` (drag in progress):
   - Calculates scroll speeds (`mv_scroll_x`, `mv_scroll_y`) if the mouse is near page borders.
   - Triggers `mv_scroll()` if not already scrolling.
3. If `mv_flag` is `false` (drag not yet started):
   - Checks if the mouse has moved at least 10 pixels from the initial position (to avoid accidental drags).
   - Sets `mv_flag` to `true`, changes the cursor to `"move"`, and blocks responsivity via `fx_pointer_block()`.

##### `mouseup` / `mouseleave`
1. Resets `mv_object` to `null`.
2. Resets `mv_flag` to `false`.
3. Re-enables page scrolling and resets the cursor.
4. Re-enables responsivity via `fx_pointer_block(false)`.

#### Usage
- Automatically registered via `fx_register_callback(mv_event)` during module initialization.
- Handles all drag-and-drop lifecycle events.


<!-- HASH:7048077777ef5588c563971b72ea12f0 -->
