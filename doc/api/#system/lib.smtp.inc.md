# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.smtp.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## SMTP Email Handling in NUOS

This file provides SMTP email functionality for the NUOS platform, including both a procedural interface (`smtp_send`) and an object-oriented class (`smtp`) for sending emails via SMTP or PHP's built-in `mail()` function. It supports plaintext and HTML emails, MIME encoding, authentication (CRAM-MD5, LOGIN, PLAIN), and TLS encryption.

---

## Functions

### `smtp_send`

Sends an email via SMTP or PHP's `mail()` function.

#### Parameters

| Name       | Type      | Description                                                                 |
|------------|-----------|-----------------------------------------------------------------------------|
| `$to`      | `string`  | Recipient email address(es). Comma-separated for multiple recipients.       |
| `$subject` | `string`  | Email subject.                                                              |
| `$body`    | `string`  | Email body content.                                                         |
| `$html`    | `bool`    | If `TRUE`, sends as HTML email with a plaintext fallback. Default: `FALSE`. |
| `$reply_to`| `string`  | Optional "Reply-To" email address.                                          |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the email was sent successfully, `FALSE` otherwise.               |

#### Inner Mechanisms

1. **MIME Handling**: Uses the `mime` class to construct the email. For HTML emails, it creates a multipart/alternative MIME structure with both HTML and plaintext versions.
2. **SMTP Connection**: Initializes the `smtp` class and delegates sending to its `send` method.
3. **Cleanup**: Ensures the SMTP connection is closed via `quit()`.

#### Usage Context

- **When to Use**: For sending emails from NUOS applications, especially when HTML formatting or SMTP authentication is required.
- **Typical Scenarios**:
  - User registration confirmation emails.
  - Password reset emails.
  - Notification emails (e.g., form submissions, system alerts).

---

## Class: `smtp`

Handles low-level SMTP communication, including connection management, authentication, and email transmission.

### Properties

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$hfile`     | `resource`| File handle for the SMTP socket connection.                                 |
| `$enabled`   | `bool`    | Indicates if the SMTP connection is active and usable.                      |
| `$mail`      | `bool`    | If `TRUE`, uses PHP's `mail()` function instead of direct SMTP.             |
| `$username`  | `string`  | SMTP username for authentication.                                           |
| `$password`  | `string`  | SMTP password for authentication.                                           |
| `$response`  | `string`  | Last SMTP server response (for error reporting).                            |

---

### `__construct`

Initializes the SMTP connection and performs authentication.

#### Parameters

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$host`      | `string`  | SMTP server hostname. If `NULL`, uses system configuration.                 |
| `$username`  | `string`  | SMTP username. If `NULL`, uses system configuration.                        |
| `$password`  | `string`  | SMTP password. If `NULL`, uses system configuration.                        |

#### Return Values

None (constructor).

#### Inner Mechanisms

1. **Configuration**: Falls back to system settings (from `system` class) if parameters are not provided.
2. **Mail Function Check**: Uses PHP's `mail()` function if configured (`email.method = "mail"`).
3. **Connection**: Opens a socket to the SMTP server on port 25.
4. **Authentication**: Calls `authenticate()` to handle SMTP authentication (CRAM-MD5, LOGIN, PLAIN) and TLS encryption if supported.

#### Usage Context

- **When to Use**: When direct control over SMTP communication is needed (e.g., custom email handling).
- **Typical Scenarios**:
  - Sending bulk emails.
  - Debugging SMTP issues.
  - Custom email workflows (e.g., queuing, retries).

---

### `authenticate`

Handles SMTP authentication and TLS encryption.

#### Parameters

None.

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if authentication succeeds, `FALSE` otherwise.                       |

#### Inner Mechanisms

1. **TLS Support**: Checks for `STARTTLS` support and enables encryption if available.
2. **Authentication Methods**:
   - **CRAM-MD5**: Uses HMAC-MD5 for secure authentication.
   - **LOGIN**: Sends username and password in base64-encoded form.
   - **PLAIN**: Sends credentials in a single base64-encoded string.
