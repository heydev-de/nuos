# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mcp.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mcp.inc)

- **Version:** `26.9.10.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp

The `mcp` class and its associated functions implement a Model Context Protocol (MCP) server endpoint for the PWNC Web Platform. It enables external AI clients (such as Claude Desktop) to interact with PWNC through a standardized JSON-RPC 2.0 interface, exposing tools, resources, memory management, and HTTP request/response handling.

### Functions

#### mcp_http_header

Retrieves an HTTP request header value by name in a case-insensitive manner.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The header name to look up |

**Returns:** `string|null` — The header value if found, otherwise `NULL`.

**Mechanisms:** Caches all headers on first call using `getallheaders()` if available, or falls back to parsing `$_SERVER` for `HTTP_*` keys. Normalizes keys to lowercase with hyphens.

**Usage:**
```php
$contentType = mcp_http_header("Content-Type");
```

#### mcp_bundle

Generates a `.mcpb` bundle file containing the MCP server manifest and client-side assets.

**Returns:** `string|false` — Path to the generated bundle file, or `FALSE` on failure.

**Mechanisms:** Loads the filemanager library, creates a target directory, builds a manifest array with semantic versioning, collects source files recursively, creates a ZIP archive, adds the manifest and files, then renames the temp file to `pwnc.mcpb`.

**Usage:**
```php
$bundlePath = mcp_bundle();
if ($bundlePath !== FALSE) {
    echo "Bundle created at: $bundlePath";
}
```

#### mcp_config

Generates a JSON configuration snippet for registering the PWNC MCP server in a client.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | `"mcpServers"` | Top-level configuration key |

**Returns:** `string` — JSON-encoded configuration.

**Usage:**
```php
$config = mcp_config();
file_put_contents("claude_mcp_config.json", $config);
```

#### mcp_diff

Computes a unified diff between two text strings using a bidirectional Myers diff algorithm with time budgeting.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | string | Original text |
| `$target` | string | Modified text |
| `$source_name` | string | Label for source (e.g., filename) |
| `$target_name` | string | Label for target |
| `$context` | int | Number of context lines per block |
| `$limit` | int|false | Maximum allowed edits; `FALSE` for no limit |
| `$budget` | float | Time budget in seconds (default `0.02`) |

**Returns:** `string|false` — Unified diff string, empty string if identical, or `FALSE` if limit exceeded.

**Usage:**
```php
$diff = mcp_diff("line1\nline2", "line1\nline3", "old.txt", "new.txt", 3, FALSE);
echo $diff;
```

### Class: mcp

Static class managing MCP server state and request processing.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `$json` | array | `NULL` | Current JSON-RPC request data |
| `$flag` | bool | `FALSE` | Reentrancy guard |
| `$abort` | bool | `FALSE` | Cancels shutdown processing |
| `$post_temp` | array | `[]` | Temporary upload file paths |
| `$message` | string | `NULL` | System message |
| `$max_length` | int | `32000` | Maximum response size |
| `$filter_context` | int | `3` | Context lines per filter match |
| `$diff_context` | int | `3` | Context lines per diff block |
| `$diff_limit` | float | `1.0` | Edits per target line |
| `$diff_margin` | float | `0.9` | Share of full response for diff |
| `$diff_budget` | float | `0.02` | Seconds of diff search |
| `$diff_expire` | int | `300` | Seconds of diff recall |

#### send_output

Sends the final JSON-RPC response with optional diff against the previous response.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$uri` | string | `NULL` | Resource URI |
| `$diff` | bool | `TRUE` | Whether to compute diff |
| `$line_offset` | int | `0` | Line numbering offset |

**Returns:** `void`

**Usage:** Called internally during shutdown to send HTTP responses.

#### send_result

Sends a tool result response and exits.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | string | Result content |
| `$error` | bool | Whether to mark as error |

**Returns:** `void`

**Usage:**
```php
mcp::send_result("Operation completed successfully.");
```

#### server_discover

Sends server discovery information including capabilities and instructions.

**Returns:** `void`

**Usage:** Called when the client requests server metadata.

#### tools_list

Sends the list of available MCP tools with their schemas.

**Returns:** `void`

**Usage:** Called when the client requests the tool list.

#### tools_get

Registers a shutdown function to send output after GET request processing.

**Returns:** `bool` — `TRUE` if registered, `FALSE` if already flagged.

**Usage:** Called internally when handling GET tool requests.

#### tools_post

Processes POST form data including file uploads and registers shutdown function.

**Returns:** `bool` — `TRUE` if registered, `FALSE` if already flagged.

