# PWNC API Documentation

[← Index](../README.md) | [`module/chat.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/chat.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Chat Module

The `chat.php` module implements a real-time chat system within the PWNC Web Platform. It provides a full-featured chat interface with user presence tracking, private messaging, system notifications, emoticons, sound alerts, away timers, and administrative controls. The module is structured around several display modes controlled by the `$chat_display` parameter, each handling a specific aspect of the chat lifecycle.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CHAT_TIMEOUT` | `30` | Timeout in seconds for user presence tracking |
| `CMS_CHAT_REFRESH` | `5000` | Refresh rate in milliseconds for receiving new messages |
| `CMS_CHAT_AWAY_TIMER_TIMEOUT` | `600000` | Away timeout in milliseconds before auto-setting status to absent |
| `CMS_CHAT_AWAY_TIMER_REFRESH` | `30000` | Away timer refresh rate in milliseconds |

### Display Modes

The module uses a `switch` statement on `$chat_display` to determine which part of the chat functionality to execute:

#### `default` (Login)

Handles the initial connection process where users enter their name and join the chat.

**Mechanism:**
1. Creates a `core` instance with the chat timeout
2. Updates the user index, removing timed-out users
3. Checks if the chat is enabled and if the user is already connected
4. If connected, displays an error message and redirects after 10 seconds
5. If not connected, attempts to log in with the provided `$chat_name`
6. On successful login, logs the access and redirects to the chat interface
7. On failed login, displays a login form with the user's name pre-filled

**Usage Example:**
```php
// User accesses the chat module without specifying a display mode
// They see a login form to enter their name and join the chat
```

#### `interface` (Main Chat Interface)

Renders the main chat interface with message display, input area, emoticons, and control panel.

**Mechanism:**
1. Creates a `core` instance and updates the user index
2. Verifies the user is still connected; redirects to disconnect if not
3. Outputs the HTML structure with:
   - Control switch checkbox
   - Menu with away/disconnect/control buttons
   - Message output area
   - Input form with emoticon selector
   - Control panel iframe
4. Includes JavaScript for:
   - Message receiving via AJAX polling
   - Message sending
   - Emoticon insertion
   - Away timer functionality
   - Sound notifications
   - Focus management

**JavaScript Functions:**

| Function | Purpose |
|----------|---------|
| `chat_onload()` | Initializes chat by starting receive loop, away timer, focusing input, and listening for control messages |
| `chat_receive(override)` | Polls for new messages at regular intervals |
| `chat_send()` | Sends the current message from the input field |
| `chat_write(...)` | Renders a message in the output area with proper formatting |
| `chat_message(...)` | Handles regular chat messages |
| `chat_private(...)` | Handles private messages with metadata |
| `chat_system(text)` | Displays system messages |
| `chat_info(text)` | Displays informational messages |
| `chat_link(text)` | Converts URLs in text to clickable links |
| `chat_emoticon(text)` | Replaces emoticon codes with images |
| `chat_focus()` | Focuses the window if enabled |
| `chat_notify(text)` | Flashes the window title for notifications |
| `chat_sound(file)` | Plays notification sounds |
| `chat_away_timer()` | Manages the away timer countdown |
| `chat_disconnect()` | Redirects to the disconnect endpoint |

**Usage Example:**
```php
// User successfully logs in and is redirected to the chat interface
// They can now send/receive messages, use emoticons, and access controls
```

#### `control` (Administrative Control Panel)

Provides an administrative interface for managing chat users and settings.

**Mechanism:**
1. Checks for `CMS_CORE_PERMISSION_CONTROL` permission
2. Loads the `core_control` library
3. Creates a `core` instance and updates the user index
4. Verifies the user is still connected
5. Sets up the control display context
6. Loads and renders the core control element based on `$core_control_object`, `$core_control_command`, and `$core_control_value`
7. Displays permission information

**Usage Example:**
```php
// Administrator accesses the control panel to manage chat users
// They can view profiles, change statuses, or disconnect users
```

#### `send` (Message Sending)

Processes outgoing messages and commands from the chat interface.

**Mechanism:**
1. Creates a `core` instance and updates the user index
2. Verifies the user is still connected
3. Trims and validates the message data
4. Processes special commands:
   - `/away`, `/afk`, `/brb` - Sets user status to absent
   - `/bye`, `/disconnect`, `/exit`, `/logout`, `/off`, `/quit` - Disconnects the user
   - Default - Sends the message to all connected users

**Usage Example:**
```php
// User types "/away" in the chat input
// Their status is set to absent and other users are notified
```

#### `receive` (Message Receiving)

Handles AJAX requests for retrieving new messages.

**Mechanism:**
1. Creates a `core` instance and updates the user index
2. Verifies the user is still connected
3. If disconnected, returns a disconnect signal `[["d"]]`
4. Retrieves new messages via `$core->receive()`
5. Processes each message based on its type:
   - System messages (`s`) - Simple text notifications
   - Private messages (`p`) - Messages with sender metadata
   - Spy messages (`i`) - Informational messages about private conversations
   - Default messages (`m`) - Regular chat messages
6. Returns messages as a JSON array

**Message Processing Details:**
- For private messages, resolves sender profiles and builds metadata
- For spy messages, builds metadata showing conversation participants
- All messages include sender profile information (name, color, image)

**Usage Example:**
```javascript
// JavaScript polls this endpoint every 5 seconds
// Receives JSON array of new messages to display
```

#### `disconnect` (User Disconnection)

Handles the disconnection process when a user leaves the chat.

**Mechanism:**
1. Creates a `core` instance and updates the user index
2. If the user is still connected, calls `$core->disconnect()`
3. Falls through to the `disconnected` case

#### `disconnected` (Disconnected Display)

Displays a simple page when a user has been disconnected from the chat.

**Mechanism:**
1. Outputs a basic HTML page with:
   - Title indicating the chat module
   - Message explaining the disconnection
   - Button to reconnect

**Usage Example:**
```php
// User's session times out or they manually disconnect
// They see a page with a reconnect button
```

### Core Integration

The module heavily relies on the `core` class for:
- User presence tracking and timeout management
- Message sending and receiving
- Profile management
- Connection state handling

### Security Considerations

- Uses `cms_permission()` for access control on the control panel
- Escapes all output using `x()` for HTML context
- Uses `q()` for JavaScript string encoding
- Implements CSRF protection through `cms_param()` and `cms_url()`
- Validates user input before processing commands

### Client-Side Architecture

The JavaScript implementation follows these patterns:
- Polling-based message retrieval (every 5 seconds)
- Event-driven message handling
- State management for away timers and notifications
- Cross-frame communication for control panel integration
- Progressive enhancement with noscript fallback


<!-- HASH:cf5442525313ca20babcedf888ae8c51 -->