3. **Fallback**: Attempts methods in order of preference (CRAM-MD5 > LOGIN > PLAIN).

#### Usage Context

- **When to Use**: Internal use by the `smtp` class during connection setup.
- **Typical Scenarios**: Secure email transmission over untrusted networks.

---

### `send`

Sends an email via SMTP or PHP's `mail()` function.

#### Parameters

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `&$mime`     | `mime`    | MIME-encoded email object (from the `mime` class).                         |
| `$from`      | `string`  | Sender email address. Overrides MIME object's "From" header if provided.   |
| `$to`        | `string`  | Recipient email address(es). Overrides MIME object's "To" header if provided. |
| `$cc`        | `string`  | CC email address(es). Overrides MIME object's "CC" header if provided.     |
| `$bcc`       | `string`  | BCC email address(es). Overrides MIME object's "BCC" header if provided.   |
| `$reply_to`  | `string`  | "Reply-To" email address.                                                   |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the email was sent successfully, `FALSE` otherwise.               |

#### Inner Mechanisms

1. **Mail Function**: If `$mail` is `TRUE`, uses PHP's `mail()` function with headers and body extracted from the MIME object.
2. **SMTP Protocol**:
   - Sends `MAIL FROM`, `RCPT TO`, and `DATA` commands.
   - Escapes lines starting with `.` to prevent premature termination.
   - Handles recipient extraction and validation.
3. **Error Handling**: Sets `$response` with error details if sending fails.

#### Usage Context

- **When to Use**: For sending MIME-encoded emails with custom headers or recipients.
- **Typical Scenarios**:
  - Sending emails with attachments.
  - Overriding default headers (e.g., "From", "Reply-To").

---

### `receive_line`

Reads a line from the SMTP server and parses the response code.

#### Parameters

None.

#### Return Values

| Type       | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| `array`    | Associative array with keys `0` (response code) and `1` (response message). |
| `bool`     | `FALSE` if the connection is closed or an error occurs.                    |

#### Inner Mechanisms

1. **Line Reading**: Reads data from the socket in chunks of 1024 bytes.
2. **Response Parsing**: Extracts the 3-digit response code and message.
3. **Multiline Handling**: Continues reading until a line ending with a space (indicating the end of the response) is received.

#### Usage Context

- **When to Use**: Internal use for SMTP communication.
- **Typical Scenarios**: Reading server responses during authentication or email transmission.

---

### `send_line`

Sends a command to the SMTP server and verifies the response.

#### Parameters

| Name       | Type      | Description                                                                 |
|------------|-----------|-----------------------------------------------------------------------------|
| `$code`    | `int`     | Expected SMTP response code (e.g., `250` for success).                      |
| `$message` | `string`  | Command to send to the SMTP server (e.g., `EHLO example.com`).              |

#### Return Values

| Type       | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| `string`   | Response message if the expected code is received.                         |
| `bool`     | `TRUE` if the expected code is received (no message), `FALSE` otherwise.   |

#### Inner Mechanisms

1. **Command Sending**: Writes the command to the socket followed by `\r\n`.
2. **Response Verification**: Uses `receive_line()` to read the response and checks if the code matches the expected value.

#### Usage Context

- **When to Use**: Internal use for SMTP communication.
- **Typical Scenarios**: Sending SMTP commands (e.g., `EHLO`, `MAIL FROM`, `QUIT`).

---

### `quit`

Closes the SMTP connection gracefully.

#### Parameters

None.

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | Always returns `TRUE`.                                                      |

#### Inner Mechanisms

1. **Command Sending**: Sends the `QUIT` command to the SMTP server.
2. **Cleanup**: Closes the socket and sets `$enabled` to `FALSE`.

#### Usage Context

- **When to Use**: To terminate an SMTP session cleanly.
- **Typical Scenarios**: After sending an email or when aborting a connection.


<!-- HASH:6462c2971b34dd00b0980e1df641137c -->
