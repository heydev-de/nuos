# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.http.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.http.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTTP Functions

This file provides a lightweight, dependency-free HTTP client implementation for the PWNC Web Platform. It operates at a low level using PHP's `stream_socket_client` and stream functions, avoiding cURL entirely. The library supports HTTP/HTTPS, chunked transfer encoding, multipart form uploads, Basic authentication, and configurable timeouts.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_HTTP_TIMEOUT_TOTAL` | `10` | Default total timeout in seconds for an entire HTTP operation. |
| `CMS_HTTP_TIMEOUT_CHUNK` | `3` | Default per-chunk timeout in seconds; if no data arrives within this window, the operation fails. |
| `CMS_HTTP_SIZE_CHUNK` | `524289` | Size in bytes of each read/write chunk (512 KB + 1 byte). Used for both sending and receiving data. |
| `CMS_HTTP_LIMIT` | `1048576` | Default maximum response body size in bytes (1 MB). Prevents unbounded memory consumption. |

---

### http_fopen

```php
function http_fopen($url, $post_data = NULL, &$header = NULL, $option = NULL)
```

#### Purpose

Establishes an HTTP/HTTPS connection, sends a request (GET or POST), reads the response status line and headers, and returns the open stream handle for further body reading. This is the core function upon which all other HTTP functions in this library are built.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | `string` | — | The target URL. May include scheme, host, port, path, query, and Basic auth credentials (e.g., `https://user:pass@example.com/path`). |
| `$post_data` | `array\|NULL` | `NULL` | If an array, it is passed to `http_build_post()` to construct a `multipart/form-data` body. If `NULL`, a GET request is made (unless `$option["body"]` is set). |
| `$header` | `array` (by reference) | `NULL` | Populated with parsed response headers. Special keys `#version`, `#status`, and `#message` hold the HTTP version, status code, and status message. Other keys are lowercased header field names. `set-cookie` headers are collected as arrays. |
| `$option` | `array\|NULL` | `NULL` | Optional configuration. Supported keys: `verify` (bool, SSL peer verification), `body` (string, raw request body), `header` (array, custom request headers), `timeout_total` (int), `timeout_chunk` (int), `limit` (int, response size limit). |

#### Return Values

| Type | Description |
|------|-------------|
| `resource` | A stream handle on success, ready for `http_fetch_data()` or `http_get_contents()`. |
| `FALSE` | On any failure: invalid URL, unsupported scheme, connection error, or malformed response. |

#### Inner Mechanisms

1. **URL Parsing**: Calls `analyze_url()` to decompose the URL into components. Returns `FALSE` if the URL is invalid or has no host.
2. **Transport Selection**: Maps `http` to `tcp://` (port 80) and `https` to `ssl://` (port 443). For HTTPS, creates a stream context with SSL verification controlled by `$option["verify"]`.
3. **Default Headers**: Sets `Host`, `User-Agent` (from `CMS_USER_AGENT`), `Accept-Encoding: identity`, and `Connection: close`.
4. **Authorization**: If the URL contains user info (`user:pass@`), encodes it as a Basic Authorization header.
5. **Body Construction**: If `$post_data` is an array, delegates to `http_build_post()`. If `$option["body"]` is set, uses it as a raw body and sets `Content-Length`.
6. **Method Determination**: Uses `POST` if a body exists, otherwise `GET`.
7. **Header Merging**: Custom headers from `$option["header"]` are merged with defaults, with defaults filling in any missing keys (case-insensitive).
8. **Connection**: Opens a socket via `stream_socket_client()` with the total timeout.
9. **State Initialization**: Stores `timeout_total`, `timeout_chunk`, and `limit` on the handle via `http_state()`.
10. **Request Transmission**: Sends the request line and headers via `http_send()`, then sends the body in chunks.
11. **Response Parsing**: Reads up to 10 header lines to handle 1xx interim responses. Parses the status line with a regex. Then reads remaining headers until an empty line.
12. **Chunked State**: Sets `chunk_remaining` to `0` if `Transfer-Encoding: chunked` is detected, or `FALSE` otherwise.

#### Usage Example

