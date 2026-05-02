# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.core.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Core Communication Relay Class

The `core` class serves as the central communication hub for the NUOS platform, managing real-time user interactions, channel operations, and status tracking. It handles user connections, messaging, profile management, and channel administration with fine-grained permission controls.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| **Permission** | | |
| `CMS_CORE_PERMISSION_CONTROL` | `"control"` | Permission identifier for control-level access. |
| `CMS_CORE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for operator-level access. |
| **Various** | | |
| `CMS_CORE_PORTAL` | `"Lobby"` | Default channel name for unassigned users. |
| **Status Flags** | | |
| `CMS_CORE_STATUS_NONE` | `0` | No status flags set. |
| `CMS_CORE_STATUS_OWNER` | `1` | User is the owner of a channel. |
| `CMS_CORE_STATUS_OPERATOR` | `2` | User is an operator in a channel. |
| `CMS_CORE_STATUS_SPY` | `4` | User can see private messages. |
| `CMS_CORE_STATUS_INVISIBLE` | `8` | User is invisible to others. |
| `CMS_CORE_STATUS_ABSENT` | `16` | User is marked as absent. |
| `CMS_CORE_STATUS_MUTE` | `32` | User is muted (cannot send messages). |
| `CMS_CORE_STATUS_BANNED` | `64` | User is banned from the system. |
| **Data Types** | | |
| `CMS_CORE_DATA_DEFAULT` | `0` | Default message type (public). |
| `CMS_CORE_DATA_SYSTEM` | `1` | System-generated message. |
| `CMS_CORE_DATA_PRIVATE` | `2` | Private message. |
| `CMS_CORE_DATA_PRIVATE_META` | `4` | Metadata for private messages (e.g., recipient list). |
| `CMS_CORE_DATA_PRIVATE_DATA` | `8` | Content of private messages. |
| `CMS_CORE_DATA_RESPONSE` | `16` | Message is a response to the sender. |
| `CMS_CORE_DATA_SPY_META` | `32` | Metadata for spies (e.g., recipient list). |
| `CMS_CORE_DATA_SPY_DATA` | `64` | Content visible to spies. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `guid` | `NULL` | Unique identifier for the current user (e.g., `"!admin"` for registered users, `"anonymous"` for guests). |
| `timeout` | `15` | Connection timeout in seconds. |
| `enabled` | `FALSE` | Indicates if the core is active for the executing user. |
| `operator` | `FALSE` | Indicates if the user has operator privileges. |
| `index` | `NULL` | `core_resource` object for tracking connected users. |
| `profile` | `NULL` | `core_resource` object for user profiles. |
| `channel` | `NULL` | `core_resource` object for permanent channels. |
| `status` | `NULL` | `core_resource` object for user status flags. |
| `data` | `NULL` | `core_resource` object for message exchange. |

---

### Constructor
#### `__construct($timeout = 15, $datapath = "#core")`

**Purpose:**
Initializes the core communication system, loads required resources, and validates user permissions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$timeout` | `int` | Connection timeout in seconds. |
| `$datapath` | `string` | Relative path to core data files (e.g., `"#core"`). |

**Return Values:**
- `void`: No explicit return, but sets `$this->enabled` to `FALSE` if initialization fails.

**Inner Mechanisms:**
1. Loads the `core_resource` library.
2. Generates a `guid` for the user (prefixed with `!` for registered users).
3. Initializes `core_resource` objects for `status`, `index`, `profile`, `channel`, and `data`.
4. Checks if the user is banned or lacks permissions, disabling the core if necessary.
5. Schedules a cleanup task every 15 minutes.

**Usage:**
- Called automatically during system initialization.
- Example:
  ```php
  $core = new \cms\core(30); // 30-second timeout
  ```

---

### Methods

---

#### `unique_name($name)`

**Purpose:**
Checks if a username is available (not already in use by another user).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Username to check. |

**Return Values:**
- `bool`: `TRUE` if the name is available, `FALSE` otherwise.

