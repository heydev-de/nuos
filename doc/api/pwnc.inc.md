# PWNC API Documentation

[← Index](README.md) | [`pwnc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/pwnc.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## pwnc.inc

The `pwnc.inc` file is the core bootstrap and initialization file for the PWNC Web Platform. It establishes the runtime environment, defines critical constants, sets up error handling, manages user authentication, handles language selection, provides caching mechanisms, and orchestrates background daemon tasks. This file is loaded by every entry point (interface, desktop, MCP) and must not be executed directly.

---

## Namespace Isolation

The entire file is wrapped in an immediately-invoked function expression (IIFE) within the `cms` namespace. This isolates all functions and constants from the global scope, preventing naming collisions.

### constant

```php
function constant($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to retrieve |

**Returns:** `mixed` — The value of the namespaced constant.

**Purpose:** Overloads PHP's built-in `constant()` function to automatically resolve constant names within the `cms` namespace.

**Usage:**
```php
// Resolves to cms\CMS_VERSION
$version = constant("CMS_VERSION");
```

### define

```php
function define($name, $value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to define |
| `$value` | mixed | The value to assign |

**Returns:** `bool` — Whether the constant was successfully defined.

**Purpose:** Overloads PHP's `define()` to automatically prefix constant names with the `cms` namespace.

### defined

```php
function defined($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to check |

**Returns:** `bool` — Whether the namespaced constant is defined.

**Purpose:** Overloads PHP's `defined()` to check for constants within the `cms` namespace.

---

## Software Information Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_SOFTWARE` | `"PWNC"` | Software identifier |
| `CMS_VERSION` | file contents or `"?"` | Version from `version.txt` |
| `CMS_COPYRIGHT` | `"© 2026 Patrick Heyer"` | Copyright string |
| `CMS_HOMEPAGE` | `"https://pwnc.it"` | Official homepage |
| `CMS_IDENTIFIER` | `CMS_SOFTWARE . "/1.0 (+https://pwnc.it)"` | HTTP User-Agent identifier |
| `CMS` | Full descriptive string | Complete software identification |

---

## Requirements & Resource Limits

### cms_ini_set_minimum

```php
function cms_ini_set_minimum($key, $value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The php.ini directive key |
| `$value` | string | The desired minimum value (e.g., `"256M"`, `"600"`) |

**Returns:** `mixed` — The old value on success, `FALSE` on failure.

**Purpose:** Sets a php.ini directive only if the current value is lower than the specified minimum. Parses shorthand notation (K, M, G) for memory and size values.

**Usage:**
```php
// Ensures memory_limit is at least 256M
cms_ini_set_minimum("memory_limit", "256M");
```

---

## Debugging

### debug

```php
function debug(...$var)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$var` | mixed... | Variable number of values to dump |

**Returns:** `void`

**Purpose:** Outputs debug information to the browser's JavaScript console using `var_dump()`. Groups output with file, line, and function context. Uses a static counter to number debug calls.

**Usage:**
```php
debug($user, $permissions, $queryResult);
// Outputs to browser console with context information
```

---

## Error Handling

### cms_error

```php
function cms_error($code, $message, $path = NULL, $line = NULL, $display = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$code` | int | PHP error code (e.g., `E_WARNING`, `E_NOTICE`) or `-1` to flush buffer |
| `$message` | string | Error message |
| `$path` | string\|NULL | File path where error occurred |
| `$line` | int\|NULL | Line number |
| `$display` | bool\|NULL | Force display mode |

**Returns:** `bool` — Always `TRUE` (errors are buffered, not suppressed)

**Purpose:** Custom error handler that buffers errors and outputs them to the browser console. In development mode, includes full stack traces with argument details. Logs errors to `CMS_DATA_PATH/#log/error.txt` with file locking.

**Inner Mechanisms:**
- Checks for silent mode via `cms_cache("cms.error.silent")`
- Determines dev mode based on IP, admin status, or permissions
- Buffers errors in a static array
- On flush (`$code === -1`), outputs all buffered errors as grouped console messages
- Logs errors with timestamps using atomic file operations (temp file + rename)

### cms_error_silent

```php
function cms_error_silent($flag = TRUE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$flag` | bool | Whether to enable silent mode |

**Returns:** `bool` — Previous silent state

**Purpose:** Toggles error silencing via the cache system. When enabled, `cms_error()` returns immediately without processing.

### cms_shutdown

```php
function cms_shutdown()
```

**Returns:** `void`

**Purpose:** Registered as a shutdown function. Captures fatal errors via `error_get_last()` and passes them to `cms_error()`. Then flushes the error buffer by calling `cms_error(-1, "")`.

---

## Input Preprocessing

### cms_request_var

```php
function cms_request_var($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the request variable |

**Returns:** `mixed` — The normalized value from `$_POST`, `$_GET`, `$_COOKIE`, or `NULL`

**Purpose:** Retrieves a request variable from any superglobal, normalizing it for UTF-8 safety.

### cms_initialize_globals

```php
function cms_initialize_globals()
```

**Returns:** `void`

**Purpose:** Merges all request variable names into `$GLOBALS`, normalizing each value. Also processes `$_FILES` entries, flattening them into global variables with prefixed names (e.g., `file_tmp_name`).

### cms_utf8_normalize

```php
function cms_utf8_normalize($value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | mixed | The value to normalize (string or array) |

**Returns:** `mixed` — The normalized value

**Purpose:** Recursively normalizes values by converting line breaks to `\n` and repairing invalid UTF-8 sequences using `utf8_normalize()`.

---

## Identification (Authentication)

### cms_identification

```php
function cms_identification()
```

**Returns:** `void`

**Purpose:** Handles the complete user authentication lifecycle:
- Processes login/logout requests
- Implements brute-force protection with attempt counting
- Supports daemon and agent (MCP) authentication
- Generates CSRF tokens
- Sets user-related constants (`CMS_USER`, `CMS_PASSWORD`, `CMS_TOKEN`, `CMS_SUPERUSER`, `CMS_NAME`, `CMS_EMAIL`, `CMS_TIMEZONE`, `CMS_PROFILE`)
- Redirects unauthenticated users to login or security pages

**Inner Mechanisms:**
- Uses `permission` class for user verification
- Tracks failed login attempts per IP address with configurable limits (`CMS_LOGIN_ATTEMPT_MAX`, `CMS_LOGIN_BLOCK_TIME`)
- Generates security tokens stored in cache with a security key cookie
- For agents (MCP), validates Bearer tokens
- For daemons, uses a pre-configured password hash

**Usage:** Called automatically during initialization. No direct invocation needed.

### cms_generate_id

```php
function cms_generate_id()
```

**Returns:** `void`

**Purpose:** Generates anonymous client fingerprints:
- `CMS_USERID`: A 32-character session ID from cookie or a RIPEMD-128 hash of browser fingerprint + IP
- `CMS_IPHASH`: An obfuscated IP hash for security checks

**Inner Mechanisms:**
- Uses a salt that changes every 60 minutes (via `cms_salt()`)
- Falls back to browser fingerprint when cookies are disabled
- The session cookie line is commented out for GDPR compliance

---

## Language

### cms_language

```php
function cms_language()
```

**Returns:** `void`

**Purpose:** Initializes language settings:
- Reads available languages from system configuration
- Determines the active language from user selection, cache, or `Accept-Language` header
- Defines `CMS_LANGUAGE_ENABLED`, `CMS_LANGUAGE_DEFAULT`, `CMS_LANGUAGE`
- Loads the appropriate language file

**Inner Mechanisms:**
- Uses `cms_language_extract()` to find the best match from `Accept-Language`
- Caches user language preference per `CMS_USERID`
- Falls back to default language if no match is found

### cms_language_extract

```php
function cms_language_extract($requested, $supported)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$requested` | string | RFC 9110/BCP 47 language string from `Accept-Language` header |
| `$supported` | string | Comma-separated list of supported languages |

**Returns:** `string\|NULL` — The best matching language code, or `NULL` if no match

**Purpose:** Finds the best matching language from a list of supported languages based on the client's `Accept-Language` header. Prioritizes exact matches and higher specificity.

### cms_language_parse

```php
function cms_language_parse($string)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Raw `Accept-Language` header value |

**Returns:** `array` — Parsed language entries sorted by priority (q-value)

**Purpose:** Parses an RFC 9110/BCP 47 language string into structured components:

| Key | Description |
|-----|-------------|
| `code` | ISO 639 language code (2-3 chars) |
| `script` | ISO 15924 script subtag (4 chars) |
| `region` | ISO 3166-1 alpha-2 or UN M.49 code |
| `variant` | Registered language variants |
| `q` | Priority value (0.0 to 1.0) |

---

## Libraries

### cms_load

```php
function cms_load($library, $exit_on_error = FALSE, $test = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$library` | string | Library name (without `lib.` prefix or `.inc` suffix) |
| `$exit_on_error` | bool | If `TRUE`, terminate with error message on failure |
| `$test` | bool | If `TRUE`, only test availability without loading |

**Returns:** `bool` — Whether the library is available/loaded

**Purpose:** Loads a library file from `CMS_SYSTEM_PATH`. Uses a static cache to prevent duplicate loading.

**Usage:**
```php
// Load the search library
if (cms_load("search")) {
    $search = new search();
}

// Test if library exists without loading
if (cms_load("memory", FALSE, TRUE)) {
    // Library is available
}
```

### cms_available

```php
function cms_available($library)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$library` | string | Library name to check |

**Returns:** `bool` — Whether the library file exists

**Purpose:** Convenience wrapper for `cms_load($library, FALSE, TRUE)`.

### cms_load_system

```php
function cms_load_system()
```

**Returns:** `void`

**Purpose:** Loads all system libraries matching the pattern `sys.*.inc` from `CMS_SYSTEM_PATH`. Uses `require_once` to prevent duplicate loading.

---

## Applications

### cms_application

```php
function cms_application($application = NULL, $instance = NULL, $permission = NULL, $user = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$application` | string\|TRUE\|NULL | Application name, `TRUE` for current, `NULL` for context |
| `$instance` | string\|TRUE\|NULL | Instance name, `TRUE` for current, `NULL` for context |
| `$permission` | string\|NULL | Permission to test |
| `$user` | string\|NULL | User to test permission for (defaults to current user) |

**Returns:** `mixed` — `bool` for permission checks, `string` for context information

**Purpose:** Multi-purpose function for:
1. **Loading applications** — Loads module files from `CMS_MODULES_PATH/#module/`
2. **Permission checks** — Tests permissions at various levels (application, instance, or combined)
3. **Context retrieval** — Returns current application, instance, or combined identifier

**Usage:**
```php
// Load the "blog" application
cms_application("blog");

// Check if current user has "edit" permission for current app.instance
if (cms_application(TRUE, TRUE, "edit")) {
    // User can edit
}

// Get current application name
$app = cms_application(TRUE);

// Check permission for specific user
if (cms_application(TRUE, TRUE, "admin", "john")) {
    // John has admin permission
}
```

### cms_instance

```php
function cms_instance()
```

**Returns:** `string` — The current instance name

**Purpose:** Convenience wrapper returning the current application instance.

### cms_permission

```php
function cms_permission($permission, $application = TRUE, $instance = TRUE, $user = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$permission` | string | The permission to test |
| `$application` | bool\|TRUE | Application context (`TRUE` = current) |
| `$instance` | bool\|TRUE | Instance context (`TRUE` = current) |
| `$user` | string\|NULL | User to test for (defaults to current) |

**Returns:** `bool` — Whether the permission is granted

**Purpose:** Convenience wrapper for `cms_application()` focused on permission testing.

**Usage:**
```php
// Test if current user has "write" permission for current app.instance
if (cms_permission("write")) {
    // Allowed
}

// Test for specific user
if (cms_permission("write", TRUE, TRUE, "alice")) {
    // Alice has write permission
}
```

---

## URL Functions

### cms_url

```php
function cms_url($address = NULL, $param = NULL, $omit_param = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$address` | string\|array\|NULL | Target URL or array of parameters |
| `$param` | array\|NULL | Parameters to add/overwrite |
| `$omit_param` | bool | If `TRUE`, omit stored parameters |

**Returns:** `string\|FALSE` — Generated URL or `FALSE` on parse failure

**Purpose:** Generates URLs by merging local parameters with global state. Handles external URLs, executable detection (`.php` files), and CSRF token injection.

**Usage:**
```php
// Generate URL for current page with additional params
$url = cms_url(NULL, ["page" => 2, "sort" => "name"]);

// Generate URL for specific address
$url = cms_url("/module/interface.php", ["ifc_page" => "settings"]);
```

### cms_param

```php
function cms_param($value = NULL, $key = NULL, $omit_token = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | mixed | Value to set, key to retrieve, array to merge, `TRUE` for all, `FALSE` to clear |
| `$key` | string\|TRUE\|NULL | Key name, `TRUE` to merge without overwriting |
| `$omit_token` | bool | If `TRUE`, omit CSRF token from querystring |

**Returns:** `mixed` — Querystring, stored value, or `bool` for set/delete operations

**Purpose:** State manager and querystring generator with CSRF protection. Maintains a static parameter store that persists across calls.

**Usage:**
```php
// Store a parameter
cms_param(["filter" => "active"], "search");

// Generate querystring with current + new params
$query = cms_param(["page" => 2]);

// Retrieve all stored parameters
$all = cms_param(TRUE);

// Clear all parameters
cms_param(FALSE);
```

### cms_build_url

```php
function cms_build_url($parts)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parts` | array | URL components (scheme, host, port, user, pass, path, query, fragment) |

**Returns:** `string` — A fully constructed URL

**Purpose:** Builds a valid URL string from an array of URL parts, handling encoding and component assembly.

---

## Cache Data Storage

### cms_cache

```php
function cms_cache($key = NULL, $value = NULL, $permanent = FALSE, $ttl = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string\|NULL | Cache key, `NULL` to return all cached values |
| `$value` | mixed | Value to store, `""` to delete, `NULL` to retrieve |
| `$permanent` | bool | If `TRUE`, persist to disk |
| `$ttl` | int\|NULL | Time-to-live in seconds |

**Returns:** `mixed` — Stored value, `bool` for set/delete operations, `array` for all values

**Purpose:** Dual-layer cache system:
- **RAM layer:** Static array for fast in-request access
- **Disk layer:** Serialized files in `CMS_DATA_PATH/#cache/` for persistence

**Inner Mechanisms:**
- Uses RIPEMD-128 hash of key for file path (4-char directory + remainder)
- Tombstone pattern: deleted keys are set to `NULL` in RAM to prevent re-fetching
- Atomic writes via temp file + rename
- Supports both string and serialized values

**Usage:**
```php
// Store a temporary value
cms_cache("user_count", 42);

// Store a permanent value
cms_cache("config", $config, TRUE);

// Retrieve a value
$count = cms_cache("user_count");

// Delete a value
cms_cache("user_count", "");

// Delete both temporary and permanent
cms_cache("user_count", "", TRUE);
```

### cms_cache_delete

```php
function cms_cache_delete($key, $permanent = TRUE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string\|array | Cache key(s) to delete |
| `$permanent` | bool | If `TRUE`, also delete from disk |

**Returns:** `bool` — Success status

**Purpose:** Deletes one or more cache entries. Accepts an array of keys for batch deletion.

### cms_cache_sync

```php
function cms_cache_sync(&$variable, $key, $default = NULL, $load_on_empty = FALSE, $no_store = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$variable` | mixed | Variable to initialize/update (by reference) |
| `$key` | string | Cache key |
| `$default` | mixed | Default value if cache is empty |
| `$load_on_empty` | bool | If `TRUE`, treat empty strings/arrays as empty |
| `$no_store` | bool | If `TRUE`, don't write back to cache |

**Returns:** `mixed` — The variable value

**Purpose:** Synchronizes a variable with cache: loads from cache if undefined/empty, or stores to cache if the variable has a new value.

**Usage:**
```php
$config = NULL;
cms_cache_sync($config, "app_config", [], TRUE);
// $config is now loaded from cache or set to []
```

### cms_cache_init

```php
function cms_cache_init(&$variable, $key, $default = NULL, $load_on_empty = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$variable` | mixed | Variable to initialize (by reference) |
| `$key` | string | Cache key |
| `$default` | mixed | Default value |
| `$load_on_empty` | bool | If `TRUE`, treat empty values as needing initialization |

**Returns:** `mixed` — The variable value

**Purpose:** Convenience wrapper for `cms_cache_sync()` with `$no_store = TRUE` — only loads from cache, never writes back.

### cms_cache_notouch

```php
function cms_cache_notouch($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key |

**Returns:** `mixed` — The cached value

**Purpose:** Retrieves a cache value without updating its access time (unlike the default `cms_cache()` which touches the file).

### cms_cache_time

```php
function cms_cache_time($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key |

**Returns:** `int\|FALSE` — Last modification time or `FALSE` if not found

**Purpose:** Returns the last modification time of a cache entry's file.

### cms_cache_touch

```php
function cms_cache_touch($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key |

**Returns:** `bool` — Success status

**Purpose:** Updates the access time of a cache entry's file.

### cms_cache_clean

```php
function cms_cache_clean($path, $force = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | Directory path to clean |
| `$force` | bool | If `TRUE`, remove all files regardless of age |

**Returns:** `bool` — Success status

**Purpose:** Recursively removes expired cache files from a directory. Files older than `CMS_CACHE_TTL` (30 days) are removed. Empty directories are also cleaned up.

---

## Daemon

### cms_daemon

```php
function cms_daemon($code, $id = NULL, $interval = 0, $status = "")
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$code` | string | PHP code to execute |
| `$id` | string\|NULL | Unique identifier to prevent duplicate queueing |
| `$interval` | int | Minimum seconds between executions |
| `$status` | string | Human-readable status description |

**Returns:** `bool` — Whether the task was queued

**Purpose:** Queues a background task for asynchronous execution. Tasks are stored as PHP files in `CMS_DATA_PATH/#daemon/`.

**Inner Mechanisms:**
- Uses file-based locking to prevent duplicate execution
- Respects interval timing to prevent requeueing
- Creates a `daemon.flag` file to signal pending tasks
- Tasks are executed by `daemon.php` via HTTP

**Usage:**
```php
// Queue a cache cleanup task
cms_daemon(
    "cms_cache_clean(CMS_DATA_PATH . \"#cache/\");",
    "cache.daemon",
    3600,
    "Cache cleanup"
);
```

### cms_daemon_status

```php
function cms_daemon_status($value = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string\|NULL | Status message to set, `NULL` to retrieve |

**Returns:** `string\|bool` — Current status or success

**Purpose:** Sets or retrieves the background worker status. Maintains a rolling log of the last 25 status entries with timestamps.

### cms_daemon_exists

```php
function cms_daemon_exists($id, $running = FALSE)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | string | Task identifier |
| `$running` | bool | If `TRUE`, check if currently executing |

**Returns:** `bool` — Whether the task exists/is running

**Purpose:** Checks if a daemon task is queued. With `$running = TRUE`, also checks for an active lock file.

### cms_daemon_running

```php
function cms_daemon_running($id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | string | Task identifier |

**Returns:** `bool` — Whether the task is currently executing

**Purpose:** Convenience wrapper for `cms_daemon_exists($id, TRUE)`.

### cms_daemon_remove

```php
function cms_daemon_remove($id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | string | Task identifier |

**Returns:** `bool` — Success status

**Purpose:** Removes a queued daemon task and its lock file. Fails if the task is currently running.

### cms_daemon_run

```php
function cms_daemon_run()
```

**Returns:** `bool` — Whether a daemon worker was invoked

**Purpose:** Triggers the asynchronous background worker by making an HTTP request to `daemon.php`. Uses advisory locking to prevent concurrent daemon processes.

---

## Flag

### cms_flag_set

```php
function cms_flag_set($key, $mode = 0)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Flag key |
| `$mode` | int | `0` = set, `1` = get, `2` = delete |

**Returns:** `bool` — Success status

**Purpose:** File-based flag management system. Flags are stored as empty files in `CMS_DATA_PATH/#flag/`. Uses file locking for concurrent access safety.

**Usage:**
```php
// Set a flag
cms_flag_set("maintenance_mode");

// Check if flag is set
if (cms_flag_get("maintenance_mode")) {
    // Maintenance mode active
}

// Remove a flag
cms_flag_del("maintenance_mode");
```

### cms_flag_get

```php
function cms_flag_get($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Flag key |

**Returns:** `bool` — Whether the flag is set

**Purpose:** Convenience wrapper for `cms_flag_set($key, 1)`.

### cms_flag_del

```php
function cms_flag_del($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Flag key |

**Returns:** `bool` — Success status

**Purpose:** Convenience wrapper for `cms_flag_set($key, 2)`.

---

## Miscellaneous

### cms_set_cookie

```php
function cms_set_cookie($array)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | array | Key-value pairs of cookies to set/delete |

**Returns:** `bool` — Success status

**Purpose:** Sets or deletes session cookies with secure defaults:
- `HttpOnly` enabled
- `SameSite=Lax`
- `Secure` flag based on protocol
- Path set to `CMS_RELATIVE_URL`

**Usage:**
```php
// Set cookies
cms_set_cookie([
    "cms_user" => "admin",
    "cms_password" => $hash
]);

// Delete cookies
cms_set_cookie([
    "cms_user" => NULL,
    "cms_password" => NULL
]);
```

### cms_salt

```php
function cms_salt($value = NULL)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string\|NULL | Context value, `NULL` for IP-based salt |

**Returns:** `string` — Binary salt value

**Purpose:** Generates and caches salts that change every 60 minutes. Two modes:
- **IP-based** (`$value = NULL`): Salt tied to client IP
- **User-based** (`$value` provided): Salt tied to `CMS_USERID` + context value

**Inner Mechanisms:**
- Uses `cms_cache()` for persistent storage
- 1048576 possible values per prefix (5 hex chars from RIPEMD-128)
- Cached in static variable for repeated calls within a request

### cms_token_tag

```php
function cms_token_tag()
```

**Returns:** `string` — 64-character hex token

**Purpose:** Generates a cryptographically secure token by combining random bytes with an HMAC tag derived from a stored secret. Used for CSRF protection and agent authentication.

**Inner Mechanisms:**
- Generates 16 random bytes
- Creates a 16-byte HMAC-SHA256 tag using the stored secret
- Returns 64-character hex string (32 bytes random + 16 bytes tag)

### cms_token_check

```php
function cms_token_check($token)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | string | Token to verify |

**Returns:** `bool` — Whether the token is valid

**Purpose:** Verifies a token generated by `cms_token_tag()` by checking the HMAC tag against the stored secret.

### cms_email_agent

```php
function cms_email_agent()
```

**Returns:** `void`

**Purpose:** Initializes the system email agent address from configuration, falling back to `mailagent@` + domain.

### cms_mcp_initialize

```php
function cms_mcp_initialize()
```

**Returns:** `void`

**Purpose:** Initializes the stateless MCP (Model Context Protocol) interface:
- Parses Bearer token from Authorization header
- Validates protocol version and method headers
- Checks origin header for security
- Parses and validates JSON-RPC 2.0 request body
- Validates required metadata fields

### cms_mcp_process

```php
function cms_mcp_process()
```

**Returns:** `void`

**Purpose:** Processes incoming MCP requests by dispatching to the appropriate `mcp::` method based on `CMS_MCP_METHOD`. Handles multi-round tool requests by restoring previous request state.

### cms_trusted_proxies

```php
function cms_trusted_proxies()
```

**Returns:** `void`

**Purpose:** Resolves the correct client IP address when behind trusted proxies. Reads trusted proxy list from `CMS_DATA_PATH/#system/trusted_proxies.txt` and processes `X-Forwarded-For`, `X-Real-IP`, `X-Forwarded-Proto`, and `X-Forwarded-Port` headers.

### cms_ip_in_cidr

```php
function cms_ip_in_cidr($ip, $cidr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ip` | string | IP address to check |
| `$cidr` | string | CIDR notation (e.g., `"192.168.1.0/24"`) |

**Returns:** `bool` — Whether the IP is within the CIDR range

**Purpose:** Checks if an IP address falls within a CIDR range. Supports both IPv4 and IPv6.

### cms_path_urlencode

```php
function cms_path_urlencode($string)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | String to encode |

**Returns:** `string` — URL-encoded string

**Purpose:** Encodes non-alphanumeric characters except `$-_.+!*'(),` according to RFC 1738. Excludes the path separator `/` to keep file paths intact.

---

## System Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_APPLICATION` | dynamic | Current application name |
| `CMS_INSTANCE` | dynamic | Current instance name |
| `CMS_DB_PREFIX` | `"cms_"` | Database table prefix |
| `CMS_IFC_EDITION` | `"ifc"` | Interface edition identifier |
| `CMS_PATH` | dynamic | Core library path |
| `CMS_SYSTEM_PATH` | `CMS_PATH . "#system/"` | System libraries path |
| `CMS_MODULES_PATH` | `CMS_PATH . "module/"` | Modules path |
| `CMS_INTERFACE_PATH` | `CMS_MODULES_PATH . "#interface/"` | Interface modules path |
| `CMS_DESKTOP_PATH` | `CMS_MODULES_PATH . "#desktop/"` | Desktop modules path |
| `CMS_IMAGES_PATH` | `CMS_PATH . "image/"` | Images path |
| `CMS_ROOT_PATH` | `dirname(CMS_PATH)` | Root filesystem path |
| `CMS_DATA_PATH` | `CMS_ROOT_PATH . "data/"` | Data directory path |
| `CMS_PROTOCOL` | `"http"` or `"https"` | Current protocol |
| `CMS_PORT` | dynamic | Server port |
| `CMS_HOST` | dynamic | Full host URL (protocol + host) |
| `CMS_DOMAIN` | dynamic | Domain name only |
| `CMS_ACTIVE_URL` | dynamic | Current script URL |
| `CMS_URL` | dynamic | Base URL |
| `CMS_MODULES_URL` | `CMS_URL . "module/"` | Modules URL |
| `CMS_IMAGES_URL` | `CMS_URL . "image/"` | Images URL |
| `CMS_JAVA_URL` | `CMS_URL . "java/"` | Java URL |
| `CMS_JAVASCRIPT_URL` | `CMS_URL . "javascript/"` | JavaScript URL |
| `CMS_SOUNDS_URL` | `CMS_URL . "sound/"` | Sounds URL |
| `CMS_ROOT_URL` | dynamic | Root URL |
| `CMS_RELATIVE_URL` | dynamic | Relative URL path |
| `CMS_DATA_URL` | `CMS_ROOT_URL . "data/"` | Data URL |
| `CMS_APACHE` | bool | Whether running under Apache |
| `CMS_CACHE_TTL` | `2592000` (30 days) | Cache time-to-live |
| `CMS_USER_AGENT` | dynamic | HTTP User-Agent string |
| `CMS_LOGIN_ATTEMPT_MAX` | `5` | Max failed login attempts |
| `CMS_LOGIN_BLOCK_TIME` | `1800` (30 min) | Login block duration |
| `CMS_PERMISSION_ALWAYS` | `"identification\|index\|check\|mcp\|security"` | Always-permitted applications |
| `CMS_REGEX_MATTER` | pattern | Regex for word matter characters |
| `CMS_REGEX_JOINT` | pattern | Regex for word joint characters |
| `CMS_REGEX_SEPARATOR` | pattern | Regex for word separator characters |
| `CMS_REGEX_WORD` | pattern | Complete word regex |
| `CMS_REGEX_BORDER` | pattern | Word border regex |
| `CMS_SECURITY_EVENT_CSRF` | `1` | CSRF security event type |

---

## HTML Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DOCTYPE_HTML` | `"<!DOCTYPE HTML>"` | HTML5 doctype |
| `CMS_BOT_CHECK` | dynamic | Bot verification preload link |
| `CMS_HTML_HEADER` | dynamic | Standard HTML header elements |
| `CMS_JAVASCRIPT` | dynamic | Standard JavaScript includes |
| `CMS_STYLESHEET` | dynamic | Standard stylesheet link |
| `CMS_CLASS` | dynamic | CSS class for current application |

---

## Initialization Sequence

The file executes the following initialization steps in order:

1. **`cms_load_system()`** — Loads all `sys.*.inc` system libraries
2. **`cms_generate_id()`** — Generates anonymous client fingerprint
3. **`cms_language()`** — Initializes language settings
4. **`cms_email_agent()`** — Sets up system email agent
5. **`cms_mcp_initialize()`** — Initializes MCP protocol
6. **`cms_identification()`** — Handles user authentication
7. **`cms_mcp_process()`** — Processes MCP requests
8. **`cms_initialize_globals()`** — Normalizes and loads request data
9. **`cms_param(CMS_INSTANCE, "cms_instance")`** — Adds instance to default parameters

---

## Daemon Tasks

The file queues several background tasks:

| Task ID | Interval | Description |
|---------|----------|-------------|
| `cache.daemon` | 3600s (1h) | Cache directory cleanup |
| `log.daemon` | 300s (5m) | Log file maintenance |
| `search.daemon.update` | 300s (5m) | Search index updates |
| `search.daemon.score` | 3600s (1h) | Search score computation |
| `memory.daemon.cleanup` | 3600s (1h) | Memory cleanup |

Each task is queued via `cms_daemon()` and executed asynchronously by `daemon.php`.


<!-- HASH:54917f05c2f8da8006b2156bc88f4594 -->
