# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.http.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.http.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTTP Functions

The `lib.http.inc` file provides a lightweight, dependency-free HTTP client implementation for the PWNC Web Platform. It enables making HTTP and HTTPS requests directly from PHP using low-level socket operations, supporting both GET and POST methods, including multipart form data with file uploads. The implementation handles timeouts, chunked transfer encoding, and basic authentication.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_HTTP_TIMEOUT_TOTAL` | `10` | Total timeout in seconds for HTTP operations |
| `CMS_HTTP_TIMEOUT_CHUNK` | `3` | Timeout in seconds for individual data chunks |
| `CMS_HTTP_SIZE_CHUNK` | `524289` | Maximum size in bytes for a single data chunk (512KB + 1) |
| `CMS_HTTP_LIMIT` | `1048576` | Maximum total response size in bytes (1MB) |

---

### http_fopen

#### Description
Establishes an HTTP connection to a remote server, sends a request (GET or POST), and returns a file handle for reading the response.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The target URL to connect to |
| `$post_data` | `array\|NULL` | Optional POST data array; if provided, sends a POST request |
| `&$header` | `array\|NULL` | Reference variable to store parsed response headers |

#### Return Values
- **resource**: A file handle on success for reading response data
- **FALSE**: On failure (invalid URL, connection error, etc.)

#### Inner Mechanisms
1. Parses the URL into components using `analyze_url()`
2. Determines transport protocol (`tcp://` for HTTP, `ssl://` for HTTPS)
3. Establishes a socket connection with timeout handling
4. Builds and sends HTTP request headers
5. For POST requests, constructs multipart form data using `http_build_post()`
6. Reads and parses the HTTP response status line and headers
7. Handles chunked transfer encoding if present
8. Returns the file handle for subsequent data reading

#### Usage Example
```php
// Simple GET request
$headers = [];
$hfile = http_fopen("https://api.example.com/data", NULL, $headers);
if ($hfile !== FALSE) {
    $response = http_fetch_data($hfile);
    print_r($headers); // Contains response headers
    echo $response;    // Contains response body
}

// POST request with data
$postData = ["name" => "John", "email" => "john@example.com"];
$hfile = http_fopen("https://api.example.com/submit", $postData, $headers);
if ($hfile !== FALSE) {
    $response = http_fetch_data($hfile);
    echo $response;
}
```

---

### http_send

#### Description
Sends data over an HTTP connection with timeout handling and non-blocking I/O.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The HTTP connection file handle |
| `$data` | `string` | The data to send |

#### Return Values
- **TRUE**: On successful data transmission
- **FALSE**: On error or timeout

#### Inner Mechanisms
1. Sets the connection to non-blocking mode
2. Sends data in chunks of `CMS_HTTP_SIZE_CHUNK` size
3. Implements timeout checking using `CMS_HTTP_TIMEOUT_CHUNK`
4. Uses `usleep()` to prevent busy waiting
5. Restores blocking mode before returning

#### Usage Example
```php
// This function is typically called internally by http_fopen
// but can be used directly for custom HTTP interactions
$hfile = stream_socket_client("tcp://example.com:80", $errno, $errstr, 10);
http_send($hfile, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
$response = fread($hfile, 4096);
fclose($hfile);
```

---

### http_fetch_header

#### Description
Reads a single header line from an HTTP connection with timeout handling.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The HTTP connection file handle |

#### Return Values
- **string**: A single header line (trimmed)
- **string** (empty): For end-of-header markers (empty line or `\r`)
- **FALSE**: On error or timeout

#### Inner Mechanisms
1. Sets the connection to non-blocking mode
2. Reads lines using `stream_get_line()` with newline delimiter
3. Handles timeouts using `CMS_HTTP_TIMEOUT_CHUNK`
4. Returns empty string for end-of-header markers
5. Skips empty lines and returns trimmed content

#### Usage Example
```php
// Used internally by http_fopen to parse response headers
$hfile = http_fopen("https://example.com", NULL, $headers);
// Headers are automatically parsed and stored in $headers
```

---

### http_fetch_data

#### Description
Retrieves the complete response body from an HTTP connection and closes the connection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The HTTP connection file handle |

#### Return Values
- **string**: The response body content
- **FALSE**: On error

#### Inner Mechanisms
1. Calls `http_get_contents()` to retrieve response data up to `CMS_HTTP_LIMIT`
2. Closes the file handle
3. Cleans up any chunked encoding state

#### Usage Example
```php
$hfile = http_fopen("https://api.example.com/users", NULL, $headers);
if ($hfile !== FALSE) {
    $responseBody = http_fetch_data($hfile);
    $users = json_decode($responseBody, TRUE);
    print_r($users);
}
```

---

### http_chunked

#### Description
Manages chunked transfer encoding state for HTTP connections.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The HTTP connection file handle |
| `$set` | `boolean\|integer` | If TRUE, flags connection as chunked; if integer, sets remaining bytes; if FALSE, retrieves state |

