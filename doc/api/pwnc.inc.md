# PWNC API Documentation

[← Index](README.md) | [`pwnc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/pwnc.inc)

- **Version:** `26.9.11.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC Core Initialization and Utility Framework (`pwnc.inc`)

## Overview

`pwnc.inc` is the central bootstrap file for the PWNC Web Platform. It initializes the runtime environment, defines core constants, sets up error handling, manages user authentication, handles language selection, loads system libraries, generates URLs, manages caching, and orchestrates background daemon tasks.

This file is automatically included by all entry points (e.g., `index.php`, `interface.php`) and must not be executed directly.

---

## Namespace Isolation

The entire file is wrapped in an immediately-invoked function expression (IIFE) to isolate its scope and prevent global namespace pollution.

```php
(function() {
    // ... all code here ...
})();
```

---

## Constant Overloading Functions

These functions override PHP's built-in `constant()`, `define()`, and `defined()` within the `cms` namespace to automatically prefix constant names with the namespace.

### `constant($name)`

**Purpose**: Retrieves a constant value from the `cms` namespace.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$name`   | string   | Name of the constant (without namespace) |

**Returns**: `mixed` – The value of the constant.

**Example**:
```php
echo constant("CMS_VERSION"); // Outputs the PWNC version
```

### `define($name, $value)`

**Purpose**: Defines a constant in the `cms` namespace.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$name`   | string   | Name of the constant                |
| `$value`  | mixed    | Value to assign                     |

**Returns**: `bool` – Whether the definition was successful.

**Example**:
```php
define("CMS_CUSTOM", "my_value");
```

### `defined($name)`

**Purpose**: Checks if a constant is defined in the `cms` namespace.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$name`   | string   | Name of the constant                |

**Returns**: `bool` – True if the constant exists.

**Example**:
```php
if (defined("CMS_VERSION")) {
    echo "Version is set.";
}
```

---

## Software Information Constants

| Constant           | Default Value                          | Description                           |
|--------------------|----------------------------------------|---------------------------------------|
| `CMS_SOFTWARE`     | `"PWNC"`                               | Software name                         |
| `CMS_VERSION`      | File contents or `"?"`                 | Version string                        |
| `CMS_COPYRIGHT`    | `"© 2026 Patrick Heyer"`               | Copyright notice                      |
| `CMS_HOMEPAGE`     | `"https://pwnc.it"`                    | Official homepage                     |
| `CMS_IDENTIFIER`   | `"PWNC/1.0 (+https://pwnc.it)"`       | HTTP User-Agent identifier            |
| `CMS`              | Full descriptive string                | Complete software info                |

---

## Requirements and Resource Limits

### Environment Checks

- CLI execution is blocked.
- Requires PHP ≥ 7.4.0.
- Requires `mysqli` and `pcre` extensions.
- Requires PCRE UTF-8 support.

### Resource Limit Configuration

#### `cms_ini_set_minimum($key, $value)`

**Purpose**: Sets a PHP ini directive only if the requested value exceeds the current setting.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Ini directive name                  |
| `$value`  | string   | Desired value (supports K/M/G suffixes) |

**Returns**: `string|bool` – Previous value or `FALSE` on failure.

**Example**:
```php
cms_ini_set_minimum("memory_limit", "512M");
```

---

## Initial Settings

- Disables user abort handling (`ignore_user_abort(TRUE)`).
- Starts output buffering.
- Sets default timezone to UTC.
- Sets file creation mask to `0002`.

---

## Debugging

### `debug(...$var)`

**Purpose**: Outputs debug information to the browser console using JavaScript `console.group()` and `console.log()`.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$var`    | mixed... | Variable(s) to dump                 |

**Returns**: `void`

**Example**:
```php
debug($myArray, $myObject);
```

---

## Error Handling

### `cms_error($code, $message, $path, $line, $display)`

**Purpose**: Custom error handler that buffers errors and outputs them via browser console or logs them to disk.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$code`   | int      | Error level                         |
| `$message`| string   | Error message                       |
| `$path`   | string   | File path where error occurred      |
| `$line`   | int      | Line number                         |
| `$display`| bool     | Force display                       |

**Returns**: `bool` – Always returns `TRUE`.

### `cms_error_silent($flag)`

**Purpose**: Toggles silent error mode.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$flag`   | bool     | Enable/disable silent mode          |

**Returns**: `bool` – Previous state.

### `cms_shutdown()`

**Purpose**: Registered shutdown function that captures fatal errors and flushes the error buffer.

**Returns**: `void`

---

## Input Preprocessing

### `cms_request_var($name)`

**Purpose**: Normalizes and returns a request variable from `$_POST`, `$_GET`, or `$_COOKIE`.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$name`   | string   | Variable name                       |

**Returns**: `mixed` – Normalized value.

### `cms_initialize_globals()`

**Purpose**: Loads normalized request data into the global scope.

**Returns**: `void`

### `cms_utf8_normalize($value)`

**Purpose**: Normalizes line breaks and repairs invalid UTF-8 sequences.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$value`  | mixed    | Value to normalize                  |

**Returns**: `mixed` – Normalized value.

---

## Identification and Authentication

### `cms_identification()`

**Purpose**: Handles user login, logout, CSRF protection, brute-force protection, and session management.

**Returns**: `void`

**Key Features**:
- Brute-force protection with attempt limiting.
- CSRF token generation and validation.
- Agent authentication via Bearer tokens.
- Session cookie management.

### `cms_generate_id()`

**Purpose**: Generates anonymous client fingerprint and IP hash.

**Returns**: `void`

**Constants Defined**:
- `CMS_USERID`: Anonymous user identifier.
- `CMS_IPHASH`: Obfuscated IP hash.

---

## Language Management

### `cms_language()`

**Purpose**: Initializes language settings based on system configuration, user preference, and browser headers.

**Returns**: `void`

### `cms_language_extract($requested, $supported)`

**Purpose**: Finds the best matching language from supported options using RFC 9110/BCP 47 parsing.

| Parameter     | Type     | Description                         |
|---------------|----------|-------------------------------------|
| `$requested`  | string   | Requested languages (e.g., `Accept-Language` header) |
| `$supported`  | string   | Comma-separated list of supported languages |

**Returns**: `string|null` – Best matching language code.

### `cms_language_parse($string)`

**Purpose**: Parses an RFC 9110/BCP 47 language string into structured components.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$string` | string   | Language string to parse            |

**Returns**: `array` – Parsed language components with priority values.

---

## Library Loading

### `cms_load($library, $exit_on_error, $test)`

**Purpose**: Loads a system library file.

| Parameter        | Type     | Description                         |
|------------------|----------|-------------------------------------|
| `$library`       | string   | Library name                        |
| `$exit_on_error` | bool     | Terminate on failure                |
| `$test`          | bool     | Test availability only              |

**Returns**: `bool` – Success status.

### `cms_available($library)`

**Purpose**: Checks if a library is available.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$library`| string   | Library name                        |

**Returns**: `bool`

### `cms_load_system()`

**Purpose**: Loads all system libraries matching `sys.*.inc`.

**Returns**: `void`

---

## Application Management

### `cms_application($application, $instance, $permission, $user)`

**Purpose**: Loads application modules and performs permission checks.

| Parameter     | Type     | Description                         |
|---------------|----------|-------------------------------------|
| `$application`| mixed    | Application name or `TRUE`/`NULL`   |
| `$instance`   | mixed    | Instance name or `TRUE`/`NULL`      |
| `$permission` | string   | Permission to check                 |
| `$user`       | string   | User to check permission for        |

**Returns**: `mixed` – Depends on context (loads module, returns string, or returns bool).

### `cms_instance()`

**Purpose**: Returns the current instance name.

**Returns**: `string`

### `cms_permission($permission, $application, $instance, $user)`

**Purpose**: Wrapper for permission checks.

| Parameter     | Type     | Description                         |
|---------------|----------|-------------------------------------|
| `$permission` | string   | Permission name                     |
| `$application`| bool     | Include application context         |
| `$instance`   | bool     | Include instance context            |
| `$user`       | string   | User to check                       |

**Returns**: `bool`

---

## URL Functions

### `cms_url($address, $param, $omit_param)`

**Purpose**: Generates URLs with merged parameters and CSRF protection.

| Parameter     | Type     | Description                         |
|---------------|----------|-------------------------------------|
| `$address`    | mixed    | Address or array of parameters      |
| `$param`      | mixed    | Parameters to add/overwrite         |
| `$omit_param` | bool     | Omit stored parameters              |

**Returns**: `string|false` – Generated URL or `FALSE` on parse error.

### `cms_param($value, $key, $omit_token)`

**Purpose**: Manages static parameter state and generates querystrings.

| Parameter     | Type     | Description                         |
|---------------|----------|-------------------------------------|
| `$value`      | mixed    | Value to set, array to merge, or `TRUE`/`FALSE` for retrieval/deletion |
| `$key`        | mixed    | Key name or `TRUE` to omit stored values |
| `$omit_token` | bool     | Omit CSRF token                     |

**Returns**: `mixed` – Depends on context.

### `cms_build_url($parts)`

**Purpose**: Builds a valid URL from parsed components.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$parts`  | array    | URL components (scheme, host, path, etc.) |

**Returns**: `string`

---

## Cache Data Storage

### `cms_cache($key, $value, $permanent, $ttl)`

**Purpose**: Dual-layer cache (RAM + permanent storage).

| Parameter   | Type     | Description                         |
|-------------|----------|-------------------------------------|
| `$key`      | string   | Cache key                           |
| `$value`    | mixed    | Value to store, `""` to delete      |
| `$permanent`| bool     | Store permanently                   |
| `$ttl`      | int      | Time-to-live in seconds             |

**Returns**: `mixed` – Stored value, `TRUE` on store/delete, `FALSE` on failure.

### `cms_cache_delete($key, $permanent)`

**Purpose**: Deletes cache entries.

| Parameter   | Type     | Description                         |
|-------------|----------|-------------------------------------|
| `$key`      | mixed    | Key or array of keys                |
| `$permanent`| bool     | Delete permanent storage too        |

**Returns**: `bool`

### `cms_cache_sync(&$variable, $key, $default, $load_on_empty, $no_store)`

**Purpose**: Initializes a variable with a cached value or updates the cache.

| Parameter      | Type     | Description                         |
|----------------|----------|-------------------------------------|
| `$variable`    | mixed    | Variable to sync (by reference)     |
| `$key`         | string   | Cache key                           |
| `$default`     | mixed    | Default value                       |
| `$load_on_empty`| bool    | Load even if variable is empty      |
| `$no_store`    | bool     | Don't store back to cache           |

**Returns**: `mixed` – The synced variable.

### `cms_cache_init(&$variable, $key, $default, $load_on_empty)`

**Purpose**: Initializes a variable with a cached value (without storing back).

| Parameter      | Type     | Description                         |
|----------------|----------|-------------------------------------|
| `$variable`    | mixed    | Variable to initialize              |
| `$key`         | string   | Cache key                           |
| `$default`     | mixed    | Default value                       |
| `$load_on_empty`| bool    | Load even if variable is empty      |

**Returns**: `mixed`

### `cms_cache_notouch($key)`

**Purpose**: Retrieves a cache value without updating its timestamp.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Cache key                           |

**Returns**: `mixed`

### `cms_cache_time($key)`

**Purpose**: Returns the last modification time of a cache entry.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Cache key                           |

**Returns**: `int|false`

### `cms_cache_touch($key)`

**Purpose**: Updates the timestamp of a cache entry.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Cache key                           |

**Returns**: `bool`

### `cms_cache_clean($path, $force)`

**Purpose**: Removes expired cache files recursively.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$path`   | string   | Directory path to clean             |
| `$force`  | bool     | Force removal of all files          |

**Returns**: `bool`

---

## Daemon Task Management

### `cms_daemon($code, $id, $interval, $status)`

**Purpose**: Queues a background task for asynchronous execution.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$code`   | string   | PHP code to execute                 |
| `$id`     | string   | Unique task identifier              |
| `$interval`| int     | Minimum interval between executions |
| `$status` | string   | Human-readable status message       |

**Returns**: `bool`

### `cms_daemon_status($value)`

**Purpose**: Sets or retrieves the daemon worker status log.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$value`  | string   | Status message to append            |

**Returns**: `string|bool`

### `cms_daemon_exists($id, $running)`

**Purpose**: Checks if a task is queued or running.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$id`     | string   | Task identifier                     |
| `$running`| bool     | Check if currently running          |

**Returns**: `bool`

### `cms_daemon_running($id)`

**Purpose**: Checks if a task is currently running.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$id`     | string   | Task identifier                     |

**Returns**: `bool`

### `cms_daemon_remove($id)`

**Purpose**: Removes a queued task.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$id`     | string   | Task identifier                     |

**Returns**: `bool`

### `cms_daemon_run()`

**Purpose**: Starts the asynchronous background worker.

**Returns**: `bool`

---

## Flag Management

### `cms_flag_set($key, $mode)`

**Purpose**: Sets, gets, or deletes a flag file.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Flag key                            |
| `$mode`   | int      | 0=set, 1=get, 2=delete              |

**Returns**: `bool`

### `cms_flag_get($key)`

**Purpose**: Checks if a flag is set.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Flag key                            |

**Returns**: `bool`

### `cms_flag_del($key)`

**Purpose**: Deletes a flag.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$key`    | string   | Flag key                            |

**Returns**: `bool`

---

## Miscellaneous Utilities

### `cms_set_cookie($array)`

**Purpose**: Sets or deletes session cookies.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$array`  | array    | Key-value pairs of cookies          |

**Returns**: `bool`

### `cms_salt($value)`

**Purpose**: Generates a salt that changes every 60 minutes.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$value`  | string   | Optional value to incorporate       |

**Returns**: `string` – Binary salt.

### `cms_email_agent()`

**Purpose**: Initializes the system email agent address.

**Returns**: `void`

### `cms_mcp_initialize()`

**Purpose**: Initializes the stateless MCP (Model Context Protocol) interface.

**Returns**: `void`

### `cms_mcp_process()`

**Purpose**: Processes incoming MCP requests.

**Returns**: `void`

### `cms_trusted_proxies()`

**Purpose**: Resolves the correct client IP behind trusted proxies.

**Returns**: `void`

### `cms_ip_in_cidr($ip, $cidr)`

**Purpose**: Checks if an IP address is within a CIDR range.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$ip`     | string   | IP address to check                 |
| `$cidr`   | string   | CIDR notation (e.g., `192.168.0.0/16`) |

**Returns**: `bool`

### `cms_path_urlencode($string)`

**Purpose**: Encodes a string for safe use in file paths per RFC 1738.

| Parameter | Type     | Description                         |
|-----------|----------|-------------------------------------|
| `$string` | string   | String to encode                    |

**Returns**: `string`

---

## System Constants

### Paths

| Constant             | Description                          |
|----------------------|--------------------------------------|
| `CMS_PATH`           | Base installation path               |
| `CMS_SYSTEM_PATH`    | System libraries path                |
| `CMS_MODULES_PATH`   | Modules path                         |
| `CMS_INTERFACE_PATH` | Interface modules path               |
| `CMS_DESKTOP_PATH`   | Desktop modules path                 |
| `CMS_IMAGES_PATH`    | Images path                          |
| `CMS_ROOT_PATH`      | Root path                            |
| `CMS_DATA_PATH`      | Data directory path                  |

### URLs

| Constant             | Description                          |
|----------------------|--------------------------------------|
| `CMS_PROTOCOL`       | `http` or `https`                    |
| `CMS_PORT`           | Server port                          |
| `CMS_HOST`           | Full host URL                        |
| `CMS_DOMAIN`         | Domain name                          |
| `CMS_ACTIVE_URL`     | Current script URL                   |
| `CMS_URL`            | Base URL                             |
| `CMS_MODULES_URL`    | Modules URL                          |
| `CMS_IMAGES_URL`     | Images URL                           |
| `CMS_JAVA_URL`       | Java resources URL                   |
| `CMS_JAVASCRIPT_URL` | JavaScript URL                       |
| `CMS_SOUNDS_URL`     | Sounds URL                           |
| `CMS_ROOT_URL`       | Root URL                             |
| `CMS_RELATIVE_URL`   | Relative URL                         |
| `CMS_DATA_URL`       | Data directory URL                   |

### Configuration

| Constant                  | Default Value                              | Description                           |
|---------------------------|--------------------------------------------|---------------------------------------|
| `CMS_APACHE`              | `function_exists("apache_get_version")`    | Apache detection                      |
| `CMS_CACHE_TTL`           | `2592000` (30 days)                        | Cache expiration time                 |
| `CMS_USER_AGENT`          | PWNC User-Agent string                     | HTTP User-Agent                       |
| `CMS_LOGIN_ATTEMPT_MAX`   | `5`                                        | Max login attempts                    |
| `CMS_LOGIN_BLOCK_TIME`    | `1800` (30 minutes)                        | Login block duration                  |
| `CMS_PERMISSION_ALWAYS`   | `"identification|index|check|mcp|security"`| Always-permitted applications         |

### Regex Patterns

| Constant             | Description                              |
|----------------------|------------------------------------------|
| `CMS_REGEX_MATTER`   | Characters that form word matter         |
| `CMS_REGEX_JOINT`    | Joint characters between words           |
| `CMS_REGEX_SEPARATOR`| Separator characters                     |
| `CMS_REGEX_WORD`     | Complete word pattern                    |
| `CMS_REGEX_BORDER`   | Word border pattern                      |

### Security Events

| Constant                 | Value | Description                     |
|--------------------------|-------|---------------------------------|
| `CMS_SECURITY_EVENT_CSRF`| `1`   | CSRF protection event         |

---

## HTML Constants

| Constant           | Description                              |
|--------------------|------------------------------------------|
| `CMS_DOCTYPE_HTML` | HTML5 doctype                            |
| `CMS_BOT_CHECK`    | Bot check preload link                   |
| `CMS_HTML_HEADER`  | Standard HTML header block               |
| `CMS_JAVASCRIPT`   | JavaScript includes                      |
| `CMS_STYLESHEET`   | Stylesheet link                          |
| `CMS_CLASS`        | CSS class for current application        |

---

## HTTP Headers

Sets standard HTTP headers including:
- Cache-Control
- Content-Type
- Date
- Last-Modified
- Referrer-Policy
- X-Content-Type-Options
- X-Frame-Options
- X-Generator

---

## Daemon Tasks

Queues periodic background tasks:
1. **Cache cleanup** (every hour)
2. **Log maintenance** (every 5 minutes)
3. **Search update processing** (every 5 minutes)
4. **Search score processing** (every hour)
5. **Memory cleanup** (every hour)

---

## Initialization Sequence

1. Load system libraries
2. Generate user ID
3. Initialize language
4. Initialize email agent
5. Initialize MCP
6. User identification
7. Process MCP
8. Register global variables
9. Add instance to default parameters


<!-- HASH:fb84173b05d96d24abb48ceb65c94c70 -->
