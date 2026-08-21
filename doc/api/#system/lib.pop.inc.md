# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.pop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.pop.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## POP Class

The `pop` class provides a lightweight, secure, and efficient implementation for interacting with **POP3 (Post Office Protocol version 3)** mailboxes. It supports both plaintext and TLS-encrypted connections, authentication via `APOP` (CRAM-MD5) or plain `USER`/`PASS`, and full MIME message parsing. The class is designed for high performance and minimal dependencies, adhering to PWNC's no-bloat philosophy.

### Key Features
- **Secure Transport**: Automatically upgrades to TLS if available.
- **Authentication**: Supports both `APOP` (preferred) and plain login.
- **Message Handling**: Retrieves statistics, lists, headers, and full MIME messages.
- **Boundary Detection**: Correctly parses multipart MIME messages with nested boundaries.
- **Error Handling**: Provides clear error states and responses.

---

### Constants

| Name                     | Value | Description                                                                 |
|--------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_POP_STATUS_ERROR`   | `0`   | Indicates an error occurred during POP3 communication.                     |
| `CMS_POP_STATUS_OK`      | `1`   | Indicates a successful POP3 response.                                      |
| `CMS_POP_STATUS_BOUNDARY`| `2`   | Indicates the start of a MIME boundary.                                    |
| `CMS_POP_STATUS_BOUNDARY_END` | `3` | Indicates the end of a MIME boundary.                                      |
| `CMS_POP_STATUS_MESSAGE_END` | `4` | Indicates the end of a message.                                            |

---

### Properties

| Name       | Default | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `$hfile`   | `NULL`  | File handle for the POP3 socket connection.                                 |
| `$response`| `NULL`  | Last raw response from the POP3 server.                                     |
| `$enabled` | `NULL`  | Boolean indicating if the connection is active and authenticated.           |
| `$error`   | `NULL`  | Boolean indicating if an error occurred during the last operation.          |

---

## Constructor: `__construct`

### Purpose
Establishes a connection to a POP3 server, negotiates TLS (if available), and authenticates the user. Sets the `$enabled` flag on success.

### Parameters

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$host`    | `string` | **Required.** POP3 server hostname or IP address.                           |
| `$username`| `string` | **Required.** Username for authentication.                                 |
| `$password`| `string` | **Required.** Password for authentication.                                 |

### Return Value
- **`void`**: No explicit return. Sets `$this->enabled` to `TRUE` on success, `FALSE` on failure.

### Inner Mechanisms
1. **Connection**: Opens a socket to the POP3 server on port 110.
2. **TLS Upgrade**: If OpenSSL is available, issues `STLS` command and enables TLS.
3. **Authentication**: Attempts `APOP` first (using MD5 challenge-response), falls back to plain `USER`/`PASS`.
4. **Error Handling**: Sets `$this->error` and closes the socket on failure.

### Usage Example
```php
$pop = new \cms\pop("mail.example.com", "user@example.com", "password123");
if (!$pop->enabled) {
    die("POP3 connection failed: " . $pop->response);
}
```

---

## Method: `get_statistics`

### Purpose
Retrieves the total number of messages and their combined size (in bytes) in the mailbox.

### Parameters
- **None**

### Return Value
- **`array|FALSE`**:
  - On success: `["count" => int, "size" => int]`
  - On failure: `FALSE`

### Inner Mechanisms
- Issues the `STAT` command to the POP3 server.
- Parses the response (e.g., `+OK 5 10240`) into an associative array.

### Usage Example
```php
$stats = $pop->get_statistics();
if ($stats) {
    echo "Messages: {$stats['count']}, Total Size: {$stats['size']} bytes";
}
```

---

## Method: `get_list`

### Purpose
Retrieves a list of all messages in the mailbox, indexed by message number and their individual sizes.

### Parameters
- **None**

### Return Value
- **`array|FALSE`**:
  - On success: `[1 => 1024, 2 => 2048, ...]` (message number => size in bytes)
  - On failure: `FALSE`

### Inner Mechanisms
- Issues the `LIST` command.
- Reads multi-line responses until termination (`.`).
- Parses each line into message number and size.

### Usage Example
```php
$list = $pop->get_list();
if ($list) {
    foreach ($list as $id => $size) {
        echo "Message #$id: $size bytes\n";
    }
}
```

---

## Method: `get_unique_id_list`

### Purpose
Retrieves a list of all messages with their unique identifiers (UIDLs), which persist across sessions.

### Parameters
- **None**

### Return Value
- **`array|FALSE`**:
  - On success: `[1 => "UID1", 2 => "UID2", ...]`
  - On failure: `FALSE`

