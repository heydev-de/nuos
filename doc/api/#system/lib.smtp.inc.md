# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.smtp.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.smtp.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## smtp

The `smtp` class provides a lightweight SMTP client implementation for sending email messages within the PWNC Web Platform. It supports multiple authentication mechanisms (ANONYMOUS, CRAM-MD5, LOGIN, PLAIN), STARTTLS encryption, and falls back to PHP's native `mail()` function when configured. The class is typically used internally by the `smtp_send()` helper function but can also be instantiated directly for advanced use cases.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$hfile` | resource\|NULL | NULL | File handle for the SMTP socket connection |
| `$enabled` | bool\|NULL | NULL | Whether the SMTP connection is active and usable |
| `$mail` | bool\|NULL | NULL | Whether to use PHP's `mail()` function instead of SMTP |
| `$username` | string\|NULL | NULL | SMTP authentication username |
| `$password` | string\|NULL | NULL | SMTP authentication password |
| `$response` | string\|NULL | NULL | Last SMTP server response message |

### __construct

Initializes the SMTP client. If no host is provided, it reads configuration from the system settings. Attempts to establish a socket connection and authenticate.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$host` | string\|NULL | NULL | SMTP server hostname (with optional port as `host:port`) |
| `$username` | string\|NULL | NULL | SMTP authentication username |
| `$password` | string\|NULL | NULL | SMTP authentication password |

#### Return Values

No explicit return value. Sets `$this->enabled` to `FALSE` and populates `$this->response` on failure.

#### Inner Mechanisms

1. If `$host` is empty, loads system configuration for email method, SMTP host, username, and password.
2. If the email method is set to `"mail"`, enables PHP mail mode and returns early.
3. Parses the host string to extract hostname and port (defaulting to 25).
4. Opens a socket connection using `fsockopen()`.
5. Sets a 30-second timeout on the connection.
6. Reads the initial server greeting (expecting code 220).
7. Calls `authenticate()` to perform the SMTP handshake and login.

#### Usage Example

```php
$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
if ($smtp->enabled) {
    // Connection successful, ready to send
} else {
    echo "SMTP Error: " . $smtp->response;
}
```

### authenticate

Performs the SMTP authentication handshake including EHLO/HELO, optional STARTTLS, and SASL authentication.

#### Parameters

None.

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | Authentication succeeded or anonymous access is available |
| `FALSE` | Authentication failed |

#### Inner Mechanisms

1. Sends `EHLO` command (falls back to `HELO` if unsupported).
2. Checks for `STARTTLS` support and enables TLS encryption if available.
3. Re-sends `EHLO` after enabling encryption.
4. Parses supported `AUTH` methods from the server response.
5. Attempts authentication in order of preference:
   - **ANONYMOUS**: No credentials needed
   - **CRAM-MD5**: Challenge-response using HMAC-MD5
   - **LOGIN**: Base64-encoded username and password
   - **PLAIN**: Base64-encoded credentials in a single string

#### Usage Example

```php
$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
if ($smtp->enabled && $smtp->authenticate()) {
    echo "Authenticated successfully";
}
```

### send

Sends an email message via SMTP or PHP's `mail()` function.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$mime` | mime | — | MIME message object containing headers and body |
| `$from` | string\|NULL | NULL | Sender email address (overrides MIME From header) |
| `$to` | string\|NULL | NULL | Recipient email addresses |
| `$cc` | string\|NULL | NULL | Carbon copy recipients |
| `$bcc` | string\|NULL | NULL | Blind carbon copy recipients |
| `$reply_to` | string\|NULL | NULL | Reply-to email address |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | Message sent successfully |
| `FALSE` | Message sending failed (check `$this->response` for details) |

#### Inner Mechanisms

1. Validates that the connection is enabled.
2. Loads the MIME library if not already loaded.
3. Extracts the sender address from parameters or MIME headers.
4. If using PHP `mail()`:
   - Extracts recipient addresses
   - Builds the message
   - Handles Windows-specific line ending conversion
   - Splits headers and body
   - Calls `mail()` with `-f` flag for sender
