# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.http.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.http.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTTP Functions

This file provides low-level HTTP client functionality for the PWNC Web Platform. It includes functions for making HTTP requests (GET/POST), handling chunked transfers, and managing HTTP headers. The implementation is designed to be lightweight, efficient, and compatible with the platform's no-bloat philosophy.

---

### Constants

| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_HTTP_TIMEOUT_TOTAL` | `10`          | Total timeout in seconds for HTTP operations.                              |
| `CMS_HTTP_TIMEOUT_CHUNK` | `3`           | Timeout in seconds for individual chunks during data transfer.             |
| `CMS_HTTP_SIZE_CHUNK`    | `524289`      | Maximum size in bytes for a single chunk (512KB + 1 byte).                 |
| `CMS_HTTP_LIMIT`         | `1048576`     | Maximum size in bytes for the entire response (1MB).                       |

---

### `http_fopen`

Opens an HTTP connection to a specified URL and returns a file handle for reading the response. Supports both GET and POST methods, HTTPS, and basic authentication.

#### Parameters

| Name               | Type      | Description                                                                 |
|--------------------|-----------|-----------------------------------------------------------------------------|
| `$url`             | `string`  | The URL to connect to.                                                      |
| `$get_header_only` | `bool`    | If `TRUE`, returns only the HTTP headers without the response body.         |
| `$post_data`       | `array`   | Associative array of POST data (key-value pairs). If `NULL`, a GET request is made. |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `resource`| A file handle for reading the response body on success.                    |
| `array`   | Associative array of HTTP headers if `$get_header_only` is `TRUE`.         |
| `FALSE`   | On failure (invalid URL, connection error, or HTTP error).                 |

#### Inner Mechanisms

1. **URL Analysis**: Uses `analyze_url()` to parse the URL into components (scheme, host, path, query, etc.).
2. **Scheme Handling**: Supports `http` and `https` schemes. HTTPS uses a non-validating SSL context for simplicity.
3. **Connection**: Establishes a socket connection using `stream_socket_client()` with a total timeout.
4. **Request Preparation**: Constructs the HTTP request based on the method (GET/POST) and includes headers for host, user-agent, and authorization if provided.
5. **Header Processing**: Reads and parses HTTP headers, including status codes and fields like `Set-Cookie`.
6. **Chunked Transfer Handling**: Detects and prepares for chunked transfer encoding if specified in the headers.

#### Usage Example

```php
// Fetch HTTP headers for a URL
$headers = http_fopen("https://pwnc.it", TRUE);
if ($headers !== FALSE) {
    echo "HTTP Status: " . $headers["#status"] . "\n";
    if (isset($headers["content-type"])) {
        echo "Content-Type: " . $headers["content-type"] . "\n";
    }
}