```php
// Simple GET request
$headers = [];
$hfile = http_fopen("https://api.example.com/data", NULL, $headers);
if ($hfile !== FALSE) {
    $body = http_fetch_data($hfile);
    echo "Status: " . $headers["#status"] . "\n";
    echo "Body: " . $body . "\n";
}

// POST with form data
$headers = [];
$hfile = http_fopen("https://api.example.com/submit", ["name" => "John", "email" => "john@example.com"], $headers);
if ($hfile !== FALSE) {
    $body = http_fetch_data($hfile);
    echo "Response: " . $body . "\n";
}

// POST with raw body and custom headers
$headers = [];
$hfile = http_fopen("https://api.example.com/json", NULL, $headers, [
    "body" => json_encode(["key" => "value"]),
    "header" => ["Content-Type: application/json"],
    "verify" => TRUE
]);
if ($hfile !== FALSE) {
    $body = http_fetch_data($hfile);
    echo "Response: " . $body . "\n";
}
```

---

### http_send

```php
function http_send($hfile, $data)
```

#### Purpose

Sends a string of data over an open HTTP stream handle in chunks, using non-blocking I/O with timeout protection. Used internally by `http_fopen()` to transmit request headers and body.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The stream handle returned by `stream_socket_client()`. |
| `$data` | `string` | The data to send. |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | All data was successfully written. |
| `FALSE` | A write error occurred or the chunk timeout was exceeded. |

#### Inner Mechanisms

1. Sets the stream to non-blocking mode.
2. Loops, writing up to `CMS_HTTP_SIZE_CHUNK` bytes per iteration via `fwrite()`.
3. If `fwrite()` returns `0` (no progress), checks if the chunk timeout (`CMS_HTTP_TIMEOUT_CHUNK`) has elapsed. If so, returns `FALSE`. Otherwise, sleeps for 10ms and retries.
4. Restores blocking mode before returning.

#### Usage Example

```php
// Typically called internally, but can be used directly:
$hfile = stream_socket_client("tcp://example.com:80", $errno, $errstr, 10);
http_send($hfile, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
$response = http_get_contents($hfile);
fclose($hfile);
```

---

### http_fetch_header

```php
function http_fetch_header($hfile)
```

#### Purpose

Reads a single line from the HTTP response headers on an open stream handle. Used internally by `http_fopen()` to parse the response status line and header fields.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The stream handle. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The trimmed header line. Returns `""` (empty string) for the end-of-headers marker (blank line). |
| `FALSE` | On timeout or read error. |

#### Inner Mechanisms

1. Sets the stream to non-blocking mode.
2. Loops while not at EOF, reading lines via `stream_get_line()` with a newline delimiter.
3. If no data is returned, checks the chunk timeout. If exceeded, returns `FALSE`. Otherwise, sleeps 10ms and retries.
4. Returns `""` for empty lines or lines containing only `\r` (end of headers).
5. Trims and returns non-empty lines.
6. Restores blocking mode if EOF is reached without finding a line.

#### Usage Example

```php
// Internal use within http_fopen; not typically called directly by application code.
$hfile = stream_socket_client("tcp://example.com:80", $errno, $errstr, 10);
http_send($hfile, "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
while (($line = http_fetch_header($hfile)) !== FALSE && $line !== "") {
    echo "Header: " . $line . "\n";
}
fclose($hfile);
```

---

### http_fetch_data

```php
function http_fetch_data($hfile)
```

#### Purpose

Reads the complete response body from an open HTTP stream handle, then closes the handle and clears its state. This is the standard way to retrieve the body after `http_fopen()` has returned a handle.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The stream handle returned by `http_fopen()`. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The response body. |
| `FALSE` | If the handle is invalid or closed. |

#### Inner Mechanisms

1. Sets the stream to non-blocking mode.
2. Calls `http_get_contents()` with the limit stored in the handle's state.
3. Closes the stream handle.
4. Clears the handle's state via `http_state($hfile)` (with no key/value, which discards).
5. Returns the buffer.

#### Usage Example

```php
$headers = [];
$hfile = http_fopen("https://api.example.com/users/123", NULL, $headers);
if ($hfile !== FALSE) {
    $body = http_fetch_data($hfile);
    $user = json_decode($body, TRUE);
    print_r($user);
}
```

---

### http_state

```php
function http_state($hfile, $key = NULL, $value = NULL)
```

#### Purpose