5. If using SMTP:
   - Sends `MAIL FROM` command
   - Extracts and validates recipient addresses
   - Sends `RCPT TO` for each recipient
   - Removes BCC headers
   - Builds the message with proper line endings
   - Sends `DATA` command
   - Writes the message content
   - Sends end-of-message marker (`.`)

#### Usage Example

```php
$mime = new mime();
$mime->add_text("Test Subject", "Hello, this is a test email.");

$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
if ($smtp->send($mime, "sender@example.com", "recipient@example.com")) {
    echo "Email sent!";
} else {
    echo "Error: " . $smtp->response;
}
$smtp->quit();
```

### receive_line

Reads a single line from the SMTP server and parses the response code and message.

#### Parameters

None.

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Array with two elements: `[code, message]` where code is the 3-digit SMTP response code and message is the response text |
| `FALSE` | If the connection is disabled, using mail mode, or reading fails |

#### Inner Mechanisms

1. Reads lines from the SMTP socket using `fgets()`.
2. Matches each line against the SMTP response pattern: `^([0-9]{3})(-| )(.*)\r\n$`
3. Accumulates multi-line responses (indicated by `-` after the code).
4. Returns the final code and message when a line with a space after the code is received.

#### Usage Example

```php
$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
$response = $smtp->receive_line();
if ($response !== FALSE) {
    echo "Server response: " . $response[0] . " " . $response[1];
}
```

### send_line

Sends a command to the SMTP server and waits for the expected response code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$code` | int | Expected 3-digit SMTP response code |
| `$message` | string | Command string to send to the server |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | Response received with expected code and non-empty message |
| `string` | Response message text (when expected code matches) |
| `FALSE` | If connection is disabled, using mail mode, or response code doesn't match |

#### Inner Mechanisms

1. Writes the command to the SMTP socket followed by `\r\n`.
2. Calls `receive_line()` to read the server response.
3. Compares the response code with the expected code.
4. Returns the response message if codes match, or `FALSE` otherwise.

#### Usage Example

```php
$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
if ($smtp->send_line(250, "NOOP") !== FALSE) {
    echo "Server accepted NOOP command";
}
```

### quit

Closes the SMTP connection gracefully by sending the `QUIT` command.

#### Parameters

None.

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | Always returns `TRUE` |

#### Inner Mechanisms

1. If using PHP `mail()` mode, returns `TRUE` immediately.
2. If the connection is enabled:
   - Sets `$this->enabled` to `FALSE`
   - Sends `QUIT` command
   - Closes the socket file handle
3. Returns `TRUE` in all cases.

#### Usage Example

```php
$smtp = new smtp("smtp.example.com:587", "user@example.com", "password123");
// ... send emails ...
$smtp->quit(); // Always call quit() when done
```

## smtp_send

Helper function that creates a MIME message and sends it using the SMTP class. Supports both HTML and plain text emails.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$to` | string | — | Recipient email address(es) |
| `$subject` | string | — | Email subject line |
| `$body` | string | — | Email body content |
| `$html` | bool | FALSE | Whether `$body` contains HTML content |
| `$reply_to` | string\|NULL | NULL | Reply-to address (NULL = use system default, FALSE = no reply-to) |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | Email sent successfully |
| `FALSE` | Email sending failed |

#### Inner Mechanisms

1. Loads the MIME library.
2. Creates a new MIME message:
   - For HTML emails: Creates a multipart/alternative message with both HTML and plain text versions
   - For plain text emails: Creates a simple text message
3. Instantiates the SMTP class.
4. Determines the reply-to address:
   - `NULL`: Uses system default from configuration
   - `FALSE`: No reply-to header
   - String: Uses the provided address
5. Sends the message using SMTP.
6. Closes the SMTP connection.
7. Returns the result.

#### Usage Example

```php
// Send a simple text email
smtp_send(
    "recipient@example.com",
    "Welcome!",
    "Thank you for registering on our site."
);

// Send an HTML email with custom reply-to
smtp_send(
    "recipient@example.com",
    "Newsletter",
    "<h1>Latest News</h1><p>Check out our new features!</p>",
    TRUE,
    "news@example.com"
);
```


<!-- HASH:20e3f674e397871a7c33765c63d48872 -->
