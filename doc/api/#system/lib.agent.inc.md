# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.agent.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.agent.inc)

- **Version:** `26.9.15.6`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Agent System

The `agent` class and its associated functions form PWNC's AI agent framework. It enables conversational interactions with LLM providers through configurable templates, manages persistent conversation contexts, executes tools via the Model Context Protocol (MCP), and handles context window overflow through intelligent compaction.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_AGENT_TEMPLATE_PATH` | `CMS_PATH . "#json/agent/"` | Directory containing provider template JSON files |
| `CMS_AGENT_DATA_PATH` | `CMS_DATA_PATH . "#agent/"` | Base directory for agent conversation data |
| `CMS_AGENT_REQUEST_LIMIT` | `32` | Maximum number of LLM requests per agent run |
| `CMS_AGENT_REQUEST_TIMEOUT` | `300` | Timeout in seconds for each HTTP request to the provider |
| `CMS_AGENT_RESPONSE_LIMIT` | `4194304` | Maximum response size in bytes (4 MB) |
| `CMS_AGENT_RESUME_LIMIT` | `5` | Maximum number of times a run can be resumed after interruption |
| `CMS_AGENT_COMPACT_LIMIT` | `3` | Maximum compaction retry attempts before giving up |
| `CMS_AGENT_COMPACT_RATIO` | `0.25` | Ratio of request size used as compaction target |
| `CMS_AGENT_COMPACT_TEXT` | `"~~~ compacted ~~~"` | Placeholder text replacing compacted content |
| `CMS_AGENT_OMITTED_TEXT` | `"~~~ omitted ~~~"` | Placeholder text for omitted request fields in context |

### agent_api_list

Returns a sorted list of available provider templates by scanning the template directory.

**Return:** `array` — Associative array where both keys and values are template names (without `.json` extension), sorted naturally.

**Inner mechanism:** Opens the template directory, reads each file, matches against a regex to extract the name from `.json` files, and returns a sorted map.

```php
$templates = agent_api_list();
// Returns: ["openai" => "openai", "anthropic" => "anthropic", ...]
```

### agent_option_parse

Parses a newline-delimited key=value text into a typed associative array.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Raw text with one `key=value` pair per line |

**Return:** `array` — Parsed key-value pairs with automatic type coercion (numeric strings become numbers, `"true"`/`"false"` become booleans).

**Inner mechanism:** Splits text by newlines, finds the first `=` in each line, trims key and value, and applies type coercion based on value content.

```php
$options = agent_option_parse("temperature=0.7\nmax_tokens=4096\nstream=true");
// Returns: ["temperature" => 0.7, "max_tokens" => 4096, "stream" => true]
```

### agent_start

Convenience function to create an agent, start a conversation, and send an initial message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username of the agent owner |
| `$message` | `string` | Initial user message to send |
| `$option` | `array\|null` | Optional configuration overrides |

**Return:** `string\|false` — The conversation run ID on success, `FALSE` on failure.

**Inner mechanism:** Instantiates an `agent` object, checks for errors, then calls `send_message()` to begin the conversation.

```php
$runId = agent_start("alice", "What is the weather today?");
if ($runId !== FALSE) {
    echo "Agent started with run ID: $runId";
}
```

## agent Class

The core agent class that manages the full lifecycle of an AI agent conversation: initialization from user permissions and provider templates, message sending, HTTP request execution, MCP tool calling, context persistence, and context compaction.

### Properties