A static state manager that associates arbitrary key-value data with a specific stream handle. Used internally to track timeouts, limits, and chunked transfer state per connection.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$hfile` | `resource` | — | The stream handle. Its integer ID is used as the storage key. |
| `$key` | `string\|NULL` | `NULL` | The state key to read or write. If `NULL`, the handle's state is discarded. |
| `$value` | `mixed\|NULL` | `NULL` | The value to write. If `NULL` (and `$key` is not `NULL`), the function reads the current value for `$key`. |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful write or discard. |
| `mixed` | The stored value when reading (or `NULL` if not set). |
| `NULL` | When discarding state on a closed handle, or when the handle is not a valid resource. |

#### Inner Mechanisms

1. Uses a `static $array` to persist state across calls.
2. Validates that the handle's resource ID still maps to the same resource (handles can be reused with different resources at the same ID).
3. If `$key` is `NULL`, unsets the handle's state entry and returns `TRUE`.
4. If `$value` is `NULL`, reads and returns the stored value for `$key`.
5. Otherwise, writes `$value` to `$array[$id][$key]` and returns `TRUE`.

#### Usage Example

```php
// Internal usage; not typically called directly.
$hfile = stream_socket_client("tcp://example.com:80", $errno, $errstr, 10);
http_state($hfile, "timeout_total", 15);
http_state($hfile, "limit", 2048);
echo http_state($hfile, "timeout_total"); // Outputs: 15
http_state($hfile); // Discards all state for this handle
```

---

### http_get_contents

```php
function http_get_contents($hfile, $length = NULL)
```

#### Purpose

Reads response body data from a stream handle, handling both `Content-Length` and `Transfer-Encoding: chunked` responses. Respects per-chunk and total timeouts, as well as a maximum length limit.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$hfile` | `resource` | — | The stream handle. |
| `$length` | `int\|NULL` | `NULL` | Maximum number of bytes to read. If `NULL`, reads until the connection closes or the stream ends. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The response body data. |
| `FALSE` | On error, timeout, or invalid handle. |

#### Inner Mechanisms

1. Retrieves `chunk_remaining` from the handle's state. If `NULL`, the handle is invalid/closed; returns `FALSE`.
2. If `chunk_remaining` is `FALSE`, the response is not chunked; sets `$remaining` to `CMS_HTTP_SIZE_CHUNK` for non-chunked reads.
3. If `chunk_remaining` is `0`, the response is chunked; reads the chunk size line (hex-encoded), converts to decimal, and reads that many bytes.
4. Reads data in chunks via `stream_get_contents()`, respecting the `$length` limit.
5. On empty reads, checks both total and per-chunk timeouts. If exceeded, returns `FALSE`. Otherwise, sleeps 10ms and retries.
6. For chunked responses, after reading a complete chunk, consumes the trailing CRLF.
7. Stores the updated `chunk_remaining` back to state for subsequent calls.

#### Usage Example

```php
// Internal usage within http_fetch_data; can be used directly for streaming reads.
$hfile = http_fopen("https://example.com/largefile", NULL, $headers);
if ($hfile !== FALSE) {
    // Read in 4KB chunks
    while (!feof($hfile)) {
        $chunk = http_get_contents($hfile, 4096);
        if ($chunk === FALSE) break;
        echo $chunk;
    }
    fclose($hfile);
}
```

---

### http_header

```php
function http_header($url)
```

#### Purpose

A convenience function that performs an HTTP request and returns only the response headers, discarding the body.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The target URL. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Parsed response headers (same structure as the `$header` parameter of `http_fopen()`). |
| `FALSE` | On connection or request failure. |

#### Inner Mechanisms

1. Calls `http_fopen()` with `NULL` post data to perform a GET request.
2. If the handle is valid, immediately closes it (discarding the body).
3. Returns the populated `$header` array.

#### Usage Example

```php
$headers = http_header("https://api.example.com/status");
if ($headers !== FALSE) {
    echo "HTTP Status: " . $headers["#status"] . "\n";
    echo "Content-Type: " . ($headers["content-type"] ?? "unknown") . "\n";
    echo "Server: " . ($headers["server"] ?? "unknown") . "\n";
}
```

---

### http_request

```php
function http_request($url, $option = NULL, &$header = NULL)
```

#### Purpose

Performs an HTTP request (GET by default) and returns the response body. This is the primary high-level function for making HTTP requests in the PWNC platform.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | `string` | — | The target URL. |
| `$option` | `array\|NULL` | `NULL` | Optional configuration. Supports `verify` (bool, defaults to `TRUE`), `body` (string, raw request body), `header` (array, custom request headers), `timeout_total`, `timeout_chunk`, and `limit`. |
| `$header` | `array` (by reference) | `NULL` | Populated with parsed response headers. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The response body. |
| `FALSE` | On failure. |

#### Inner Mechanisms

1. Defaults `$option["verify"]` to `TRUE` for SSL certificate verification.
2. Calls `http_fopen()` with `NULL` post data (GET request) and the provided options.
3. If the handle is valid, calls `http_fetch_data()` to retrieve and return the body.

#### Usage Example

