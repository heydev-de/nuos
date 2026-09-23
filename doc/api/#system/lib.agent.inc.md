# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.agent.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.agent.inc)

- **Version:** `26.9.22.6`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_AGENT_TEMPLATE_PATH` | `CMS_PATH . "#json/agent/"` | Directory containing provider template JSON files |
| `CMS_AGENT_DATA_PATH` | `CMS_DATA_PATH . "#agent/"` | Base directory for agent conversation data |
| `CMS_AGENT_REQUEST_LIMIT` | `32` | Maximum number of LLM requests per agent run |
| `CMS_AGENT_REQUEST_TIMEOUT` | `300` | Timeout in seconds for each HTTP request to the provider |
| `CMS_AGENT_RESPONSE_LIMIT` | `4194304` | Maximum response size in bytes (4 MB) |
| `CMS_AGENT_RESUME_LIMIT` | `5` | Maximum number of times a crashed run can be resumed |
| `CMS_AGENT_COMPACT_LIMIT` | `3` | Maximum number of compaction retry loops per run |
| `CMS_AGENT_COMPACT_RATIO` | `0.25` | Ratio of request size to target for context compaction |
| `CMS_AGENT_COMPACT_TEXT` | `"~~~ compacted ~~~"` | Placeholder text replacing compacted content |
| `CMS_AGENT_OMITTED_TEXT` | `"~~~ omitted ~~~"` | Placeholder text for omitted request fields |
| `CMS_AGENT_CONTEXT_SEPARATOR` | `"\u{E000}"` | Unicode private-use character used as context separator |

## Functions

### agent_api_list

Returns a sorted list of available provider templates by scanning the template directory.

**Return values:**
- `array` — Associative array where both keys and values are template names (without `.json` extension), sorted naturally.

**Inner mechanisms:** Opens the template directory, reads each file, matches files ending in `.json`, extracts the base name, and returns a sorted map.

**Usage example:**
```php
$templates = agent_api_list();
// Returns: ["openai" => "openai", "anthropic" => "anthropic", ...]
foreach ($templates as $name => $label) {
    echo "Available provider: $name\n";
}
```

### agent_option_parse

Parses a multi-line text string of `key=value` pairs into an associative array with type coercion.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Multi-line text containing `key=value` pairs, one per line |

**Return values:**
- `array` — Associative array of parsed key-value pairs. Numeric strings are converted to numbers, `"true"`/`"false"` (case-insensitive) are converted to booleans.

**Inner mechanisms:** Splits text by newlines, finds the first `=` on each line, trims key and value, and applies type coercion.

**Usage example:**
```php
$config = agent_option_parse("temperature=0.7\nmax_tokens=4096\nstream=true");
// Returns: ["temperature" => 0.7, "max_tokens" => 4096, "stream" => true]
```

### agent_start

Convenience function to create a new agent and immediately send a message.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | `string` | — | Username of the agent owner |
| `$message` | `string` | — | Initial message to send to the agent |
| `$owner` | `string` | `NULL` | Conversation owner (defaults to `CMS_SUPERUSER`) |
| `$id` | `string` | `NULL` | Context ID (auto-generated if not provided) |
| `$option` | `array` | `NULL` | Additional options to pass to the agent |

**Return values:**
- `string|FALSE` — The agent context ID on success, or `FALSE` on failure.

**Inner mechanisms:** Creates an `agent` instance, checks for errors, then calls `send_message()`. If successful, returns the agent ID; otherwise returns `FALSE`.

**Usage example:**
```php
$agentId = agent_start("assistant1", "Hello, what is the weather today?");
if ($agentId !== FALSE) {
    echo "Agent started with ID: $agentId\n";
}
```

## Class: agent

The `agent` class implements an AI agent that communicates with LLM providers, manages conversation context, executes tools via MCP, and runs as a daemon process.

### Properties

| Property | Visibility | Type | Description |
|----------|-----------|------|-------------|
| `$id` | public | `string\|NULL` | Unique context identifier for this conversation |
| `$error` | public | `string\|NULL` | Error message if initialization or operation failed |
| `$resume_count` | public | `int` | Number of times this run has been resumed after a crash |
| `$user` | private | `string\|NULL` | Username of the agent |
| `$template` | private | `array\|NULL` | Decoded provider template configuration |
| `$option` | private | `array` | Merged configuration options (runtime + user data + defaults) |
| `$option_raw` | private | `array\|NULL` | Original option parameter as passed to constructor |
| `$tool_list` | private | `array` | Provider-shaped tool declaration list |
| `$owner` | private | `string\|NULL` | Conversation owner username |
| `$data_path` | private | `string` | Filesystem path for this conversation's data |
| `$context_offset` | private | `int` | Byte offset of last consumed context entry |
| `$message` | private | `array` | Provider-shaped message list (indexed by context offset) |
| `$compactable` | private | `array` | Indices of tool result entries eligible for compaction |
| `$mcp_client` | private | `mcp_client\|NULL` | MCP client instance for tool execution |
| `$mcp_tool_list` | private | `array` | MCP tool list for HTTP header mirroring |
| `$mcp_token` | private | `string\|NULL` | Hashed MCP authorization token |
| `$request_limit` | private | `int` | Maximum requests per run |
| `$request_timeout` | private | `int` | Timeout per HTTP request in seconds |
| `$request_prev` | private | `array` | Previous request data for diff computation |
| `$request_size` | private | `int` | Size of the last request body in bytes |
| `$interrupted` | private | `bool` | Whether an interrupt signal was received |
| `$finish_reason` | private | `string\|NULL` | Reason the run finished |

### __construct

Initializes a new agent instance, loading user permissions, provider template, and configuration.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | `string` | — | Username of the agent |
| `$owner` | `string` | `NULL` | Conversation owner (defaults to `CMS_SUPERUSER`) |
| `$id` | `string` | `NULL` | Context ID (auto-generated if not provided) |
| `$option` | `array` | `NULL` | Runtime options to override defaults |

**Return values:** None (constructor). Sets `$this->error` on failure.

**Inner mechanisms:**
1. Generates or assigns a context ID
2. Loads user permission data from `#system/permission`
3. Creates the conversation data directory
4. Loads and validates the provider template JSON
5. Merges configuration from multiple sources: runtime options → user data → parsed options → template defaults
6. Validates required configuration fields
7. Builds the system prompt using MCP system prompt and permission instructions
8. Sets request limits from options or constants

