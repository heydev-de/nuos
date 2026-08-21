# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.ims.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.ims.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Instant Messaging System (IMS) Module

This file implements the **Instant Messaging System (IMS)** for the PWNC Web Platform's desktop interface. It provides real-time messaging between users with thread-based conversations, filtering capabilities, and a clean UI integrated into the desktop environment.

The module handles:
- Message storage and retrieval using PWNC's `core_resource` data abstraction
- User filtering (by sender/receiver or thread)
- Message composition, sending, replying, and deletion
- Notification management
- UI rendering with proper escaping and localization

---

### Constants

| Name                          | Value/Default | Description                                                                 |
|-------------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_IMS_STATUS_NONE` | `0`           | Message status: Not yet processed (default for received messages)          |
| `CMS_DESKTOP_IMS_STATUS_SENT` | `1`           | Message status: Sent by current user (used for tracking sent messages)     |

---

### Initialization and Setup

#### Core Resource Initialization
```php
$ims = new core_resource(
    CMS_DATA_PATH . "#desktop/ims.core",
    ["id" => "string[8]",
     "thread" => "string[8]",
     "time" => "string[20]",
     "owner" => "_string[40]",
     "receiver" => "_string[40]",
     "sender" => "_string[40]",
     "status" => "byte",
     "text" => "_string[1500]",
     "hash" => "string[32]"]);
```
- **Purpose**: Initializes the data store for IMS messages.
- **Parameters**:
  - **Path**: `CMS_DATA_PATH . "#desktop/ims.core"` – Physical storage location.
  - **Schema**: Defines message structure (IDs, timestamps, users, content, etc.).
- **Inner Mechanisms**:
  - Uses PWNC's `core_resource` for efficient, schema-bound data storage.
  - `_string` types are nullable; `string` types are required.
  - `byte` status field uses bitmasking (e.g., `CMS_DESKTOP_IMS_STATUS_SENT`).

#### Receiver List Population
```php
$list_receiver = NULL;
$data = new data("#system/permission");
$data->move("first");
while ($key = $data->move("next")) {
    if (!permission_is_user($key)) continue;
    $_key = substr($key, 5);
    if (nstreq($_key, DESKTOP_USER) && cms_permission(CMS_APPLICATION, NULL, NULL, $_key))
        $list_receiver[$data->get($key, "name")] = $_key;
}
```
- **Purpose**: Builds a list of users who can receive messages from the current user.
- **Inner Mechanisms**:
  - Iterates over system permissions to find valid user accounts.
  - Excludes the current user (`DESKTOP_USER`).
  - Checks application-level permissions.
  - Populates `$list_receiver` as `name => user_id` for UI selection.

---

### Message Handling and Sub-Display Logic

#### Interface Message Switch (`CMS_IFC_MESSAGE`)
Handles incoming interface commands (e.g., `send`, `reply`, `delete`, `filter_*`).

---

### `filter_user` and `filter_thread` Cases
```php
case "filter_user":
    $filter_user = $ifc_param;
    break;
case "filter_thread":
    $filter_thread = $ifc_param;
    break;
case "filter_reset":
    $filter_user = NULL;
    $filter_thread = NULL;
    break;
```
- **Purpose**: Sets or resets user/thread filters for message display.
- **Parameters**:
  - `$ifc_param`: Incoming filter value (user ID or thread ID).
- **Usage Context**:
  - Used when user clicks a sender/receiver or thread link in the UI.
  - `filter_reset` clears all filters.

---

### `_send` and `_reply` Cases (Internal Processing)
```php
case "_send":
case "_reply":
    if (isset($ifc_param1) && $ifc_param2) {
        $time = time();
        $hash = hash32($thread . $time . DESKTOP_USER . $ifc_param2);
        foreach ($ifc_param1 AS $key => $value) {
            // Insert for receiver
            $ims->next(["owner" => NULL], TRUE);
            $ims->set([...]);
            // Insert for sender (current user)
            $ims->next(["owner" => NULL]);
            $ims->set([...]);
            // Create notification
            $file = CMS_DATA_PATH . "#desktop/" . encode_filename($value) . "/ims.flag";
            write_file($file, "1");
        }
        $ifc_response = CMS_MSG_DONE;
        break;
    }
    $ifc_response = CMS_MSG_ERROR;
```
- **Purpose**: Internal handler for sending/replying to messages.
- **Parameters**:
  - `$ifc_param1`: Array of receiver user IDs.
  - `$ifc_param2`: Message text.
  - `$thread`: Thread ID (generated or existing).
- **Return/Response**:
  - `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.
