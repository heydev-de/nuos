# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.core.inc

## Overview

The `core` class is the central communication relay for the PWNC Web Platform. It manages real-time user connectivity, channel membership, messaging, profile data, and status flags using file-based resource storage (`core_resource`). The class operates on four primary data resources:

- **index** — tracks currently connected users and their active channel
- **profile** — stores persistent user profile information (name, color, text, image)
- **channel** — stores permanent channel definitions (name, description, password)
- **status** — tracks per-user, per-channel status flags (banned, muted, owner, etc.)
- **data** — acts as a message buffer for inter-user and system messages

All operations are concurrency-safe through resource locking and position backup/restore patterns. The class is instantiated once per request and provides the foundation for chat, notifications, and presence features.

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PERMISSION_CONTROL` | `"control"` | Permission level for control access |
| `CMS_CORE_PERMISSION_OPERATOR` | `"operator"` | Permission level for operator/administrator access |

### General Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PORTAL` | `"Lobby"` | Default/lobby channel name |

### Status Flags (bitmask)

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_STATUS_NONE` | `0` | No status |
| `CMS_CORE_STATUS_OWNER` | `1` | Channel owner |
| `CMS_CORE_STATUS_OPERATOR` | `2` | Channel operator |
| `CMS_CORE_STATUS_SPY` | `4` | Spy mode (can see private messages) |
| `CMS_CORE_STATUS_INVISIBLE` | `8` | Invisible to other users |
| `CMS_CORE_STATUS_ABSENT` | `16` | Marked as absent/away |
| `CMS_CORE_STATUS_MUTE` | `32` | Muted (cannot send messages) |
| `CMS_CORE_STATUS_BANNED` | `64` | Banned from channel |

### Data Type Flags (bitmask)

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_DATA_DEFAULT` | `0` | Default message type |
| `CMS_CORE_DATA_SYSTEM` | `1` | System-generated message |
| `CMS_CORE_DATA_PRIVATE` | `2` | Private/direct message |
| `CMS_CORE_DATA_PRIVATE_META` | `4` | Private message metadata (receiver info) |
| `CMS_CORE_DATA_PRIVATE_DATA` | `8` | Private message content |
| `CMS_CORE_DATA_RESPONSE` | `16` | Response to sender |
| `CMS_CORE_DATA_SPY_META` | `32` | Spy metadata |
| `CMS_CORE_DATA_SPY_DATA` | `64` | Spy data content |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | Generic user ID; `"!username"` for permanent users, `CMS_USERID` for anonymous |
| `$timeout` | `int` | `15` | Connection timeout in seconds |
| `$enabled` | `bool` | `FALSE` | Whether the core is active for the executing user |
| `$operator` | `bool` | `FALSE` | Whether the executing user is an operator/administrator |
| `$index` | `core_resource\|NULL` | `NULL` | Resource for connected users index |
| `$profile` | `core_resource\|NULL` | `NULL` | Resource for user profiles |
| `$channel` | `core_resource\|NULL` | `NULL` | Resource for permanent channels |
| `$status` | `core_resource\|NULL` | `NULL` | Resource for user status flags |
| `$data` | `core_resource\|NULL` | `NULL` | Resource for message buffer |

## core

### __construct