**Usage example:**
```php
$agent = new agent("assistant1", CMS_SUPERUSER, NULL, ["limit" => 10]);
if ($agent->error) {
    echo "Error: " . $agent->error . "\n";
} else {
    echo "Agent ready with ID: " . $agent->id . "\n";
}
```

### send_message

Sends a message to an existing or new conversation, triggering the agent run loop.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$message` | `string` | — | The user message to send |
| `$user` | `string` | — | Username of the sender |

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on error or no message.

**Inner mechanisms:**
1. Checks for prior errors or empty messages
2. If a conversation file exists, appends the message to context and resumes the run queue
3. If no conversation exists, starts a new run with the message
4. Triggers daemon execution

**Usage example:**
```php
$agent = new agent("assistant1");
if ($agent->send_message("What is 2+2?", "user1")) {
    echo "Message sent successfully.\n";
}
```

### interrupt (static)

Signals an active agent run to interrupt, or removes a stale daemon task.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$owner` | `string` | Conversation owner |
| `$user` | `string` | Agent username |
| `$id` | `string` | Context ID |

**Return values:**
- `bool` — `TRUE` if the interrupt flag was set or the daemon task was removed.

**Inner mechanisms:** Constructs a daemon key, checks if the run is active, and either sets an interrupt flag or removes the daemon task directly.

**Usage example:**
```php
agent::interrupt(CMS_SUPERUSER, "assistant1", "2026-01-15_10-30-00_abc123");
```

### delete (static)

