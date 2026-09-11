# PWNC API Documentation

[← Index](README.md) | [`index.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/index.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## index.php

This file serves as the entry point for the PWNC Web Platform's CMS module. It performs a minimal redirect to the desktop interface, ensuring users are routed to the main application interface upon accessing the root of the CMS module.

### Anonymous Function (IIFE)

#### What it does
The immediately-invoked function expression (IIFE) executes as soon as the file is loaded. It generates a redirect URL using the `cms_url()` utility function and sends an HTTP 303 "See Other" response to the client, directing them to the desktop module.

#### Parameters
None.

#### Return Values
None. The function terminates execution via `exit()` after sending the header.

#### Inner Mechanisms
1. **Namespace Declaration**: The file operates within the `cms` namespace, ensuring proper scoping of internal functions and classes.
2. **Module Initialization**: The `require("pwnc.inc")` statement loads the core PWNC framework, which includes essential utilities like `cms_url()`.
3. **URL Generation**: `cms_url(CMS_MODULES_URL . "desktop.php")` constructs a fully qualified URL to the desktop module, incorporating any necessary global state or parameters.
4. **HTTP Redirect**: The `header()` function sends a `Location` header with the generated URL, using status code `303` to indicate a temporary redirect suitable for POST-to-GET transitions.
5. **Termination**: `exit()` halts further script execution to prevent unintended output or processing.

#### Usage Context
This file is typically accessed when a user navigates to the base URL of the CMS module (e.g., `https://example.com/cms/`). It ensures that visitors are automatically redirected to the desktop interface without requiring manual navigation.

#### Example
When a user visits `https://example.com/cms/index.php`, the following occurs:
1. The server loads `index.php`.
2. The IIFE executes and calls `cms_url()` to build the desktop URL (e.g., `https://example.com/cms/modules/desktop.php`).
3. A `303 See Other` response is sent with the `Location` header set to the desktop URL.
4. The browser follows the redirect and loads the desktop interface.

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_MODULES_URL` | *(Defined in pwnc.inc)* | Base path to the modules directory within the CMS. |


<!-- HASH:4186a2bb651e5ec25c4d53b206e9a5b0 -->
