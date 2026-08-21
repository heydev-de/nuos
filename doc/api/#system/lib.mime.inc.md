# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.mime.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.mime.inc)

- **Version:** `26.8.4.6`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MIME Handling Module

This module provides comprehensive MIME (Multipurpose Internet Mail Extensions) message handling capabilities for the PWNC Web Platform. It includes functions for encoding/decoding MIME headers, parsing/building email addresses, managing multipart messages, and handling various MIME content types. The module follows RFC 2045-2049, RFC 2822, and RFC 2231 standards.

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MIME_TYPE_TEXT` | `"text"` | Base MIME type for text content |
| `CMS_MIME_TYPE_IMAGE` | `"image"` | Base MIME type for image content |
| `CMS_MIME_TYPE_AUDIO` | `"audio"` | Base MIME type for audio content |
| `CMS_MIME_TYPE_VIDEO` | `"video"` | Base MIME type for video content |
| `CMS_MIME_TYPE_APPLICATION` | `"application"` | Base MIME type for application data |
| `CMS_MIME_TYPE_MESSAGE_RFC822` | `"message/rfc822"` | MIME type for embedded RFC822 messages |
| `CMS_MIME_TYPE_MESSAGE_PARTIAL` | `"message/partial"` | MIME type for partial messages |
| `CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY` | `"message/external-body"` | MIME type for external body references |
| `CMS_MIME_ENCODING_7BIT` | `"7bit"` | 7-bit content transfer encoding |
| `CMS_MIME_ENCODING_8BIT` | `"8bit"` | 8-bit content transfer encoding |
| `CMS_MIME_ENCODING_BASE64` | `"base64"` | Base64 content transfer encoding |
| `CMS_MIME_ENCODING_BINARY` | `"binary"` | Binary content transfer encoding |
| `CMS_MIME_ENCODING_QUOTED_PRINTABLE` | `"quoted-printable"` | Quoted-printable content transfer encoding |
| `CMS_MIME_DISPOSITION_INLINE` | `"inline"` | Inline content disposition |
| `CMS_MIME_DISPOSITION_ATTACHMENT` | `"attachment"` | Attachment content disposition |
| `CMS_MIME_HEADER_TYPE_ADDRESS` | `1` | Header type for address fields |
| `CMS_MIME_HEADER_TYPE_MIME` | `2` | Header type for MIME-specific fields |
| `CMS_MIME_HEADER_TYPE_IDENTIFICATION` | `3` | Header type for identification fields |
| `CMS_MIME_HEADER_TYPE_INFORMATIONAL` | `4` | Header type for informational fields |
| `CMS_MIME_RFC2231_PARAM_CONTINUATION` | `FALSE` | Flag for RFC2231 parameter continuation support |

---

## Functions

### `mime_rfc2047_decode($string)`

**Purpose:**
Decodes MIME encoded-words in headers according to RFC 2047. Handles both Base64 (`B`/`b`) and Quoted-Printable (`Q`/`q`) encoded words.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Input string containing RFC 2047 encoded-words |

**Return Values:**
`string` - Decoded string with normalized UTF-8 characters

**Inner Mechanisms:**
1. Uses `preg_replace_callback` to find encoded-word patterns (`=?charset?encoding?encoded-text?=`)
2. For Base64 encoding, decodes using `base64_decode()`
3. For Quoted-Printable encoding, decodes using `quoted_printable_decode()` and replaces underscores with spaces
4. Converts character sets to UTF-8 using `utf8_convert()` and normalizes with `utf8_normalize()`
5. Handles whitespace between encoded-words to prevent duplication

**Usage Context:**
Used when parsing incoming email headers or MIME messages to decode internationalized header values.

**Example:**
```php
$encoded = "=?UTF-8?B?8J+NgSBQV05DIFdlYiBQbGF0Zm9ybQ==?=";
$decoded = mime_rfc2047_decode($encoded);
// Returns: "🌐 PWNC Web Platform"
```

---

### `mime_rfc2047_encode($string, &$filling_level = NULL)`

**Purpose:**
Encodes strings into RFC 2047 encoded-words for MIME headers. Automatically handles line folding and character encoding.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | String to encode |
| `$filling_level` | `int` | Reference to current line length (for folding) |

**Return Values:**
`string` - RFC 2047 encoded string

**Inner Mechanisms:**
1. Checks if encoding is needed by testing for non-ASCII or problematic characters
2. If no encoding needed, applies header folding using `wordwrap()`
3. For encoding needed:
   - Splits string into UTF-8 characters
   - Encodes each character using Quoted-Printable rules
   - Builds encoded-words with proper line folding
   - Maintains line length ≤ 78 characters
4. Uses static lookup table for efficient encoding of repeated characters

**Usage Context:**
Used when constructing email headers with non-ASCII characters or when line length exceeds limits.

**Example:**
```php
$filling = 0;
$encoded = mime_rfc2047_encode("PWNC Web Platform 🌐", $filling);
// Returns: "=?utf-8?q?PWNC_Web_Platform_=F0=9F=8C=90?="
```

---

### `mime_unquote_rfc2822($string)`

**Purpose:**
Removes RFC 2822 quoting from strings while preserving RFC 2047 encoded-words.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Quoted string to process |

**Return Values:**
`string` - Unquoted string with decoded encoded-words

**Inner Mechanisms:**
1. Processes string character-by-character
2. Handles quoted strings (`"..."`) and escaped characters (`\"`)
3. Preserves RFC 2047 encoded-words by only decoding unquoted text
4. Reconstructs the final string from processed tokens

