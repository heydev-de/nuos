# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.mailbox.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Mailbox Module

The `desktop.mailbox.inc` file provides a comprehensive email management interface within the NUOS web platform. It handles email retrieval, composition, organization, and spam filtering, integrating with POP3/SMTP protocols and MIME message parsing.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILBOX_PROPERTY_DATE` | `1` | Property identifier for message date. |
| `CMS_MAILBOX_PROPERTY_FROM` | `2` | Property identifier for sender information. |
| `CMS_MAILBOX_PROPERTY_SUBJECT` | `3` | Property identifier for message subject. |
| `CMS_MAILBOX_PROPERTY_STATUS` | `4` | Property identifier for message status flags. |
| `CMS_MAILBOX_PROPERTY_SPAM_INDICATOR` | `5` | Property identifier for spam probability. |
| `CMS_MAILBOX_PROPERTY_SIZE` | `6` | Property identifier for message size. |
| `CMS_MAILBOX_STATUS_NONE` | `0` | No status flags set. |
| `CMS_MAILBOX_STATUS_IMPORTANCE_HIGH` | `1` | Message marked as high importance. |
| `CMS_MAILBOX_STATUS_READ` | `2` | Message has been read. |
| `CMS_MAILBOX_STATUS_SENT` | `4` | Message was sent. |
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

## Functions

### `mailbox_directory()`

**Purpose:**
Generates a list of available mailbox directories (containers) for the current desktop object.

**Parameters:**
None.

**Return Values:**
- `array`: Associative array mapping display names to directory names.

**Inner Mechanisms:**
- Scans the desktop object's directory for subdirectories.
- Uses predefined container names if available; otherwise, falls back to raw directory names.
- Excludes `.` and `..` directories.

**Usage:**
- Used to populate dropdown menus for container selection.
- Called during mailbox interface rendering.

---

### `mailbox_convert($value, $charset = NULL)`

**Purpose:**
Converts email text content to UTF-8 and normalizes it.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Text content to convert. |
| `$charset` | `string\|NULL` | Source character set. If `NULL`, defaults to ISO-8859-1. |

**Return Values:**
- `string`: Normalized UTF-8 text.

**Inner Mechanisms:**
- Detects if input is not UTF-8.
- Converts from specified charset (or ISO-8859-1) to UTF-8.
- Normalizes UTF-8 output.

**Usage:**
- Used during email parsing to ensure consistent text encoding.
- Called by `mailbox_text()` and other message processing functions.

---

### `mailbox_text(&$data)`

**Purpose:**
Extracts and concatenates text content from a MIME message for spam analysis.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$data` | `data` | MIME message data object (passed by reference). |

**Return Values:**
- `string`: Concatenated text content from subject and plaintext/HTML bodies.

**Inner Mechanisms:**
- Iterates through MIME parts.
- Extracts subject and text/plain or text/html bodies.
- Converts content to UTF-8 and strips HTML markup if necessary.
- Skips non-text parts.

**Usage:**
- Used by spam filter to evaluate message content.
- Called during message retrieval and spam training.

---

### `mailbox_attachment(&$data)`

**Purpose:**
Determines if a MIME message contains attachments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$data` | `data` | MIME message data object (passed by reference). |

**Return Values:**
- `bool`: `TRUE` if message contains attachments or embedded messages; `FALSE` otherwise.

**Inner Mechanisms:**
- Iterates through MIME parts.
- Returns `TRUE` for non-multipart/non-text parts or embedded messages (message/rfc822).

**Usage:**
- Used to set the `CMS_MAILBOX_STATUS_ATTACHMENT` flag.
- Called during message indexing.

---

## Message Handling

The module processes messages based on the `CMS_IFC_MESSAGE` parameter, which triggers different actions.

---

### `retrieve`

**Purpose:**
Retrieves emails from a POP3 server, processes them, and stores them in the appropriate container.

**Parameters:**
None (uses global state).

**Return Values:**
None (sets `$ifc_response`).

**Inner Mechanisms:**
- Establishes POP3 connection using desktop object credentials.
- Retrieves unique message IDs.
- Processes each message:
  - Parses headers and body.
  - Converts text to UTF-8.
  - Evaluates spam probability.
  - Stores message in `#inbox` or `#spam` container.
  - Updates message index.
- Trains spam filter with message content.
- Deletes messages from server if configured.

**Usage:**
- Triggered by user action to check for new emails.
- Called via interface command.

---

### `select_container`

**Purpose:**
Changes the active mailbox container.

**Parameters:**
- `$ifc_param`: Container identifier.

**Return Values:**
None (sets `$container`).

**Usage:**
- Triggered when user selects a different mailbox folder.

---

### `display` / `_display`

**Purpose:**
Renders a message for viewing (`display`) or serves a MIME part as a download (`_display`).

**Parameters:**
- `$ifc_param`: Message identifier.
- `$part`: MIME part identifier (for `_display`).

**Return Values:**
- `display`: Renders HTML interface.
- `_display`: Outputs raw MIME part with HTTP headers.