| Property | Visibility | Type | Description |
|----------|------------|------|-------------|
| `$id` | public | `string\|null` | Unique run identifier |
| `$error` | public | `string\|null` | Error message if initialization or execution failed |
| `$resume_count` | public | `int` | Number of times this run has been resumed |
| `$user` | private | `string\|null` | Username of the agent |
| `$template` | private | `array\|null` | Decoded provider template |
| `$option` | private | `array` | Resolved configuration options |
| `$option_raw` | private | `array\|null` | Original option parameter passed to constructor |
| `$tool_list` | private | `array` | Provider-shaped tool list for the request |
| `$owner` | private | `string\|null` | Conversation owner username |
| `$data_path` | private | `string` | Filesystem path for conversation data |
| `$context_offset` | private | `int` | Byte offset of last consumed context entry |
| `$message` | private | `array` | Provider-shaped message list (indexed by file offset) |
| `$compactable` | private | `array` | Indices of tool result entries eligible for compaction |
| `$mcp_client` | private | `mcp_client\|null` | MCP client instance for tool execution |
| `$mcp_tool_list` | private | `array` | Available MCP tools |
| `$mcp_token` | private | `string\|null` | Hashed MCP authorization token |
| `$request_limit` | private | `int` | Max requests per run |
| `$request_timeout` | private | `int` | Per-request timeout in seconds |
| `$request_prev` | private | `array` | Previous request for diff computation |
| `$request_size` | private | `int` | Size of last request body in bytes |
| `$interrupted` | private | `bool` | Whether an interrupt signal was received |
| `$finish_reason` | private | `string\|null` | Reason the run finished |

### __construct

Initializes the agent by loading user permissions, creating a conversation directory, loading and validating the provider template, merging configuration options, and setting request limits.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username of the agent |
| `$owner` | `string\|null` | Conversation owner (defaults to `CMS_SUPERUSER`) |
| `$option` | `array\|null` | Runtime configuration overrides |

**Inner mechanism:**
1. Loads user permission data via `data("#system/permission")`
2. Creates a conversation directory using `mkpath()`
3. Reads and decodes the provider template JSON from `CMS_AGENT_TEMPLATE_PATH`
4. Validates template version is `1`
5. Merges options in priority order: runtime options → user data → parsed option string → template defaults
6. Validates all required fields are configured
7. Generates a system prompt via `mcp::system_prompt()`
8. Sets request limits from options or constants

```php
$agent = new agent("alice", CMS_SUPERUSER, ["limit" => 10, "timeout" => 120]);
if ($agent->error) {
    echo "Error: " . $agent->error;
}
```

### send_message

Sends a user message to the agent, either continuing an existing conversation or starting a new one.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `string` | The user's message text |
| `$user` | `string` | Username of the message sender |

**Return:** `bool` — `TRUE` on success, `FALSE` on error or empty message.

**Inner mechanism:**
- If a conversation file already exists, appends the user message to context and continues the run via `run_queue()`
- If no conversation exists, starts a new run with the message
- Triggers `cms_daemon_run()` to process the queue

```php
$agent = new agent("alice");
$agent->send_message("Explain quantum computing", "alice");
```

### interrupt (static)

Interrupts a running agent by setting a flag or removing the daemon task.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | The agent run ID to interrupt |

**Return:** `bool` — Result of `cms_flag_set()` or `cms_daemon_remove()`.

**Inner mechanism:** If the daemon is running, sets an interrupt flag that the run loop checks. If not running, removes the daemon task entirely.

```php
agent::interrupt("2026-01-15_10-30-00_abc123");
```

### send_request (private)

Sends an HTTP request to the LLM provider with the current message context and tool list.

**Return:** `array\|false` — Decoded JSON response array on success, `FALSE` on failure.

**Inner mechanism:**
1. Loads the `http` library
2. Builds the request URL and body using template placeholders
3. Computes a diff of the request against the previous one to minimize context storage
4. Writes the request to context
5. Sends the HTTP POST request with configured timeout and response size limit
6. Decodes and stores the JSON response

```php
// Called internally during run_start()
$response = $this->send_request();
```

### token_issue (private)

Creates a temporary MCP authorization token for the agent run.

**Return:** `string\|false` — The raw token string on success, `FALSE` on failure.

**Inner mechanism:** Generates a tagged token via `cms_token_tag()`, hashes it with `hash64()`, stores the mapping in the permission token map, and returns the raw token.

```php
$token = $this->token_issue();
$this->option["mcp_token"] = $token;
```

### token_revoke (private)

Removes the temporary MCP authorization token.

**Return:** `bool` — `TRUE` on success, `FALSE` if no token exists or save fails.

**Inner mechanism:** Deletes the hashed token from the permission token map and saves.

```php
$this->token_revoke(); // Called during run_stop()
```

### context_add (private)

