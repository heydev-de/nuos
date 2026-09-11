# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_control.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_control.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Control Functions

This file provides the core control panel functionality for the PWNC Web Platform's communication system. It handles the rendering and processing of user profiles, channel management, status moderation, and messaging controls within the core control interface.

The functions work together to provide a comprehensive administrative and user-facing control panel for managing chat channels, user profiles, and moderation actions.

### Constants and Configuration

| Name | Default | Description |
|------|---------|-------------|
| `$max_image_size` | `5120000` | Maximum allowed profile image upload size in bytes (5 MB) |

### Status Flags

| Flag | Description |
|------|-------------|
| `CMS_CORE_STATUS_INVISIBLE` | User is invisible to non-operators |
| `CMS_CORE_STATUS_OWNER` | User is the owner of the resource |
| `CMS_CORE_STATUS_OPERATOR` | User is an operator/moderator |
| `CMS_CORE_STATUS_SPY` | User is in spy mode |
| `CMS_CORE_STATUS_ABSENT` | User is marked as absent |
| `CMS_CORE_STATUS_MUTE` | User is muted |
| `CMS_CORE_STATUS_BANNED` | User is banned |

### Permission Constants

| Constant | Description |
|----------|-------------|
| `CMS_CORE_PERMISSION_OPERATOR` | Permission level for operators |

---

## core_control_profile

Renders a user profile card within the core control interface. This function displays a user's avatar, name, description, and status indicators with appropriate CSS classes and icons based on the user's role and status.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$core` | `object` | Reference to the core object providing status, profile, and permission data |
| `$guid` | `string` | The user's GUID (Globally Unique Identifier). May be prefixed with `!` for special users |
| `$channel` | `string\|NULL` | The channel context for status lookup. If `NULL`, no channel-specific status is checked |
| `$url` | `string\|NULL` | Optional URL to wrap the profile in an anchor tag. If empty, renders as a plain div |

### Return Values

This function does not return a value. It outputs HTML directly to the response stream.

### Inner Mechanisms

1. **Status Retrieval**: Calls `$core->get_status($guid, $channel)` to determine the user's current status flags.
2. **Visibility Check**: If the current user is not an operator and the profile belongs to the current user with an invisible status, the function returns early without rendering.
3. **Profile Data**: Retrieves the user's profile data (name, color, image, text) via `$core->get_profile($guid)`.
4. **Role Determination**: Determines the user's role through a priority-based check:
   - **Owner**: GUID starts with `!` and the current user has operator permission for that GUID, OR the status flag `CMS_CORE_STATUS_OWNER` is set
   - **Operator**: Status flag `CMS_CORE_STATUS_OPERATOR` is set
   - **Member**: GUID starts with `!` (but not an owner)
   - **Guest**: Default fallback
5. **Special Status**: If the current user is an operator, additional status indicators are appended:
   - **Spy**: Appends spy status text and CSS class
   - **Invisible**: Appends invisible status text, modifies icon, and adds CSS class
6. **Additional Status**: Appends status text and CSS classes for absent, muted, and banned statuses.
7. **Color Handling**: If the user has a custom color, calculates an appropriate text color (white or black) based on luminance, and generates an inline style attribute.
8. **HTML Output**: Outputs either a `<div>` or `<a>` wrapper depending on whether a URL was provided, followed by the profile image, name, description, and status.

### Usage Context

This function is called internally by `core_control()` to render user listings in various views (channel user lists, profile index, user messaging). It can also be used independently to render a profile card anywhere in the interface.

### Usage Example

```php
// Render a user profile card as a link to their profile page
core_control_profile($core, $user_guid, $current_channel, 
    cms_url(["core_control_object" => "profile:$user_guid"]));

