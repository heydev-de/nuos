# PWNC API Documentation

[← Index](../README.md) | [`javascript/defer.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/defer.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## defer.js

### Overview

`defer.js` is a lazy-loading and deferred resource initialization script for the PWNC Web Platform. It delays the loading of media elements (`AUDIO`, `IFRAME`, `IMG`, `VIDEO`) until they are needed — either when the page finishes loading or when the user scrolls near them. This improves initial page load performance by deferring non-critical assets.

The script uses `data-defer-src` attributes as placeholders for the actual `src` values, which are swapped in during processing. It also supports responsive image features like `srcset` and `sizes`, and intelligently sets `loading` and `fetchpriority` attributes based on element visibility within the current viewport.

---

## Global Variables

| Name | Type | Description |
|------|------|-------------|
| `defer_done` | `boolean` | Flag to ensure `defer_process()` runs only once per page lifecycle. |

---

## defer_process()

### Purpose

Processes all media elements marked with `data-defer-src`, determining their visibility relative to the current viewport, and then activates their real `src`, `srcset`, and `sizes` attributes. It also configures appropriate `loading` and `fetchpriority` behaviors.

### Parameters

None.

### Return Value

None.

### Inner Mechanisms

1. **Guard Clause**: If `defer_done` is already `true`, the function exits immediately to prevent duplicate execution.
2. **Viewport Calculation**:
   - Calls `fx_update_window_position()` and `fx_update_window_size()` to get current scroll and window dimensions.
   - Computes the bounding rectangle of the visible viewport (`page_x1`, `page_y1`, `page_x2`, `page_y2`).
3. **Element Collection**:
   - Queries the DOM for all `AUDIO`, `IFRAME`, `IMG`, and `VIDEO` elements that have a `data-defer-src` attribute.
4. **Visibility & Size Evaluation**:
   - For each element, calculates its position using `fx_offset_left()` and `fx_offset_top()`.
   - Checks if the element intersects with the viewport using simple bounding-box collision detection.
   - If the element lacks a `data-defer-sizes` attribute, computes an estimated width based on aspect ratio (from `width`/`height` attributes) and intrinsic size.
5. **Attribute Activation**:
   - Iterates over collected elements again.
   - If the element has `data-defer-srcset` or `srcset`, it sets the `sizes` attribute accordingly:
     - Uses `data-defer-sizes` if available.
     - Otherwise, uses the computed width.
   - Swaps `data-defer-srcset` → `srcset` if present.
   - Sets the real `src` from `data-defer-src`.
   - Removes all `data-defer-*` attributes after activation.
   - Sets `fetchpriority` to `"high"` for visible elements and `"low"` for hidden ones.
   - Sets `loading` to `"eager"` for visible elements and `"lazy"` for hidden ones.

### Usage Context

This function is automatically invoked either:
- On `document_load` event if the document is still parsing.
- On `window_load` event if the document has finished parsing.

It relies on several global utility functions:
- `fx_update_window_position()`
- `fx_update_window_size()`
- `fx_offset_left()`
- `fx_offset_top()`
- `fx_width()`
- `fx_height()`
- `fx_event_listen()`

These are assumed to be defined elsewhere in the PWNC frontend framework.

### Example

Given this HTML:

```html
<img data-defer-src="photo.jpg" width="800" height="600" />
<iframe data-defer-src="embed.html" data-defer-sizes="500px"></iframe>
```

After `defer_process()` runs:

```html
<img src="photo.jpg" fetchpriority="low" loading="lazy" />
<iframe src="embed.html" sizes="500px" fetchpriority="low" loading="lazy"></iframe>
```

If the image was visible in the viewport at the time of processing, it would instead receive:

```html
<img src="photo.jpg" fetchpriority="high" loading="eager" />
```


<!-- HASH:1d8f565981dcfc96c41fdfcb7770417e -->
