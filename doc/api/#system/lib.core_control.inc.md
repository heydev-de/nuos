# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.core_control.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Core Control Functions

This file provides the core control functionality for the NUOS web platform's real-time communication system. It handles user profiles, channel management, messaging, and status control within a chat-like interface.

---

## `core_control_profile`

Renders a user profile card with status information, avatar, and name.

### Purpose
Displays a visual representation of a user's profile, including their status (owner, operator, guest, etc.), avatar, name, and description. The profile can be clickable if a URL is provided.

### Parameters

| Name      | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `$core`   | object | Reference to the core object containing user and channel data.              |
| `$guid`   | string | Global Unique Identifier of the user whose profile is being rendered.       |
| `$channel`| string | (Optional) Channel context for status determination. Default: `NULL`.       |
| `$url`    | string | (Optional) URL to link the profile to. If empty, renders as a non-clickable div. Default: `NULL`. |

### Return Values
None. Outputs HTML directly to the buffer.

### Inner Mechanisms
1. **Status Retrieval**: Fetches the user's status in the given channel (or globally if no channel is specified).
2. **Visibility Check**: Skips rendering if the user is invisible and the viewer is not an operator.
3. **Profile Data**: Retrieves the user's profile (name, image, color, text).
4. **Status Determination**:
   - Checks for special roles (owner, operator, admin) and applies corresponding CSS classes and icons.
   - Applies additional status flags (spy, invisible, absent, mute, banned) and modifies the display accordingly.
5. **Color Contrast**: Dynamically calculates a readable text color (black or white) based on the user's background color.
6. **HTML Rendering**:
   - Renders a clickable anchor or a div based on the presence of a URL.
   - Displays the user's avatar (if available), name, and description.
   - Shows the computed status text.

### Usage Context
- Used within channel user lists, private message views, and profile management interfaces.
- Called by `core_control` when listing users in a channel or displaying a profile.

---

## `core_control`

Main control function for the chat system. Handles all interactions related to channels, users, profiles, and status management.

### Purpose
Serves as the central dispatcher for all chat-related actions:
- Channel connection, creation, editing, and deletion.
- User messaging.
- Profile editing.
- Status management (mute, ban, operator assignment, etc.).

### Parameters

| Name                  | Type   | Description                                                                 |
|-----------------------|--------|-----------------------------------------------------------------------------|
| `$core`               | object | Reference to the core object.                                               |
| `$core_control_object`| string | (Optional) Object type and identifier (e.g., `channel:general`, `user:123`). Default: `NULL`. |
| `$core_control_command`| string | (Optional) Command to execute (e.g., `connect`, `edit`, `message`). Default: `NULL`. |
| `$core_control_value` | array  | (Optional) Command parameters (e.g., channel name, message text). Default: `NULL`. |

### Return Values
None. Outputs HTML and JavaScript directly to the buffer.

### Inner Mechanisms
1. **Object Parsing**: Splits `$core_control_object` into type and identifier (e.g., `channel` and `general`).
2. **External Refresh Control**: Uses `postMessage` to enable/disable parent frame refresh based on the current view.
3. **Command Dispatching**: Routes commands to the appropriate handler based on the object type:
   - **Channel**: Connect, create, edit, delete.
   - **User**: Send messages.
   - **Profile**: Edit profile data (name, color, description, avatar).
   - **Status**: Apply status changes (mute, ban, operator, etc.).
4. **Form Generation**: Renders a form with navigation, context-specific UI, and command buttons.
5. **Dynamic UI**:
   - **Channel View**: Lists users, shows channel description, and provides owner/moderator controls.
   - **Channel Creation/Editing**: Input fields for name, description, and password.
   - **User Messaging**: Recipient profile and message input.
   - **Profile Editing**: Input fields for name, color, description, and avatar upload.
   - **Status Management**: Checkbox matrix for applying status flags to users in channels.
6. **Permission Checks**: Restricts certain actions (e.g., editing admin profiles, deleting channels) based on user permissions.
7. **Image Handling**: Validates and processes uploaded avatars (size, type, dimensions).

### Usage Context
- Primary entry point for all chat-related interactions.
- Used in the chat control panel to manage channels, users, and profiles.
- Handles both user-initiated actions (e.g., sending a message) and administrative actions (e.g., banning a user).

### Constants and Variables

#### Constants
| Name                          | Value/Default | Description                                                                 |
|-------------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_CORE_STATUS_INVISIBLE`   | Bitmask       | User status flag for invisibility.                                          |
| `CMS_CORE_STATUS_OWNER`       | Bitmask       | User status flag for channel ownership.                                     |
| `CMS_CORE_STATUS_OPERATOR`    | Bitmask       | User status flag for channel operator.                                      |
| `CMS_CORE_STATUS_SPY`         | Bitmask       | User status flag for spy mode (visible only to operators).                  |
| `CMS_CORE_STATUS_ABSENT`      | Bitmask       | User status flag for absence.                                               |
| `CMS_CORE_STATUS_MUTE`        | Bitmask       | User status flag for muting.                                                |
| `CMS_CORE_STATUS_BANNED`      | Bitmask       | User status flag for banning.                                               |
| `CMS_CORE_PERMISSION_OPERATOR`| Bitmask       | Permission flag for operator status.                                        |
| `CMS_L_*`                     | String        | Localized strings for UI labels (e.g., `CMS_L_CORE_CONTROL_001` for "Channels"). |

#### Variables
| Name                  | Scope       | Description                                                                 |
|-----------------------|-------------|-----------------------------------------------------------------------------|
| `$max_image_size`     | Local       | Maximum allowed size for uploaded avatars (5,120,000 bytes).                |
| `$type`               | Local       | Parsed object type (e.g., `channel`, `user`).                               |
| `$object`             | Local       | Parsed object identifier (e.g., channel name, user GUID).                   |
| `$channel`            | Local       | Current active channel.                                                     |
| `$status`             | Local       | User or channel status bitmask.                                             |
| `$profile`            | Local       | User profile data array.                                                    |
| `$list`, `$_list`     | Local       | Temporary arrays for channel/user lists in status management.               |

### JavaScript Functions
- **`core_control_channel_switch`**: Toggles visibility of extended channel options (description, password) based on the "permanent" checkbox.
- **`core_control_profile_image_select`**: Triggers file input dialog for avatar upload.
- **`core_control_profile_image_add`**: Validates and previews an uploaded avatar image.

### Notes
- **Security**: Uses `x()` for HTML escaping and `cms_url()` for URL generation with CSRF protection.
- **Localization**: All UI strings are localized via `CMS_L_*` constants.
- **Performance**: Efficiently handles large user/channel lists with minimal database queries.
- **Extensibility**: New object types or commands can be added by extending the `switch` blocks.


<!-- HASH:1474353eee2d3c63c1da5716d4a1a904 -->
