# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.pop.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## POP Class

The `pop` class provides a lightweight implementation for interacting with POP3 (Post Office Protocol version 3) mail servers. It supports basic operations such as retrieving email statistics, listing messages, fetching headers and bodies, and deleting messages. The class also handles TLS encryption if the OpenSSL extension is available.

---

### Properties

| Name       | Value/Default | Description                                                                 |
|------------|---------------|-----------------------------------------------------------------------------|
| `hfile`    | `NULL`        | File handle for the socket connection to the POP3 server.                   |
| `response` | `NULL`        | Stores the last response received from the POP3 server.                     |
| `enabled`  | `NULL`        | Boolean indicating whether the connection to the POP3 server is active.     |

---

### `__construct($host, $username, $password)`

#### Purpose
Establishes a connection to a POP3 server, authenticates the user, and initializes the session. Supports both plaintext and APOP authentication methods, as well as TLS encryption if available.

#### Parameters

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$host`    | `string` | The hostname or IP address of the POP3 server.                              |
| `$username`| `string` | The username for authentication.                                            |
| `$password`| `string` | The password for authentication.                                            |

#### Return Values
- **`void`**: No explicit return value. Sets `enabled` to `TRUE` on success, `FALSE` otherwise.

#### Inner Mechanisms
1. Attempts to open a socket connection to the POP3 server on port 110.
2. Checks for a successful connection response (`+OK`).
3. If OpenSSL is available, attempts to upgrade the connection to TLS using the `STLS` command.
4. Attempts authentication using either APOP (if the server provides a timestamp) or plaintext `USER`/`PASS` commands.
5. On failure, closes the connection and sets `enabled` to `FALSE`.

#### Usage Context
- Used to initialize a POP3 session before performing any email operations.
- Example:
  ```php
  $pop = new \cms\pop("mail.example.com", "user", "password");
  if (!$pop->enabled) {
      die("Connection failed: " . $pop->response);
  }
  ```

---

### `get_statistics()`

#### Purpose
Retrieves the total number of messages and their combined size in bytes from the POP3 server.

#### Parameters
- **None**

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array with keys `count` (number of messages) and `size` (total size in bytes).
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Checks if the connection is enabled.
2. Sends the `STAT` command to the server.
3. Parses the response into an array.

#### Usage Context
- Used to quickly assess the mailbox status before fetching messages.
- Example:
  ```php
  $stats = $pop->get_statistics();
  if ($stats) {
      echo "Messages: {$stats['count']}, Size: {$stats['size']} bytes";
  }
  ```

---

### `get_list()`

#### Purpose
Retrieves a list of all messages in the mailbox, including their unique identifiers and sizes.

#### Parameters
- **None**

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array where keys are message indices and values are their sizes in bytes.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Checks if the connection is enabled.
2. Sends the `LIST` command to the server.
3. Parses the multiline response into an associative array.

#### Usage Context
- Used to enumerate messages for selective retrieval or deletion.
- Example:
  ```php
  $list = $pop->get_list();
  if ($list) {
      foreach ($list as $index => $size) {
          echo "Message $index: $size bytes";
      }
  }
  ```

---

### `get_unique_id_list()`

#### Purpose
Retrieves a list of all messages in the mailbox with their unique identifiers (UIDLs).

#### Parameters
- **None**

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array where keys are message indices and values are their UIDLs.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Checks if the connection is enabled.
2. Sends the `UIDL` command to the server.
3. Parses the multiline response into an associative array.

#### Usage Context
- Used to track messages across sessions (UIDLs persist even if message indices change).
- Example:
  ```php
  $uidl_list = $pop->get_unique_id_list();
  if ($uidl_list) {
      foreach ($uidl_list as $index => $uidl) {
          echo "Message $index: UIDL $uidl";
      }
  }
  ```

---

### `get_header($index)`

#### Purpose
Retrieves the headers of a specific message by its index.

#### Parameters

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `int`    | The index of the message to fetch. |

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array of headers (e.g., `subject`, `from`, `to`).
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Checks if the connection is enabled.
2. Sends the `TOP $index 0` command to fetch headers only.
3. Parses the response using `receive_header()`.
4. Ignores the body by reading until the end of the response.

#### Usage Context
- Used to inspect message metadata without downloading the entire message.
- Example:
  ```php
  $headers = $pop->get_header(1);
  if ($headers) {
      echo "Subject: " . ($headers['subject'] ?? 'No subject');
  }
  ```

---

### `receive_header()`

#### Purpose
Parses the raw header data received from the POP3 server into a structured associative array. Handles folded headers and MIME-encoded values (e.g., `=?UTF-8?Q?...?=`).

#### Parameters
- **None**

#### Return Values
- **`array|NULL`**:
  - On success: Associative array of headers with lowercase keys.
  - On failure: `NULL`.

#### Inner Mechanisms
1. Reads lines from the server until an empty line (`\r\n`) is encountered.
2. Merges folded headers (lines starting with whitespace) into the previous header.
3. Decodes MIME-encoded headers (e.g., `base64` or `quoted-printable`).

#### Usage Context
- Internal method used by `get_header()` and `get_message()` to parse headers.
- Example:
  ```php
  $headers = $pop->receive_header();
  ```

---

### `get_message($index)`

#### Purpose
Retrieves a complete message (headers and body) by its index, including support for multipart MIME messages (e.g., attachments).

#### Parameters

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `int`    | The index of the message to fetch. |

#### Return Values
- **`data|FALSE`**:
  - On success: A `data` object containing the parsed message structure.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Checks if the connection is enabled and loads the `mime` library.
2. Sends the `RETR $index` command to fetch the message.
3. Parses the message into a hierarchical `data` object, handling:
   - Multipart boundaries.
   - Nested messages.
   - Content transfer encodings (e.g., `base64`, `quoted-printable`).
4. Calculates the total size of the message.

#### Usage Context
- Used to fetch entire messages, including attachments, for processing or display.
- Example:
  ```php
  $message = $pop->get_message(1);
  if ($message) {
      echo "Subject: " . $message->get(1, "subject");
      echo "Body: " . $message->get(1, "#body");
  }
  ```

---

### `delete($index)`

#### Purpose
Marks a message for deletion from the POP3 server. Messages are not actually deleted until the session is closed or the `QUIT` command is sent.

#### Parameters

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `int`    | The index of the message to delete. |

#### Return Values
- **`bool`**:
  - `TRUE` on success.
  - `FALSE` on failure.

#### Inner Mechanisms
1. Checks if the connection is enabled.
2. Sends the `DELE $index` command to the server.

#### Usage Context
- Used to remove messages from the server after processing.
- Example:
  ```php
  if ($pop->delete(1)) {
      echo "Message marked for deletion";
  }
  ```

---

### `execute($command)`

#### Purpose
Sends a command to the POP3 server and reads the response.

#### Parameters

| Name      | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$command`| `string` | The POP3 command to execute (e.g., `STAT`, `RETR`). |

