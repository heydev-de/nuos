# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.smtp.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.smtp.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## SMTP Email Handling

This file provides SMTP email functionality for the PWNC Web Platform. It includes:

1. A standalone function `smtp_send()` for quick email dispatch
2. A full-featured `smtp` class for low-level SMTP operations

The implementation supports:
- Plaintext and HTML emails
- Multiple authentication methods (CRAM-MD5, LOGIN, PLAIN, ANONYMOUS)
- TLS encryption
- PHP's native `mail()` function as fallback
- MIME message construction

---

## Function: `smtp_send()`

Convenience wrapper for sending emails without manual SMTP connection handling.

### Parameters

| Name       | Type    | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `$to`      | string  | Recipient email address(es)                                                 |
| `$subject` | string  | Email subject                                                               |
| `$body`    | string  | Email body content                                                          |
| `$html`    | bool    | Whether the body contains HTML (default: `FALSE`)                          |
| `$reply_to`| string\|`NULL`\|`FALSE` | Reply-to address. `NULL` uses system default, `FALSE` omits the header. |

### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| bool    | `TRUE` on successful delivery, `FALSE` on failure                           |

### Inner Mechanisms

1. Loads the MIME library for message construction
2. Creates multipart/alternative MIME structure for HTML emails
3. Falls back to plaintext if HTML is disabled
4. Initializes SMTP connection using system configuration
5. Handles reply-to address resolution
6. Delegates actual delivery to the `smtp` class

### Usage Context

Use this function for one-off email sending when you don't need fine-grained control over the SMTP connection.

### Example

```php
// Send a simple plaintext email
smtp_send(
    "user@example.com",
    "Welcome to PWNC",
    "Your account has been created successfully."
);

// Send an HTML email with custom reply-to
smtp_send(
    "user@example.com",
    "Your Weekly Digest",
    "<h1>Latest Updates</h1><p>Here's what's new this week...</p>",
    TRUE,
    "noreply@example.com"
);
```

---

## Class: `smtp`

Low-level SMTP client with support for multiple authentication methods and encryption.

### Properties

| Name         | Default | Description                                      |
|--------------|---------|--------------------------------------------------|
| `$hfile`     | `NULL`  | File handle for the SMTP socket connection       |
| `$enabled`   | `NULL`  | Whether the SMTP connection is active            |
| `$mail`      | `NULL`  | Whether to use PHP's `mail()` function           |
| `$username`  | `NULL`  | SMTP username                                    |
| `$password`  | `NULL`  | SMTP password                                    |
| `$response`  | `NULL`  | Last SMTP server response                        |

---

### Method: `__construct()`

Initializes an SMTP connection using system configuration or provided credentials.

#### Parameters

| Name         | Type   | Description                                      |
|--------------|--------|--------------------------------------------------|
| `$host`      | string | SMTP server host (optional)                      |
| `$username`  | string | SMTP username (optional)                         |
| `$password`  | string | SMTP password (optional)                         |

#### Return Values

None (constructor)

#### Inner Mechanisms

1. Checks if PHP's `mail()` function should be used
2. Falls back to system configuration if no host is provided
3. Validates host format (with optional port)
4. Establishes socket connection
5. Sets timeout and initiates handshake
6. Attempts authentication

#### Usage Context

Use when you need persistent SMTP connections or advanced control over email delivery.

#### Example

```php
// Initialize using system configuration
$smtp = new smtp();

// Initialize with custom credentials
$smtp = new smtp(
    "smtp.example.com:587",
    "username",
    "password"
);
```

---

### Method: `authenticate()`

Handles SMTP authentication using the best available method.

#### Parameters

None

#### Return Values

| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `TRUE` if authentication succeeded, `FALSE` otherwise |

#### Inner Mechanisms

1. Attempts EHLO/HELO handshake
2. Negotiates STARTTLS if available
3. Tries authentication methods in order:
   - ANONYMOUS
   - CRAM-MD5
   - LOGIN
   - PLAIN

#### Usage Context

Called automatically during connection initialization. Rarely needs manual invocation.

---

### Method: `send()`

Sends a MIME message via SMTP.

#### Parameters

| Name         | Type    | Description                                      |
|--------------|---------|--------------------------------------------------|
| `&$mime`     | `mime`  | MIME message object (passed by reference)        |
| `$from`      | string  | Sender address (optional)                        |
| `$to`        | string  | Recipient address(es) (optional)                 |
| `$cc`        | string  | CC address(es) (optional)                        |
| `$bcc`       | string  | BCC address(es) (optional)                       |
| `$reply_to`  | string  | Reply-to address (optional)                      |

#### Return Values

| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `TRUE` on successful delivery, `FALSE` on failure |

#### Inner Mechanisms

1. Validates connection state
2. Extracts sender/recipient information
3. Handles PHP `mail()` fallback if configured
4. For SMTP:
   - Sets sender
   - Sets recipients
   - Builds and sends message data
   - Handles dot-stuffing

#### Usage Context

Use when you need to send pre-constructed MIME messages with custom headers.

#### Example

```php
$smtp = new smtp();
$mime = new mime();
$mime->add_text("Test Subject", "This is a test message");

if ($smtp->send($mime, "sender@example.com", "recipient@example.com")) {
    echo "Email sent successfully!";
} else {
    echo "Error: " . $smtp->response;
}
```

---

### Method: `receive_line()`

Reads a line from the SMTP server.

#### Parameters

None

#### Return Values

| Type    | Description                                      |
|---------|--------------------------------------------------|
| array\|`FALSE` | Array with `[code, message]` or `FALSE` on failure |

#### Inner Mechanisms

1. Reads data from the socket
2. Handles multi-line responses
3. Parses SMTP response codes
4. Stores last response in `$this->response`

#### Usage Context

Internal method used for SMTP communication. Rarely needs manual invocation.

---

### Method: `send_line()`

Sends a command to the SMTP server and waits for response.

#### Parameters

| Name       | Type   | Description                                      |
|------------|--------|--------------------------------------------------|
| `$code`    | int    | Expected response code                           |
| `$message` | string | Command to send                                  |

#### Return Values

| Type    | Description                                      |
|---------|--------------------------------------------------|
| string\|bool | Response message if available, `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms

1. Sends the command
2. Waits for response
3. Validates response code
4. Returns response message if available

#### Usage Context

Internal method used for SMTP communication. Rarely needs manual invocation.

---

### Method: `quit()`

Closes the SMTP connection.

#### Parameters

None

#### Return Values

| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | Always returns `TRUE`                            |

#### Inner Mechanisms

1. Sends QUIT command
2. Closes socket connection
3. Updates connection state

#### Usage Context

Should be called when done with the SMTP connection to ensure proper cleanup.

#### Example

```php
$smtp = new smtp();
$mime = new mime();
$mime->add_text("Test", "Message");
$smtp->send($mime, "from@example.com", "to@example.com");
$smtp->quit();
```


<!-- HASH:20e3f674e397871a7c33765c63d48872 -->