**Usage Context:**
Used when parsing email address fields or header values that may contain quoted strings.

**Example:**
```php
$quoted = '"John Doe" <john@example.com>';
$unquoted = mime_unquote_rfc2822($quoted);
// Returns: "John Doe <john@example.com>"
```

---

### `mime_quote_rfc2822($string)`

**Purpose:**
Adds RFC 2822 quoting to strings when they contain special characters.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | String to potentially quote |

**Return Values:**
`string` - Quoted string if needed, original string otherwise

**Inner Mechanisms:**
1. Checks for characters that require quoting: `"(),.:;<=>?@[\]` and spaces
2. Uses `addcslashes()` to escape quotes and backslashes
3. Wraps in double quotes if any special characters are found

**Usage Context:**
Used when constructing email headers to ensure proper formatting of values containing special characters.

**Example:**
```php
$name = 'John "Awesome" Doe';
$quoted = mime_quote_rfc2822($name);
// Returns: "\"John \\\"Awesome\\\" Doe\""
```

---

### `mime_extract_rfc2822_address($string)`

**Purpose:**
Parses RFC 2822 address fields into structured arrays.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Address field string to parse |

**Return Values:**
`array` - Structured array with format `[group][address] => name`

**Inner Mechanisms:**
1. Lexer phase:
   - Processes string character-by-character
   - Handles quotes, comments, literals, and address delimiters
   - Builds token array
2. Parser phase:
   - Processes tokens to extract groups, addresses, and names
   - Handles quoted and encoded strings
   - Returns structured array

**Usage Context:**
Used when parsing email headers like `To`, `From`, `Cc`, or `Bcc` fields.

**Example:**
```php
$addresses = 'John Doe <john@example.com>, "Team, PWNC" <team@pwnc.it>';
$parsed = mime_extract_rfc2822_address($addresses);
/*
Returns:
[
    '' => [
        'john@example.com' => 'John Doe',
        'team@pwnc.it' => 'Team, PWNC'
    ]
]
*/
```

---

### `mime_build_rfc2822_address($array, &$filling_level = NULL)`

**Purpose:**
Builds RFC 2822 compliant address fields from structured arrays.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Structured address array (from `mime_extract_rfc2822_address`) |
| `$filling_level` | `int` | Reference to current line length (for folding) |

**Return Values:**
`string` - RFC 2822 compliant address string

**Inner Mechanisms:**
1. Processes groups and addresses
2. Encodes names using `mime_rfc2047_encode()`
3. Handles line folding to maintain ≤ 78 characters per line
4. Properly formats address syntax: `Name <address>` or just `address`

**Usage Context:**
Used when constructing email headers from structured address data.

**Example:**
```php
$addresses = [
    '' => [
        'john@example.com' => 'John Doe',
        'team@pwnc.it' => 'Team, PWNC'
    ]
];
$filling = 0;
$built = mime_build_rfc2822_address($addresses, $filling);
// Returns: "John Doe <john@example.com>, =?utf-8?q?Team=2C_PWNC?= <team@pwnc.it>"
```

---

### `mime_extract_rfc2822_header($string)`

**Purpose:**
Parses RFC 2822 header values into structured arrays, handling parameters and RFC 2231 encoding.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Header value string to parse |

**Return Values:**
`array` - Structured array with `#value` for main value and parameters as keys

