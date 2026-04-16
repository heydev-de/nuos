# NUOS API Documentation

[← Index](../README.md) | [`module/chat.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Chat Module (`module/chat.php`)

The **Chat Module** provides real-time communication functionality within the NUOS platform. It handles user connection, message exchange, and interface rendering for a web-based chat system. The module integrates with the core system for user management, session handling, and data persistence.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_CHAT_TIMEOUT` | `30` | Timeout in seconds before a user is considered disconnected. |
| `CMS_CHAT_REFRESH` | `5000` | Refresh rate in milliseconds for fetching new messages. |
| `CMS_CHAT_AWAY_TIMER_TIMEOUT` | `600000` | Time in milliseconds before a user is marked as "away" (10 minutes). |
| `CMS_CHAT_AWAY_TIMER_REFRESH` | `30000` | Refresh rate in milliseconds for the away timer (30 seconds). |

---

## Core Logic Flow

The module operates via a **display switch** (`$chat_display`) that routes execution to different submodules:

1. **Default**: Login interface for entering a username.
2. **Interface**: Main chat UI with message input, user list, and settings.
3. **Control**: Administrative panel for user management (requires `CMS_CORE_PERMISSION_CONTROL`).
4. **Send**: Processes outgoing messages and commands.
5. **Receive**: Fetches and formats incoming messages for the client.
6. **Disconnect/Disconnected**: Handles user disconnection and displays a reconnect prompt.

---

### `default` Case (Login Interface)

#### Purpose
Renders the login form for users to enter a chat username. Validates the username against the core system and redirects to the chat interface upon successful connection.

#### Inner Mechanisms
1. **Core Initialization**: Creates a `core` object with a timeout of `CMS_CHAT_TIMEOUT`.
2. **User Validation**:
   - Checks if the user is banned or already connected.
   - Displays an error message if validation fails.
3. **Connection Attempt**:
   - Uses `$core->connect($chat_name)` to log the user in.
   - Logs the connection event and redirects to the interface.
4. **Form Rendering**:
   - Displays a form with a text input for the username.
   - Shows an error if the username is already taken.
   - Focuses the input field via JavaScript.

#### Usage Context
- Triggered when no `$chat_display` is specified or when the user is not logged in.
- Used for initial user authentication before entering the chat.

---

### `interface` Case (Main Chat UI)

#### Purpose
Renders the primary chat interface, including:
- Message output area.
- Input field for sending messages.
- Emoticon picker.
- User control panel (settings, away status, disconnect).

#### Inner Mechanisms
1. **Core Initialization**: Validates the user’s session.
2. **UI Components**:
   - **Menu**: Contains buttons for away status, disconnect, and control panel.
   - **Output**: Empty `<div>` where messages are dynamically inserted.
   - **Input**: Text field for message composition with emoticon support.
   - **Control Panel**: Settings for focus behavior, sound, and away timer.
3. **Emoticon Handling**:
   - Scans the `CMS_IMAGES_PATH/emoticon` directory for image files.
   - Generates buttons for each emoticon, which insert text shortcuts into the input field.
4. **JavaScript Integration**:
   - `chat_onload()`: Initializes the chat, sets focus, and starts the message receiver.
   - Event listeners for message handling, control panel interactions, and away timer.

#### Usage Context
- Primary interface for users to send/receive messages.
- Dynamically updates via AJAX (`asr_send`) to fetch new messages.

---

### `control` Case (Administrative Panel)

#### Purpose
Provides administrative controls for managing users (e.g., kicking, banning, or viewing profiles). Requires `CMS_CORE_PERMISSION_CONTROL`.

#### Inner Mechanisms
1. **Permission Check**: Exits if the user lacks the required permission.
2. **Core Initialization**: Validates the user’s session.
3. **UI Rendering**:
   - Loads the `core_control` module to display user management tools.
   - Uses an `<iframe>` to embed the control panel.
4. **Default Display**: Sets `chat_display` as the default control view.

#### Usage Context
- Accessible via the "Control" button in the chat interface.
- Used by moderators/administrators to manage chat participants.

---

### `send` Case (Message Processing)

#### Purpose
Processes outgoing messages and commands from users.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$chat_data` | `string` | The raw message or command input by the user. |

#### Inner Mechanisms
1. **Validation**: Checks if the user is connected and if the message is non-empty.
2. **Command Handling**:
   - **Away Commands**: `/away`, `/afk`, `/brb` mark the user as absent.
   - **Disconnect Commands**: `/bye`, `/disconnect` log the user out.
   - **Default**: Sends the message to all users or specified recipients.
3. **Core Interaction**: Uses `$core->send($data)` to broadcast the message.

#### Usage Context
- Triggered via AJAX when a user submits a message or command.
- Processes both regular messages and system commands.

---

### `receive` Case (Message Fetching)

#### Purpose
Fetches and formats incoming messages for the client.

#### Return Value
| Type | Description |
|------|-------------|
| `string` (JSON) | A JSON-encoded array of messages, each with a type (`m`, `p`, `s`, `i`, `d`) and associated data. |

#### Inner Mechanisms
1. **Validation**: Checks if the user is connected.
2. **Message Processing**:
   - **System Messages**: Broadcast messages (e.g., "User X has joined").
   - **Private Messages**: Direct messages between users, with metadata for recipients.
   - **Spy Messages**: Messages intercepted by moderators (requires `CMS_CORE_DATA_SPY`).
   - **Default Messages**: Regular chat messages.
3. **Data Formatting**:
   - Each message is converted to an array with fields for `guid`, `name`, `color`, `image`, and `text`.
   - Uses `json_encode` to return the data to the client.

#### Usage Context
- Called periodically by the client via AJAX to fetch new messages.
- Handles all message types (public, private, system) and formats them for display.

---

### `disconnect` / `disconnected` Cases

#### Purpose
Handles user disconnection and displays a reconnect prompt.

#### Inner Mechanisms
1. **Disconnect**:
   - Calls `$core->disconnect()` to log the user out.
   - Falls through to the `disconnected` case.
2. **Disconnected**:
   - Renders a simple HTML page with a reconnect button.
   - Displays a message indicating the user has been disconnected.

#### Usage Context
- Triggered when a user manually disconnects or is timed out.
- Provides a way for users to rejoin the chat.

---

## JavaScript Functions

### `chat_onload()`
#### Purpose
Initializes the chat interface on page load.
#### Inner Mechanisms
- Calls `chat_receive()` to fetch messages.
- Starts the away timer.
- Sets focus to the message input field.
- Listens for messages from the control iframe.

---

### `chat_control_message(event)`
#### Purpose
Handles messages from the control iframe (e.g., refresh requests).
#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `event` | `MessageEvent` | Contains the message data from the iframe. |

#### Inner Mechanisms
- Processes messages like `chat_control_submit` (close control panel) or `chat_control_refresh_enable` (enable auto-refresh).

---

### `chat_receive(override = false)`
#### Purpose
Fetches new messages from the server.
#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `override` | `boolean` | If `true`, bypasses the refresh delay. |

#### Inner Mechanisms
- Uses `asr_send` to call the `receive` endpoint.
- Implements a delay to avoid excessive requests (respects `CMS_CHAT_REFRESH`).

---

### `chat_send()`
#### Purpose
Sends a message to the server.
#### Inner Mechanisms
- Validates the message input.
- Uses `asr_send` to call the `send` endpoint.
- Clears the input field and resets the away timer.

---

### `chat_write(type, guid, name, color, image, text, meta)`
#### Purpose
Renders a message in the chat output area.
#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `string` | Message type (`""`, `"private"`, `"system"`, `"info"`). |
| `guid` | `string` | User GUID. |
| `name` | `string` | User name. |
| `color` | `string` | User’s color (hex). |
| `image` | `string` | User’s avatar URL. |
| `text` | `string` | Message text. |
| `meta` | `string` | Metadata (e.g., private message recipients). |

#### Inner Mechanisms
- Formats the message with HTML, including links, emoticons, and user avatars.
- Auto-scrolls the output area if the user is near the bottom.

---

### `chat_notify(text)`
#### Purpose
Displays a browser notification (flashes the title bar).
#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | Notification text. |

#### Inner Mechanisms
- Alternates the browser title between the notification text and `…` to create a flashing effect.

---

### `chat_sound(file)`
#### Purpose
Plays a sound effect (e.g., for new messages).
#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | `string` | Path to the sound file. |

#### Inner Mechanisms
- Uses the `Audio` API to play sounds.
- Respects the sound toggle setting.

---

### `chat_away_timer()`
#### Purpose
Tracks user inactivity and marks them as "away" after a timeout.
#### Inner Mechanisms
- Decrements the timer every `CMS_CHAT_AWAY_TIMER_REFRESH` milliseconds.
- Sends `/away` if the timer reaches zero.
- Resets the timer when the user sends a message.

---

## Key Utility Functions

### `cms_url($addr, $param, $omit)`
#### Purpose
Generates URLs for chat endpoints (e.g., `send`, `receive`).
#### Usage Example
```php
cms_url(["chat_display" => "send", "chat_data" => "\x1B%data%"]);
```

### `q($s, $t = TRUE, $bin = FALSE)`
#### Purpose
Escapes strings for JavaScript/JSON output.
#### Usage Example
```php
echo(q(CMS_L_MOD_CHAT_005)); // Escapes a language string for JS.
```

### `x($s)`
#### Purpose
Escapes strings for XML/HTML output.
#### Usage Example
```php
echo(x($chat_name)); // Escapes a username for HTML.
```

### `image_process($url, $size)`
#### Purpose
Resizes user avatars for display in the chat.
#### Usage Example
```php
image_process(CMS_DATA_URL . "core/" . r($profile["image"]), 35);
```

---

## Integration with Core System

### `core` Class
- **`$core->connect($name)`**: Logs a user into the chat.
- **`$core->disconnect()`**: Logs a user out.
- **`$core->send($data)`**: Broadcasts a message.
- **`$core->receive()`**: Fetches new messages.
- **`$core->get_profile($guid)`**: Retrieves user details (name, color, avatar).

### `log` Class
- **`$log->access("connected", $name)`**: Logs user connections.

### `cms_permission($perm)`
- Checks if the user has the required permission (e.g., `CMS_CORE_PERMISSION_CONTROL`).

---

## Typical Usage Scenarios

1. **User Login**:
   - User navigates to the chat module.
   - Enters a username and submits the form.
   - The `default` case validates the username and redirects to the `interface`.

2. **Sending a Message**:
   - User types a message in the input field and presses Enter.
   - The `send` case processes the message and broadcasts it to other users.

3. **Receiving Messages**:
   - The `interface` periodically calls `chat_receive()` to fetch new messages.
   - Messages are rendered in the output area via `chat_write()`.

4. **Administrative Actions**:
   - A moderator opens the control panel via the `control` case.
   - Uses `core_control` to manage users (e.g., kick/ban).

5. **Disconnection**:
   - User clicks "Disconnect" or is timed out.
   - The `disconnect` case logs the user out and displays a reconnect prompt.


<!-- HASH:6ee04e3a83114e8ab17b8303b525a3e8 -->
