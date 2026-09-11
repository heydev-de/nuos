# PWNC API Documentation

[← Index](../README.md) | [`module/http.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/http.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## module/http.php

This file implements an HTTP proxy module for the PWNC MCP (Master Control Panel). It acts as a server-side HTTP client that fetches content from a specified URL and relays the response back to the caller, including response headers. This is useful for bypassing CORS restrictions or accessing external resources from the server side.

### Module Flow

The module executes within an immediately-invoked function expression (IIFE) and follows these steps:

1. **Access Control**: Exits if not running in MCP context (`CMS_MCP_ACTIVE`)
2. **Library Loading**: Loads the `http` library via `cms_load("http")`
3. **URL Resolution**: Retrieves and validates the `target_url` parameter, converting it to an absolute path
4. **Data Preparation**: Checks if this is a POST request and extracts form data if so
5. **HTTP Request**: Sends the request using `http_fopen()` and captures response headers
6. **Response Handling**: Fetches the response body using `http_fetch_data()`
7. **Header Forwarding**: Removes existing headers and forwards all response headers
8. **Body Output**: Echoes the response body to the client

### Parameters

| Name | Source | Description |
|------|--------|-------------|
| `target_url` | `mcp::$json["params"]["arguments"]["target_url"]` | The URL to fetch content from |
| `name` | `mcp::$json["params"]["name"]` | Request method identifier; "post" triggers POST behavior |
| `form_data` | `mcp::$json["params"]["arguments"]["form_data"]` | Associative array of POST data when method is "post" |

### Key Functions Used

#### http_fopen($url, $data, &$header)

Sends an HTTP request to the specified URL.

**Parameters:**
- `$url` (string): The target URL to request
- `$data` (array\|NULL): POST data array or NULL for GET requests
- `&$header` (array): Reference variable populated with response headers

**Returns:** File handle on success, FALSE on failure

**Usage Example:**
```php
// Fetch content from an external API
$headers = [];
$handle = http_fopen("https://api.example.com/data", NULL, $headers);
if ($handle !== FALSE) {
    $content = http_fetch_data($handle);
    // Process $content and $headers
}
```

#### http_fetch_data($handle)

Reads the complete response body from an HTTP file handle.

**Parameters:**
- `$handle` (resource): File handle returned by `http_fopen()`

**Returns:** Response body as string, FALSE on failure

**Usage Example:**
```php
// After obtaining a handle from http_fopen()
$body = http_fetch_data($handle);
if ($body !== FALSE) {
    echo $body; // Output the response content
}
```

### Error Handling

The module uses `mcp::send_error()` to report errors at each critical step:

1. "Internal error." - When http library fails to load
2. "Invalid `target_url`." - When URL resolution fails
3. "Connection failed." - When HTTP request fails
4. "No response." - When response body cannot be retrieved

### Header Processing

Response headers are forwarded to the client with special handling for `Set-Cookie` headers:

```php
// Multiple Set-Cookie headers are forwarded individually
foreach ($value AS $_value) {
    header("$key: $_value", FALSE);
}
```

The `FALSE` parameter in `header()` prevents replacement of previous headers with the same name, allowing multiple cookies to be set.

### Usage Scenario

A frontend application needs to fetch data from an external API that doesn't support CORS. Instead of making a direct browser request (which would be blocked), the application sends a request to this MCP module with the target URL. The module fetches the data server-side and returns it to the client, effectively acting as a CORS proxy.

**Client-side example:**
```javascript
// Request to MCP module instead of direct API call
fetch('/mcp.php', {
    method: 'POST',
    body: JSON.stringify({
        name: 'get',
        arguments: {
            target_url: 'https://external-api.com/data'
        }
    })
})
.then(response => response.text())
.then(data => console.log(data));
```


<!-- HASH:fa54cdffdfcd53017b909ec015c493f1 -->
