# NUOS API Documentation

[← Index](README.md) | [`index.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Root Redirector (`index.php`)

### Overview
This file serves as the entry point for the NUOS web platform. Its sole purpose is to redirect incoming root-level requests (`/`) to the platform's primary module, the desktop interface. This ensures users and developers are immediately directed to the main application environment upon accessing the base URL.

---

### Inner Mechanisms

#### Immediate Execution
The file uses an **Immediately Invoked Function Expression (IIFE)** to execute the redirect logic as soon as the script is loaded. This pattern prevents any unintended code execution or variable leakage into the global scope.

#### Redirect Logic
- **Header-Based Redirect**: Uses PHP’s `header()` function to issue an HTTP 303 ("See Other") redirect.
- **URL Resolution**: Leverages `cms_url()` to dynamically construct the target URL, ensuring consistency with the platform’s routing and parameter management systems.
- **Termination**: Calls `exit()` immediately after the redirect to prevent further script execution.

#### Key Dependencies
- **`nuos.inc`**: The core bootstrap file, loaded via `require()`. Provides access to all NUOS utilities, including `cms_url()`.
- **`CMS_MODULES_URL`**: A platform-defined constant pointing to the modules directory, used to resolve the path to `desktop.php`.

---

### Functions & Logic

#### Anonymous IIFE
```php
(function() {
    header("Location: " . cms_url(CMS_MODULES_URL . "desktop.php"), TRUE, 303);
    exit();
})();
```

| Aspect               | Details                                                                                     |
|----------------------|---------------------------------------------------------------------------------------------|
| **Purpose**          | Redirects the root request to the desktop module.                                          |
| **Parameters**       | None (anonymous function).                                                                  |
| **Return Values**    | None. Terminates script execution.                                                          |
| **Inner Mechanisms** | - Constructs the target URL using `cms_url()` to merge global state with the desktop path.  |
|                      | - Issues an HTTP 303 redirect via `header()`.                                               |
|                      | - Terminates execution with `exit()`.                                                       |
| **Usage Context**    | - **Entry Point**: Only executed when accessing the root URL (`/`).                         |
|                      | - **Development**: No direct usage; serves as a routing mechanism.                          |

---

### Constants & Variables

| Name               | Value/Default               | Description                                                                                     |
|--------------------|-----------------------------|-------------------------------------------------------------------------------------------------|
| `CMS_MODULES_URL`  | Platform-defined constant   | Base path to the modules directory (e.g., `/modules/`). Used to resolve `desktop.php`.         |

---

### Usage Scenarios

#### Typical Use Case
- **User Access**: When a user navigates to the root domain (e.g., `https://example.com/`), the server executes this script, redirecting them to `https://example.com/modules/desktop.php`.
- **Development**: Developers may modify this file to point to a different default module (e.g., during onboarding or testing).

#### Edge Cases
- **Disabled Redirects**: If `header()` calls are disabled (e.g., in CLI mode), the script will fail silently after `exit()`.
- **Custom Entry Points**: Override this behavior by replacing `index.php` or configuring server-level redirects (e.g., via `.htaccess`).

---

### Notes
- **Performance**: The IIFE pattern ensures minimal overhead, as no additional functions or classes are loaded.
- **Security**: The use of `cms_url()` guarantees CSRF protection and proper parameter escaping.
- **Extensibility**: To change the default module, replace `desktop.php` with another module path (e.g., `onboarding.php`).


<!-- HASH:3084fa4388e8934b5188bee17bc86d1e -->
