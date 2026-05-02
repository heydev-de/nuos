# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.mime.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## MIME Handling Utilities (`lib.mime.inc`)

This file provides utilities for parsing, constructing, and managing MIME (Multipurpose Internet Mail Extensions) messages in the NUOS platform. It includes functions for extracting RFC 2822 addresses and headers, encoding MIME headers, and a `mime` class for building complex multipart messages (e.g., emails with attachments).

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MIME_TYPE_TEXT` | `"text"` | MIME type for text content. |
| `CMS_MIME_TYPE_IMAGE` | `"image"` | MIME type for image content. |
| `CMS_MIME_TYPE_AUDIO` | `"audio"` | MIME type for audio content. |
| `CMS_MIME_TYPE_VIDEO` | `"video"` | MIME type for video content. |
| `CMS_MIME_TYPE_APPLICATION` | `"application"` | MIME type for application-specific data. |
| `CMS_MIME_TYPE_MESSAGE_RFC822` | `"message/rfc822"` | MIME type for RFC 822 messages. |
| `CMS_MIME_TYPE_MESSAGE_PARTIAL` | `"message/partial"` | MIME type for partial messages. |
| `CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY` | `"message/external-body"` | MIME type for messages with external bodies. |
| `CMS_MIME_CHARSET_UTF_8` | `"utf-8"` | UTF-8 character set. |
| `CMS_MIME_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 character set. |
| *(Additional `CMS_MIME_CHARSET_*` constants omitted for brevity; follow the same pattern.)* | | |
| `CMS_MIME_CHARSET_US_ASCII` | `"us-ascii"` | US-ASCII character set. |
| `CMS_MIME_ENCODING_7BIT` | `"7bit"` | 7-bit encoding. |
| `CMS_MIME_ENCODING_8BIT` | `"8bit"` | 8-bit encoding. |
| `CMS_MIME_ENCODING_BASE64` | `"base64"` | Base64 encoding. |
| `CMS_MIME_ENCODING_BINARY` | `"binary"` | Binary encoding. |
| `CMS_MIME_ENCODING_QUOTED_PRINTABLE` | `"quoted-printable"` | Quoted-printable encoding. |
| `CMS_MIME_DISPOSITION_INLINE` | `"inline"` | Inline content disposition. |
| `CMS_MIME_DISPOSITION_ATTACHMENT` | `"attachment"` | Attachment content disposition. |

---

## Functions

### `mime_extract_rfc2822_address($string)`

**Purpose:**
Parses an RFC 2822-compliant address string (e.g., email headers) into a structured array of groups, addresses, and display names.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | RFC 2822 address string (e.g., `"John Doe <john@example.com>, Jane <jane@example.com>"`). |

**Return Values:**
- `array`: Nested associative array structured as `[group] => [address] => [name]`.

**Inner Mechanisms:**
1. **Lexer:** Tokenizes the input string, handling:
   - Comment blocks (`(...)`).
   - Address blocks (`<...>`).
   - Domain literals (`[...]`).
   - Quoted strings (`"..."`).
   - Group delimiters (`:`, `;`, `,`).
2. **Parser:** Processes tokens to extract groups, addresses, and names.

**Usage Context:**
- Parsing email headers (`From`, `To`, `Cc`, `Bcc`) for display or processing.
- Validating or extracting email addresses from user input.

---

### `mime_extract_rfc2822_header($string)`

**Purpose:**
Parses an RFC 2822 header string (e.g., `Content-Type: text/plain; charset="utf-8"`) into a structured array of parameters.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | RFC 2822 header string (e.g., `"text/plain; charset=utf-8"`). |

**Return Values:**
- `array`: Associative array with `#value` for the main value and keys for parameters (e.g., `["#value" => "text/plain", "charset" => "utf-8"]`).

