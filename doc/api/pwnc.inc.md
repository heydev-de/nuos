# PWNC API Documentation

[← Index](README.md) | [`pwnc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/pwnc.inc)

- **Version:** `26.9.22.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## pwnc.inc

The `pwnc.inc` file is the core bootstrap and runtime foundation of the PWNC Web Platform. It is executed at the very beginning of every request and performs critical initialization tasks including:

- **Namespace isolation**: Wraps all core functions in the `cms` namespace to avoid conflicts with user-defined functions.
- **Environment validation**: Checks PHP version, required extensions, and CLI restrictions.
- **Resource configuration**: Sets minimum `max_execution_time`, `memory_limit`, `post_max_size`, etc.
- **Error handling**: Registers a custom error handler and shutdown function that buffers errors and outputs them via browser console.
- **Input normalization**: Normalizes request variables (POST, GET, COOKIE, FILES) into `$GLOBALS` with UTF-8 repair.
- **User identification**: Handles login, logout, CSRF protection, brute-force protection, and agent (MCP) authentication.
- **Language initialization**: Parses `Accept-Language` headers and selects the best matching supported language.
- **Library and application loading**: Provides lazy-loading mechanisms for system libraries and modular applications.
- **URL generation**: Builds URLs with CSRF tokens, querystring merging, and parameter state management.
- **Caching**: Implements a dual-layer cache (in-RAM + filesystem) with TTL support.
- **Daemon scheduling**: Queues background tasks for cache cleanup, log maintenance, search indexing, and memory cleanup.
- **System constants**: Defines dozens of constants for paths, URLs, configuration, regex patterns, and security events.
- **HTTP headers**: Sets appropriate headers for content type, caching, referrer policy, and security.

---

## Namespace Isolation

The entire file is wrapped in an immediately-invoked function expression (IIFE) within the `cms` namespace. This ensures that all functions defined here are scoped to `cms\` and do not pollute the global namespace.

### constant

```php
function constant($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to retrieve. |

**Returns**: `mixed` — The value of the constant `cms\$name`.

**Purpose**: Overloads PHP's built-in `constant()` function so that when called within the `cms` namespace, it automatically prefixes the constant name with the namespace. This allows code to reference constants like `CMS_VERSION` without explicitly writing `cms\CMS_VERSION`.

**Inner mechanism**: Delegates to `\constant(__NAMESPACE__ . "\\$name")`.

**Usage**:
```php
// Within the cms namespace
echo constant("CMS_VERSION"); // Resolves to cms\CMS_VERSION
```

### define

```php
function define($name, $value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to define. |
| `$value` | mixed | The value to assign. |

**Returns**: `bool` — Whether the constant was successfully defined.

**Purpose**: Overloads PHP's `define()` to automatically namespace constant definitions.

**Inner mechanism**: Delegates to `\define(__NAMESPACE__ . "\\$name", $value)`.

**Usage**:
```php
define("CMS_SOFTWARE", "PWNC"); // Defines cms\CMS_SOFTWARE
```

### defined

```php
function defined($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the constant to check. |

**Returns**: `bool` — Whether the constant `cms\$name` is defined.

**Purpose**: Overloads PHP's `defined()` to check for namespaced constants.

**Inner mechanism**: Delegates to `\defined(__NAMESPACE__ . "\\$name")`.

**Usage**:
```php
if (defined("CMS_USER")) exit(); // Checks cms\CMS_USER
```

### json_decode

```php
function json_decode($json, $associative = NULL, $depth = 512, $flags = 0)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$json` | string | — | The JSON string to decode. |
| `$associative` | bool\|null | `NULL` | Whether to return associative arrays. |
| `$depth` | int | `512` | Maximum nesting depth. |
| `$flags` | int | `0` | JSON decode flags. |

**Returns**: `mixed` — The decoded value, with empty objects `{}` preserved as `stdClass` even when `$associative` is `true`.

**Purpose**: A wrapper around PHP's `json_decode` that fixes a known issue where empty JSON objects `{}` are incorrectly converted to empty arrays when `$associative` is `true`. This function detects empty arrays that originated from `{}` and restores them as `stdClass` objects.

**Inner mechanism**:
1. Calls native `json_decode` with the given parameters.
2. If `$associative` is falsy, the result is an array, and the original JSON contains `{}`, it re-decodes the JSON as objects and recursively walks both structures to restore empty `{}` as `stdClass`.
3. Uses a static closure `$fix` that recursively traverses the decoded array and the object-decoded version, replacing empty arrays with their corresponding `stdClass` counterparts.

**Usage**:
```php
$json = '{"items": [], "name": "test"}';
$data = json_decode($json, true);
// $data['items'] will be an empty stdClass, not an empty array
```

---

## Software Information Constants

These constants define the platform's identity and are used throughout the system.

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_SOFTWARE` | `"PWNC"` | Software name. |
| `CMS_VERSION` | File contents of `version.txt` or `"?"` | Version string. |
| `CMS_COPYRIGHT` | `"© 2026 Patrick Heyer"` | Copyright notice. |
| `CMS_HOMEPAGE` | `"https://pwnc.it"` | Official homepage URL. |
| `CMS_IDENTIFIER` | `CMS_SOFTWARE . "/1.0 (+" . CMS_HOMEPAGE . ")"` | User-Agent identifier string. |
| `CMS` | Full descriptive string | Combined software info. |

---

## Requirements / Resources

### PHP Version Check

```php
if (PHP_SAPI === "cli") die("CLI execution is not allowed.");
if (version_compare(PHP_VERSION, "7.4.0", "<")) die("PHP7 >= 7.4.0 is required.");
if (! extension_loaded("mysqli")) die("MySQLi support is required.");
if (! extension_loaded("pcre")) die("PCRE support is required.");
if (! preg_match("/\pL/u", "a")) die("PCRE UTF-8 support is required.");
```

**Purpose**: Validates that the runtime environment meets minimum requirements:
- CLI execution is blocked.
- PHP ≥ 7.4.0 is required.
- MySQLi extension is required.
- PCRE extension with UTF-8 support is required.

### cms_ini_set_minimum

```php
function cms_ini_set_minimum($key, $value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The php.ini directive name (e.g., `memory_limit`). |
| `$value` | string | The desired minimum value (e.g., `"256M"`). |

**Returns**: `string\|bool` — The old value if the setting was increased, `FALSE` on failure, or the current value if already sufficient.

**Purpose**: Ensures that PHP resource limits are set to at least the specified minimum. If the current value is lower, it attempts to raise it via `ini_set`.

**Inner mechanism**:
1. Parses the value string (e.g., `"256M"`) into bytes using a unit multiplier array.
2. Retrieves the current `ini_get` value and parses it similarly.
3. If the current value is unlimited (negative), returns it as-is.
4. If the desired value exceeds the current value, calls `ini_set`.
5. Otherwise, returns the current value.

**Usage**:
```php
cms_ini_set_minimum("memory_limit", "256M");
cms_ini_set_minimum("max_execution_time", "600");
```

---

## Initial Settings

| Setting | Value | Description |
|---------|-------|-------------|
| `ignore_user_abort(TRUE)` | — | Continue execution even if client disconnects. |
| `ob_implicit_flush(FALSE)` | — | Disable implicit output flushing. |
| `ob_start()` | — | Start output buffering. |
| `CMS_TIMEZONE_DEFAULT` | `"UTC"` | Default timezone. |
| `umask(0002)` | — | File creation mask for group-writable files. |
| `CMS_MCP` | `FALSE` (if not defined) | Whether MCP (Model Context Protocol) mode is active. |

---

## Debugging

### debug

```php
function debug(...$var)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$var` | mixed... | Variable-length list of values to dump. |

**Returns**: `void`

**Purpose**: Outputs debug information to the browser's JavaScript console. Each call creates a collapsible console group with the file, line, and calling function name, followed by `var_dump` output of all provided values.

**Inner mechanism**:
1. Uses `debug_backtrace` to get the calling file and line.
2. Starts output buffering and calls `var_dump` on each argument.
3. Emits a `<script>` tag that creates a `console.group` with styled headers and logs the dumped output.
4. Increments a static counter for sequential debug call numbering.

**Usage**:
```php
debug($user, $permissions, "Login successful");
// Outputs a styled console group in the browser
```

---

## Error Handling

### cms_error

```php
function cms_error($code, $message, $path = NULL, $line = NULL, $display = NULL)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$code` | int | — | PHP error code (e.g., `E_WARNING`). |
| `$message` | string | — | Error message. |
| `$path` | string\|null | `NULL` | File where the error occurred. |
| `$line` | int\|null | `NULL` | Line number. |
| `$display` | bool\|null | `NULL` | Force display mode. |

**Returns**: `bool` — Always `TRUE` (errors are handled, not propagated).

**Purpose**: Custom error handler that buffers errors and outputs them as styled JavaScript console messages. In development mode, it includes full stack traces with argument details. Errors are also logged to a file with file locking.

**Inner mechanism**:
1. **Silent mode**: If `cms.error.silent` cache key is set, returns `TRUE` immediately.
2. **Error reporting check**: If the error code is not in the current `error_reporting` level, returns `FALSE` (let PHP handle it).
3. **Development mode detection**: Enabled if `$display === TRUE`, remote address is localhost, user is admin, or user has `debug` permission.
4. **Error buffering**: Stores error messages in a static array for batch output.
5. **Error type mapping**: Maps PHP error codes to display names, colors, and console methods (`error`, `warn`, `info`, `log`).
6. **Verbose output**: In dev mode, generates a full backtrace with file, line, function, class, and argument details.
7. **Console output**: When `$code === -1` (shutdown signal), outputs all buffered errors as a collapsed console group.
8. **File logging**: Writes errors to `CMS_DATA_PATH/#log/error.txt` using atomic file operations with exclusive locks.

**Usage**:
```php
// Automatically registered as error handler
set_error_handler(__NAMESPACE__ . "\\cms_error");
```

### cms_error_silent

```php
function cms_error_silent($flag = TRUE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$flag` | bool | `TRUE` | Whether to enable silent mode. |

**Returns**: `bool` — The result of `cms_cache("cms.error.silent", (bool)$flag)`.

**Purpose**: Toggles silent error mode. When enabled, `cms_error` returns immediately without processing any errors.

**Usage**:
```php
cms_error_silent(TRUE);  // Suppress all errors
cms_error_silent(FALSE); // Resume error handling
```

### cms_shutdown

```php
function cms_shutdown()
```

**Returns**: `void`

**Purpose**: Registered as a shutdown function. Captures fatal errors (E_ERROR, E_PARSE, E_CORE_ERROR, E_COMPILE_ERROR) and passes them to `cms_error`. Then signals the error handler to flush the buffer by calling `cms_error(-1, "")`.

**Inner mechanism**:
1. Calls `error_get_last()` to retrieve the last fatal error.
2. If a fatal error is found, passes it to `cms_error`.
3. Calls `cms_error(-1, "")` to trigger buffer flush and console output.

**Usage**:
```php
register_shutdown_function(__NAMESPACE__ . "\\cms_shutdown");
```

---

## Input Preprocessing

### cms_request_var

```php
function cms_request_var($name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | string | The name of the request variable. |

**Returns**: `mixed` — The normalized value from `$_POST`, `$_GET`, `$_COOKIE`, or `NULL` if not found.

**Purpose**: Retrieves a request variable from POST, GET, or COOKIE (in that order) and normalizes it for UTF-8 safety.

**Inner mechanism**: Checks `$_POST[$name]`, then `$_GET[$name]`, then `$_COOKIE[$name]`, and passes the result through `cms_utf8_normalize`.

**Usage**:
```php
$username = cms_request_var("cms_login_user");
```

### cms_initialize_globals

```php
function cms_initialize_globals()
```

**Returns**: `void`

**Purpose**: Loads all request data (POST, GET, COOKIE, FILES) into the `$GLOBALS` array with UTF-8 normalization. This makes request variables accessible as global variables throughout the application.

**Inner mechanism**:
1. Merges keys from `$_COOKIE`, `$_GET`, and `$_POST`.
2. For each key, sets `$GLOBALS[$key]` to the normalized request variable.
3. For `$_FILES`, flattens the structure: each file's properties (`name`, `size`, `tmp_name`, `error`, `type`) are stored as `$GLOBALS["key_property"]` (except `tmp_name` which becomes `$GLOBALS["key"]`).

**Usage**:
```php
cms_initialize_globals();
// Now $GLOBALS["cms_user"] is available
```

### cms_utf8_normalize

```php
function cms_utf8_normalize($value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | mixed | The value to normalize (scalar or array). |

**Returns**: `mixed` — The normalized value with line breaks converted to `\n` and invalid UTF-8 repaired.

**Purpose**: Recursively normalizes input data by converting all line break styles (`\r\n`, `\r`) to `\n` and repairing invalid UTF-8 sequences.

**Inner mechanism**:
- If `$value` is an array, recursively maps `cms_utf8_normalize` over its elements.
- Otherwise, applies `preg_replace("/\r\n?/", "\n", ...)` to normalize line breaks, then passes through `utf8_normalize` (a custom multibyte-safe function).

**Usage**:
```php
$clean = cms_utf8_normalize($_POST["comment"]);
```

---

## Identification

### cms_identification

```php
function cms_identification()
```

**Returns**: `void`

**Purpose**: The core authentication and authorization function. Handles user login, logout, CSRF protection, brute-force attack prevention, and agent (MCP) authentication. Sets global constants for the authenticated user.

**Inner mechanism**:
1. **Security check**: Exits if any user-related constant is already defined (prevents re-initialization).
2. **Logout handling**: If `cms_logout` is set, deletes cookies, clears the security token from cache, and redirects to the root URL.
3. **Brute-force protection**: Tracks failed login attempts per IP address. Blocks login after `CMS_LOGIN_ATTEMPT_MAX` (5) attempts for `CMS_LOGIN_BLOCK_TIME` (1800 seconds / 30 minutes).
4. **Authentication modes**:
   - **Daemon**: Uses a fixed "daemon" user with a hashed password.
   - **Agent (MCP)**: Authenticates via `CMS_MCP_AUTH` bearer token.
   - **Login form**: Processes POST login with CSRF protection.
   - **Default**: Falls back to "anonymous" user.
5. **CSRF protection**: For non-anonymous, non-daemon users, generates or verifies a security token. If missing, redirects to a security error page.
6. **Permission check**: Verifies the user has permission to access the current application. If not, redirects to the identification prompt.
7. **Agent instructions**: For MCP agents, handles periodic instruction display based on tool call intervals.
8. **Cookie management**: Sets session cookies for authenticated users.
9. **Constants**: Defines `CMS_USER`, `CMS_PASSWORD`, `CMS_TOKEN`, `CMS_SUPERUSER`, `CMS_NAME`, `CMS_EMAIL`, `CMS_TIMEZONE`, `CMS_PROFILE`, and `CMS_MCP_ACTIVE`.

**Usage**:
```php
cms_identification();
// After this call, CMS_USER, CMS_SUPERUSER, etc. are defined
```

### cms_generate_id

```php
function cms_generate_id()
```

**Returns**: `void`

**Purpose**: Generates an anonymous client fingerprint (`CMS_USERID`) and an obfuscated IP hash (`CMS_IPHASH`). These are used for anonymous session consistency and security checks.

**Inner mechanism**:
1. **Security check**: Exits if `CMS_USERID` or `CMS_IPHASH` is already defined.
2. **Salt**: Retrieves a time-based salt (changes every 60 minutes) via `cms_salt()`.
3. **Session ID**: If the `cms_session` cookie is a valid 32-character hash, uses it directly. Otherwise, generates a fingerprint from browser headers and IP address, hashed with RIPEMD-128.
4. **IP hash**: Generates an obfuscated hash of the client's IP address using the same salt.

**Usage**:
```php
cms_generate_id();
// CMS_USERID and CMS_IPHASH are now defined
```

---

## Language

### cms_language

```php
function cms_language()
```

**Returns**: `void`

**Purpose**: Initializes language settings by determining the best matching language from the system's supported languages, the user's selection, and the browser's `Accept-Language` header.

**Inner mechanism**:
1. **Security check**: Exits if language constants are already defined.
2. **System default**: Reads the default language from the system configuration.
3. **User selection**: Checks for `cms_select_language` (explicit selection) or `cms_language` (stored preference) request variables.
4. **Validation**: Ensures selected languages are in the supported list.
5. **Fallback**: If no language is set, parses the `Accept-Language` header using `cms_language_extract` and falls back to the system default.
6. **Storage**: Caches the selected language per user ID.
7. **Loading**: Requires the appropriate language file from `CMS_PATH/#language/`.

**Usage**:
```php
cms_language();
// CMS_LANGUAGE, CMS_LANGUAGE_DEFAULT, CMS_LANGUAGE_ENABLED are defined
```

### cms_language_extract

```php
function cms_language_extract($requested, $supported)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$requested` | string | The `Accept-Language` header value. |
| `$supported` | string | Comma-separated list of supported languages. |

**Returns**: `string\|null` — The best matching supported language, or `NULL` if no match.

**Purpose**: Finds the best matching language from a list of supported languages based on the RFC 9110 / BCP 47 language string from the `Accept-Language` header.

**Inner mechanism**:
1. Parses the requested languages using `cms_language_parse`.
2. Splits supported languages into an array.
3. For each requested language, compares against supported languages:
   - **Exact match**: Same number of subtags and exact match.
   - **Partial match**: Prefix match with higher specificity preferred.
4. Prioritizes higher `q` values (quality/priority) and higher specificity (more subtags matched).

**Usage**:
```php
$best = cms_language_extract("en-US,en;q=0.9,de;q=0.8", "en,de,fr");
// Returns "en"
```

### cms_language_parse

```php
function cms_language_parse($string)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | The `Accept-Language` header value. |

**Returns**: `array` — Parsed language entries sorted by priority (q-value), each containing `code`, `script`, `region`, `variant`, and `q`.

**Purpose**: Parses an RFC 9110 / BCP 47 language string into its components and sorts by priority.

**Inner mechanism**:
1. Normalizes the string (lowercase, no whitespace).
2. Uses a regex to extract language tags and their q-values.
3. For each match, creates an entry with code, script, region, variant, and q-value.
4. Skips entries with `q=0.0` (explicitly not accepted).
5. Sorts the result by q-value in descending order using `uasort`.

**Usage**:
```php
$parsed = cms_language_parse("en-US,en;q=0.9,de;q=0.8");
// Returns array sorted by q-value
```

---

## Libraries

### cms_load

```php
function cms_load($library, $exit_on_error = FALSE, $test = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$library` | string | — | The library name (without `lib.` prefix or `.inc` suffix). |
| `$exit_on_error` | bool | `FALSE` | If `TRUE`, terminates execution on failure. |
| `$test` | bool | `FALSE` | If `TRUE`, only tests availability without loading. |

**Returns**: `bool` — `TRUE` if the library is available/loaded, `FALSE` otherwise.

**Purpose**: Loads a system library file from `CMS_SYSTEM_PATH/lib.$library.inc`. Supports testing availability without loading and fatal error on failure.

**Inner mechanism**:
1. Uses a static `$loaded` array to cache load state per library.
2. If not yet loaded, checks if the file exists at `CMS_SYSTEM_PATH/lib.$library.inc`.
3. If the file doesn't exist:
   - If `$exit_on_error`, calls `die()` with an error message.
   - Stores `FALSE` in the cache.
4. If the file exists and `$test` is `FALSE`, loads it via `require` inside a closure (for scope isolation).
5. Returns the cached load state.

**Usage**:
```php
if (cms_load("search")) {
    $search = new search();
}
// Or test without loading:
if (cms_available("search")) {
    // Library exists but not yet loaded
}
```

### cms_available

```php
function cms_available($library)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$library` | string | The library name to test. |

**Returns**: `bool` — `TRUE` if the library file exists, `FALSE` otherwise.

**Purpose**: Convenience wrapper around `cms_load` that tests library availability without loading it.

**Usage**:
```php
if (cms_available("memory")) {
    // Memory library is available
}
```

### cms_load_system

```php
function cms_load_system()
```

**Returns**: `void`

**Purpose**: Loads all system libraries matching the pattern `sys.*.inc` from `CMS_SYSTEM_PATH`. This is called once during initialization.

**Inner mechanism**:
1. Security check: Exits if `CMS_LOAD_SYSTEM_EXECUTED` is already defined.
2. Opens `CMS_SYSTEM_PATH` directory.
3. Iterates through files, loading those matching `/^sys\.([^\.]+)\.inc$/`.
4. Each file is loaded via `require_once` inside a closure.

**Usage**:
```php
cms_load_system(); // Called during initialization
```

---

## Applications

### cms_application

```php
function cms_application($application = NULL, $instance = NULL, $permission = NULL, $user = NULL)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$application` | string\|bool\|null | `NULL` | Application name, `TRUE` for current, `NULL` for context. |
| `$instance` | string\|bool\|null | `NULL` | Instance name, `TRUE` for current, `NULL` for context. |
| `$permission` | string\|null | `NULL` | Permission to test. |
| `$user` | string\|null | `NULL` | User to test permission for (defaults to current user). |

**Returns**: `mixed` — `string` for context queries, `bool` for permission checks.

**Purpose**: A multi-purpose function that handles application loading, permission checking, and context information retrieval. The behavior depends on the combination of arguments:

- **No arguments**: Returns the current `application.instance` string.
- **`$application = TRUE`**: Returns the current application name.
- **`$instance = TRUE`**: Returns the current instance name.
- **With `$permission`**: Tests the specified permission for the given application/instance/user context.
- **With `$application` (string)**: Loads the specified application module from `CMS_MODULES_PATH/#module/mod.$application.inc`.

**Inner mechanism**:
1. Uses static variables to cache the current application, instance, and permission object.
2. For permission checks, delegates to `$_permission->given()` or `$_permission->test()`.
3. For application loading, constructs the file path, checks existence, and loads via `require` inside a closure.
4. Temporarily modifies the application/instance context during loading, then restores it.

**Usage**:
```php
// Get current application.instance
$context = cms_application();

// Test permission
if (cms_application(TRUE, TRUE, "edit")) {
    // User can edit in current application.instance
}

// Load an application
cms_application("blog");
```

### cms_instance

```php
function cms_instance()
```

**Returns**: `string` — The current instance name.

**Purpose**: Convenience wrapper that returns the current instance by calling `cms_application(NULL, TRUE)`.

**Usage**:
```php
$instance = cms_instance();
```

### cms_permission

```php
function cms_permission($permission, $application = TRUE, $instance = TRUE, $user = NULL)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$permission` | string | — | The permission to test. |
| `$application` | bool | `TRUE` | Whether to include the current application in the permission path. |
| `$instance` | bool | `TRUE` | Whether to include the current instance in the permission path. |
| `$user` | string\|null | `NULL` | User to test for (defaults to current user). |

**Returns**: `bool` — Whether the user has the specified permission.

**Purpose**: Convenience wrapper around `cms_application` for permission checking. The `$application` and `$instance` parameters control the scope of the permission check:
- `TRUE`: Include current application/instance.
- `FALSE`/`NULL`: Exclude from the permission path.

**Usage**:
```php
// Test application.instance.edit permission
if (cms_permission("edit")) { ... }

// Test application.edit permission (ignore instance)
if (cms_permission("edit", TRUE, NULL)) { ... }

// Test global edit permission
if (cms_permission("edit", NULL, NULL)) { ... }

// Test for a specific user
if (cms_permission("edit", TRUE, TRUE, "admin")) { ... }
```

---

## URL Functions

### cms_url

```php
function cms_url($address = NULL, $param = NULL, $omit_param = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$address` | string\|array\|null | `NULL` | URL address or array of parameters. |
| `$param` | array\|null | `NULL` | Additional parameters to merge. |
| `$omit_param` | bool | `FALSE` | Whether to omit stored parameters. |

**Returns**: `string\|bool` — The generated URL, or `FALSE` on parse failure.

**Purpose**: Generates a URL by merging the given address and parameters with the current request's stored parameters. Handles external URLs, executable scripts, and CSRF token injection.

**Inner mechanism**:
1. If `$address` is an array, treats it as parameters and uses the current active URL.
2. Parses the address URL into components.
3. Determines if the URL is external (different host) or executable (`.php` extension).
4. Merges query parameters from the address with the provided `$param` array.
5. Generates the querystring via `cms_param`, with CSRF token injection unless omitted or external.
6. Builds the final URL using `cms_build_url`.

**Usage**:
```php
// Generate URL for current page with added parameter
$url = cms_url(NULL, ["page" => 2]);

// Generate URL for a specific script
$url = cms_url("module/blog/index.php", ["action" => "edit"]);

// Generate URL without CSRF token
$url = cms_url("api/data.php", [], TRUE);
```

### cms_param

```php
function cms_param($value = NULL, $key = NULL, $omit_token = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | mixed | `NULL` | Value to set, key to retrieve, or array to merge. |
| `$key` | string\|bool\|null | `NULL` | Key name, `TRUE` to return all, `FALSE` to delete. |
| `$omit_token` | bool | `FALSE` | Whether to omit the CSRF token from the querystring. |

**Returns**: `mixed` — Depends on mode: `string` (querystring), `bool` (set/delete), `mixed` (retrieve), `array` (all values).

**Purpose**: A state manager for query parameters. It maintains a static array of parameters that persist across calls within the same request. Supports setting, retrieving, deleting, and generating querystrings.

**Inner mechanism**:
- **Key management** (`$key` is a string):
  - `$value === FALSE`: Deletes the key and returns its previous value.
  - Otherwise: Sets `$param[$key] = $value` and returns `TRUE`.
- **Bulk operations** (`$key` is `NULL` or `TRUE`):
  - `$value === TRUE`: Returns all stored parameters.
  - `$value === FALSE`: Clears all stored parameters.
  - `$value` is an array: Merges with stored parameters and generates a querystring.
  - `$value` is a string: Retrieves the stored value for that key.
- **Querystring generation**: Recursively flattens nested arrays into `key[subkey]=value` format, URL-encodes values, and prepends the CSRF token unless `$omit_token` is `TRUE`.

**Usage**:
```php
// Set a parameter
cms_param(["page" => 1], "cms_page");

// Get all parameters
$all = cms_param(TRUE);

// Generate querystring with current + new params
$query = cms_param(["sort" => "name"]);

// Delete a parameter
$old = cms_param(FALSE, "cms_page");
```

### cms_build_url

```php
function cms_build_url($parts)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parts` | array | URL components (scheme, host, port, user, pass, path, query, fragment). |

**Returns**: `string` — The assembled URL.

**Purpose**: Constructs a valid URL string from an array of URL components, handling all edge cases for proper URL formatting.

**Inner mechanism**:
1. If a host is present, prepends the scheme (if present) and `//`.
2. Adds user info (with optional password) if present.
3. Adds the host and port (if present).
4. Ensures the path starts with `/` if a host is present.
5. Appends path, query (prefixed with `?`), and fragment (prefixed with `#`).

**Usage**:
```php
$url = cms_build_url([
    "scheme" => "https",
    "host" => "example.com",
    "path" => "/path",
    "query" => "foo=bar"
]);
// Returns "https://example.com/path?foo=bar"
```

---

## Cache Data Storage

### cms_cache

```php
function cms_cache($key = NULL, $value = NULL, $permanent = FALSE, $ttl = NULL)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string\|null | `NULL` | Cache key. |
| `$value` | mixed | `NULL` | Value to store, `""` to delete. |
| `$permanent` | bool | `FALSE` | Whether to store permanently on disk. |
| `$ttl` | int\|null | `NULL` | Time-to-live in seconds. |

**Returns**: `mixed` — The cached value (retrieval), `bool` (store/delete), or `array` (all values).

**Purpose**: A dual-layer cache system combining in-RAM storage (static array) with optional persistent filesystem storage. Supports TTL-based expiration and atomic file operations.

**Inner mechanism**:
- **No key** (`$key === NULL`): Returns all cached values.
- **Store** (`$value !== NULL` or `$permanent !== FALSE`):
  - If `$value === ""`: Deletes the entry (sets tombstone in RAM, removes file if permanent).
  - Otherwise: Stores in RAM. If `$permanent`, serializes and writes to a file at `CMS_DATA_PATH/#cache/{hash_prefix}/{hash_suffix}` using atomic rename.
- **Retrieve**:
  - Checks RAM cache first.
  - If not found, checks filesystem for `.ser` (serialized) or plain text file.
  - Updates file modification time (touch) unless `$ttl === FALSE`.
  - Stores result in RAM cache.

**Usage**:
```php
// Store a temporary value
cms_cache("user_count", 42);

// Store a permanent value
cms_cache("config", $config, TRUE);

// Retrieve a value
$count = cms_cache("user_count");

// Retrieve without updating time
$value = cms_cache("config", NULL, FALSE, FALSE);

// Delete a value
cms_cache("user_count", "");
```

### cms_cache_delete

```php
function cms_cache_delete($key, $permanent = TRUE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string\|array | — | Cache key(s) to delete. |
| `$permanent` | bool | `TRUE` | Whether to also delete permanent storage. |

**Returns**: `bool` — `TRUE` if all deletions succeeded.

**Purpose**: Deletes one or more cache entries. Accepts a single key or an array of keys.

**Usage**:
```php
cms_cache_delete("user_count");
cms_cache_delete(["key1", "key2", "key3"]);
```

### cms_cache_sync

```php
function cms_cache_sync(&$variable, $key, $default = NULL, $load_on_empty = FALSE, $no_store = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$variable` | mixed | — | Variable to initialize/update (passed by reference). |
| `$key` | string | — | Cache key. |
| `$default` | mixed | `NULL` | Default value if cache is empty. |
| `$load_on_empty` | bool | `FALSE` | Whether to treat empty strings/0 as empty. |
| `$no_store` | bool | `FALSE` | Whether to skip storing the variable back to cache. |

**Returns**: `mixed` — The final value of `$variable`.

**Purpose**: Synchronizes a variable with the cache. If the variable is undefined or empty, loads from cache. If still empty, uses the default. If the variable was modified, stores it back to cache.

**Inner mechanism**:
1. Checks if `$variable` is empty (using `isset` or `empty` based on `$load_on_empty`).
2. If empty, loads from cache via `cms_cache($key)`.
3. If still empty, sets to `$default`.
4. If not empty and `$no_store` is `FALSE`, stores the variable back to cache.

**Usage**:
```php
$config = NULL;
cms_cache_sync($config, "app_config", [], TRUE);
// $config is now loaded from cache or set to []
```

### cms_cache_init

```php
function cms_cache_init(&$variable, $key, $default = NULL, $load_on_empty = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$variable` | mixed | — | Variable to initialize (passed by reference). |
| `$key` | string | — | Cache key. |
| `$default` | mixed | `NULL` | Default value if cache is empty. |
| `$load_on_empty` | bool | `FALSE` | Whether to treat empty strings/0 as empty. |

**Returns**: `mixed` — The final value of `$variable`.

**Purpose**: Convenience wrapper around `cms_cache_sync` with `$no_store = TRUE`. Only loads from cache, never stores back.

**Usage**:
```php
$settings = NULL;
cms_cache_init($settings, "app_settings", []);
```

### cms_cache_notouch

```php
function cms_cache_notouch($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key. |

**Returns**: `mixed` — The cached value without updating its timestamp.

**Purpose**: Retrieves a cache value without updating the file's modification time. Useful for read-only access where TTL should not be reset.

**Usage**:
```php
$value = cms_cache_notouch("config");
```

### cms_cache_time

```php
function cms_cache_time($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key. |

**Returns**: `int\|bool` — The Unix timestamp of the last modification, or `FALSE` if not found.

**Purpose**: Returns the last modification time of a cache entry's file.

**Usage**:
```php
$last_modified = cms_cache_time("config");
```

### cms_cache_touch

```php
function cms_cache_touch($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Cache key. |

**Returns**: `bool` — Whether the touch operation succeeded.

**Purpose**: Updates the modification time of a cache entry's file, effectively resetting its TTL.

**Usage**:
```php
cms_cache_touch("config");
```

### cms_cache_clean

```php
function cms_cache_clean($path, $force = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$path` | string | — | Directory path to clean. |
| `$force` | bool | `FALSE` | Whether to remove all files regardless of TTL. |

**Returns**: `bool` — `TRUE` on success.

**Purpose**: Recursively removes expired cache files from a directory. Files older than `CMS_CACHE_TTL` (30 days) are removed. Empty directories are also removed.

**Inner mechanism**:
1. Uses a depth-first traversal with manual directory handles.
2. For each file, checks if it's expired (based on `CMS_CACHE_TTL`) or if `$force` is `TRUE`.
3. Removes expired files and empty directories.
4. Uses `flock` for safe concurrent access.

**Usage**:
```php
cms_cache_clean(CMS_DATA_PATH . "#cache/");
```

---

## Daemon

### cms_daemon

```php
function cms_daemon($code, $id = NULL, $interval = 0, $status = "")
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$code` | string | — | PHP code to execute in the background. |
| `$id` | string\|null | `NULL` | Unique identifier to prevent duplicate queueing. |
| `$interval` | int | `0` | Minimum seconds between executions. |
| `$status` | string | `""` | Human-readable status message. |

**Returns**: `bool` — `TRUE` if the task was queued, `FALSE` if already queued or interval not met.

**Purpose**: Queues a background task for asynchronous execution by the daemon worker. The task is stored as a PHP file in `CMS_DATA_PATH/#daemon/` and executed by `daemon.php`.

**Inner mechanism**:
1. Creates the daemon directory if it doesn't exist.
2. Computes a hash of the `$id` (or `$code` if `$id` is empty) to determine the task file path.
3. If the task file exists:
   - If it has content (queued), returns `FALSE`.
   - If `$interval > 0` and the last execution was within the interval, returns `FALSE`.
4. Wraps the code in a PHP file with namespace declaration and status message.
5. Writes atomically using a temp file and `rename`.
6. If `$interval` is `0`, sets the file's modification time to 1 (epoch) to indicate it's ready.
7. Creates a `daemon.flag` file to signal that tasks are available.

**Usage**:
```php
cms_daemon(
    "cleanup_old_records();",
    "cleanup.task",
    3600,
    "Database cleanup"
);
```

### cms_daemon_status

```php
function cms_daemon_status($value = NULL)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | string\|null | `NULL` | Status message to set, or `NULL` to retrieve. |

**Returns**: `string\|bool` — Current status (retrieval) or `TRUE`/`FALSE` (set).

**Purpose**: Manages the daemon's status log. When called with a value, appends a timestamped message to the status file. When called without a value, returns the current status.

**Inner mechanism**:
1. If `$value` is empty, reads and returns the status file contents.
2. If `$value` is provided:
   - Reads the last 25 lines from the existing status file (if any).
   - Appends a new timestamped entry.
   - Writes atomically using a temp file and `rename`.

**Usage**:
```php
// Set status
cms_daemon_status("Processing batch 1 of 10");

// Get status
$status = cms_daemon_status();
```

### cms_daemon_exists

```php
function cms_daemon_exists($id, $running = FALSE)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$id` | string | — | Task identifier. |
| `$running` | bool | `FALSE` | Whether to check if the task is currently running. |

**Returns**: `bool` — Whether the task exists (and optionally is running).

**Purpose**: Checks if a daemon task with the given ID is queued or currently running.

**Inner mechanism**:
1. Computes the task file path from the ID hash.
2. If the file doesn't exist or is empty, returns `FALSE`.
3. If `$running` is `TRUE`, also checks for a `.lock` file and attempts a non-blocking exclusive lock to determine if the task is currently executing.

**Usage**:
```php
if (cms_daemon_exists("search.daemon.update")) {
    // Task is queued
}
if (cms_daemon_running("search.daemon.update")) {
    // Task is currently executing
}
```

### cms_daemon_running

```php
function cms_daemon_running($id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | string | Task identifier. |

**Returns**: `bool` — Whether the task is currently running.

**Purpose**: Convenience wrapper around `cms_daemon_exists($id, TRUE)`.

**Usage**:
```php
if (cms_daemon_running("cache.daemon")) {
    // Cache cleanup is in progress
}
```

### cms_daemon_remove

```php
function cms_daemon_remove($id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | string | Task identifier. |

**Returns**: `bool` — `TRUE` if the task was removed, `FALSE` if it's still running.

**Purpose**: Removes a queued daemon task. If the task is currently running, the removal fails.

**Inner mechanism**:
1. Checks if the task is running via `cms_daemon_running`.
2. If running, returns `FALSE`.
3. If the task file's modification time is 1 (epoch, indicating ready state), deletes the file.
4. Otherwise, empties the file content (marks as completed).
5. Removes the lock file if it exists.

**Usage**:
```php
cms_daemon_remove("search.daemon.update");
```

### cms_daemon_run

```php
function cms_daemon_run()
```

**Returns**: `bool` — `TRUE` if the daemon was invoked, `FALSE` if already running or no tasks.

**Purpose**: Triggers the daemon worker by making an HTTP request to `daemon.php`. Uses an advisory lock to prevent concurrent daemon invocations.

**Inner mechanism**:
1. Checks for the `daemon.flag` file (indicates tasks are available).
2. Attempts a non-blocking exclusive lock on `daemon.lock`.
3. If locked (daemon already running), returns `FALSE`.
4. Releases the lock and makes an HTTP request to `CMS_MODULES_URL . "daemon.php"` with a 1-second timeout.
5. Suppresses errors during the HTTP call and restores the previous error handler.

**Usage**:
```php
cms_daemon_run(); // Called during initialization
```

---

## Flag

### cms_flag_set

```php
function cms_flag_set($key, $mode = 0)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | — | Flag key. |
| `$mode` | int | `0` | `0` = set, `1` = get, `2` = delete. |

**Returns**: `bool` — Whether the operation succeeded.

**Purpose**: Manages boolean flags using file-based locking. Flags are used for inter-process communication and synchronization.

**Inner mechanism**:
1. Creates the flag directory if needed.
2. Computes a hash of the key for the flag file path.
3. **Set mode** (`$mode === 0`): Creates an empty file with `LOCK_EX`.
4. **Get mode** (`$mode === 1`): Opens the file and attempts a non-blocking exclusive lock. Returns `TRUE` if the lock succeeds (flag is set).
5. **Delete mode** (`$mode === 2`): Deletes the file and closes the handle.
6. Uses a static array to cache file handles for the same key within a request.

**Usage**:
```php
// Set a flag
cms_flag_set("maintenance_mode");

// Check if flag is set
if (cms_flag_get("maintenance_mode")) { ... }

// Delete a flag
cms_flag_del("maintenance_mode");
```

### cms_flag_get

```php
function cms_flag_get($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Flag key. |

**Returns**: `bool` — Whether the flag is set.

**Purpose**: Convenience wrapper around `cms_flag_set($key, 1)`.

**Usage**:
```php
if (cms_flag_get("maintenance_mode")) {
    // Maintenance mode is active
}
```

### cms_flag_del

```php
function cms_flag_del($key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | Flag key. |

**Returns**: `bool` — Whether the flag was deleted.

**Purpose**: Convenience wrapper around `cms_flag_set($key, 2)`.

**Usage**:
```php
cms_flag_del("maintenance_mode");
```

---

## Miscellaneous

### cms_set_cookie

```php
function cms_set_cookie($array)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | array | Key-value pairs of cookies to set or delete. |

**Returns**: `bool` — `TRUE` if all cookies were set successfully.

**Purpose**: Sets or deletes multiple cookies with consistent security options. A value of `""` (empty string) deletes the cookie by setting its expiration to the past.

**Inner mechanism**:
1. Defines cookie options: `httponly`, `path`, `samesite=Strict`, `secure` (based on protocol).
2. For each key-value pair:
   - Skips if the cookie already has the same value.
   - Sets `expires` to `1` (past) for deletion, `0` (session) for setting.
   - Calls `setcookie` with the options.

**Usage**:
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

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$value` | string\|null | `NULL` | Custom value to base the salt on. |

**Returns**: `string` — Binary salt data (32 bytes).

**Purpose**: Generates and caches a salt that changes every 60 minutes. The salt is used for password hashing and IP/user ID obfuscation.

**Inner mechanism**:
1. If `$value` is `NULL`, generates an IP-based salt using `$_SERVER["REMOTE_ADDR"]`.
2. If `$value` is provided, generates a user-based salt using `CMS_USERID . "." . $value`.
3. Computes a cache key from the prefix and a 5-character hash of the value.
4. Checks the cache file's modification time. If older than 60 minutes, generates a new random salt.
5. Caches the salt in RAM for the current request.

**Usage**:
```php
// IP-based salt
$salt = cms_salt();

// User-based salt for password hashing
$salt = cms_salt("password");
```

### cms_token_tag

```php
function cms_token_tag()
```

**Returns**: `string` — A 64-character hex token.

**Purpose**: Generates a cryptographically secure token for CSRF protection. The token consists of 32 bytes of random data and a 16-byte HMAC tag, both hex-encoded.

**Inner mechanism**:
1. Retrieves or generates a secret key (32 random bytes) stored permanently in cache.
2. Generates 16 bytes of random data.
3. Computes an HMAC-SHA256 of the random data using the secret, truncated to 16 bytes.
4. Returns `bin2hex(random . tag)` (64 hex characters).

**Usage**:
```php
$token = cms_token_tag();
// Store in form: <input type="hidden" name="csrf_token" value="$token">
```

### cms_token_check

```php
function cms_token_check($token)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | string | The token to verify. |

**Returns**: `bool` — Whether the token is valid.

**Purpose**: Verifies a CSRF token generated by `cms_token_tag`. Uses constant-time comparison to prevent timing attacks.

**Inner mechanism**:
1. Validates token length (must be 64 hex characters).
2. Decodes the random portion (first 32 hex chars) and tag portion (last 32 hex chars).
3. Retrieves the stored secret from cache.
4. Recomputes the HMAC and compares using `hash_equals`.

**Usage**:
```php
if (cms_token_check($_POST["csrf_token"])) {
    // Token is valid
}
```

### cms_email_agent

```php
function cms_email_agent()
```

**Returns**: `void`

**Purpose**: Initializes the system email agent address. Reads from system configuration or defaults to `mailagent@domain`.

**Inner mechanism**:
1. Security check: Exits if `CMS_EMAIL_AGENT` is already defined.
2. Reads the email address from system configuration (`email.address`).
3. If empty, defaults to `mailagent@` + domain (without `www.` prefix).
4. Defines `CMS_EMAIL_AGENT`.

**Usage**:
```php
cms_email_agent();
// CMS_EMAIL_AGENT is now defined
```

### cms_mcp_initialize

```php
function cms_mcp_initialize()
```

**Returns**: `void`

**Purpose**: Initializes the Model Context Protocol (MCP) integration. Parses MCP-specific HTTP headers, validates the request, and sets up MCP constants.

**Inner mechanism**:
1. **Security check**: Exits if MCP constants are already defined.
2. **Header parsing**: Extracts `Authorization` (Bearer token), `MCP-Protocol-Version`, and `MCP-Method` headers.
3. **Non-MCP early return**: If not an MCP request and no method/version headers, returns early.
4. **Origin validation**: Checks that the `Origin` header matches the server host.
5. **Authorization**: If no auth token, returns 401 with metadata endpoint.
6. **Header validation**: Ensures method and version headers are present.
7. **Accept header check**: Validates that the client accepts JSON or event-stream responses.
8. **Protocol version check**: Only supports `2026-07-28`.
9. **JSON parsing**: Reads and parses the request body as JSON.
10. **Structure validation**: Checks for `jsonrpc: "2.0"`, `method`, and `id` fields.
11. **Meta data validation**: Checks for required protocol metadata fields.
12. **Header/JSON consistency**: Verifies that header values match JSON body values.

**Usage**:
```php
cms_mcp_initialize();
// CMS_MCP_AUTH, CMS_MCP_VERSION, CMS_MCP_METHOD are defined
```

### cms_mcp_process

```php
function cms_mcp_process()
```

**Returns**: `void`

**Purpose**: Processes the MCP request by dispatching to the appropriate handler method based on the MCP method.

**Inner mechanism**:
1. Returns early if not in MCP active mode.
2. Handles multi-round tool requests by restoring previous request state.
3. Dispatches based on `CMS_MCP_METHOD`:
   - `server/discover`: Calls `mcp::server_discover()`.
   - `tools/list`: Calls `mcp::tools_list()`.
   - `tools/call`: Dispatches to specific tool handlers based on the tool name.
   - `resources/list`: Calls `mcp::resources_list()`.
   - `resources/read`: Calls `mcp::resources_read()`.
   - Unknown methods: Calls `mcp::method_not_found()`.

**Usage**:
```php
cms_mcp_process(); // Called during initialization
```

### cms_trusted_proxies

```php
function cms_trusted_proxies()
```

**Returns**: `void`

**Purpose**: Configures the correct client IP address, protocol, and port when running behind trusted reverse proxies. Reads the trusted proxy list from `CMS_DATA_PATH/#system/trusted_proxies.txt`.

**Inner mechanism**:
1. Retrieves the `X-Forwarded-For` header (or `X-Real-IP` as fallback).
2. Reads the trusted proxies list from the configuration file.
3. Walks the IP trace from the client to the server, finding the first untrusted IP.
4. If a trusted proxy is detected, updates `$_SERVER` variables for `REMOTE_ADDR`, `HTTPS`, and `SERVER_PORT`.
5. Cleans up proxy-related headers.

**Usage**:
```php
cms_trusted_proxies(); // Called during initialization
```

### cms_ip_in_cidr

```php
function cms_ip_in_cidr($ip, $cidr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ip` | string | The IP address to check. |
| `$cidr` | string | The CIDR notation (e.g., `192.168.1.0/24`). |

**Returns**: `bool` — Whether the IP is within the CIDR range.

**Purpose**: Checks if an IP address falls within a given CIDR range. Supports both IPv4 and IPv6.

**Inner mechanism**:
1. If no `/` in the CIDR, performs a direct string comparison.
2. Converts both IP and subnet to binary using `inet_pton`.
3. Generates a subnet mask from the bit count.
4. Applies the mask to both addresses and compares.

**Usage**:
```php
if (cms_ip_in_cidr("192.168.1.5", "192.168.1.0/24")) {
    // IP is in the subnet
}
```

### cms_path_urlencode

```php
function cms_path_urlencode($string)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | The string to encode. |

**Returns**: `string` — The URL-encoded string.

**Purpose**: Encodes non-alphanumeric characters in a file path for safe URL usage, following RFC 1738. Excludes the path separator `/` and certain safe characters (`$-_.+!*'()`).

**Inner mechanism**: Uses `preg_replace_callback` to replace any character not in the allowed set with its percent-encoded hex representation.

**Usage**:
```php
$encoded = cms_path_urlencode("/path/to/file with spaces.txt");
// Returns "/path/to/file%20with%20spaces.txt"
```

---

## System Constants

These constants are defined during the initialization phase and provide system-wide configuration:

### System

| Constant | Description |
|----------|-------------|
| `CMS_APPLICATION` | Current application name (derived from script filename). |
| `CMS_INSTANCE` | Current instance name (empty by default). |
| `CMS_DB_PREFIX` | Database table prefix (`"cms_"`). |
| `CMS_IFC_EDITION` | Interface edition identifier (`"ifc"`). |

### Paths

| Constant | Description |
|----------|-------------|
| `CMS_PATH` | Base path of the PWNC installation. |
| `CMS_SYSTEM_PATH` | Path to system libraries (`#system/`). |
| `CMS_MODULES_PATH` | Path to modules (`module/`). |
| `CMS_INTERFACE_PATH` | Path to interface modules. |
| `CMS_DESKTOP_PATH` | Path to desktop modules. |
| `CMS_IMAGES_PATH` | Path to images. |
| `CMS_ROOT_PATH` | Root path of the web application. |
| `CMS_DATA_PATH` | Path to data directory (`data/`). |

### URLs

| Constant | Description |
|----------|-------------|
| `CMS_PROTOCOL` | `http` or `https`. |
| `CMS_PORT` | Server port number. |
| `CMS_HOST` | Full host URL (protocol + host). |
| `CMS_DOMAIN` | Domain name without port. |
| `CMS_ACTIVE_URL` | URL of the current script. |
| `CMS_URL` | Base URL of the installation. |
| `CMS_MODULES_URL` | URL to modules directory. |
| `CMS_IMAGES_URL` | URL to images directory. |
| `CMS_JAVA_URL` | URL to Java directory. |
| `CMS_JAVASCRIPT_URL` | URL to JavaScript directory. |
| `CMS_SOUNDS_URL` | URL to sounds directory. |
| `CMS_ROOT_URL` | Root URL of the web application. |
| `CMS_RELATIVE_URL` | Relative URL path. |
| `CMS_DATA_URL` | URL to data directory. |

### Configuration

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_APACHE` | bool | Whether Apache functions are available. |
| `CMS_CACHE_TTL` | `2592000` (30 days) | Cache time-to-live in seconds. |
| `CMS_USER_AGENT` | string | User-Agent string for HTTP requests. |
| `CMS_LOGIN_ATTEMPT_MAX` | `5` | Maximum failed login attempts before blocking. |
| `CMS_LOGIN_BLOCK_TIME` | `1800` (30 min) | Login block duration in seconds. |
| `CMS_PERMISSION_ALWAYS` | `"identification\|index\|check\|mcp\|security"` | Permissions always granted. |

### Regex Patterns

| Constant | Description |
|----------|-------------|
| `CMS_REGEX_MATTER` | Matches characters that can be part of a word (letters, marks, numbers, etc.). |
| `CMS_REGEX_JOINT` | Matches joint characters (hyphens, punctuation) that connect words. |
| `CMS_REGEX_SEPARATOR` | Matches separator characters (whitespace, punctuation, etc.). |
| `CMS_REGEX_WORD` | Matches a complete word (matter + joints). |
| `CMS_REGEX_BORDER` | Matches word borders (separators + joints). |

### Security Events

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_SECURITY_EVENT_CSRF` | `1` | CSRF security event identifier. |

---

## Initialization Sequence

The following functions are called in order during the initialization phase:

1. **`cms_load_system()`** — Loads all system libraries.
2. **`cms_generate_id()`** — Generates anonymous client fingerprint and IP hash.
3. **`cms_language()`** — Initializes language settings.
4. **`cms_email_agent()`** — Initializes system email agent.
5. **`cms_mcp_initialize()`** — Initializes MCP protocol.
6. **`cms_identification()`** — Handles user authentication and authorization.
7. **`cms_mcp_process()`** — Processes MCP requests.
8. **`cms_initialize_globals()`** — Loads request data into `$GLOBALS`.
9. **`cms_param(CMS_INSTANCE, "cms_instance")`** — Adds instance to default parameters.

---

## HTML Constants

| Constant | Description |
|----------|-------------|
| `CMS_DOCTYPE_HTML` | HTML5 doctype declaration. |
| `CMS_BOT_CHECK` | Bot check preload link (empty for authenticated users). |
| `CMS_HTML_HEADER` | Standard HTML header with charset, title, generator, robots, and base URL. |
| `CMS_JAVASCRIPT` | Script tags for core JavaScript files. |
| `CMS_STYLESHEET` | Stylesheet link tag. |
| `CMS_CLASS` | CSS class for the current application. |

---

## HTTP Headers

The following headers are set during initialization:

| Header | Value | Condition |
|--------|-------|-----------|
| `Cache-Control` | `no-cache, must-revalidate` | Anonymous user |
| `Cache-Control` | `no-store, no-cache, must-revalidate` | Authenticated user |
| `Content-Type` | `text/html; charset=utf-8` | Always |
| `Date` | Current GMT date | Always |
| `Last-Modified` | Current GMT date | Always |
| `Referrer-Policy` | `strict-origin-when-cross-origin` | Always |
| `X-Content-Type-Options` | `nosniff` | Always |
| `X-Generator` | `CMS_IDENTIFIER` | Always |

---

## Daemon Tasks

The following background tasks are scheduled during initialization:

| Task ID | Interval | Description |
|---------|----------|-------------|
| `cache.daemon` | 3600s (1 hour) | Cache file cleanup. |
| `log.daemon` | 300s (5 minutes) | Log file maintenance. |
| `search.daemon.update` | 300s (5 minutes) | Search index updates. |
| `search.daemon.score` | 3600s (1 hour) | Search score computation. |
| `memory.daemon.cleanup` | 3600s (1 hour) | Memory cleanup. |

After scheduling, `cms_daemon_run()` is called to trigger the daemon worker if tasks are available.


<!-- HASH:d3c6766bbf1ef5af5c2fbd794c11ffc4 -->