Deletes an agent conversation, interrupting active runs or removing the context file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$owner` | `string` | Conversation owner |
| `$user` | `string` | Agent username |
| `$id` | `string` | Context ID |

**Return values:**
- `bool` — `TRUE` if the conversation was deleted or signals were set for an active run.

**Inner mechanisms:** If the run is active, sets interrupt and delete flags. Otherwise, deletes the context file and clears any signals.

**Usage example:**
```php
if (agent::delete(CMS_SUPERUSER, "assistant1", "2026-01-15_10-30-00_abc123")) {
    echo "Conversation deleted.\n";
}
```

### send_request (private)

Sends an HTTP request to the LLM provider and processes the response.

**Parameters:** None.

**Return values:**
- `array|FALSE` — Decoded JSON response array on success, `FALSE` on failure.

**Inner mechanisms:**
1. Loads the HTTP library
2. Builds the request URL, headers, and body using template placeholders
3. Logs the request to context (with diff computation to omit unchanged fields)
4. Sends the HTTP POST request with configured timeout and response size limit
5. Decodes the JSON response and logs it to context
6. Returns the decoded response or `FALSE` on failure

**Usage context:** Called internally by `run_start()` during the agent's run loop.

### token_issue (private)

Issues a temporary MCP authorization token for the agent.

**Parameters:** None.

**Return values:**
- `string|FALSE` — The raw token string on success, `FALSE` on failure.

**Inner mechanisms:** Generates a tagged token, hashes it, stores it in the permission token data store with owner/user/id metadata, and returns the raw token.

**Usage context:** Called by `run_start()` to authenticate MCP tool calls.

### token_revoke (private)

Revokes the current MCP authorization token.

**Parameters:** None.

**Return values:**
- `bool` — `TRUE` on success, `FALSE` if no token exists or deletion fails.

**Inner mechanisms:** Deletes the hashed token from the permission token data store and clears the local reference.

**Usage context:** Called by `run_stop()` to clean up after a run completes.

### context_add (private)

Appends a new entry to the conversation context file.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | `mixed` | — | The payload data to store |
| `$type` | `string` | `"message"` | Entry type: `"message"`, `"result"`, `"request"`, `"response"`, `"stop"`, `"compress"` |
| `$from` | `mixed` | `NULL` | Source identifier (e.g., `"system"`, `"user.username"`, `NULL`) |
| `$tools` | `mixed` | `NULL` | Tool call metadata |
| `$reasoning` | `string\|NULL` | `NULL` | Reasoning text extracted from the response |

**Return values:**
- `int|FALSE` — The byte offset of the new entry on success, `FALSE` on failure.

**Inner mechanisms:**
1. Opens the context file in append-binary mode
2. Acquires an exclusive lock
3. Seeks to end of file and records the offset
4. Builds a context entry with timestamp, index, type, and optional fields
5. Determines the text path within the provider-shaped data based on type and source
6. JSON-encodes the entry and appends it as a new line
7. Returns the byte offset for indexing

**Usage context:** Called throughout the agent lifecycle to persist messages, tool results, requests, responses, and control entries.

### context_restore

Restores the agent's in-memory message state from the context file.

**Parameters:** None.

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on error.

**Inner mechanisms:** Resets message and compactable arrays, resets context offset, and calls `context_import()`.

**Usage example:**
```php
$agent = new agent("assistant1");
$agent->context_restore();
// $agent->message now contains all persisted messages
```

### context_import (private)

Imports new context entries from the file since the last import.

**Parameters:** None.

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on corruption or I/O error.

**Inner mechanisms:**
1. Opens the context file for reading
2. Seeks to the last consumed offset
3. Reads line by line, decoding each JSON entry
4. For `message` and `result` types, appends to the message list
5. For `compress` types, compacts the referenced message
6. Updates the context offset to the current file position
7. Sorts messages by index

**Usage context:** Called by `context_restore()` and during the run loop to pick up new messages.

### context_overflow (private)

Checks if the provider response indicates a context window overflow.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$response` | `array` | The decoded provider response |

**Return values:**
- `bool` — `TRUE` if the response matches the overflow pattern, `FALSE` otherwise.

**Inner mechanisms:** Retrieves the overflow pattern from the template, then checks both the error field and the stop reason field in the response for a match.

**Usage context:** Called in the run loop to detect when the context window is full and compaction is needed.