### Inner Mechanisms
- Issues the `UIDL` command.
- Parses multi-line responses into message number and UID.

### Usage Example
```php
$uids = $pop->get_unique_id_list();
if ($uids) {
    echo "Message UIDs: " . print_r($uids, TRUE);
}
```

---

## Method: `get_header`

### Purpose
Retrieves the headers of a specific message (without the body).

### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

### Return Value
- **`array|FALSE`**:
  - On success: Associative array of headers (e.g., `["subject" => "Hello", "from" => "user@example.com"]`).
  - On failure: `FALSE`

### Inner Mechanisms
- Issues the `TOP $index 0` command to fetch headers only.
- Parses headers into an associative array, unfolding multi-line headers (e.g., `Subject: ...`).

### Usage Example
```php
$headers = $pop->get_header(1);
if ($headers) {
    echo "Subject: " . ($headers["subject"] ?? "No Subject");
}
```

---

## Method: `get_message`

### Purpose
Retrieves a full MIME message, including all parts, headers, and decoded bodies. Returns a structured `data` object.

### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

### Return Value
- **`data|FALSE`**:
  - On success: A `data` object with nested structure representing the MIME message.
  - On failure: `FALSE`

### Inner Mechanisms
1. **MIME Parsing**: Uses the `mime` library to decode headers and bodies.
2. **Boundary Handling**: Detects and processes multipart boundaries (`--boundary`).
3. **Body Decoding**: Decodes `base64`, `quoted-printable`, etc., based on `Content-Transfer-Encoding`.
4. **Charset Conversion**: Converts body content to UTF-8 if a charset is specified.
5. **Nested Containers**: Handles nested `multipart` and `message` MIME types.

### Usage Example
```php
$message = $pop->get_message(1);
if ($message) {
    $subject = $message->get(1, "subject");
    $body = $message->get(2, "#body"); // First part's body
    echo "Subject: $subject\nBody: $body";
}
```

---

## Method: `delete`

### Purpose
Marks a message for deletion from the mailbox. Deletion occurs on `QUIT`.

### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

### Return Value
- **`bool`**: `TRUE` on success, `FALSE` on failure.

### Inner Mechanisms
- Issues the `DELE $index` command.

### Usage Example
```php
if ($pop->delete(1)) {
    echo "Message 1 marked for deletion.";
}
```

---

## Method: `quit`

### Purpose
Closes the POP3 connection, committing deletions if no errors occurred.

### Parameters
- **None**

### Return Value
- **`bool`**: `TRUE` on success, `FALSE` on failure.

### Inner Mechanisms
- Issues the `QUIT` command if no errors occurred.
- Closes the socket and sets `$enabled` to `FALSE`.

### Usage Example
```php
$pop->quit();
```

---

## Private Method: `execute`

### Purpose
Sends a command to the POP3 server and waits for a response.

### Parameters

| Name      | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$command`| `string` | POP3 command (e.g., `STAT`, `RETR 1`). |

### Return Value
- **`bool`**: `TRUE` if the server responded with `+OK`, `FALSE` otherwise.

### Inner Mechanisms
- Writes the command to the socket.
- Calls `receive()` to read the response.

---

## Private Method: `receive`

### Purpose
Reads a single line from the POP3 server and processes it.

### Parameters

| Name       | Type     | Default | Description                     |
|------------|----------|---------|---------------------------------|
| `$boundary`| `string` | `NULL`  | MIME boundary to detect.        |

### Return Value
- **`int`**: One of the `CMS_POP_STATUS_*` constants.

### Inner Mechanisms
- Reads a line from the socket.
- Handles byte-stuffing (`.` escaping).
- Detects MIME boundaries if `$boundary` is provided.
- Sets `$this->error` on `-ERR` responses.

---

## Private Method: `receive_header`

### Purpose
Reads and parses MIME headers until an empty line (`\r\n`) is encountered.

### Parameters

| Name       | Type     | Default | Description                     |
|------------|----------|---------|---------------------------------|
| `$boundary`| `string` | `NULL`  | MIME boundary to detect.        |

### Return Value
- **`array|int`**:
  - On success: Associative array of headers.
  - On boundary/error: One of the `CMS_POP_STATUS_*` constants.

### Inner Mechanisms
- Calls `receive()` repeatedly until headers end.
- Unfolds multi-line headers (e.g., `Subject: ...`).
- Parses headers into an associative array (case-insensitive keys).


<!-- HASH:f1cfdcafff04cf1ee1d182c6483bbd91 -->
