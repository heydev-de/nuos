# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mcp.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mcp.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mcp

The `mcp` class and its associated functions implement a Model Context Protocol (MCP) server within the PWNC Web Platform. It provides a JSON-RPC 2.0 interface that allows external MCP clients (such as AI agents) to interact with the platform. The server supports tool discovery, resource listing and reading, memory management, user/group lookups, and HTTP request proxying.

### Functions

#### mcp_http_header

Retrieves an HTTP request header value by name in a case-insensitive manner.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the HTTP header to retrieve. |

**Returns:** `string|null` — The header value if found, otherwise `NULL`.

**Mechanisms:**
- Uses a static cache (`$array`) to avoid re-parsing headers on repeated calls.
- On first call, populates the cache using `getallheaders()` if available, otherwise falls back to parsing `$_SERVER` for `HTTP_*` keys.
- Normalizes header names to lowercase with hyphens replacing underscores.

**Usage:**
```php
$contentType = mcp_http_header("Content-Type");
// Returns the Content-Type header value or NULL if not present.
```

---

#### mcp_bundle

Creates a `.mcpb` bundle file containing the MCP client-side resources and a manifest for installation into compatible MCP clients.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** `string|false` — The path to the generated `.mcpb` file on success, or `FALSE` on failure.

**Mechanisms:**
- Loads the `filemanager` library to recursively collect files from `CMS_PATH . "mcp/#mcpb/"`.
- Generates a semantic version string from `CMS_VERSION`.
- Builds a manifest array with metadata, server configuration, user config schema, and compatibility info.
- Creates a ZIP archive containing the manifest and collected files.
- Saves the archive to `CMS_DATA_PATH . "#mcp/mcpb/pwnc.mcpb"`.

**Usage:**
```php
$bundlePath = mcp_bundle();
if ($bundlePath !== FALSE) {
    echo "Bundle created at: $bundlePath";
}
```

---

#### mcp_config

Generates a JSON configuration snippet for registering the PWNC MCP server in an MCP client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The configuration key under which the server is registered. Default: `"mcpServers"`. |

**Returns:** `string` — A JSON-encoded configuration string.

**Mechanisms:**
- Constructs a unique identifier based on `CMS_DOMAIN`.
- Builds a configuration object with the server type (`http`), URL (`CMS_MODULES_URL . "mcp.php"`), and an authorization header placeholder.

**Usage:**
```php
$config = mcp_config();
echo $config;
// Outputs JSON like: {"mcpServers":{"pwnc-example-com":{"type":"http","url":"...","headers":{"Authorization":"Bearer {API key}"}}}}
```

---

#### mcp_diff

Computes a unified diff between two text strings using a modified Myers diff algorithm with time-budget constraints.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | string | The original text. |
| `$target` | string | The modified text. |
| `$source_name` | string | Label for the source (e.g., filename). |
| `$target_name` | string | Label for the target. |
| `$context` | int | Number of context lines per diff block. |
| `$limit` | int|false | Maximum number of edits allowed; `FALSE` for no limit. |
| `$budget` | float | Time budget in seconds for the search phase. Default: `0.02`. |

**Returns:** `string|false` — A unified diff string, an empty string if no changes, or `FALSE` if the limit is exceeded.

**Mechanisms:**
- Splits input into lines and trims common prefix/suffix.
- Maps lines to unique tokens for comparison.
- Uses a bidirectional Myers diff algorithm with forward and backward searches.
- Implements a time budget to prevent excessive computation.
- Groups changes into blocks with context lines and renders a unified diff format.

**Usage:**
```php
$diff = mcp_diff(
    "line1\nline2\nline3",
    "line1\nmodified\nline3",
    "old.txt",
    "new.txt",
    3,
    100
);
echo $diff;
// Outputs a unified diff showing the change from "line2" to "modified".
```

---

## Class: mcp

### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `$json` | array | `NULL` | The current JSON-RPC request data. |
| `$flag` | bool | `FALSE` | Reentrancy guard for tool execution. |
| `$abort` | bool | `FALSE` | Cancels shutdown processing when set. |
| `$post_temp` | array | `[]` | Temporary file paths from POST uploads. |
| `$message` | string | `NULL` | System message to prepend to responses. |
| `$max_length` | int | `32000` | Maximum response size in bytes. |
| `$filter_context` | int | `3` | Context lines per filter match. |
| `$diff_context` | int | `3` | Context lines per diff block. |
| `$diff_limit` | float | `1.0` | Edits per target line ratio. |
| `$diff_margin` | float | `0.9` | Share of full response to use diff. |
| `$diff_budget` | float | `0.02` | Seconds of search time for diff. |
| `$diff_expire` | int | `300` | Seconds before cached response expires. |

