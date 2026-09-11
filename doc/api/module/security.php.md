# PWNC API Documentation

[← Index](../README.md) | [`module/security.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/security.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Security Module

The `security.php` module serves as a security event handler within the PWNC Web Platform. It is responsible for displaying security-related messages to users, particularly for Cross-Site Request Forgery (CSRF) protection events. When a security event is triggered, this module renders an HTML page that informs the user about the security issue and provides navigation options.

### Module Overview

This module operates as an immediately-invoked function expression (IIFE) that:

1. Loads the core PWNC framework via `pwnc.inc`
2. Renders a complete HTML document with appropriate headers and styling
3. Handles different security events through a switch statement
4. Displays contextual messages based on the security event type
5. Provides navigation links for user recovery

### Global Variables

| Variable | Type | Description |
|----------|------|-------------|
| `$event` | string | The security event identifier (e.g., `CMS_SECURITY_EVENT_CSRF`) |
| `$location` | string | The URL or location associated with the security event |

### Event Handling

#### `CMS_SECURITY_EVENT_CSRF`

**Purpose**: Handles CSRF (Cross-Site Request Forgery) security events by displaying an appropriate message to the user.

**Parameters**:
- `$event`: Should be set to `CMS_SECURITY_EVENT_CSRF` to trigger this case
- `$location`: Optional URL string indicating where the CSRF attempt was detected

**Return Values**: None (outputs HTML directly)

**Inner Mechanisms**:
1. Checks if `$location` is empty using `stre()` function
2. If empty, displays a generic CSRF message using `CMS_L_MOD_SECURITY_003`
3. If not empty, displays the specific location using `x()` for XML escaping
4. Generates navigation links using `cms_url()` for URL generation
5. Applies `x()` for XML escaping on all output to prevent XSS vulnerabilities

**Usage Context**:
This event is typically triggered when the platform detects a potential CSRF attack, such as when a form submission lacks a valid CSRF token or when suspicious cross-origin requests are detected.

**Example**:
```php
// Triggering a CSRF security event
$event = CMS_SECURITY_EVENT_CSRF;
$location = "https://example.com/suspicious-request";
// Include the security module to display the warning page
include("module/security.php");
```

### Output Structure

The module generates a complete HTML document with:

1. **DOCTYPE and HTML structure**: Using `CMS_DOCTYPE_HTML` constant
2. **Head section**: Including `CMS_HTML_HEADER` and `CMS_STYLESHEET`
3. **Body**: With class from `CMS_CLASS` constant
4. **Content section**: Containing:
   - A heading (`CMS_L_MOD_SECURITY_001`)
   - A descriptive paragraph with contextual information
   - Navigation links for user action

### Security Features

1. **Output Escaping**: All dynamic content is escaped using `x()` function to prevent XSS attacks
2. **URL Generation**: Uses `cms_url()` for safe URL construction
3. **Conditional Display**: Adapts message content based on available context
4. **Immediate Termination**: Calls `exit()` after rendering to prevent further execution

### Language Constants

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_SECURITY_001` | Main heading for security events |
| `CMS_L_MOD_SECURITY_002` | Message template with location placeholder |
| `CMS_L_MOD_SECURITY_003` | Generic message when no location is specified |
| `CMS_L_MOD_SECURITY_004` | Link text for returning to home page |
| `CMS_L_MOD_SECURITY_005` | Link text for following the specified location |

### Typical Usage Scenario

When a user submits a form without a valid CSRF token, the platform would:

1. Detect the invalid token
2. Set `$event` to `CMS_SECURITY_EVENT_CSRF`
3. Optionally set `$location` to the referring URL
4. Include this security module to display a warning page
5. Terminate execution to prevent the unauthorized action

This approach ensures users are informed about security issues while maintaining the platform's security posture through proper output escaping and controlled execution flow.


<!-- HASH:78c24bb9e960c6bfd666132729ef959e -->