Initializes the core communication relay by loading the resource library, generating a user GUID, setting up all data resources, checking user permissions and ban status, and performing periodic cleanup.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$timeout` | `int` | `15` | Connection timeout in seconds |
| `$datapath` | `string` | `"#core"` | Data path prefix for resource files |

**Return Value**

No explicit return. Sets up object properties. If the resource library cannot be loaded or the user is banned, `$this->enabled` remains `FALSE` and `$this->status` is set to `NULL`.

**Inner Mechanisms**

1. Loads the `core_resource` library via `cms_load()`.
2. Generates a GUID: uses `CMS_USERID` for anonymous users, or `"!username"` for permanent users.
3. Creates a `core_resource` for status tracking with fields: `guid` (string[41]), `channel` (string[20]), `status` (byte).
4. Checks if the user is banned via `get_status()` and `cms_permission()`. If banned and not an owner/operator, disables the core.
5. If enabled, creates resources for index, profile, channel, and data.
6. Performs cleanup every 15 minutes using `cms_cache()` to track the last cleanup time.

**Usage Example**

```php
$core = new core(30, "#core");
if ($core->enabled) {
    $core->connect("Alice");
    $core->send("Hello, world!");
}
```

### unique_name

Checks whether a given display name is available (not already used by another user).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | The display name to check |

**Return Value**

`bool` — `TRUE` if the name is available, `FALSE` if it is already in use or the core is disabled.

**Inner Mechanisms**

1. Returns `FALSE` immediately if the core is not enabled.
2. Backs up the profile resource offset.
3. Locks the profile resource for thread safety.
4. Cleans the name: strips spaces and truncates to 40 characters using `utf8_substr()`.
5. Iterates through all profiles with matching names, skipping the current user's own GUID.
6. If any other user has the same name, returns `FALSE`.
7. Restores the resource offset and unlocks.

**Usage Example**

```php
if ($core->unique_name("Alice")) {
    echo "Name is available!";
} else {
    echo "Name is taken.";
}
```

### connect

Registers the current user as connected, creates or retrieves their profile, sets initial status, and broadcasts a join message to the channel.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | `string` | `""` | Display name for guest users |

**Return Value**

`bool` — `TRUE` on successful connection, `FALSE` if the core is disabled, the user is already connected, or the name is unavailable.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Checks if the user is already connected by seeking their GUID in the index.
3. Locks index, profile, and status resources.
4. **Recurring user**: If a profile already exists for this GUID, reuses the stored name.
5. **Guest user** (anonymous): Cleans the name, checks availability via `unique_name()`, generates a random color, and creates a new profile entry.
6. **New permanent user**: Cleans `CMS_NAME`, makes it unique by appending `_1`, `_2`, etc. if needed, and creates a new profile.
7. Inserts the user into the index with their GUID and current timestamp.
8. If the user is an operator, sets `OWNER` and `OPERATOR` status flags.
9. Broadcasts a system message (`CMS_L_CORE_001`) announcing the user's arrival.
10. Restores resource positions and unlocks.

**Usage Example**

```php
$core->connect("Alice");
// User "Alice" is now connected and visible in the Lobby
```

### disconnect

Removes the current user from the index, optionally cleans up their profile and status, deletes pending messages, and broadcasts a leave message.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$clean` | `bool` | `TRUE` | Whether to delete the user's profile and status if not banned or permanent |

**Return Value**

