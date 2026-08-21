# PWNC API Documentation

[← Index](../README.md) | [`module/security.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/security.php)

- **Version:** `26.6.17.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Security Module (`module/security.php`)

The **Security Module** handles security-related events in the PWNC Web Platform, primarily focusing on **CSRF (Cross-Site Request Forgery)** protection. When triggered, it renders a user-friendly error page with navigation options to return to the previous location or the homepage.

This module is **automatically invoked** by the platform when a CSRF token validation fails, ensuring secure state transitions across requests.

---

### **Constants**
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_SECURITY_EVENT_CSRF` | `csrf` | Event identifier for CSRF violations. |
| `CMS_L_MOD_SECURITY_001` | Localized string | Title for the CSRF error page (e.g., "Security Violation"). |
| `CMS_L_MOD_SECURITY_002` | Localized string | Error message template (e.g., "Invalid request from %s"). |
| `CMS_L_MOD_SECURITY_003` | Localized string | Fallback text for empty `$location` (e.g., "an unknown source"). |
| `CMS_L_MOD_SECURITY_004` | Localized string | Link text for homepage navigation (e.g., "Return to Homepage"). |
| `CMS_L_MOD_SECURITY_005` | Localized string | Link text for previous location (e.g., "Go Back"). |

---

### **Core Logic**
The module operates as an **immediately-invoked function expression (IIFE)** that:
1. **Renders a full HTML page** with platform-standard doctype, headers, and styles.
2. **Switches on the `$event`** (currently only handles `CMS_SECURITY_EVENT_CSRF`).
3. **Displays a CSRF error message** with:
   - A title and descriptive text (including the `$location` if available).
   - Navigation links to the previous page (if `$location` exists) or the homepage.
4. **Terminates execution** via `exit()` to prevent further processing.

---

### **Key Functions/Mechanisms**
#### **1. Event Handling**
- **Purpose**: Determines the security event type and renders the appropriate response.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$event` | `string` | Security event identifier (e.g., `CMS_SECURITY_EVENT_CSRF`). |
  | `$location` | `string` | The URL or path where the violation occurred (used for navigation links). |
- **Return Value**: None (outputs HTML directly).
- **Inner Mechanisms**:
  - Uses `stre()`/`nstre()` to check if `$location` is empty.
  - Escapes dynamic content with `x()` for HTML safety.
  - Generates URLs using `cms_url()` with CSRF protection.
- **Usage Context**:
  - Triggered by the platform’s core security layer when a CSRF token is missing or invalid.
  - Example: A form submission without a valid token redirects here.

---

#### **2. URL Generation**
- **`cms_url()`** (imported from `pwnc.inc`):
  - **Purpose**: Constructs a secure URL with CSRF tokens and merged parameters.
  - **Usage in Module**:
    ```php
    cms_url($location)  // Generates a URL for the previous location.
    cms_url(CMS_ROOT_URL . "index.php")  // Generates a URL for the homepage.
    ```

---

### **Usage Example**
#### **Scenario: CSRF Violation Handling**
When a user submits a form without a valid CSRF token, the platform:
1. Sets `$event = CMS_SECURITY_EVENT_CSRF`.
2. Sets `$location` to the referring page (e.g., `/account/settings`).
3. Invokes this module, which renders:
   ```html
   <div>
     <h1>Security Violation</h1>
     <p>Invalid request from /account/settings</p>
     <a href="/account/settings?csrf=valid_token">Go Back</a> |
     <a href="/index.php?csrf=valid_token">Return to Homepage</a>
   </div>
   ```

---

### **Integration Notes**
- **Automatic Invocation**: No direct calls are needed; the platform routes security events here.
- **Customization**: Override localized strings (e.g., `CMS_L_MOD_SECURITY_001`) in language files.
- **Extensibility**: Add new `case` blocks to the `switch` statement for additional security events (e.g., `CMS_SECURITY_EVENT_XSS`).


<!-- HASH:78c24bb9e960c6bfd666132729ef959e -->