**Inner Mechanisms:**
1. Lexer phase:
   - Processes string character-by-character
   - Handles quotes, comments, and parameter delimiters
   - Builds token array
2. Parser phase:
   - Extracts main value and parameters
   - Handles RFC 2231 parameter encoding and continuation
   - Converts character sets to UTF-8

**Usage Context:**
Used when parsing MIME headers like `Content-Type` or `Content-Disposition`.

**Example:**
```php
$header = 'text/plain; charset="utf-8"; filename*=utf-8\'\'example.txt';
$parsed = mime_extract_rfc2822_header($header);
/*
Returns:
[
    '#value' => 'text/plain',
    'charset' => 'utf-8',
    'filename' => 'example.txt'
]
*/
```

---

### `mime_build_rfc2231_param($name, $value, &$filling_level = NULL)`

**Purpose:**
Builds RFC 2231 compliant parameter values for MIME headers.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Parameter name |
| `$value` | `string` | Parameter value |
| `$filling_level` | `int` | Reference to current line length (for folding) |

**Return Values:**
`string` - RFC 2231 compliant parameter string

**Inner Mechanisms:**
1. Determines if value needs encoding (contains non-ASCII)
2. If RFC 2231 continuation disabled, uses simple parameter syntax
3. If continuation enabled:
   - Splits value into UTF-8 characters
   - Encodes each character as needed
   - Builds parameter fragments with proper line folding
   - Handles single vs. multiple line cases

**Usage Context:**
Used when constructing MIME headers with parameters that may contain non-ASCII characters.

**Example:**
```php
$filling = 0;
$param = mime_build_rfc2231_param('filename', 'example-文件.txt', $filling);
// Returns: "; filename*=utf-8''example-%E6%96%87%E4%BB%B6.txt"
```

---

### `mime_build_rfc2822_header($array, &$filling_level = NULL)`

**Purpose:**
Builds complete RFC 2822 header values from structured arrays.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Structured header array (from `mime_extract_rfc2822_header`) |
| `$filling_level` | `int` | Reference to current line length (for folding) |

**Return Values:**
`string` - RFC 2822 compliant header value

**Inner Mechanisms:**
1. Encodes main value using `mime_rfc2047_encode()`
2. Processes each parameter using `mime_build_rfc2231_param()`
3. Combines main value and parameters into single string

**Usage Context:**
Used when constructing MIME headers from structured data.

**Example:**
```php
$header = [
    '#value' => 'text/plain',
    'charset' => 'utf-8',
    'filename' => 'example.txt'
];
$filling = 0;
$built = mime_build_rfc2822_header($header, $filling);
// Returns: "text/plain; charset=\"utf-8\"; filename=\"example.txt\""
```

---

### `mime_convert_header($name, $value)`

**Purpose:**
Converts header values to their proper RFC-compliant format based on header type.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Header name |
| `$value` | `string` | Header value to convert |

**Return Values:**
`string` - Properly formatted header value

**Inner Mechanisms:**
1. Determines header type using `mime_header_type()`
2. Processes value based on type:
   - Address fields: extract and rebuild addresses
   - MIME fields: extract and rebuild header structure
   - Identification fields: return as-is
   - Informational fields: decode and re-encode

**Usage Context:**
Used when constructing email headers to ensure proper formatting.

**Example:**
```php
$converted = mime_convert_header('Subject', 'Hello 世界');
// Returns: "=?utf-8?q?Hello_=E4=B8=96=E7=95=8C?="
```

---

### `mime_encode_body($value, $content_transfer_encoding)`

**Purpose:**
Encodes message bodies according to specified content transfer encoding.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Body content to encode |
| `$content_transfer_encoding` | `string` | Encoding type (from constants) |

**Return Values:**
`string` - Encoded body content

**Inner Mechanisms:**
1. Handles different encoding types:
   - 7bit: removes non-ASCII characters
   - Base64: encodes using `base64_encode()` and `chunk_split()`
   - Quoted-Printable: encodes using `quoted_printable_encode()`
2. Returns original value for unsupported encodings

**Usage Context:**
Used when building MIME messages to encode body content.

**Example:**
```php
$body = "Hello 世界";
$encoded = mime_encode_body($body, CMS_MIME_ENCODING_BASE64);
// Returns: base64 encoded string with proper line breaks
```

---

### `mime_decode_body($value, &$encoding, &$charset)`

