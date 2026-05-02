# NUOS API Documentation

[← Index](../README.md) | [`javascript/fx.js`](https://github.com/heydev-de/nuos/blob/main/nuos/javascript/fx.js)

- **Version:** `26.4.29.1`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Overview

The `fx.js` file provides a comprehensive set of utility functions for handling animations, element manipulation, window management, event handling, and touch interactions in the NUOS web platform. It establishes a robust event system with debouncing, touch emulation, and global state tracking for mouse, touch, and keyboard interactions. The file also includes helper functions for positioning, sizing, and styling DOM elements, as well as advanced gesture recognition (swipe, pinch, zoom).

---

## Animation

### `fx_animation_frame(callback, delay = 0)`

Schedules a callback function to be executed on the next animation frame, optionally after a delay.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| callback  | function or string | The function to execute. If a string, it is converted to a function.        |
| delay     | number             | Optional delay in milliseconds before executing the callback. Default: `0`. |

**Return Value:**
- Returns the request ID from `requestAnimationFrame` or `setTimeout`.

**Inner Mechanisms:**
- If `callback` is a string, it is converted to a function using `new Function()`.
- If `delay` is greater than `0`, the callback is wrapped in a `setTimeout` before being passed to `requestAnimationFrame`.

**Usage:**
- Used to create smooth animations by synchronizing with the browser's repaint cycle.
- Example: `fx_animation_frame(() => console.log("Animated!"), 100);`

---

## Element Manipulation

### `fx_move(object, left, top)`

Repositions a DOM element to specified coordinates.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| left      | number             | The target left position in pixels.                                         |
| top       | number             | The target top position in pixels.                                          |

**Return Value:**
- None.

**Inner Mechanisms:**
- Converts `object` to a DOM element if it is a string.
- Rounds `left` and `top` to the nearest integer.
- Only updates the style if the new position differs from the current one.

**Usage:**
- Example: `fx_move("myElement", 100, 200);`

---

### `fx_style(object, property, value = null, priority = false)`

Gets or sets a CSS style property on a DOM element.

| Parameter  | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| object     | object or string   | The DOM element or its ID.                                                  |
| property   | string             | The CSS property name (e.g., `"color"`).                                    |
| value      | string or boolean  | The value to set. If `false` or `""`, the property is removed. Default: `null`. |
| priority   | boolean            | If `true`, sets the property with `!important`. Default: `false`.           |

**Return Value:**
- If `value` is `null`, returns the computed value of the property.
- If setting a value, returns `true` on success.

**Inner Mechanisms:**
- Uses `setProperty` to apply styles with optional `!important`.
- Uses `removeProperty` to clear styles.
- Uses `getComputedStyle` to retrieve computed values.

**Usage:**
- Get: `const color = fx_style("myElement", "color");`
- Set: `fx_style("myElement", "color", "red", true);`

---

### `fx_visible(object, set = null)`

Gets or sets the visibility of a DOM element.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| set       | boolean or null    | If `true`, makes the element visible; if `false`, hides it. Default: `null`. |

**Return Value:**
- If `set` is `null`, returns `true` if the element is visible.
- If setting visibility, returns the result of `fx_style`.

**Inner Mechanisms:**
- Uses `fx_style` to toggle the `visibility` property between `"visible"` and `"hidden"`.

**Usage:**
- Get: `const isVisible = fx_visible("myElement");`
- Set: `fx_visible("myElement", false);`

---

### `fx_change_image(object, image_url)`

Changes the `src` attribute of an `<img>` element.

| Parameter  | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| object     | object or string   | The `<img>` element or its ID.                                              |
| image_url  | string             | The new image URL.                                                          |

**Return Value:**
- None.

**Inner Mechanisms:**
- Only updates the `src` if the element is an `<img>`.

**Usage:**
- Example: `fx_change_image("myImage", "new-image.jpg");`

---

## Window Manipulation

### `fx_scrollto(object)`

Smoothly scrolls the window to bring a DOM element into view.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |

**Return Value:**
- None.

**Inner Mechanisms:**
- Uses a custom animation loop with damping for smooth scrolling.
- Calculates the target position and animates the scroll container (window or a scrollable element).
- Uses `fx_animation_frame` to schedule scroll updates.

**Usage:**
- Example: `fx_scrollto("myElement");`

---

### `fx_adjust_window(object = window)`

Adjusts the size and position of a popup window to fit its content.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| object    | object  | The window object to adjust. Default: `window`.                             |

**Return Value:**
- None.

**Inner Mechanisms:**
- Only works for popup windows (those with an `opener`).
- Uses `fx_document_size` to determine the required dimensions.
- Resizes and repositions the window to fit within the screen bounds.

**Usage:**
- Example: `fx_adjust_window();`

---

## Element Positioning

### `fx_left(object, relative = false)`

Gets the left position of a DOM element.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| relative  | boolean            | If `true`, returns the offset relative to the parent. Default: `false`.     |

**Return Value:**
- The left position in pixels.

**Inner Mechanisms:**
- If `relative` is `true`, returns `offsetLeft`.
- Otherwise, uses `fx_offset_left` to account for cropping by parent elements.

**Usage:**
- Example: `const left = fx_left("myElement");`

---

### `fx_top(object, relative = false)`

Gets the top position of a DOM element.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| relative  | boolean            | If `true`, returns the offset relative to the parent. Default: `false`.     |

**Return Value:**
- The top position in pixels.

**Inner Mechanisms:**
- If `relative` is `true`, returns `offsetTop`.
- Otherwise, uses `fx_offset_top` to account for cropping by parent elements.

**Usage:**
- Example: `const top = fx_top("myElement");`

---

### `fx_offset_left(object, no_cropping = false)`

Gets the absolute left position of a DOM element, accounting for cropping by parent elements.

| Parameter    | Type               | Description                                                                 |
|--------------|--------------------|-----------------------------------------------------------------------------|
| object       | object or string   | The DOM element or its ID.                                                  |
| no_cropping  | boolean            | If `true`, ignores parent element cropping. Default: `false`.               |

**Return Value:**
- The absolute left position in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` to get the element's position relative to the viewport.
- If `no_cropping` is `false`, iterates through parent elements to find the maximum left position.

**Usage:**
- Example: `const absLeft = fx_offset_left("myElement");`

---

### `fx_offset_top(object, no_cropping = false)`

Gets the absolute top position of a DOM element, accounting for cropping by parent elements.

| Parameter    | Type               | Description                                                                 |
|--------------|--------------------|-----------------------------------------------------------------------------|
| object       | object or string   | The DOM element or its ID.                                                  |
| no_cropping  | boolean            | If `true`, ignores parent element cropping. Default: `false`.               |

**Return Value:**
- The absolute top position in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` to get the element's position relative to the viewport.
- If `no_cropping` is `false`, iterates through parent elements to find the maximum top position.

**Usage:**
- Example: `const absTop = fx_offset_top("myElement");`

---

## Element Dimensions

### `fx_width(object, no_cropping = false)`

Gets the width of a DOM element, accounting for cropping by parent elements.

| Parameter    | Type               | Description                                                                 |
|--------------|--------------------|-----------------------------------------------------------------------------|
| object       | object or string   | The DOM element or its ID.                                                  |
| no_cropping  | boolean            | If `true`, ignores parent element cropping. Default: `false`.               |

**Return Value:**
- The width in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` to get the element's dimensions.
- If `no_cropping` is `false`, iterates through parent elements to find the minimum right and maximum left positions.

**Usage:**
- Example: `const width = fx_width("myElement");`

---

### `fx_height(object, no_cropping = false)`

Gets the height of a DOM element, accounting for cropping by parent elements.

| Parameter    | Type               | Description                                                                 |
|--------------|--------------------|-----------------------------------------------------------------------------|
| object       | object or string   | The DOM element or its ID.                                                  |
| no_cropping  | boolean            | If `true`, ignores parent element cropping. Default: `false`.               |

**Return Value:**
- The height in pixels.

**Inner Mechanisms:**
- Uses `getBoundingClientRect` to get the element's dimensions.
- If `no_cropping` is `false`, iterates through parent elements to find the minimum bottom and maximum top positions.

**Usage:**
- Example: `const height = fx_height("myElement");`

---

## Window Positioning

### `fx_position_left()`

Gets the horizontal scroll position as a percentage of the document width.

**Return Value:**
- The scroll position as a percentage (e.g., `25.5`).

**Inner Mechanisms:**
- Uses `fx_document_width` and `fx_window_left` to calculate the percentage.

**Usage:**
- Example: `const posLeft = fx_position_left();`

---

### `fx_position_top()`

Gets the vertical scroll position as a percentage of the document height.

**Return Value:**
- The scroll position as a percentage (e.g., `50.0`).

**Inner Mechanisms:**
- Uses `fx_document_height` and `fx_window_top` to calculate the percentage.

**Usage:**
- Example: `const posTop = fx_position_top();`

---

## Document Dimensions

### `fx_document_size(object = window)`

Calculates the total dimensions of the document, including all child elements.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| object    | object  | The window object. Default: `window`.                                       |

**Return Value:**
- An object with `width` and `height` properties, or `null` if no elements are found.

**Inner Mechanisms:**
- Uses a stack-based approach to traverse the DOM and calculate the maximum right and bottom positions of all elements.

**Usage:**
- Example: `const size = fx_document_size();`

---

## Swipe Functionality

### `fx_swipe(object, callback)`

Enables swipe gesture detection on a DOM element.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| callback  | function           | A function to call when a swipe is detected. Receives `(object, direction)`. |

**Return Value:**
- None.

**Inner Mechanisms:**
- Tracks touch/mouse movements to detect swipe gestures (left, right, up, down).
- Uses `fx_event_listen` to bind touch and mouse events.
- Calls the callback with the detected direction (`"l"`, `"r"`, `"u"`, `"d"`).

**Usage:**
- Example:
  ```javascript
  fx_swipe("myElement", (obj, dir) => {
      console.log(`Swiped ${dir}`);
  });
  ```

---

## Move/Pinch Functionality

### `fx_move_zoom(object, callback)`

Enables move and zoom (pinch) gesture detection on a DOM element.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or string   | The DOM element or its ID.                                                  |
| callback  | function           | A function to call when a move or zoom is detected. Receives `(object, vx, vy, z, zx, zy)`. |

**Return Value:**
- None.

**Inner Mechanisms:**
- Tracks touch/mouse movements and wheel events to detect dragging and zooming.
- Uses `fx_event_listen` to bind touch, mouse, and wheel events.
- Calls the callback with movement vectors (`vx`, `vy`) and zoom details (`z`, `zx`, `zy`).

**Usage:**
- Example:
  ```javascript
  fx_move_zoom("myElement", (obj, vx, vy, z, zx, zy) => {
      console.log(`Moved: ${vx}, ${vy}; Zoomed: ${z} at ${zx}, ${zy}`);
  });
  ```

---

## Miscellaneous

### `fx_pointer_block(set = true)`

Blocks or unblocks all pointer events on the document.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| set       | boolean | If `true`, blocks pointer events; if `false`, unblocks them. Default: `true`. |

**Return Value:**
- None.

**Inner Mechanisms:**
- Creates or removes a `<style>` element that disables pointer events, touch actions, and user selection for all elements.

**Usage:**
- Example: `fx_pointer_block(true);`

---

### `fx_pointer_object()`

Gets the DOM element under the mouse pointer, even if pointer events are blocked.

**Return Value:**
- The DOM element under the pointer.

**Inner Mechanisms:**
- Temporarily disables the pointer-blocking style to use `document.elementFromPoint`.

**Usage:**
- Example: `const element = fx_pointer_object();`

---

## Global Event Values

The following global variables track the state of the mouse, touch, keyboard, and window:

| Variable               | Type    | Description                                                                 |
|------------------------|---------|-----------------------------------------------------------------------------|
| `fx_document_width`    | number  | The total width of the document.                                            |
| `fx_document_height`   | number  | The total height of the document.                                           |
| `fx_window_left`       | number  | The horizontal scroll position of the window.                               |
| `fx_window_top`        | number  | The vertical scroll position of the window.                                 |
| `fx_window_width`      | number  | The width of the viewport.                                                  |
| `fx_window_height`     | number  | The height of the viewport.                                                 |
| `fx_mouse_key`         | number  | The currently pressed mouse button (1: left, 2: middle, 3: right).          |
| `fx_mouse_x`           | number  | The mouse X position relative to the document.                              |
| `fx_mouse_y`           | number  | The mouse Y position relative to the document.                              |
| `fx_mouse_window_x`    | number  | The mouse X position relative to the viewport.                              |
| `fx_mouse_window_y`    | number  | The mouse Y position relative to the viewport.                              |
| `fx_touch1_x`          | number  | The primary touch X position relative to the document.                      |
| `fx_touch1_y`          | number  | The primary touch Y position relative to the document.                      |
| `fx_touch1_window_x`   | number  | The primary touch X position relative to the viewport.                      |
| `fx_touch1_window_y`   | number  | The primary touch Y position relative to the viewport.                      |
| `fx_touch2_x`          | number  | The secondary touch X position relative to the document (or `null`).        |
| `fx_touch2_y`          | number  | The secondary touch Y position relative to the document (or `null`).        |
| `fx_touch2_window_x`   | number  | The secondary touch X position relative to the viewport (or `null`).        |
| `fx_touch2_window_y`   | number  | The secondary touch Y position relative to the viewport (or `null`).        |
| `fx_keyboard_key`      | number  | The currently pressed keyboard key code.                                    |
| `fx_event_object`      | object  | The current event object.                                                   |
| `fx_scroll_container`  | object  | The scrollable container (window or `document.body`).                       |

---

## Event Update Functions

### `fx_update_window_position()`

Updates the global scroll position variables (`fx_window_left`, `fx_window_top`).

**Return Value:**
- None.

**Inner Mechanisms:**
- Adjusts mouse and touch positions to account for scrolling.

**Usage:**
- Called internally during scroll events.

---

### `fx_update_window_size()`

Updates the global document and viewport size variables.

**Return Value:**
- None.

**Inner Mechanisms:**
- Uses `document.documentElement` or `fx_scroll_container` to get dimensions.

**Usage:**
- Called internally during resize events.

---

### `fx_update_mouse_position(e)`

Updates the global mouse position variables.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| e         | object  | The mouse event object.                                                     |

**Return Value:**
- None.

**Inner Mechanisms:**
- Extracts `clientX` and `clientY` from the event and adjusts for scroll position.

**Usage:**
- Called internally during mouse events.

---

### `fx_update_touch_position(e)`

Updates the global touch position variables.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| e         | object  | The touch event object.                                                     |

**Return Value:**
- None.

**Inner Mechanisms:**
- Extracts positions from the first and second touch points (if available).

**Usage:**
- Called internally during touch events.

---

## Event Settings

### `fx_noscroll(set = true)`

Enables or disables scroll prevention.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| set       | boolean | If `true`, prevents scrolling. Default: `true`.                             |

**Return Value:**
- None.

**Inner Mechanisms:**
- Sets the `fx_noscroll_flag` global variable.

**Usage:**
- Example: `fx_noscroll(true);`

---

### `fx_nodebounce(set = true)`

Enables or disables event debouncing.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| set       | boolean | If `true`, disables debouncing. Default: `true`.                            |

**Return Value:**
- None.

**Inner Mechanisms:**
- Sets the `fx_nodebounce_flag` global variable.

**Usage:**
- Example: `fx_nodebounce(false);`

---

### `fx_noaniframe(set = true)`

Enables or disables animation frame usage for events.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| set       | boolean | If `true`, disables `requestAnimationFrame` for events. Default: `true`.    |

**Return Value:**
- None.

**Inner Mechanisms:**
- Sets the `fx_noaniframe_flag` global variable.

**Usage:**
- Example: `fx_noaniframe(false);`

---

## Event Management

### `fx_event_callback(event, e)`

The default event callback that updates global state variables.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| event     | string  | The event name (e.g., `"mousedown"`).                                       |
| e         | object  | The event object.                                                           |

**Return Value:**
- None.

**Inner Mechanisms:**
- Updates global variables based on the event type (e.g., mouse position, keyboard key).

**Usage:**
- Called internally by the event system.

---

### `fx_event_raise(event, e)`

Raises an event and processes it through the callback chain.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| event     | string  | The event name.                                                             |
| e         | object  | The event object.                                                           |

**Return Value:**
- None.

**Inner Mechanisms:**
- Uses `requestAnimationFrame` to defer event processing unless `fx_noaniframe_flag` is set.
- Calls `_fx_event_raise` to process the event.

**Usage:**
- Called internally by event listeners.

---

### `_fx_event_raise(event, e)`

Processes an event through the callback chain and updates global state.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| event     | string  | The event name.                                                             |
| e         | object  | The event object.                                                           |

**Return Value:**
- None.

**Inner Mechanisms:**
- Saves the current global state.
- Processes each callback in the chain, restoring state between callbacks.
- Handles event-specific logic (e.g., touch emulation, ghost busting).

**Usage:**
- Called internally by `fx_event_raise`.

---

### `fx_event_consume()`

Cleans up event state after processing.

**Return Value:**
- None.

**Inner Mechanisms:**
- Clears the event state for the current slot.

**Usage:**
- Called internally after event processing.

---

## Full Event Registration

### `fx_register_callback(callback)`

Registers a callback function to receive events.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| callback  | function | The callback function.                                                     |

**Return Value:**
- None.

**Inner Mechanisms:**
- Adds the callback to the `fx_callback` array if not already present.

**Usage:**
- Example: `fx_register_callback(myCallback);`

---

### `fx_unregister_callback(callback)`

Unregisters a callback function.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| callback  | function | The callback function to remove.                                            |

**Return Value:**
- None.

**Inner Mechanisms:**
- Sets the callback to `null` in the `fx_callback` array.

**Usage:**
- Example: `fx_unregister_callback(myCallback);`

---

## Select Event Registration

### `fx_event_listen(object, event, _function = null, passive = true, capture = false)`

Binds an event listener to a DOM element or registers a callback for global events.

| Parameter  | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| object     | object, string, or array | The DOM element, its ID, or an array of elements.                          |
| event      | string             | The event name.                                                             |
| _function  | function           | The callback function. Default: `null`.                                     |
| passive    | boolean            | If `true`, marks the listener as passive. Default: `true`.                  |
| capture    | boolean            | If `true`, uses capture phase. Default: `false`.                            |

**Return Value:**
- None.

**Inner Mechanisms:**
- For DOM elements, uses `addEventListener`.
- For global events (e.g., `"mousedown"`), registers the callback with event filtering.

**Usage:**
- DOM: `fx_event_listen("myElement", "click", myCallback);`
- Global: `fx_event_listen("mousedown", myCallback);`

---

### `fx_event_remove(object, event = "", _function = null)`

Removes an event listener or unregisters a callback.

| Parameter | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| object    | object or function | The DOM element or callback function.                                       |
| event     | string             | The event name. Default: `""`.                                              |
| _function | function           | The callback function. Default: `null`.                                     |

**Return Value:**
- None.

**Inner Mechanisms:**
- For DOM elements, uses `removeEventListener`.
- For callbacks, removes the event from the callback's `fx_event` array and unregisters it.

**Usage:**
- DOM: `fx_event_remove("myElement", "click", myCallback);`
- Global: `fx_event_remove(myCallback, "mousedown");`

---

## Debouncing

### `fx_event_debounce(event, e)`

Debounces an event to limit the rate of execution.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| event     | string  | The event name.                                                             |
| e         | object  | The event object.                                                           |

**Return Value:**
- None.

**Inner Mechanisms:**
- Uses a global object (`fx_event_debounce_list`) to track debounce state.
- If debouncing is active, schedules the event for later execution.

**Usage:**
- Called internally for debounceable events (e.g., `"mousemove"`).

---

### `_fx_event_debounce(event, e)`

Executes a debounced event.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| event     | string  | The event name.                                                             |
| e         | object  | The event object.                                                           |

**Return Value:**
- None.

**Inner Mechanisms:**
- Updates the debounce state and raises the event.

**Usage:**
- Called internally by `fx_event_debounce`.

---

## Ghost Event Busting

### `fx_ghost_buster()`

Prevents ghost clicks (duplicate events from touch interactions).

**Return Value:**
- None.

**Inner Mechanisms:**
- Uses an `AbortController` to manage event listeners.
- Blocks mouse events that do not match the expected touch emulation pattern.

**Usage:**
- Called internally during touch events.

---

## Event Initialization

The file initializes event listeners for the following events:
- Window: `load`, `resize`, `beforeunload`.
- Document: `DOMContentLoaded`.
- Mouse: `mousedown`, `mousemove`, `mouseup`, `mouseleave`, `dblclick`.
- Touch: `touchstart`, `touchmove`, `touchend`, `touchcancel`.
- Keyboard: `keydown`, `keypress`, `keyup`.

Each listener calls `fx_event_raise` or `fx_event_debounce` to process the event.


<!-- HASH:12a19248d1b16659d45579a5c2c8d1ab -->
