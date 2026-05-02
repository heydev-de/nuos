# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.ims.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Instant Messaging System (IMS) Module

This file implements the **Instant Messaging System (IMS)** for the NUOS desktop interface. It provides a real-time, thread-based messaging system between users with filtering, sending, replying, and deleting capabilities. The module integrates with the NUOS permission system to dynamically populate the list of available message recipients.

---

### Constants

| Name                          | Value/Default | Description                                                                 |
|-------------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_IMS_STATUS_NONE` | `0`           | Message status: Not sent (draft or received).                              |
| `CMS_DESKTOP_IMS_STATUS_SENT` | `1`           | Message status: Sent (outgoing).                                           |

---

### Global Variables

| Name            | Value/Default | Description                                                                 |
|-----------------|---------------|-----------------------------------------------------------------------------|
| `$filter_user`  | `NULL`        | Current user filter applied to the message list.                            |
| `$filter_thread`| `NULL`        | Current thread filter applied to the message list.                          |
| `$list_receiver`| `NULL`        | Associative array of available message recipients (`name => user_id`).      |
| `$ims`          | `core_resource`| Core data object managing IMS messages with defined schema.                 |

---

### Core Resource Initialization

The `$ims` object is instantiated as a `core_resource` with the following schema:

| Field      | Type            | Description                                                                 |
|------------|-----------------|-----------------------------------------------------------------------------|
| `id`       | `string[8]`     | Unique message identifier.                                                  |
| `thread`   | `string[8]`     | Thread identifier (groups related messages).                                |
| `time`     | `string[20]`    | Timestamp of the message.                                                   |
| `owner`    | `_string[40]`   | User ID who owns the message (receiver for sent, sender for received).      |
| `receiver` | `_string[40]`   | User ID of the message recipient.                                           |
| `sender`   | `_string[40]`   | User ID of the message sender.                                              |
| `status`   | `byte`          | Message status (bitmask: `CMS_DESKTOP_IMS_STATUS_*`).                       |
| `text`     | `_string[1500]` | Message content (max 1500 characters).                                      |
| `hash`     | `string[32]`    | Unique hash for message grouping (identifies replies in the same thread).   |

---

## Message Handling & Sub-Display Logic

The module processes incoming interface commands (`CMS_IFC_MESSAGE`) via a `switch` structure. Each case handles a specific user action.

---

### `case "filter_user"`

#### Purpose
Applies a user filter to the message list. Only messages involving the specified user are displayed.

#### Parameters
| Parameter      | Type     | Description                                      |
|----------------|----------|--------------------------------------------------|
| `$ifc_param`   | `string` | User ID to filter by.                            |

#### Inner Mechanisms
- Sets `$filter_user` to the provided user ID.
- Triggers a refresh of the message list on next display.

#### Usage Context
Used when a user clicks on a sender/recipient link in the message list.

---

### `case "filter_thread"`

#### Purpose
Applies a thread filter. Only messages belonging to the specified thread are displayed.

#### Parameters
| Parameter      | Type     | Description                                      |
|----------------|----------|--------------------------------------------------|
| `$ifc_param`   | `string` | Thread ID to filter by.                          |

#### Inner Mechanisms
- Sets `$filter_thread` to the provided thread ID.
- Triggers a refresh of the message list.

#### Usage Context
Used when a user clicks on a message to view the entire conversation.

---

### `case "filter_reset"`

#### Purpose
Resets all active filters (user and thread).

#### Inner Mechanisms
- Sets both `$filter_user` and `$filter_thread` to `NULL`.
- Restores the full message list.

#### Usage Context
Triggered via the "Cancel" command in the interface.

---

### `case "_send"` and `case "_reply"`

#### Purpose
Internal handlers for sending and replying to messages. These prepare data and delegate to the UI for confirmation.

#### Parameters
| Parameter      | Type            | Description                                      |
|----------------|-----------------|--------------------------------------------------|
| `$ifc_param1`  | `array|null`    | Array of recipient user IDs.                     |
| `$ifc_param2`  | `string|null`   | Message text.                                    |

#### Inner Mechanisms
- **`_send`**: Generates a new thread ID and prepares an empty message form.
- **`_reply`**: Loads the original message text and sets the sender as the sole recipient.
- Both cases set `$thread`, `$text`, and `$message` for use in the UI.
- If no recipients are provided, an error is returned.

#### Return Values
- On success: `$ifc_response = CMS_MSG_DONE` (via UI interaction).
- On failure: `$ifc_response = CMS_MSG_ERROR`.

#### Usage Context
Called internally by the `send` and `reply` cases after user confirmation.

---

### `case "send"` and `case "reply"`

#### Purpose
UI handlers for initiating a new message or reply. They validate permissions and prepare the interface.

#### Parameters
| Parameter      | Type     | Description                                      |
|----------------|----------|--------------------------------------------------|
| `$ifc_param`   | `string` | For `reply`: message ID to reply to.             |

#### Inner Mechanisms
- Validates that `$list_receiver` is populated (i.e., there are available recipients).
- For `reply`: Loads the original message and sets the sender as the default recipient.
- For `send`: Initializes a new thread.
- Constructs an `ifc` (interface control) object with:
  - A multiselect for recipients.
  - A textarea for message input.
  - The original message text (for replies) displayed as read-only info.

#### Usage Context
Triggered via the "New Message" or "Reply" commands in the desktop interface.

---

### `case "delete"`

#### Purpose
Deletes selected messages based on their hash.

#### Parameters
| Parameter      | Type     | Description                                      |
|----------------|----------|--------------------------------------------------|
| `$list`        | `array`  | Array of message hashes to delete (from checkboxes). |

#### Inner Mechanisms
- Flips `$list` for O(1) hash lookup.
- Applies filters (`owner`, `thread`) to the message data.
- For **sent messages**: Only deletes if the current user is the sender **and** the recipient matches the filter (if any).
- For **received messages**: Only deletes if the current user is the owner **and** the sender matches the filter (if any).
- Sets `$ifc_response = CMS_MSG_DONE` on completion.

#### Usage Context
Triggered via the "Delete Selected" command.

---

## Main Display Logic

Renders the full message list with filtering, selection, and command controls.

---

### Notification Handling

#### Purpose
Removes the desktop notification flag (`ims.flag`) when the IMS interface is opened.

#### Inner Mechanisms
- Checks for the existence of `DESKTOP_PATH . "ims.flag"`.
- Deletes the file if present, clearing the "new message" indicator.

---

### Menu Construction

#### Purpose
Builds the command menu for the IMS interface.

#### Menu Items
| Label (Localized)                     | Command Target               | Action Triggered       |
|---------------------------------------|------------------------------|------------------------|
| `CMS_L_COMMAND_REFRESH`               | `desktop/command_refresh`    | Refreshes the view.    |
| `CMS_L_DESKTOP_IMS_001` (New Message) | `desktop/command_create`     | Triggers `send`.       |
| `CMS_L_COMMAND_DELETE_SELECTED`       | `desktop/command_delete`     | Triggers `delete`.     |
| `CMS_L_COMMAND_PREVIOUS` (Cancel)     | `desktop/command_cancel`     | Triggers `filter_reset`.|

---

### Message List Rendering

#### Purpose
Displays all messages matching the current filters, grouped by thread and sorted chronologically.

#### Inner Mechanisms
- Applies filters (`owner`, `thread`, `user`) to the `$ims` data.
- Groups messages by `hash` to identify replies in the same thread.
- Sorts messages by `time` + `id` in descending order (newest first).
- Uses `ifc_varied()` to alternate row colors for readability.
- For **sent messages**:
  - Displays all recipients.
  - Uses `icon_message_sent`.
- For **received messages**:
  - Displays the sender.
  - Uses `icon_message`.
  - Shows a "Reply" link if the sender is not the current user.
- Provides checkboxes for bulk selection.

#### Selection Controls
- **All**: Selects all messages.
- **Invert**: Inverts the current selection.
- **None**: Deselects all messages.

#### Usage Context
Automatically rendered when the IMS module is loaded or refreshed.

---

## Key Utility Functions Used

| Function               | Purpose                                                                 |
|------------------------|-------------------------------------------------------------------------|
| `cms_load()`           | Loads the `core_resource` library.                                      |
| `unique_id()`          | Generates a unique 8-character ID for messages and threads.             |
| `hash32()`             | Generates a 32-character hash for message grouping.                     |
| `ifc()`                | Constructs the interface control object for forms and commands.         |
| `ifc_table_open()` / `ifc_table_close()` | Wraps tabular data in standard NUOS table markup.               |
| `ifc_varied()`         | Returns alternating `class="varied"` for table rows.                    |
| `ifc_post()`           | JavaScript helper for posting interface commands.                       |
| `qrx()`                | Escapes strings for use in JavaScript (combines `q()` and `r()`).       |
| `x()`                  | Escapes strings for XML/HTML output.                                    |
| `nl2br()`              | Converts newlines to `<br>` tags.                                       |
| `friendly_date()`      | Formats timestamps into human-readable dates.                           |
| `image()`              | Generates an `<img>` tag for desktop icons.                             |
| `permission_is_user()` | Checks if a permission key refers to a user.                            |
| `cms_permission()`     | Checks if the current user has permission to interact with another user.|
| `data()`               | Loads user metadata (e.g., display names) from the system permission store.|
| `streq()` / `nstreq()` | String equality/inequality checks.                                      |
| `stre()` / `nstre()`   | Empty/non-empty string checks.                                          |

---

## Integration Points

- **Permission System**: Uses `#system/permission` to resolve user display names and validate recipient permissions.
- **Desktop Interface**: Integrates with the desktop command system (`desktop/command_*`).
- **Notification System**: Uses `ims.flag` to signal new messages to the desktop shell.
- **Localization**: All user-facing strings are localized via `CMS_L_*` constants.
- **URL Generation**: Uses `ifc_post()` for in-module navigation without full page reloads.

---

## Typical Usage Scenarios

1. **Sending a New Message**:
   - User clicks "New Message".
   - Module renders a form with recipient multiselect and textarea.
   - On submission, `_send` creates messages for each recipient and the sender.

2. **Replying to a Message**:
   - User clicks "Reply" on a received message.
   - Module loads the original message and pre-fills the recipient.
   - On submission, `_reply` creates a new message in the same thread.

3. **Filtering Messages**:
   - User clicks on a sender/recipient name.
   - Module applies a user filter and refreshes the list.
   - User clicks on a message to view the full thread.

4. **Deleting Messages**:
   - User selects messages via checkboxes.
   - Clicks "Delete Selected".
   - Module removes messages matching the current filters.

5. **Receiving Notifications**:
   - New messages trigger the creation of `ims.flag`.
   - Desktop shell displays a notification.
   - Opening IMS clears the flag.


<!-- HASH:c4b7aa947d3174682fb7d0db508dcab1 -->