### context_compact (private)

Compacts eligible tool result entries to reduce context size.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$target` | `int` | Target number of bytes to free |

**Return values:**
- `int` — Total bytes freed.

**Inner mechanisms:** Iterates over compactable entries, calls `message_compact()` on each, logs compression entries, and stops when the target is reached.

**Usage context:** Called when context overflow is detected to reduce the conversation size.

### message_add (private)

Adds a message to both the context file and the in-memory message list.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$message` | `mixed` | — | The message data to add |
| `$from` | `string\|NULL` | `NULL` | Source identifier |
| `$type` | `string` | `"message"` | Entry type |
| `$tools` | `mixed` | `NULL` | Tool call metadata |
| `$reasoning` | `string\|NULL` | `NULL` | Reasoning text |

**Return values:**
- `int|FALSE` — The context offset on success, `FALSE` on failure.

**Inner mechanisms:** Calls `context_add()` to persist the entry, then `message_append()` to update the in-memory list.

**Usage context:** Used throughout the run loop to add assistant messages, tool results, and system prompts.

### message_append (private)

Appends a message to the in-memory message list.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `mixed` | The message data |
| `$type` | `string` | Entry type (`"message"` or `"result"`) |
| `$index` | `int` | The context offset index |

**Return values:** None.

**Inner mechanisms:** If the type is `"result"`, marks the index as compactable. Stores the message at the given index.

**Usage context:** Called by `message_add()` and `context_import()`.

### message_compact (private)

Compacts a single message entry by replacing its content with a placeholder.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The message index to compact |

**Return values:**
- `int` — Bytes freed by compaction.

**Inner mechanisms:**
1. Removes the index from the compactable list
2. Retrieves the content path from the template's compact configuration
3. Traverses the content structure to find the text field
4. If the content is longer than the compact text placeholder, replaces it and calculates freed bytes
5. Updates the in-memory message

**Usage context:** Called by `context_compact()` during context overflow handling.

### run_queue (private)

Creates or resumes a daemon task to run the agent.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$message` | `string\|NULL` | `NULL` | Initial message for a new run |
| `$user` | `string\|NULL` | `NULL` | Sender username |

**Return values:**
- `bool` — `TRUE` if the daemon task was created or already exists, `FALSE` on failure.

**Inner mechanisms:**
1. Constructs a daemon key from owner, user, and ID
2. Creates a daemon task that instantiates a new agent, sets resume count, and calls `run_start()`
3. If the daemon task is created, returns `TRUE`
4. If message is `NULL` (resume case), checks if the daemon task exists

**Usage context:** Called by `send_message()` and `run_shutdown()` to start or resume the agent run.

### run_start

Main entry point for the agent's run loop, executed within a daemon task.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$message` | `string\|NULL` | `NULL` | Initial message for a new conversation |
| `$user` | `string\|NULL` | `NULL` | Sender username |

**Return values:**
- `bool` — `TRUE` on successful completion, `FALSE` on error.

**Inner mechanisms:**
1. Clears interrupt and delete signals
2. Issues an MCP authorization token
3. Registers a shutdown function for crash recovery
4. Initializes MCP client or configures server-side tool execution
5. Restores context from the context file
6. Seeds the conversation with system prompt and initial message if needed
7. Enters the main run loop:
   - Checks for interrupt signals
   - Imports new context entries
   - Sends requests to the provider
   - Handles context overflow with compaction
   - Resolves named values from the response
   - Checks for errors
   - Translates the stop reason into an action (`stop`, `tool`, `resume`)
   - Shapes the assistant reply message
   - Collects tool call metadata
   - Extracts reasoning text
   - Adds the message to context
   - Executes tool calls or resumes based on the action
8. Stops the run when the request limit is exceeded

**Usage example:**
```php
// This is typically called internally by the daemon, but can be invoked directly:
$agent = new agent("assistant1");
$agent->run_start("Hello, how are you?", "user1");
```

### run_stop (private)

