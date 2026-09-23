# PWNC API Documentation

[← Index](../README.md) | [`module/mcp.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/mcp.php)

- **Version:** `26.9.21.15`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp.php — MCP Protocol Handler Module

This file implements the **Model Context Protocol (MCP)** entry point for the PWNC Web Platform. It serves as a bridge between an MCP client (e.g., an AI assistant or external tool) and the internal PWNC system, enabling secure, authenticated access to platform resources via OAuth 2.0 and PKCE flows.

The module handles:

- **OAuth 2.0 metadata discovery**
- **Client registration**
- **Authorization code flow with PKCE**
- **Token exchange**
- **Resource resolution and proxying**

It also resolves incoming MCP requests to internal PHP scripts, rewriting the request context accordingly.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MCP` | `TRUE` | Prevents self-integration; ensures the module is only loaded once. |
| `CMS_MCP_TARGET` | Path string | Defines the resolved target script path for execution in the global scope. |

---

## Helper Functions

### `$resolve($target)`

Resolves a given URL target into a valid local PHP file path within the PWNC root directory.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$target` | `string` | The target URL to resolve. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Absolute filesystem path to the resolved PHP script. |
| `FALSE` | If the target is invalid, outside the root, or not a `.php` file. |

#### Inner Mechanisms

1. Parses the target URL.
2. Validates that the host matches the current server (`HTTP_HOST`).
3. Validates the scheme (`http` or `https`) against the current environment.
4. Resolves the document root using `__FILE__` and `SCRIPT_NAME`.
5. Checks if the path points to a directory and appends `index.php` if so.
6. Ensures the final path is contained within the root and ends with `.php`.

#### Usage Example

```php
$path = $resolve("https://example.com/desktop.php");
if ($path !== FALSE) {
    require($path);
}
```

---

## OAuth 2.0 Endpoints

### Metadata Endpoint (`metadata`)

Returns OAuth resource metadata compliant with [RFC 9700](https://datatracker.ietf.org/doc/html/rfc9700).

#### Response

```json
{
  "type": "oauth-resource-metadata",
  "resource": "https://your.pwnc.instance/",
  "authorization_servers": ["https://your.pwnc.instance/?mcp_message=oauth"],
  "scopes_supported": ["mcp"],
  "bearer_methods_supported": ["header"]
}
```

---

### OAuth Configuration (`oauth`)

Returns OpenID Connect / OAuth 2.0 configuration for the MCP service.

#### Response

```json
{
  "issuer": "https://your.pwnc.instance/",
  "authorization_endpoint": "https://your.pwnc.instance/",
  "token_endpoint": "https://your.pwnc.instance/",
  "registration_endpoint": "https://your.pwnc.instance/?mcp_message=register",
  "scopes_supported": ["mcp"],
  "response_types_supported": ["code"],
  "grant_types_supported": ["authorization_code"],
  "code_challenge_methods_supported": ["S256", "plain"],
  "token_endpoint_auth_methods_supported": ["none"],
  "protected_resources": ["https://your.pwnc.instance/"]
}
```

---

### Client Registration (`register`)

Registers a new MCP client dynamically.

#### Input (JSON Body)

| Field | Type | Description |
|-------|------|-------------|
| `redirect_uris` | `array` | List of allowed redirect URIs. |

#### Response

```json
{
  "client_id": "<random_hex>",
  "client_id_issued_at": 1712345678,
  "redirect_uris": ["https://client.example.com/callback"],
  "token_endpoint_auth_method": "none",
  "grant_types": ["authorization_code"],
  "response_types": ["code"]
}
```

---

### Authorization Code Flow (`code`, `confirm`)

Handles the interactive authorization step where the user confirms granting access.

#### Parameters

| Name | Source | Description |
|------|--------|-------------|
| `client_id` | `$_REQUEST` | Identifier of the requesting client. |
| `redirect_uri` | `$_REQUEST` | URI to redirect back after authorization. |
| `state` | `$_REQUEST` | Opaque value used by the client to maintain state. |
| `code_challenge` | `$_REQUEST` | PKCE challenge. |
| `code_challenge_method` | `$_REQUEST` | Either `S256` or `plain`. |
| `resource` | `$_REQUEST` | Resource indicator (must match active URL). |
| `token` | `$_REQUEST` | Existing permission token (for confirmation). |

#### Behavior

- On `code`: Validates parameters and renders an HTML form for user confirmation.
- On `confirm`: Verifies the provided token, generates a one-time authorization code, stores it in cache, and redirects with the code.

#### Example HTML Form Output

```html
<form method="post" action="/mcp.php">
  <input type="hidden" name="client_id" value="...">
  <input type="hidden" name="redirect_uri" value="...">
  <input type="hidden" name="state" value="...">
  <input type="hidden" name="code_challenge" value="...">
  <input type="hidden" name="code_challenge_method" value="S256">
  <label for="mcp-token">Enter your token:</label><br>
  <input id="mcp-token" name="token" type="text"><br>
  <button name="mcp_message" type="submit" value="cancel">Cancel</button>
  <button name="mcp_message" type="submit" value="confirm">Confirm</button>
</form>
```

---

### Token Exchange (`authorization_code`)

Exchanges an authorization code for an access token.

#### Parameters

| Name | Source | Description |
|------|--------|-------------|
| `code` | `$_REQUEST` | Authorization code received from the `confirm` step. |
| `code_verifier` | `$_REQUEST` | PKCE verifier used to validate the challenge. |
| `client_id` | `$_REQUEST` | Must match the original client ID. |
| `redirect_uri` | `$_REQUEST` | Must match the original redirect URI. |
| `resource` | `$_REQUEST` | Must match the active resource URL. |

#### Response

```json
{
  "access_token": "<decrypted_token>",
  "token_type": "Bearer",
  "scope": "mcp",
  "expires_in": 3600
}
```

#### Inner Mechanisms

1. Retrieves cached authorization data using the code.
2. Deletes the code from cache to prevent reuse.
3. Validates client ID, redirect URI, and resource.
4. Performs PKCE verification (`S256` or `plain`).
5. Decrypts the stored token and retrieves the associated user.
6. Returns the access token with expiration info.

---

## Request Context Rewriting

After handling OAuth logic, the script resolves the actual target script to execute based on the MCP JSON-RPC payload.

### Target Resolution

```php
$target = mcp::$json["params"]["arguments"]["target_url"] ??
          dirname($_SERVER["SCRIPT_NAME"]) . "/desktop.php";
```

If the target cannot be resolved locally, it falls back to an HTTP proxy script (`http.php`).

### Context Rewriting

Rewrites standard PHP superglobals to simulate a direct request to the target script:

| Variable | Description |
|---------|-------------|
| `$_SERVER["SCRIPT_NAME"]` | Set to the resolved script path. |
| `$_SERVER["SCRIPT_FILENAME"]` | Set to the absolute file path. |
| `$_SERVER["QUERY_STRING"]` | Preserved from the target URL. |
| `$_SERVER["REQUEST_URI"]` | Reconstructed from path and query. |
| `$_GET` | Populated from parsed query string. |

### Execution

Finally, changes the working directory and includes the target script:

```php
chdir(dirname(CMS_MCP_TARGET));
require(CMS_MCP_TARGET);
```

This allows the target script to run as if it were directly accessed, maintaining compatibility with existing PWNC modules.

---

## Security Considerations

- All inputs are validated and sanitized.
- OAuth tokens are encrypted at rest using `encrypt()` and decrypted with `decrypt()`.
- Authorization codes are short-lived (60 seconds) and single-use.
- Resource indicators must match the active URL exactly.
- PKCE is enforced for all authorization flows.
- CSRF protection is implicit through the use of `state` and `code_verifier`.


<!-- HASH:33422953970402829427724d2d587df6 -->