// Render a user profile card without a link (e.g., in a user list)
core_control_profile($core, $user_guid, $current_channel);
```

---

## core_control

The main control panel function that processes commands and renders the core control interface. It handles channel management (create, edit, delete, connect), user messaging, profile editing, and status moderation.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$core` | `object` | Reference to the core object providing channel, index, profile, and status management |
| `$core_control_object` | `string\|NULL` | The object type and identifier in `type:id` format (e.g., `channel:general`, `profile:abc123`). If `NULL`, defaults to the current channel |
| `$core_control_command` | `string\|NULL` | The command to execute (e.g., `connect`, `create`, `edit`, `delete`, `message`, `apply`) |
| `$core_control_value` | `array\|NULL` | The values associated with the command (e.g., channel name, password, message data, status flags) |

### Return Values

This function does not return a value. It outputs HTML and JavaScript directly to the response stream.

### Inner Mechanisms

#### Object Parsing
Parses `$core_control_object` by splitting on `:` to extract the type (e.g., `channel`, `user`, `profile`, `status`) and the object identifier.

#### External Refresh Control
Outputs JavaScript to enable or disable external refresh via `parent.postMessage()`, depending on whether the object is a channel-related type.

#### Command Processing
Processes commands based on the object type:

- **channel**: Handles `connect` command to join a channel with optional password
- **channel_create**: Handles `create` command to create a new channel with optional permanence, description, and password
- **channel_edit**: Handles `edit` command to modify an existing channel's description and password
- **channel_delete**: Handles `delete` command to remove a channel
- **user**: Handles `message` command to send a message to a specific user
- **profile**: Handles `edit` command to update a user's profile (name, color, text, and image upload)
- **status/status_channel/status_user/status_all**: Handles `apply` command to batch-update status flags for users across channels

#### Profile Image Upload
When editing a profile, validates uploaded images:
- Checks file existence and type (gif, jpeg, png, webp)
- Validates file size against `$max_image_size`
- Generates a unique filename and moves the uploaded file to the core data directory

#### Interface Rendering
Renders the control panel interface with:
- A navigation menu with links to channels, channel index, profile, and status views
- Context-specific forms and controls based on the current object type
- User listings with profile cards
- Status management forms with checkboxes for each status flag
- JavaScript for client-side interactions (image preview, form submission)

### Usage Context

This function is the primary entry point for the core control panel. It is typically called from a controller or template that passes the current core object and request parameters.

### Usage Example

```php
// Render the main control panel for the current channel
core_control($core);

// Process a channel creation command
core_control($core, "channel_create", "create", [
    "channel" => "new_channel",
    "text" => "A new channel",
    "password" => "secret123",
    "permanent" => 1
]);

// Process a profile edit command
core_control($core, "profile:$user_guid", "edit", [
    "name" => "New Name",
    "color" => "#ff0000",
    "text" => "Updated description"
]);

// Process a status moderation command
core_control($core, "status_channel:general", "apply", [
    "general" => [
        "user123" => [
            CMS_CORE_STATUS_MUTE => 1,
            CMS_CORE_STATUS_BANNED => 0
        ]
    ]
]);
```

### Sub-Functions and Helpers Used

| Function | Description |
|----------|-------------|
| `flag($status, $flag)` | Checks if a specific flag is set in a status bitmask |
| `cms_permission($permission, ...)` | Checks if the current user has a specific permission |
| `cms_url($params)` | Generates a URL with merged parameters |
| `cms_param($params)` | Manages static parameter state and generates query strings |
| `image($icon, ...)` | Renders an icon image |
| `image_process($url, $size)` | Processes and returns a resized image URL |
| `jscript($code)` | Outputs JavaScript code |
| `x($string)` | XML-escapes a string |
| `q($string)` | JS/JSON-style string encoder |
| `r($string)` | Raw URL-encodes a string |
| `stre($value)` | Checks if a value is empty |
| `nstre($value)` | Checks if a value is not empty |
| `streq($a, $b)` | Checks string equality |
| `nstreq($a, $b)` | Checks string inequality |
| `format_bytesize($bytes)` | Formats a byte count as a human-readable string |
| `unique_id()` | Generates a unique identifier |
| `file_extension($filename)` | Extracts the file extension |
| `mkpath($path)` | Creates a directory path recursively |
| `move_uploaded_file($from, $to)` | Moves an uploaded file |


<!-- HASH:1784eaf557fc149b7d9362aee6c6775b -->