**Inner Mechanisms:**
1. Locks the `profile` resource to prevent race conditions.
2. Iterates through all profiles to check for name collisions.
3. Skips the current user's profile.

**Usage:**
- Used during user registration or profile updates.
- Example:
  ```php
  if ($core->unique_name("Alice")) {
      // Name is available
  }
  ```

---

#### `connect($name = "")`

**Purpose:**
Connects a user to the system, creating or updating their profile.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Username for guests. Registered users use their stored name. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., name taken, already connected).

**Inner Mechanisms:**
1. Checks if the user is already connected.
2. For guests:
   - Generates a random color if the name is available.
   - Creates a new profile.
3. For registered users:
   - Retrieves their stored profile.
   - Makes the name unique if necessary (e.g., `"Alice_1"`).
4. Adds the user to the `index` and sets their status.
5. Sends a system message to the channel.

**Usage:**
- Called when a user logs in or joins a channel.
- Example:
  ```php
  $core->connect("Guest123");
  ```

---

#### `disconnect($clean = TRUE)`

**Purpose:**
Disconnects a user from the system, optionally cleaning up their profile.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$clean` | `bool` | If `TRUE`, deletes the profile for non-banned guests. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Removes the user from the `index`.
2. Deletes their profile if they are a guest and not banned.
3. Deletes all pending messages for the user.
4. Updates the channel status.

**Usage:**
- Called when a user logs out or is kicked.
- Example:
  ```php
  $core->disconnect(); // Clean up profile
  ```

---

#### `get_status($guid = NULL, $channel = NULL)`

**Purpose:**
Retrieves the combined status flags for a user in a specific channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Defaults to the current user. |
| `$channel` | `string` | Channel name. If `NULL`, returns global status. |

**Return Values:**
- `int`: Bitmask of status flags (e.g., `CMS_CORE_STATUS_OWNER | CMS_CORE_STATUS_MUTE`).

**Inner Mechanisms:**
1. Combines global and channel-specific status flags using bitwise OR.

**Usage:**
- Used to check permissions or visibility.
- Example:
  ```php
  $status = $core->get_status("!admin", "General");
  if (flag($status, CMS_CORE_STATUS_OWNER)) {
      // User is an owner
  }
  ```

---

#### `set_status($status = CMS_CORE_STATUS_NONE, $guid = NULL, $channel = NULL)`

**Purpose:**
Sets the status flags for a user in a specific channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$status` | `int` | Bitmask of status flags. |
| `$guid` | `string` | User identifier. Defaults to the current user. |
| `$channel` | `string` | Channel name. If `NULL`, sets global status. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Creates or updates the status entry for the user/channel combination.

**Usage:**
- Used internally by `status()` to persist status changes.
- Example:
  ```php
  $core->set_status(CMS_CORE_STATUS_MUTE, "!user123", "General");
  ```

---

#### `delete_status($guid = NULL)`

**Purpose:**
Deletes all status entries for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Defaults to the current user. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Iterates through all status entries and deletes those matching the `guid`.

**Usage:**
- Called when a user is banned or leaves the system.
- Example:
  ```php
  $core->delete_status("!user123");
  ```

---

#### `update_status($channel)`

**Purpose:**
Removes all status entries for a channel if it no longer exists.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

**Return Values:**
- `bool`: `TRUE` if the channel was deleted, `FALSE` otherwise.

**Inner Mechanisms:**
1. Checks if the channel exists in either the `index` (temporary) or `channel` (permanent) resources.
2. Deletes all status entries for the channel if it does not exist.

**Usage:**
- Called when a channel is deleted or all users leave.
- Example:
  ```php
  $core->update_status("OldChannel");
  ```

---

#### `update_index($query_connection = FALSE)`