// Make a GET request and read the response
$handle = http_fopen("https://pwnc.it/api/data");
if ($handle !== FALSE) {
    $response = http_fetch_data($handle);
    echo "Response: " . $response . "\n";
}
```

---

### `http_send`

Sends data over an open HTTP connection with chunked timeout handling.

#### Parameters

| Name     | Type       | Description                     |
|----------|------------|---------------------------------|
| `$hfile` | `resource` | Open file handle for the connection. |
| `$data`  | `string`   | Data to send.                   |

#### Return Values

| Type    | Description                     |
|---------|---------------------------------|
| `TRUE`  | On success.                     |
| `FALSE` | On failure (timeout or error).  |

#### Inner Mechanisms

1. **Non-Blocking Mode**: Temporarily sets the socket to non-blocking mode to handle timeouts.
2. **Chunked Sending**: Sends data in chunks, checking for timeouts between chunks.
3. **Timeout Handling**: Fails if the total time for sending a chunk exceeds `CMS_HTTP_TIMEOUT_CHUNK`.

#### Usage Example

```php
$handle = http_fopen("https://pwnc.it/api/post", FALSE, ["key" => "value"]);
if ($handle !== FALSE) {
    $data = "Additional data to send";
    if (http_send($handle, $data)) {
        $response = http_fetch_data($handle);
        echo "Response: " . $response . "\n";
    }
}
```

---

### `http_fetch_header`

Fetches a single HTTP header line from an open connection.

#### Parameters

| Name     | Type       | Description                     |
|----------|------------|---------------------------------|
| `$hfile` | `resource` | Open file handle for the connection. |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | A single header line (e.g., `Content-Type: text/html`).                    |
| `""`      | Empty string indicates the end of headers.                                 |
| `FALSE`   | On failure (timeout or error).                                             |

#### Inner Mechanisms

1. **Non-Blocking Mode**: Uses non-blocking mode to handle timeouts while reading headers.
2. **Line Reading**: Reads data line by line using `stream_get_line()`.
3. **Timeout Handling**: Fails if no data is received within `CMS_HTTP_TIMEOUT_CHUNK`.

#### Usage Example

```php
$handle = http_fopen("https://pwnc.it");
if ($handle !== FALSE) {
    while (($header = http_fetch_header($handle)) !== FALSE) {
        if ($header === "") break; // End of headers
        echo "Header: " . $header . "\n";
    }
    $response = http_fetch_data($handle);
    echo "Response: " . $response . "\n";
}
```

---

### `http_fetch_data`

Fetches the entire response body from an open HTTP connection.

#### Parameters

| Name     | Type       | Description                     |
|----------|------------|---------------------------------|
| `$hfile` | `resource` | Open file handle for the connection. |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | The response body.                                                          |
| `FALSE`   | On failure (timeout, error, or closed handle).                              |

#### Inner Mechanisms

1. **Chunked Transfer Handling**: Uses `http_chunked()` to check if the response is chunked.
2. **Data Reading**: Reads data in chunks, respecting timeouts and size limits.
3. **Cleanup**: Closes the handle and cleans up chunked transfer state after reading.

#### Usage Example

```php
$handle = http_fopen("https://pwnc.it");
if ($handle !== FALSE) {
    $response = http_fetch_data($handle);
    echo "Response: " . $response . "\n";
}
```

---

### `http_chunked`

Manages chunked transfer encoding state for an open HTTP connection.

#### Parameters

| Name    | Type       | Description                                                                 |
|---------|------------|-----------------------------------------------------------------------------|
| `$hfile`| `resource` | Open file handle for the connection.                                        |
| `$set`  | `bool/int` | If `TRUE`, flags the handle as using chunked transfer. If an integer, sets the remaining chunk size. |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | Remaining chunk size if the handle is flagged for chunked transfer.        |
| `NULL`    | If the handle is closed or invalid.                                         |
| `FALSE`   | If the handle is not flagged for chunked transfer.                          |

#### Inner Mechanisms

1. **Static Storage**: Uses a static array to track chunked transfer state for multiple handles.
2. **Handle Identification**: Uses the resource ID to uniquely identify handles.
3. **State Management**: Stores the remaining chunk size and cleans up state when handles are closed.

#### Usage Example

```php
$handle = http_fopen("https://pwnc.it");
if ($handle !== FALSE) {
    // Check if the response is chunked
    $is_chunked = http_chunked($handle);
    if ($is_chunked !== FALSE) {
        echo "Response is chunked.\n";
    }
    $response = http_fetch_data($handle);
    echo "Response: " . $response . "\n";
}
```

---

### `http_get_contents`

Reads data from an open HTTP connection, handling both chunked and non-chunked transfers.

#### Parameters

| Name      | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| `$hfile`  | `resource` | Open file handle for the connection.                                        |
| `$length` | `int`      | Maximum number of bytes to read. If `NULL`, reads until the end of the response. |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | The data read from the connection.                                          |
| `FALSE`   | On failure (timeout, error, or closed handle).                              |

#### Inner Mechanisms

1. **Chunked Transfer Handling**: Uses `http_chunked()` to manage chunked transfer state.
2. **Data Reading**: Reads data in chunks, respecting timeouts and size limits.
3. **Timeout Handling**: Fails if the total time exceeds `CMS_HTTP_TIMEOUT_TOTAL` or if a chunk takes longer than `CMS_HTTP_TIMEOUT_CHUNK`.

#### Usage Example

```php
$handle = http_fopen("https://pwnc.it");
if ($handle !== FALSE) {
    $response = http_get_contents($handle, 1024); // Read up to 1KB
    echo "Partial Response: " . $response . "\n";
}
```

---

### `http_header`

Fetches HTTP headers for a specified URL.

#### Parameters

| Name  | Type     | Description                     |
|-------|----------|---------------------------------|
| `$url`| `string` | The URL to fetch headers from.  |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `array` | Associative array of HTTP headers, including `#status` for the status code. |
| `FALSE` | On failure.                                                                 |

#### Inner Mechanisms

1. **Wrapper**: Uses `http_fopen()` with `$get_header_only` set to `TRUE`.

#### Usage Example

```php
$headers = http_header("https://pwnc.it");
if ($headers !== FALSE) {
    echo "Status: " . $headers["#status"] . "\n";
    if (isset($headers["content-type"])) {
        echo "Content-Type: " . $headers["content-type"] . "\n";
    }
}
```

---

### `http_post`

Sends a POST request to a specified URL with form data.

#### Parameters

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$url`  | `string` | The URL to send the POST request to. |
| `$data` | `array`  | Associative array of POST data (key-value pairs). |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | The response body.                                                          |
| `FALSE`   | On failure (invalid data, connection error, or HTTP error).                 |

#### Inner Mechanisms

1. **Data Validation**: Checks if `$data` is an array.
2. **Connection**: Uses `http_fopen()` to open a connection and send the POST data.
3. **Response Handling**: Uses `http_fetch_data()` to read the response.

#### Usage Example

```php
$response = http_post("https://pwnc.it/api/submit", [
    "username" => "test",
    "password" => "secret"
]);
if ($response !== FALSE) {
    echo "Response: " . $response . "\n";
}
```


<!-- HASH:2e3d96d6bf2225aeff635b3a45fee882 -->
