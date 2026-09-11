# PWNC API Documentation

[← Index](../README.md) | [`javascript/fx.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/fx.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC FX JavaScript Module

The `fx.js` file is the core client-side interaction library for the PWNC Web Platform. It provides a comprehensive set of utilities for animation, element manipulation, positioning, gesture recognition (swipe, pinch-to-zoom, move), event management, and global state tracking for mouse, touch, and keyboard inputs.

This module operates on a global state model where various properties (like `fx_mouse_x`, `fx_window_left`, etc.) are continuously updated by event listeners. Functions within this module rely on these global variables to perform their operations, making it a tightly integrated system for handling complex UI interactions.

## Global State Variables

These variables maintain the current state of the browser window, document, mouse, touch points, and scroll container.

| Variable | Default | Description |
|----------|---------|-------------|
| `fx_document_width` | `0` | Current width of the document in pixels |
| `fx_document_height` | `0` | Current height of the document in pixels |
| `fx_window_left` | `0` | Horizontal scroll position of the window/container |
| `fx_window_top` | `0` | Vertical scroll position of the window/container |
| `fx_window_width` | `0` | Width of the viewport in pixels |
| `fx_window_height` | `0` | Height of the viewport in pixels |
| `fx_mouse_key` | `false` | Currently pressed mouse button (1-indexed) or `false` |
| `fx_mouse_x` | `0` | X coordinate of the mouse cursor relative to the document |
| `fx_mouse_y` | `0` | Y coordinate of the mouse cursor relative to the document |
| `fx_mouse_window_x` | `0` | X coordinate of the mouse cursor relative to the viewport |
| `fx_mouse_window_y` | `0` | Y coordinate of the mouse cursor relative to the viewport |
| `fx_touch1_x` | `0` | X coordinate of the first touch point relative to the document |
| `fx_touch1_y` | `0` | Y coordinate of the first touch point relative to the document |
| `fx_touch1_window_x` | `0` | X coordinate of the first touch point relative to the viewport |
| `fx_touch1_window_y` | `0` | Y coordinate of the first touch point relative to the viewport |
| `fx_touch2_x` | `0` | X coordinate of the second touch point relative to the document |
| `fx_touch2_y` | `0` | Y coordinate of the second touch point relative to the document |
| `fx_touch2_window_x` | `0` | X coordinate of the second touch point relative to the viewport |
| `fx_touch2_window_y` | `0` | Y coordinate of the second touch point relative to the viewport |
| `fx_keyboard_key` | `false` | Currently pressed keyboard key code or `false` |
| `fx_scroll_container` | `null` | Reference to the element used for scrolling (`window` or `document.body`) |
| `fx_noscroll_flag` | `false` | Flag to prevent scrolling on touch devices |
| `fx_callback` | `[]` | Array of registered callback functions for custom events |
| `fx_ghost_buster_control` | `null` | AbortController for ghost click prevention |
| `fx_ghost_buster_zuul` | `false` | Flag to disable ghost click prevention temporarily |
| `fx_touch_time` | `-1` | Timestamp of the last touch start |
| `fx_touch_x` | `0` | X coordinate of the initial touch point |
| `fx_touch_y` | `0` | Y coordinate of the initial touch point |
| `fx_touch_window_x` | `0` | Viewport-relative X coordinate of the initial touch |
| `fx_touch_window_y` | `0` | Viewport-relative Y coordinate of the initial touch |
| `fx_touch_count` | `0` | Counter for consecutive taps |
| `fx_touch_timer` | `null` | Timer for tap/double-tap detection |
| `fx_touch_detail` | `42` | Detail value for synthesized mouse events |

## Animation

### fx_animation_frame

Schedules a function to be executed on the next animation frame, optionally after a delay.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `callback` | `string\|function` | - | Function to execute or a string containing code to evaluate |
| `delay` | `number` | `0` | Delay in milliseconds before scheduling the animation frame |

**Returns:** `number\|int` - The ID returned by `setTimeout` (if delayed) or `requestAnimationFrame`

**Mechanisms:**
- If `callback` is a string, it's converted to a function using `new Function()`
- If `delay > 0`, uses `setTimeout` to wait before calling `requestAnimationFrame`
- Otherwise, directly calls `requestAnimationFrame`

**Usage:**
```javascript
// Execute a function on the next animation frame
fx_animation_frame(function(time) {
    console.log("Animation frame at:", time);
});

// Execute after a 100ms delay
fx_animation_frame("console.log('Delayed animation')", 100);
```

## Element Manipulation

### fx_move

Moves an element to specified coordinates by setting its `left` and `top` CSS properties.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element to move |
| `left` | `number` | - | Target X coordinate in pixels |
| `top` | `number` | - | Target Y coordinate in pixels |

**Returns:** `void`

**Mechanisms:**
- Resolves string IDs to DOM elements
- Floors coordinate values to integers
- Only updates style properties if values have changed
- Sets `style.left` and `style.top` with "px" suffix

**Usage:**
```javascript
// Move element with ID 'myElement' to position (100, 50)
fx_move('myElement', 100, 50);

// Move a DOM element directly
var el = document.getElementById('draggable');
fx_move(el, 200, 150);
```

### fx_style

Gets or sets CSS properties on an element with optional priority.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `property` | `string` | - | CSS property name |
| `value` | `string\|null` | `null` | Value to set, or `null` to retrieve |
| `priority` | `boolean` | `false` | Whether to set as `!important` |

**Returns:** `boolean\|string` - `true` if set/removed successfully, computed value if retrieving

**Mechanisms:**
- String IDs are resolved to DOM elements
- Empty string or `false` value removes the property
- Non-null value sets the property with optional "important" priority
- Null value retrieves the computed style value

**Usage:**
```javascript
// Set a CSS property
fx_style('myElement', 'backgroundColor', 'red');

// Set with important priority
fx_style('myElement', 'zIndex', '9999', true);

// Remove a property
fx_style('myElement', 'opacity', '');

// Get a computed style value
var bgColor = fx_style('myElement', 'backgroundColor');
```

### fx_visible

Toggles or checks the visibility of an element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `set` | `boolean\|null` | `null` | `true` to show, `false` to hide, `null` to check |

**Returns:** `boolean` - `true` if visible (when checking), `false` otherwise

**Mechanisms:**
- Delegates to `fx_style` to set or retrieve the `visibility` property
- When `set` is `null`, checks if current visibility is "visible"

**Usage:**
```javascript
// Hide an element
fx_visible('myElement', false);

// Show an element
fx_visible('myElement', true);

// Check if element is visible
if (fx_visible('myElement')) {
    console.log('Element is visible');
}
```

### fx_change_image

Changes the source of an `<img>` element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLImageElement` | - | Element ID or image DOM element |
| `image_url` | `string` | - | New image URL |

**Returns:** `void`

**Mechanisms:**
- Resolves string IDs to DOM elements
- Only updates `src` if the element is an `<img>` tag

**Usage:**
```javascript
// Change image source
fx_change_image('avatar', '/images/new-avatar.png');
```

## Window Manipulation

### fx_scrollto

Smoothly scrolls the window/container to bring an element into view.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `HTMLElement` | - | Element to scroll to |

**Returns:** `void`

**Mechanisms:**
- Checks if the element is within a fixed-position ancestor (skips if so)
- Uses `AbortController` to cancel scrolling on user interaction
- Listens for `keydown`, `mousedown`, `touchstart`, and `wheel` events to abort
- Animates scroll position over 500ms using `requestAnimationFrame`
- Calculates target position with a 20% vertical offset from the top

**Usage:**
```javascript
// Scroll to element smoothly
fx_scrollto(document.getElementById('section-about'));
```

### fx_adjust_window

Adjusts the size and position of a popup window to fit its content.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `Window` | `window` | Window object to adjust |

**Returns:** `void`

**Mechanisms:**
- Only operates on windows with an `opener` property (popups)
- Calculates target dimensions based on document size and screen availability
- Ensures minimum dimensions of 900x600
- Resizes and moves the window within screen bounds
- Focuses the window after adjustment

**Usage:**
```javascript
// Adjust popup window size when content loads
window.addEventListener('load', function() {
    fx_adjust_window(window);
});
```

## Element Positioning

### fx_left

Gets the absolute or relative horizontal position of an element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `relative` | `boolean` | `false` | If `true`, returns offset relative to parent |

**Returns:** `number` - X coordinate in pixels

**Mechanisms:**
- When `relative` is `true`, returns `offsetLeft`
- Otherwise, uses `fx_offset_left` with cropping disabled

**Usage:**
```javascript
// Get absolute position
var x = fx_left('myElement');

// Get position relative to parent
var relX = fx_left('myElement', true);
```

### fx_top

Gets the absolute or relative vertical position of an element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `relative` | `boolean` | `false` | If `true`, returns offset relative to parent |

**Returns:** `number` - Y coordinate in pixels

**Mechanisms:**
- When `relative` is `true`, returns `offsetTop`
- Otherwise, uses `fx_offset_top` with cropping disabled

**Usage:**
```javascript
// Get absolute position
var y = fx_top('myElement');

// Get position relative to parent
var relY = fx_top('myElement', true);
```

### fx_offset_left

Calculates the left offset of an element, optionally accounting for parent clipping.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `no_cropping` | `boolean` | `false` | If `true`, ignores parent clipping |

**Returns:** `number` - Left offset in pixels

**Mechanisms:**
- Uses `getBoundingClientRect()` for precise positioning
- When `no_cropping` is `false`, walks up parent chain to find maximum left boundary
- Adds `fx_window_left` to account for scroll position

**Usage:**
```javascript
// Get offset with parent clipping consideration
var offset = fx_offset_left('myElement');

// Get raw offset without clipping
var rawOffset = fx_offset_left('myElement', true);
```

### fx_offset_top

Calculates the top offset of an element, optionally accounting for parent clipping.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `no_cropping` | `boolean` | `false` | If `true`, ignores parent clipping |

**Returns:** `number` - Top offset in pixels

**Mechanisms:**
- Uses `getBoundingClientRect()` for precise positioning
- When `no_cropping` is `false`, walks up parent chain to find maximum top boundary
- Adds `fx_window_top` to account for scroll position

**Usage:**
```javascript
// Get offset with parent clipping consideration
var offset = fx_offset_top('myElement');

// Get raw offset without clipping
var rawOffset = fx_offset_top('myElement', true);
```

## Element Dimensions

### fx_width

Gets the width of an element, optionally accounting for parent clipping.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `no_cropping` | `boolean` | `false` | If `true`, ignores parent clipping |

**Returns:** `number` - Width in pixels

**Mechanisms:**
- Uses `getBoundingClientRect()` for precise dimensions
- When `no_cropping` is `false`, walks up parent chain to find minimum right boundary
- Returns `Math.max(0, right - left)` to ensure non-negative result

**Usage:**
```javascript
// Get width with parent clipping consideration
var width = fx_width('myElement');

// Get raw width without clipping
var rawWidth = fx_width('myElement', true);
```

### fx_height

Gets the height of an element, optionally accounting for parent clipping.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `no_cropping` | `boolean` | `false` | If `true`, ignores parent clipping |

**Returns:** `number` - Height in pixels

**Mechanisms:**
- Uses `getBoundingClientRect()` for precise dimensions
- When `no_cropping` is `false`, walks up parent chain to find minimum bottom boundary
- Returns `Math.max(0, bottom - top)` to ensure non-negative result

**Usage:**
```javascript
// Get height with parent clipping consideration
var height = fx_height('myElement');

// Get raw height without clipping
var rawHeight = fx_height('myElement', true);
```

## Window Positioning

### fx_position_left

Calculates the horizontal scroll position as a percentage of document width.

**Parameters:** None

**Returns:** `number` - Percentage (0-100) of horizontal scroll position

**Mechanisms:**
- Divides `fx_window_left` by `fx_document_width`
- Multiplies by 100 and rounds to 2 decimal places

**Usage:**
```javascript
// Get horizontal scroll percentage
var scrollPercent = fx_position_left();
console.log("Scrolled " + scrollPercent + "% horizontally");
```

### fx_position_top

Calculates the vertical scroll position as a percentage of document height.

**Parameters:** None

**Returns:** `number` - Percentage (0-100) of vertical scroll position

**Mechanisms:**
- Divides `fx_window_top` by `fx_document_height`
- Multiplies by 100 and rounds to 2 decimal places

**Usage:**
```javascript
// Get vertical scroll percentage
var scrollPercent = fx_position_top();
console.log("Scrolled " + scrollPercent + "% vertically");
```

## Document Dimensions

### fx_document_size

Calculates the total size of the document by traversing all visible elements.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `Window` | `window` | Window object whose document to measure |

**Returns:** `Object\|null` - Object with `width` and `height` properties, or `null` if no elements found

**Mechanisms:**
- Traverses DOM tree using a stack-based approach
- Skips elements without `offsetParent` (hidden elements)
- Accounts for element margins
- Skips elements with CSS transforms or auto dimensions
- Tracks maximum X and Y coordinates across all elements

**Usage:**
```javascript
// Get document dimensions
var size = fx_document_size();
if (size) {
    console.log("Document size:", size.width, "x", size.height);
}
```

## Swipe Functionality

### fx_swipe

Adds swipe gesture detection to an element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `callback` | `function` | - | Function called with `(element, direction)` on swipe |

**Returns:** `void`

**Mechanisms:**
- Tracks mouse/touch coordinates during interaction
- Uses `correct_direction` to filter out jitter
- Determines swipe direction based on movement thresholds (>20px)
- Supports four directions: "l" (left), "r" (right), "u" (up), "d" (down)
- Prevents default click behavior after swipe
- Handles both mouse and touch events

**Usage:**
```javascript
// Add swipe detection to an element
fx_swipe('carousel', function(element, direction) {
    if (direction === 'l') {
        showNextSlide();
    } else if (direction === 'r') {
        showPrevSlide();
    }
});
```

## Move/Pinch Functionality

### fx_move_zoom

Adds pan and pinch-to-zoom gesture support to an element.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement` | - | Element ID or DOM element |
| `callback` | `function` | - | Function called with `(element, vx, vy, zoom, zoomX, zoomY)` |

**Returns:** `void`

**Mechanisms:**
- Tracks single-pointer movement for panning
- Tracks two-pointer distance for pinch zoom
- Implements momentum/flick scrolling with exponential decay
- Uses `clear_vector` to reset velocity after 250ms of inactivity
- Handles mouse, touch, and wheel events
- Prevents default click behavior during gestures

**Callback Parameters:**
- `element`: The target element
- `vx`: Horizontal movement delta
- `vy`: Vertical movement delta
- `zoom`: Zoom delta (positive for zoom in, negative for zoom out)
- `zoomX`: X coordinate of zoom center relative to element
- `zoomY`: Y coordinate of zoom center relative to element

**Usage:**
```javascript
// Add pan and zoom to an image viewer
fx_move_zoom('image-container', function(element, vx, vy, zoom, zoomX, zoomY) {
    // Apply transformations
    var currentTransform = element.style.transform || '';
    element.style.transform = currentTransform + 
        ` translate(${vx}px, ${vy}px) scale(${1 + zoom})`;
});
```

## Misc Functions

### fx_pointer_block

Blocks or restores pointer events on all elements.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `set` | `boolean` | `true` | `true` to block, `false` to restore |

**Returns:** `void`

**Mechanisms:**
- Creates a `<style>` element with global `pointer-events: none` rule
- Removes the style element when restoring
- Used to temporarily disable all interactions

**Usage:**
```javascript
// Block all pointer interactions
fx_pointer_block(true);

// Restore pointer interactions
fx_pointer_block(false);
```

### fx_pointer_object

Gets the topmost element at the current mouse position, temporarily bypassing pointer blocking.

**Parameters:** None

**Returns:** `HTMLElement\|null` - The element at the mouse position

**Mechanisms:**
- Temporarily disables the pointer block style
- Uses `document.elementFromPoint` with current mouse coordinates
- Re-enables the pointer block style

**Usage:**
```javascript
// Get element under cursor even when pointer events are blocked
var element = fx_pointer_object();
if (element) {
    console.log("Element under cursor:", element.tagName);
}
```

## Event Update Functions

### fx_update_window_position

Updates global scroll position variables based on the current scroll container.

**Parameters:** None

**Returns:** `void`

**Mechanisms:**
- Reads scroll position from `fx_scroll_container`
- Adjusts mouse and touch coordinates to account for scroll changes
- Updates `fx_window_left` and `fx_window_top`

**Usage:**
```javascript
// Called automatically by scroll event listeners
// Can be called manually to sync state
fx_update_window_position();
```

### fx_update_window_size

Updates global window and document dimension variables.

**Parameters:** None

**Returns:** `void`

**Mechanisms:**
- Reads dimensions from `fx_scroll_container` or `documentElement`
- Updates `fx_document_width`, `fx_document_height`, `fx_window_width`, and `fx_window_height`

**Usage:**
```javascript
// Called automatically by resize event listeners
// Can be called manually to sync state
fx_update_window_size();
```

### fx_update_mouse_position

Updates global mouse position variables from a mouse event.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `e` | `MouseEvent` | - | Mouse event object |

**Returns:** `void`

**Mechanisms:**
- Sets both window-relative and document-relative coordinates
- Resets touch point 2 to `null`

**Usage:**
```javascript
// Called automatically by mouse event listeners
// Can be called manually with a custom event
document.addEventListener('mousemove', function(e) {
    fx_update_mouse_position(e);
});
```

### fx_update_touch_position

Updates global touch position variables from a touch event.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `e` | `TouchEvent` | - | Touch event object |

**Returns:** `void`

**Mechanisms:**
- Sets coordinates for both touch points
- Updates window-relative and document-relative positions
- Handles single and multi-touch scenarios

**Usage:**
```javascript
// Called automatically by touch event listeners
// Can be called manually with a custom event
document.addEventListener('touchmove', function(e) {
    fx_update_touch_position(e);
});
```

## Event Settings

### fx_noscroll

Sets a flag to prevent scrolling on touch devices.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `set` | `boolean` | `true` | Whether to prevent scrolling |

**Returns:** `void`

**Mechanisms:**
- Simply sets the `fx_noscroll_flag` variable
- Checked in touch event handlers to call `preventDefault`

**Usage:**
```javascript
// Prevent scrolling during a modal interaction
fx_noscroll(true);

// Re-enable scrolling
fx_noscroll(false);
```

## Event Management

### fx_event_raise

Raises a custom event to all registered callbacks.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `event` | `string` | - | Event name to raise |
| `e` | `Event` | - | Original event object |

**Returns:** `void`

**Mechanisms:**
- Defers execution to the next animation frame
- Sets `this` context to the calling element
- Calls `_fx_event_raise` internally

**Usage:**
```javascript
// Raise a custom event
fx_event_raise.call(document.body, 'custom_event', originalEvent);
```

### _fx_event_raise

Internal function that processes event callbacks.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `event` | `string` | - | Event name to process |
| `e` | `Event` | - | Original event object |

**Returns:** `void`

**Mechanisms:**
- Iterates through `fx_callback` array
- Filters callbacks by `fx_event` property
- Calls each matching callback with appropriate context
- Handles both object-style and function-style callbacks
- Removes "once" callbacks after execution
- Manages ghost click prevention flag

**Usage:**
```javascript
// Internal function, typically not called directly
// Called by fx_event_raise after animation frame delay
```

## Full Event Registration

### fx_register_callback

Registers a callback function for custom events.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `callback` | `function\|Object` | - | Callback to register |

**Returns:** `void`

**Mechanisms:**
- Adds callback to `fx_callback` array if not already present
- Callbacks can be plain functions or objects with `callback` and `fx_event` properties

**Usage:**
```javascript
// Register a callback for multiple events
function myHandler(event) {
    console.log("Event received:", event);
}
myHandler.fx_event = ['mousedown', 'keydown'];
fx_register_callback(myHandler);
```

### fx_unregister_callback

Removes a previously registered callback.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `callback` | `function\|Object` | - | Callback to remove |

**Returns:** `void`

**Mechanisms:**
- Finds and removes callback from `fx_callback` array
- Uses `indexOf` and `splice` for removal

**Usage:**
```javascript
// Unregister a previously registered callback
fx_unregister_callback(myHandler);
```

## Select Event Registration

### fx_event_listen

Registers event listeners with PWNC's event system.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `string\|HTMLElement\|Array` | - | Element(s) to listen on |
| `event` | `string\|Array` | - | Event name(s) to listen for |
| `_function` | `function` | `null` | Callback function |
| `passive` | `boolean` | `true` | Whether to use passive event listener |
| `capture` | `boolean` | `false` | Whether to use capture phase |

**Returns:** `void`

**Mechanisms:**
- Supports arrays for both `object` and `event` parameters
- For window/document objects, maps native events to PWNC event names
- For regular elements, uses standard `addEventListener`
- For string parameters, swaps them to support shorthand syntax
- Registers callbacks in the global `fx_callback` array

**Usage:**
```javascript
// Listen for mousedown on an element
fx_event_listen('myButton', 'mousedown', function(e) {
    console.log('Button pressed');
});

// Listen for multiple events
fx_event_listen('myElement', ['mousedown', 'mouseup'], function(e) {
    console.log(e.type, 'on element');
});

// Shorthand syntax
fx_event_listen('mousedown', 'myElement', function(e) {
    // This swaps parameters - event first, then element
});
```

### fx_event_remove

Removes event listeners registered through the PWNC system.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `object` | `HTMLElement\|Object` | - | Element or callback object |
| `event` | `string` | `""` | Event name to remove |
| `_function` | `function` | `null` | Specific function to remove |

**Returns:** `void`

**Mechanisms:**
- For PWNC-registered callbacks, removes from `fx_event` array
- If no events remain, unregisters the callback entirely
- For standard DOM elements, uses `removeEventListener`

**Usage:**
```javascript
// Remove a specific event from a callback
fx_event_remove(myCallback, 'mousedown');

// Remove all events from a callback
fx_event_remove(myCallback);

// Remove standard DOM listener
fx_event_remove(element, 'click', clickHandler);
```

## Ghost Event Busting

### fx_ghost_buster

Prevents ghost clicks on touch devices by intercepting mouse events.

**Parameters:** None

**Returns:** `void`

**Mechanisms:**
- Creates an `AbortController` for managing event listeners
- Listens for mouse events in capture phase
- Prevents events that don't match the expected touch detail
- Aborts after a click event to allow normal interaction

**Usage:**
```javascript
// Called automatically after touchstart
// Can be called manually to reset ghost click prevention
fx_ghost_buster();
```

## Event Initialization

The module automatically initializes by adding event listeners to `window` and `document` for:

- **Window events**: `load`, `resize`, `beforeunload`
- **Document events**: `DOMContentLoaded`, `scroll`
- **Mouse events**: `mousedown`, `mousemove`, `mouseup`, `mouseleave`, `dblclick`
- **Touch events**: `touchstart`, `touchmove`, `touchend`, `touchcancel`
- **Keyboard events**: `keydown`, `keypress`, `keyup`

These listeners update global state variables and raise PWNC custom events through the event system.

**Usage:**
```javascript
// No manual initialization needed
// The module self-initializes on load

// Example: Listen for PWNC custom events
fx_event_listen(window, 'window_load', function() {
    console.log('Window loaded');
});

fx_event_listen(document, 'mousedown', function(e) {
    console.log('Mouse down at:', fx_mouse_x, fx_mouse_y);
});


<!-- HASH:eb2135759222a4acccb1e8a17e7dea64 -->
