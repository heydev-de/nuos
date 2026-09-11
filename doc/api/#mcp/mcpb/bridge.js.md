# PWNC API Documentation

[← Index](../../README.md) | [`#mcp/mcpb/bridge.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23mcp/mcpb/bridge.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# MCP STDIO Bridge

## Overview

The `bridge.js` file implements a **STDIO-based MCP (Model Context Protocol) bridge** for the PWNC Web Platform. It acts as a communication intermediary between an MCP client (e.g., an AI assistant or tool consumer) and a remote PWNC server endpoint.

The bridge reads JSON-RPC 2.0 messages from standard input (`stdin`), forwards them over HTTP/HTTPS to a configured PWNC server, and relays the responses back to the client via standard output (`stdout`). It supports request cancellation, notifications, and proper error handling.

### Configuration

| Variable | Source | Description |
|----------|--------|-------------|
| `TARGET` | `process.env.PWNC_URL` | Base URL of the PWNC server (e.g., `https://pwnc.it/api/mcp`) |
| `TOKEN`  | `process.env.PWNC_TOKEN` | Bearer token used for authentication with the PWNC server |
| `VERSION`| Hardcoded (`"2026-07-28"`) | Default MCP protocol version sent in headers if not specified in request metadata |

### Internal State

| Variable | Type | Description |
|----------|------|-------------|
| `pending` | `Map` | Tracks in-flight HTTP requests by their JSON-RPC ID, enabling cancellation support |

---

## Functions

### `send(value)`

Sends a JSON-serializable value to stdout as a newline-delimited JSON message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `value` | `any` | Any JSON-serializable object to be sent to the client |

#### Return Value

None.

#### Mechanism

Serializes the input using `JSON.stringify()` and writes it to `process.stdout`, appending a newline character (`\n`) to conform to the MCP/STDIO message framing format.

#### Usage Example

```javascript
send({ jsonrpc: "2.0", id: 1, result: { status: "ok" } });
// Output: {"jsonrpc":"2.0","id":1,"result":{"status":"ok"}}\n
```

---

### `log(message)`

Logs a diagnostic message to stderr prefixed with `PWNC:`.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `message` | `string` | Message to log |

#### Return Value

None.

#### Mechanism

Writes the message to `process.stderr` with a `PWNC:` prefix, ensuring logs do not interfere with stdout-based JSON-RPC communication.

#### Usage Example

```javascript
log("Starting bridge...");
// stderr: PWNC: Starting bridge...
```

---

### `error(id, code, message)`

Sends a JSON-RPC error response or logs the error if it's a notification.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `id` | `number \| string \| null \| undefined` | JSON-RPC request ID; if `null` or `undefined`, the message is treated as a notification and only logged |
| `code` | `number` | JSON-RPC error code (e.g., `-32603` for internal error) |
| `message` | `string` | Human-readable error description |

#### Return Value

None.

#### Mechanism

If the `id` is present, sends a structured JSON-RPC 2.0 error response via `send()`. Otherwise, logs the message using `log()`.

#### Usage Example

```javascript
error(1, -32603, "Internal error occurred");
// stdout: {"jsonrpc":"2.0","id":1,"error":{"code":-32603,"message":"Internal error occurred"}}

error(null, -32603, "Notification failed");
// stderr: PWNC: Notification failed
```

---

### `header_value(value)`

Encodes a string value for safe use in HTTP headers, applying base64 encoding if necessary.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `value` | `string` | The header value to encode |

#### Return Value

`string` – Either the original value (if safe) or a base64-encoded MIME-style encoded word.

#### Mechanism

Checks if the value contains only printable ASCII characters (excluding spaces) and does not match a base64-encoded pattern. If valid, returns the value as-is. Otherwise, encodes it using base64 with `=?base64?...?=` format.

#### Usage Example

```javascript
header_value("simple-name");
// Returns: "simple-name"

header_value("café");
// Returns: "=?base64?Y2Fmw6k=?="
```

---

### `mirror_name(json)`

Extracts a name or URI from specific JSON-RPC methods for inclusion in HTTP headers.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `json` | `object` | Parsed JSON-RPC request object |

#### Return Value

`string \| undefined` – The extracted name/URI, or `undefined` if the method doesn't require mirroring.

#### Mechanism

Inspects the `json.method` field and extracts `params.name` or `params.uri` for known methods (`tools/call`, `prompts/get`, `resources/read`).

#### Usage Example

```javascript
mirror_name({ method: "tools/call", params: { name: "calculator" } });
// Returns: "calculator"

mirror_name({ method: "ping" });
// Returns: undefined
```

---

### `forward(json)`

Forwards a JSON-RPC request to the PWNC server via HTTP/HTTPS and handles the response.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `json` | `object` | Parsed JSON-RPC request object |

#### Return Value

None.

#### Mechanism

1. Serializes the JSON-RPC request into a UTF-8 buffer.
2. Constructs HTTP headers including:
   - `Content-Type`, `Content-Length`
   - `Authorization` (Bearer token)
   - `MCP-Protocol-Version` (from metadata or default)
   - `MCP-Method` (the JSON-RPC method name)
   - `MCP-Name` (if applicable, derived from `mirror_name`)
3. Sends the request using Node.js `http` or `https` module depending on the target protocol.
4. On response:
   - Parses the response body.
   - Handles special cases:
     - `202 Accepted`: No response body (notification).
     - `text/event-stream`: Unsupported streaming response.
   - Forwards the parsed JSON response to the client via `send()`.
5. On error:
   - Deletes the request from `pending`.
   - Sends an appropriate JSON-RPC error.
6. Registers cancellable requests in the `pending` map.

#### Usage Example

```javascript
forward({
  jsonrpc: "2.0",
  id: 1,
  method: "tools/call",
  params: { name: "calculator", arguments: { a: 5, b: 3 } }
});
// Sends POST to PWNC server with headers and body, then relays response
```

---

## Main Execution Flow

### Initialization

1. Validates that `PWNC_URL` and `PWNC_TOKEN` environment variables are set.
2. Exits with code 1 if either is missing.

### Input Handling

Uses Node.js `readline` interface to read lines from `stdin`:

1. **Empty lines**: Ignored.
2. **Invalid JSON**: Logs a parse error.
3. **Missing method**: Logs and discards.
4. **Cancellation notification** (`notifications/cancelled`):
   - Retrieves the corresponding pending request.
   - Marks it as cancelled and destroys the underlying HTTP request.
5. **All other requests**: Forwarded to the PWNC server via `forward()`.

### Termination

When `stdin` closes (EOF), the process exits cleanly with code 0.

#### Usage Scenario

A typical deployment might look like:

```bash
export PWNC_URL="https://pwnc.it/api/mcp"
export PWNC_TOKEN="your-secret-token"
node bridge.js
```

The bridge would then be ready to receive JSON-RPC messages on stdin and forward them to the PWNC server.


<!-- HASH:d680800245b777b987ca1c6d4070c2f7 -->