Finalizes the agent run, handling cleanup and callback delivery.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$finish_reason` | `string` | — | Reason the run is stopping |
| `$error` | `bool` | `FALSE` | Whether this is an error stop |
| `$crash` | `bool` | `FALSE` | Whether this is a crash (skip callback delivery) |

**Return values:**
- `bool` — `TRUE` if the context was deleted, `FALSE` otherwise.

**Inner mechanisms:**
1. Sets the finish reason
2. If a delete flag is set, deletes the context file and clears signals
3. Otherwise, writes a stop entry to the context
4. If not a crash, retrieves the last assistant message and delivers it:
   - Via a callback agent (synthetic tool result) if configured
   - Via memory storage if a callback string is configured
5. Revokes the MCP token
6. Returns whether the context was deleted

**Usage context:** Called at the end of `run_start()` and by `run_shutdown()` for crash recovery.

### run_interrupted (private)

Checks if an interrupt signal has been received.

**Parameters:** None.

**Return values:**
- `bool` — `TRUE` if interrupted, `FALSE` otherwise.

**Inner mechanisms:** Checks the local `$interrupted` flag first, then checks the daemon flag for an interrupt signal. If found, clears the flag and sets the local flag.

**Usage context:** Called at the start of each iteration in the run loop.

### run_shutdown (private)

Handles crash recovery by resuming the agent run.

**Parameters:** None.

**Return values:** None.

**Inner mechanisms:**
1. If the run already finished, returns early
2. Attempts to stop with a crash reason (which may delete the context)
3. If the resume count exceeds the limit, stops with an error
4. Otherwise, re-queues the run for resumption

**Usage context:** Registered as a shutdown function in `run_start()`.

### mcp_client_init (private)

Initializes the MCP client and retrieves the tool list.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$list_tools` | `bool` | `TRUE` | Whether to collect tools for declaration |

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. Returns early if already initialized
2. Loads the `mcp_client` library
3. Creates a new `mcp_client` instance with the MCP URL, token, and timeouts
4. Retrieves the tool list from the MCP server
5. Collects tools for HTTP header mirroring and tool declaration

**Usage context:** Called by `run_start()` and `mcp_execute()` to ensure the MCP client is ready.

### mcp_execute (private)

Executes tool calls received from the provider.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$call` | `array` | — | List of tool call objects from the provider |
| `$resolved` | `array` | `[]` | List of call IDs already answered by the provider |

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. Iterates over each tool call
2. Skips calls without a name or already resolved by the provider
3. Checks for interrupt signals
4. Initializes the MCP client if needed
5. Decodes arguments (handles JSON string format from OpenAI-style providers)
6. Executes the tool call via the MCP client
7. Collects results and errors
8. Calls `mcp_result()` to add results to context

**Usage context:** Called by `run_start()` when the provider requests tool execution.

### mcp_result (private)

Adds tool execution results to the conversation context.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$block` | `array` | Result blocks shaped by the template |
| `$list` | `array` | List of call IDs and error flags |

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:** If the template has a grouped result message, adds all blocks as a single result. Otherwise, adds each block individually.

**Usage context:** Called by `mcp_execute()` after tool execution.

### mcp_generate (private)

Generates a synthetic tool call and result, used for callback delivery.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Tool name (e.g., `"call_reply"`) |
| `$content` | `string` | Content for the tool result |
| `$argument` | `array` | Arguments for the tool call |
| `$error` | `bool` | Whether this is an error result |

