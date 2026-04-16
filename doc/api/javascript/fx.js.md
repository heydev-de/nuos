# NUOS API Documentation

[← Index](../README.md) | [`javascript/fx.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview

The `fx.js` file provides a comprehensive set of utility functions for handling animations, element manipulation, window management, event handling, and touch interactions in the NUOS web platform. It abstracts common frontend tasks such as element positioning, dimension calculations, swipe gestures, and event management to simplify development and ensure consistent behavior across different devices and browsers.

---

## Animation

### `fx_animation_frame`
Schedules a callback function to be executed on the next animation frame, optionally after a delay.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| callback  | function   | -       | Function to execute on the next animation frame. Can be a string (eval'd). |
| delay     | number     | 0       | Delay in milliseconds before executing the callback.                       |

**Return Value:**
- `number`: ID of the scheduled animation frame or timeout (for cleanup).

**Inner Mechanisms:**
- If `callback` is a string, it is converted to a function using `new Function()`.
- If `delay` is greater than 0, the callback is wrapped in a `setTimeout` before being passed to `requestAnimationFrame`.

**Usage:**
- Used for smooth animations or delayed execution of UI updates.
- Example: `fx_animation_frame(() => console.log("Animated!"), 100);`

---

## Element Manipulation

### `fx_move`
Moves an element to specified coordinates.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| left      | number     | -       | Target left position in pixels.                                            |
| top       | number     | -       | Target top position in pixels.                                             |

**Inner Mechanisms:**
- Converts `left` and `top` to integers.
- Compares current and target positions to avoid unnecessary style updates.

**Usage:**
- Repositioning UI elements dynamically.
- Example: `fx_move("myElement", 100, 200);`

---

### `fx_style`
Gets or sets CSS style properties on an element.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| property  | string     | -       | CSS property name (e.g., "color").                                         |
| value     | string     | null    | Value to set. If `null`, retrieves the current value.                      |
| priority  | boolean    | false   | If `true`, sets the property with `!important`.                            |

**Return Value:**
- `string|boolean`: Current property value (if `value` is `null`), or `true` on success.

**Inner Mechanisms:**
- Uses `setProperty` and `removeProperty` for style manipulation.
- Falls back to `getComputedStyle` for retrieval.

**Usage:**
- Dynamic styling of elements.
- Example: `fx_style("myElement", "opacity", "0.5");`

---

### `fx_visible`
Gets or sets the visibility of an element.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| set       | boolean    | null    | If `true`, makes the element visible; if `false`, hides it.                |

**Return Value:**
- `boolean`: Current visibility state if `set` is `null`.

**Inner Mechanisms:**
- Delegates to `fx_style` for visibility control.

**Usage:**
- Toggling element visibility.
- Example: `fx_visible("myElement", false);`

---

### `fx_change_image`
Changes the `src` attribute of an `<img>` element.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| image_url | string     | -       | New image URL.                                                             |

**Inner Mechanisms:**
- Checks if the element is an `<img>` before updating `src`.

**Usage:**
- Dynamic image updates.
- Example: `fx_change_image("myImage", "new-image.jpg");`

---

## Window Manipulation

### `fx_scrollto`
Smoothly scrolls the window to bring an element into view.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |

**Inner Mechanisms:**
- Uses a damping effect for smooth scrolling.
- Calculates intermediate positions over 200 frames.
- Delegates to `fx_animation_frame` for scheduling.

**Usage:**
- Scrolling to specific elements (e.g., after user interaction).
- Example: `fx_scrollto("sectionHeader");`

---

### `fx_adjust_window`
Adjusts the size and position of a popup window to fit its content.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object     | window  | Window object to adjust.                                                   |

**Inner Mechanisms:**
- Uses `fx_document_size` to calculate content dimensions.
- Resizes and repositions the window to fit within screen bounds.

**Usage:**
- Optimizing popup windows for content visibility.
- Example: `fx_adjust_window(myPopupWindow);`

---

## Element Positioning

### `fx_left`
Gets the left position of an element relative to the document or its offset parent.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| relative  | boolean    | false   | If `true`, returns `offsetLeft`; otherwise, uses `fx_offset_left`.         |

**Return Value:**
- `number`: Left position in pixels.

**Usage:**
- Positioning calculations.
- Example: `const leftPos = fx_left("myElement");`

---

### `fx_top`
Gets the top position of an element relative to the document or its offset parent.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| relative  | boolean    | false   | If `true`, returns `offsetTop`; otherwise, uses `fx_offset_top`.           |

**Return Value:**
- `number`: Top position in pixels.

**Usage:**
- Positioning calculations.
- Example: `const topPos = fx_top("myElement");`

---

### `fx_offset_left`
Calculates the left position of an element, accounting for cropping by parent elements.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| no_cropping | boolean  | false   | If `true`, ignores parent cropping.                                        |

**Return Value:**
- `number`: Left position in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` for accurate positioning.
- Iterates through parent elements to adjust for cropping.

**Usage:**
- Precise positioning in complex layouts.
- Example: `const offsetLeft = fx_offset_left("myElement", true);`

---

### `fx_offset_top`
Calculates the top position of an element, accounting for cropping by parent elements.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| no_cropping | boolean  | false   | If `true`, ignores parent cropping.                                        |

**Return Value:**
- `number`: Top position in pixels.

**Inner Mechanisms:**
- Similar to `fx_offset_left` but for vertical positioning.

**Usage:**
- Precise positioning in complex layouts.
- Example: `const offsetTop = fx_offset_top("myElement");`

---

## Element Dimensions

### `fx_width`
Gets the width of an element, accounting for cropping by parent elements.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| no_cropping | boolean  | false   | If `true`, ignores parent cropping.                                        |

**Return Value:**
- `number`: Width in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` for accurate measurement.
- Iterates through parent elements to adjust for cropping.

**Usage:**
- Dynamic layout calculations.
- Example: `const width = fx_width("myElement");`

---

### `fx_height`
Gets the height of an element, accounting for cropping by parent elements.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| no_cropping | boolean  | false   | If `true`, ignores parent cropping.                                        |

**Return Value:**
- `number`: Height in pixels.

**Inner Mechanisms:**
- Similar to `fx_width` but for vertical measurement.

**Usage:**
- Dynamic layout calculations.
- Example: `const height = fx_height("myElement");`

---

## Window Positioning

### `fx_position_left`
Gets the horizontal scroll position as a percentage of the document width.

**Return Value:**
- `number`: Scroll position as a percentage (0-100).

**Usage:**
- Tracking scroll progress.
- Example: `const scrollPercent = fx_position_left();`

---

### `fx_position_top`
Gets the vertical scroll position as a percentage of the document height.

**Return Value:**
- `number`: Scroll position as a percentage (0-100).

**Usage:**
- Tracking scroll progress.
- Example: `const scrollPercent = fx_position_top();`

---

## Document Dimensions

### `fx_document_size`
Calculates the total dimensions of the document content.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object     | window  | Window object to measure.                                                  |

**Return Value:**
- `object|null`: `{ width: number, height: number }` or `null` if no content.

**Inner Mechanisms:**
- Uses a stack-based approach to traverse the DOM and calculate maximum dimensions.
- Skips elements with `transform` or `auto` dimensions.

**Usage:**
- Determining the full size of dynamic content.
- Example: `const docSize = fx_document_size();`

---

## Swipe Functionality

### `fx_swipe`
Enables swipe gesture detection on an element.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| callback  | function   | -       | Function to call on swipe. Receives `(object, direction)` where direction is "l", "r", "u", or "d". |

**Inner Mechanisms:**
- Tracks touch/mouse movements to detect swipe direction.
- Uses thresholds (20px) to distinguish swipes from taps.

**Usage:**
- Implementing swipeable carousels or menus.
- Example: `fx_swipe("swipeArea", (el, dir) => console.log(dir));`

---

## Move/Pinch Functionality

### `fx_move_zoom`
Enables move and pinch-to-zoom gestures on an element.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string | -    | DOM element or its ID.                                                     |
| callback  | function   | -       | Function to call on move/zoom. Receives `(object, vx, vy, z, zx, zy)` where `vx`/`vy` are movement vectors, `z` is zoom delta, and `zx`/`zy` are zoom center coordinates. |

**Inner Mechanisms:**
- Supports both mouse and touch events.
- Handles single-finger moves, two-finger pinches, and mouse wheel zooming.
- Includes inertia/flick effects for smooth transitions.

**Usage:**
- Implementing interactive maps or image viewers.
- Example: `fx_move_zoom("zoomArea", (el, vx, vy, z) => console.log(vx, vy, z));`

---

## Miscellaneous

### `fx_pointer_block`
Blocks or unblocks all pointer events on the page.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| set       | boolean    | true    | If `true`, blocks pointer events; if `false`, restores them.               |

**Inner Mechanisms:**
- Creates or removes a global CSS style that disables pointer events.

**Usage:**
- Preventing interactions during loading or animations.
- Example: `fx_pointer_block(true);`

---

### `fx_pointer_object`
Gets the element under the mouse pointer, ignoring any pointer-blocking styles.

**Return Value:**
- `object|null`: DOM element under the pointer.

**Inner Mechanisms:**
- Temporarily disables the pointer-blocking style to query the element.

**Usage:**
- Debugging or custom pointer interactions.
- Example: `const element = fx_pointer_object();`

---

## Global Event Values

| Variable               | Type    | Description                                                                 |
|------------------------|---------|-----------------------------------------------------------------------------|
| fx_document_width      | number  | Total width of the document.                                               |
| fx_document_height     | number  | Total height of the document.                                              |
| fx_window_left         | number  | Current horizontal scroll position.                                        |
| fx_window_top          | number  | Current vertical scroll position.                                          |
| fx_window_width        | number  | Width of the viewport.                                                     |
| fx_window_height       | number  | Height of the viewport.                                                    |
| fx_mouse_key           | number  | Current mouse button state (1: left, 2: right, 3: middle).                 |
| fx_mouse_x             | number  | Mouse X position relative to the document.                                 |
| fx_mouse_y             | number  | Mouse Y position relative to the document.                                 |
| fx_mouse_window_x      | number  | Mouse X position relative to the viewport.                                 |
| fx_mouse_window_y      | number  | Mouse Y position relative to the viewport.                                 |
| fx_touch1_x            | number  | First touch X position relative to the document.                           |
| fx_touch1_y            | number  | First touch Y position relative to the document.                           |
| fx_touch1_window_x     | number  | First touch X position relative to the viewport.                           |
| fx_touch1_window_y     | number  | First touch Y position relative to the viewport.                           |
| fx_touch2_x            | number  | Second touch X position relative to the document (or `null`).              |
| fx_touch2_y            | number  | Second touch Y position relative to the document (or `null`).              |
| fx_touch2_window_x     | number  | Second touch X position relative to the viewport (or `null`).              |
| fx_touch2_window_y     | number  | Second touch Y position relative to the viewport (or `null`).              |
| fx_keyboard_key        | number  | Last pressed key code.                                                     |
| fx_event_object        | object  | Current event object.                                                      |
| fx_scroll_container    | object  | Container element for scrolling (window or body).                          |

---

## Event Update Functions

### `fx_update_window_position`
Updates global scroll position variables.

**Inner Mechanisms:**
- Adjusts mouse/touch positions to account for scrolling.

**Usage:**
- Internal use during event handling.

---

### `fx_update_window_size`
Updates global document and viewport dimensions.

**Usage:**
- Internal use during event handling.

---

### `fx_update_mouse_position`
Updates global mouse position variables.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| e         | object     | Mouse event object.                                                        |

**Usage:**
- Internal use during event handling.

---

### `fx_update_touch_position`
Updates global touch position variables.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| e         | object     | Touch event object.                                                        |

**Usage:**
- Internal use during event handling.

---

## Event Settings

| Variable              | Type    | Description                                                                 |
|-----------------------|---------|-----------------------------------------------------------------------------|
| fx_noscroll_flag      | boolean | If `true`, prevents default scroll behavior on touch devices.              |
| fx_nodebounce_flag    | boolean | If `true`, disables event debouncing.                                      |
| fx_noaniframe_flag    | boolean | If `true`, disables animation frame scheduling for events.                 |

### `fx_noscroll`
Enables or disables scroll prevention.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| set       | boolean    | true    | If `true`, prevents scrolling.                                             |

---

### `fx_nodebounce`
Enables or disables event debouncing.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| set       | boolean    | true    | If `true`, disables debouncing.                                            |

---

### `fx_noaniframe`
Enables or disables animation frame scheduling for events.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| set       | boolean    | true    | If `true`, disables animation frames.                                      |

---

## Event Management

### `fx_event_callback`
Default event callback function that updates global state variables.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| event     | string     | Event type.                                                                 |
| e         | object     | Event object.                                                              |

**Usage:**
- Internal use for event handling.

---

### `fx_event_raise`
Raises an event and processes it through all registered callbacks.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| event     | string     | Event type.                                                                 |
| e         | object     | Event object.                                                              |

**Inner Mechanisms:**
- Uses `requestAnimationFrame` for smooth event processing.
- Maintains a stack of event states to ensure consistency across callbacks.

**Usage:**
- Internal use for event propagation.

---

### `fx_event_consume`
Cleans up the event stack after processing.

**Usage:**
- Internal use for event management.

---

## Event Registration

### `fx_register_callback`
Registers a callback function to receive events.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| callback  | function   | Callback function to register.                                             |

**Usage:**
- Adding custom event handlers.
- Example: `fx_register_callback(myHandler);`

---

### `fx_unregister_callback`
Unregisters a callback function.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| callback  | function   | Callback function to unregister.                                           |

**Usage:**
- Removing event handlers.
- Example: `fx_unregister_callback(myHandler);`

---

### `fx_event_listen`
Adds an event listener to an object or registers a callback for specific events.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object/string/array | - | DOM element, its ID, or an array of elements.                             |
| event     | string     | -       | Event type.                                                                 |
| _function | function   | null    | Callback function. If `null`, `event` is treated as the callback.          |
| passive   | boolean    | true    | If `true`, marks the listener as passive.                                  |
| capture   | boolean    | false   | If `true`, uses capture phase.                                             |

**Inner Mechanisms:**
- Maps standard events to internal event types (e.g., "load" → "window_load").
- Supports both direct event listeners and callback registration.

**Usage:**
- Adding event listeners to DOM elements.
- Example: `fx_event_listen("myButton", "click", () => console.log("Clicked!"));`

---

### `fx_event_remove`
Removes an event listener or unregisters a callback.

| Parameter | Type       | Default | Description                                                                 |
|-----------|------------|---------|-----------------------------------------------------------------------------|
| object    | object     | -       | DOM element or callback function.                                          |
| event     | string     | ""      | Event type (for callbacks).                                                |
| _function | function   | null    | Callback function (for DOM elements).                                      |

**Usage:**
- Removing event listeners.
- Example: `fx_event_remove(myButton, "click", myHandler);`

---

## Debouncing

### `fx_event_debounce`
Debounces an event to limit its frequency.

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| event     | string     | Event type.                                                                 |
| e         | object     | Event object.                                                              |

**Inner Mechanisms:**
- Limits event processing to 30 FPS.
- Uses a queue to ensure events are processed in order.

**Usage:**
- Internal use for performance optimization.

---

## Ghost Event Busting

### `fx_ghost_buster`
Prevents ghost clicks and duplicate events on touch devices.

**Inner Mechanisms:**
- Uses an `AbortController` to manage event listeners.
- Blocks events that do not match expected touch behavior.

**Usage:**
- Internal use for touch event handling.

---

## Event Initialization
The file automatically initializes event listeners for:
- Window load, resize, and unload.
- Document load.
- Mouse and touch events.
- Keyboard events.

**Usage:**
- No manual initialization required.


<!-- HASH:a729fb7761479d2a11ebc9c60b4751f9 -->