**Purpose:**
Updates the connection timestamps for all users and disconnects those who have timed out.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$query_connection` | `bool` | If `TRUE`, skips updating the current user's timestamp. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Iterates through all connected users.
2. Updates the current user's timestamp unless `$query_connection` is `TRUE`.
3. Disconnects users whose last activity exceeds the timeout.

**Usage:**
- Called periodically to maintain active connections.
- Example:
  ```php
  $core->update_index(); // Update all timestamps
  ```

---

#### `connect_channel($channel, $password = "")`

**Purpose:**
Moves a user to a different channel, validating permissions and passwords.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Target channel name. Use `NULL` for the portal. |
| `$password` | `string` | Password for protected channels. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., invalid password, banned).

**Inner Mechanisms:**
1. Validates permissions (administrators, owners, or operators can bypass restrictions).
2. Checks if the user is banned from the channel.
3. Validates the password for protected channels.
4. Notifies the current and target channels of the move.
5. Grants owner/operator status if the channel is new.

**Usage:**
- Called when a user switches channels.
- Example:
  ```php
  $core->connect_channel("General", "secret123");
  ```

---

#### `create_channel($channel, $text = "", $password = "")`

**Purpose:**
Creates a new permanent channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | Channel description. |
| `$password` | `string` | Password for the channel. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., channel exists, invalid name).

**Inner Mechanisms:**
1. Validates that the channel name is not reserved (e.g., `"Lobby"`).
2. Ensures the user is an administrator.
3. Creates the channel and grants owner/operator status to the creator.

**Usage:**
- Called by administrators to create new channels.
- Example:
  ```php
  $core->create_channel("Support", "User support channel", "support123");
  ```

---

#### `set_channel($channel, $text = "", $password = "")`

**Purpose:**
Updates the description or password of an existing channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | New description. |
| `$password` | `string` | New password. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., no permissions).

**Inner Mechanisms:**
1. Validates that the user is an administrator or the channel owner.

**Usage:**
- Called by administrators or owners to modify channels.
- Example:
  ```php
  $core->set_channel("General", "Main discussion channel", "newpass123");
  ```

---

#### `delete_channel($channel)`

**Purpose:**
Deletes a permanent channel and cleans up its status entries.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., no permissions).

**Inner Mechanisms:**
1. Validates that the user is an administrator or the channel owner.
2. Deletes the channel and all associated status entries.

**Usage:**
- Called by administrators or owners to remove channels.
- Example:
  ```php
  $core->delete_channel("OldChannel");
  ```

---

#### `send($data, $receiver = NULL, $status = CMS_CORE_DATA_DEFAULT)`

**Purpose:**
Sends a message to one or more users in the current channel.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$data` | `string` | Message content. |
| `$receiver` | `string|array` | Recipient `guid` or array of `guid`s. If `NULL`, sends to all users in the channel. |
| `$status` | `int` | Message type (e.g., `CMS_CORE_DATA_PRIVATE`, `CMS_CORE_DATA_SYSTEM`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., muted, not connected).

**Inner Mechanisms:**
1. Validates that the sender is connected and not muted.
2. For private messages:
   - Sends metadata (recipient list) and content separately.
   - Allows spies to see private messages.
3. Handles invisible users by skipping non-recipients.
4. Stores messages in the `data` resource.

**Usage:**
- Called to send public or private messages.
- Example:
  ```php
  $core->send("Hello, world!"); // Public message
  $core->send("Secret", "!user123", CMS_CORE_DATA_PRIVATE); // Private message
  ```

---

#### `receive($delete = TRUE)`