---

### Methods

#### send_output

Sends the final JSON-RPC response, optionally computing a diff against the previous response.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$uri` | string|null | The target URI for the response. |
| `$diff` | bool | Whether to compute a diff. Default: `TRUE`. |
| `$line_offset` | int|bool | Line offset for numbering; `FALSE` to disable. |

**Returns:** void

**Mechanisms:**
- Scans outgoing headers for `Content-Type` and `Location`.
- Retrieves the previous response from cache for diff computation.
- Encodes binary content as base64.
- Applies line numbering and truncation for large responses.
- Sends a JSON-RPC 2.0 response with headers, system message, and content.

**Usage:**
```php
// Called internally during shutdown after a GET request.
mcp::send_output("pwnc/module/desktop.php", TRUE, 0);
```

---

#### send_result

Sends a tool result as a JSON-RPC response and exits.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | string | The result content. |
| `$error` | bool | Whether to mark the result as an error. Default: `FALSE`. |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further shutdown processing.
- Cleans all output buffers.
- Sends a JSON-RPC 2.0 response with the content and optional error flag.

**Usage:**
```php
mcp::send_result("Operation completed successfully.");
```

---

#### server_discover

Sends server capabilities and metadata for MCP client discovery.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Sends a JSON-RPC 2.0 response with server info, supported versions, instructions, and capabilities.

**Usage:**
```php
// Called when the client requests server capabilities.
mcp::server_discover();
```

---

#### tools_list

Sends the list of available MCP tools to the client.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Deletes the previous cached response.
- Sends a JSON-RPC 2.0 response listing all available tools with their schemas.

**Usage:**
```php
// Called when the client requests the tool list.
mcp::tools_list();
```

---

#### tools_get

Registers a shutdown function to send the response after a GET request completes.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** `bool` — `TRUE` if registered, `FALSE` if already flagged.

**Mechanisms:**
- Uses `$flag` to prevent re-execution.
- Registers a shutdown function that calls `send_output()` if not aborted.

**Usage:**
```php
// Called internally to handle GET requests.
mcp::tools_get();
```

---

#### tools_post

Handles POST requests by parsing form data and file uploads, then registers a shutdown function.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** `bool` — `TRUE` if registered, `FALSE` if already flagged.

**Mechanisms:**
- Sets `$flag` to prevent re-execution.
- Registers a shutdown function that cleans temp files and calls `send_output()`.
- Parses `form_data` from the JSON request into `$_POST` and `$_FILES`.
- Handles file uploads by decoding base64 data and creating temporary files.

**Usage:**
```php
// Called internally to handle POST requests.
mcp::tools_post();
```

---

#### tools_get_response

Retrieves the most recent response, optionally with line range or filter.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Retrieves the cached response.
- Extracts a line range if specified.
- Filters lines if a filter string is provided, with context lines.
- Sends the response via `send_output()`.

**Usage:**
```php
// Called when the client requests a previous response.
mcp::tools_get_response();
```

---

#### tools_resources_list

Sends a markdown-formatted list of available resources.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Calls `get_resource_list()` to retrieve resources.
- Formats the list as markdown and sends it via `send_result()`.

**Usage:**
```php
// Called when the client requests the resource list.
mcp::tools_resources_list();
```

---

#### tools_readme_first

Sends the platform README as the initial resource, optionally setting a system message.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets a system message if the client lacks elicitation capabilities.
- Calls `tools_resources_read()` with `"pwnc:README.md"`.

**Usage:**
```php
// Called when the client requests initial instructions.
mcp::tools_readme_first();
```

---

#### tools_resources_read

Reads and sends a specific resource by URI.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$uri` | string|null | The resource URI. Defaults to the request parameter. |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Calls `get_resource()` to retrieve the resource.
- Sends the resource content via `send_output()`.

**Usage:**
```php
// Called when the client requests a specific resource.
mcp::tools_resources_read("pwnc:README.md");
```

---

#### memory_address

Parses a memory identifier into region and ID components.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string|null | The memory identifier (e.g., `"scope.token/123"`). |

