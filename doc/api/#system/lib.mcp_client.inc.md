# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.mcp_client.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.mcp_client.inc)

- **Version:** `26.9.21.15`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp_client

The `mcp_client` class implements a client for the Model Context Protocol (MCP), enabling communication with MCP-compatible servers. It supports tool invocation, resource access, prompt retrieval, and server discovery over HTTP with JSON-RPC 2.0 semantics. The class handles authentication via bearer tokens, manages request timeouts, and processes both standard JSON responses and Server-Sent Events (SSE) streams.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | string\|NULL | NULL | The MCP server endpoint URL. |
| `$token` | string\|NULL | NULL | Bearer token used for authentication. |
| `$info` | array | `[]` | Client identification metadata (name, version). |
| `$capability` | array | `[]` | Client capabilities advertised to the server. |
| `$error` | string\|NULL | NULL | Stores the last error message encountered. |
| `$timeout_total` | int | 30 | Total request timeout in seconds. |
| `$timeout_chunk` | int | 30 | Per-chunk timeout in seconds for streaming responses. |
| `$id` | int | 0 | Static request counter for unique JSON-RPC IDs. |
| `VERSION` | string | `"2026-07-28"` | The MCP protocol version this client implements. |

### Constructor

#### `__construct($url, $token, $option = NULL)`

Initializes the MCP client with connection details and optional configuration.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | string | The MCP server endpoint URL. |
| `$token` | string | Bearer token for authentication. |
| `$option` | array\|NULL | Optional configuration array. |

**Option Keys:**

| Key | Type | Description |
|-----|------|-------------|
| `timeout_total` | int | Overrides default total timeout. |
| `timeout_chunk` | int | Overrides default chunk timeout. |
| `capability` | array | Sets client capabilities. |

**Example:**
```php
$client = new mcp_client(
    "https://mcp.example.com",
    "secret-token",
    ["timeout_total" => 60, "capability" => ["tools" => true]]
);
```

### Methods

#### `send_request($method, $param = NULL, $option = NULL)` *(private)*

Sends a JSON-RPC request to the MCP server and processes the response.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$method` | string | The MCP method to call (e.g., `"tools/list"`). |
| `$param` | array\|NULL | Request parameters. |
| `$option` | array\|NULL | Additional options including `meta`, `mirror`, and `limit`. |

**Returns:**
- `array` on success containing the result data.
- `FALSE` on failure, with `$this->error` set.

**Mechanism:**
1. Loads the HTTP library via `cms_load("http")`.
2. Merges protocol metadata into parameters.
3. Encodes the request as JSON-RPC 2.0.
4. Sends via `http_request()` with appropriate headers.
5. Parses response (JSON or SSE) using `get_response()`.
6. Validates protocol-level errors and result types.

**Example:**
```php
// Called internally by public methods like tools_list()
$result = $client->send_request("tools/list");
if ($result === FALSE) {
    echo "Error: " . $client->error;
}
```

#### `get_response($body, $type)` *(private)*

Parses the HTTP response body, handling both JSON and Server-Sent Events (SSE) formats.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$body` | string | Raw response body. |
| `$type` | string | Content-Type header value. |

**Returns:**
- `array` containing decoded JSON response.
- `NULL` if parsing fails or response is invalid.

**Mechanism:**
- If content type is not `text/event-stream`, decodes body as JSON.
- For SSE responses, splits body into lines, extracts `data:` fields, and decodes each event as JSON.
- Returns the first valid JSON object with an `id` field.

#### `build_header($method, $name = NULL, $param = NULL, $option = NULL)` *(private)*

Constructs HTTP headers for the MCP request.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$method` | string | MCP method name. |
| `$name` | string\|NULL | Resource/tool name for `MCP-Name` header. |
| `$param` | array\|NULL | Request parameters for mirroring. |
| `$option` | array\|NULL | Options including `mirror` paths. |

**Returns:**
- `array` of HTTP headers.

**Mechanism:**
- Sets standard headers (`Content-Type`, `Accept`, `MCP-Protocol-Version`, `MCP-Method`).
- Adds `Authorization: Bearer <token>` if token is present.
- Adds `MCP-Name` header if name is provided.
- Mirrors specified parameters into custom `MCP-Param-*` headers based on schema mapping.

#### `map_header($schema, $path, &$map, &$seen)` *(private static)*

Recursively maps JSON schema properties to HTTP header names using `x-mcp-header` annotations.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$schema` | array | JSON schema to process. |
| `$path` | string | Current path in the schema. |
| `$map` | array | Reference to output mapping array. |
| `$seen` | array | Reference to track seen header names. |

