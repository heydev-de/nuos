# NUOS API Documentation

[← Index](../README.md) | [`module/identification.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Identification (`module/identification.php`)

**Overview:**
This module handles user identification and cookie availability checks for the NUOS web platform. It ensures that cookies are enabled in the user's browser before proceeding with authentication. If cookies are disabled, it displays an error message. If cookies are enabled, it loads the identification prompt (login form).

---

### **Core Logic Flow**

| Step | Condition | Action |
|------|-----------|--------|
| 1    | Cookie `cms_check_cookie` is **not set** | Check if the `cms_check_cookie` GET parameter is also missing. If so, set a test cookie and refresh the page. |
| 2    | Cookie `cms_check_cookie` is **not set** after refresh | Display an error message indicating cookies are disabled. |
| 3    | Cookie `cms_check_cookie` **is set** | Proceed to load the identification prompt (login form). |

---

### **Key Functions & Mechanisms**

#### **Cookie Availability Check**
- **Purpose:**
  Determines whether the user's browser supports cookies by attempting to set and verify a test cookie (`cms_check_cookie`).

- **Parameters:**
  None (relies on `$_COOKIE` and `$_GET` superglobals).

- **Return Values:**
  None (outputs HTML directly).

- **Inner Mechanisms:**
  - If the test cookie is not set, the script checks for a `cms_check_cookie` GET parameter.
  - If the parameter is missing, it sets the cookie and refreshes the page using `header("Refresh: 0")`.
  - If the cookie remains unset after refresh, it assumes cookies are disabled and displays an error.

- **Usage Context:**
  - **Mandatory pre-authentication step** for all NUOS modules requiring session management.
  - Ensures compliance with security policies requiring cookie support.

---

#### **Cookie Setting (`cms_set_cookie`)**
- **Purpose:**
  Wrapper for PHP’s `setcookie()` with NUOS-specific defaults (e.g., path, domain, secure flags).

- **Parameters:**

  | Name | Type | Description |
  |------|------|-------------|
  | `$data` | `array` | Associative array of cookie names and values. |

- **Return Values:**
  - `bool`: `TRUE` if the cookie was successfully set, `FALSE` otherwise.

- **Inner Mechanisms:**
  - Recursively processes the `$data` array to set multiple cookies.
  - Applies platform-wide defaults (e.g., `httponly`, `secure`).

- **Usage Context:**
  - Used internally by `identification.php` to set the test cookie.
  - Available globally for other modules requiring cookie management.

---

#### **URL Redirection (`cms_url`)**
- **Purpose:**
  Generates a fully qualified URL with query parameters, merging local and global state.

- **Parameters:**

  | Name | Type | Description |
  |------|------|-------------|
  | `$addr` | `array|string` | Target path or associative array of query parameters. |
  | `$param` | `array` | Additional query parameters to merge. |
  | `$omit` | `bool` | If `TRUE`, omits global parameters. |

- **Return Values:**
  - `string`: The generated URL.

- **Inner Mechanisms:**
  - Merges `$addr` and `$param` with the current request’s query string.
  - Applies CSRF protection via `cms_param()`.

- **Usage Context:**
  - Used to refresh the page after setting the test cookie.
  - Core utility for all URL generation in NUOS.

---

#### **HTML Output**
- **Purpose:**
  Renders the HTML skeleton for cookie-related messages or the identification prompt.

- **Structure:**
  - **Cookie Error:** Displays `CMS_L_NOCOOKIE` (localized string).
  - **Identification Prompt:** Loads `mod.identification.inc` (login form).

- **Usage Context:**
  - Directly outputs HTML; no return values.
  - Relies on NUOS constants (`CMS_DOCTYPE_HTML`, `CMS_HTML_HEADER`, etc.) for consistency.

---

### **Constants & Localization**

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_IDENTIFICATION_007` | Localized string for the cookie refresh message (e.g., "Redirecting to %s..."). |
| `CMS_L_NOCOOKIE` | Localized error message for disabled cookies. |
| `CMS_CLASS` | CSS class for the `<body>` element. |

---

### **Dependencies**
- **`nuos.inc`:** Core NUOS utilities (e.g., `cms_url`, `cms_set_cookie`).
- **`mod.identification.inc`:** Login form module (loaded dynamically).

---

### **Typical Usage Scenarios**
1. **Initial Page Load:**
   - User accesses a protected page → `identification.php` checks for cookies.
   - If cookies are disabled, the user sees an error.
   - If cookies are enabled, the login form loads.

2. **Cookie Test Redirect:**
   - After setting the test cookie, the page refreshes to verify cookie support.

3. **Integration with Other Modules:**
   - All NUOS modules requiring authentication include this file as a pre-check.


<!-- HASH:e0348895bf50f69c6b0fedc03be9e525 -->
