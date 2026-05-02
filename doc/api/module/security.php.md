# NUOS API Documentation

[← Index](../README.md) | [`module/security.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Security Handler (`module/security.php`)

This module provides a centralized security event handler for the NUOS platform, primarily focused on CSRF (Cross-Site Request Forgery) attack mitigation. It renders a user-friendly error page when a security violation is detected, ensuring users are informed and can safely return to the application.

---

### Overview

The `security.php` module is a self-contained, immediately-invoked function expression (IIFE) that:
- Outputs a complete HTML document with consistent NUOS styling.
- Handles security-related events (currently only CSRF).
- Displays contextual error messages with a link to return to the root of the application.
- Terminates script execution after rendering the response.

This module is designed to be lightweight, dependency-free, and invoked only in exceptional security contexts.

---

### Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$event` | Global | The security event identifier (e.g., `CMS_SECURITY_EVENT_CSRF`). Determines which security violation occurred. |
| `$location` | Global | The URL or context where the security violation was detected. Used to provide user feedback. |
| `CMS_DOCTYPE_HTML` | Constant | HTML5 doctype declaration. |
| `CMS_HTML_HEADER` | Constant | Standard HTML `<head>` content (meta tags, title, etc.). |
| `CMS_STYLESHEET` | Constant | Link to the platform-wide CSS stylesheet. |
| `CMS_CLASS` | Constant | CSS class name for the `<body>` element. |
| `CMS_SECURITY_EVENT_CSRF` | Constant | Event identifier for CSRF violations. |
| `CMS_L_MOD_SECURITY_001` | Constant | Localized heading: "Security Violation". |
| `CMS_L_MOD_SECURITY_002` | Constant | Localized message template: "The request from %s was denied due to security restrictions." |
| `CMS_L_MOD_SECURITY_003` | Constant | Localized fallback text: "an unknown location". |
| `CMS_L_MOD_SECURITY_004` | Constant | Localized link text: "Return to Homepage". |
| `CMS_ROOT_URL` | Constant | Base URL of the application (e.g., `/`). |

---

### Core Logic

The module uses a `switch` statement to handle different security events. Currently, only the `CMS_SECURITY_EVENT_CSRF` case is implemented, with a `default` case that falls back to it.

#### Event: `CMS_SECURITY_EVENT_CSRF`

##### Purpose
Renders a user-facing error page when a CSRF token validation fails, preventing unauthorized or malicious state-changing requests.

##### Inner Mechanisms
1. **Contextual Message**: Uses `sprintf()` to insert the origin of the request (`$location`) into a localized string.
2. **Fallback Handling**: If `$location` is empty (`stre($location)`), it substitutes "an unknown location".
3. **Escaping**: All dynamic output is escaped using `x()` (XML/HTML escaping) to prevent XSS.
4. **Navigation Link**: Provides a link back to the application root using `cms_url(CMS_ROOT_URL)` for safe URL generation.

##### Output Structure
- A `<div>` containing:
  - An `<h1>` with the security violation heading.
  - A `<p>` with the contextual error message.
  - An `<a>` link to return to the homepage.

##### Termination
After rendering, the script calls `exit()` to halt further execution, ensuring no unintended behavior follows a security violation.

---

### Usage Context

#### When to Use
- This module is **not called directly by developers**.
- It is **automatically invoked** by the NUOS platform when:
  - A CSRF token is missing or invalid in a state-changing request (e.g., form submission, AJAX call).
  - The platform's security layer detects a potential attack vector.

#### Typical Scenarios
1. **Form Submission**: A user submits a form without a valid CSRF token (e.g., expired session, missing token).
2. **AJAX Request**: A background request fails CSRF validation.
3. **Direct URL Access**: A user attempts to access a sensitive endpoint directly without proper authentication or tokens.

#### Integration with Other Utilities
- **`cms_url()`**: Used to generate safe, absolute URLs with proper escaping and CSRF protection.
- **`x()`**: Ensures all dynamic content is XML/HTML-escaped to prevent injection.
- **`stre()`**: Checks if `$location` is empty to provide a user-friendly fallback.

---

### Example Output (CSRF Event)
```html
<!DOCTYPE html>
<html>
<head>
    <!-- Standard NUOS head content -->
</head>
<body class="nuos-body">
    <section>
        <div>
            <h1>Security Violation</h1>
            <p>The request from /admin/settings was denied due to security restrictions.</p>
            <a href="/">Return to Homepage</a>
        </div>
    </section>
</body>
</html>
```

---

### Best Practices
- **Do not modify this file** unless extending security event handling.
- **Ensure CSRF tokens** are included in all state-changing requests (forms, AJAX) via `cms_param()` or `querystring()`.
- **Localization**: All user-facing strings are constants, allowing for easy translation without modifying logic.


<!-- HASH:832c7b9aca88a2fc626d62f2d5724706 -->
