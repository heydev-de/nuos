# PWNC API Documentation

[← Index](../README.md) | [`javascript/fx.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/fx.js)

- **Version:** `26.7.5.5`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## FX.js – Frontend Animation and Interaction Utilities

**Overview**
`fx.js` provides a lightweight, dependency-free JavaScript library for handling animations, element manipulation, touch interactions, and event management in the PWNC Web Platform. It abstracts common DOM operations, positioning calculations, and gesture recognition (swipe, pinch, move) while maintaining high performance and cross-browser compatibility.

---

## Animation

### `fx_animation_frame(callback, delay = 0)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `function` or `string` | Function to execute on the next animation frame. If a string is provided, it is converted to a function. |
| `delay` | `number` | Optional delay in milliseconds before executing the callback. |

**Return Value**
- `number`: Returns the request ID from `requestAnimationFrame` or `setTimeout` for cancellation.

**Mechanism**
- If `callback` is a string, it is dynamically converted to a function using `new Function()`.
- If `delay` is greater than 0, the callback is wrapped in a `setTimeout` to defer execution.
- Uses `requestAnimationFrame` for smooth animations, ensuring optimal rendering performance.

**Usage Context**
- Ideal for creating smooth animations or deferring DOM updates until the next repaint cycle.
- Useful for performance-sensitive operations where direct DOM manipulation could cause jank.

**Example**
```javascript
// Fade in an element after 500ms
fx_animation_frame(function() {
    document.getElementById("box").style.opacity = 1;
}, 500);
```

---

## Element Manipulation

### `fx_move(object, left, top)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | DOM element or its ID. |
| `left` | `number` | Target left position in pixels. |
| `top` | `number` | Target top position in pixels. |

**Mechanism**
- Resolves the element if an ID is provided.
- Rounds positions to integers to avoid sub-pixel rendering issues.
- Only updates the `style.left` or `style.top` if the new value differs from the current one.

**Usage Context**
- Used to reposition elements absolutely or relatively within their containing block.
- Common in drag-and-drop, tooltips, or custom UI components.

**Example**
```javascript
// Move a modal to the center of the viewport
const modal = document.getElementById("modal");
fx_move(modal, (window.innerWidth - modal.offsetWidth) / 2, 100);
```

---

### `fx_style(object, property, value = null, priority = false)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | DOM element or its ID. |
| `property` | `string` | CSS property name (e.g., `"opacity"`). |
| `value` | `string` or `boolean` | Value to set. If `""` or `false`, the property is removed. |
| `priority` | `boolean` | If `true`, sets the property with `!important`. |

**Return Value**
- `boolean` or `string`: Returns `true` on successful set/remove, the computed value on get, or `false` if the element is invalid.

**Mechanism**
- Acts as a getter if `value` is `null`.
- Uses `setProperty()` with optional `!important` flag.
- Uses `removeProperty()` to clear a style.

**Usage Context**
- Unified interface for reading, setting, or removing inline styles.
- Useful for dynamic theming, responsive adjustments, or state-based styling.

**Example**
```javascript
// Toggle visibility of a sidebar
fx_style("sidebar", "display", fx_style("sidebar", "display") === "none" ? "block" : "none");
```

---

### `fx_visible(object, set = null)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | DOM element or its ID. |
| `set` | `boolean` or `null` | If `true`, makes element visible; if `false`, hides it. If `null`, returns current visibility. |

**Return Value**
- `boolean`: Current visibility state if `set` is `null`.

**Mechanism**
- Uses `fx_style()` internally to toggle the `visibility` property.

**Usage Context**
- Quick visibility toggling without affecting layout (unlike `display: none`).

**Example**
```javascript
// Hide a loading spinner when content is ready
fx_visible("spinner", false);
```

---

### `fx_change_image(object, image_url)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Image element or its ID. |
| `image_url` | `string` | New image source URL. |

**Mechanism**
- Only applies to `<img>` elements.
- Updates the `src` attribute.

**Usage Context**
- Dynamic image swapping in galleries, avatars, or thumbnails.

**Example**
```javascript
// Update a profile picture
fx_change_image("avatar", "/images/user123.jpg");
```

---

## Window Manipulation

### `fx_scrollto(object)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element to scroll into view. |

**Mechanism**
- Calculates the target scroll position relative to the viewport.
- Uses smooth animation via `requestAnimationFrame`.
- Aborts on user interaction (keyboard, mouse, touch, wheel).
- Stops when the target is reached or movement is negligible.

**Usage Context**
- Smooth scrolling to sections, anchors, or dynamically loaded content.

**Example**
```javascript
// Scroll to a form after validation error
fx_scrollto("error-message");
```

---

### `fx_adjust_window(object = window)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `Window` | Popup window to adjust. Defaults to current window. |

**Mechanism**
- Only works on popup windows with an opener.
- Resizes and repositions the window to fit its content.
- Ensures the window stays within screen bounds.

**Usage Context**
- Useful for popups (e.g., login, media viewer) that need to fit their content.

**Example**
```javascript
// Open and adjust a popup
const popup = window.open("/preview", "_blank", "width=800,height=600");
popup.onload = () => fx_adjust_window(popup);
```

---

## Element Positioning

### `fx_left(object, relative = false)`
### `fx_top(object, relative = false)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element or its ID. |
| `relative` | `boolean` | If `true`, returns position relative to offset parent. If `false`, returns absolute position. |

**Return Value**
- `number`: Position in pixels.

**Mechanism**
- Uses `offsetLeft`/`offsetTop` for relative positioning.
- Uses `fx_offset_left()`/`fx_offset_top()` for absolute positioning.

**Usage Context**
- Determining element position for tooltips, overlays, or collision detection.

**Example**
```javascript
// Position a tooltip above an element
const button = document.getElementById("help-button");
const tooltip = document.getElementById("tooltip");
fx_style(tooltip, "top", fx_top(button) - tooltip.offsetHeight - 10 + "px");
```

---

### `fx_offset_left(object, no_cropping = false)`
### `fx_offset_top(object, no_cropping = false)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element or its ID. |
| `no_cropping` | `boolean` | If `true`, ignores parent clipping. |

**Return Value**
- `number`: Absolute position in pixels.

**Mechanism**
- Uses `getBoundingClientRect()` for initial position.
- Traverses up the DOM to account for clipped parents (e.g., `overflow: hidden`).
- Adds current scroll position to convert from viewport to document coordinates.

**Usage Context**
- Accurate positioning of floating elements (e.g., dropdowns, modals) even when ancestors clip content.

**Example**
```javascript
// Position a dropdown below a button, even if inside a scrollable container
const dropdown = document.getElementById("dropdown");
fx_style(dropdown, "left", fx_offset_left("menu-button") + "px");
fx_style(dropdown, "top", fx_offset_top("menu-button") + 40 + "px");
```

---

## Element Dimensions

### `fx_width(object, no_cropping = false)`
### `fx_height(object, no_cropping = false)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element or its ID. |
| `no_cropping` | `boolean` | If `true`, returns full width/height regardless of clipping. |

**Return Value**
- `number`: Width or height in pixels.

**Mechanism**
- Uses `getBoundingClientRect()` for initial dimensions.
- Traverses up the DOM to find the smallest visible area due to clipping.
- Returns `0` if the element is fully clipped.

**Usage Context**
- Calculating available space for dynamic layouts or responsive components.

**Example**
```javascript
// Check if an element is visible within its container
const element = document.getElementById("banner");
if (fx_width(element) > 0 && fx_height(element) > 0) {
    console.log("Element is visible");
}
```

---

## Window Positioning

### `fx_position_left()`
### `fx_position_top()`

**Return Value**
- `number`: Scroll position as a percentage of total document size.

**Mechanism**
- Calculates the ratio of scroll position to document dimensions.
- Rounds to 2 decimal places for readability.

**Usage Context**
- Progress indicators, scroll-linked animations, or lazy loading.

**Example**
```javascript
// Update a scroll progress bar
document.getElementById("progress").style.width = fx_position_left() + "%";
```

---

## Document Dimensions

### `fx_document_size(object = window)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `Window` | Window context to measure. |

**Return Value**
- `object` or `null`: `{ width: number, height: number }` if content exceeds viewport; otherwise `null`.

**Mechanism**
- Traverses the DOM from `<body>` to find the farthest element.
- Accounts for margins and transforms.
- Returns `null` if all content fits within the viewport.

**Usage Context**
- Determining if scrollbars are needed or adjusting window size in popups.

**Example**
```javascript
// Check if content overflows
const size = fx_document_size();
if (size && size.height > window.innerHeight) {
    console.log("Content overflows vertically");
}
```

---

## Swipe Functionality

### `fx_swipe(object, callback)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element or its ID to detect swipes on. |
| `callback` | `function` | Function called on swipe: `callback(object, direction)`. Direction is `"l"`, `"r"`, `"u"`, or `"d"`. |

**Mechanism**
- Tracks mouse and touch events to detect swipe gestures.
- Ignores small movements (< 20px).
- Determines direction based on dominant axis.
- Prevents accidental clicks during swipes.

**Usage Context**
- Mobile-friendly carousels, image galleries, or navigation menus.

**Example**
```javascript
// Add swipe navigation to a photo gallery
fx_swipe("gallery", function(element, direction) {
    if (direction === "l") showNextImage();
    if (direction === "r") showPreviousImage();
});
```

---

## Move and Zoom (Pinch) Functionality

### `fx_move_zoom(object, callback)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element or its ID to detect move/zoom on. |
| `callback` | `function` | Function called on interaction: `callback(object, vx, vy, z, zx, zy)`. |

| Parameter (callback) | Type | Description |
|----------------------|------|-------------|
| `vx` | `number` | Horizontal movement in pixels. |
| `vy` | `number` | Vertical movement in pixels. |
| `z` | `number` | Zoom factor (positive = zoom in). |
| `zx` | `number` | X-coordinate of zoom center. |
| `zy` | `number` | Y-coordinate of zoom center. |

**Mechanism**
- Supports mouse drag, touch pan, and pinch-to-zoom.
- Includes inertia (flick) for smooth deceleration.
- Prevents text selection and drag-and-drop during interaction.

**Usage Context**
- Interactive maps, image viewers, or custom drawing applications.

**Example**
```javascript
// Create a zoomable image viewer
fx_move_zoom("viewer", function(element, vx, vy, z, zx, zy) {
    const img = element.querySelector("img");
    img.style.transform = `translate(${vx}px, ${vy}px) scale(${1 + z})`;
});
```

---

## Miscellaneous

### `fx_pointer_block(set = true)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `set` | `boolean` | If `true`, blocks all pointer events; if `false`, restores them. |

**Mechanism**
- Injects a global CSS rule to disable pointer events, touch action, and user selection.
- Creates a `<style>` element with ID `fx_pointer_block`.

**Usage Context**
- Preventing interaction during animations, loading states, or modal overlays.

**Example**
```javascript
// Block interaction during a critical animation
fx_pointer_block(true);
setTimeout(() => fx_pointer_block(false), 1000);
```

---

### `fx_pointer_object()`

**Return Value**
- `HTMLElement`: The topmost element under the mouse cursor.

**Mechanism**
- Temporarily disables the pointer block (if active).
- Uses `document.elementFromPoint()` to find the element.

**Usage Context**
- Hit testing, custom tooltips, or context-aware UI.

**Example**
```javascript
// Show a tooltip for the element under the mouse
document.addEventListener("mousemove", function() {
    const element = fx_pointer_object();
    if (element && element.title) {
        showTooltip(element.title);
    }
});
```

---

## Global Event Values

| Variable | Type | Description |
|----------|------|-------------|
| `fx_document_width` | `number` | Total document width. |
| `fx_document_height` | `number` | Total document height. |
| `fx_window_left` | `number` | Current horizontal scroll position. |
| `fx_window_top` | `number` | Current vertical scroll position. |
| `fx_window_width` | `number` | Viewport width. |
| `fx_window_height` | `number` | Viewport height. |
| `fx_mouse_key` | `number` | Current mouse button (1 = left, 2 = right, 3 = middle). |
| `fx_mouse_x` | `number` | Mouse X position in document coordinates. |
| `fx_mouse_y` | `number` | Mouse Y position in document coordinates. |
| `fx_mouse_window_x` | `number` | Mouse X position in viewport coordinates. |
| `fx_mouse_window_y` | `number` | Mouse Y position in viewport coordinates. |
| `fx_touch1_x`, `fx_touch1_y` | `number` | First touch position in document coordinates. |
| `fx_touch1_window_x`, `fx_touch1_window_y` | `number` | First touch position in viewport coordinates. |
| `fx_touch2_x`, `fx_touch2_y` | `number` | Second touch position (or `null`). |
| `fx_touch2_window_x`, `fx_touch2_window_y` | `number` | Second touch position in viewport coordinates. |
| `fx_keyboard_key` | `number` | Last pressed key code. |
| `fx_scroll_container` | `HTMLElement` or `Window` | Element that handles scrolling (window or body). |

---

## Event Update Functions

### `fx_update_window_position()`
### `fx_update_window_size()`
### `fx_update_mouse_position(e)`
### `fx_update_touch_position(e)`

**Purpose**
- Maintain global state for mouse, touch, and scroll positions.
- Called automatically on relevant events.

**Usage Context**
- Internal use; developers should not need to call these directly.

---

## Event Settings

### `fx_noscroll(set = true)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `set` | `boolean` | If `true`, prevents scrolling on touch devices. |

**Mechanism**
- Sets a flag used in touch event handlers to call `preventDefault()`.

**Usage Context**
- Preventing scroll during fullscreen interactions (e.g., games, drawing apps).

**Example**
```javascript
// Disable scroll during a game
fx_noscroll(true);
```

---

## Event Management

### `fx_event_raise(event, e)`
### `_fx_event_raise(event, e)`

**Purpose**
- Dispatch custom or native events to registered callbacks.
- Ensures callbacks are executed in the next animation frame.

**Mechanism**
- Uses `requestAnimationFrame` to batch event handling.
- Supports both function and object-style callbacks.
- Handles "once" events (e.g., `window_load`).

**Usage Context**
- Internal event system; prefer `fx_event_listen()`.

---

### `fx_register_callback(callback)`
### `fx_unregister_callback(callback)`

**Purpose**
- Register or unregister a callback from the global event system.

**Usage Context**
- Advanced use; typically handled automatically by `fx_event_listen()`.

---

### `fx_event_listen(object, event, _function = null, passive = true, capture = false)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string`, `HTMLElement`, or `array` | Element, ID, or array of elements. |
| `event` | `string` or `array` | Event name(s) to listen for. |
| `_function` | `function` | Callback function. |
| `passive` | `boolean` | If `true`, indicates the callback will not call `preventDefault()`. |
| `capture` | `boolean` | If `true`, uses capture phase. |

**Mechanism**
- Supports multiple objects or events.
- Maps native events (e.g., `"load"`) to custom names (e.g., `"window_load"`).
- Registers callbacks in the global event system.

**Usage Context**
- Unified event binding for both native and custom events.

**Example**
```javascript
// Listen for clicks on multiple buttons
fx_event_listen(["btn1", "btn2"], "click", function(e) {
    console.log("Button clicked:", e.target.id);
});
```

---

### `fx_event_remove(object, event = "", _function = null)`

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `function` or `HTMLElement` | Callback or element. |
| `event` | `string` | Event name to remove. |
| `_function` | `function` | Callback to remove (for native events). |

**Mechanism**
- Removes a callback from the global system or a native event listener.

**Usage Context**
- Cleanup to prevent memory leaks.

**Example**
```javascript
// Remove a previously added click handler
fx_event_remove(document.getElementById("btn"), "click", clickHandler);
```

---

## Ghost Event Busting

### `fx_ghost_buster()`

**Purpose**
- Prevents "ghost clicks" on touch devices caused by delayed mouse events.

**Mechanism**
- Uses an `AbortController` to cancel event listeners after a real click.
- Blocks mouse events that don't match the touch detail.

**Usage Context**
- Automatically enabled on touch devices; no manual intervention needed.

---

## Event Initialization

**Overview**
- Sets up global event listeners for window, document, mouse, touch, and keyboard.
- Initializes scroll container and updates global state on load.
- Handles touch-to-click conversion and ghost event prevention.

**Usage Context**
- Internal initialization; developers should not modify this section.


<!-- HASH:eb2135759222a4acccb1e8a17e7dea64 -->