#### Return Values
- **`bool`**:
  - `TRUE` if the command succeeded.
  - `FALSE` if the command failed or the connection is closed.

#### Inner Mechanisms
1. Writes the command to the socket.
2. Reads the response using `receive()`.

#### Usage Context
- Internal method used by other methods to interact with the POP3 server.
- Example:
  ```php
  $pop->execute("NOOP");
  ```

---

### `receive($boundary = NULL)`

#### Purpose
Reads a single line from the POP3 server and checks for termination conditions (e.g., end of message, error).

#### Parameters

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$boundary`| `string` | Optional boundary string for multipart messages. Stops reading if matched. |

#### Return Values
- **`bool`**:
  - `TRUE` if the response is valid.
  - `FALSE` if the response indicates an error or termination.

#### Inner Mechanisms
1. Reads a line from the socket.
2. Checks for:
   - End of message (`.` on a line by itself).
   - Multipart boundary (if provided).
   - Error response (`-ERR`).

#### Usage Context
- Internal method used by `execute()` and other methods to read server responses.
- Example:
  ```php
  while ($pop->receive($boundary)) {
      echo $pop->response;
  }
  ```

---

### `quit()`

#### Purpose
Closes the POP3 session, committing any deletions and releasing resources.

#### Parameters
- **None**

#### Return Values
- **`bool`**:
  - `TRUE` if the session was closed successfully.
  - `FALSE` if the connection was not enabled.

#### Inner Mechanisms
1. Sends the `QUIT` command to the server.
2. Closes the socket connection.
3. Sets `enabled` to `FALSE`.

#### Usage Context
- Used to cleanly terminate a POP3 session.
- Example:
  ```php
  $pop->quit();
  ```


<!-- HASH:42d673a287dfb4fba86fc6dbaf3df099 -->