**Usage:** Called internally when handling POST tool requests.

#### tools_get_response

Retrieves and optionally filters/line-ranges the last cached response.

**Returns:** `void`

**Usage:** Called when the client invokes the `get_response` tool.

#### tools_resources_list

Sends a markdown-formatted list of available resources.

**Returns:** `void`

**Usage:** Called when the client invokes the `resources_list` tool.

#### tools_readme_first

Sends a welcome message with user identity and instructions.

**Returns:** `void`

**Usage:** Called when the client invokes the `readme_first` tool.

#### tools_resources_read

Reads and sends a specific resource by URI.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$uri` | string | `NULL` | Resource URI |

**Returns:** `void`

**Usage:** Called when the client invokes the `resources_read` tool.

#### memory_address

Parses a memory identifier into region and ID components.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Memory address string |

**Returns:** `array` — `[region, id]` pair.

**Usage:**
```php
list($region, $id) = mcp::memory_address("user.alice/42");
```

#### tools_memory_remember

Creates or updates a memory record.

**Returns:** `void`

**Usage:** Called when the client invokes the `memory_remember` tool.

#### tools_memory_recall

Recalls memories based on query, region, or specific ID.

**Returns:** `void`

**Usage:** Called when the client invokes the `memory_recall` tool.

#### tools_memory_rate

Rates or deletes a memory record.

**Returns:** `void`

**Usage:** Called when the client invokes the `memory_rate` tool.

#### tools_whois

Looks up user or group information.

**Returns:** `void`

**Usage:** Called when the client invokes the `whois` tool.

#### resources_list

Sends the list of available resources in JSON-RPC format.

**Returns:** `void`

**Usage:** Called when the client requests the resource list via JSON-RPC.

#### resources_read

Reads and sends a specific resource in JSON-RPC format.

**Returns:** `void`

**Usage:** Called when the client requests a resource via JSON-RPC.

#### get_resource_list

Recursively scans the MCP directory for available resources.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$base` | string | `NULL` | Base directory path |

**Returns:** `array` — List of resource entries.

**Usage:** Called internally to build resource lists.

#### get_resource

Retrieves a resource by URI, supporting both `memory:` and `pwnc:` schemes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$uri` | string | Resource URI |

**Returns:** `array` — Resource data with `type` and `data` keys, or `error` key on failure.

**Usage:**
```php
$data = mcp::get_resource("pwnc:README.md");
```

#### input_required

Sends an input-required response, caching the current request for later restoration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$input_requests` | array | Input request definitions |

**Returns:** `void`

**Usage:** Called when user input is needed during tool execution.

#### message

Requests user acknowledgment via elicitation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$text` | string | Message to display |

**Returns:** `bool` — `TRUE` if sent, `FALSE` if client doesn't support elicitation.

**Usage:**
```php
mcp::message("Please acknowledge this important notice.");
```

#### confirm

Requests user confirmation via elicitation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Confirmation message |
| `$key` | string | `"confirm"` | Response key |

**Returns:** `bool` — `TRUE` if sent, `FALSE` if client doesn't support elicitation.

**Usage:**
```php
mcp::confirm("Do you want to proceed?");
```

#### restore_request

Restores a previously cached request after input was provided.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$request_state` | string | Cache identifier |

**Returns:** `bool` — `TRUE` if restored, `FALSE` if invalid.

**Usage:** Called internally when resuming after input.

#### error

Sends a JSON-RPC error response and exits.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$code` | int | `1` | Error code |
| `$message` | string | `NULL` | Error message |
| `$data` | mixed | `NULL` | Additional error data |
| `$http_code` | int | `400` | HTTP status code |

**Returns:** `void`

**Usage:**
```php
mcp::error(-32603, "Internal error occurred.");
```

#### method_not_found

Sends a "method not found" error.

**Returns:** `void`

**Usage:** Called when an unknown JSON-RPC method is requested.

#### unknown_tool

Sends an "unknown tool" error.

**Returns:** `void`

**Usage:** Called when an unknown tool name is requested.

#### send_error

Sends an error result via `send_result`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$message` | string | Error message |

**Returns:** `void`

**Usage:**
```php
mcp::send_error("Something went wrong.");
```

#### permission_list

Formats a permission array into a readable string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | array | Permission entries |

**Returns:** `string` — Formatted permission list.

**Usage:**
```php
echo mcp::permission_list(["user.alice" => "Alice"]);
```


<!-- HASH:359012bf3d3bb1a3dfc3b451b8ac7eba -->