**Returns:**
- `TRUE` if mapping succeeds.
- `FALSE` if schema is invalid or header names conflict.

**Mechanism:**
- Iterates through schema properties.
- For properties with `x-mcp-header`, validates the header name format and uniqueness.
- Ensures only primitive types (boolean, integer, string) are mapped.
- Recursively processes nested objects.

#### `server_discover()`

Discovers server capabilities and information.

**Returns:**
- `array` containing server info and capabilities.
- `FALSE` on failure.

**Example:**
```php
$info = $client->server_discover();
if ($info !== FALSE) {
    echo "Server: " . $info["name"];
}
```

#### `tools_list($option = NULL)`

Lists available tools from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` of tool definitions with `schema` and `header` mappings.
- `FALSE` on failure.

**Mechanism:**
- Calls `send_request("tools/list")`.
- Processes each tool's `inputSchema` to build header mappings via `map_header()`.
- Skips tools with invalid annotations.

**Example:**
```php
$tools = $client->tools_list();
foreach ($tools as $tool) {
    echo "Tool: " . $tool["name"] . "\n";
}
```

#### `tools_call($name, $argument = NULL, $option = NULL)`

Invokes a specific tool by name with given arguments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | string | Tool name to invoke. |
| `$argument` | array\|NULL | Tool arguments. |
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` containing tool execution result.
- `FALSE` on failure.

**Example:**
```php
$result = $client->tools_call("search", ["query" => "PHP"]);
if ($result !== FALSE) {
    echo mcp_client::to_text($result);
}
```

#### `resources_list($option = NULL)`

Lists available resources from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` of resource definitions.
- `FALSE` on failure.

**Example:**
```php
$resources = $client->resources_list();
foreach ($resources["resources"] as $resource) {
    echo "Resource: " . $resource["uri"] . "\n";
}
```

#### `resources_read($uri, $option = NULL)`

Reads a specific resource by URI.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$uri` | string | Resource URI to read. |
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` containing resource content.
- `FALSE` on failure.

**Example:**
```php
$content = $client->resources_read("file:///example.txt");
if ($content !== FALSE) {
    echo mcp_client::to_text($content);
}
```

#### `prompts_list($option = NULL)`

Lists available prompts from the MCP server.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` of prompt definitions.
- `FALSE` on failure.

**Example:**
```php
$prompts = $client->prompts_list();
foreach ($prompts["prompts"] as $prompt) {
    echo "Prompt: " . $prompt["name"] . "\n";
}
```

#### `prompts_get($name, $argument = NULL, $option = NULL)`

Retrieves a specific prompt by name with arguments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | string | Prompt name to retrieve. |
| `$argument` | array\|NULL | Prompt arguments. |
| `$option` | array\|NULL | Additional request options. |

**Returns:**
- `array` containing prompt content.
- `FALSE` on failure.

**Example:**
```php
$prompt = $client->prompts_get("greeting", ["name" => "World"]);
if ($prompt !== FALSE) {
    echo mcp_client::to_text($prompt);
}
```

#### `to_text($result)` *(static)*

Converts MCP result content blocks into plain text.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$result` | array | Result array from tool/prompt/resource calls. |

**Returns:**
- `string` containing concatenated text content.

**Mechanism:**
- Iterates through `content` blocks.
- Handles `text`, `resource`, and `resource_link` block types.
- For resources, includes URI and MIME type if binary.

**Example:**
```php
$result = $client->tools_call("weather", ["city" => "Rome"]);
echo mcp_client::to_text($result);
```

#### `encode_value($value)` *(static)*

Encodes a value for safe inclusion in HTTP headers.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | mixed | Value to encode. |

**Returns:**
- `string` encoded value.

**Mechanism:**
- Converts booleans to `"true"`/`"false"`.
- Casts other values to string.
- Applies base64 encoding (`=?base64?...?==`) if value contains whitespace, non-ASCII characters, or starts with `=`.

**Example:**
```php
$encoded = mcp_client::encode_value("Hello World");
// Returns: "=?base64?SGVsbG8gV29ybGQ=?="
```


<!-- HASH:35473a4c0ab2f898a725ca730ba449d5 -->
