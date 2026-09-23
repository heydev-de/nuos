# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.ims.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.ims.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop IMS (Internal Messaging System)

This file implements the **Internal Messaging System (IMS)** for the PWNC desktop interface. It provides a complete messaging solution allowing users to send, receive, reply to, delete, and filter messages within the desktop environment. The system uses a core resource file (`ims.core`) to persist message data and integrates with the desktop UI through interactive forms and tables.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DESKTOP_IMS_STATUS_NONE` | `0` | Message has not been sent yet (draft or pending). |
| `CMS_DESKTOP_IMS_STATUS_SENT` | `1` | Message has been sent by the current user. |

### Properties

| Variable | Type | Description |
|----------|------|-------------|
| `$ims` | `core_resource` | Core resource object managing the `ims.core` data file. |
| `$list_receiver` | `array` | List of valid message receivers (users with permission). |
| `$filter_user` | `string` | Currently filtered user (sender/receiver). |
| `$filter_thread` | `string` | Currently filtered thread ID. |

---

## Message Handling / Sub Display

Handles all message-related operations including sending, replying, filtering, and deletion via interface commands.

### `switch (CMS_IFC_MESSAGE)`

Processes incoming interface messages to perform actions like filtering, sending, replying, or deleting messages.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `CMS_IFC_MESSAGE` | `string` | The type of interface command being processed. |
| `$ifc_param` | `mixed` | Primary parameter for the command (e.g., user/thread filter or message ID). |
| `$ifc_param1` | `array` | Secondary parameter (e.g., list of receivers or message text). |
| `$ifc_param2` | `string` | Tertiary parameter (e.g., message body text). |

#### Usage Example

```php
// Send a message to selected users
CMS_IFC_MESSAGE = "_send";
$ifc_param1 = ["user1", "user2"]; // Receivers
$ifc_param2 = "Hello everyone!";  // Message text
```

---

### `case "filter_user"`

Sets the active user filter for displayed messages.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Username to filter by. |

#### Usage Example

```php
// Filter messages involving user "john"
CMS_IFC_MESSAGE = "filter_user";
$ifc_param = "john";
```

---

### `case "filter_thread"`

Sets the active thread filter for displayed messages.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Thread ID to filter by. |

#### Usage Example

```php
// Filter messages in thread "abc12345"
CMS_IFC_MESSAGE = "filter_thread";
$ifc_param = "abc12345";
```

---

### `case "filter_reset"`

Resets both user and thread filters.

#### Usage Example

```php
// Clear all filters
CMS_IFC_MESSAGE = "filter_reset";
```

---

### `case "_reply"` / `case "_send"`

Processes actual message sending or replying. Creates two records per message: one for the sender (status=SENT) and one for each receiver (status=NONE).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `array` | List of receiver usernames. |
| `$ifc_param2` | `string` | Message text content. |
| `$thread` | `string` | Thread identifier (from context). |

#### Inner Mechanism

1. Generates a unique hash for the message using `hash32()`.
2. For each receiver:
   - Creates a record owned by the receiver with status `NONE`.
   - Creates a record owned by the sender with status `SENT`.
3. Writes a notification flag file for each receiver.

#### Usage Example

```php
// Reply to a message
CMS_IFC_MESSAGE = "_reply";
$ifc_param = "msg123";           // Original message ID
$ifc_param1 = ["bob"];           // Receiver
$ifc_param2 = "Thanks for your message!"; // Reply text
```

---

### `case "reply"` / `case "send"`

Displays the message composition form. Prepares parameters for the interactive form (`ifc`) based on whether it's a new message or reply.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Message ID (for replies). |
| `$list_receiver` | `array` | Available receivers for selection. |

#### Inner Mechanism

1. For "reply": Loads original message to pre-fill text and determine thread.
2. For "send": Generates a new thread ID.
3. Builds an interactive form with:
   - Multi-select receiver list
   - Text area for message input
   - Submit button

#### Usage Example

```php
// Open reply form
CMS_IFC_MESSAGE = "reply";
$ifc_param = "msg123"; // ID of message to reply to
```

---

### `case "delete"`

Deletes selected messages based on current filters.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | `array` | Selected message hashes to delete. |
| `$filter_user` | `string` | Optional user filter. |
| `$filter_thread` | `string` | Optional thread filter. |

#### Inner Mechanism

1. Flips the selection list for quick lookup.
2. Iterates through all messages owned by the current user.
3. Applies filters if set.
4. Deletes messages matching selected hashes, respecting sender/receiver roles.

#### Usage Example

```php
// Delete selected messages
CMS_IFC_MESSAGE = "delete";
$list = ["hash1", "hash2"]; // Selected message hashes
```

---

## Main Display

Renders the main messaging interface showing received and sent messages in a table format.

### Notification Cleanup

Removes the local notification flag file upon display.

```php
$path = DESKTOP_PATH . "ims.flag";
if (is_file($path)) unlink($path);
```

### Menu Construction

Builds contextual menu items based on available actions and filters.

| Condition | Menu Item |
|-----------|-----------|
| Always | Refresh |
| If receivers exist | Create new message |
| Always | Delete selected |
| If filters active | Cancel filters |

### Message Retrieval & Display

Fetches and displays messages owned by the current user, applying any active filters.

#### Inner Mechanism

1. Queries messages with optional thread filter.
2. Separates sent vs received messages.
3. Groups sent messages by hash to show all receivers.
4. Sorts messages by time (newest first).
5. Renders each message in a table row with:
   - Checkbox for selection
   - Date and sender/receiver info
   - Message content with reply option (for received messages)

#### Usage Example

```php
// Display all messages (no filters)
$filter_user = NULL;
$filter_thread = NULL;
// System automatically renders the message table
```

### Selection Controls

Provides buttons to select/deselect all messages in the list.

```php
$ifc->set(CMS_L_ALL, "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE, "button", "javascript:ifc_list_deactivate();");
```


<!-- HASH:077872ef6b2997ceda98d310d1289917 -->