**Returns:** `array` — An array containing `[region, id]`.

**Mechanisms:**
- Uses a regex to parse the identifier.
- Handles numeric-only IDs by defaulting to the user's personal region.

**Usage:**
```php
list($region, $id) = mcp::memory_address("scope.my_token/42");
// $region = "scope.my_token", $id = 42
```

---

#### tools_memory_remember

Creates or updates a memory record.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Loads the `memory` library.
- Parses the memory address from the request.
- Calls `memory->add()` or `memory->set()` based on whether an ID is provided.
- Sends a success or error response.

**Usage:**
```php
// Called when the client creates or updates a memory.
mcp::tools_memory_remember();
```

---

#### tools_memory_recall

Recalls memories based on region, ID, or search query.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Loads the `memory` library.
- Parses the memory address and query from the request.
- Depending on parameters, lists regions, searches memories, shows a specific memory, or displays a region overview.
- Formats the output as markdown and sends it via `send_result()`.

**Usage:**
```php
// Called when the client recalls memories.
mcp::tools_memory_recall();
```

---

#### tools_memory_rate

Rates or deletes a memory record.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Loads the `memory` library.
- Parses the memory address and rating from the request.
- Calls `memory->rate()` with the specified rating.
- Sends a success or error response.

**Usage:**
```php
// Called when the client rates a memory.
mcp::tools_memory_rate();
```

---

#### tools_whois

Looks up information about a user or group.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Parses the key from the request (defaults to the current user).
- Validates the key format.
- Retrieves the record from the permission data store.
- Formats the output as markdown with identity, groups/members, and sends it via `send_result()`.

**Usage:**
```php
// Called when the client requests user/group info.
mcp::tools_whois();
```

---

#### tools_wake_up

Activates an agent for a specified user with a message.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Checks operator permissions.
- Validates the user key and message.
- Loads the `agent` library.
- Calls `agent_start()` to activate the agent.
- Sends a success or error response.

**Usage:**
```php
// Called when the client wants to wake up an agent.
mcp::tools_wake_up();
```

---

#### resources_list

Sends the list of available resources as a JSON-RPC response.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Calls `get_resource_list()` to retrieve resources.
- Sends a JSON-RPC 2.0 response with the resource list.

**Usage:**
```php
// Called when the client requests the resource list via JSON-RPC.
mcp::resources_list();
```

---

#### resources_read

Reads and sends a specific resource by URI as a JSON-RPC response.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Retrieves the URI from the request.
- Calls `get_resource()` to fetch the resource.
- Sends a JSON-RPC 2.0 response with the resource content (text or base64 blob).

**Usage:**
```php
// Called when the client requests a specific resource via JSON-RPC.
mcp::resources_read();
```

---

#### get_resource_list

Recursively scans a directory for resource files and builds a list with metadata.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$base` | string|null | The base directory to scan. Defaults to `CMS_PATH . "mcp/resources"`. |

**Returns:** `array` — A list of resource entries with `uri`, `name`, `mimeType`, and optional `title`/`description`.

**Mechanisms:**
- Uses `RecursiveDirectoryIterator` and `RecursiveIteratorIterator` to traverse the directory.
- For Markdown files, attempts to parse YAML front matter for title and description.
- Returns an array of resource entries.

**Usage:**
```php
$resources = mcp::get_resource_list();
foreach ($resources as $resource) {
    echo $resource["uri"] . "\n";
}
```

---

#### get_resource

Retrieves a resource by URI, supporting both `pwnc:` and `memory:` schemes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$uri` | string | The resource URI. |

**Returns:** `array` — An array with `type` and `data` keys, or an `error` key on failure.

**Mechanisms:**
- Validates the URI scheme.
- For `memory:` URIs, loads the memory library and retrieves the attachment.
- For `pwnc:` URIs, constructs the file path from `CMS_PATH`.
- Reads the file and returns its content with the MIME type.

**Usage:**
```php
$data = mcp::get_resource("pwnc:README.md");
if (!isset($data["error"])) {
    echo $data["data"];
}
```

---

#### input_required

