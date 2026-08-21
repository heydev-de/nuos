# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Communication Relay Class

The `core` class serves as the central communication hub for the PWNC Web Platform, managing real-time user interactions, channel operations, and status tracking. It handles user connections, message passing, profile management, and status updates in a multi-user environment.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PERMISSION_CONTROL` | `"control"` | Permission identifier for control operations. |
| `CMS_CORE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for operator-level access. |
| `CMS_CORE_PORTAL` | `"Lobby"` | Default channel name for unassigned users. |
| `CMS_CORE_STATUS_NONE` | `0` | No status flags set. |
| `CMS_CORE_STATUS_OWNER` | `1` | User is the owner of a channel. |
| `CMS_CORE_STATUS_OPERATOR` | `2` | User is an operator in a channel. |
| `CMS_CORE_STATUS_SPY` | `4` | User can see private messages. |
| `CMS_CORE_STATUS_INVISIBLE` | `8` | User is invisible to others. |
| `CMS_CORE_STATUS_ABSENT` | `16` | User is marked as absent. |
| `CMS_CORE_STATUS_MUTE` | `32` | User is muted and cannot send messages. |
| `CMS_CORE_STATUS_BANNED` | `64` | User is banned from the system. |
| `CMS_CORE_DATA_DEFAULT` | `0` | Default message type. |
| `CMS_CORE_DATA_SYSTEM` | `1` | System-generated message. |
| `CMS_CORE_DATA_PRIVATE` | `2` | Private message. |
| `CMS_CORE_DATA_PRIVATE_META` | `4` | Metadata for private messages (e.g., recipient list). |
| `CMS_CORE_DATA_PRIVATE_DATA` | `8` | Content of a private message. |
| `CMS_CORE_DATA_RESPONSE` | `16` | Message is a response to the sender. |
| `CMS_CORE_DATA_SPY_META` | `32` | Metadata visible to spies. |
| `CMS_CORE_DATA_SPY_DATA` | `64` | Message content visible to spies. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$guid` | `NULL` | Unique identifier for the current user. |
| `$timeout` | `15` | Connection timeout in seconds. |
| `$enabled` | `FALSE` | Indicates if the core is active for the executing user. |
| `$operator` | `FALSE` | Indicates if the user has operator privileges. |
| `$index` | `NULL` | Resource object for tracking connected users. |
| `$profile` | `NULL` | Resource object for user profiles. |
| `$channel` | `NULL` | Resource object for channel definitions. |
| `$status` | `NULL` | Resource object for user statuses. |
| `$data` | `NULL` | Resource object for message data exchange. |

---

### Constructor: `__construct`

#### Purpose
Initializes the core communication system, loads required resources, and sets up user-specific data structures.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$timeout` | `int` | Connection timeout in seconds. Default: `15`. |
| `$datapath` | `string` | Relative path to core data files. Default: `"#core"`. |

#### Return Value
- **`void`**: No explicit return; initializes object state.

#### Inner Mechanisms
1. Loads the `core_resource` library.
2. Generates a unique user ID (`$guid`) based on the current user.
3. Initializes resource objects for status, index, profile, channel, and data.
4. Checks user permissions and bans; disables core if banned.
5. Sets up a cleanup timer every 15 minutes.

#### Usage Example
```php
$core = new \cms\core(30); // 30-second timeout
if ($core->enabled) {
    // Core is active and user is permitted
}
```

---

### Method: `unique_name`

#### Purpose
Checks if a username is available (not already in use by another user).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Username to check. |

#### Return Value
- **`bool`**: `TRUE` if the name is unique; `FALSE` otherwise.

#### Inner Mechanisms
1. Locks the profile resource to prevent race conditions.
2. Iterates through all profiles to check for name collisions.
3. Skips the current user’s own profile.

#### Usage Example
```php
if ($core->unique_name("Alice")) {
    // Name is available
} else {
    // Name is taken
}
```

---

### Method: `connect`

#### Purpose
Connects a user to the system, initializes their profile, and notifies the channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Optional username. If empty, uses the current user’s name. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Checks if the user is already connected.
2. For guests, generates a random color and assigns a unique name.
3. For registered users, ensures the name is unique.
4. Updates the index, sets operator status if applicable, and sends a join notification.

#### Usage Example
```php
if ($core->connect("Alice")) {
    // User connected successfully
} else {
    // Connection failed (e.g., name taken or already connected)
}
```

---

### Method: `disconnect`

