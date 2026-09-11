# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.mime.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.mime.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MIME Class

The `mime` class provides a comprehensive framework for constructing, encoding, and decoding MIME-compliant messages, particularly email messages. It supports multipart structures, various content types (text, images, files), RFC 2047 header encoding/decoding, RFC 2231 parameter encoding, and proper handling of boundaries and content IDs.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MIME_TYPE_TEXT` | `"text"` | MIME type for text content |
| `CMS_MIME_TYPE_IMAGE` | `"image"` | MIME type for image content |
| `CMS_MIME_TYPE_AUDIO` | `"audio"` | MIME type for audio content |
| `CMS_MIME_TYPE_VIDEO` | `"video"` | MIME type for video content |
| `CMS_MIME_TYPE_APPLICATION` | `"application"` | MIME type for application data |
| `CMS_MIME_TYPE_MESSAGE_RFC822` | `"message/rfc822"` | MIME type for RFC822 messages |
| `CMS_MIME_TYPE_MESSAGE_PARTIAL` | `"message/partial"` | MIME type for partial messages |
| `CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY` | `"message/external-body"` | MIME type for external body references |
| `CMS_MIME_ENCODING_7BIT` | `"7bit"` | 7-bit content transfer encoding |
| `CMS_MIME_ENCODING_8BIT` | `"8bit"` | 8-bit content transfer encoding |
| `CMS_MIME_ENCODING_BASE64` | `"base64"` | Base64 content transfer encoding |
| `CMS_MIME_ENCODING_BINARY` | `"binary"` | Binary content transfer encoding |
| `CMS_MIME_ENCODING_QUOTED_PRINTABLE` | `"quoted-printable"` | Quoted-printable content transfer encoding |
| `CMS_MIME_DISPOSITION_INLINE` | `"inline"` | Inline content disposition |
| `CMS_MIME_DISPOSITION_ATTACHMENT` | `"attachment"` | Attachment content disposition |
| `CMS_MIME_HEADER_TYPE_ADDRESS` | `1` | Header type for address fields (To, From, etc.) |
| `CMS_MIME_HEADER_TYPE_MIME` | `2` | Header type for MIME-specific headers |
| `CMS_MIME_HEADER_TYPE_IDENTIFICATION` | `3` | Header type for identification headers |
| `CMS_MIME_HEADER_TYPE_INFORMATIONAL` | `4` | Header type for informational headers |
| `CMS_MIME_RFC2231_PARAM_CONTINUATION` | `FALSE` | RFC 2231 parameter continuation support flag |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Internal data structure for storing MIME parts |

### Methods

#### `__construct($name = NULL)`

Initializes a new MIME message instance with an internal data structure.

**Parameters:**
- `$name` (string, optional): Name for the internal data structure.

**Example:**
```php
$mime = new mime("my_email");
```

#### `add_text($subject = NULL, $body = NULL, $subtype = "plain", $parent_key = NULL, $content_id = NULL)`

Adds a text part to the MIME message.

**Parameters:**
- `$subject` (string, optional): Subject of the text part
- `$body` (string, optional): Content of the text part
- `$subtype` (string): Text subtype (default: "plain")
- `$parent_key` (mixed, optional): Parent container key
- `$content_id` (string, optional): Content ID for the part

**Returns:** 
- `mixed`: Result of `$this->data->insert()` or `FALSE` if parent is not a container

**Example:**
```php
$mime = new mime();
$mime->add_multipart("mixed");
$mime->add_text("Hello World", "This is a plain text message.");
```

#### `add_file($file, $filename = NULL, $attachment = TRUE, $parent_key = NULL, $content_id = NULL)`

Adds a file attachment to the MIME message.

**Parameters:**
- `$file` (string): Path to the file to attach
- `$filename` (string, optional): Filename for the attachment (defaults to basename of `$file`)
- `$attachment` (boolean): Whether to treat as attachment (TRUE) or inline (FALSE)
- `$parent_key` (mixed, optional): Parent container key
- `$content_id` (string, optional): Content ID for the part

**Returns:**
- `mixed`: Result of `$this->data->insert()` or `FALSE` if parent is not a container

**Example:**
```php
$mime = new mime();
$mime->add_multipart("mixed");
$mime->add_file("/path/to/document.pdf", "document.pdf");
```

#### `add_part($content_type = CMS_MIME_TYPE_TEXT, $subtype = "plain", $subject = NULL, $body = NULL, $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE, $content_disposition = CMS_MIME_DISPOSITION_INLINE, $name = NULL, $parent_key = NULL, $content_id = NULL)`

Adds a generic MIME part to the message.

**Parameters:**
- `$content_type` (string): Main content type
- `$subtype` (string): Content subtype
- `$subject` (string, optional): Subject of the part
- `$body` (string, optional): Body content
- `$content_transfer_encoding` (string): Transfer encoding method
- `$content_disposition` (string): Disposition type
- `$name` (string, optional): Name for the part
- `$parent_key` (mixed, optional): Parent container key
- `$content_id` (string, optional): Content ID for the part

**Returns:**
- `mixed`: Result of `$this->data->insert()` or `FALSE` if parent is not a container

**Example:**
```php
$mime = new mime();
$mime->add_multipart("mixed");
$mime->add_part(
    CMS_MIME_TYPE_IMAGE,
    "jpeg",
    NULL,
    file_get_contents("/path/to/image.jpg"),
    CMS_MIME_ENCODING_BASE64,
    CMS_MIME_DISPOSITION_ATTACHMENT,
    "image.jpg"
);
```

#### `add_multipart($subtype = "mixed", $subject = NULL, $parent_key = NULL)`

Creates a multipart container in the MIME message.

**Parameters:**
- `$subtype` (string): Multipart subtype (default: "mixed")
- `$subject` (string, optional): Subject for the multipart container
- `$parent_key` (mixed, optional): Parent container key

**Returns:**
- `mixed`: Result of `$this->data->insert()` or `FALSE` if parent is not a container

**Example:**
```php
$mime = new mime();
$mime->add_multipart("mixed");
$mime->add_multipart("alternative", NULL, $mime->data->get("first"));
```

#### `add_message($subtype = "rfc822", $data = NULL, $parent_key = NULL)`

Embeds another MIME message within the current message.

**Parameters:**
- `$subtype` (string): Message subtype (default: "rfc822")
- `$data` (mime, optional): Another mime instance to embed
- `$parent_key` (mixed, optional): Parent container key

**Returns:**
- `mixed`: Result of `$this->data->insert()` or `FALSE` if parent is not a container

**Example:**
```php
$outer = new mime();
$inner = new mime();
$inner->add_text("Forwarded message", "Original content");
$outer->add_multipart("mixed");
$outer->add_message("rfc822", $inner);
```

#### `build($from = NULL, $to = NULL, $cc = NULL, $bcc = NULL, $reply_to = NULL)`

Builds the complete MIME message with headers and body.

**Parameters:**
- `$from` (string, optional): Sender email address
- `$to` (string, optional): Recipient email address
- `$cc` (string, optional): CC recipient email address
- `$bcc` (string, optional): BCC recipient email address
- `$reply_to` (string, optional): Reply-to email address

**Returns:**
- `string`: Complete MIME message

**Example:**
```php
$mime = new mime();
$mime->add_multipart("mixed");
$mime->add_text("Hello", "World");
$message = $mime->build("sender@example.com", "recipient@example.com");
```

#### `save($name = NULL)`

Saves the MIME message data to persistent storage.

**Parameters:**
- `$name` (string, optional): Name to save under

**Example:**
```php
$mime = new mime("draft_email");
$mime->add_text("Subject", "Body");
$mime->save();
```

#### `content_id()`

Generates a unique content ID for MIME parts.

**Returns:**
- `string`: Unique content ID in the format `<count.message_count.pid.timestamp@domain>`

**Example:**
```php
$mime = new mime();
$id = $mime->content_id(); // e.g., "<1.0.1234.1620000000@example.com>"
```

### Related Functions

#### `mime_quote_rfc2045($string)`

Quotes a string according to RFC 2045 rules for use in MIME headers.

**Parameters:**
- `$string` (string): String to quote

**Returns:**
- `string`: Quoted string

#### `mime_rfc2047_decode($string)`

Decodes RFC 2047 encoded words in a string.

**Parameters:**
- `$string` (string): String containing RFC 2047 encoded words

**Returns:**
- `string`: Decoded string

#### `mime_rfc2047_encode($string, &$filling_level = NULL)`

Encodes a string using RFC 2047 encoding.

**Parameters:**
- `$string` (string): String to encode
- `$filling_level` (int, optional): Reference to track line length for folding

**Returns:**
- `string`: RFC 2047 encoded string

#### `mime_unquote_rfc2822($string)`

Unquotes a string according to RFC 2822 rules.

**Parameters:**
- `$string` (string): Quoted string

**Returns:**
- `string`: Unquoted string

#### `mime_quote_rfc2822($string)`

Quotes a string according to RFC 2822 rules.

**Parameters:**
- `$string` (string): String to quote

**Returns:**
- `string`: Quoted string

#### `mime_extract_rfc2822_address($string)`

Extracts email addresses from an RFC 2822 address string.

**Parameters:**
- `$string` (string): RFC 2822 address string

**Returns:**
- `array`: Associative array of groups, addresses, and names

#### `mime_build_rfc2822_address($array, &$filling_level = NULL)`

Builds an RFC 2822 address string from an array.

**Parameters:**
- `$array` (array): Array of groups, addresses, and names
- `$filling_level` (int, optional): Reference to track line length for folding

**Returns:**
- `string`: RFC 2822 formatted address string

#### `mime_extract_rfc2822_header($string)`

Extracts header parameters from an RFC 2822 header value.

**Parameters:**
- `$string` (string): Header value string

**Returns:**
- `array`: Associative array of parameters

#### `mime_build_rfc2231_param($name, $value, &$filling_level = NULL)`

Builds an RFC 2231 parameter for MIME headers.

**Parameters:**
- `$name` (string): Parameter name
- `$value` (string): Parameter value
- `$filling_level` (int, optional): Reference to track line length for folding

**Returns:**
- `string`: RFC 2231 formatted parameter

#### `mime_build_rfc2822_header($array, &$filling_level = NULL)`

Builds an RFC 2822 header from an array of values.

**Parameters:**
- `$array` (array): Array containing "#value" and parameters
- `$filling_level` (int, optional): Reference to track line length for folding

**Returns:**
- `string`: RFC 2822 formatted header

#### `mime_convert_header($name, $value)`

Converts a header value based on its type.

**Parameters:**
- `$name` (string): Header name
- `$value` (string): Header value

**Returns:**
- `string`: Converted header value

#### `mime_encode_body($value, $content_transfer_encoding)`

Encodes a message body according to the specified content transfer encoding.

**Parameters:**
- `$value` (string): Body content to encode
- `$content_transfer_encoding` (string): Encoding method

**Returns:**
- `string`: Encoded body content

#### `mime_decode_body($value, &$encoding, &$charset)`

Decodes a message body and converts its charset.

**Parameters:**
- `$value` (string): Body content to decode
- `$encoding` (string): Reference to store detected encoding
- `$charset` (string): Reference to store detected charset

**Returns:**
- `string`: Decoded body content

#### `mime_header_type($name)`

Determines the type of a MIME header.

**Parameters:**
- `$name` (string): Header name

**Returns:**
- `int`: Header type constant (1-4)


<!-- HASH:38c4933ba0b2097a71b4d190e9d40945 -->