**Purpose:**
Decodes message bodies and converts character sets to UTF-8.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Encoded body content |
| `$encoding` | `string` | Reference to content transfer encoding |
| `$charset` | `string` | Reference to character set |

**Return Values:**
`string` - Decoded and converted body content

**Inner Mechanisms:**
1. Decodes based on encoding:
   - Base64: decodes using `base64_decode()`
   - Quoted-Printable: decodes using `quoted_printable_decode()`
2. Converts character set to UTF-8 using `utf8_convert()` and `utf8_normalize()`
3. Updates encoding and charset references

**Usage Context:**
Used when processing incoming MIME messages to decode body content.

**Example:**
```php
$encoding = 'base64';
$charset = 'iso-8859-1';
$body = "SGVsbG8g8J+NgA=="; // "Hello 🌐" in base64
$decoded = mime_decode_body($body, $encoding, $charset);
// Returns: "Hello 🌐" in UTF-8
// $encoding becomes "8bit"
// $charset becomes "utf-8"
```

---

### `mime_header_type($name)`

**Purpose:**
Determines the type of a MIME header field.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Header name |

**Return Values:**
`int` - Header type constant

**Inner Mechanisms:**
1. Compares header name against known types
2. Returns appropriate constant:
   - Address fields (To, From, Cc, etc.)
   - MIME fields (Content-Type, Content-Disposition)
   - Identification fields (Message-ID, Date, etc.)
   - Informational fields (default)

**Usage Context:**
Used to determine how to process different header fields.

**Example:**
```php
$type = mime_header_type('Subject');
// Returns: CMS_MIME_HEADER_TYPE_INFORMATIONAL
```

---

## `mime` Class

The `mime` class provides a high-level interface for constructing MIME messages with support for multipart structures, attachments, and proper MIME formatting.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Internal data structure for storing message parts |

### Constructor

#### `__construct($name = NULL)`

**Purpose:**
Creates a new MIME message instance.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Optional name for persistent storage |

**Inner Mechanisms:**
1. Initializes internal `data` object for storing message parts
2. If name provided, loads existing message from storage

**Example:**
```php
$message = new mime(); // New message
// or
$message = new mime('draft1'); // Load existing message
```

---

### Methods

#### `add_text($subject = NULL, $body = NULL, $subtype = "plain", $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a text part to the MIME message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subject` | `string` | Optional subject for the part |
| `$body` | `string` | Text content |
| `$subtype` | `string` | MIME subtype (plain, html, etc.) |
| `$parent_key` | `mixed` | Key of parent container (for multipart) |
| `$content_id` | `string` | Optional content ID |

**Return Values:**
`mixed` - Key of the added part or `FALSE` on failure

**Inner Mechanisms:**
1. Validates parent container if specified
2. Sets up proper MIME headers for text content
3. Uses Quoted-Printable encoding by default
4. Adds part to the message structure

**Example:**
```php
$message = new mime();
$message->add_text('Greetings', 'Hello from PWNC!', 'plain');
$message->add_text(NULL, '<h1>Hello</h1>', 'html');
```

---

#### `add_file($file, $filename = NULL, $attachment = TRUE, $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a file as a MIME part (attachment or inline).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$file` | `string` | Path to file |
| `$filename` | `string` | Optional filename override |
| `$attachment` | `bool` | `TRUE` for attachment, `FALSE` for inline |
| `$parent_key` | `mixed` | Key of parent container |
| `$content_id` | `string` | Optional content ID |

**Return Values:**
`mixed` - Key of the added part or `FALSE` on failure

**Inner Mechanisms:**
1. Determines MIME type using `get_mime_type()`
2. Reads file content using `read_file()`
3. Sets appropriate encoding (Base64 for binary, Quoted-Printable for text)
4. Sets content disposition (attachment or inline)
5. Adds part to the message structure

**Example:**
```php
$message = new mime();
$message->add_file('/path/to/document.pdf');
$message->add_file('/path/to/image.png', 'logo.png', false, null, 'logo1');
```

---

#### `add_part($content_type = CMS_MIME_TYPE_TEXT, $subtype = "plain", $subject = NULL, $body = NULL, $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE, $content_disposition = CMS_MIME_DISPOSITION_INLINE, $name = NULL, $parent_key = NULL, $content_id = NULL)`

