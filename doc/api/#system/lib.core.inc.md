# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core.inc)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The `core` class in `#system/lib.core.inc` is the **Communication Relay (Core)** class for the PWNC Web Platform. It serves as the central hub for real-time communication between users, managing connections, channels, messaging, user profiles, and status flags. It operates on file-based resource objects (`core_resource`) for persistent storage of connection state, profiles, channels, and message buffers.

The class implements a multi-user communication system with support for:
- Temporary and permanent channels
- User status flags (owner, operator, spy, invisible, absent, mute, banned)
- Private and system messaging
- Profile management with images
- Automatic cleanup of stale data

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PERMISSION_CONTROL` | `"control"` | Permission level for control access |
| `CMS_CORE_PERMISSION_OPERATOR` | `"operator"` | Permission level for operator access |

### General Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PORTAL` | `"Lobby"` | Default/temporary channel name (the lobby) |

### Status Constants (bit flags)

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_STATUS_NONE` | `0` | No status |
| `CMS_CORE_STATUS_OWNER` | `1` | Channel owner |
| `CMS_CORE_STATUS_OPERATOR` | `2` | Channel operator |
| `CMS_CORE_STATUS_SPY` | `4` | Spy mode (can see private messages) |
| `CMS_CORE_STATUS_INVISIBLE` | `8` | Invisible to other users |
| `CMS_CORE_STATUS_ABSENT` | `16` | User is absent/away |
| `CMS_CORE_STATUS_MUTE` | `32` | User is muted |
| `CMS_CORE_STATUS_BANNED` | `64` | User is banned |

### Data Type Constants (bit flags)

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_DATA_DEFAULT` | `0` | Default message type |
| `CMS_CORE_DATA_SYSTEM` | `16` | System message |
| `CMS_CORE_DATA_PRIVATE` | `2` | Private message |
| `CMS_CORE_DATA_PRIVATE_META` | `4` | Private message metadata |
| `CMS_CORE_DATA_PRIVATE_DATA` | `8` | Private message data |
| `CMS_CORE_DATA_RESPONSE` | `16` | Response to sender |
| `CMS_CORE_DATA_SPY_META` | `32` | Spy message metadata |
| `CMS_CORE_DATA_SPY_DATA` | `64` | Spy message data |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | Generic user ID; `"!username"` for permanent users, `CMS_USERID` for anonymous |
| `$timeout` | `int` | `15` | Connection timeout in seconds |
| `$enabled` | `bool` | `FALSE` | Whether the core is active for the executing user |
| `$operator` | `bool` | `FALSE` | Whether the executing user is an operator/administrator |
| `$index` | `core_resource\|NULL` | `NULL` | Connection index resource (tracks connected users) |
| `$profile` | `core_resource\|NULL` | `NULL` | User profile resource |
| `$channel` | `core_resource\|NULL` | `NULL` | Permanent channel resource |
| `$status` | `core_resource\|NULL` | `NULL` | User status resource |
| `$data` | `core_resource\|NULL` | `NULL` | Data exchange buffer resource |

## core

### __construct

