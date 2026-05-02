# NUOS API Documentation

[← Index](README.md) | [`nuos.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview

The `nuos.inc` file is the core initialization and configuration file for the NUOS web platform. It establishes the foundational environment, including error handling, input preprocessing, user identification, language settings, library loading, URL management, caching, and daemon task scheduling. This file also defines critical system constants, paths, and security configurations required for the platform to operate.

---

## Namespace Isolation

The entire code is wrapped in an anonymous function to isolate the `cms` namespace, preventing naming conflicts with other libraries or user code.

---

## Overloaded PHP Functions

### `constant()`
**Purpose:**
Overloads the native PHP `constant()` function to automatically prepend the `cms` namespace to constant names.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Name of the constant to retrieve. |

**Return Values:**
- `mixed`: The value of the constant if it exists.
- `null`: If the constant does not exist.

**Inner Mechanisms:**
- Prepends the current namespace (`cms`) to the provided constant name before calling the native `constant()` function.

**Usage:**
Used internally to ensure constants are resolved within the `cms` namespace.

---

### `define()`
**Purpose:**
Overloads the native PHP `define()` function to automatically prepend the `cms` namespace to constant names during definition.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | `string` | | Name of the constant to define. |
| `$value` | `mixed` | | Value to assign to the constant. |
| `$case_insensitive` | `bool` | `FALSE` | Whether the constant should be case-insensitive. |

**Return Values:**
- `bool`: `TRUE` if the constant was defined successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Prepends the current namespace (`cms`) to the provided constant name before calling the native `define()` function.

**Usage:**
Used to define constants within the `cms` namespace, ensuring no naming conflicts.

---

### `defined()`
**Purpose:**
Overloads the native PHP `defined()` function to check for constants within the `cms` namespace.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Name of the constant to check. |

**Return Values:**
- `bool`: `TRUE` if the constant exists, `FALSE` otherwise.

**Inner Mechanisms:**
- Prepends the current namespace (`cms`) to the provided constant name before calling the native `defined()` function.

**Usage:**
Used to check the existence of constants within the `cms` namespace.

---

## Software Information

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_SOFTWARE` | `"NUOS"` | Name of the software. |
| `CMS_VERSION` | Content of `version.txt` or `"?"` | Current version of the software. |
| `CMS_COPYRIGHT` | `"© 2026 Patrick Heyer"` | Copyright notice. |
| `CMS_HOMEPAGE` | `"https://nuos-web.com"` | Homepage URL. |
| `CMS_IDENTIFIER` | `CMS_SOFTWARE . "/1.0 (+" . CMS_HOMEPAGE . ")"` | Full software identifier. |
| `CMS` | `CMS_SOFTWARE . " " . CMS_VERSION . " " . CMS_COPYRIGHT . ", " . CMS_HOMEPAGE` | Comprehensive software description. |

---

## Requirements and Resources

### System Requirements
The following checks are performed to ensure the environment meets the minimum requirements:

1. **PHP Version:** Requires PHP 7.4.0 or higher.
2. **MySQL Support:** Requires either `mysqli` or `mysql` extension.
3. **PCRE Support:** Requires the `pcre` extension with UTF-8 support.
4. **Sodium Support:** Requires the `sodium` extension for cryptographic operations.

### Resource Limits
The `cms_ini_set_minimum()` function ensures minimum resource limits are set for the following PHP configurations:

| Configuration | Minimum Value |
|---------------|---------------|
| `max_execution_time` | `600` (10 minutes) |
| `max_file_uploads` | `500` |
| `memory_limit` | `256M` |
| `post_max_size` | `505M` |
| `upload_max_filesize` | `500M` |

---

### `cms_ini_set_minimum()`
**Purpose:**
Sets a PHP configuration value to the higher of its current value or a specified minimum.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | PHP configuration key (e.g., `memory_limit`). |
| `$value` | `string` | Minimum value to enforce (e.g., `256M`). |

**Return Values:**
- `mixed`: The new value if successful, `FALSE` on failure.

**Inner Mechanisms:**
- Parses the provided value and current value to compare them numerically, accounting for unit suffixes (e.g., `K`, `M`, `G`).
- Sets the configuration to the higher value.

**Usage:**
Used to enforce minimum resource limits during initialization.

---

## Initial Settings

### Configuration
The following initial settings are applied:

1. **User Abort:** `ignore_user_abort(TRUE)` prevents script termination if the user aborts the request.
2. **Output Buffering:** `ob_implicit_flush(FALSE)` and `ob_start()` enable output buffering.
3. **Timezone:** Sets the default timezone to `UTC`.
4. **File Permissions:** `umask(0002)` sets default file permissions to `664` (directories: `775`).

---

## Error Handling

### `cms_error()`
**Purpose:**
Handles errors by logging, buffering, and optionally displaying them based on the context.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$code` | `int` | | Error code (e.g., `E_ERROR`). Use `-1` to output buffered errors. |
| `$message` | `string` | | Error message. |
| `$path` | `string` | `NULL` | File path where the error occurred. |
| `$line` | `int` | `NULL` | Line number where the error occurred. |
| `$display` | `bool` | `NULL` | Force display of the error regardless of context. |

**Return Values:**
- `bool`: `TRUE` if the error was handled, `FALSE` otherwise.

**Inner Mechanisms:**
- **Silent Mode:** Errors are ignored if `cms.error.silent` is set in the cache.
- **Error Buffering:** Errors are stored in a static array for later output.
- **Error Output:** Errors are output to the browser console if:
  - Explicitly requested (`$display = TRUE`).
  - The request originates from a local development environment (`127.0.0.1` or `::1`).
  - The user is an administrator (`CMS_SUPERUSER = "admin"`).
- **Error Logging:** Errors are logged to `CMS_DATA_PATH . "#log/error.txt"` with a maximum size of 500 KB.
- **Stack Trace:** Detailed stack traces are included in the output if the error is displayed.

**Usage:**
- Called by the error handler (`set_error_handler`) to process PHP errors.
- Called with `$code = -1` to output buffered errors during script termination.

---

### `cms_error_silent()`
**Purpose:**
Enables or disables silent error handling mode.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$flag` | `bool` | `TRUE` | Whether to enable silent mode. |

**Return Values:**
- `bool`: The previous silent mode state.

**Inner Mechanisms:**
- Stores the silent mode flag in the cache under the key `cms.error.silent`.

**Usage:**
Used to suppress error output during sensitive operations (e.g., background tasks).

---

### `cms_shutdown()`
**Purpose:**
Captures fatal errors and outputs the error buffer during script termination.

**Inner Mechanisms:**
- Checks for fatal errors using `error_get_last()`.
- Calls `cms_error()` to handle fatal errors.
- Outputs buffered errors by calling `cms_error(-1, "")`.

**Usage:**
Registered as a shutdown function (`register_shutdown_function`) to ensure errors are handled during script termination.

---

## Input Preprocessing

### `cms_initialize_globals()`
**Purpose:**
Sanitizes and loads superglobal data (`$_COOKIE`, `$_GET`, `$_POST`, `$_FILES`) into the global scope.

**Inner Mechanisms:**
- Normalizes line breaks and repairs invalid UTF-8 in input data using `cms_utf8_normalize()`.
- Loads sanitized data into `$GLOBALS` for direct access.
- Handles `$_FILES` by flattening the array structure (e.g., `$_FILES["key"]["tmp_name"]` becomes `$GLOBALS["key_tmp_name"]`).

**Usage:**
Called during initialization to ensure all input data is sanitized and accessible.

---

### `cms_utf8_normalize()`
**Purpose:**
Normalizes line breaks and repairs invalid UTF-8 in input data.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `mixed` | Input value (string or array). |

**Return Values:**
- `mixed`: Normalized value (string or array).

**Inner Mechanisms:**
- Recursively processes arrays.
- Replaces `\r\n` and `\r` with `\n`.
- Uses the `utf8_normalize()` function (assumed to be defined elsewhere) to repair invalid UTF-8.

**Usage:**
Used by `cms_initialize_globals()` to sanitize input data.

---

## Identification

### `cms_identification()`
**Purpose:**
Handles user authentication, logout, account expiration, CSRF protection, and brute-force attack prevention.

**Inner Mechanisms:**
1. **Logout:** Clears authentication cookies and redirects to the root URL.
2. **CSRF Protection:** Generates and verifies security tokens for non-anonymous users.
3. **Login Attempts:**
   - Limits login attempts to `CMS_LOGIN_ATTEMPT_MAX` (5) per IP address.
   - Blocks further attempts for `CMS_LOGIN_BLOCK_TIME` (30 minutes) after exceeding the limit.
4. **Authentication:**
   - Validates user credentials against the `permission` system.
   - Sets constants for the authenticated user (`CMS_USER`, `CMS_PASSWORD`, `CMS_TOKEN`, etc.).
5. **Redirection:**
   - Redirects unauthorized users to the identification prompt.
   - Redirects administrators to the setup page if the system is not configured.
6. **Account Expiration:** Sets an expiration timestamp for user accounts if configured.

**Usage:**
Called during initialization to authenticate users and enforce security policies.

---

### `cms_generate_id()`
**Purpose:**
Generates anonymous fingerprints and IP hashes for the current client.

**Inner Mechanisms:**
- Uses a salt (`cms.salt_id`) to obfuscate client-specific data (e.g., `HTTP_USER_AGENT`, `REMOTE_ADDR`).
- Generates a unique user ID (`CMS_USERID`) using `ripemd128`.
- Generates an IP hash (`CMS_IPHASH`) for the client's IP address.

**Usage:**
Called during initialization to generate unique identifiers for anonymous users.

---

## Language

### `cms_language()`
**Purpose:**
Initializes language settings based on user preferences and system configuration.

**Inner Mechanisms:**
1. **Default Language:** Retrieves the default language from the system configuration.
2. **User Language:** Retrieves the user's preferred language from cookies or cache.
3. **Language Selection:** Allows users to select a language via `$cms_select_language`.
4. **Fallback:** Uses the browser's `Accept-Language` header if no language is set.
5. **Language File:** Loads the appropriate language file (`#language/[lang].language.inc`).

**Usage:**
Called during initialization to set the language for the current session.

---

### `cms_language_extract()`
**Purpose:**
Finds the best match between the user's requested languages and the system's supported languages.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$requested` | `string` | RFC 9110 / BCP 47 language string (e.g., `en-US,en;q=0.9`). |
| `$supported` | `string` | Comma-separated list of supported languages (e.g., `en,de,fr`). |

**Return Values:**
- `string|null`: The best-matching language or `NULL` if no match is found.

**Inner Mechanisms:**
- Parses the requested languages using `cms_language_parse()`.
- Compares requested languages against supported languages, prioritizing exact matches and higher specificity.

**Usage:**
Used by `cms_language()` to determine the best language for the user.

---

### `cms_language_parse()`
**Purpose:**
Parses an RFC 9110 / BCP 47 language string into its components.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Language string (e.g., `en-US,en;q=0.9`). |

**Return Values:**
- `array`: Parsed language components, including:
  - `code`: ISO 639 language code.
  - `script`: ISO 15924 script subtag.
  - `region`: ISO 3166-1 or UN M.49 region code.
  - `variant`: Registered language variants.
  - `q`: Priority value (0.0 to 1.0).

**Inner Mechanisms:**
- Uses regular expressions to parse the language string into components.
- Sorts languages by priority (`q` value).

**Usage:**
Used by `cms_language_extract()` to parse language preferences.

---

## Libraries

### `cms_load()`
**Purpose:**
Loads a library file or checks its availability.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$library` | `string` | | Name of the library to load (e.g., `"search"`). |
| `$exit_on_error` | `bool` | `FALSE` | Whether to terminate execution if the library cannot be loaded. |
| `$test` | `bool` | `FALSE` | Whether to only test for library availability without loading it. |

**Return Values:**
- `bool`: `TRUE` if the library is loaded/available, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks if the library has already been loaded or tested.
- Loads the library from `CMS_SYSTEM_PATH . "lib.$library.inc"`.
- Stores the load state in a static array to avoid redundant loading.

**Usage:**
- Used to load system libraries (e.g., `search`, `log`).
- Used to test library availability with `$test = TRUE`.

---

### `cms_available()`
**Purpose:**
Checks if a library is available without loading it.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$library` | `string` | Name of the library to check. |

**Return Values:**
- `bool`: `TRUE` if the library is available, `FALSE` otherwise.

**Inner Mechanisms:**
- Calls `cms_load()` with `$test = TRUE`.

**Usage:**
Used to verify library availability before attempting to load it.

---

### `cms_load_system()`
**Purpose:**
Loads all system libraries from the `#system` directory.

**Inner Mechanisms:**
- Scans `CMS_SYSTEM_PATH` for files matching `sys.*.inc`.
- Loads each matching file.

**Usage:**
Called during initialization to load core system libraries.

---

## Applications

### `cms_application()`
**Purpose:**
Loads an application module or checks permissions.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$application` | `string|null` | `NULL` | Name of the application to load. Use `TRUE` to check permissions for the current application. |
| `$instance` | `string|null` | `NULL` | Instance of the application. Use `TRUE` to check permissions for the current instance. |
| `$permission` | `string|null` | `NULL` | Permission to check. |
| `$user` | `string|null` | `NULL` | User to check permissions for. |

**Return Values:**
- `mixed`:
  - `bool`: `TRUE` if the permission is granted, `FALSE` otherwise.
  - `string`: Current application, instance, or application.instance.

**Inner Mechanisms:**
- **Application Loading:** Loads the application module from `CMS_MODULES_PATH . "#module/mod.$application.inc"` if the user has permission.
- **Permission Checks:** Uses the `permission` system to verify access.
- **Context Information:** Returns the current application, instance, or application.instance if no parameters are provided.

**Usage:**
- Used to load application modules dynamically.
- Used to check permissions for specific applications, instances, or users.

---

### `cms_instance()`
**Purpose:**
Returns the current application instance.

**Return Values:**
- `string`: Current instance.

**Usage:**
Used to retrieve the current instance name.

---

### `cms_permission()`
**Purpose:**
Checks permissions for the current or specified user.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$permission` | `string` | | Permission to check. |
| `$application` | `bool` | `TRUE` | Whether to check the application's permission. |
| `$instance` | `bool` | `TRUE` | Whether to check the instance's permission. |
| `$user` | `string|null` | `NULL` | User to check permissions for. |

**Return Values:**
- `bool`: `TRUE` if the permission is granted, `FALSE` otherwise.

**Inner Mechanisms:**
- Calls `cms_application()` with the appropriate parameters to check permissions.

**Usage:**
Used to verify user permissions for specific actions.

---

## URL Functions

### `cms_url()`
**Purpose:**
Generates a URL for the current or specified address, merging stored and provided parameters.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string|array` | `NULL` | URL or array of parameters. If `NULL`, uses the current URL. |
| `$param` | `array` | `NULL` | Parameters to add/overwrite. |
| `$omit_param` | `bool` | `FALSE` | Whether to omit stored parameters. |

**Return Values:**
- `string|bool`: Generated URL or `FALSE` on failure.

**Inner Mechanisms:**
- Parses the provided address using `parse_url()`.
- Merges the address's query parameters with `$param`.
- Builds the URL using `cms_build_url()`.

**Usage:**
Used to generate URLs for navigation, forms, and API calls.

---

### `cms_param()`
**Purpose:**
Manages querystring parameters, including storage, retrieval, and generation.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `mixed` | `NULL` | Value to store or key to retrieve. Use `TRUE` to retrieve all stored values. Use `FALSE` to delete values. |
| `$key` | `string|bool` | `NULL` | Key to store/retrieve. Use `TRUE` to omit stored values during generation. |
| `$omit_token` | `bool` | `FALSE` | Whether to omit the CSRF token during generation. |

**Return Values:**
- `mixed`:
  - `string`: Generated querystring.
  - `bool`: `TRUE` if the value was stored/deleted, `FALSE` otherwise.
  - `array`: All stored values if `$value = TRUE`.
  - `mixed`: Stored value for the specified key.

**Inner Mechanisms:**
- **Storage:** Stores values in a static array (`$param`).
- **Retrieval:** Retrieves values from the static array.
- **Deletion:** Deletes values from the static array.
- **Querystring Generation:** Recursively processes parameters and constructs a querystring.
- **CSRF Protection:** Prepends the CSRF token (`CMS_TOKEN`) to the querystring unless `$omit_token = TRUE`.

**Usage:**
- Used to manage URL parameters across requests.
- Used to generate querystrings for forms and links.

---

### `cms_build_url()`
**Purpose:**
Builds a valid URL from its components.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$parts` | `array` | URL components (e.g., `scheme`, `host`, `path`, `query`). |

**Return Values:**
- `string`: Constructed URL.

**Inner Mechanisms:**
- Constructs the URL from the provided components, handling edge cases (e.g., missing scheme, path normalization).

**Usage:**
Used by `cms_url()` to construct the final URL.

---

## Cache Data Storage

### `cms_cache()`
**Purpose:**
Manages temporary and permanent cache storage.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$key` | `string` | `NULL` | Cache key. Use `NULL` to store a value with an automatic numeric key. |
| `$value` | `mixed` | `NULL` | Value to store. Use `""` to delete the key. |
| `$permanent` | `bool` | `FALSE` | Whether to store the value permanently (on disk). |
| `$notouch` | `bool` | `FALSE` | Whether to avoid updating the access time for permanent values. |

**Return Values:**
- `mixed`:
  - `bool`: `TRUE` if the value was stored/deleted, `FALSE` otherwise.
  - `mixed`: Cached value for the specified key.
  - `array`: All stored and previously retrieved values if `$key = NULL`.

**Inner Mechanisms:**
- **Temporary Storage:** Uses a static array (`$cache`) for in-memory storage.
- **Permanent Storage:** Stores values in `CMS_DATA_PATH . "#cache/"` using a hashed directory structure.
- **Deletion:** Deletes values from both temporary and permanent storage if `$value = ""`.
- **Retrieval:** Retrieves values from temporary storage if available, otherwise from permanent storage.

**Usage:**
- Used to cache data temporarily or permanently.
- Used to retrieve cached data.

---

### `cms_cache_sync()`
**Purpose:**
Initializes an undefined or empty variable with a cached value or updates the cache with the variable's value.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$variable` | `mixed` | Variable to synchronize. |
| `$key` | `string` | Cache key. |
| `$load_on_empty` | `bool` | Whether to load the cache value if the variable is empty. |

**Inner Mechanisms:**
- If the variable is undefined or empty (and `$load_on_empty = TRUE`), loads the value from the cache.
- Otherwise, stores the variable's value in the cache.

**Usage:**
Used to synchronize variables with cached values.

---

### `cms_cache_notouch()`
**Purpose:**
Retrieves a cached value without updating its access time.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Cache key. |

**Return Values:**
- `mixed`: Cached value.

**Inner Mechanisms:**
- Calls `cms_cache()` with `$notouch = TRUE`.

**Usage:**
Used to retrieve cached values without affecting their expiration time.

---

### `cms_cache_time()`
**Purpose:**
Returns the last modification time of a cached value.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Cache key. |

**Return Values:**
- `int|bool`: Timestamp of the last modification or `FALSE` if the key does not exist.

**Usage:**
Used to check the age of cached values.

---

### `cms_cache_touch()`
**Purpose:**
Updates the access time of a cached value.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Cache key. |

**Return Values:**
- `bool`: `TRUE` if the access time was updated, `FALSE` otherwise.

**Usage:**
Used to reset the expiration time of cached values.

---

### `cms_cache_clean()`
**Purpose:**
Removes expired cache files from the specified directory.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$path` | `string` | | Path to the cache directory. |
| `$force` | `bool` | `FALSE` | Whether to force deletion of all files. |

**Return Values:**
- `bool`: `TRUE` if the cleanup was successful, `FALSE` otherwise.

**Inner Mechanisms:**
- Recursively traverses the cache directory.
- Deletes files that have not been accessed within `CMS_CACHE_TTL` (30 days) unless `$force = TRUE`.

**Usage:**
Used to clean up expired cache files during initialization.

---

## Daemon

### `cms_daemon()`
**Purpose:**
Adds a background task to the daemon queue.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$code` | `string` | | PHP code to execute in the background. |
| `$id` | `string` | `NULL` | Unique identifier to prevent duplicate queueing. |
| `$interval` | `int` | `0` | Minimum interval (in seconds) before the task can be requeued. |
| `$status` | `string` | `""` | Status message for the task. |

**Return Values:**
- `bool`: `TRUE` if the task was queued successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Generates a unique filename for the task using a hash of the `$id` or `$code`.
- Writes the task code to a file in `CMS_DATA_PATH . "#daemon/"`.
- Creates a flag file (`daemon.flag`) to indicate that tasks are available.

**Usage:**
Used to schedule background tasks (e.g., search updates, log cleaning).

---

### `cms_daemon_status()`
**Purpose:**
Sets or retrieves the status of the background worker.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | `NULL` | Status message to set. Use `NULL` to retrieve the current status. |

**Return Values:**
- `string|bool`: Current status if `$value = NULL`, `TRUE` if the status was set successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Stores the status in `CMS_DATA_PATH . "#daemon/daemon.status"`.
- Limits the status file to the last 25 lines.

**Usage:**
Used to log the progress of background tasks.

---

### `cms_daemon_exists()`
**Purpose:**
Checks if a task with the specified ID is queued.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Task ID. |

**Return Values:**
- `bool`: `TRUE` if the task is queued, `FALSE` otherwise.

**Usage:**
Used to check if a task is already in the queue.

---

### `cms_daemon_run()`
**Purpose:**
Starts a discrete asynchronous background worker.

**Return Values:**
- `bool`: `TRUE` if the daemon was started successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks for the presence of the `daemon.flag` file.
- Attempts to acquire an advisory lock (`daemon.lock`) to prevent multiple daemons from running simultaneously.
- Invokes the daemon worker (`daemon.php`) via an HTTP request.

**Usage:**
Called during initialization to start background tasks.

---

## Miscellaneous

### `cms_set_cookie()`
**Purpose:**
Stores or deletes session cookies.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Associative array of cookie names and values. Use `NULL` to delete a cookie. |

**Return Values:**
- `bool`: `TRUE` if the cookies were set successfully, `FALSE` otherwise.

**Inner Mechanisms:**
- Sets cookies with the following options:
  - `httponly`: `TRUE`
  - `path`: `CMS_RELATIVE_URL`
  - `samesite`: `Strict`
  - `secure`: `TRUE` if `CMS_PROTOCOL = "https"`
- Deletes cookies by setting their expiration time to the past.

**Usage:**
Used to manage session cookies (e.g., authentication, language preferences).

---

### `cms_email_agent()`
**Purpose:**
Initializes the system email address.

**Inner Mechanisms:**
- Retrieves the email address from the system configuration.
- Falls back to `mailagent@[CMS_DOMAIN]` if no address is configured.

**Usage:**
Called during initialization to set the system email address (`CMS_EMAIL_AGENT`).

---

### `cms_path_urlencode()`
**Purpose:**
Encodes non-alphanumeric characters in a string according to RFC 1738, excluding path separators (`/`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | String to encode. |

**Return Values:**
- `string`: Encoded string.

**Inner Mechanisms:**
- Uses `preg_replace_callback()` to encode non-alphanumeric characters (except `$-_.+!*'(),`).

**Usage:**
Used to encode file paths for URLs.

---

## System Constants

### System
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_APPLICATION` | `$application` | Current application name. |
| `CMS_INSTANCE` | `$instance` | Current instance name. |
| `CMS_DB_PREFIX` | `"cms_"` | Database table prefix. |
| `CMS_IFC_EDITION` | `"ifc"` | Interface edition identifier. |

### Paths
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_PATH` | `dirname(__FILE__) . "/"` | Absolute path to the NUOS installation. |
| `CMS_SYSTEM_PATH` | `CMS_PATH . "#system/"` | Path to system libraries. |
| `CMS_MODULES_PATH` | `CMS_PATH . "module/"` | Path to application modules. |
| `CMS_INTERFACE_PATH` | `CMS_MODULES_PATH . "#interface/"` | Path to interface modules. |
| `CMS_DESKTOP_PATH` | `CMS_MODULES_PATH . "#desktop/"` | Path to desktop modules. |
| `CMS_IMAGES_PATH` | `CMS_PATH . "image/"` | Path to image assets. |
| `CMS_ROOT_PATH` | `dirname(CMS_PATH) . "/"` | Path to the root directory. |
| `CMS_DATA_PATH` | `CMS_ROOT_PATH . "data/"` | Path to data storage. |

### URLs
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_PROTOCOL` | `"https"` or `"http"` | Protocol used for the current request. |
| `CMS_HOST` | `CMS_PROTOCOL . "://" . $http_host` | Full host URL. |
| `CMS_DOMAIN` | `$http_host` without port | Domain name. |
| `CMS_ACTIVE_URL` | `CMS_HOST . $php_self` | URL of the current script. |
| `CMS_URL` | `CMS_HOST . $base_url` | Base URL of the NUOS installation. |
| `CMS_MODULES_URL` | `CMS_URL . "module/"` | URL to application modules. |
| `CMS_IMAGES_URL` | `CMS_URL . "image/"` | URL to image assets. |
| `CMS_JAVA_URL` | `CMS_URL . "java/"` | URL to Java applets. |
| `CMS_JAVASCRIPT_URL` | `CMS_URL . "javascript/"` | URL to JavaScript files. |
| `CMS_SOUNDS_URL` | `CMS_URL . "sound/"` | URL to sound files. |
| `CMS_ROOT_URL` | `dirname(CMS_URL) . "/"` | Root URL. |
| `CMS_RELATIVE_URL` | `$relative_url` | Relative URL path. |
| `CMS_DATA_URL` | `CMS_ROOT_URL . "data/"` | URL to data storage. |

### Configuration
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_APACHE` | `function_exists("apache_get_version")` | Whether the server is Apache. |
| `CMS_CACHE_TTL` | `2592000` (30 days) | Cache time-to-live in seconds. |
| `CMS_USER_AGENT` | `"Mozilla/5.0 (compatible) NUOS/1.0 (+[CMS_HOST])"` | User agent string. |
| `CMS_LOGIN_ATTEMPT_MAX` | `5` | Maximum number of failed login attempts before blocking. |
| `CMS_LOGIN_BLOCK_TIME` | `1800` (30 minutes) | Duration of login block after exceeding attempts. |
| `CMS_PERMISSION_ALWAYS` | `"identification|index|check|security"` | Applications always accessible to all users. |

### Regex Patterns
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_REGEX_MATTER` | `(?![\\x7C])[\p{Cf}\p{L}\p{M}\p{N}\p{Pc}\p{S}\\x23\\x25\\x27\\x2F\\x5C]` | Characters allowed in content. |
| `CMS_REGEX_JOINT` | `(?![\\x22\\x23\\x25\\x27\\x2F\\x3B\\x5C\\x{00A1}\\x{00B6}\\x{00B7}\\x{00BF}])[\p{Pd}\p{Po}]` | Characters allowed as word joints. |
| `CMS_REGEX_SEPARATOR` | `[\s\p{Pe}\p{Pf}\p{Pi}\p{Ps}\p{Z}\\x0B\\x22\\x3B\\x7C\\x{00A1}\\x{00B6}\\x{00B7}\\x{00BF}]` | Characters treated as separators. |
| `CMS_REGEX_WORD` | `(?:" . CMS_REGEX_MATTER . "\|(?:" . CMS_REGEX_JOINT . ")+(?=" . CMS_REGEX_MATTER . "))+` | Regex pattern for words. |
| `CMS_REGEX_BORDER` | `(?:" . CMS_REGEX_SEPARATOR . "\|(?:" . CMS_REGEX_JOINT . ")+(?!" . CMS_REGEX_MATTER . "))+` | Regex pattern for word borders. |

### Security Event Types
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_SECURITY_EVENT_CSRF` | `1` | CSRF security event type. |

---

## Initialization

The following functions are called during initialization to set up the environment:

1. `cms_load_system()`: Loads system libraries.
2. `cms_generate_id()`: Generates user and IP hashes.
3. `cms_initialize_globals()`: Sanitizes and loads superglobal data.
4. `cms_language()`: Initializes language settings.
5. `cms_email_agent()`: Initializes the system email address.
6. `cms_cache_clean()`: Cleans up expired cache files.
7. `cms_identification()`: Authenticates the user.

---

## HTML Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DOCTYPE_HTML` | `"<!DOCTYPE HTML>"` | HTML5 doctype. |
| `CMS_BOT_CHECK` | Preload link for bot check | Preloads the bot check image for anonymous users. |
| `CMS_HTML_HEADER` | Meta tags and title | Standard HTML header. |
| `CMS_JAVASCRIPT` | Script tags | JavaScript files for the platform. |
| `CMS_STYLESHEET` | Stylesheet link | Link to the platform stylesheet. |
| `CMS_CLASS` | `"cms-[application]"` | CSS class for the current application. |

---

## HTTP Headers

The following HTTP headers are set during initialization:

1. **Cache-Control:**
   - `no-cache, must-revalidate` for anonymous users.
   - `no-store, no-cache, must-revalidate` for authenticated users.
2. **Content-Type:** `text/html; charset=utf-8`.
3. **Date:** Current date in GMT.
4. **Last-Modified:** Current date in GMT.
5. **X-Generator:** `CMS_IDENTIFIER`.

---

## Daemon Tasks

The following daemon tasks are scheduled during initialization:

1. **Search Update Processing:**
   - Task: Updates the search index.
   - ID: `search.daemon.update`.
   - Interval: 300 seconds (5 minutes).
2. **Search Score Processing:**
   - Task: Computes search scores.
   - ID: `search.daemon.score`.
   - Interval: 3600 seconds (1 hour).
3. **Log Cleaning:**
   - Task: Cleans up old log entries.
   - ID: `log.daemon`.
   - Interval: 300 seconds (5 minutes).

The `cms_daemon_run()` function is called to start the background worker.

---

## Clean Up

The following global variables are unset at the end of the script to prevent leakage:

- `$cms_user`
- `$cms_password`
- `$cms_login_user`
- `$cms_login_password`


<!-- HASH:b3d3a5a4a65a36bb6fd0c0a2c710f2ce -->