**Inner Mechanisms:**
1. **Lexer:** Tokenizes the input string, handling:
   - Comment blocks (`(...)`).
   - Parameter delimiters (`;`, `=`).
   - Quoted strings (`"..."`).
2. **Parser:** Processes tokens to separate the main value from parameters.

**Usage Context:**
- Parsing MIME headers (e.g., `Content-Type`, `Content-Disposition`) for dynamic content handling.
- Extracting metadata (e.g., charset, filename) from email attachments.

---

### `mime_encode_header($value)`

**Purpose:**
Encodes a header value for MIME compliance, handling non-ASCII characters via quoted-printable encoding and line folding.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Header value to encode (e.g., subject line). |

**Return Values:**
- `string`: Encoded header value (e.g., `"=?utf-8?q?Hello=20World?="`).

**Inner Mechanisms:**
1. **ASCII Check:** Returns the value as-is if it contains only ASCII characters.
2. **Quoted-Printable Encoding:** Encodes non-ASCII characters (e.g., `é` → `=E9`).
3. **Line Folding:** Splits long lines at 76 characters, preferring breaks at spaces.

**Usage Context:**
- Encoding email subject lines or headers with non-ASCII characters.
- Ensuring compliance with RFC 2047 for internationalized headers.

---

## Class: `mime`

**Purpose:**
Constructs and manages MIME messages (e.g., emails with attachments, multipart structures).

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Internal `data` object for storing MIME parts. |

---

### `__construct($name = NULL)`

**Purpose:**
Initializes a new MIME message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | *(Optional)* Name for the `data` object (unused in current implementation). |

**Return Values:**
- `void`

---

### `add_text($subject = NULL, $body = NULL, $subtype = "plain", $charset = CMS_MIME_CHARSET_UTF_8, $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a text part to the MIME message (e.g., email body).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subject` | `string` | *(Optional)* Subject of the part. |
| `$body` | `string` | *(Optional)* Text content. |
| `$subtype` | `string` | *(Default: `"plain"`)* Subtype (e.g., `"plain"`, `"html"`). |
| `$charset` | `string` | *(Default: `CMS_MIME_CHARSET_UTF_8`)* Character set. |
| `$parent_key` | `mixed` | *(Optional)* Key of the parent container (for nested parts). |
| `$content_id` | `string` | *(Optional)* Content-ID for referencing (e.g., in HTML emails). |

**Return Values:**
- `mixed`: Key of the added part or `FALSE` on failure.

**Usage Context:**
- Adding plain text or HTML email bodies.
- Creating nested multipart structures (e.g., `multipart/alternative`).

---

### `add_file($file, $filename = NULL, $attachment = TRUE, $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a file as a MIME part (e.g., email attachment).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$file` | `string` | Path to the file. |
| `$filename` | `string` | *(Optional)* Override filename (defaults to basename of `$file`). |
| `$attachment` | `bool` | *(Default: `TRUE`)* Whether to mark as attachment (`TRUE`) or inline (`FALSE`). |
| `$parent_key` | `mixed` | *(Optional)* Key of the parent container. |
| `$content_id` | `string` | *(Optional)* Content-ID for referencing. |

**Return Values:**
- `mixed`: Key of the added part or `FALSE` on failure.

**Usage Context:**
- Attaching files to emails.
- Embedding images in HTML emails (with `attachment = FALSE`).

---