**Purpose:**
Adds a custom MIME part to the message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$content_type` | `string` | MIME content type |
| `$subtype` | `string` | MIME subtype |
| `$subject` | `string` | Optional subject |
| `$body` | `string` | Part content |
| `$content_transfer_encoding` | `string` | Encoding type |
| `$content_disposition` | `string` | Disposition type |
| `$name` | `string` | Optional name |
| `$parent_key` | `mixed` | Key of parent container |
| `$content_id` | `string` | Optional content ID |

**Return Values:**
`mixed` - Key of the added part or `FALSE` on failure

**Inner Mechanisms:**
1. Validates parent container if specified
2. Constructs proper MIME headers
3. Handles name parameter for both Content-Type and Content-Disposition
4. Generates content ID if not provided
5. Adds part to the message structure

**Example:**
```php
$message = new mime();
$message->add_part(
    CMS_MIME_TYPE_APPLICATION,
    'json',
    'Configuration',
    '{"key": "value"}',
    CMS_MIME_ENCODING_8BIT,
    CMS_MIME_DISPOSITION_ATTACHMENT,
    'config.json'
);
```

---

#### `add_multipart($subtype = "mixed", $subject = NULL, $parent_key = NULL)`

**Purpose:**
Adds a multipart container to the message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | Multipart subtype (mixed, alternative, etc.) |
| `$subject` | `string` | Optional subject |
| `$parent_key` | `mixed` | Key of parent container |

**Return Values:**
`mixed` - Key of the added container or `FALSE` on failure

**Inner Mechanisms:**
1. Validates parent container if specified
2. Generates unique boundary string
3. Sets up container structure with opening and closing markers
4. Adds container to the message structure

**Example:**
```php
$message = new mime();
$mixed = $message->add_multipart('mixed');
$alt = $message->add_multipart('alternative', null, $mixed);
$message->add_text('Hello', 'Plain text version', 'plain', $alt);
$message->add_text('Hello', '<h1>HTML version</h1>', 'html', $alt);
$message->add_file('/path/to/image.png', null, false, $mixed);
```

---

#### `add_message($subtype = "rfc822", $data = NULL, $parent_key = NULL)`

**Purpose:**
Adds an embedded message to the MIME structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | Message subtype (rfc822, partial, etc.) |
| `$data` | `mime` | MIME message to embed |
| `$parent_key` | `mixed` | Key of parent container |

**Return Values:**
`mixed` - Key of the added message or `FALSE` on failure

**Inner Mechanisms:**
1. Validates parent container if specified
2. Sets up message container
3. Merges embedded message data into current structure
4. Adds container markers

**Example:**
```php
$message = new mime();
$embedded = new mime();
$embedded->add_text('Embedded', 'This is an embedded message');
$message->add_message('rfc822', $embedded);
```

---

#### `build($from = NULL, $to = NULL, $cc = NULL, $bcc = NULL, $reply_to = NULL)`

**Purpose:**
Builds the complete MIME message with all headers and parts.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$from` | `string` | From address |
| `$to` | `string` | To address(es) |
| `$cc` | `string` | Cc address(es) |
| `$bcc` | `string` | Bcc address(es) |
| `$reply_to` | `string` | Reply-To address |

**Return Values:**
`string` - Complete MIME message as string

**Inner Mechanisms:**
1. Sets primary headers (MIME-Version, Message-ID, Date, etc.)
2. Processes each part in the message structure
3. Handles multipart boundaries
4. Encodes body content
5. Formats headers properly
6. Increments message count cache

**Example:**
```php
$message = new mime();
$message->add_text('Hello', 'This is a test message');
$mime = $message->build('sender@pwnc.it', 'recipient@example.com');
mail('recipient@example.com', 'Test', $mime, "From: sender@pwnc.it\r\nContent-Type: text/plain; charset=utf-8");
```

---

#### `save($name = NULL)`

**Purpose:**
Saves the MIME message to persistent storage.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Name for storage |

**Inner Mechanisms:**
1. Delegates to internal `data` object's save method

**Example:**
```php
$message = new mime();
$message->add_text('Draft', 'This is a draft message');
$message->save('draft1');
```

---

#### `content_id()`

**Purpose:**
Generates a unique Content-ID for MIME parts.

**Return Values:**
`string` - Generated Content-ID

**Inner Mechanisms:**
1. Counts parts in current message
2. Uses process ID, timestamp, and domain to generate unique ID
3. Follows format: `<part.message.pid.timestamp@domain>`

**Example:**
```php
$message = new mime();
$id = $message->content_id();
// Returns something like: "<0.0.12345.1625097600@pwnc.it>"
```


<!-- HASH:975279541be6b8be9bb827f439caa207 -->