- **Inner Mechanisms**:
  - Creates two copies of each message: one for the receiver, one for the sender.
  - Uses `hash32()` to group messages in the same thread/session.
  - Creates a `.flag` file in the receiver's desktop data directory to trigger UI notifications.
  - Uses `unique_id(8)` for message and thread IDs.

#### Usage Example
```php
// Simulate sending a message to user "alice" in a new thread
$ifc_param1 = ["alice"];
$ifc_param2 = "Hello, Alice!";
$thread = unique_id(8); // New thread
CMS_IFC_MESSAGE = "_send";
include "desktop.ims.inc";
// $ifc_response will be CMS_MSG_DONE on success
```

---

### `send` and `reply` Cases (UI Display)
```php
case "send":
case "reply":
    if (!$list_receiver) {
        $ifc_response = CMS_L_DESKTOP_IMS_004; // "No valid receivers"
        break;
    }
    // Determine thread, text, and receivers
    switch (CMS_IFC_MESSAGE) {
        case "reply":
            if ($ims->seek(["id" => $ifc_param])) {
                $value = $ims->get();
                $thread = $value["thread"];
                $text = $value["text"];
                $ifc_param1 = [$value["sender"]];
                $message = "_reply";
                break;
            }
        case "send":
            $thread = unique_id(8);
            $text = NULL;
            $ifc_param1 = NULL;
            $message = "_send";
            break;
    }
    // Render UI
    $ifc = new ifc(...);
    ifc_popover_open(CMS_L_DESKTOP_IMS_002); // "Select Receivers"
    ksort($list_receiver, SORT_NATURAL | SORT_FLAG_CASE);
    $ifc->set($list_receiver, "multiselect 20x14", $ifc_param1);
    ifc_popover_close();
    if (nstre($text)) echo("<div class=\"text\">" . x($text) . "</div>");
    $ifc->set(CMS_L_DESKTOP_IMS_003, "textarea 0x20 1500 w", $ifc_param2);
    $ifc->close();
```
- **Purpose**: Renders the message composition UI.
- **Parameters**:
  - `$ifc_param`: For `reply`, the message ID to reply to.
- **Inner Mechanisms**:
  - Uses `ifc` class to generate interactive forms.
  - `multiselect 20x14`: Renders a 20-item tall, 14-column wide multi-select dropdown.
  - `textarea 0x20 1500 w`: 20-line, 1500-character limited, word-wrapped textarea.
  - `x()` escapes HTML content for safe display.
  - On reply, pre-fills the receiver and displays the original message.

#### Usage Example
```php
// Trigger reply UI for message ID "abc12345"
CMS_IFC_MESSAGE = "reply";
$ifc_param = "abc12345";
include "desktop.ims.inc";
// Renders a popover with receiver pre-selected and original message shown
```

---

### `delete` Case
```php
case "delete":
    if (empty($list)) break;
    $list = array_flip($list);
    $filter = ["owner" => DESKTOP_USER];
    if ($filter_thread) $filter["thread"] = $filter_thread;
    $ims->reset();
    while ($ims->next($filter)) {
        $value = $ims->get();
        if (isset($list[$value["hash"]])) {
            if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT) {
                if ((!$filter_user) || streq($value["receiver"], $filter_user))
                    $ims->del();
            } elseif ((!$filter_user) || streq($value["sender"], $filter_user))
                $ims->del();
        }
    }
    $ifc_response = CMS_MSG_DONE;
    break;
```
- **Purpose**: Deletes selected messages.
- **Parameters**:
  - `$list`: Array of message hashes to delete (from checkbox selection).
- **Inner Mechanisms**:
  - Only deletes messages owned by the current user.
  - For sent messages, only deletes if the filter matches the receiver.
  - For received messages, only deletes if the filter matches the sender.
  - Uses `array_flip()` for O(1) hash lookup.

#### Usage Example
```php
// Simulate deleting messages with hashes "hash1", "hash2"
$list = ["hash1", "hash2"];
CMS_IFC_MESSAGE = "delete";
include "desktop.ims.inc";
// Messages matching the hashes are deleted
```

---

### Main Display Logic

#### Notification Cleanup
```php
$path = DESKTOP_PATH . "ims.flag";
if (is_file($path)) unlink($path);
```
- **Purpose**: Removes the IMS notification flag when the user opens the IMS module.

#### Menu Setup
```php
$menu = [CMS_L_COMMAND_REFRESH . "|desktop/command_refresh" => NULL];
if ($list_receiver) $menu[CMS_L_DESKTOP_IMS_001 . "|desktop/command_create"] = "send";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|desktop/command_delete"] = "delete";
if ($filter_user || $filter_thread) $menu[CMS_L_COMMAND_PREVIOUS . "|desktop/command_cancel"] = "filter_reset";
```
- **Purpose**: Builds the top-level UI menu.
- **Dynamic Items**:
  - "New Message" only appears if there are valid receivers.
  - "Cancel Filter" appears only if a filter is active.