### `add_part($content_type = CMS_MIME_TYPE_TEXT, $subtype = "plain", $subject = NULL, $body = NULL, $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE, $charset = CMS_MIME_CHARSET_UTF_8, $content_disposition = CMS_MIME_DISPOSITION_INLINE, $name = NULL, $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a generic MIME part (low-level method for `add_text` and `add_file`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$content_type` | `string` | *(Default: `CMS_MIME_TYPE_TEXT`)* MIME type (e.g., `"text"`, `"image"`). |
| `$subtype` | `string` | *(Default: `"plain"`)* Subtype (e.g., `"plain"`, `"jpeg"`). |
| `$subject` | `string` | *(Optional)* Subject of the part. |
| `$body` | `string` | *(Optional)* Raw content. |
| `$content_transfer_encoding` | `string` | *(Default: `CMS_MIME_ENCODING_QUOTED_PRINTABLE`)* Encoding (e.g., `"base64"`, `"quoted-printable"`). |
| `$charset` | `string` | *(Default: `CMS_MIME_CHARSET_UTF_8`)* Character set. |
| `$content_disposition` | `string` | *(Default: `CMS_MIME_DISPOSITION_INLINE`)* Disposition (`"inline"` or `"attachment"`). |
| `$name` | `string` | *(Optional)* Filename or part name. |
| `$parent_key` | `mixed` | *(Optional)* Key of the parent container. |
| `$content_id` | `string` | *(Optional)* Content-ID. |

**Return Values:**
- `mixed`: Key of the added part or `FALSE` on failure.

**Usage Context:**
- Custom MIME parts (e.g., calendar invites, vCards).
- Advanced use cases requiring fine-grained control over headers.

---

### `add_multipart($subtype = "mixed", $subject = NULL, $parent_key = NULL)`

**Purpose:**
Adds a multipart container (e.g., `multipart/mixed`, `multipart/alternative`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | *(Default: `"mixed"`)* Subtype (e.g., `"mixed"`, `"alternative"`). |
| `$subject` | `string` | *(Optional)* Subject of the container. |
| `$parent_key` | `mixed` | *(Optional)* Key of the parent container. |

**Return Values:**
- `mixed`: Key of the added container or `FALSE` on failure.

**Usage Context:**
- Creating emails with attachments (`multipart/mixed`).
- HTML emails with plain-text fallback (`multipart/alternative`).

---

### `add_message($subtype = "rfc822", $data = NULL, $parent_key = NULL)`

**Purpose:**
Adds an embedded message (e.g., forwarded email).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | *(Default: `"rfc822"`)* Subtype (e.g., `"rfc822"`). |
| `$data` | `mime` | `mime` object representing the embedded message. |
| `$parent_key` | `mixed` | *(Optional)* Key of the parent container. |

**Return Values:**
- `mixed`: Key of the added message or `FALSE` on failure.

**Usage Context:**
- Forwarding emails as attachments.
- Embedding messages in other messages (e.g., digests).

---

### `build($from = NULL, $to = NULL, $cc = NULL, $bcc = NULL, $reply_to = NULL)`

**Purpose:**
Compiles the MIME message into a raw string for sending.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$from` | `string` | *(Optional)* Sender address (RFC 2822 format). |
| `$to` | `string` | *(Optional)* Recipient address(es). |
| `$cc` | `string` | *(Optional)* Carbon copy address(es). |
| `$bcc` | `string` | *(Optional)* Blind carbon copy address(es). |
| `$reply_to` | `string` | *(Optional)* Reply-to address. |

**Return Values:**
- `string`: Raw MIME message ready for sending.

**Inner Mechanisms:**
1. **Header Construction:** Adds standard headers (`From`, `To`, `Cc`, `Bcc`, `MIME-Version`).
2. **Boundary Handling:** Manages multipart boundaries for nested structures.
3. **Encoding:** Applies `7bit`, `base64`, or `quoted-printable` encoding to parts.
4. **Message Count:** Tracks process-wide message count for unique `Content-ID` generation.

**Usage Context:**
- Finalizing an email before sending via `mail()` or SMTP.
- Debugging MIME structures by inspecting the raw output.

---

### `save($name = NULL)`

**Purpose:**
Saves the MIME message to the `data` object's storage.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | *(Optional)* Name for the saved data. |

**Return Values:**
- `void`

**Usage Context:**
- Persisting MIME messages for later use.
- Caching constructed messages.


<!-- HASH:e4da1a2361f8c19e695f066f223ed779 -->
