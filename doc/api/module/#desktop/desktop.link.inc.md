# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.link.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.link.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Link Module (`desktop.link.inc`)

The **Desktop Link** module provides functionality for embedding external URLs within the PWNC desktop interface. It handles:

- **URL validation** (via Content Security Policy and X-Frame-Options headers)
- **Embedding external content** in an `<iframe>`
- **Link management** (save, reload, breakout to full window)
- **User interaction** (input, load, breakout)

---

## Core Logic

### Message Handling (`CMS_IFC_MESSAGE`)

The module processes three interface messages:

| Message  | Purpose                                                                 |
|----------|-------------------------------------------------------------------------|
| `display`| Validates if a URL can be embedded (CSP/XFO checks) and redirects or shows a fallback UI. |
| `reload` | Resets the URL input field.                                             |
| `save`   | Persists the current URL to the desktop object.                         |

---

### `display` Message

#### Purpose
Validates whether an external URL can be embedded in an `<iframe>` by checking:
1. **Content Security Policy (CSP)** headers (`frame-ancestors` directive).
2. **X-Frame-Options (XFO)** headers (`DENY` or `SAMEORIGIN`).

If embedding is allowed, it redirects to the URL. Otherwise, it renders a fallback UI with a button to open the link in a new window.

#### Inner Mechanisms
1. **Caching**: Uses `cms_cache()` to store CSP/XFO validation results for 1 hour.
2. **Header Parsing**:
   - Splits CSP policies into tokens and checks for `frame-ancestors`.
   - Supports wildcards (`*`, `*.example.com`) and `self` in CSP.
   - Falls back to XFO if CSP is absent.
3. **Fallback UI**: Renders a button to open the URL in the top window if embedding is blocked.

#### Usage Example
```php
// Embed an external URL (e.g., in a desktop object)
$url = "https://example.com";
cms_url(["desktop_display" => "interface",
         "ifc_message" => "display",
         "ifc_param" => $url]);
```
- If `example.com` allows embedding, the user is redirected.
- If blocked, a button appears to open the link externally.

---

### `reload` Message

#### Purpose
Resets the URL input field to its default state.

#### Inner Mechanisms
- Clears `$ifc_param1` (the URL input value).

#### Usage Example
```php
// Trigger a reload (e.g., after canceling edits)
cms_url(["desktop_display" => "interface",
         "ifc_message" => "reload"]);
```

---

### `save` Message

#### Purpose
Saves the current URL to a desktop object.

#### Parameters
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$object`     | `string` | Desktop object ID.                   |
| `$ifc_param1` | `string` | URL to save.                         |

#### Return Value
- Updates `$ifc_response` with `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).

#### Inner Mechanisms
- Uses `$desktop->object_set()` and `$desktop->save()` to persist the URL.

#### Usage Example
```php
// Save a URL to a desktop object
$object = "my_link_object";
$url = "https://pwnc.it";
cms_url(["desktop_display" => "interface",
         "ifc_message" => "save",
         "ifc_param1" => $url,
         "object" => $object]);
```

---

## Main Display

### Purpose
Renders the primary UI for managing a desktop link, including:
- A URL input field.
- Buttons for **Save**, **Reload**, and **Breakout** (open in new window).
- An `<iframe>` to embed the URL.

### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `user`    | `string` | Desktop user ID (from `DESKTOP_USER`). |
| `object`  | `string` | Desktop object ID.                   |

### Inner Mechanisms
1. **URL Resolution**:
   - Uses `$desktop->object_get()` to fetch the saved URL.
   - Falls back to `blank.htm` if no URL is set.
2. **Security**:
   - The `<iframe>` includes permissive `allow` attributes for modern web APIs (camera, microphone, etc.).
3. **JavaScript Events**:
   - `desktop_link_load()`: Updates the `<iframe>` src when the URL input changes.
   - `desktop_link_breakout()`: Opens the URL in the top window.
   - Listens for `Enter` key in the URL input to trigger loading.

### Usage Example
```php
// Render the desktop link UI
$object = "my_link_object";
cms_url(["desktop_display" => "interface",
         "object" => $object]);
```

---

## JavaScript Functions

### `desktop_link_load()`
Loads the URL from the input field into the `<iframe>`.

### `desktop_link_breakout()`
Opens the current URL in the top window (breaks out of the iframe).

### Event Listeners
| Event          | Target               | Action                                  |
|----------------|----------------------|-----------------------------------------|
| `keydown`      | URL input            | Loads the URL on `Enter`.               |
| `load`         | `<iframe>`           | Updates the input field with the iframe's current URL. |
| `window_load`  | Window               | Blurs the URL input on page load.       |

---

## Constants and Labels

| Constant                     | Description                          |
|------------------------------|--------------------------------------|
| `CMS_L_DESKTOP_LINK_001`     | "Breakout" button label.             |
| `CMS_L_DESKTOP_LINK_002`     | "Link" (title).                      |
| `CMS_L_DESKTOP_LINK_003`     | "Reload" button label.               |
| `CMS_L_DESKTOP_LINK_004`     | "This page cannot be embedded."      |
| `CMS_L_DESKTOP_LINK_005`     | "Open in new window" button label.   |
| `CMS_L_COMMAND_SAVE`         | "Save" button label.                 |
| `CMS_L_URL`                  | "URL" input label.                   |

---

## Security Considerations
1. **CSP/XFO Validation**: Prevents clickjacking by blocking disallowed embeds.
2. **Escaping**: Uses `q()` for JavaScript strings and `x()` for HTML attributes.
3. **CSRF Protection**: Relies on `cms_url()` for parameter handling.
4. **Sandboxing**: The `<iframe>` is isolated but allows modern web APIs via `allow` attributes.


<!-- HASH:285eb4945f32792441de34dee6d41797 -->
