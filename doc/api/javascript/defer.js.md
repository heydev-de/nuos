# NUOS API Documentation

[← Index](../README.md) | [`javascript/defer.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Deferred Media Loading Module (`defer.js`)

Core JavaScript module responsible for **lazy-loading and prioritization** of media elements (`<img>`, `<audio>`, `<video>`, `<iframe>`) based on viewport visibility. Implements a **deferral pattern** where media sources are initially stored in `data-defer-*` attributes and swapped into standard `src`/`srcset` attributes only when the page is fully loaded. This reduces initial page load time and bandwidth usage while ensuring optimal resource prioritization.

---

### Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `defer_done` | `false` | Boolean flag preventing multiple executions of `defer_process()`. Set to `true` after first run. |

---

### `defer_process()`

#### Purpose
Orchestrates the **deferred loading** of media elements by:
1. Calculating viewport visibility for each deferred element.
2. Determining optimal `sizes` attribute values based on element dimensions.
3. Swapping `data-defer-*` attributes into standard `src`/`srcset` attributes.
4. Assigning `fetchpriority` and `loading` attributes based on visibility.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
1. **Viewport Calculation**:
   - Uses `fx_window_*` global variables (`fx_window_left`, `fx_window_top`, `fx_window_width`, `fx_window_height`) to define the visible viewport rectangle.
   - For each element, computes its bounding box (`object_x1`, `object_y1`, `object_x2`, `object_y2`) using `fx_offset_left()`, `fx_offset_top()`, `fx_width()`, and `fx_height()`.
   - Determines visibility by checking for overlap between the element's bounding box and the viewport.

2. **Dimension Handling**:
   - If `data-defer-sizes` is absent, calculates a default `sizes` value using the element's aspect ratio (if `width` and `height` attributes are present).
   - Falls back to `object.width` if no aspect ratio is available.

3. **Attribute Swapping**:
   - Moves `data-defer-src` to `src`.
   - Moves `data-defer-srcset` to `srcset` if present.
   - Sets `sizes` to either the value of `data-defer-sizes` or a calculated pixel value.

4. **Prioritization**:
   - Sets `fetchpriority="high"` and `loading="eager"` for visible elements.
   - Sets `fetchpriority="low"` and `loading="lazy"` for non-visible elements.

#### Usage Context
- **Automatically triggered** on `document_load` (if DOM is still loading) or `window_load` (if DOM is already complete).
- Designed for **static or dynamic content** where media elements are present but not immediately needed.
- **Critical for performance** in content-heavy pages (e.g., galleries, blogs, dashboards).

---

### Event Binding Logic
```javascript
if (document.readyState === "loading")
    fx_event_listen("document_load", defer_process);
else
    fx_event_listen("window_load", defer_process);
```

#### Purpose
Ensures `defer_process()` runs **once** after the DOM is fully loaded, regardless of the initial `readyState`.

#### Inner Mechanisms
- Checks `document.readyState` to determine the appropriate event:
  - `"loading"`: Binds to `document_load` (custom event fired when DOM parsing completes).
  - Otherwise: Binds to `window_load` (standard `load` event, fired after all resources are loaded).

#### Usage Context
- **Framework-agnostic** integration with NUOS's custom event system (`fx_event_listen`).
- Guarantees deferred media processing occurs **after** all other critical resources (CSS, JS) are loaded.


<!-- HASH:a82eb1926ff41ee5f212b420c6ada52f -->
