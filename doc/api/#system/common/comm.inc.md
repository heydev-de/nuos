# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/comm.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview
The `comm.inc` file in the NUOS web platform provides utility functions for inter-module communication and system notifications. The primary function, `notify_admin`, is responsible for sending administrative notifications by creating a message in the internal messaging system (IMS) and flagging the admin inbox for new messages.

This function is part of the `cms` namespace and is designed to alert administrators about critical events or errors within the system.

---

## Functions

### `notify_admin($string)`

#### Purpose
Sends a notification to the system administrator by:
1. Creating a new message in the IMS (Internal Messaging System) core resource.
2. Flagging the admin inbox to indicate a new message.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | The notification text to be sent to the administrator.                     |

#### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the notification was successfully created and flagged.           |
| `FALSE`   | If the `core_resource` library fails to load.                              |

#### Inner Mechanisms
1. **Library Loading**:
   - Attempts to load the `core_resource` library. If loading fails, the function returns `FALSE`.

2. **Core Resource Initialization**:
   - Initializes a `core_resource` object (`$ims`) with a schema for IMS messages. The schema defines fields such as `id`, `thread`, `time`, `owner`, `receiver`, `sender`, `status`, `text`, and `hash`.

3. **Message Creation**:
   - Generates a unique thread ID (`unique_id(8)`) and timestamp (`time()`).
   - Prepends a system identifier (`CMS_L_COMMON_001`) to the notification text.
   - Creates a hash (`hash32`) for the message using the thread ID, timestamp, and notification text.

4. **Message Storage**:
   - Seeks an empty slot in the IMS core resource (`$ims->seek(["owner" => NULL], TRUE)`).
   - Sets the message fields with the generated values and stores the message.

5. **Flagging Admin Inbox**:
   - Opens (or creates) a flag file (`ims.flag`) in the admin's desktop directory.
   - Locks the file (`flock`), writes a value (`1`), and closes it to indicate a new message.

#### Usage Context
- **Error Handling**: Used to notify administrators about system errors, warnings, or critical events.
- **Debugging**: Can be invoked during debugging to log important system states or failures.
- **Automated Alerts**: Integrated into background processes (e.g., cron jobs) to alert admins about failures or completed tasks.

#### Typical Scenarios
1. **Exception Handling**:
   ```php
   try {
       // Critical operation
   } catch (Exception $e) {
       cms\notify_admin("Operation failed: " . $e->getMessage());
   }
   ```

2. **Background Tasks**:
   ```php
   if (! perform_maintenance()) {
       cms\notify_admin("Maintenance task failed at " . date("Y-m-d H:i:s"));
   }
   ```

3. **User Actions**:
   ```php
   if (user_action_requires_admin_attention($user_id)) {
       cms\notify_admin("User $user_id requires review.");
   }
   ```


<!-- HASH:6d8d99c79c3010acace0d35be04fb386 -->