#### Purpose
Disconnects a user from the system, cleans up their profile (if applicable), and notifies the channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$clean` | `bool` | If `TRUE`, deletes the user’s profile (for guests). Default: `TRUE`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Locks resources to prevent race conditions.
2. Removes the user from the index.
3. Deletes the profile if the user is a guest and not banned.
4. Cleans up messages addressed to the user.

#### Usage Example
```php
$core->disconnect(); // Disconnects the current user
```

---

### Method: `get_status`

#### Purpose
Retrieves the combined status of a user (global and channel-specific).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID. Default: current user. |
| `$channel` | `string` | Channel name. Default: `NULL` (global status). |

#### Return Value
- **`int`**: Bitmask of status flags (e.g., `CMS_CORE_STATUS_OWNER | CMS_CORE_STATUS_MUTE`).

#### Inner Mechanisms
1. Locks the status resource.
2. Retrieves global status and merges it with channel-specific status.

#### Usage Example
```php
$status = $core->get_status();
if (flag($status, CMS_CORE_STATUS_BANNED)) {
    // User is banned
}
```

---

### Method: `set_status`

#### Purpose
Sets the status of a user in a specific channel or globally.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$status` | `int` | Status bitmask. Default: `CMS_CORE_STATUS_NONE`. |
| `$guid` | `string` | User ID. Default: current user. |
| `$channel` | `string` | Channel name. Default: `NULL` (global status). |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Locks the status resource.
2. Updates or creates a status entry for the user/channel combination.

#### Usage Example
```php
$core->set_status(CMS_CORE_STATUS_MUTE, "user123", "General");
```

---

### Method: `delete_status`

#### Purpose
Removes all status entries for a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID. Default: current user. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Locks the status resource.
2. Deletes all status entries matching the user ID.

#### Usage Example
```php
$core->delete_status("user123");
```

---

### Method: `update_status`

#### Purpose
Cleans up status entries for a channel that no longer exists.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

#### Return Value
- **`bool`**: `TRUE` if the channel was cleaned up; `FALSE` otherwise.

#### Inner Mechanisms
1. Checks if the channel exists (temporary or permanent).
2. If not, deletes all status entries for that channel.

#### Usage Example
```php
$core->update_status("OldChannel");
```

---

### Method: `update_index`

#### Purpose
Updates the connection status of users and disconnects those who have timed out.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$query_connection` | `bool` | If `TRUE`, only checks for timeouts without updating the current user’s time. Default: `FALSE`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Locks the index resource.
2. Updates the current user’s last activity time.
3. Disconnects users whose last activity exceeds the timeout.

#### Usage Example
```php
$core->update_index(); // Runs periodically to clean up inactive users
```

---

### Method: `connect_channel`

#### Purpose
Connects a user to a channel, handling permissions and notifications.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$password` | `string` | Channel password (if required). Default: `""`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Checks permissions (administrators, owners, or correct password).
2. Notifies the current channel of the user’s departure.
3. Grants owner/operator status if the channel is new.
4. Notifies the new channel of the user’s arrival.

#### Usage Example
```php
if ($core->connect_channel("General", "secret123")) {
    // User joined the channel
}
```

---

### Method: `create_channel`

#### Purpose
Creates a new permanent channel (administrators only).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | Channel description. Default: `""`. |
| `$password` | `string` | Channel password. Default: `""`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Validates the channel name (not empty, not "Lobby").
2. Checks if the channel already exists.
3. Grants the creator owner/operator status.

#### Usage Example
```php
$core->create_channel("Support", "User support channel", "support123");
```

---

### Method: `set_channel`

#### Purpose
Updates the description or password of an existing channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | New description. Default: `""`. |
| `$password` | `string` | New password. Default: `""`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Requires administrator or owner permissions.
2. Updates the channel’s metadata.

#### Usage Example
```php
$core->set_channel("Support", "Dedicated support channel", "newpass123");
```

---

### Method: `delete_channel`

#### Purpose
Deletes a channel and cleans up associated status entries.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Requires administrator or owner permissions.
2. Deletes the channel and notifies users.

#### Usage Example
```php
$core->delete_channel("OldChannel");
```

---

### Method: `send`

