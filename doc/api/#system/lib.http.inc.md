# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.http.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## HTTP Functions

This file provides low-level HTTP client functionality for the NUOS platform. It includes functions for making HTTP requests (GET, POST, HEAD), handling timeouts, chunked data transfer, and response parsing. These utilities are designed for internal use by higher-level modules requiring external HTTP communication.

---

### Constants

| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_HTTP_TIMEOUT_TOTAL` | `10`          | Total timeout in seconds for HTTP operations.                              |
| `CMS_HTTP_TIMEOUT_CHUNK` | `3`           | Timeout in seconds for individual data chunks during transfer.             |
| `CMS_HTTP_SIZE_CHUNK`    | `524289`      | Maximum size in bytes for a single data chunk (512 KB + 1 byte).           |
| `CMS_HTTP_LIMIT`         | `1048576`     | Maximum total response size in bytes (1 MB).                               |

---

### `http_fopen`

#### Purpose
Establishes an HTTP connection to a remote server and sends a request. Supports GET, POST, and HEAD methods. Returns either a file handle for data streaming or parsed header information.

#### Parameters

| Name               | Type      | Description                                                                 |
|--------------------|-----------|-----------------------------------------------------------------------------|
| `$url`             | `string`  | Full URL to connect to. Must include scheme (`http` or `https`).           |
| `$get_header_only` | `bool`    | If `TRUE`, returns parsed headers only. If `FALSE`, returns a file handle.  |
| `$post_data`       | `array`   | Associative array of POST data. If provided, a POST request is sent.       |

#### Return Values
- **`resource|array|FALSE`**:
  - On success with `$get_header_only=TRUE`: Associative array of parsed headers, including `#status` for the HTTP status code.
  - On success with `$get_header_only=FALSE`: File handle (`resource`) for reading response data.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. **URL Analysis**: Uses `analyze_url()` to decompose the URL into components (scheme, host, path, query, user, pass).
2. **Scheme Handling**: Supports `http` and `https` (with SSL context, peer verification disabled).
3. **Connection**: Uses `stream_socket_client()` with timeouts and blocking modes.
4. **Request Construction**: Builds HTTP/1.0 requests with `Host`, `User-Agent`, and `Authorization` headers if credentials are provided.
5. **POST Data**: Encodes POST data as `x-www-form-urlencoded` and sets `Content-Type` and `Content-Length`.
6. **Header Parsing**: Reads and parses response headers into an associative array (lowercase keys).

#### Usage Context
- **Typical Scenarios**:
  - Fetching remote resources (e.g., APIs, external content).
  - Checking HTTP headers (e.g., for redirects, content types, or status codes).
  - Sending form data to external endpoints.
- **Integration**: Used by `http_header()`, `http_post()`, and higher-level utilities like `cms_load()` for remote module fetching.

---

### `http_send`

#### Purpose
Sends data over an established HTTP connection in non-blocking mode with chunked timeout handling.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | File handle from `http_fopen()`.     |
| `$data`  | `string`   | Data to send.                        |

#### Return Values
- **`bool`**:
  - `TRUE` on success.
  - `FALSE` on failure (timeout or write error).

#### Inner Mechanisms
1. **Non-Blocking Mode**: Temporarily sets the stream to non-blocking to handle timeouts.
2. **Chunked Transfer**: Sends data in segments, checking for timeouts between chunks.
3. **Timeout Handling**: Fails if no data is sent within `CMS_HTTP_TIMEOUT_CHUNK`.

#### Usage Context
- **Internal Use**: Called by `http_fopen()` to transmit the HTTP request.
- **Error Handling**: Ensures partial writes or timeouts are detected early.

---

### `http_fetch_header`

#### Purpose
Reads a single HTTP header line from a connection, handling timeouts and empty lines.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | File handle from `http_fopen()`.     |

#### Return Values
- **`string|FALSE`**:
  - On success: A single header line (trimmed).
  - On empty line: `""` (end of headers).
  - On failure: `FALSE` (timeout or error).

#### Inner Mechanisms
1. **Non-Blocking Mode**: Temporarily sets the stream to non-blocking.
2. **Line Reading**: Uses `stream_get_line()` to read up to `CMS_HTTP_SIZE_CHUNK` bytes or until `\n`.
3. **Timeout Handling**: Fails if no data is received within `CMS_HTTP_TIMEOUT_CHUNK`.

#### Usage Context
- **Internal Use**: Called by `http_fopen()` to parse response headers.

---

### `http_fetch_data`

#### Purpose
Reads the entire response body from an HTTP connection, enforcing size limits and timeouts.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | File handle from `http_fopen()`.     |

#### Return Values
- **`string|FALSE`**:
  - On success: Response body (up to `CMS_HTTP_LIMIT` bytes).
  - On failure: `FALSE` (timeout, error, or size limit exceeded).

#### Inner Mechanisms
1. **Non-Blocking Mode**: Temporarily sets the stream to non-blocking.
2. **Chunked Reading**: Uses `stream_get_contents()` to read data in chunks of `CMS_HTTP_SIZE_CHUNK`.
3. **Timeout Handling**: Enforces both total (`CMS_HTTP_TIMEOUT_TOTAL`) and chunk (`CMS_HTTP_TIMEOUT_CHUNK`) timeouts.
4. **Size Limit**: Stops reading if the response exceeds `CMS_HTTP_LIMIT`.

#### Usage Context
- **Typical Scenarios**:
  - Fetching remote content (e.g., JSON, HTML, or binary data).
  - Downloading files or media.
- **Integration**: Used by `http_post()` and other high-level fetchers.

---

### `http_header`

#### Purpose
Convenience wrapper for fetching HTTP headers from a URL.

#### Parameters

| Name   | Type     | Description               |
|--------|----------|---------------------------|
| `$url` | `string` | URL to fetch headers from.|

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array of headers (see `http_fopen`).
  - On failure: `FALSE`.

#### Usage Context
- **Typical Scenarios**:
  - Checking remote resource status (e.g., 200, 404).
  - Validating content types or redirects before fetching data.

---

### `http_post`

#### Purpose
Sends a POST request with form data to a URL and returns the response body.

#### Parameters

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$url`  | `string` | URL to send the POST request to.     |
| `$data` | `array`  | Associative array of POST data.      |

#### Return Values
- **`string|FALSE`**:
  - On success: Response body.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. **Input Validation**: Rejects non-array `$data`.
2. **Connection**: Uses `http_fopen()` to establish a POST connection.
3. **Data Fetching**: Uses `http_fetch_data()` to read the response.

#### Usage Context
- **Typical Scenarios**:
  - Submitting form data to APIs.
  - Interacting with external services requiring POST requests.


<!-- HASH:50d4984ec9627b1c4c1aaae6d481df2a -->