```php
// Simple GET
$headers = [];
$body = http_request("https://api.example.com/users", NULL, $headers);
if ($body !== FALSE) {
    echo "Status: " . $headers["#status"] . "\n";
    $users = json_decode($body, TRUE);
    print_r($users);
}

// GET with custom headers
$headers = [];
$body = http_request("https://api.example.com/users", [
    "header" => ["X-API-Key: secret123", "Accept: application/json"]
], $headers);

// POST with raw JSON body
$headers = [];
$body = http_request("https://api.example.com/users", [
    "body" => json_encode(["name" => "Alice", "email" => "alice@example.com"]),
    "header" => ["Content-Type: application/json"]
], $headers);
```

---

### http_post

```php
function http_post($url, $data, &$header = NULL)
```

#### Purpose

Performs an HTTP POST request with multipart form data (including file uploads) and returns the response body.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | `string` | — | The target URL. |
| `$data` | `array` | — | Form data array. Scalar values become regular form fields. File uploads use the format `["data" => "...", "filename" => "...", "type" => "..."]`. |
| `$header` | `array` (by reference) | `NULL` | Populated with parsed response headers. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The response body. |
| `FALSE` | On failure or if `$data` is not an array. |

#### Inner Mechanisms

1. Validates that `$data` is an array; returns `FALSE` otherwise.
2. Calls `http_fopen()` with `$data` as post data, which triggers `http_build_post()` to construct the multipart body.
3. If the handle is valid, calls `http_fetch_data()` to retrieve and return the body.

#### Usage Example

```php
// Simple form POST
$headers = [];
$body = http_post("https://api.example.com/login", [
    "username" => "admin",
    "password" => "secret"
], $headers);
echo "Login response: " . $body . "\n";

// File upload
$headers = [];
$body = http_post("https://api.example.com/upload", [
    "title" => "My Photo",
    "file" => [
        "data" => base64_encode(file_get_contents("/path/to/photo.jpg")),
        "filename" => "photo.jpg",
        "type" => "image/jpeg"
    ]
], $headers);
echo "Upload response: " . $body . "\n";
```

---

### http_build_post

```php
function http_build_post($data)
```

#### Purpose

Constructs a `multipart/form-data` request body from a nested array, supporting both scalar fields and file uploads. Returns the Content-Type header (with boundary) and the body as an array of string segments.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$data` | `array` | The form data. Nested arrays use PHP-style bracket notation (e.g., `field[subkey]`). File uploads are arrays containing a `data` key. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | A two-element array: `[0]` is the Content-Type and Content-Length header string, `[1]` is an array of body string segments. |

#### Inner Mechanisms

1. Creates a `RecursiveArrayIterator` subclass that treats arrays containing a `data` key as leaves (file uploads), preventing further recursion into them.
2. Wraps the iterator in a `RecursiveIteratorIterator` with `LEAVES_ONLY` mode to flatten the structure.
3. Generates a unique multipart boundary using `unique_id()`.
4. For each leaf value:
   - **File upload** (array with `data` key): Constructs a MIME part with `Content-Disposition` (including filename), `Content-Type` (from `type` key or `get_mime_type()`), and `Content-Transfer-Encoding: base64`. The file data is included as-is (expected to be base64-encoded).
   - **Scalar value**: Constructs a simple form-data part with the field name and value.
5. Appends the closing boundary.
6. Computes the total content length by summing all body segment lengths.
7. Returns the header string and body array.

#### Usage Example

```php
// Build a multipart body manually
list($header, $body) = http_build_post([
    "name" => "John Doe",
    "email" => "john@example.com",
    "avatar" => [
        "data" => base64_encode(file_get_contents("/tmp/avatar.png")),
        "filename" => "avatar.png",
        "type" => "image/png"
    ],
    "metadata" => [
        "category" => "user",
        "priority" => "high"
    ]
]);

echo $header; // Content-Type: multipart/form-data; boundary=...
// Content-Length: ...

foreach ($body as $segment) {
    echo $segment;
}
// --boundary
// Content-Disposition: form-data; name="name"
//
// John Doe
// --boundary
// Content-Disposition: form-data; name="email"
//
// john@example.com
// --boundary
// Content-Disposition: form-data; name="avatar"; filename="avatar.png"
// Content-Type: image/png
// Content-Transfer-Encoding: base64
//
// <base64 data>
// --boundary
// Content-Disposition: form-data; name="metadata[category]"
//
// user
// --boundary
// Content-Disposition: form-data; name="metadata[priority]"
//
// high
// --boundary--
```


<!-- HASH:2808d4a59823b42ce2f8675402f54178 -->