#### Return Values
- **TRUE**: When setting chunked flag
- **integer**: Remaining bytes in current chunk (when retrieving)
- **FALSE**: When connection is not flagged as chunked
- **NULL**: When connection resource is invalid/closed

#### Inner Mechanisms
1. Uses a static array to track chunked encoding state per connection
2. Identifies connections by their integer resource ID
3. Validates that the resource hasn't been closed/replaced
4. Stores and retrieves remaining byte counts for chunk processing

#### Usage Example
```php
// This function is used internally by http_fopen and http_get_contents
// to manage chunked transfer encoding
$hfile = http_fopen("https://example.com/chunked", NULL, $headers);
// Chunked encoding is automatically handled
$response = http_fetch_data($hfile);
```

---

### http_get_contents

#### Description
Reads response data from an HTTP connection, handling both regular and chunked transfer encoding.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The HTTP connection file handle |
| `$length` | `integer\|NULL` | Maximum bytes to read; NULL for unlimited (up to CMS_HTTP_LIMIT) |

#### Return Values
- **string**: The response data
- **FALSE**: On error or timeout

#### Inner Mechanisms
1. Checks for chunked encoding state via `http_chunked()`
2. For chunked responses:
   - Reads chunk size in hexadecimal
   - Reads exactly that many bytes
   - Skips trailing CRLF after each chunk
3. For regular responses:
   - Reads data in chunks of `CMS_HTTP_SIZE_CHUNK`
4. Implements both total and per-chunk timeouts
5. Updates remaining chunk state for subsequent calls

#### Usage Example
```php
$hfile = http_fopen("https://api.example.com/large-file", NULL, $headers);
if ($hfile !== FALSE) {
    // Read up to 10KB
    $data = http_get_contents($hfile, 10240);
    file_put_contents("download.bin", $data);
}
```

---

### http_header

#### Description
Performs an HTTP request and returns only the response headers.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The target URL |

#### Return Values
- **array**: Parsed response headers on success
- **FALSE**: On failure

#### Inner Mechanisms
1. Calls `http_fopen()` to establish connection and send request
2. Closes the connection immediately
3. Returns the parsed headers array

#### Usage Example
```php
// Check if a remote file exists without downloading it
$headers = http_header("https://example.com/file.zip");
if ($headers !== FALSE && isset($headers["#status"]) && $headers["#status"] == 200) {
    echo "File exists and is accessible";
} else {
    echo "File not found or inaccessible";
}
```

---

### http_post

#### Description
Sends an HTTP POST request with form data and returns the response body.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The target URL |
| `$data` | `array` | POST data array (can include file uploads) |
| `&$header` | `array\|NULL` | Reference variable to store parsed response headers |

#### Return Values
- **string**: The response body on success
- **FALSE**: On failure

#### Inner Mechanisms
1. Validates that `$data` is an array
2. Calls `http_fopen()` with POST data
3. Retrieves response body using `http_fetch_data()`
4. Stores response headers in the reference variable

#### Usage Example
```php
// Submit form data
$postData = [
    "username" => "john_doe",
    "password" => "secret123"
];
$response = http_post("https://api.example.com/login", $postData, $headers);
if ($response !== FALSE) {
    $result = json_decode($response, TRUE);
    if ($result["success"]) {
        echo "Login successful";
    }
}

// File upload
$postData = [
    "document" => [
        "data" => base64_encode(file_get_contents("/path/to/file.pdf")),
        "filename" => "document.pdf",
        "type" => "application/pdf"
    ],
    "description" => "My document"
];
$response = http_post("https://api.example.com/upload", $postData, $headers);
```

---

### http_build_post

#### Description
Constructs multipart/form-data body for HTTP POST requests, including file uploads.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$data` | `array` | POST data array with scalar values and/or file arrays |

#### Return Values
- **array**: Contains two elements:
  - `[0]` (string): HTTP headers (Content-Type and Content-Length)
  - `[1]` (array): Body parts as array of strings

#### Inner Mechanisms
1. Uses a recursive iterator to traverse nested arrays
2. Generates a unique boundary string for multipart separation
3. For file uploads (arrays with "data" key):
   - Sets appropriate Content-Disposition with filename
   - Determines MIME type from filename or explicit "type" field
   - Base64 encodes file data
4. For scalar values:
   - Sets simple Content-Disposition
5. Calculates total content length
6. Returns headers and body parts separately

#### Usage Example
```php
// This function is called internally by http_fopen for POST requests
// but can be used directly for custom implementations
$postData = [
    "title" => "My Article",
    "tags" => ["php", "http", "web"],
    "attachment" => [
        "data" => base64_encode(file_get_contents("image.jpg")),
        "filename" => "photo.jpg",
        "type" => "image/jpeg"
    ]
];
list($headers, $bodyParts) = http_build_post($postData);
// $headers contains Content-Type and Content-Length
// $bodyParts contains the multipart body segments
```


<!-- HASH:12fea0329bce89a34127c2cebc23eec3 -->