Appends a context entry to the conversation file with file locking.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$data` | `mixed` | The data to store (message, request, response, etc.) |
| `$type` | `string` | Entry type: `"message"`, `"request"`, `"response"`, `"tool"`, `"stop"`, `"compress"` |
| `$from` | `string\|null` | Source identifier (e.g., `"user.alice"`) |

**Return:** `int\|false` — The byte offset of the new entry, or `FALSE` on failure.

**Inner mechanism:** Opens the conversation file in append-binary mode, acquires an exclusive lock, seeks to end, records the offset, builds a JSON entry with timestamp and type, writes it as a newline-delimited JSON line, and returns the offset.

```php
$offset = $this->context_add(["text" => "Hello"], "message", "user.alice");
```

### context_restore

Resets in-memory message state and imports the full conversation context from disk.

**Return:** `bool` — `TRUE` on success, `FALSE` on corruption.

**Inner mechanism:** Clears message and compactable arrays, resets context offset, then calls `context_import()` to load all entries from the beginning.

```php
$this->context_restore(); // Called at start of run_start()
```

### context_import (private)

Imports context entries from the conversation file starting at the current offset.

**Return:** `bool` — `TRUE` on success, `FALSE` on file corruption.

**Inner mechanism:** Opens the conversation file, acquires a shared lock, seeks to the stored offset, reads newline-delimited JSON entries, and processes each by type:
- `"message"` and `"result"`: Appends to the message list via `message_append()`
- `"compress"`: Triggers compaction via `message_compact()`

Updates the context offset to the current file position after import.

```php
$this->context_import(); // Called in run loop and context_restore()
```

### context_overflow (private)

Checks if the provider response indicates a context window overflow.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$response` | `array` | The decoded provider response |

**Return:** `bool` — `TRUE` if overflow detected, `FALSE` otherwise.

**Inner mechanism:** Reads the overflow pattern from the template, extracts the error or stop reason from the response using `array_get_path()`, and checks if it matches the pattern.

```php
if ($this->context_overflow($response)) {
    $this->context_compact($target);
}
```

### context_compact (private)

Compacts tool result entries in the message list to free context space.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$target` | `int` | Target number of bytes to free |

**Return:** `int` — Total bytes freed.

**Inner mechanism:** Iterates over compactable entries, calls `message_compact()` on each, records compression operations in context, and stops when the target is reached.

```php
$freed = $this->context_compact(1024);
```

### message_add (private)

Adds a message to both the in-memory list and the persistent context file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `mixed` | The message data |
| `$from` | `string\|null` | Source identifier |
| `$type` | `string` | Message type (default: `"message"`) |

**Return:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanism:** Calls `context_add()` to persist the entry, then `message_append()` to add it to the in-memory list.

```php
$this->message_add($prompt, "system");
```

### message_append (private)

Appends a message to the in-memory list and marks it as compactable if it's a tool result.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `mixed` | The message data |
| `$type` | `string` | Message type |
| `$index` | `int` | Byte offset index |

**Inner mechanism:** If type is `"result"`, adds the index to the compactable list. Stores the message in the message array at the given index.

```php
$this->message_append($data, "result", 1024);
```

### message_compact (private)

Replaces large content blocks in a tool result message with a compact placeholder.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The message index to compact |

**Return:** `int` — Bytes freed by compaction.

**Inner mechanism:** Reads the compact configuration from the template (list field and text path), traverses the content structure, and replaces any content block longer than the compact text with `CMS_AGENT_COMPACT_TEXT`.

```php
$freed = $this->message_compact(2048);
```

### run_queue (private)

Creates or resumes a daemon task to execute the agent run.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `string\|null` | Initial message (for new runs) |
| `$user` | `string\|null` | Sender username |

**Return:** `bool` — `TRUE` if daemon task created or resumed, `FALSE` on failure.

**Inner mechanism:** Generates a PHP daemon script that instantiates a new agent, restores its state, and calls `run_start()`. The script is registered with `cms_daemon()` using a unique ID. If the daemon task already exists (resumed run), returns whether it still exists.

```php
$this->run_queue($message, $user); // Called from send_message()
```

### run_start

Main execution loop for the agent run. Handles MCP setup, context restoration, and the request-response cycle.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `string\|null` | Initial user message |
| `$user` | `string\|null` | Sender username |

**Return:** `string` — The finish reason (e.g., `"stop"`, `"interrupted"`, `"request_limit"`).

**Inner mechanism:**
1. Generates a run ID if needed
2. Clears interrupt signals
3. Issues an MCP authorization token
4. Sets up shutdown handler for resume capability
5. Configures MCP execution mode (provider-side or PWNC-side)
6. Restores conversation context
7. Seeds the conversation with system prompt and user message if new
8. Enters the main loop:
   - Checks for interrupts
   - Imports new context entries
   - Sends request to provider
   - Handles context overflow via compaction
   - Processes the response (extracts values, adds assistant message)
   - Executes tool calls or stops based on the action
9. Returns the finish reason

```php
// Called internally by the daemon task
$reason = $this->run_start("Hello agent", "alice");
```

### run_stop (private)

Finalizes the agent run with a finish reason.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$finish_reason` | `string` | Why the run stopped |

