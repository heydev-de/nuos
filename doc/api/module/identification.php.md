# PWNC API Documentation

[← Index](../README.md) | [`module/identification.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/identification.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Identification Module

The `identification.php` module serves as the entry point for user authentication within the PWNC Web Platform. It handles the initial identification prompt displayed to users when they access the system. This module checks for cookie availability, manages cookie-based session initialization, and renders the appropriate identification interface.

### Core Logic Flow

The module operates through an immediately-invoked function expression (IIFE) that executes upon inclusion. The flow follows these steps:

1. **Cookie Availability Check**: Verifies if the `cms_check_cookie` cookie exists.
2. **Cookie Setting Attempt**: If the cookie is missing, attempts to set it and redirect.
3. **Fallback Handling**: If cookies cannot be set, displays a no-cookie warning.
4. **Identification Prompt**: If cookies are available, renders the identification form.

### Cookie Management

| Variable | Type | Description |
|----------|------|-------------|
| `$_COOKIE["cms_check_cookie"]` | string | Cookie identifier used to verify browser cookie support |
| `$_GET["cms_check_cookie"]` | string | GET parameter fallback for cookie verification |
| `$location` | string | Global variable storing the current URL for redirection |

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `cms_check_cookie` | array | Cookie configuration array passed to `cms_set_cookie()` |
| `location` | string | URL parameter for maintaining state during redirection |

### Return Values

This module does not return values directly. Instead, it outputs HTML content and may send HTTP headers for redirection.

### Inner Mechanisms

#### Cookie Verification Process

When no `cms_check_cookie` cookie is present:
1. Checks if the `cms_check_cookie` GET parameter is also absent.
2. Attempts to set the cookie using `cms_set_cookie(["cms_check_cookie" => 1])`.
3. If successful, constructs a redirect URL using `cms_url()` with both the cookie flag and current location.
4. Sends a `Refresh` header to redirect the browser (preferred over `Location` to ensure cookie persistence).
5. Outputs an HTML page informing the user of the redirect.

#### No-Cookie Fallback

If cookie setting fails:
1. Outputs an HTML page with the `CMS_L_NOCOOKIE` message.
2. Informs the user that cookies must be enabled to proceed.

#### Identification Prompt Rendering

When cookies are confirmed available:
1. Outputs the HTML document structure with JavaScript and stylesheet includes.
2. Loads the identification form template from `CMS_MODULES_PATH . "#module/mod.identification.inc"` via a nested IIFE.
3. Closes the HTML document.

### Usage Context

This module is typically loaded automatically by the PWNC framework when a user accesses a protected resource without an active session. It acts as a gatekeeper, ensuring that:
- Browser cookie support is verified before proceeding.
- Users are redirected appropriately during the cookie-setting process.
- The identification interface is only shown when the environment supports session management.

### Example Scenario

When a user navigates to a protected admin page:
1. The framework includes `identification.php`.
2. The module detects no `cms_check_cookie` cookie.
3. It sets the cookie and redirects back to the original page.
4. On the second request, the cookie is present, so the identification form is displayed.
5. The user enters credentials, which are processed by subsequent authentication logic.

### Key Functions Used

| Function | Purpose |
|----------|---------|
| `cms_set_cookie()` | Sets cookies with PWNC-specific configuration |
| `cms_url()` | Generates URLs while preserving global state parameters |
| `x()` | Escapes strings for safe HTML output |
| `sprintf()` | Formats the redirect message with the target URL |

### Constants Referenced

| Constant | Description |
|----------|-------------|
| `CMS_DOCTYPE_HTML` | HTML doctype declaration |
| `CMS_HTML_HEADER` | Standard HTML head content |
| `CMS_STYLESHEET` | CSS stylesheet link tag |
| `CMS_JAVASCRIPT` | JavaScript include tag |
| `CMS_CLASS` | Body class attribute value |
| `CMS_L_MOD_IDENTIFICATION_007` | Language string for redirect message |
| `CMS_L_NOCOOKIE` | Language string for cookie-disabled message |
| `CMS_MODULES_PATH` | Filesystem path to module templates |


<!-- HASH:a8fd0f5846bc4b94f0cda39efdd757a6 -->
