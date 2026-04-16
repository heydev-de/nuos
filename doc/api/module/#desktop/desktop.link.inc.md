# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.link.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Link Module (`desktop.link.inc`)

Core interface component for managing and displaying external or internal URLs within a desktop-like environment. This module provides a user interface for setting, saving, and interacting with URL-based content in an iframe, including breakout functionality to open the URL in the full browser window.

---

### Overview

The `desktop.link.inc` file handles:
- **URL Management**: Loading, displaying, and saving URLs associated with desktop objects.
- **User Interface**: Rendering an interactive iframe with controls for URL input, saving, and breakout navigation.
- **Event Handling**: JavaScript-driven interactions for dynamic URL updates and iframe management.

This module is typically used in scenarios where users need to embed or interact with external web content within the NUOS desktop environment, such as:
- Embedding third-party web applications.
- Displaying internal or external documentation.
- Providing a controlled environment for web-based tools.

---

### Interface Flow

#### Message Handling (`CMS_IFC_MESSAGE`)

The module responds to two interface commands:

| Command | Purpose | Inner Mechanism |
|---------|---------|-----------------|
| `reload` | Resets the interface state. | Clears `$ifc_param1` to force a reload of the default URL. |
| `save` | Saves the current URL to the desktop object. | Calls `$desktop->object_set()` to update the URL property of the object, then triggers a save operation. The response is set to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure. |

---

### Main Display Logic

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | Contextual data passed to the interface controller (`ifc`). Contains `user` (current desktop user) and `object` (target desktop object ID). |

#### URL Resolution

The URL is determined in the following priority order:
1. **Interface Parameter (`$ifc_param1`)**: If set and not empty, it is used as the URL.
2. **Desktop Object Property**: If no interface parameter is provided, the URL is fetched from the desktop object using `$desktop->object_get($object, "url")`.
3. **Fallback**: If no URL is found, the default `CMS_URL . "blank.htm"` is used.

The resolved URL is then processed through `cms_url()` to ensure it is properly formatted and context-aware.

---

### Interface Controller (`ifc`)

The interface controller is initialized with the following properties:

| Property | Type | Description |
|----------|------|-------------|
| **Title** | `string` | `CMS_L_DESKTOP_LINK_002` (localized title for the link interface). |
| **Commands** | `array` | Associative array of command labels and their actions: <br> - `CMS_L_DESKTOP_LINK_003 \| desktop/command_refresh` → `reload` <br> - `CMS_L_COMMAND_SAVE \| desktop/command_save` → `save` <br> - `CMS_L_DESKTOP_LINK_001 \| desktop/command_breakout` → JavaScript breakout function. |
| **Parameters** | `array` | `$param` (user and object context). |

#### Interface Fields

| Field Label | Type | Description |
|-------------|------|-------------|
| `CMS_L_URL` | `text` | Input field for the URL, limited to 60 characters and 256 bytes. Pre-populated with the resolved URL. |
| `Laden` | `button` | Button labeled "Laden" (German for "Load") that triggers `desktop_link_load()` to reload the iframe with the current input URL. |

---

### HTML Output

The module renders the following HTML structure:

1. **Menu Button**:
   ```html
   <a id="ifc-desktop-link-menu" href="javascript:;">…</a>
   ```
   - Triggers the interface command menu.

2. **Iframe**:
   ```html
   <iframe id="desktop-link-output" src="[escaped URL]" allow="[permissions]"></iframe>
   ```
   - **`id`**: `desktop-link-output` (used for JavaScript targeting).
   - **`src`**: The resolved and escaped URL, processed through `x(cms_url($url))` for XML safety.
   - **`allow`**: Comprehensive permissions for modern web APIs (camera, microphone, geolocation, fullscreen, etc.).

---

### JavaScript Functions

#### `desktop_link_breakout()`
**Purpose**: Opens the current iframe URL in the full browser window, replacing the current page.
**Parameters**: None.
**Return**: None.
**Mechanism**:
- Calls `window.location.replace()` with the URL encoded via `q()` for JavaScript safety.
**Usage**: Triggered by the "Breakout" command in the interface.

#### `desktop_link_load()`
**Purpose**: Reloads the iframe with the URL entered in the input field.
**Parameters**: None.
**Return**: None.
**Mechanism**:
- Retrieves the current value of the `ifc_param1` input field using `ifc_get()`.
- Updates the iframe `src` attribute to the new URL.
**Usage**: Triggered by the "Laden" button or pressing Enter in the URL input field.

#### Event Listeners

| Event | Target | Handler | Purpose |
|-------|--------|---------|---------|
| `keydown` | `ifc_param1` | Checks for `Enter` key; prevents default and calls `desktop_link_load()`. | Enables keyboard-driven URL submission. |
| `load` | `iframe` | Attempts to update `ifc_param1` with the iframe's current `location.href`. | Syncs the input field with the iframe's URL. |
| `window_load` | `window` | Blurs `ifc_param1` to remove focus from the input field. | Improves UX by preventing accidental input on page load. |

---

### Closing

The interface controller is closed with `$ifc->close()`, which finalizes the HTML output and cleans up resources.

---

### Typical Usage Scenarios

1. **Embedding External Tools**:
   - Users configure a desktop object to point to an external web application (e.g., a dashboard or analytics tool).
   - The iframe provides a sandboxed environment for interaction.

2. **Internal Content Links**:
   - URLs can point to internal NUOS routes (e.g., `content://page_id` or `media://file_id`), resolved via `translate_url()` in other parts of the system.

3. **Controlled Breakout**:
   - Users can "break out" of the iframe to view the content in the full browser window when needed.

4. **Persistent URL Storage**:
   - URLs are saved to the desktop object, allowing users to return to the same content across sessions.


<!-- HASH:32e21e4984d55f8c50e23d991f235e42 -->