**Inner Mechanisms:**
- `display`:
  - Renders message headers and body.
  - Handles attachments and embedded content.
  - Marks message as read.
- `_display`:
  - Serves MIME part with appropriate `Content-Type` and `Content-Disposition`.
  - Supports ETag caching.

**Usage:**
- `display`: Triggered when user clicks on a message.
- `_display`: Triggered when user downloads an attachment.

---

### `move`

**Purpose:**
Moves selected messages to a different container.

**Parameters:**
- `$list`: Array of message identifiers.
- `$target`: Target container.

**Return Values:**
None (sets `$ifc_response`).

**Inner Mechanisms:**
- Renames message files from source to target container.

**Usage:**
- Triggered when user moves messages via selection and dropdown.

---

### Message Composition

Handles multiple composition-related actions: `mail`, `compose`, `compose_edit`, `compose_reply`, `compose_relay`, `_compose`, `compose_send`, `compose_save`.

#### `mail` / `compose`

**Purpose:**
Creates a new draft message.

**Parameters:**
- `$to`: Recipient address (for `mail`).

**Inner Mechanisms:**
- Initializes a new MIME message.
- Sets default sender and signature.
- Saves draft in `#draft` container.

#### `compose_edit`

**Purpose:**
Loads an existing draft for editing.

**Parameters:**
- `$ifc_param`: Draft message identifier.

**Inner Mechanisms:**
- Loads draft from container.
- Presets input fields with draft content.

#### `compose_reply`

**Purpose:**
Creates a reply draft.

**Parameters:**
- `$ifc_param`: Original message identifier.

**Inner Mechanisms:**
- Extracts reply-to address and subject.
- Includes original message text with signature.
- Saves draft in `#draft` container.

#### `compose_relay`

**Purpose:**
Creates a forward/relay draft.

**Parameters:**
- `$ifc_param`: Original message identifier.

**Inner Mechanisms:**
- Attaches original message as `message/rfc822`.
- Saves draft in `#draft` container.

#### `_compose`

**Purpose:**
Renders the composition interface.

**Inner Mechanisms:**
- Displays address book, input fields, and attachments.
- Provides JavaScript helpers for address selection.

#### `compose_send`

**Purpose:**
Sends a draft message via SMTP.

**Inner Mechanisms:**
- Updates draft with current input.
- Establishes SMTP connection.
- Sends message.
- Moves message to `#outbox` container.

#### `compose_save`

**Purpose:**
Saves draft without sending.

**Inner Mechanisms:**
- Updates draft with current input.

**Usage:**
- Triggered by user actions during message composition.

---

### Container Management

#### `create_container`

**Purpose:**
Creates a new mailbox container.

**Parameters:**
- `$ifc_param`: Container name.

**Inner Mechanisms:**
- Creates directory for container.

#### `rename_container`

**Purpose:**
Renames an existing container.

**Parameters:**
- `$ifc_param`: New container name.

**Inner Mechanisms:**
- Renames container directory.

#### `delete_container`

**Purpose:**
Deletes a container and its contents.

**Inner Mechanisms:**
- Moves messages to `#trashbin`.
- Deletes non-data files using `filemanager_delete()`.
- Removes container directory.

**Usage:**
- Triggered by user actions for container management.

---

### Spam Training

#### `train_bad`

**Purpose:**
Marks a message as spam and trains the spam filter.

**Parameters:**
- `$ifc_param`: Message identifier.

**Inner Mechanisms:**
- Moves message to `#spam` container.
- Trains spam filter with message text.
- Updates message status.

#### `train_good`

**Purpose:**
Marks a message as non-spam and trains the spam filter.

**Parameters:**
- `$ifc_param`: Message identifier.

**Inner Mechanisms:**
- Moves message from `#spam` to `#inbox`.
- Trains spam filter with message text.
- Updates message status.

**Usage:**
- Triggered by user actions to improve spam filtering.

---

### `empty_trashbin`

**Purpose:**
Empties the trashbin by deleting messages locally and remotely.

**Inner Mechanisms:**
- Retrieves list of messages in `#trashbin`.
- Establishes POP3 connection.
- Deletes messages from server.
- Removes local message files and index entries.

**Usage:**
- Triggered by user action to clean up trash.

---

### `configure` / `_configure`

**Purpose:**
Displays (`configure`) and processes (`_configure`) mailbox configuration.

**Parameters:**
- `$ifc_param1`–`$ifc_param9`: Configuration values.

**Inner Mechanisms:**
- `configure`: Renders configuration form.
- `_configure`: Saves configuration to desktop object.

**Usage:**
- Triggered by user action to update mailbox settings.

---

## Main Display

Renders the mailbox interface with:
- Container list.
- Message list with sorting and filtering.
- Selection controls and action menu.

**Inner Mechanisms:**
- Lists containers with icons and names.
- Displays messages with status indicators (read, spam, attachment).
- Provides JavaScript functions for interaction (`c()`, `d()`, `b()`, `g()`).

**Usage:**
- Default view when accessing the mailbox interface.


<!-- HASH:22b88c648ede6c70980f96b6565d2494 -->
