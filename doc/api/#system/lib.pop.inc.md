# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.pop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.pop.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## pop

The `pop` class provides a lightweight, dependency-free implementation of a POP3 client in PHP. It enables connecting to a POP3 server, authenticating (via APOP or plain USER/PASS), retrieving mailbox statistics, listing messages, fetching headers and full MIME-decoded messages, and deleting messages.

It uses raw socket communication (`fsockopen`) and supports optional TLS encryption via `STLS`. The class relies on the `mime` library for parsing and decoding email content.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$hfile` | `NULL` | File handle to the POP3 connection |
| `$response` | `NULL` | Last server response line |
| `$enabled` | `NULL` | Whether the connection is active and authenticated |
| `$error` | `NULL` | Error flag set on communication failure |

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_POP_STATUS_ERROR` | 0 | Communication error occurred |
| `CMS_POP_STATUS_OK` | 1 | Normal data line received |
| `CMS_POP_STATUS_BOUNDARY` | 2 | MIME multipart boundary start |
| `CMS_POP_STATUS_BOUNDARY_END` | 3 | MIME multipart boundary end |
| `CMS_POP_STATUS_MESSAGE_END` | 4 | End of message marker (`.`) |

### __construct

```php
public function __construct(string $host, string $username, string $password)
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$host` | `string` | POP3 server hostname (required) |
| `$username` | `string` | Authentication username |
| `$password` | `string` | Authentication password |

#### Description

Establishes a connection to the specified POP3 server on port 110. If the OpenSSL extension is available, it attempts to upgrade the connection to TLS using `STLS`. Authentication is performed using either APOP (if the server supports it) or plain `USER`/`PASS` commands.

#### Usage Example

```php
$pop = new pop("mail.example.com", "user@example.com", "secret");
if ($pop->enabled) {
    echo "Connected successfully.";
} else {
    echo "Connection failed: " . $pop->response;
}
```

### get_statistics

```php
public function get_statistics(): array|false
```

#### Return Values

- **array**: On success, contains `count` (message count) and `size` (total mailbox size in bytes).
- **false**: If not enabled or command fails.

#### Description

Sends the `STAT` command to retrieve mailbox statistics.

#### Usage Example

```php
$stats = $pop->get_statistics();
if ($stats) {
    echo "Messages: {$stats['count']}, Size: {$stats['size']} bytes";
}
```

### get_list

```php
public function get_list(): array|null|false
```

#### Return Values

- **array**: Message number => size mapping.
- **null**: If no messages found.
- **false**: If not enabled or command fails.

#### Description

Retrieves a list of all messages with their sizes using the `LIST` command.

#### Usage Example

```php
$list = $pop->get_list();
if ($list) {
    foreach ($list as $msgNum => $size) {
        echo "Message $msgNum: $size bytes\n";
    }
}
```

### get_unique_id_list

```php
public function get_unique_id_list(): array|null|false
```

#### Return Values

- **array**: Message number => unique ID mapping.
- **null**: If no messages found.
- **false**: If not enabled or command fails.

#### Description

Retrieves unique identifiers for each message using the `UIDL` command.

#### Usage Example

```php
$ids = $pop->get_unique_id_list();
if ($ids) {
    foreach ($ids as $msgNum => $uid) {
        echo "Message $msgNum UID: $uid\n";
    }
}
```

### get_header

```php
public function get_header(int $index): array|false
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Message number (1-based) |

#### Return Values

- **array**: Associative array of header fields.
- **false**: If not enabled or command fails.

#### Description

Fetches only the headers of a message using the `TOP` command.

#### Usage Example

```php
$headers = $pop->get_header(1);
if ($headers) {
    echo "Subject: " . ($headers['subject'] ?? 'N/A') . "\n";
    echo "From: " . ($headers['from'] ?? 'N/A') . "\n";
}
```

### receive_header

```php
private function receive_header(?string $boundary = NULL): array|int
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$boundary` | `string\|NULL` | Optional MIME boundary string |

#### Return Values

- **array**: Parsed header fields.
- **int**: Status constant (`CMS_POP_STATUS_*`) on error or boundary detection.

#### Description

Internal method that reads and parses RFC 2822 headers from the socket stream, handling line folding and boundary markers.

### get_message

```php
public function get_message(int $index): object|false
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Message number (1-based) |

#### Return Values

- **object**: A `data` object containing parsed MIME structure.
- **false**: If not enabled, mime library missing, or command fails.

#### Description

Retrieves and fully parses a message including MIME multipart structures, content transfer encodings, and character sets. Requires the `mime` library.

#### Usage Example

```php
$message = $pop->get_message(1);
if ($message) {
    $subject = $message->get(1, "subject");
    $body = $message->get(1, "#body");
    echo "Subject: $subject\n";
    echo "Body: $body\n";
}
```

### delete

```php
public function delete(int $index): bool
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Message number to delete |

#### Return Values

- **bool**: `true` on success, `false` otherwise.

#### Description

Marks a message for deletion using the `DELE` command. The message is actually removed when the connection is closed with `QUIT`.

#### Usage Example

```php
if ($pop->delete(1)) {
    echo "Message marked for deletion.";
}
```

### execute

```php
private function execute(string $command): bool
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$command` | `string` | POP3 command to send |

#### Return Values

- **bool**: `true` if no error, `false` otherwise.

#### Description

Sends a command to the POP3 server and waits for a response. Sets `$this->error` on failure.

### receive

```php
private function receive(?string $boundary = NULL): int
```

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$boundary` | `string\|NULL` | Optional MIME boundary to detect |

#### Return Values

- **int**: One of the `CMS_POP_STATUS_*` constants.

#### Description

Reads a single line from the socket. Handles end-of-message markers, MIME boundaries, and byte-stuffing (leading dots).

### quit

```php
public function quit(): bool
```

#### Return Values

- **bool**: `true` on success, `false` if not enabled.

#### Description

Closes the POP3 connection gracefully by sending the `QUIT` command.

#### Usage Example

```php
$pop->quit();
echo "Connection closed.";
```


<!-- HASH:f1cfdcafff04cf1ee1d182c6483bbd91 -->