Initializes the core communication relay by loading the resource library, generating a user GUID, setting up status tracking, checking permissions, and creating all resource objects.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$timeout` | `int` | `15` | Connection timeout in seconds |
| `$datapath` | `string` | `"#core"` | Data path prefix for resource files |

**Return Value:** `void` (implicitly returns `NULL` if resource library fails to load)

**Inner Mechanisms:**
1. Loads the `core_resource` library via `cms_load()`. If it fails, returns early.
2. Generates a GUID: uses `CMS_USERID` for anonymous users, or `"!username"` for permanent users.
3. Creates a `core_resource` for status tracking with fields: `guid` (string[41]), `channel` (_string[20]), `status` (byte).
4. Checks if the user is banned via `get_status()` and `flag()`. If banned and not an owner/operator, sets `$status` to `NULL` and returns (core disabled).
5. If not banned, sets `$enabled = TRUE` and creates resource objects for index, profile, channel, and data.
6. Runs cleanup every 15 minutes via `cms_cache()` and `clean()`.

**Usage Example:**
```php
$core = new core(30, "#core");
if ($core->enabled) {
    // User is allowed to use the communication relay
    $core->connect("Alice");
}
```

### unique_name

Checks if a given display name is available (not already in use by another user).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | The name to check for uniqueness |

**Return Value:** `bool` — `TRUE` if the name is available, `FALSE` if it's already taken

**Inner Mechanisms:**
1. Returns `FALSE` immediately if core is not enabled.
2. Backs up the profile resource offset.
3. Locks the profile resource to prevent race conditions.
4. Cleans the name: strips spaces and truncates to 40 characters using `utf8_substr()`.
5. Iterates through all profiles checking for a matching name.
6. Skips the current user's own profile.
7. If a match is found, sets result to `FALSE` and breaks.
8. Unlocks the resource and restores the offset.

**Usage Example:**
```php
if ($core->unique_name("Alice")) {
    echo "Name 'Alice' is available!";
} else {
    echo "Name 'Alice' is already taken.";
}
```

### connect

Connects a user to the communication relay, creating their profile and index entry.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | `string` | `""` | Display name for the user (used for guests) |

**Return Value:** `bool` — `TRUE` on successful connection, `FALSE` if already connected or name is taken

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up index and profile resource offsets.
3. Checks if user is already connected (GUID exists in index). If so, returns `FALSE`.
4. Locks index, profile, and status resources.
5. **Recurring user:** If profile exists, reuses the stored name.
6. **Guest (anonymous):** Cleans the name, checks uniqueness via `unique_name()`. If available, generates a random color and creates a profile. If name is taken, returns `FALSE`.
7. **New user:** Cleans `CMS_NAME`, makes it unique by appending `_1`, `_2`, etc. if needed, and creates a profile.
8. Finds a free index slot and inserts the user with current timestamp.
9. If the user is an operator, sets OWNER and OPERATOR status flags.
10. Sends a system message announcing the user's arrival.
11. Unlocks resources and restores offsets.

**Usage Example:**
```php
// Connect a guest user with display name "Bob"
if ($core->connect("Bob")) {
    echo "Connected successfully!";
} else {
    echo "Connection failed - name taken or already connected.";
}
```

### disconnect

Disconnects a user from the communication relay, cleaning up their index entry, profile (if not banned/permanent), and messages.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$clean` | `bool` | `TRUE` | Whether to clean up the user's profile |