`bool` — `TRUE` on successful disconnection, `FALSE` if the core is disabled or no user is found in the index.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Locks the index resource and retrieves the current user's GUID.
3. If no user is found, unlocks and returns `FALSE`.
4. Backs up all resource offsets.
5. Locks index, profile, status, and data resources.
6. Retrieves the user's current channel and profile name.
7. Temporarily switches to the disconnecting user's GUID to send a system leave message (`CMS_L_CORE_002`).
8. Deletes the user from the index.
9. Checks if the user is banned. If not banned and not a permanent user (GUID doesn't start with `!`), and `$clean` is `TRUE`, deletes their profile.
10. Deletes all messages addressed to this user from the data buffer.
11. Restores resource positions and unlocks.
12. Calls `update_status()` to clean up channel status.

**Usage Example**

```php
$core->disconnect();
// User is removed from the index and their profile is cleaned up
```

### get_status

Retrieves the combined status flags for a user, optionally including channel-specific status.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to the current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel name; if provided, channel-specific status is OR'd with global status |

**Return Value**

`int\|bool` — The combined status bitmask, or `FALSE` if the status resource is `NULL`.

**Inner Mechanisms**

1. Returns `FALSE` if `$this->status` is `NULL`.
2. Defaults `$guid` to the current user's GUID if not provided.
3. Backs up the status resource offset and locks it.
4. Initializes status to `CMS_CORE_STATUS_NONE` (0).
5. Seeks the global status (channel = `NULL`) for the given GUID and retrieves the status value.
6. If a channel is specified, seeks the channel-specific status and OR's it with the global status.
7. Restores the resource offset and unlocks.

**Usage Example**

```php
$status = $core->get_status(NULL, "general");
if (flag($status, CMS_CORE_STATUS_BANNED)) {
    echo "User is banned from this channel.";
}
```

### set_status

Sets or updates the status flags for a user in a specific channel (or globally if channel is `NULL`).

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$status` | `int` | `CMS_CORE_STATUS_NONE` | Status bitmask to set |
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to the current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel name; `NULL` for global status |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the status resource offset and locks it.
4. Uses `next()` with `TRUE` (create-if-not-exists) to find or create a status entry for the given GUID and channel.
5. Sets the status value.
6. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->set_status(CMS_CORE_STATUS_MUTE, $targetGuid, "general");
// Mutes the target user in the "general" channel
```

### delete_status

Removes all status entries for a given user across all channels.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to the current user's GUID |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the status resource offset and locks it.
4. Iterates through all status entries matching the GUID and deletes each one.
5. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->delete_status($userGuid);
// Removes all status flags for the user across all channels
```

### update_status

Cleans up status entries for a channel that no longer has any connected users or permanent channel definition.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name to check and clean up |

**Return Value**

`bool` — `TRUE` if the channel had no users and no permanent definition (status was cleaned), `FALSE` if the core is disabled, the channel is empty, or the channel still has users/permanent definition.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled or the channel is empty (`stre()` check).
2. Backs up resource offsets for index, channel, and status.
3. Locks all three resources.
4. Checks if any user is currently connected to the channel (via index seek).
5. Checks if a permanent channel definition exists (via channel seek).
6. If neither exists, deletes all status entries for that channel.
7. Restores resource positions and unlocks.

**Usage Example**

```php
$core->update_status("old_temp_channel");
// Removes all status entries for a channel that no longer has users
```

### update_index

Iterates through all connected users, updates the current user's timestamp, and disconnects users whose connection has timed out.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$query_connection` | `bool` | `FALSE` | If `TRUE`, skips updating the current user's timestamp |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled or timeout is 0.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled or `$this->timeout` is 0.
2. Backs up the index resource offset and locks it.
3. Calculates the timeout threshold: `time() - $this->timeout`.
4. Iterates through all connected users:
   - If the user is the current user and `$query_connection` is `FALSE`, updates their timestamp to the current time.
   - If any user's timestamp is older than the timeout threshold, calls `disconnect(FALSE)` to remove them.
5. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->update_index();
// Updates current user's timestamp and disconnects timed-out users
```

### connect_channel

Moves the current user to a different channel, performing permission checks, broadcasting join/leave messages, and creating temporary channel ownership if needed.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Target channel name |
| `$password` | `string` | `""` | Password for password-protected channels |

**Return Value**

`bool` — `TRUE` on successful channel switch, `FALSE` if the core is disabled, the user lacks permission, the channel requires a password and none was given, or the user is already in the target channel.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Backs up the channel resource offset and locks it.
3. **Permission check loop** (do-while with break):
   - Operators are always permitted.
   - Channel owners are always permitted.
   - Channel operators are always permitted.
   - If not banned: checks if a permanent channel exists, if it has a password, and if the given password matches.
   - If any check fails, unlocks and returns `FALSE`.
4. Backs up the index resource offset and locks it.
5. Retrieves the user's current channel.
6. If the target channel is the same as the current channel, returns `FALSE`.
7. Broadcasts a leave message (`CMS_L_CORE_004`) to the current channel.
8. **Temporary channel creation**: If the channel doesn't exist as permanent or temporary, grants `OWNER` and `OPERATOR` status to the current user for that channel.
9. Updates the user's channel in the index.
10. Broadcasts a join message (`CMS_L_CORE_003`) to the target channel.
11. Restores resource positions and unlocks.
12. Calls `update_status()` to clean up the previous channel.

**Usage Example**

```php
$core->connect_channel("general", "secret123");
// Moves user to the "general" channel, providing password if needed
```

### create_channel

Creates a new permanent channel with optional description and password. Only administrators can create channels.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Channel name |
| `$text` | `string` | `""` | Channel description |
| `$password` | `string` | `""` | Channel password |

**Return Value**

`bool` — `TRUE` on successful creation, `FALSE` if the core is disabled, the channel name is empty, the name is "Lobby", the user is not an operator, or the channel already exists.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Trims the channel name; returns `FALSE` if empty.
3. Returns `FALSE` if the channel name is "Lobby" (reserved).
4. Backs up the channel resource offset and locks it.
5. Checks that the user is an operator and that the channel doesn't already exist.
6. Creates the channel entry with name, description, and password.
7. Grants `OWNER` and `OPERATOR` status to the creator for the new channel.
8. Sends a system message (`CMS_L_CORE_005`) to the creator confirming creation.
9. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->create_channel("private_chat", "Private discussion", "mypassword");
// Creates a new permanent channel named "private_chat"
```

### set_channel

Updates the description and password of an existing permanent channel. Only administrators or channel owners can modify channels.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Channel name to modify |
| `$text` | `string` | `""` | New channel description |
| `$password` | `string` | `""` | New channel password |

**Return Value**

`bool` — `TRUE` on successful update, `FALSE` if the core is disabled, the channel doesn't exist, or the user lacks permission.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Backs up the channel resource offset and locks it.
3. Checks that the channel exists and that the user is an operator or the channel owner.
4. Updates the channel's text and password.
5. Sends a system message (`CMS_L_CORE_006`) to the creator confirming the update.
6. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->set_channel("general", "General discussion area", "");
// Updates the description of the "general" channel
```

### delete_channel

Deletes a permanent channel. Only administrators or channel owners can delete channels.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name to delete |

**Return Value**

`bool` — `TRUE` on successful deletion, `FALSE` if the core is disabled or the user lacks permission.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Backs up the channel resource offset and locks it.
3. Iterates through all channel entries matching the name.
4. For each match, checks if the user is an operator or the channel owner.
5. If permitted, deletes the channel entry.
6. Restores the resource offset and unlocks.
7. If any channel was deleted, sends a system message (`CMS_L_CORE_007`) to the creator and calls `update_status()` to clean up channel status.

**Usage Example**

```php
$core->delete_channel("old_discussion");
// Deletes the "old_discussion" permanent channel
```

### send

Broadcasts a message to all users in the current channel, with support for private messages, system messages, spy mode, and invisible senders.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | `string` | — | Message content |
| `$receiver` | `string\|array\|NULL` | `NULL` | Target user GUID(s) for private messages; `NULL` for channel broadcast |
| `$status` | `int` | `CMS_CORE_DATA_DEFAULT` | Message type flags (system, private, etc.) |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled or the sender is not connected.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Backs up the index resource offset and locks it.
3. Checks if the sender is connected; returns `FALSE` if not.
4. Retrieves the sender's current channel and status.
5. Determines if the sender has override privileges (operator, system message, channel owner, or channel operator).
6. If the sender is muted and doesn't have override privileges, returns `FALSE`.
7. If a receiver is specified, marks the message as private.
8. If the sender is invisible, other invisible users can still see the message.
9. Revokes the `ABSENT` status if this is a default message.
10. Backs up the data resource offset and locks it.
11. Iterates through all users in the current channel:
    - **Private message handling**: If the message is private, checks if the current user is the intended receiver, the sender, or a spy. System messages are skipped for non-matching receivers.
    - **Invisible sender handling**: If the sender is invisible, only the sender and other invisible users can see the message.
    - **Spy handling**: If the receiver is a spy, the message is split into metadata and data entries with spy flags.
    - **Message delivery**: Creates data entries for each recipient with appropriate status flags.
12. Restores resource positions and unlocks.

**Usage Example**

```php
// Broadcast to channel
$core->send("Hello, everyone!");

// Private message
$core->send("Secret message", $targetGuid);

// System message
$core->send("Server restarting", NULL, CMS_CORE_DATA_SYSTEM);
```

### receive

Retrieves all pending messages addressed to the current user from the data buffer, optionally deleting them after retrieval.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$delete` | `bool` | `TRUE` | Whether to delete messages after retrieval |

**Return Value**

`array` — Array of message data entries, each containing `guid`, `receiver`, `status`, and `data` fields. Returns an empty array if the core is disabled.

**Inner Mechanisms**

1. Returns an empty array if the core is disabled.
2. Backs up the data resource offset and locks it.
3. Resets the data resource to the beginning.
4. Iterates through all entries where the receiver matches the current user's GUID.
5. Collects each entry's data into the result array.
6. If `$delete` is `TRUE`, removes each message after collecting it.
7. Restores the resource offset and unlocks.

**Usage Example**

```php
$messages = $core->receive();
foreach ($messages as $msg) {
    echo "From: " . $msg['guid'] . " - " . $msg['data'] . "\n";
}
```

### get_profile

Retrieves the profile information for a user.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to the current user's GUID |

**Return Value**

`array\|bool` — Associative array with profile fields (`guid`, `name`, `color`, `text`, `reserved`, `image`) on success, or `FALSE` if the core is disabled or the profile is not found.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the profile resource offset and locks it.
4. Seeks the profile entry for the given GUID.
5. If found, retrieves all fields; otherwise returns `FALSE`.
6. Restores the resource offset and unlocks.

**Usage Example**

```php
$profile = $core->get_profile($userGuid);
if ($profile !== FALSE) {
    echo "Name: " . $profile['name'] . "\n";
    echo "Color: " . $profile['color'] . "\n";
}
```

### set_profile

Updates the profile information for a user, with permission checks and name uniqueness validation.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | — | User GUID to update |
| `$name` | `string` | — | Display name |
| `$color` | `string` | `""` | Hex color code (e.g., `#ff0000`) |
| `$text` | `string` | `""` | Profile text/bio |
| `$image` | `string` | `""` | Image filename |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled or the user lacks permission.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. **Permission check** (do-while with break):
   - Self is always permitted.
   - Non-operators can only modify their own profile.
   - Guests (GUID starts with `!`) are always permitted.
   - Administrators cannot be modified by non-administrators.
4. Backs up the profile resource offset and locks it.
5. Preserves current image and name values.
6. Cleans the name: strips spaces and truncates to 40 characters.
7. Validates the name: if empty or not unique, reverts to the previous name.
8. Validates the color: must match `/^#[0-9a-f]{6}$/i`; otherwise set to empty.
9. Handles image: if a new image file exists, removes the old one; otherwise keeps the current image.
10. Sets the profile entry with all fields.
11. Restores the resource offset and unlocks.
12. Sends a system message (`CMS_L_CORE_008`) to the user confirming the update.

**Usage Example**

```php
$core->set_profile(NULL, "Alice", "#ff0000", "Hello, I'm Alice!", "avatar.png");
// Updates the current user's profile
```

### delete_profile

Deletes a user's profile, including their image file.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to the current user's GUID |
| `$system` | `bool` | `FALSE` | System override flag; bypasses permission checks |

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled or the user lacks permission.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. **Permission check** (do-while with break):
   - System override (`$system = TRUE`) bypasses all checks.
   - Non-operators can only delete their own profile.
   - Guests (GUID starts with `!`) are always permitted.
   - Administrators cannot be deleted by non-administrators.
4. Backs up the profile resource offset and locks it.
5. Iterates through all profile entries matching the GUID.
6. For each match, removes the image file if it exists, then deletes the profile entry.
7. Restores the resource offset and unlocks.

**Usage Example**

```php
$core->delete_profile($userGuid);
// Deletes the specified user's profile and image
```

### status

Sets, clears, or tests status flags for a user, with comprehensive permission checks and notification broadcasting.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `int` | `CMS_CORE_STATUS_NONE` | Status flag to set/clear (positive to set, negative to clear) |
| `$guid` | `string\|NULL` | `NULL` | Target user GUID; defaults to the current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel name; `NULL` for global status |
| `$system` | `bool` | `FALSE` | System override; bypasses permission checks |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests whether the status change is possible without applying it |

**Return Value**

`bool` — `TRUE` if the status change is possible (or was applied), `FALSE` if the core is disabled, the status is already in the desired state, or permission is denied.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Defaults `$guid` to the current user's GUID.
3. Determines if this is a set (`$value > 0`) or clear operation.
4. Takes the absolute value of `$value` as the status flag.
5. Checks if the status is already in the desired state; returns `FALSE` if so.
6. **Permission checks** (if not `$system`):
   - **General check**: Self is always permitted; guests are permitted; administrators are protected.
   - **Specific checks by status type**:
     - `OWNER`/`OPERATOR`: Can only be applied to others; cannot be applied to muted/banned users; requires operator or channel owner privileges.
     - `SPY`/`INVISIBLE`: Can only be applied to self; requires operator privileges.
     - `ABSENT`: Can only be applied to self; always global.
     - `MUTE`/`BANNED`: Can only be applied to others; cannot be applied to owners/operators; requires operator, owner, or operator privileges.
7. If `$test` is `TRUE`, returns `TRUE` without applying changes.
8. Backs up resource offsets and locks index and status resources.
9. Computes the new status value.
10. Determines if the status change has an "effect" (visible impact) based on channel context:
    - Both target and current channel are portal (Lobby).
    - Only target channel is portal.
    - Target and current channels are the same and not portal.
11. Sends appropriate notification messages based on the status type:
    - `INVISIBLE` (set): Sends a false disconnect message to the channel.
    - `INVISIBLE` (clear): Sends a false connect message to the channel.
    - `BANNED` (set, with effect): Kicks the user from the channel or disconnects them entirely.
12. Saves the new status via `set_status()`.
13. Restores resource positions and unlocks.

**Usage Example**

```php
// Mute a user in the current channel
$core->status(CMS_CORE_STATUS_MUTE, $targetGuid, NULL);

// Test if banning is possible
if ($core->status(CMS_CORE_STATUS_BANNED, $targetGuid, NULL, FALSE, TRUE)) {
    $core->status(CMS_CORE_STATUS_BANNED, $targetGuid, NULL);
}
```

### clean

Performs garbage collection by removing profiles, statuses, and messages for users who are no longer connected and whose accounts no longer exist.

**Parameters**

None.

**Return Value**

`bool` — `TRUE` on success, `FALSE` if the core is disabled.

**Inner Mechanisms**

1. Returns `FALSE` if the core is disabled.
2. Backs up all resource offsets and locks index, profile, status, and data resources.
3. Builds a list of valid user accounts from:
   - The `permission` class's data (basic user accounts).
   - The database profile table (extended user accounts).
4. **Clean profiles**: Iterates through all profiles. For permanent users (GUID starts with `!`), checks if the account still exists. For all users, checks if they are currently connected. Removes orphaned profiles and their image files.
5. **Clean status**: Same logic as profiles — removes status entries for disconnected users whose accounts no longer exist.
6. **Clean data**: Removes all messages addressed to disconnected users.
7. Restores resource positions and unlocks.

**Usage Example**

```php
$core->clean();
// Removes orphaned profiles, statuses, and messages
```

### switch_guid

Temporarily switches the current user context to a different GUID, allowing operations to be performed as another user. Uses a static stack for nested switches.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | — | Target GUID to switch to; `NULL` with `$reset = TRUE` to restore |
| `$reset` | `bool` | `FALSE` | If `TRUE`, restores the previous GUID from the stack |

**Return Value**

No explicit return value.

**Inner Mechanisms**

1. If `$reset` is `TRUE`:
   - Pops the previous GUID and operator status from the static `$stack`.
   - Restores them to the current object.
2. If `$reset` is `FALSE`:
   - Pushes the current GUID and operator status onto the static `$stack`.
   - Sets the new GUID.
   - Recomputes the operator status: `TRUE` if the GUID starts with `!` and the user has operator permission.

**Usage Example**

```php
$core->switch_guid("!admin_user");
// Now operating as admin_user
$core->send("System message", NULL, CMS_CORE_DATA_SYSTEM);
$core->reset_guid();
// Back to original user
```

### reset_guid

Restores the previous user context after a `switch_guid()` call.

**Parameters**

None.

**Return Value**

No explicit return value.

**Inner Mechanisms**

Calls `switch_guid(NULL, TRUE)` to pop the previous GUID and operator status from the stack.

**Usage Example**

```php
$core->switch_guid("!admin_user");
$core->send("Important notice", NULL, CMS_CORE_DATA_SYSTEM);
$core->reset_guid();
// Operations now use the original user's context
```


<!-- HASH:d29101ce880700f173ebf8b3d1df3f95 -->
