# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.mailbox.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.mailbox.inc)

- **Version:** `26.9.7.12`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mailbox Module

The `desktop.mailbox.inc` file implements the complete email mailbox interface for the PWNC desktop environment. It provides functionality for retrieving emails via POP3, composing and sending messages via SMTP, managing message containers (inbox, draft, outbox, trashbin, spam), displaying messages with MIME support, and training a spam filter.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILBOX_PROPERTY_DATE` | 1 | Message date property identifier |
| `CMS_MAILBOX_PROPERTY_FROM` | 2 | Message sender property identifier |
| `CMS_MAILBOX_PROPERTY_SUBJECT` | 3 | Message subject property identifier |
| `CMS_MAILBOX_PROPERTY_STATUS` | 4 | Message status property identifier |
| `CMS_MAILBOX_PROPERTY_SPAM_INDICATOR` | 5 | Spam score property identifier |
| `CMS_MAILBOX_PROPERTY_SIZE` | 6 | Message size property identifier |
| `CMS_MAILBOX_STATUS_NONE` | 0 | No special status |
| `CMS_MAILBOX_STATUS_IMPORTANCE_HIGH` | 1 | High importance flag |
| `CMS_MAILBOX_STATUS_READ` | 2 | Read status flag |
| `CMS_MAILBOX_STATUS_SENT` | 4 | Sent status flag |
| `CMS_MAILBOX_STATUS_BAD` | 8 | Marked as spam flag |
| `CMS_MAILBOX_STATUS_GOOD` | 16 | Marked as not spam flag |
| `CMS_MAILBOX_STATUS_DRAFT` | 32 | Draft status flag |
| `CMS_MAILBOX_STATUS_ATTACHMENT` | 64 | Contains attachment flag |
| `CMS_MAILBOX_DRAFT_TYPE_MESSAGE` | 1 | New message draft type |
| `CMS_MAILBOX_DRAFT_TYPE_REPLY` | 2 | Reply draft type |
| `CMS_MAILBOX_DRAFT_TYPE_RELAY` | 3 | Forwarded message draft type |
| `CMS_MAILBOX_DRAFT_OPTION_TEXT` | 1 | Text body option identifier |
| `CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT` | 2 | Attachment option identifier |

### Functions

#### mailbox_text

Extracts plain text content from a MIME message for spam filtering or preview purposes.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Reference to a data object containing MIME message parts |

**Returns:** `string` - Concatenated plain text from subject and body parts

**Mechanisms:**
1. Iterates through all MIME parts in the message
2. Extracts and decodes the subject using RFC 2047
3. For text parts, converts HTML to plain text if needed
4. Concatenates all text content with spaces

**Usage Example:**
```php
// Used internally during spam filtering
$text = mailbox_text($data_message);
$spam_score = $category->evaluate_spam($text);
```

#### mailbox_address

Converts RFC 2822 address headers into HTML links with interactive address book functionality.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Raw RFC 2822 address header value |

**Returns:** `string` - HTML with clickable address links

**Mechanisms:**
1. Parses the address header using `mime_extract_rfc2822_address`
2. For each address group, creates HTML anchor tags
3. Links trigger JavaScript `a()` function for address book integration
4. Handles both named and unnamed addresses

**Usage Example:**
```php
// Display sender address in message view
$from_html = mailbox_address($data_message->get($key, "from"));
echo "<td>$from_html</td>";
```

#### mailbox_attachment

Checks if a MIME message contains attachments or embedded messages.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Reference to a data object containing MIME message parts |

**Returns:** `bool` - TRUE if attachments or embedded messages exist, FALSE otherwise

**Mechanisms:**
1. Iterates through all MIME parts
2. Returns TRUE immediately if an embedded message (`message/rfc822`) is found
3. Returns TRUE if any part is not multipart or text
4. Returns FALSE if all parts are multipart or text

**Usage Example:**
```php
// Set attachment status flag during message indexing
if (mailbox_attachment($data_message)) {
    $status |= CMS_MAILBOX_STATUS_ATTACHMENT;
}
```

#### mailbox_path

Generates filesystem paths for mailbox storage with caching.

**Parameters:**
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$container` | `string` | `NULL` | Container name (e.g., `#inbox`, `#draft`) |
| `$filename` | `string` | `NULL` | Filename within container |

**Returns:** `string` - Full filesystem path to mailbox resource