**Return Value:** `bool` — `TRUE` on successful disconnection, `FALSE` if no user is connected

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Locks the index resource and checks if a user is connected. If not, returns `FALSE`.
3. Backs up all resource offsets.
4. Locks index, profile, status, and data resources.
5. Retrieves the user's channel and profile name.
6. Temporarily switches to the disconnecting user's GUID to send a disconnect system message.
7. Deletes the user's index entry.
8. Checks if the user is banned. If not banned and not a permanent user (GUID doesn't start with `!`), deletes their profile.
9. Deletes all messages addressed to the disconnecting user.
10. Unlocks all resources and restores offsets.
11. Calls `update_status()` to clean up channel status.

**Usage Example:**
```php
// Disconnect the current user
if ($core->disconnect()) {
    echo "Disconnected successfully.";
}
```

### get_status

Retrieves the combined status flags for a user, optionally including channel-specific status.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel name for channel-specific status |

**Return Value:** `int` — Combined status bit flags, or `FALSE` if status resource is `NULL`

**Inner Mechanisms:**
1. Returns `FALSE` if `$this->status` is `NULL`.
2. Defaults `$guid` to the current user's GUID if not provided.
3. Backs up the status resource offset and locks it.
4. Initializes status to `CMS_CORE_STATUS_NONE` (0).
5. Retrieves the user's global status (channel = `NULL`).
6. If a channel is specified, retrieves and ORs the channel-specific status.
7. Unlocks the resource and restores the offset.

**Usage Example:**
```php
$status = $core->get_status(NULL, "general");
if (flag($status, CMS_CORE_STATUS_BANNED)) {
    echo "User is banned in the 'general' channel.";
}
```

### set_status

Sets the status flags for a user in a specific channel or globally.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$status` | `int` | `CMS_CORE_STATUS_NONE` | Status bit flags to set |
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel name; `NULL` for global status |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the status resource offset and locks it.
4. Uses `next()` with `TRUE` (create if not exists) to find or create the status entry.
5. Sets the status fields.
6. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Set the current user as absent
$core->set_status(CMS_CORE_STATUS_ABSENT);
```

### delete_status

Removes all status entries for a given user.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the status resource offset and locks it.
4. Iterates through all status entries matching the GUID and deletes each one.
5. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Clear all status for a user
$core->delete_status("!admin_user");
```

### update_status

Cleans up status entries for a channel that no longer has any connected users or permanent channel definition.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name to clean up |

**Return Value:** `bool` — `TRUE` if cleanup was performed, `FALSE` if core is not enabled or channel is empty

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled or channel is empty (`stre()` check).
2. Backs up index, channel, and status resource offsets.
3. Locks all three resources.
4. Checks if the channel exists in either the index (temporary) or channel (permanent) resources.
5. If the channel doesn't exist in either, deletes all status entries for that channel.
6. Unlocks resources and restores offsets.

**Usage Example:**
```php
// Clean up status for a deleted channel
$core->update_status("old_channel");
```

### update_index

Updates the connection index by refreshing the current user's timestamp and disconnecting users who have timed out.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$query_connection` | `bool` | `FALSE` | If `TRUE`, only queries connection status without updating |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled or timeout is 0

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled or timeout is 0.
2. Backs up the index resource offset and locks it.
3. Calculates the timeout threshold: `time() - $this->timeout`.
4. Iterates through all connected users:
   - If not querying and the user is the current user, updates their timestamp.
   - If a user's timestamp is below the timeout threshold, calls `disconnect(FALSE)` to remove them.
5. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Refresh connection timestamps and clean up timed-out users
$core->update_index();
```

### connect_channel

Moves the current user to a different channel, with permission checks and channel creation for temporary channels.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Target channel name |
| `$password` | `string` | `""` | Password for password-protected channels |

**Return Value:** `bool` — `TRUE` on successful channel switch, `FALSE` on failure

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up the channel resource offset and locks it.
3. **Permission check loop:**
   - Operators always permitted.
   - Channel owners always permitted.
   - Channel operators always permitted.
   - Banned users are denied.
   - For non-banned users: checks if the channel exists as a permanent channel. If it has a password, verifies the provided password.
4. If permission denied, unlocks and returns `FALSE`.
5. Backs up the index resource offset and locks it.
6. Retrieves the current channel. If target equals current, returns `FALSE` (no change).
7. Sends a system message to the current channel announcing departure.
8. **Channel creation for temporary channels:** If the channel doesn't exist as permanent or temporary, grants OWNER and OPERATOR status to the current user for that channel.
9. Updates the user's channel in the index.
10. Sends a system message to the target channel announcing arrival.
11. Unlocks resources, restores offsets, and calls `update_status()` for the previous channel.

**Usage Example:**
```php
// Switch to the "general" channel
if ($core->connect_channel("general")) {
    echo "Joined 'general' channel.";
}

// Join a password-protected channel
$core->connect_channel("secret", "mypassword");
```

### create_channel

Creates a new permanent channel.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Channel name |
| `$text` | `string` | `""` | Channel description/text |
| `$password` | `string` | `""` | Channel password |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled, name is empty, name is "Lobby", or user is not an operator

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Trims the channel name with `utf8_trim()`. Returns `FALSE` if empty.
3. Returns `FALSE` if the channel name is "Lobby" (reserved).
4. Backs up the channel resource offset and locks it.
5. Checks that the user is an operator AND the channel doesn't already exist.
6. If conditions met, creates the channel entry with name, text, and password.
7. Grants OWNER and OPERATOR status for the new channel.
8. Sends a system message to the user confirming creation.
9. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Create a new permanent channel (operator only)
if ($core->create_channel("projects", "Project discussions", "proj123")) {
    echo "Channel 'projects' created.";
}
```

### set_channel

Updates an existing permanent channel's text and password.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$channel` | `string` | — | Channel name to update |
| `$text` | `string` | `""` | New channel description/text |
| `$password` | `string` | `""` | New channel password |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled, channel doesn't exist, or user lacks permission

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up the channel resource offset and locks it.
3. Checks that the channel exists AND the user is either an operator or the channel owner.
4. If conditions met, updates the channel's text and password.
5. Sends a system message to the user confirming the update.
6. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Update channel description (operator or owner only)
if ($core->set_channel("general", "General discussion forum")) {
    echo "Channel updated.";
}
```

### delete_channel

Deletes a permanent channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name to delete |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled or user lacks permission

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up the channel resource offset and locks it.
3. Iterates through all channel entries matching the name.
4. For each match, checks if the user is an operator or the channel owner.
5. If permitted, deletes the channel entry.
6. Unlocks the resource and restores the offset.
7. If deletion occurred, sends a system message to the user and calls `update_status()` to clean up channel status.

**Usage Example:**
```php
// Delete a permanent channel (operator or owner only)
if ($core->delete_channel("old_projects")) {
    echo "Channel deleted.";
}
```

### send

Sends a message to all users in the current channel, with support for private messages, system messages, and spy functionality.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | `string` | — | Message content |
| `$receiver` | `string\|array\|NULL` | `NULL` | Target receiver(s); `NULL` for channel broadcast |
| `$status` | `int` | `CMS_CORE_DATA_DEFAULT` | Message type flags |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled or sender is not connected

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up the index resource offset and locks it.
3. Checks if the sender is connected. If not, returns `FALSE`.
4. Retrieves the sender's channel and status.
5. Determines if the sender has override privileges (operator, system message, owner, or operator status).
6. Checks if the sender is muted. If muted and no override, returns `FALSE`.
7. If a receiver is specified, marks the message as private.
8. Backs up the data resource offset and locks it.
9. Iterates through all users in the current channel:
   - **Private message handling:** If the message is private, checks if the current user is the receiver, the sender, or a spy. Spies get metadata and data copies.
   - **Invisible sender handling:** If the sender is invisible, only the sender and other invisible users can see the message.
   - **Message placement:** For spies/senders, places metadata and data entries. For regular recipients, places a single message entry with appropriate status flags.
10. Unlocks resources and restores offsets.

**Usage Example:**
```php
// Send a message to the current channel
$core->send("Hello, everyone!");

// Send a private message to a specific user
$core->send("Secret info", "user_guid");

// Send a system message
$core->send("Server maintenance in 5 minutes", NULL, CMS_CORE_DATA_SYSTEM);
```

### receive

Retrieves all messages addressed to the current user.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$delete` | `bool` | `TRUE` | Whether to delete messages after retrieval |

**Return Value:** `array` — Array of message data arrays, or `FALSE` if core is not enabled

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Initializes an empty data array.
3. Backs up the data resource offset and locks it.
4. Iterates through all data entries where the receiver matches the current user's GUID.
5. For each match, adds the full data entry to the result array.
6. If `$delete` is `TRUE`, removes the message after retrieval.
7. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Retrieve and delete all messages for the current user
$messages = $core->receive();
foreach ($messages as $msg) {
    echo "From: " . $msg['guid'] . " - " . $msg['data'] . "\n";
}
```

### get_profile

Retrieves a user's profile information.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |

**Return Value:** `array\|bool` — Profile data array on success, `FALSE` if not found or core is not enabled

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. Backs up the profile resource offset and locks it.
4. Seeks the profile entry matching the GUID.
5. If found, returns the full profile data array. If not found, returns `FALSE`.
6. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Get the current user's profile
$profile = $core->get_profile();
if ($profile) {
    echo "Name: " . $profile['name'] . "\n";
    echo "Color: " . $profile['color'] . "\n";
}
```

### set_profile

Updates a user's profile information (name, color, text, image).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |
| `$name` | `string` | — | Display name |
| `$color` | `string` | `""` | Hex color code (e.g., `#ff0000`) |
| `$text` | `string` | `""` | Profile text/bio |
| `$image` | `string` | `""` | Image filename |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled or permission denied

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. **Permission check:** Self is always permitted. Non-self requires operator status. Guests (GUID starts with `!`) are permitted. Administrators of the target user are untouchable.
4. Backs up the profile resource offset and locks it.
5. Preserves current image and name settings.
6. Cleans the name: strips spaces, truncates to 40 characters.
7. Verifies name: if empty or not unique, reverts to the current name.
8. Verifies color: must match `#rrggbb` pattern; otherwise set to empty.
9. Handles image: if a new image file exists, removes the old one. Otherwise keeps the current image.
10. Sets the profile data.
11. Unlocks the resource, restores the offset, and sends a system message to the user.

**Usage Example:**
```php
// Update the current user's profile
$core->set_profile(NULL, "Alice", "#ff0000", "Hello, I'm Alice!", "avatar.png");
```

### delete_profile

Deletes a user's profile and associated image.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | User GUID; defaults to current user's GUID |
| `$system` | `bool` | `FALSE` | System override flag (bypasses permission checks) |

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled or permission denied

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. **Permission check:** System override bypasses checks. Non-system requires operator status. Self is always permitted. Guests are permitted. Administrators of the target user are untouchable.
4. Backs up the profile resource offset and locks it.
5. Iterates through all profile entries matching the GUID.
6. For each match, removes the associated image file if it exists.
7. Deletes the profile entry.
8. Unlocks the resource and restores the offset.

**Usage Example:**
```php
// Delete a user's profile (operator only, or self)
$core->delete_profile("!old_user");
```

### status

Sets, clears, or tests a status flag for a user, with comprehensive permission checking and notification.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `int` | `CMS_CORE_STATUS_NONE` | Status flag to set/clear |
| `$guid` | `string\|NULL` | `NULL` | Target user GUID; defaults to current user's GUID |
| `$channel` | `string\|NULL` | `NULL` | Channel context; `NULL` for global |
| `$system` | `bool` | `FALSE` | System override (bypasses permission checks) |
| `$test` | `bool` | `FALSE` | If `TRUE`, only tests if the status change is possible |

**Return Value:** `bool` — `TRUE` if the status change is possible/successful, `FALSE` otherwise

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Defaults `$guid` to the current user's GUID.
3. Determines if this is a set (`$value > 0`) or clear operation.
4. Takes the absolute value of `$value` for the status flag.
5. Checks if the status is already in the desired state. If so, returns `FALSE`.
6. **Permission checks (if not system override):**
   - **General check:** Self is always permitted. Guests are permitted. Administrators of the target user are untouchable.
   - **Specific checks by status type:**
     - **OWNER/OPERATOR:** Can only be applied to others. Cannot be applied to muted or banned users. Can be applied by administrators or channel owners.
     - **SPY/INVISIBLE:** Can only be applied to self. Can be applied by administrators.
     - **ABSENT:** Can only be applied to self. Always global (channel set to `NULL`).
     - **MUTE/BANNED:** Can only be applied to others. Cannot be applied to owners or operators. Can be applied by administrators, channel owners, or channel operators.
7. If `$test` is `TRUE`, returns `TRUE` without making changes.
8. Locks index and status resources.
9. Computes the new status value.
10. Determines if the status change has an "effect" on the current channel (complex logic involving portal vs. non-portal channels).
11. Sends appropriate notification messages based on the status type.
12. For BANNED status with effect, kicks the user out of the channel or disconnects them.
13. Saves the new status via `set_status()`.
14. For INVISIBLE status being cleared with effect, sends a false connect message.
15. Unlocks resources and restores offsets.

**Usage Example:**
```php
// Mute a user in the current channel (owner/operator only)
$core->status(CMS_CORE_STATUS_MUTE, "user_guid");

// Set yourself as absent
$core->status(CMS_CORE_STATUS_ABSENT);

// Test if you can ban a user
if ($core->status(CMS_CORE_STATUS_BANNED, "user_guid", NULL, FALSE, TRUE)) {
    echo "Ban is possible.";
}
```

### clean

Performs garbage collection by removing stale profiles, statuses, and messages for users who are no longer connected and don't have permanent accounts.

**Parameters:** None

**Return Value:** `bool` — `TRUE` on success, `FALSE` if core is not enabled

**Inner Mechanisms:**
1. Returns `FALSE` if core is not enabled.
2. Backs up all resource offsets and locks all resources.
3. Builds a list of valid user accounts from:
   - The `permission` class's data (basic user accounts)
   - The database profile table (extended user accounts)
4. **Clean profiles:** Iterates through all profiles. For permanent users (GUID starts with `!`), checks if the account still exists. For all users, checks if they're currently connected. Removes orphaned profiles and their image files.
5. **Clean status:** Same logic as profiles — removes status entries for disconnected users whose accounts no longer exist.
6. **Clean data:** Removes all messages addressed to disconnected users.
7. Unlocks all resources and restores offsets.

**Usage Example:**
```php
// Run garbage collection (typically called automatically every 15 minutes)
$core->clean();
```

### switch_guid

Temporarily switches the current user context to a different GUID, allowing operations on behalf of another user.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$guid` | `string\|NULL` | `NULL` | Target GUID to switch to; `NULL` with `$reset = TRUE` to restore |
| `$reset` | `bool` | `FALSE` | If `TRUE`, restores the previous GUID from the stack |

**Return Value:** `void`

**Inner Mechanisms:**
1. Uses a static `$stack` array to maintain a stack of previous GUID/operator pairs.
2. **Reset mode (`$reset = TRUE`):** Pops the last entry from the stack and restores `$this->guid` and `$this->operator`.
3. **Switch mode:** Pushes the current `$this->guid` and `$this->operator` onto the stack, then sets the new GUID. The operator status is recalculated based on whether the new GUID is a permanent user with operator permissions.

**Usage Example:**
```php
// Temporarily operate as another user
$core->switch_guid("!admin_user");
$core->send("System message from admin");
$core->reset_guid(); // Restore original context
```

### reset_guid

Restores the previous user context after a `switch_guid()` call.

**Parameters:** None

**Return Value:** `void`

**Inner Mechanisms:**
Calls `switch_guid(NULL, TRUE)` to pop the previous GUID/operator pair from the stack and restore it.

**Usage Example:**
```php
$core->switch_guid("!admin_user");
// ... perform operations as admin ...
$core->reset_guid(); // Back to original user
```


<!-- HASH:e810519ffd02f5fb455c79eb8d7e3b91 -->