**Purpose:**
Retrieves all pending messages for the current user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$delete` | `bool` | If `TRUE`, deletes messages after retrieval. |

**Return Values:**
- `array`: List of messages, each containing `guid`, `receiver`, `status`, and `data`.

**Inner Mechanisms:**
1. Locks the `data` resource to prevent race conditions.
2. Collects all messages addressed to the current user.

**Usage:**
- Called to fetch messages for display.
- Example:
  ```php
  $messages = $core->receive();
  foreach ($messages as $message) {
      echo $message["data"];
  }
  ```

---

#### `get_profile($guid = NULL)`

**Purpose:**
Retrieves the profile information for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Defaults to the current user. |

**Return Values:**
- `array|bool`: Associative array of profile data (e.g., `name`, `color`, `image`) or `FALSE` if not found.

**Inner Mechanisms:**
1. Locks the `profile` resource to ensure consistency.

**Usage:**
- Called to display user information.
- Example:
  ```php
  $profile = $core->get_profile("!admin");
  echo $profile["name"];
  ```

---

#### `set_profile($guid, $name, $color = "", $text = "", $image = "")`

**Purpose:**
Updates the profile information for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Defaults to the current user. |
| `$name` | `string` | New username. |
| `$color` | `string` | Hex color code (e.g., `"#FF0000"`). |
| `$text` | `string` | Profile description. |
| `$image` | `string` | Path to a profile image (relative to `CMS_DATA_PATH . "core/"`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure (e.g., no permissions, invalid name).

**Inner Mechanisms:**
1. Validates permissions (self, administrators, or guests).
2. Ensures the username is unique.
3. Handles image updates (deletes old image if a new one is provided).

**Usage:**
- Called to update user profiles.
- Example:
  ```php
  $core->set_profile("!user123", "Alice", "#00FF00", "Hello!", "alice.png");
  ```

---

#### `delete_profile($guid = NULL, $system = FALSE)`

**Purpose:**
Deletes a user's profile and associated image.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Defaults to the current user. |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates permissions (self, administrators, or system override).
2. Deletes the profile and associated image.

**Usage:**
- Called when a user is banned or leaves the system.
- Example:
  ```php
  $core->delete_profile("!user123");
  ```

---

#### `status($value = CMS_CORE_STATUS_NONE, $guid = NULL, $channel = NULL, $system = FALSE, $test = FALSE)`

**Purpose:**
Modifies the status flags for a user, with permission checks and notifications.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Status flag to add (`> 0`) or remove (`< 0`). |
| `$guid` | `string` | User identifier. Defaults to the current user. |
| `$channel` | `string` | Channel name. If `NULL`, applies globally. |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. |
| `$test` | `bool` | If `TRUE`, checks permissions without applying changes. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates permissions based on the status flag (e.g., only administrators can ban users).
2. Sends notifications to the channel and/or affected user.
3. Handles special cases (e.g., invisible users, bans).

**Usage:**
- Called to grant or revoke status flags.
- Example:
  ```php
  $core->status(CMS_CORE_STATUS_MUTE, "!user123", "General"); // Mute a user
  $core->status(-CMS_CORE_STATUS_INVISIBLE); // Make self visible
  ```

---

#### `clean()`

**Purpose:**
Removes orphaned profiles, status entries, and messages for users who no longer exist.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Retrieves a list of valid user accounts from the `permission` and `profile` resources.
2. Deletes profiles, status entries, and messages for users not in the list.

**Usage:**
- Called periodically to maintain system cleanliness.
- Example:
  ```php
  $core->clean();
  ```

---

#### `switch_guid($guid, $reset = FALSE)`

**Purpose:**
Temporarily switches the current user context to another user (e.g., for impersonation).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier to switch to. |
| `$reset` | `bool` | If `TRUE`, restores the previous context. |

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Uses a static stack to track context changes.
2. Updates `$this->guid` and `$this->operator`.

**Usage:**
- Used internally to send messages as another user.
- Example:
  ```php
  $core->switch_guid("!admin");
  $core->send("System message", NULL, CMS_CORE_DATA_SYSTEM);
  $core->reset_guid();
  ```

---

#### `reset_guid()`

**Purpose:**
Restores the previous user context after `switch_guid()`.

**Return Values:**
- `void`

**Usage:**
- Called after impersonating another user.
- Example:
  ```php
  $core->reset_guid();
  ```


<!-- HASH:e3bce5d7213ebc9cc829e9f3766a7fbe -->
