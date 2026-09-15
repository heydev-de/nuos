# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.link.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.link.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Link Interface

This file implements the **Desktop Link** interface within the PWNC Web Platform. It provides a mechanism for users to interact with external URLs through an embedded iframe, while respecting security policies such as Content Security Policy (CSP) and X-Frame-Options headers. The interface allows users to load external content, save the current URL, reload the view, or break out of the iframe to navigate directly to the target URL.

The file is structured around a `switch` statement driven by the `CMS_IFC_MESSAGE` constant, which determines the current operation mode:

- `display`: Handles rendering and security checks for displaying external content.
- `reload`: Resets parameters for a fresh display.
- `save`: Persists the current URL associated with a desktop object.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | (string) | Determines the current interface action (`display`, `reload`, `save`). |
| `$ifc_param` | (string) | The URL being processed or displayed. |
| `$ifc_param1` | (string) | Secondary parameter used for storing or retrieving the URL. |
| `$cache_id` | (string) | Cache key for storing CSP/XFO header results. |
| `$flag` | (bool\|NULL) | Indicates whether the URL is allowed to be framed. |
| `$found` | (bool) | Tracks if a valid `frame-ancestors` directive was found in CSP. |
| `$header` | (array\|FALSE) | HTTP response headers retrieved from the target URL. |
| `$url` | (string) | Final resolved URL to be displayed or saved. |
| `$object` | (string) | Identifier for the desktop object being manipulated. |
| `$desktop` | (object) | Reference to the desktop session handler. |

---

## Message Handling: Display Mode

### Purpose

When `CMS_IFC_MESSAGE == "display"`, this section performs the following actions:

1. Checks a cache for previously computed CSP/XFO results.
2. If not cached, retrieves HTTP headers from the target URL.
3. Parses CSP `frame-ancestors` directives to determine if the URL can be safely embedded.
4. Falls back to checking `X-Frame-Options` if no CSP is present.
5. Caches the result and either redirects or renders a fallback UI.

### Inner Mechanisms

#### Caching Logic

```php
$cache_id = "desktop.link_csp.$ifc_param";
$flag = (cms_cache_time($cache_id) >= (time() - 3600)) ? cms_cache($cache_id) : NULL;
```

Uses `cms_cache()` to store and retrieve the result of the CSP/XFO check for one hour.

#### Header Retrieval

```php
if (($flag === NULL) && cms_load("http") && (($header = http_header($ifc_param)) !== FALSE))
```

Loads the `http` library and fetches headers from the target URL using `http_header()`.

#### CSP Parsing

Iterates over comma/semicolon-separated CSP directives. For each `frame-ancestors` directive:

- Splits tokens and evaluates them against the current domain.
- Supports special values like `'self'`, `'none'`, and wildcards (`*`).
- Converts wildcard patterns into regex for matching.

#### XFO Fallback

If no `frame-ancestors` directive is found, checks `X-Frame-Options`:

- `deny`: Blocks framing.
- `sameorigin`: Allows only if the domain matches `CMS_DOMAIN`.

#### Redirect or Fallback UI

If `$flag` is true, issues a 301 redirect to the target URL. Otherwise, renders a JavaScript-based fallback that attempts to redirect via `window.top.location.replace()`.

### Usage Example

```php
// Assume CMS_IFC_MESSAGE = "display"
// and $ifc_param = "https://example.com/page"
// This will:
// 1. Check cache for CSP result
// 2. Fetch headers from example.com
// 3. Evaluate frame-ancestors policy
// 4. Either redirect or show fallback UI
```

---

## Message Handling: Reload Mode

### Purpose

Resets secondary parameters to prepare for a new display cycle.

### Code

```php
case "reload":
    $ifc_param1 = NULL;
    break;
```

Clears `$ifc_param1` so the next display starts fresh.

### Usage Example

Triggered when the user clicks the "Reload" button in the interface.

---

## Message Handling: Save Mode

### Purpose

Persists the current URL to the desktop object's configuration.

### Code

```php
case "save":
    $desktop->object_set($object, "url", $ifc_param1);
    $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

Sets the `url` property of the desktop object and saves the state.

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Desktop object identifier. |
| `$ifc_param1` | string | URL to save. |

### Return Values

- `$ifc_response`: Set to `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

### Usage Example

```php
// User clicks "Save"
// $ifc_param1 = "https://example.com/saved-page"
// Saves the URL to the desktop object
```

---

## Main Display Rendering

### Purpose

Renders the main interface including:

- URL input field
- Load button
- Embedded iframe for content display
- JavaScript for dynamic behavior

### Key Components

#### URL Resolution

```php
if (isset($ifc_param1) && nstre($ifc_param1)) $url = $ifc_param1;
elseif (stre($url = $desktop->object_get($object, "url"))) $url = CMS_URL . "blank.htm";
$url = cms_url($url, NULL, TRUE);
```

Determines the final URL to display, falling back to a blank page if none is set.

#### Interface Setup

Creates an `ifc` instance with commands for:

- Reloading the view
- Saving the current URL
- Breaking out of the iframe

#### HTML Output

Includes:

- A styled container div
- An anchor menu toggle (`…`)
- An iframe pointing to the internal display endpoint

#### JavaScript Functions

| Function | Description |
|----------|-------------|
| `desktop_link_breakout()` | Navigates the top window to the current URL. |
| `desktop_link_load()` | Updates the iframe source with the input URL. |
| `keydown` listener | Triggers `desktop_link_load()` on Enter key. |
| `load` listener | Syncs input field with iframe's current location. |
| `window_load` listener | Blurs the input field after load. |

### Usage Example

```php
// Renders the full desktop link interface
// User can type a URL, click "Laden" (Load), and view content in iframe
// Can also save the URL or break out to navigate directly
```

---

## Security Considerations

- **CSP/XFO Checks**: Prevents clickjacking by validating framing policies.
- **Cache Layer**: Reduces repeated header lookups for the same URL.
- **Escaping**: Uses `q()` for JS-safe output and `x()` for XML-safe attributes.
- **Redirect Handling**: Falls back to client-side redirect if server-side is blocked.

---

## Summary

This file implements a secure, user-friendly interface for loading and managing external URLs within the PWNC desktop environment. It balances usability with security by enforcing modern web standards like CSP and XFO, while providing intuitive controls for navigation, saving, and breakout functionality.


<!-- HASH:285eb4945f32792441de34dee6d41797 -->