Sends an `input_required` response to request user input from the MCP client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$input_requests` | array | An array of input request definitions. |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Generates a unique request state and caches the current request.
- Sends a JSON-RPC 2.0 response with `resultType: "input_required"` and the input requests.

**Usage:**
```php
mcp::input_required([
    "message" => [
        "method" => "elicitation/create",
        "params" => [
            "mode" => "form",
            "message" => "Please confirm:",
            "requestedSchema" => [
                "type" => "object",
                "required" => ["acknowledged"],
                "properties" => [
                    "acknowledged" => ["type" => "boolean"]
                ]
            ]
        ]
    ]
]);
```

---

#### message

Requests a simple acknowledgment message from the user via the MCP client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$text` | string | The message to display. |

**Returns:** `bool` — `TRUE` if the input was requested, `FALSE` if the client doesn't support elicitation.

**Mechanisms:**
- Checks if the client supports elicitation.
- Calls `input_required()` with a boolean acknowledgment schema.

**Usage:**
```php
if (mcp::message("Please acknowledge this message.")) {
    // Input was requested.
}
```

---

#### confirm

Requests a confirmation (yes/no) from the user via the MCP client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$text` | string | The confirmation message. |
| `$key` | string | The key for the input request. Default: `"confirm"`. |

**Returns:** `bool` — `TRUE` if the input was requested, `FALSE` if the client doesn't support elicitation.

**Mechanisms:**
- Checks if the client supports elicitation.
- Calls `input_required()` with a boolean confirmation schema.

**Usage:**
```php
if (mcp::confirm("Do you want to proceed?")) {
    // Confirmation was requested.
}
```

---

#### restore_request

Restores a previously cached request after user input is received.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$request_state` | string | The request state identifier. |

**Returns:** `bool` — `TRUE` if the request was restored, `FALSE` if invalid.

**Mechanisms:**
- Retrieves the cached request using `cms_cache_notouch()`.
- Deletes the cached request.
- Updates the request ID and merges input responses.
- Sets `self::$json` to the restored request.

**Usage:**
```php
if (mcp::restore_request($requestState)) {
    // Request restored, continue processing.
}
```

---

#### error

Sends a JSON-RPC error response and exits.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$code` | int | The error code. Default: `1`. |
| `$message` | string|null | The error message. |
| `$data` | mixed|null | Additional error data. |
| `$http_code` | int | The HTTP status code. Default: `400`. |

**Returns:** void

**Mechanisms:**
- Sets `$abort` to prevent further processing.
- Cleans all output buffers.
- Sends a JSON-RPC 2.0 error response with the specified code, message, and data.

**Usage:**
```php
mcp::error(-32603, "Internal error", NULL, 500);
```

---

#### method_not_found

Sends a "method not found" error response.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Calls `error()` with code `-32601` and HTTP status `404`.

**Usage:**
```php
// Called when an unknown method is requested.
mcp::method_not_found();
```

---

#### unknown_tool

Sends an "unknown tool" error response.

| Parameter | Type | Description |
|-----------|------|-------------|
| None | — | — |

**Returns:** void

**Mechanisms:**
- Calls `error()` with code `-32602` and the tool name from the request.

**Usage:**
```php
// Called when an unknown tool is requested.
mcp::unknown_tool();
```

---

#### send_error

Sends an error result as a tool response.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$message` | string | The error message. |

**Returns:** void

**Mechanisms:**
- Calls `send_result()` with `$error` set to `TRUE`.

**Usage:**
```php
mcp::send_error("Something went wrong.");
```

---

#### system_prompt

Generates a system prompt for the MCP client, optionally prepending an existing message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$message` | string|null | An existing message to prepend to. |
| `$user` | string | The username. Default: `CMS_SUPERUSER`. |

**Returns:** `string` — The generated system prompt.

**Mechanisms:**
- Retrieves user and group information from the permission data store.
- Constructs a welcome message with identity, designation, and group membership.
- Appends instructions about checking memories and resources.
- Prepends the existing message if provided.

**Usage:**
```php
$prompt = mcp::system_prompt("Additional instructions here.");
echo $prompt;
```

---

#### permission_list

Formats an array of permissions into a readable string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | array | An array of permission keys and names. |

**Returns:** `string` — A formatted string of permissions.

**Mechanisms:**
- Iterates over the array and formats each entry as `` `key` (name) ``.
- Returns `"(none)"` if the array is empty.

**Usage:**
```php
$list = mcp::permission_list(["user.alice" => "Alice", "user.bob" => "Bob"]);
echo $list; // `alice` (Alice), `bob` (Bob)
```


<!-- HASH:fd57d9a95e79fd8f412c2be14456e651 -->