**Mechanisms:**
1. Uses static cache to avoid repeated encoding
2. Encodes object/container/filename using `encode_filename`
3. Constructs path from `DESKTOP_PATH`, object name, container, and filename
4. Handles predefined containers (`#inbox`, `#draft`, etc.)

**Usage Example:**
```php
// Get path to a specific message file
$path = mailbox_path("#inbox", "$message_id.dat");
$data_message = new data($path);

// Get path to mailbox index
$index_path = mailbox_path(NULL, "mailbox.dat");
```

### Message Handling

The module handles various message operations through a switch on `CMS_IFC_MESSAGE`:

#### retrieve

Connects to a POP3 server and downloads new messages.

**Process:**
1. Establishes POP3 connection using object credentials
2. Retrieves unique identifier list from server
3. For each new message:
   - Downloads message content
   - Applies spam filtering if enabled
   - Saves message to appropriate container (inbox or spam)
   - Updates message index with metadata
   - Optionally deletes from server
4. Updates spam filter training data

**Usage Context:** Triggered by the "Refresh" menu command to synchronize with remote mail server.

#### display / _display

Renders message viewing interface.

**Process:**
1. Opens message data file
2. Creates interface with reply/relay/close commands
3. Displays message headers (subject, from, date, to, cc)
4. Iterates through MIME parts:
   - Shows attachments with download links
   - Displays text content (plain or HTML converted to plain)
5. Sets read status on first display
6. For `_display` mode, outputs raw content with HTTP headers for browser viewing

**Usage Context:** Primary message viewing interface when user clicks on a message.

#### move

Moves selected messages between containers.

**Process:**
1. Validates selection and target container
2. Renames message files from source to target container
3. Reports success or error

**Usage Context:** Triggered by the "Move" button in the selection controls.

#### compose / mail / compose_edit / compose_reply / compose_relay / _compose / compose_send / compose_save

Handles message composition workflow.

**Process:**
1. **New Message (`mail`/`compose`):** Creates empty draft with default structure
2. **Edit (`compose_edit`):** Loads existing draft into form fields
3. **Reply (`compose_reply`):** Pre-fills with reply-to address and quoted text
4. **Relay (`compose_relay`):** Attaches original message as RFC822 part
5. **Send/Save (`_compose`):** 
   - Updates draft with form data
   - Handles attachments
   - Computes message size
   - For send: Establishes SMTP connection and transmits
   - For save: Stores as draft

**Usage Context:** Full message composition interface with form fields for recipients, subject, body, and attachments.

#### create_container / rename_container / delete_container

Manages mailbox containers (folders).

**Process:**
1. **Create:** Makes new directory under object path
2. **Rename:** Renames directory
3. **Delete:** Moves `.dat` files to trashbin, then deletes directory

**Usage Context:** Folder management through desktop interface commands.

#### train_bad / train_good

Trains the spam filter with user feedback.

**Process:**
1. Loads message text content
2. Undoes previous training if applicable
3. Moves message to appropriate container (spam/inbox)
4. Updates message status flags
5. Feeds text to spam filter for training
6. Updates filter model

**Usage Context:** User marks messages as spam/not-spam to improve filtering.

#### empty_trashbin

Permanently deletes messages from trashbin.

**Process:**
1. Scans trashbin for message files
2. Optionally connects to POP3 to delete remote copies
3. Removes local files and updates message index

**Usage Context:** Cleanup operation to free storage space.

#### configure / _configure

Manages mailbox configuration settings.

**Process:**
1. **Display:** Shows form with current settings (name, email, credentials, hosts, signature, spam threshold, delete flag)
2. **Save:** Updates object properties and saves configuration

**Usage Context:** Configuration interface accessed through settings menu.

### Main Display

Renders the primary mailbox interface with:

1. **Menu:** Refresh, compose, create folder, rename, delete, clean, configure
2. **Container Selector:** Dropdown to switch between folders
3. **Message List:** Table with sortable columns (date/size, from/subject, spam indicator)
4. **Selection Controls:** Select all, invert, clear, move with target selector

**Key Features:**
- Sortable columns with visual indicators
- Visual status icons (sent, draft, spam, high importance, unread)
- Attachment indicators
- Spam training controls
- Batch operations for selected messages

**Usage Context:** Default view when accessing the mailbox interface, showing all messages in the current container.


<!-- HASH:69fed0c1e2e80d1a90a25cba5d2e2f30 -->