#### Purpose
Sends a message to a channel or specific user(s), handling visibility and permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$data` | `string` | Message content. |
| `$receiver` | `string|array` | Recipient(s). `NULL` for channel-wide. |
| `$status` | `int` | Message type (e.g., `CMS_CORE_DATA_PRIVATE`). Default: `CMS_CORE_DATA_DEFAULT`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Checks if the sender is connected and not muted.
2. Handles private messages, spies, and invisible users.
3. Stores messages in the data buffer for delivery.

#### Usage Example
```php
// Send a public message
$core->send("Hello, everyone!");

// Send a private message
$core->send("Hi Alice!", "alice123", CMS_CORE_DATA_PRIVATE);
```

---

### Method: `receive`

#### Purpose
Retrieves and optionally deletes messages addressed to the current user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$delete` | `bool` | If `TRUE`, deletes messages after retrieval. Default: `TRUE`. |

#### Return Value
- **`array`**: List of messages, each as an associative array with keys: `guid`, `receiver`, `status`, `data`.

#### Inner Mechanisms
1. Locks the data resource.
2. Collects all messages for the current user.

#### Usage Example
```php
$messages = $core->receive();
foreach ($messages as $message) {
    echo "From: {$message['guid']}, Message: {$message['data']}";
}
```

---

### Method: `get_profile`

#### Purpose
Retrieves the profile information of a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID. Default: current user. |

#### Return Value
- **`array|bool`**: Associative array of profile data or `FALSE` if not found.

#### Inner Mechanisms
1. Locks the profile resource.
2. Returns profile data if the user exists.

#### Usage Example
```php
$profile = $core->get_profile("alice123");
if ($profile) {
    echo "Name: {$profile['name']}, Color: {$profile['color']}";
}
```

---

### Method: `set_profile`

#### Purpose
Updates the profile information of a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID. Default: current user. |
| `$name` | `string` | New username. |
| `$color` | `string` | New color (hex format). Default: `""`. |
| `$text` | `string` | New description. Default: `""`. |
| `$image` | `string` | New image path. Default: `""`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions (self, administrators, or guests).
2. Cleans up the name and color.
3. Handles image updates (deletes old image if replaced).

#### Usage Example
```php
$core->set_profile(NULL, "Alice", "#ff0000", "Hello, I'm Alice!", "alice.png");
```

---

### Method: `delete_profile`

#### Purpose
Deletes a user’s profile and associated image.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID. Default: current user. |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. Default: `FALSE`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions (self, administrators, or system override).
2. Deletes the profile and associated image.

#### Usage Example
```php
$core->delete_profile("guest123");
```

---

### Method: `status`

#### Purpose
Modifies the status of a user (e.g., mute, ban, invisible) and notifies affected parties.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Status to add (`> 0`) or remove (`< 0`). |
| `$guid` | `string` | User ID. Default: current user. |
| `$channel` | `string` | Channel name. Default: `NULL` (global). |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. Default: `FALSE`. |
| `$test` | `bool` | If `TRUE`, only checks permissions without applying changes. Default: `FALSE`. |

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions based on the status type.
2. Handles special cases (e.g., invisible users, bans).
3. Sends notifications to the user, channel, or initiator.

#### Usage Example
```php
// Mute a user
$core->status(CMS_CORE_STATUS_MUTE, "user123", "General");

// Unmute a user
$core->status(-CMS_CORE_STATUS_MUTE, "user123", "General");
```

---

### Method: `clean`

#### Purpose
Removes orphaned profiles, statuses, and messages for users who no longer exist.

#### Return Value
- **`bool`**: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Retrieves a list of valid user accounts from permissions and profiles.
2. Deletes profiles, statuses, and messages for non-existent users.

#### Usage Example
```php
$core->clean(); // Runs periodically to clean up orphaned data
```

---

### Method: `switch_guid`

#### Purpose
Temporarily switches the context to another user (e.g., for sending messages as that user).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User ID to switch to. |
| `$reset` | `bool` | If `TRUE`, restores the previous context. Default: `FALSE`. |

#### Return Value
- **`void`**: No return value.

#### Inner Mechanisms
1. Uses a static stack to track context switches.
2. Updates `$guid` and `$operator` properties.

#### Usage Example
```php
$core->switch_guid("admin123");
$core->send("System message", NULL, CMS_CORE_DATA_SYSTEM);
$core->reset_guid();
```

---

### Method: `reset_guid`

#### Purpose
Restores the previous user context after a `switch_guid` call.

#### Return Value
- **`void`**: No return value.

#### Usage Example
```php
$core->reset_guid();
```


<!-- HASH:114bd1de1d852da6de5fa0bcd9d643db -->
