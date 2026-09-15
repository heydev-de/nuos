# PWNC API Documentation

[← Index](../../README.md) | [`mcp/#mcpb/bridge.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/mcp/%23mcpb/bridge.js)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# MCP STDIO Bridge

## Overview

The `bridge.js` file implements a **MCP (Model Context Protocol) STDIO bridge** for the PWNC Web Platform. It acts as a bidirectional communication layer between an MCP-compliant client (e.g., a language model tool consumer) and a remote PWNC server endpoint.

The bridge reads JSON-RPC 2.0 messages from standard input (`stdin`), forwards them over HTTP(S) to a configured PWNC server, and relays the responses back to standard output (`stdout`). It supports request cancellation, metadata mirroring via headers, and proper handling of notifications and streaming responses.

### Configuration Constants

| Name | Default | Description |
|------|---------|-------------|
| `TARGET` | `process.env.PWNC_URL` | Base URL of the remote PWNC server. Must be set. |
| `TOKEN` | `process.env.PWNC_TOKEN` | Bearer token used for authentication. Must be set. |
| `VERSION` | `"2026-07-28"` | Protocol version string sent in `MCP-Protocol-Version` header. |

---

## Global State

| Name | Type | Description |
|------|------|-------------|
| `pending` | `Map` | Tracks in-flight HTTP requests by their JSON-RPC `id`, enabling cancellation. |

---

## Functions

### `send(value)`

Sends a JSON-serializable value to stdout as a newline-delimited JSON message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `value` | `any` | Any JSON-serializable object or value to send. |

#### Return Value

None.

#### Inner Mechanism

Uses `JSON.stringify()` to serialize the value and writes it to `process.stdout` followed by a newline character (`\n`).

#### Usage Example

```javascript
send({ jsonrpc: "2.0", id: 1, result: "ok" });
// Outputs: {"jsonrpc":"2.0","id":1,"result":"ok"}\n
```

---

### `log(message)`

Logs a diagnostic message to stderr prefixed with `PWNC:`.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `message` | `string` | The message to log. |

#### Return Value

None.

#### Inner Mechanism

Writes the message to `process.stderr` with a `PWNC: ` prefix and a trailing newline.

#### Usage Example

```javascript
log("Starting bridge...");
// Outputs to stderr: PWNC: Starting bridge...
```

---

### `error(id, code, message)`

Sends a JSON-RPC error response or logs the message if it's a notification.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `id` | `number \| string \| null \| undefined` | The JSON-RPC request ID. If `null` or `undefined`, the message is logged instead of sent. |
| `code` | `number` | JSON-RPC error code (e.g., `-32603` for internal error). |
| `message` | `string` | Human-readable error message. |

#### Return Value

None.

#### Inner Mechanism

If `id` is `null` or `undefined`, the function assumes this is a notification and logs the message. Otherwise, it sends a structured JSON-RPC 2.0 error response via `send()`.

#### Usage Example

```javascript
error(1, -32603, "Internal error occurred");
// Sends: {"jsonrpc":"2.0","id":1,"error":{"code":-32603,"message":"Internal error occurred"}}
```

---

### `header_value(value)`

Encodes a string for safe use in HTTP headers, using base64 encoding if necessary.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `value` | `string` | The header value to encode. |

#### Return Value

`string` – Either the original value (if safe) or a base64-encoded RFC 2047-style string.

#### Inner Mechanism

Checks if the value contains only printable ASCII characters (excluding spaces) and does not match a base64 pattern. If valid, returns the value as-is. Otherwise, encodes it using base64 with `=?base64?...?=` wrapping.

#### Usage Example

```javascript
header_value("tools/call");
// Returns: "tools/call"

header_value("Hello World!");
// Returns: "=?base64?SGVsbG8gV29ybGQh?="
```

---

### `mirror_name(json)`

Extracts a name or URI from a JSON-RPC request for inclusion in HTTP headers.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `json` | `object` | The parsed JSON-RPC request object. |

#### Return Value

`string \| undefined` – The extracted name/URI, or `undefined` if not applicable.

#### Inner Mechanism

Inspects the `json.method` field and extracts `params.name` or `params.uri` depending on the method type (`tools/call`, `prompts/get`, or `resources/read`).

#### Usage Example

```javascript
mirror_name({ method: "tools/call", params: { name: "my_tool" } });
// Returns: "my_tool"

mirror_name({ method: "resources/read", params: { uri: "file:///test.txt" } });
// Returns: "file:///test.txt"
```

---

### `forward(json)`

Forwards a JSON-RPC request to the remote PWNC server via HTTP(S) POST.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `json` | `object` | The parsed JSON-RPC request object. |

#### Return Value

None.

#### Inner Mechanism

1. Serializes the JSON-RPC request into a UTF-8 buffer.
2. Constructs HTTP headers including:
   - `Content-Type`, `Content-Length`
   - `Authorization: Bearer <TOKEN>`
   - `MCP-Protocol-Version` from metadata or default
   - `MCP-Method` from the request method
   - `MCP-Name` if a name/URI is mirrored
3. Sends the request using Node.js `http` or `https` module based on the target protocol.
4. On response:
   - Deletes the request from `pending`
   - Handles `202 Accepted` (notifications)
   - Rejects `text/event-stream` responses
   - Parses and forwards the JSON response via `send()`
5. On error:
   - Deletes the request from `pending`
   - Sends an error response unless the request was cancelled

#### Usage Example

```javascript
forward({
  jsonrpc: "2.0",
  id: 1,
  method: "tools/call",
  params: { name: "list_files", arguments: {} }
});
// Forwards the request to the PWNC server and waits for response
```

---

## Main Entry Point

### Initialization

The script checks for required environment variables:

```javascript
if (TARGET === "") { log("PWNC_URL is not set.");   process.exit(1); };
if (TOKEN  === "") { log("PWNC_TOKEN is not set."); process.exit(1); };
```

Exits with code 1 if either `PWNC_URL` or `PWNC_TOKEN` is missing.

### STDIN Listener

Uses Node.js `readline` to listen for incoming JSON-RPC messages on stdin:

```javascript
readline.createInterface({input: process.stdin})
```

#### `.on("line", ...)` Handler

Processes each line of input:

1. Ignores empty lines.
2. Parses the line as JSON.
3. Discards input without a `method` field.
4. Handles `notifications/cancelled` by destroying the associated HTTP request.
5. Forwards all other valid JSON-RPC requests via `forward()`.

#### `.on("close", ...)` Handler

Terminates the process when stdin closes:

```javascript
.on("close", () => process.exit(0));
```

---

## Usage Scenario

This bridge is typically used as a **STDIO transport** for MCP clients that communicate with a remote PWNC server. The client sends JSON-RPC messages to the bridge's stdin, and the bridge forwards them to the server, returning responses to stdout.

### Example Workflow

1. **Environment Setup**:
   ```bash
   export PWNC_URL="https://pwnc.example.com/mcp"
   export PWNC_TOKEN="your-secret-token"
   ```

2. **Run the Bridge**:
   ```bash
   node bridge.js
   ```

3. **Client Sends Request** (via stdin):
   ```json
   {"jsonrpc":"2.0","id":1,"method":"tools/list"}
   ```

4. **Bridge Forwards** to `https://pwnc.example.com/mcp` with appropriate headers.

5. **Server Responds**, and the bridge sends the response back to stdout:
   ```json
   {"jsonrpc":"2.0","id":1,"result":{"tools":[...]}}
   ```

6. **Cancellation** (if supported by client):
   ```json
   {"jsonrpc":"2.0","method":"notifications/cancelled","params":{"requestId":1}}
   ```
   The bridge destroys the pending HTTP request and stops waiting for a response.


<!-- HASH:d680800245b777b987ca1c6d4070c2f7 -->