#### Message Retrieval and Grouping
```php
$filter = ["owner" => DESKTOP_USER];
if ($filter_thread) $filter["thread"] = $filter_thread;
$message = NULL;
$list_hash = NULL;
$ims->reset();
while ($ims->next($filter)) {
    $value = $ims->get();
    if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT) {
        if ((!$filter_user) || streq($value["receiver"], $filter_user)) {
            $hash = $value["hash"];
            if (!isset($list_hash[$hash])) $message[$value["time"] . $value["id"]] = $value;
            $list_hash[$hash][] = $value["receiver"];
        }
    } elseif ((!$filter_user) || streq($value["sender"], $filter_user)) {
        $message[$value["time"] . $value["id"]] = $value;
    }
}
```
- **Purpose**: Fetches and groups messages for display.
- **Inner Mechanisms**:
  - Groups sent messages by `hash` to avoid duplicate display.
  - Uses `time . id` as array key to ensure chronological sorting.
  - Applies user/thread filters.

#### Empty State
```php
if (!$message) {
    $ifc->set(CMS_L_DESKTOP_IMS_005, "info"); // "No messages found"
    $ifc->close();
}
```

#### Message Rendering
```php
ifc_table_open();
echo("<colgroup><col style=\"WIDTH:0\"><col style=\"WIDTH:200px\"><col></colgroup>");
krsort($message);
foreach ($message AS $value) {
    $varied = ifc_varied();
    if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT) {
        // Render sent message
    } else {
        // Render received message
    }
}
```
- **Purpose**: Renders messages in a table with alternating row colors.
- **Inner Mechanisms**:
  - `ifc_varied()`: Returns ` class="varied"` for alternating row styling.
  - `krsort()`: Sorts messages newest-first.
  - Uses `x()`, `qx()`, and `image()` for safe output and icon display.
  - Clickable links for filtering by user or thread.

#### Sent Message Rendering
```php
echo("<tr><td class=\"select\">");
$ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
echo("</td><td$varied>" . x(friendly_date($value["time"])) . " " . CMS_L_DESKTOP_IMS_013 . ":");
foreach ($receiver AS $_value)
    echo("<br><a href=\"javascript:ifc_post('filter_user','" . qx($_value) . "');\" title=\"" . CMS_L_DESKTOP_IMS_009 . "\">" .
         image("desktop/icon_receiver") . " " . (($_receiver = $data->get("user.$_value", "name")) ? $_receiver : $_value) . "</a>");
echo("</td><td$varied><a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" title=\"" . CMS_L_DESKTOP_IMS_010 . "\" class=\"br\">" .
     image("desktop/icon_message_sent") . " " . nl2br(x($value["text"])) . "</a></td></tr>");
```
- **Purpose**: Displays messages sent by the current user.
- **Features**:
  - Lists all receivers.
  - Clickable links to filter by user or thread.
  - `nl2br()` preserves line breaks in message text.

#### Received Message Rendering
```php
echo("<tr><td class=\"select\">");
$ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
echo("</td><td class=\"$class\">" . x(friendly_date($value["time"])) . " " . CMS_L_DESKTOP_IMS_014 . ":<br>" .
     "<a href=\"javascript:ifc_post('filter_user','" . qx($sender) . "');\" title=\"" . CMS_L_DESKTOP_IMS_009 . "\">" .
     image("desktop/icon_receiver") . " " . x($_sender) . "</a></td><td class=\"$class\">" .
     "<a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" title=\"" . CMS_L_DESKTOP_IMS_010 . "\" class=\"br\">" .
     image("desktop/icon_message") . " " . nl2br(x($value["text"])) . "</a>");
if (nstreq($sender, DESKTOP_USER))
    echo("<br><a href=\"javascript:ifc_post('reply','" . qx($value["id"]) . "');\">" .
         image("desktop/command_create") . " " . CMS_L_DESKTOP_IMS_011 . "</a>");
echo("</td></tr>");
```
- **Purpose**: Displays messages received by the current user.
- **Features**:
  - "Reply" link only appears if the sender is not the current user.
  - Uses `related` or `related2` CSS classes for visual grouping.

#### Selection Controls
```php
echo("<tr><td colspan=\"3\" class=\"select\" style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL, "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE, "button", "javascript:ifc_list_deactivate();");
echo("</td></tr>");
ifc_table_close();
$ifc->close();
```
- **Purpose**: Provides bulk selection controls (Select All, Invert, Deselect).
- **Inner Mechanisms**:
  - Uses `ifc_list_*` JavaScript functions for client-side list management.


<!-- HASH:40971ffb476e7bfc19afa77d199df52e -->
