# PWNC API Documentation

[← Index](../README.md) | [`module/mcp.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/mcp.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp.php

The `mcp.php` file serves as the entry point for the **Model Context Protocol (MCP)** integration within the PWNC Web Platform. It implements an OAuth 2.0 authorization server tailored for MCP clients, enabling secure delegation of access to platform resources. The module handles authentication flows, token issuance, and request routing to internal scripts or external proxies.

### Key Responsibilities:
- Implements OAuth 2.0 endpoints for metadata, registration, authorization code flow, and token exchange.
- Validates incoming requests against expected parameters and security constraints.
- Routes resolved internal PHP scripts directly or delegates to an HTTP proxy for external/static resources.
- Manages session state via caching mechanisms for temporary storage of authorization codes.

---

## Core Functions

### `$resolve($target)`
Resolves a given target URL into a valid local PHP script path within the PWNC installation root.

| Parameter | Type   | Description |
|-----------|--------|-------------|
| `$target` | string | A URL or path to resolve. |

**Returns:**  
- `string|false`: Resolved absolute file path if valid; `FALSE` otherwise.

**Mechanisms:**
1. Parses the target URL using `parse_url()`.
2. Checks host and scheme consistency with current server context.
3. Resolves the path relative to the application root.
4. Ensures the final path is contained within the root directory and ends with `.php`.

**Usage Example:**
```php
$path = $resolve("https://example.com/desktop.php");
if ($path !== false) {
    require($path);
}
```

---

### `$response($value, $http_code)`
Sends a JSON-encoded HTTP response and terminates execution.

| Parameter   | Type    | Description |
|-------------|---------|-------------|
| `$value`    | mixed   | Data to encode as JSON. Arrays are sent directly; scalars wrapped in `{"error": "..."}`. |
| `$http_code`| int     | HTTP status code to send. Defaults to `400`. |

**Returns:**  
None — exits after sending headers and body.

**Usage Example:**
```php
$response(["access_token" => "abc123"], 200);
```

---

### `$verify_resource($value)`
Validates whether a provided resource URL matches the active platform URL.

| Parameter | Type   | Description |
|-----------|--------|-------------|
| `$value`  | string | Resource URL to validate. |

**Returns:**  
- `bool`: `TRUE` if the resource matches `CMS_ACTIVE_URL`; `FALSE` otherwise.

**Usage Example:**
```php
if (!$verify_resource($_REQUEST["resource"])) {
    $response("invalid_target");
}
```

---

## OAuth Message Handling

### Supported Messages:
Handled via `$_REQUEST["mcp_message"]` or inferred from well-known paths:

| Message       | Purpose |
|---------------|---------|
| `metadata`    | Returns OAuth resource metadata per RFC 8414. |
| `oauth`       | Provides OpenID Connect discovery configuration. |
| `register`    | Registers a new client dynamically. |
| `cancel`      | Redirects user back with error state. |
| `confirm`     | Confirms token retrieval during interactive auth. |
| `code`        | Handles initial authorization request. |
| `authorization_code` | Exchanges authorization code for access token. |

Each case processes specific input fields and enforces validation rules before proceeding.

---

## Request Routing

After handling OAuth logic, the script resolves the requested target:

1. If the target maps to a local PHP file, it loads that script directly.
2. Otherwise, it falls back to `http.php` as a proxy for external/static content.

Context variables like `$_SERVER['SCRIPT_NAME']`, `$_GET`, etc., are rewritten accordingly to simulate direct access.

**Example Flow:**
```php
// Client sends MCP request targeting desktop.php
$target = "https://platform.example.com/desktop.php";
$script = $resolve($target); // e.g., /var/www/desktop.php
require($script);
```

---

## Security Considerations

- All inputs are validated using strict checks (`stre`, `nstreq`) and parsed safely.
- Tokens are encrypted at rest using `encrypt()` and stored temporarily in cache.
- PKCE (Proof Key for Code Exchange) is enforced for authorization code exchanges.
- CSRF protection is implicitly handled through state management in `cms_param`.

---

## Constants Used

| Constant               | Description |
|------------------------|-------------|
| `CMS_ACTIVE_URL`       | Base URL of the active platform instance. |
| `CMS_MODULES_URL`      | Publicly accessible module base URL. |
| `CMS_L_MOD_MCP_*`      | Language strings for UI/messages. |
| `CMS_DOCTYPE_HTML`     | HTML doctype declaration. |
| `CMS_HTML_HEADER`      | Standard HTML head section. |
| `CMS_STYLESHEET`       | Stylesheet link tag(s). |
| `CMS_CLASS`            | Body class attribute value. |

---

## Typical Usage Scenario

A developer integrates an MCP-compatible IDE plugin pointing to `mcp.php`. When the plugin initiates an OAuth handshake:

1. Plugin requests `/mcp.php?mcp_message=metadata`.
2. Server responds with supported scopes and endpoints.
3. Plugin redirects user to `/mcp.php?mcp_message=code&client_id=...`.
4. User confirms access via form submission (`mcp_message=confirm`).
5. Authorization code issued and cached.
6. Plugin exchanges code for access token via `mcp_message=authorization_code`.
7. Server validates and returns bearer token.
8. Subsequent MCP calls include token in `Authorization` header.


<!-- HASH:3dd945ee9a95bf83fb71088070566b44 -->