**Return:** `string` — The finish reason.

**Inner mechanism:** Records the finish reason, writes a stop entry to context, and revokes the MCP token.

```php
return $this->run_stop("interrupted");
```

### run_interrupted (private)

Checks whether an interrupt signal has been set for this run.

**Return:** `bool` — `TRUE` if interrupted, `FALSE` otherwise.

**Inner mechanism:** Checks the internal `$interrupted` flag first, then checks the `agent.interrupt.{id}` flag. If set, deletes the flag and sets the internal flag.

```php
if ($this->run_interrupted()) return $this->run_stop("interrupted");
```

### run_shutdown (private)

Shutdown handler that ensures the run is properly stopped and can be resumed.

**Inner mechanism:** If the run hasn't finished normally, stops it with `"killed"`. If resume count is under the limit, creates a new daemon task to resume the run.

```php
register_shutdown_function(function() { $this->run_shutdown(); });
```

### mcp_execute (private)

Executes MCP tool calls from the provider response.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$call` | `array` | Tool call data from the provider response |

**Return:** `bool` — `TRUE` on success, `FALSE` if no valid tool calls.

**Inner mechanism:**
1. Iterates over tool calls from the provider response
2. Extracts tool name, ID, and arguments using template paths
3. Checks for interrupt signals
4. Calls the MCP tool via `mcp_client->tools_call()`
5. Records tool execution metadata in context
6. Builds result blocks using template placeholders
7. Adds results to the message list (grouped or ungrouped)

```php
$this->mcp_execute($value["call"] ?? []);
```

### replace_placeholder (private)

Recursively replaces `%placeholder%` tokens in arrays, strings, or scalar values.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `mixed` | The template data (string, array, or scalar) |
| `$replacement` | `array` | Key-value map of replacement values |

**Return:** `mixed` — The data with placeholders replaced.

**Inner mechanism:**
- For strings: If the entire string is a single placeholder (`%key%`), returns the replacement value directly. Otherwise, performs partial replacement via `preg_replace_callback()`.
- For arrays: Recursively processes each value. Keys starting with `[` and ending with `]` are treated as optional fields and omitted if the value is blank.
- For scalars: Returns as-is.

```php
$result = $this->replace_placeholder(
    $this->template["url"],
    ["endpoint" => "https://api.example.com", "model" => "gpt-4"]
);
```

### generate_id (static)

Generates a unique run identifier.

**Return:** `string` — A timestamp-based unique ID.

**Inner mechanism:** Combines a formatted date string with a unique ID from `unique_id()`.

```php
$id = agent::generate_id();
// Returns: "2026-01-15_10-30-00_abc123def456"
```

### generate_user_prompt (private)

Formats a user message into the provider's expected prompt format.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$message` | `string` | The raw user message |
| `$user` | `string\|null` | Username of the sender |
| `$action` | `string` | Action verb (default: `"wrote"`) |

**Return:** `array` — The formatted prompt with placeholders replaced.

**Inner mechanism:** If a user is provided, prefixes the message with a formatted attribution. Then applies template placeholder replacement using the user template and option values.

```php
$prompt = $this->generate_user_prompt("Hello", "alice", "wrote");
// Produces: "`alice` (Alice Smith) wrote: Hello"
```


<!-- HASH:54c6f10a89247cee5966c2f7308385e9 -->
