# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_control.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_control.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Control Functions

This file provides the core control interface for the PWNC Web Platform's real-time communication system. It handles user profiles, channel management, messaging, and status administration through a unified interface.

---

## `core_control_profile`

Renders a user profile card with status information, avatar, and styling based on user permissions and status flags.

### Parameters

| Name      | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `$core`   | object | Core system object (passed by reference). Provides access to user data.    |
| `$guid`   | string | Unique user identifier.                                                     |
| `$channel`| string | Channel name (optional). Used to determine user status in a specific channel.|
| `$url`    | string | Target URL for the profile card (optional). If empty, renders as a `<div>`. |

### Return Value

None. Outputs HTML directly.

### Inner Mechanisms

1. **Status Retrieval**: Fetches user status using `$core->get_status()`.
2. **Visibility Check**: Skips rendering if the user is invisible and the viewer is not an operator.
3. **Profile Data**: Retrieves profile data (name, image, color, text) via `$core->get_profile()`.
4. **Role Determination**:
   - Checks for owner (`!` prefix) or operator status.
   - Applies CSS classes and icons based on role.
5. **Status Flags**: Appends additional status indicators (spy, invisible, absent, mute, banned) if applicable.
6. **Color Contrast**: Dynamically calculates text color (black/white) for contrast against the user's chosen background color.
7. **HTML Output**: Renders the profile card with:
   - Avatar (if available).
   - Name and status.
   - Description (if available).

### Usage

Used to display user profiles in channel user lists, private messages, and profile management views.

#### Example

```php
core_control_profile($core, "user123", "general", "/profile/user123");
```
Renders a clickable profile card for `user123` in the `general` channel, linking to their profile page.

---

## `core_control`

Main control interface for the communication system. Handles all interactions: channel management, user messaging, profile editing, and status administration.

### Parameters

| Name                  | Type   | Description                                                                 |
|-----------------------|--------|-----------------------------------------------------------------------------|
| `$core`               | object | Core system object (passed by reference).                                   |
| `$core_control_object`| string | Target object in `type:id` format (e.g., `channel:general`).                |
| `$core_control_command`| string | Command to execute (e.g., `connect`, `edit`, `message`).                   |
| `$core_control_value` | array  | Command parameters (e.g., `["name" => "New Name"]`).                        |

### Return Value

None. Outputs HTML and JavaScript directly.

### Inner Mechanisms

1. **Object Parsing**: Splits `$core_control_object` into `type` and `object` (ID).
2. **Command Execution**: Routes commands based on `type` and `$core_control_command`:
   - **Channel**: Connect, create, edit, delete.
   - **User**: Send messages.
   - **Profile**: Edit profile data (including image uploads).
   - **Status**: Apply status flags to users/channels.
3. **Form Generation**: Renders context-specific forms with:
   - Hidden fields for commands.
   - Input fields for parameters (e.g., channel name, message text).
   - Submit buttons.
4. **Dynamic UI**: Uses JavaScript for interactive elements (e.g., toggling channel options, image previews).
5. **Permission Checks**: Restricts actions based on user roles (e.g., only operators can edit other profiles).
6. **Image Handling**: Validates and processes uploaded profile images (size, type, extension).

### Usage

This function is the backbone of the communication interface. It is called by the frontend to render all control panels.

#### Example: Connect to a Channel

```php
core_control($core, "channel:general", "connect", ["password" => "secret"]);
```
Connects the user to the `general` channel with the password `secret`.

#### Example: Edit Profile

```php
core_control($core, "profile:user123", "edit", [
    "name"  => "New Name",
    "color" => "#ff0000",
    "text"  => "Hello, world!"
]);
```
Updates the profile of `user123` with new name, color, and description.

#### Example: Send a Message

```php
core_control($core, "user:user456", "message", ["data" => "Hi there!"]);
```
Sends a private message to `user456`.

#### Example: Apply Status Flags

```php
core_control($core, "status_channel:general", "apply", [
    "general" => [
        "user123" => [
            CMS_CORE_STATUS_MUTE => 1
        ]
    ]
]);
```
Mutes `user123` in the `general` channel.

---

## Constants and Flags

The function relies on the following constants (defined elsewhere in the codebase):

| Constant                     | Description                                  |
|------------------------------|----------------------------------------------|
| `CMS_CORE_STATUS_OWNER`      | User is the channel owner.                   |
| `CMS_CORE_STATUS_OPERATOR`   | User is a channel operator.                  |
| `CMS_CORE_STATUS_INVISIBLE`  | User is invisible in the channel.            |
| `CMS_CORE_STATUS_SPY`        | User is spying on the channel (operator-only).|
| `CMS_CORE_STATUS_ABSENT`     | User is marked as absent.                    |
| `CMS_CORE_STATUS_MUTE`       | User is muted.                               |
| `CMS_CORE_STATUS_BANNED`     | User is banned from the channel.             |
| `CMS_CORE_PERMISSION_OPERATOR`| Permission flag for operators.               |

---

## Helper Functions

The following utility functions are used internally:

| Function               | Description                                                                 |
|------------------------|-----------------------------------------------------------------------------|
| `flag($status, $flag)` | Checks if `$status` has the `$flag` bit set.                               |
| `image($icon)`         | Renders an icon image.                                                      |
| `image_process($url, $size)`| Processes an image URL for resizing.                                   |
| `cms_url($params)`     | Generates a URL with the given parameters.                                 |
| `x($string)`           | Escapes HTML special characters.                                           |
| `jscript($code)`       | Outputs JavaScript code.                                                   |
| `unique_id()`          | Generates a unique ID for uploaded files.                                  |
| `file_extension($name)`| Extracts the file extension from a filename.                               |
| `mkpath($path)`        | Creates a directory if it doesn’t exist.                                   |
| `format_bytesize($size)`| Formats a byte size into a human-readable string.                       |

---

## Typical Scenarios

1. **Channel Management**:
   - **Create**: Operators can create permanent channels with descriptions and passwords.
   - **Edit**: Owners can modify channel descriptions and passwords.
   - **Delete**: Owners can delete channels.
   - **Connect**: Users can join channels (with password if required).

2. **User Interaction**:
   - **Messaging**: Users can send private messages to each other.
   - **Profile Editing**: Users can edit their own profiles (name, color, description, avatar).

3. **Status Administration**:
   - **Operators**: Can mute, ban, or spy on users in any channel.
   - **Owners**: Can assign operator status in their channels.
   - **Global Status**: Admins can manage status flags across all channels and users.

4. **Profile Viewing**:
   - Users can view profiles of others in the channel or via direct links.
   - Operators can access a full profile index.


<!-- HASH:1784eaf557fc149b7d9362aee6c6775b -->
