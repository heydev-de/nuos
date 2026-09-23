# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.agent.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.agent.inc)

- **Version:** `26.9.23.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Agent Interface

The `ifc.agent.inc` file is the interface controller for the **Agent** module in the PWNC Web Platform. It manages the lifecycle of AI agent interactions, including object selection (owner/user/chat), message display, sending new messages, interrupting ongoing processes, and deleting chats. It integrates with the `agent` library and provides both server-side logic and client-side JavaScript for real-time chat rendering.

---

## `$agent_process_object`

A closure that parses and validates an agent object identifier string (e.g., `owner.user.id`) and stores it in the permanent cache.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$_object` | `string` | The raw object identifier string to process. |

### Inner Mechanisms

1. Splits the input string by `.` into up to 3 parts: `$owner`, `$user`, `$id`.
2. If no owner is provided, defaults to `CMS_SUPERUSER`.
3. If the current user is not an operator and tries to access another owner, resets to `CMS_SUPERUSER`.
4. If a user is specified but the current user lacks permission, clears `$user` and `$id`.
5. Reconstructs the object string and caches it if changed.

### Usage Example

```php
$agent_process_object("admin.john.chat123");
// Sets $owner = "admin", $user = "john", $id = "chat123"
// Caches the object string for future requests
```

---

## Message Handling

Handles different interface messages via a `switch` on `CMS_IFC_MESSAGE`.

### `select`

Selects an agent object based on the interface parameter.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | The object identifier to select. Defaults to `CMS_SUPERUSER`. |

#### Usage Example

```php
// Triggered by ifc_post('select', 'admin.john.chat123')
$agent_process_object("admin.john.chat123");
```

---

### `display`

Displays chat messages from a JSON file, supporting pagination and tool call/result rendering.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$owner` | `string` | The owner of the agent data. |
| `$user` | `string` | The user whose chat to display. |
| `$id` | `string` | The chat identifier. |
| `$from` | `int` | Starting line number (-1 for all). |
| `$to` | `int` | Ending line number (-1 for all). |

#### Inner Mechanisms

1. Constructs a file path from owner/user/id.
2. Reads the JSON file line by line.
3. Prescans lines to track pending tool calls.
4. Renders messages, tool calls, results, and system prompts.
5. Outputs line numbers followed by rendered HTML.

#### Usage Example

```php
// Request: ?ifc_message=display&object=admin.john.chat123&from=0&to=50
// Returns first 50 lines of chat with HTML rendering
```

---

### `send`

Sends a new message to an agent.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | The message text to send. |
| `$context` | `string` | Optional context appended to the message. |
| `$user` | `string` | Target user. |
| `$owner` | `string` | Target owner. |
| `$id` | `string` | Target chat ID. |

#### Return Values

- On success: `"1\nowner.user.newid"`
- On failure: `"0\n<error message>"`

#### Usage Example

```php
// Request: ?ifc_message=send&object=admin.john.&ifc_param=Hello+world
// Creates a new chat or continues existing one
```

---

### `interrupt`

Interrupts an ongoing agent process.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$owner` | `string` | The owner of the agent. |
| `$user` | `string` | The user of the agent. |
| `$id` | `string` | The chat ID to interrupt. |

#### Return Values

- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

#### Usage Example

```php
// Request: ?ifc_message=interrupt&object=admin.john.chat123
// Stops the currently running agent process
```

---

### `delete`

Deletes a chat (operators only).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$owner` | `string` | The owner of the agent. |
| `$user` | `string` | The user of the agent. |
| `$id` | `string` | The chat ID to delete. |

#### Return Values

- Sets `$ifc_response` to `CMS_MSG_DONE` on success
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure

#### Usage Example

```php
// Request: ?ifc_message=delete&object=admin.john.chat123
// Removes the chat file and resets object to user level
```

---

## Main Display

Renders the agent interface with owner/user/chat selection and message display area.

### Owner Selection

Displays a popover with available owners (only for operators).

#### Inner Mechanisms

1. Scans `CMS_AGENT_DATA_PATH` for owner directories.
2. Renders clickable links to select owners.

#### Usage Example

```php
// For operators, shows list of all agent owners
// Clicking an owner triggers ifc_post('select', 'ownername')
```

---

### User Selection

Displays a popover with available users/agents for the selected owner.

#### Inner Mechanisms

1. For `CMS_SUPERUSER`: scans system permissions for users with agent access.
2. For operators: scans owner directory for user subdirectories.
3. Renders clickable links to select users.

#### Usage Example

```php
// Shows list of users under selected owner
// Clicking a user triggers ifc_post('select', 'owner.username')
```

---

### Chat Selection

Displays a popover with recent chats for the selected user.

#### Inner Mechanisms

1. Scans user directory for `.json` files.
2. Uses `SplMinHeap` to keep the 100 most recent chats by modification time.
3. Renders clickable links with friendly dates.

#### Usage Example

```php
// Shows list of recent chats for selected user
// Clicking a chat triggers ifc_post('select', 'owner.user.chatid')
```

---

## JavaScript Functions

Client-side functions for real-time chat display and interaction.

### `agent_run_interrupt()`

Sends an interrupt request to stop the current agent process.

#### Usage Example

```javascript
// Called when user clicks the interrupt button
agent_run_interrupt();
```

---

### `agent_display_load(from, to, callback)`

Loads chat messages from the server within a line range.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `from` | `int` | Starting line number. |
| `to` | `int` | Ending line number. |
| `callback` | `function` | Called with returned HTML. |

#### Usage Example

```javascript
agent_display_load(0, 50, (html) => {
    agent_display.innerHTML = html;
});
```

---

### `agent_display_init()`

Initializes the chat display by loading the first batch of messages.

#### Usage Example

```javascript
// Automatically called on page load
agent_display_init();
```

---

### `agent_display_update()`

Periodically polls for new messages and appends them to the display.

#### Usage Example

```javascript
// Automatically called every 5 seconds after init
agent_display_update();
```

---

### `agent_display_scroll(behavior)`

Scrolls to the last message in the display.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `behavior` | `string` | Scroll behavior: `"smooth"` or `"instant"`. |

#### Usage Example

```javascript
agent_display_scroll("smooth");
```

---

### `agent_display_reveal()`

Loads older messages when the user scrolls to the top.

#### Usage Example

```javascript
// Automatically triggered on scroll event
agent_display_reveal();
```

---

### `agent_marked()`

Applies Markdown rendering to message text elements.

#### Usage Example

```javascript
// Called after new messages are added to the display
agent_marked();
```

---

### `agent_get_context()`

Retrieves context from the opener window's form data.

#### Return Values

- `string` containing URL and form data, or empty string.

#### Usage Example

```javascript
const context = agent_get_context();
// Returns: "https://example.com\nfield1: value1\nfield2: value2"
```

---

### Speech Recognition

Implements speech-to-text input using the Web Speech API.

#### Features

- Supports on-device speech recognition when available
- Falls back to cloud-based recognition
- Handles language pack installation
- Provides visual feedback via button classes

#### Usage Example

```javascript
// User clicks microphone button to start/stop speech recognition
// Recognized speech is inserted into the message textarea
```


<!-- HASH:8f683e0e7cf0c327deae39b1cc247ece -->
