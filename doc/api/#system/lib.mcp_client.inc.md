# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.mcp_client.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.mcp_client.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp_client

The `mcp_client` class implements a client for the Model Context Protocol (MCP), enabling communication with MCP-compatible servers. It supports tool invocation, resource access, prompt retrieval, and server discovery over HTTP with JSON-RPC 2.0 semantics. The class handles authentication via bearer tokens, manages request timeouts, and processes both standard JSON responses and Server-Sent Events (SSE) streams.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string | NULL | MCP server endpoint URL |
| `$token` | string | NULL | Bearer token for authentication |
| `$info` | array | `[]` | Client identification metadata |
| `$capability` | array | `[]` | Client capabilities declaration |
| `$error` | string | NULL | Last error message |
| `$timeout_total` | int | 30 | Total request timeout in seconds |
| `$timeout_chunk` | int | 30 | Per-chunk timeout in seconds |
| `$id` | int | 0 | Static request counter |
| `VERSION` | string | `"2026-07-28"` | MCP protocol version |

### __construct

Initializes the MCP client with connection details and optional configuration.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | string | MCP server endpoint URL |
| `$token` | string | Bearer token for authentication |
| `$option` | array | Optional configuration: `timeout_total`, `timeout_chunk`, `capability` |

**Example:**
```php
$client = new mcp_client("https://mcp.example.com", "secret-token", [
    "timeout_total" => 60,
    "capability" => ["tools" => TRUE]
]);
```

### send_request

Sends a JSON-RPC request to the MCP server and processes the response.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$method` | string | MCP method name (e.g., `tools/call`) |
| `$param` | array | Request parameters |
| `$option` | array | Additional options: `meta`, `mirror`, `limit` |

**Returns:** array on success, FALSE on failure (sets `$this->error`)

**Inner Mechanisms:**
1. Loads HTTP library via `cms_load("http")`
2. Merges protocol metadata into parameters
3. Encodes request as JSON-RPC 2.0
4. Sends HTTP POST with custom headers
5. Parses response (JSON or SSE)
6. Validates protocol-level errors and result types

### get_response

Parses HTTP response body, handling both JSON and Server-Sent Events formats.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$body` | string | Raw HTTP response body |
| `$type` | string | Content-Type header value |

**Returns:** array (parsed JSON) or NULL on failure

**Inner Mechanisms:**
- For non-SSE responses: directly decodes JSON
- For SSE responses: parses event lines, collects `data:` fields, decodes JSON when event boundary reached
- Handles unterminated final events

### build_header

Constructs HTTP headers for MCP requests.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$method` | string | MCP method name |
| `$name` | string | Resource/tool name (optional) |
| `$param` | array | Request parameters |
| `$option` | array | Header mirroring options |

**Returns:** array of HTTP headers

**Inner Mechanisms:**
- Sets standard headers: Content-Type, Accept, MCP-Protocol-Version, MCP-Method
- Adds Authorization header if token present
- Adds MCP-Name header for named resources/tools
- Mirrors specified parameters into custom MCP-Param-* headers

### map_header

Recursively maps JSON schema properties to HTTP header names based on `x-mcp-header` annotations.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$schema` | array | JSON schema definition |
| `$path` | string | Current property path |
| `$map` | array | Result mapping (by reference) |
| `$seen` | array | Track seen header names (by reference) |

**Returns:** TRUE on success, FALSE on validation failure

**Inner Mechanisms:**
- Validates header names match HTTP token format
- Ensures header name uniqueness
- Restricts mapped types to boolean, integer, or string
- Recursively processes nested objects

### server_discover

Retrieves server capabilities and information.

**Returns:** array (server info) or FALSE on failure

**Example:**
```php
$info = $client->server_discover();
if ($info !== FALSE) {
    echo "Server: " . $info["name"];
}
```

### tools_list

Lists available tools from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array | Request options |

**Returns:** array of tool definitions or FALSE on failure

**Inner Mechanisms:**
- Calls `tools/list` method
- Processes each tool's input schema
- Maps schema properties to HTTP headers via `map_header`
- Invalidates tools with malformed header annotations

**Example:**
```php
$tools = $client->tools_list();
foreach ($tools as $tool) {
    echo "Tool: " . $tool["name"] . "\n";
}
```

### tools_call

Invokes a specific tool with given arguments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | string | Tool name |
| `$argument` | array | Tool arguments |
| `$option` | array | Request options |

**Returns:** array (tool result) or FALSE on failure

**Example:**
```php
$result = $client->tools_call("search", ["query" => "PHP"]);
if ($result !== FALSE) {
    echo mcp_client::to_text($result);
}
```

### resources_list

Lists available resources from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array | Request options |

**Returns:** array (resource list) or FALSE on failure

### resources_read

Reads a specific resource by URI.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$uri` | string | Resource URI |
| `$option` | array | Request options |

**Returns:** array (resource content) or FALSE on failure

**Example:**
```php
$resource = $client->resources_read("file:///config/settings.json");
if ($resource !== FALSE) {
    echo mcp_client::to_text($resource);
}
```

### prompts_list

Lists available prompts from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array | Request options |

**Returns:** array (prompt list) or FALSE on failure

### prompts_get

Retrieves a specific prompt by name with arguments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | string | Prompt name |
| `$argument` | array | Prompt arguments |
| `$option` | array | Request options |

**Returns:** array (prompt result) or FALSE on failure

**Example:**
```php
$prompt = $client->prompts_get("greeting", ["name" => "World"]);
if ($prompt !== FALSE) {
    echo mcp_client::to_text($prompt);
}
```

### to_text

Converts MCP result content blocks to plain text.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$result` | array | MCP result with content blocks |

**Returns:** string (concatenated text content)

**Inner Mechanisms:**
- Processes `text`, `resource`, and `resource_link` block types
- For resources: uses embedded text or generates URI/mime-type description
- Joins all text segments with newlines

**Example:**
```php
$result = $client->tools_call("search", ["query" => "test"]);
$text = mcp_client::to_text($result);
echo $text;
```

### encode_value

Encodes a value for safe inclusion in HTTP headers.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | mixed | Value to encode |

**Returns:** string (encoded value)

**Inner Mechanisms:**
- Converts booleans to "true"/"false"
- Casts other values to string
- Applies base64 encoding (RFC 2047 style) for values containing whitespace, non-ASCII characters, or starting with `=?`

**Example:**
```php
$encoded = mcp_client::encode_value("Hello World");
// Returns: "=?base64?SGVsbG8gV29ybGQ=?="
```


<!-- HASH:8cbec5da4636251e938bd9f86d5189c5 -->