**Return values:**
- `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. Generates a unique call ID
2. Encodes arguments if configured
3. Builds a synthetic tool call entry using template placeholders
4. Shapes it into an assistant message and adds it to context
5. Builds a synthetic tool result block and adds it via `mcp_result()`
6. Re-queues the run

**Usage context:** Called by `run_stop()` to deliver callback messages to the calling agent.

### daemon_key (private)

Generates the daemon key for this agent instance.

**Parameters:** None.

**Return values:**
- `string` — The daemon key in the format `"owner.user.id"`.

**Inner mechanisms:** Concatenates owner, user, and ID with dots.

**Usage context:** Used by `run_queue()`, `run_stop()`, `run_interrupted()`, and `interrupt()`/`delete()` for daemon task management.

### replace_placeholder (private)

Recursively replaces `%placeholder%` tokens in arrays or strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `mixed` | The data structure containing placeholders |
| `$replacement` | `array` | Associative array of replacement values |

**Return values:**
- `mixed` — The data structure with placeholders replaced.

**Inner mechanisms:**
1. If the input is a string matching the full placeholder pattern (`%name%`), returns the replacement value directly
2. If the input is a string with partial placeholders, replaces each `%name%` with the corresponding value
3. If the input is an array, recursively processes each value
4. Handles optional fields (keys wrapped in `[brackets]`) by omitting them when the value is blank
5. Handles list arrays by omitting blank values

**Usage example:**
```php
$result = $agent->replace_placeholder(
    ["url" => "https://%endpoint%/v1/chat", "model" => "%model%"],
    ["endpoint" => "api.openai.com", "model" => "gpt-4"]
);
// Returns: ["url" => "https://api.openai.com/v1/chat", "model" => "gpt-4"]
```

### extract_text (static)

Extracts text from a nested data structure using a slash-separated path.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$data` | `mixed` | The data structure to extract from |
| `$path` | `string` | Slash-separated path (e.g., `"choices/0/message/content"`) |

**Return values:**
- `string` — The extracted text, or empty string if not found.

**Inner mechanisms:**
1. If the path is empty, returns the data as a string if scalar
2. Splits the path by `/`
3. Traverses the data structure following each path segment
4. Supports wildcard `*` to join all elements at that level
5. Returns empty string if any segment is missing or data is not an array

**Usage example:**
```php
$text = agent::extract_text($response, "choices/0/message/content");
// Extracts the content field from an OpenAI-style response
```

### generate_id (static)

Generates a unique context ID.

**Parameters:** None.

**Return values:**
- `string` — A unique ID in the format `"YYYY-MM-DD_HH-MM-SS_random"`.

**Inner mechanisms:** Combines a timestamp with a random unique ID.

**Usage example:**
```php
$id = agent::generate_id();
// Returns something like: "2026-01-15_10-30-00_a1b2c3d4"
```

### generate_user_prompt (private)

Generates a user prompt message shaped by the provider template.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$message` | `string` | — | The raw user message |
| `$user` | `string\|NULL` | `NULL` | Username of the sender |

**Return values:**
- `array` — The shaped user prompt message.

**Inner mechanisms:**
1. If a user is specified, prefixes the message with the username and action
2. Applies template placeholder replacement using the message text and options

**Usage context:** Called by `send_message()` and `run_start()` to create user prompt entries.

## Typical Usage Flow

1. **Start an agent:**
```php
$agentId = agent_start("assistant1", "Explain quantum computing");
```

2. **Send follow-up messages:**
```php
$agent = new agent("assistant1", CMS_SUPERUSER, $agentId);
$agent->send_message("Can you give me a simpler explanation?", "user1");
```

3. **Interrupt a running agent:**
```php
agent::interrupt(CMS_SUPERUSER, "assistant1", $agentId);
```

4. **Delete a conversation:**
```php
agent::delete(CMS_SUPERUSER, "assistant1", $agentId);
```

## Provider Template Structure

Provider templates are JSON files stored in `#json/agent/` that define:

- `version` — Template version (must be `1`)
- `url` — API endpoint URL with placeholders
- `header` — HTTP headers with placeholders
- `request` — Request body template with placeholders
- `response` — Response parsing configuration
- `text` — Paths to text fields within provider-shaped data
- `value` — Paths to named values (error, reason, reply, call)
- `action` — Mapping of stop reasons to actions (`stop`, `tool`, `resume`)
- `system` — System prompt template
- `user` — User prompt template
- `assistant` — Assistant message template
- `tool` — Tool declaration template
- `call` — Tool call path configuration
- `result` — Tool result template
- `server` — Server-side MCP configuration (optional)
- `compact` — Compaction configuration
- `overflow` — Overflow detection pattern
- `required` — List of required configuration fields
- `default` — Default option values
- `instruction` — Instruction template with placeholders


<!-- HASH:dbe873fa6423eb681aabb7a94fe8a005 -->
