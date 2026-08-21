# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.mailbox.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.mailbox.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Mailbox Module

The `desktop.mailbox.inc` file provides a comprehensive email client interface within the PWNC Web Platform. It handles email retrieval, composition, management, and spam filtering through a desktop-like interface. The module integrates with POP3/SMTP protocols and supports MIME message parsing, draft management, and container-based organization of emails.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILBOX_PROPERTY_DATE` | `1` | Property identifier for message date. |
| `CMS_MAILBOX_PROPERTY_FROM` | `2` | Property identifier for sender address. |
| `CMS_MAILBOX_PROPERTY_SUBJECT` | `3` | Property identifier for message subject. |
| `CMS_MAILBOX_PROPERTY_STATUS` | `4` | Property identifier for message status flags. |
| `CMS_MAILBOX_PROPERTY_SPAM_INDICATOR` | `5` | Property identifier for spam probability. |
| `CMS_MAILBOX_PROPERTY_SIZE` | `6` | Property identifier for message size in bytes. |
| `CMS_MAILBOX_STATUS_NONE` | `0` | No special status. |
| `CMS_MAILBOX_STATUS_IMPORTANCE_HIGH` | `1` | Message marked as high importance. |
| `CMS_MAILBOX_STATUS_READ` | `2` | Message has been read. |
| `CMS_MAILBOX_STATUS_SENT` | `4` | Message has been sent. |
| `CMS_MAILBOX_STATUS_BAD` | `8` | Message marked as spam. |
| `CMS_MAILBOX_STATUS_GOOD` | `16` | Message marked as non-spam. |
| `CMS_MAILBOX_STATUS_DRAFT` | `32` | Message is a draft. |
| `CMS_MAILBOX_STATUS_ATTACHMENT` | `64` | Message contains attachments. |
| `CMS_MAILBOX_DRAFT_TYPE_MESSAGE` | `1` | Draft type: new message. |
| `CMS_MAILBOX_DRAFT_TYPE_REPLY` | `2` | Draft type: reply. |
| `CMS_MAILBOX_DRAFT_TYPE_RELAY` | `3` | Draft type: forward/relay. |
| `CMS_MAILBOX_DRAFT_OPTION_TEXT` | `1` | Draft option: text body. |
| `CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT` | `2` | Draft option: attachment. |

---

### Functions

---

#### `mailbox_text(&$data)`

**Purpose:**
Extracts and concatenates plain text content from a MIME message structure for spam analysis or preview.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Reference to a `data` object containing MIME message parts. |

**Return Value:**
`string` – Concatenated plain text content of the message.

**Inner Mechanisms:**
- Iterates through all MIME parts in the message.
- Extracts the `Content-Type` header and decodes it.
- For `text/plain` or `text/html` parts, the body is extracted and HTML is converted to plain text.
- Subject lines are also included in the output.
- Returns a single string with all text content, trimmed of trailing whitespace.

**Usage Context:**
Used during spam filtering to analyze message content and during message display for previews.

**Example:**
```php
$data = new data("path/to/message.dat");
$text = mailbox_text($data); // Returns plain text for spam analysis
```

---

#### `mailbox_address($value)`

**Purpose:**
Converts a raw RFC 2822 address string (e.g., `"John Doe" <john@example.com>`) into HTML links for interactive display.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Raw email address string in RFC 2822 format. |

**Return Value:**
`string` – HTML string with clickable address links.

**Inner Mechanisms:**
- Parses the input using `mime_extract_rfc2822_address()`.
- For each address, generates a link that triggers a JavaScript function `a(group, name, email)`.
- Displays group names (e.g., mailing lists) with a group icon.
- Uses `qx()` and `x()` for safe escaping.

**Usage Context:**
Used in message display to render sender, recipient, and CC/BCC fields.

**Example:**
```php
$from = '"Support Team" <support@company.com>';
echo mailbox_address($from);
// Outputs: <a href="javascript:a('','Support Team','support@company.com');" ...>...</a>
```

---

#### `mailbox_attachment(&$data)`

**Purpose:**
Determines whether a MIME message contains attachments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Reference to a `data` object containing MIME message parts. |

**Return Value:**
`bool` – `TRUE` if the message contains attachments, `FALSE` otherwise.

**Inner Mechanisms:**
- Scans all MIME parts.
- Returns `TRUE` if any part is not `multipart/*` or `text/*`, or if a part is of type `message/rfc822` (embedded message).
- Used to set the `CMS_MAILBOX_STATUS_ATTACHMENT` flag.

**Usage Context:**
Used during message indexing and display to indicate attachment presence.

**Example:**
```php
$data = new data("path/to/message.dat");
if (mailbox_attachment($data)) {
    echo "This message has attachments.";
}
```

---

#### `mailbox_path($container = NULL, $filename = NULL)`

**Purpose:**
Generates the filesystem path for mailbox containers and message files.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$container` | `string` | Container name (e.g., `#inbox`, `#draft`). Default: `NULL`. |
| `$filename` | `string` | Filename (e.g., message ID). Default: `NULL`. |

**Return Value:**
`string` – Full filesystem path.

**Inner Mechanisms:**
- Uses a static cache to avoid repeated filename encoding.
- Encodes container and filename using `encode_filename()`.
- Prepends `DESKTOP_PATH` and the current object directory.
- Handles predefined containers (e.g., `#inbox`) with localized names.

**Usage Context:**
Used throughout the module to locate message and index files.

**Example:**
```php
$path = mailbox_path("#inbox", "abc123.dat");
// Returns: /path/to/desktop/user/object/#inbox/abc123.dat
```

---

### Main Logic Flow

The module is driven by the `CMS_IFC_MESSAGE` switch, which handles various interface actions:

| Action | Description |
|-------|-------------|
| `retrieve` | Fetches emails from POP3 server, applies spam filtering, and stores them. |
| `display`, `_display` | Displays a message or serves a MIME part as a download. |
| `move` | Moves selected messages between containers. |
| `compose`, `compose_edit`, `compose_reply`, `compose_relay` | Creates or edits drafts. |
| `compose_send`, `compose_save` | Sends or saves a draft. |
| `create_container`, `rename_container`, `delete_container` | Manages mailbox folders. |
| `train_bad`, `train_good` | Trains the spam filter. |
| `empty_trashbin` | Deletes messages from trash and optionally from server. |
| `configure`, `_configure` | Configures email account settings. |

---

### Example: Sending an Email

```php
// Assume user fills out a form with:
// to: "user@example.com"
// subject: "Hello"
// body: "This is a test."

// After form submission, the following is triggered:
$message = unique_id(); // e.g., "abc123"
$mime = new mime(mailbox_path("#draft", "$message.dat"));

// Create multipart frame
$key = $mime->add_multipart();
$mime->data->set('"John Doe" <john@domain.com>', $key, "from");
$mime->data->set("user@example.com", $key, "to");
$mime->data->set("Hello", $key, "subject");
$mime->data->set(CMS_MAILBOX_DRAFT_TYPE_MESSAGE, $key, "#draft_type");
$mime->data->set(CMS_MAILBOX_STATUS_DRAFT, $key, "#status");

// Add text body
$key = $mime->add_text(NULL, "This is a test.", "plain", $key);
$mime->data->set(CMS_MAILBOX_DRAFT_OPTION_TEXT, $key, "#draft_option");
$mime->data->save();

// Later, when sending:
$smtp = new smtp("smtp.domain.com", "john@domain.com", "password");
if ($smtp->send($mime)) {
    // Move to outbox
    rename(mailbox_path("#draft", "$message.dat"), mailbox_path("#outbox", "$message.dat"));
}
```

---

### Key Features

- **Container-Based Organization**: Emails are stored in folders (e.g., Inbox, Drafts, Sent).
- **Spam Filtering**: Integrated with the `category` module for Bayesian spam detection.
- **Draft Management**: Supports saving, editing, and sending drafts.
- **MIME Support**: Full parsing and rendering of multipart messages.
- **Attachment Handling**: Supports file uploads and downloads.
- **Localization**: Uses language constants (e.g., `CMS_L_DESKTOP_MAILBOX_001`).
- **Efficient Storage**: Uses binary `.dat` files and a central index for fast access.

---

### Dependencies

- `pop` – POP3 client for retrieving emails.
- `smtp` – SMTP client for sending emails.
- `mime` – MIME message parser and builder.
- `category` – Optional spam filtering module.
- `filemanager` – For recursive directory operations.

The module is designed for high performance and low resource usage, adhering to PWNC’s zero-dependency philosophy.


<!-- HASH:00d5d32eadaf437297d801f496a02818 -->
