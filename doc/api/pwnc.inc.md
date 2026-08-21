# PWNC API Documentation

[← Index](README.md) | [`pwnc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/pwnc.inc)

- **Version:** `26.8.14.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC Core Initialization (`pwnc.inc`)

This file serves as the main entry point and initialization script for the PWNC Web Platform. It establishes the foundational environment, handles system requirements, configures core settings, and manages user identification, language selection, and application loading.

---

## Table of Contents

- [Namespace Isolation](#namespace-isolation)
- [Software Information](#software-information)
- [Requirements and Resources](#requirements-and-resources)
- [Initial Settings](#initial-settings)
- [Debugging](#debugging)
  - [`debug()`](#debug)
- [Error Handling](#error-handling)
  - [`cms_error()`](#cms_error)
  - [`cms_error_silent()`](#cms_error_silent)
  - [`cms_shutdown()`](#cms_shutdown)
- [Input Preprocessing](#input-preprocessing)
  - [`cms_initialize_globals()`](#cms_initialize_globals)
  - [`cms_utf8_normalize()`](#cms_utf8_normalize)
- [Identification](#identification)
  - [`cms_identification()`](#cms_identification)
  - [`cms_generate_id()`](#cms_generate_id)
- [Language](#language)
  - [`cms_language()`](#cms_language)
  - [`cms_language_extract()`](#cms_language_extract)
  - [`cms_language_parse()`](#cms_language_parse)
- [Libraries](#libraries)
  - [`cms_load()`](#cms_load)
  - [`cms_available()`](#cms_available)
  - [`cms_load_system()`](#cms_load_system)
- [Applications](#applications)
  - [`cms_application()`](#cms_application)
  - [`cms_instance()`](#cms_instance)
  - [`cms_permission()`](#cms_permission)
- [URL Functions](#url-functions)
  - [`cms_url()`](#cms_url)
  - [`cms_param()`](#cms_param)
  - [`cms_build_url()`](#cms_build_url)
- [Cache Data Storage](#cache-data-storage)
  - [`cms_cache()`](#cms_cache)
  - [`cms_cache_delete()`](#cms_cache_delete)
  - [`cms_cache_sync()`](#cms_cache_sync)
  - [`cms_cache_init()`](#cms_cache_init)
  - [`cms_cache_notouch()`](#cms_cache_notouch)
  - [`cms_cache_time()`](#cms_cache_time)
  - [`cms_cache_touch()`](#cms_cache_touch)
  - [`cms_cache_clean()`](#cms_cache_clean)
- [Daemon](#daemon)
  - [`cms_daemon()`](#cms_daemon)
  - [`cms_daemon_status()`](#cms_daemon_status)
  - [`cms_daemon_exists()`](#cms_daemon_exists)
  - [`cms_daemon_running()`](#cms_daemon_running)
  - [`cms_daemon_remove()`](#cms_daemon_remove)
  - [`cms_daemon_run()`](#cms_daemon_run)
- [Flag](#flag)
  - [`cms_flag_set()`](#cms_flag_set)
  - [`cms_flag_get()`](#cms_flag_get)
  - [`cms_flag_del()`](#cms_flag_del)
- [Miscellaneous](#miscellaneous)
  - [`cms_set_cookie()`](#cms_set_cookie)
  - [`cms_salt()`](#cms_salt)
  - [`cms_email_agent()`](#cms_email_agent)
  - [`cms_http_header()`](#cms_http_header)
  - [`cms_mcp_initialize()`](#cms_mcp_initialize)
  - [`cms_trusted_proxies()`](#cms_trusted_proxies)
  - [`cms_ip_in_cidr()`](#cms_ip_in_cidr)
  - [`cms_path_urlencode()`](#cms_path_urlencode)

---

## Namespace Isolation

The entire file is wrapped in an anonymous function to isolate the `cms` namespace. This prevents naming collisions and ensures that all constants and functions are defined within the `cms` namespace.

Two helper functions are defined to ensure that `define()`, `defined()`, and `constant()` operate within the `cms` namespace:

- `constant($name)`: Retrieves a constant from the `cms` namespace.
- `define($name, $value)`: Defines a constant in the `cms` namespace.
- `defined($name)`: Checks if a constant is defined in the `cms` namespace.

---

## Software Information

The following constants define the software identity and versioning:

| Name               | Value/Default                     | Description                                                                 |
|--------------------|-----------------------------------|-----------------------------------------------------------------------------|
| `CMS_SOFTWARE`     | `"PWNC"`                          | Name of the software.                                                       |
| `CMS_VERSION`      | Content of `version.txt` or `"?"` | Current version of the software.                                            |
| `CMS_COPYRIGHT`    | `"© 2026 Patrick Heyer"`          | Copyright notice.                                                           |
| `CMS_HOMEPAGE`     | `"https://pwnc.it"`               | Official homepage of the software.                                          |
| `CMS_IDENTIFIER`   | `CMS_SOFTWARE . "/1.0 (+" . CMS_HOMEPAGE . ")"` | Full identifier string for the software.                                    |
| `CMS`              | `CMS_SOFTWARE . " " . CMS_VERSION . " " . CMS_COPYRIGHT . ", " . CMS_HOMEPAGE` | Comprehensive software description. |

---

## Requirements and Resources

The script enforces the following requirements:

- **Execution Environment**: Disallows CLI execution.
- **PHP Version**: Requires PHP 7.4.0 or higher.
- **Extensions**: Requires `mysqli` and `pcre` extensions with UTF-8 support.

The `cms_ini_set_minimum()` function ensures that PHP resource limits meet the following minimum values:

| Setting                | Minimum Value | Description                                                                 |
|------------------------|---------------|-----------------------------------------------------------------------------|
| `max_execution_time`   | `600`         | Maximum script execution time in seconds.                                   |
| `max_file_uploads`     | `500`         | Maximum number of files allowed to be uploaded simultaneously.              |
| `memory_limit`         | `256M`        | Maximum memory a script may consume.                                        |
| `post_max_size`        | `505M`        | Maximum size of POST data.                                                  |
| `upload_max_filesize`  | `500M`        | Maximum size of an uploaded file.                                           |

---

## Initial Settings

The following initial settings are configured:

- **User Abort**: `ignore_user_abort(TRUE)` prevents script termination if the user aborts the request.
- **Output Buffering**: `ob_implicit_flush(FALSE)` and `ob_start()` enable output buffering.
- **Timezone**: Sets the default timezone to `UTC`.
- **File Permissions**: `umask(0002)` sets default file permissions to `664` (rw-rw-r--).

---

## Debugging

### `debug()`

**Purpose**: Outputs debug information to the browser's console.

**Parameters**:

| Name  | Type      | Description                     |
|-------|-----------|---------------------------------|
| `...$var` | `mixed` | Variables to debug. Accepts any number of arguments. |

**Return Values**: None.

**Inner Mechanisms**:
- Captures the debug backtrace to determine the calling file, line, and function.
- Uses `var_dump()` to generate debug output for each variable.
- Outputs the debug information as a JavaScript `console.group()` with a formatted title and the debug output as a `console.log()`.

**Usage Context**:
- Used during development to inspect variables and execution flow.
- Output is only visible in the browser's developer console.

**Example**:
```php
$testArray = [1, 2, 3];
$testString = "Hello, World!";
debug($testArray, $testString);
```
**Explanation**: Outputs the contents of `$testArray` and `$testString` to the browser's console, grouped under a collapsible debug entry.

---

## Error Handling

### `cms_error()`

**Purpose**: Handles and logs errors, warnings, and notices. Outputs errors to the browser console in development mode.

**Parameters**:

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$code`     | `int`     | Error code. Use `-1` to output the buffered errors.                        |
| `$message`  | `string`  | Error message.                                                              |
| `$path`     | `string`  | File path where the error occurred. Defaults to `NULL`.                     |
| `$line`     | `int`     | Line number where the error occurred. Defaults to `NULL`.                   |
| `$display`  | `bool`    | Forces error display regardless of mode. Defaults to `NULL`.                |

**Return Values**: `bool` – `TRUE` if the error was handled, `FALSE` if the error was ignored.

**Inner Mechanisms**:
- **Silent Mode**: Errors are ignored if `cms.error.silent` is set in the cache.
- **Error Buffering**: Errors are stored in a static array for later output.
- **Development Mode**: Determines if verbose error output should be displayed based on:
  - Explicit `$display` parameter.
  - Local development environment (IP address).
  - Admin user status.
  - Debug permission.
- **Error Types**: Maps error codes to human-readable names and colors for console output.
- **Verbose Output**: In development mode, includes a full stack trace with argument details.
- **Logging**: Prepends errors to a log file in `CMS_DATA_PATH . "#log/error.txt"`.
- **Output**: On `$code = -1`, outputs all buffered errors to the browser console if in development mode.

**Usage Context**:
- Registered as the default error handler via `set_error_handler()`.
- Used to handle all runtime errors, warnings, and notices.
- Provides detailed error information during development while suppressing it in production.

**Example**:
```php
// Trigger a user warning
trigger_error("This is a test warning", E_USER_WARNING);
```
**Explanation**: Logs the warning and outputs it to the browser console if in development mode.

---

### `cms_error_silent()`

**Purpose**: Enables or disables silent error handling mode.

**Parameters**:

| Name    | Type   | Description                                  |
|---------|--------|----------------------------------------------|
| `$flag` | `bool` | `TRUE` to enable silent mode, `FALSE` to disable. Defaults to `TRUE`. |

**Return Values**: `bool` – The previous silent mode state.

**Inner Mechanisms**:
- Uses `cms_cache()` to store the silent mode state under the key `cms.error.silent`.

**Usage Context**:
- Used to suppress error output during sensitive operations or in production.

**Example**:
```php
// Enable silent mode
cms_error_silent(TRUE);

// Disable silent mode
cms_error_silent(FALSE);
```
**Explanation**: Enables or disables the suppression of error output.

---

### `cms_shutdown()`

**Purpose**: Captures fatal errors and outputs the error buffer on script termination.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- Uses `error_get_last()` to check for fatal errors (e.g., `E_ERROR`, `E_PARSE`).
- Calls `cms_error()` to handle any fatal errors.
- Calls `cms_error(-1, "")` to output the buffered errors.

**Usage Context**:
- Registered as a shutdown function via `register_shutdown_function()`.
- Ensures that fatal errors are logged and displayed appropriately.

**Example**:
```php
// No direct usage; automatically called on script termination.
```
**Explanation**: Automatically handles fatal errors and outputs buffered errors when the script terminates.

---

## Input Preprocessing

### `cms_initialize_globals()`

**Purpose**: Sanitizes and loads superglobal data (`$_COOKIE`, `$_GET`, `$_POST`, `$_FILES`) into the global scope.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- Prevents multiple executions by defining `CMS_INITIALIZE_GLOBALS_EXECUTED`.
- Iterates over `$_COOKIE`, `$_GET`, and `$_POST` superglobals, normalizing their values using `cms_utf8_normalize()` and storing them in `$GLOBALS`.
- Processes `$_FILES` superglobal, normalizing each file attribute and storing them in `$GLOBALS` with keys formatted as `fieldname_attribute`.

**Usage Context**:
- Called during initialization to sanitize and normalize user input.
- Ensures that all input data is UTF-8 compliant and uses consistent line breaks.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Sanitizes and normalizes all superglobal input data for use in the global scope.

---

### `cms_utf8_normalize()`

**Purpose**: Normalizes line breaks and repairs invalid UTF-8 strings.

**Parameters**:

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `$value` | `mixed` | String or array to normalize.   |

**Return Values**: `mixed` – Normalized string or array.

**Inner Mechanisms**:
- Recursively processes arrays using `array_map()`.
- Replaces `\r\n` and `\r` with `\n` for consistent line breaks.
- Uses `utf8_normalize()` to repair invalid UTF-8 sequences.

**Usage Context**:
- Used to sanitize user input and ensure UTF-8 compliance.
- Called by `cms_initialize_globals()` to normalize superglobal data.

**Example**:
```php
$input = "Hello\r\nWorld";
$normalized = cms_utf8_normalize($input);
// $normalized = "Hello\nWorld"
```
**Explanation**: Normalizes line breaks in the input string.

---

## Identification

### `cms_identification()`

**Purpose**: Handles user authentication, logout, account expiration, CSRF protection, and brute-force attack prevention.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **Security Check**: Exits if user-related constants are already defined.
- **Logout**: Handles logout requests by clearing cookies and redirecting to the root URL.
- **CSRF Protection**: Generates and verifies security tokens for non-anonymous users.
- **Login Attempts**: Limits login attempts to prevent brute-force attacks.
- **User Verification**: Verifies user credentials using the `permission` class.
- **Session Management**: Sets cookies for authenticated users and handles redirects for unauthorized access.
- **Account Expiration**: Sets account expiration for non-anonymous users if applicable.
- **Constants**: Defines user-related constants (`CMS_USER`, `CMS_PASSWORD`, `CMS_TOKEN`, etc.).

**Usage Context**:
- Called during initialization to authenticate users and set up their session.
- Manages user state, permissions, and security tokens.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Authenticates the user, sets up their session, and enforces security measures.

---

### `cms_generate_id()`

**Purpose**: Generates anonymous fingerprints and IP hashes for the current client.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **Salt**: Uses a salt that changes every 60 minutes to prevent tracking.
- **Fingerprint**: Generates a fingerprint from HTTP headers and IP address as a fallback when cookies are disabled.
- **Session ID**: Generates a session ID using `hash("ripemd128", $salt . $id)`.
- **Constants**: Defines `CMS_USERID` and `CMS_IPHASH` constants.

**Usage Context**:
- Called during initialization to generate unique identifiers for anonymous users.
- Used for tracking and security purposes.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Generates unique identifiers for the current client based on their IP and HTTP headers.

---

## Language

### `cms_language()`

**Purpose**: Initializes language settings based on user preferences and system configuration.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **Constants**: Defines `CMS_LANGUAGE_ENABLED`, `CMS_LANGUAGE_DEFAULT`, and `CMS_LANGUAGE`.
- **System Configuration**: Retrieves the default language from the `system` class.
- **User Preference**: Retrieves the user's selected language from cookies or cache.
- **Fallback**: Uses `cms_language_extract()` to determine the best language match from the `Accept-Language` header if no language is set.
- **Language File**: Includes the appropriate language file based on the selected language.

**Usage Context**:
- Called during initialization to set the language for the current session.
- Ensures that the user interface is displayed in the user's preferred language.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Sets the language for the current session based on user preferences and system configuration.

---

### `cms_language_extract()`

**Purpose**: Finds the best match of an RFC 9110 / BCP 47 language string in a list of supported languages.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$requested` | `string` | Comma-separated list of requested languages with optional priority values. |
| `$supported` | `string` | Comma-separated list of supported languages.                               |

**Return Values**: `string|null` – The best matching language or `NULL` if no match is found.

**Inner Mechanisms**:
- Parses the requested languages using `cms_language_parse()`.
- Compares each requested language against the supported languages.
- Prioritizes matches based on:
  - Exact match.
  - Higher specifity (e.g., `en-US` over `en`).
  - Higher priority value (`q` parameter).

**Usage Context**:
- Used by `cms_language()` to determine the best language match from the `Accept-Language` header.

**Example**:
```php
$requested = "en-US,en;q=0.9,fr;q=0.8";
$supported = "en,fr,de";
$bestMatch = cms_language_extract($requested, $supported);
// $bestMatch = "en"
```
**Explanation**: Finds the best matching language from the requested languages based on priority and specifity.

---

### `cms_language_parse()`

**Purpose**: Parses an RFC 9110 / BCP 47 language string into its components.

**Parameters**:

| Name     | Type     | Description                     |
|----------|----------|---------------------------------|
| `$string` | `string` | Language string to parse.       |

**Return Values**: `array` – An associative array of parsed language components.

**Component Mapping**:

| Key      | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `code`   | ISO 639 language code (2-3 characters).                                    |
| `script` | ISO 15924 script subtag (4 characters).                                     |
| `region` | ISO 3166-1 alpha-2 or UN M.49 code (2 characters or 3 digits).             |
| `variant`| Registered language variants.                                               |
| `q`      | Priority value (0.0 to 1.0).                                               |

**Inner Mechanisms**:
- Normalizes the input string by removing whitespace and converting to lowercase.
- Uses `preg_match_all()` to extract language components and priority values.
- Filters out languages with a priority of `0.0`.
- Sorts languages by priority in descending order.

**Usage Context**:
- Used by `cms_language_extract()` to parse the `Accept-Language` header.

**Example**:
```php
$languageString = "en-US;q=0.9,fr;q=0.8";
$parsed = cms_language_parse($languageString);
/*
$parsed = [
    "en-US" => [
        "code" => "en",
        "script" => "",
        "region" => "US",
        "variant" => "",
        "q" => 0.9
    ],
    "fr" => [
        "code" => "fr",
        "script" => "",
        "region" => "",
        "variant" => "",
        "q" => 0.8
    ]
];
*/
```
**Explanation**: Parses the language string into its components and sorts them by priority.

---

## Libraries

### `cms_load()`

**Purpose**: Loads or checks the availability of a library.

**Parameters**:

| Name             | Type      | Description                                                                 |
|------------------|-----------|-----------------------------------------------------------------------------|
| `$library`       | `string`  | Name of the library to load.                                                |
| `$exit_on_error` | `bool`    | If `TRUE`, terminates execution with an error message if the library cannot be loaded. Defaults to `FALSE`. |
| `$test`          | `bool`    | If `TRUE`, only checks if the library is available without loading it. Defaults to `FALSE`. |

**Return Values**:
- `bool` – `TRUE` if the library is loaded or available, `FALSE` otherwise.

**Inner Mechanisms**:
- Uses a static array `$loaded` to track the load state of libraries.
- Checks if the library file exists in `CMS_SYSTEM_PATH` with the filename format `lib.$library.inc`.
- If `$test` is `TRUE`, returns the availability of the library without loading it.
- If `$exit_on_error` is `TRUE`, terminates execution with an error message if the library cannot be loaded.

**Usage Context**:
- Used to load system libraries dynamically.
- Can be used to check library availability before loading.

**Example**:
```php
// Load the "database" library
cms_load("database");

// Check if the "search" library is available
if (cms_load("search", FALSE, TRUE)) {
    // Library is available
}
```
**Explanation**: Loads a library or checks its availability.

---

### `cms_available()`

**Purpose**: Checks if a library is available.

**Parameters**:

| Name       | Type     | Description                     |
|------------|----------|---------------------------------|
| `$library` | `string` | Name of the library to check.   |

**Return Values**: `bool` – `TRUE` if the library is available, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_load($library, FALSE, TRUE)` to check library availability.

**Usage Context**:
- Used to check if a library can be loaded before attempting to use it.

**Example**:
```php
if (cms_available("search")) {
    // Use the search library
}
```
**Explanation**: Checks if the "search" library is available.

---

### `cms_load_system()`

**Purpose**: Loads all system libraries.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- Prevents multiple executions by defining `CMS_LOAD_SYSTEM_EXECUTED`.
- Scans `CMS_SYSTEM_PATH` for files matching the pattern `sys.*.inc`.
- Loads each matching file.

**Usage Context**:
- Called during initialization to load all system libraries.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Loads all system libraries required for the platform to function.

---

## Applications

### `cms_application()`

**Purpose**: Loads applications, checks permissions, and retrieves application or instance information.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$application` | `mixed` | Application name, `TRUE` to use the current application, or `NULL` to retrieve information. |
| `$instance`    | `mixed` | Instance name, `TRUE` to use the current instance, or `NULL` to retrieve information. |
| `$permission`  | `string` | Permission to check.                                                        |
| `$user`        | `string` | User to check permissions for. Defaults to `NULL` (current user).          |

**Return Values**:
- `bool` – `TRUE` if the permission is granted, `FALSE` otherwise.
- `string` – Application or instance information if no permission is specified.

**Inner Mechanisms**:
- **Static Variables**: Uses static variables to store the current application, instance, and permission object.
- **Permission Checks**: Uses the `permission` class to check permissions for the current or specified user.
- **Application Loading**: Loads the application module if the user has the required permissions.
- **Information Retrieval**: Returns the current application, instance, or application.instance combination if no permission is specified.

**Usage Context**:
- Used to load applications dynamically.
- Used to check permissions for specific applications, instances, or actions.
- Used to retrieve information about the current application or instance.

**Example**:
```php
// Load the "blog" application
cms_application("blog");

// Check if the current user has "edit" permission for the current application.instance
if (cms_application(TRUE, TRUE, "edit")) {
    // User has permission
}

// Get the current application.instance
$appInstance = cms_application();
```
**Explanation**: Loads an application, checks permissions, or retrieves application information.

---

### `cms_instance()`

**Purpose**: Retrieves the current instance name.

**Parameters**: None.

**Return Values**: `string` – The current instance name.

**Inner Mechanisms**:
- Calls `cms_application(NULL, TRUE)` to retrieve the current instance.

**Usage Context**:
- Used to retrieve the current instance name for display or logic purposes.

**Example**:
```php
$instance = cms_instance();
// $instance = "default" (or the current instance name)
```
**Explanation**: Retrieves the name of the current instance.

---

### `cms_permission()`

**Purpose**: Checks permissions for the current or specified user.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$permission` | `string` | Permission to check.                                                        |
| `$application` | `bool`   | If `TRUE`, checks permissions for the current application. Defaults to `TRUE`. |
| `$instance`    | `bool`   | If `TRUE`, checks permissions for the current instance. Defaults to `TRUE`. |
| `$user`        | `string` | User to check permissions for. Defaults to `NULL` (current user).          |

**Return Values**: `bool` – `TRUE` if the permission is granted, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_application()` with the appropriate parameters to check permissions.

**Usage Context**:
- Used to check if the current or specified user has a specific permission.

**Example**:
```php
// Check if the current user has "edit" permission for the current application.instance
if (cms_permission("edit")) {
    // User has permission
}

// Check if the "admin" user has "delete" permission for the current application
if (cms_permission("delete", TRUE, NULL, "admin")) {
    // Admin has permission
}
```
**Explanation**: Checks if a user has a specific permission for the current application or instance.

---

## URL Functions

### `cms_url()`

**Purpose**: Generates a URL for the current or specified address with optional parameters.

**Parameters**:

| Name          | Type      | Description                                                                 |
|---------------|-----------|-----------------------------------------------------------------------------|
| `$address`    | `mixed`   | Address to generate the URL for. Can be a string, array, or object with `__toString()`. Defaults to `NULL` (current address). |
| `$param`      | `array`   | Parameters to add or overwrite in the URL. Defaults to `NULL`.              |
| `$omit_param` | `bool`    | If `TRUE`, omits stored parameters from the URL. Defaults to `FALSE`.       |

**Return Values**: `string|bool` – The generated URL or `FALSE` on failure.

**Inner Mechanisms**:
- **External/Executable Detection**: Determines if the address is external or executable (ends with `.php`).
- **Parameter Handling**: Merges the provided parameters with the stored parameters using `cms_param()`.
- **URL Construction**: Uses `cms_build_url()` to construct the URL from its parts.

**Usage Context**:
- Used to generate URLs for internal and external addresses with optional parameters.
- Handles CSRF protection tokens automatically.

**Example**:
```php
// Generate a URL for the current address with additional parameters
$url = cms_url(NULL, ["page" => 2, "sort" => "date"]);

// Generate a URL for an external address
$externalUrl = cms_url("https://example.com", ["ref" => "pwnc"]);

// Generate a URL omitting stored parameters
$cleanUrl = cms_url("blog.php", ["id" => 123], TRUE);
```
**Explanation**: Generates URLs with optional parameters, handling internal and external addresses.

---

### `cms_param()`

**Purpose**: Manages querystring parameters, including storage, retrieval, and generation.

**Parameters**:

| Name          | Type      | Description                                                                 |
|---------------|-----------|-----------------------------------------------------------------------------|
| `$value`      | `mixed`   | Value to store or key to retrieve.                                          |
| `$key`        | `mixed`   | Key to store the value under. If `TRUE`, generates a querystring omitting stored values. Defaults to `NULL`. |
| `$omit_token` | `bool`    | If `TRUE`, omits the security token from the generated querystring. Defaults to `FALSE`. |

**Return Values**:
- `string` – Generated querystring.
- `mixed` – Stored value or `NULL` if the key does not exist.
- `bool` – `TRUE` if the value was stored or deleted, `FALSE` otherwise.

**Inner Mechanisms**:
- **Storage**: Uses a static array `$param` to store parameters.
- **Value Management**: Stores, retrieves, or deletes values based on the provided key.
- **Querystring Generation**: Recursively processes parameters to generate a querystring.
- **CSRF Protection**: Prepends the security token to the querystring unless `$omit_token` is `TRUE`.

**Usage Context**:
- Used to manage querystring parameters for URL generation.
- Used to store and retrieve temporary values.

**Example**:
```php
// Store a parameter
cms_param("value1", "key1");

// Retrieve a parameter
$value = cms_param("key1");

// Generate a querystring with stored parameters
$querystring = cms_param();

// Generate a querystring with additional parameters
$querystring = cms_param(["page" => 2]);

// Generate a querystring omitting stored parameters
$querystring = cms_param(["page" => 2], TRUE);

// Delete a parameter
cms_param(FALSE, "key1");
```
**Explanation**: Manages querystring parameters, including storage, retrieval, and generation.

---

### `cms_build_url()`

**Purpose**: Constructs a valid URL from its parts.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$parts` | `array` | Associative array of URL parts. |

**Return Values**: `string` – The constructed URL.

**Inner Mechanisms**:
- Constructs the URL from the provided parts, including scheme, host, port, path, query, and fragment.
- Handles URL encoding for user, password, and fragment components.

**Usage Context**:
- Used by `cms_url()` to construct the final URL.

**Example**:
```php
$parts = [
    "scheme" => "https",
    "host" => "example.com",
    "path" => "/path/to/resource",
    "query" => "param1=value1&param2=value2",
    "fragment" => "section1"
];
$url = cms_build_url($parts);
// $url = "https://example.com/path/to/resource?param1=value1&param2=value2#section1"
```
**Explanation**: Constructs a URL from its parts.

---

## Cache Data Storage

### `cms_cache()`

**Purpose**: Manages temporary and permanent cache storage.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$key`       | `string`  | Cache key.                                                                  |
| `$value`     | `mixed`   | Value to store. If `""`, deletes the value. Defaults to `NULL`.             |
| `$permanent` | `bool`    | If `TRUE`, stores the value permanently. Defaults to `FALSE`.               |
| `$notouch`   | `bool`    | If `TRUE`, does not update the access time when retrieving the value. Defaults to `FALSE`. |

**Return Values**:
- `mixed` – The cached value or `NULL` if the key does not exist.
- `bool` – `TRUE` if the value was stored or deleted, `FALSE` otherwise.
- `array` – All stored and previously retrieved values if `$key` is `NULL`.

**Inner Mechanisms**:
- **Temporary Storage**: Uses a static array `$cache` for temporary storage.
- **Permanent Storage**: Stores values in the filesystem under `CMS_DATA_PATH . "#cache/"`.
- **Tombstones**: Uses `NULL` to mark deleted keys in the temporary cache.
- **File Handling**: Uses unique temporary files and atomic operations to ensure data integrity.

**Usage Context**:
- Used to store and retrieve temporary and permanent cache values.
- Used to manage cache expiration and cleanup.

**Example**:
```php
// Store a temporary value
cms_cache("temp_key", "temp_value");

// Store a permanent value
cms_cache("perm_key", "perm_value", TRUE);

// Retrieve a value
$value = cms_cache("temp_key");

// Retrieve a value without updating its access time
$value = cms_cache("perm_key", NULL, FALSE, TRUE);

// Delete a value
cms_cache("temp_key", "");

// Delete a permanent value
cms_cache("perm_key", "", TRUE);

// Retrieve all cached values
$allValues = cms_cache();
```
**Explanation**: Manages temporary and permanent cache storage for values.

---

### `cms_cache_delete()`

**Purpose**: Deletes one or more cache keys.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$key`       | `mixed`   | Key or array of keys to delete.                                             |
| `$permanent` | `bool`    | If `TRUE`, deletes permanent values. Defaults to `TRUE`.                    |

**Return Values**: `bool` – `TRUE` if all keys were deleted successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_cache($key, "", $permanent)` for each key.

**Usage Context**:
- Used to delete cache values, including permanent storage.

**Example**:
```php
// Delete a single key
cms_cache_delete("temp_key");

// Delete multiple keys
cms_cache_delete(["key1", "key2"]);

// Delete a permanent key
cms_cache_delete("perm_key", TRUE);
```
**Explanation**: Deletes one or more cache keys, optionally including permanent storage.

---

### `cms_cache_sync()`

**Purpose**: Initializes an undefined or empty variable with a cache value or updates the cache with the variable's value.

**Parameters**:

| Name             | Type      | Description                                                                 |
|------------------|-----------|-----------------------------------------------------------------------------|
| `&$variable`     | `mixed`   | Variable to synchronize.                                                    |
| `$key`           | `string`  | Cache key.                                                                  |
| `$default`       | `mixed`   | Default value to use if the cache value is empty. Defaults to `NULL`.      |
| `$load_on_empty` | `bool`    | If `TRUE`, loads the cache value if the variable is empty. Defaults to `FALSE`. |
| `$no_store`      | `bool`    | If `TRUE`, does not store the variable's value in the cache. Defaults to `FALSE`. |

**Return Values**: `mixed` – The synchronized value.

**Inner Mechanisms**:
- Checks if the variable is undefined or empty.
- If the variable is empty and `$load_on_empty` is `TRUE`, retrieves the value from the cache.
- If the cache value is empty, uses the default value.
- If the variable is not empty and `$no_store` is `FALSE`, stores the variable's value in the cache.

**Usage Context**:
- Used to synchronize variables with cache values, ensuring that they are initialized or updated.

**Example**:
```php
$variable = "";
$value = cms_cache_sync($variable, "cache_key", "default_value", TRUE);
// $variable = "default_value" (if cache is empty)
// $value = "default_value"

$variable = "new_value";
$value = cms_cache_sync($variable, "cache_key");
// Cache is updated with "new_value"
```
**Explanation**: Synchronizes a variable with a cache value, initializing it if empty or updating the cache if not.

---

### `cms_cache_init()`

**Purpose**: Initializes an undefined or empty variable with a cache value.

**Parameters**:

| Name             | Type      | Description                                                                 |
|------------------|-----------|-----------------------------------------------------------------------------|
| `&$variable`     | `mixed`   | Variable to initialize.                                                     |
| `$key`           | `string`  | Cache key.                                                                  |
| `$default`       | `mixed`   | Default value to use if the cache value is empty. Defaults to `NULL`.      |
| `$load_on_empty` | `bool`    | If `TRUE`, loads the cache value if the variable is empty. Defaults to `FALSE`. |

**Return Values**: `mixed` – The initialized value.

**Inner Mechanisms**:
- Calls `cms_cache_sync($variable, $key, $default, $load_on_empty, TRUE)` to initialize the variable without storing it in the cache.

**Usage Context**:
- Used to initialize variables with cache values without updating the cache.

**Example**:
```php
$variable = "";
$value = cms_cache_init($variable, "cache_key", "default_value", TRUE);
// $variable = "default_value" (if cache is empty)
// $value = "default_value"
```
**Explanation**: Initializes a variable with a cache value without updating the cache.

---

### `cms_cache_notouch()`

**Purpose**: Retrieves a cache value without updating its access time.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$key` | `string` | Cache key.        |

**Return Values**: `mixed` – The cached value or `NULL` if the key does not exist.

**Inner Mechanisms**:
- Calls `cms_cache($key, NULL, FALSE, TRUE)`.

**Usage Context**:
- Used to retrieve cache values without affecting their expiration time.

**Example**:
```php
$value = cms_cache_notouch("cache_key");
```
**Explanation**: Retrieves a cache value without updating its access time.

---

### `cms_cache_time()`

**Purpose**: Retrieves the last access time of a cache key.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$key` | `string` | Cache key.        |

**Return Values**: `int|bool` – The last access time as a Unix timestamp or `FALSE` if the key does not exist.

**Inner Mechanisms**:
- Computes the cache file path using `hash("ripemd128", $key)`.
- Checks the modification time of the serialized or string cache file.

**Usage Context**:
- Used to determine when a cache value was last accessed.

**Example**:
```php
$time = cms_cache_time("cache_key");
// $time = 1625097600 (Unix timestamp)
```
**Explanation**: Retrieves the last access time of a cache key.

---

### `cms_cache_touch()`

**Purpose**: Updates the access time of a cache key.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$key` | `string` | Cache key.        |

**Return Values**: `bool` – `TRUE` if the access time was updated, `FALSE` otherwise.

**Inner Mechanisms**:
- Computes the cache file path using `hash("ripemd128", $key)`.
- Updates the modification time of the serialized or string cache file using `touch()`.

**Usage Context**:
- Used to update the access time of a cache key to prevent expiration.

**Example**:
```php
$success = cms_cache_touch("cache_key");
// $success = TRUE (if the key exists)
```
**Explanation**: Updates the access time of a cache key.

---

### `cms_cache_clean()`

**Purpose**: Removes expired cache files from a directory.

**Parameters**:

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$path`  | `string` | Path to the directory to clean.                                             |
| `$force` | `bool`   | If `TRUE`, removes all files regardless of their expiration time. Defaults to `FALSE`. |

**Return Values**: `bool` – `TRUE` if the directory was cleaned successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- Recursively traverses the directory.
- Removes files that have expired based on `CMS_CACHE_TTL` (30 days).
- Removes empty directories.

**Usage Context**:
- Used to clean up expired cache files to free up disk space.

**Example**:
```php
$success = cms_cache_clean(CMS_DATA_PATH . "#cache/");
```
**Explanation**: Removes expired cache files from the specified directory.

---

## Daemon

### `cms_daemon()`

**Purpose**: Adds a background task to the daemon queue.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$code`     | `string` | PHP code to execute in the background.                                      |
| `$id`       | `string` | Unique identifier for the task. If empty, uses a hash of the code. Defaults to `NULL`. |
| `$interval` | `int`    | Minimum interval in seconds before the task can be requeued. Defaults to `0`. |
| `$status`   | `string` | Status message to log when the task starts. Defaults to `""`.               |

**Return Values**: `bool` – `TRUE` if the task was added successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- **Path Handling**: Ensures the daemon directory exists using `mkpath()`.
- **Task File**: Creates a task file with the provided code and status message.
- **Duplicate Prevention**: Prevents duplicate tasks by checking the task ID.
- **Interval Handling**: Prevents requeueing if the interval has not elapsed.
- **Flag File**: Creates a flag file to indicate that tasks are available.

**Usage Context**:
- Used to queue background tasks for asynchronous execution.

**Example**:
```php
$code = "echo 'Background task executed at ' . date('Y-m-d H:i:s');";
$success = cms_daemon($code, "example_task", 3600, "Example task started");
// $success = TRUE (if the task was added successfully)
```
**Explanation**: Adds a background task to the daemon queue with a unique ID and minimum interval.

---

### `cms_daemon_status()`

**Purpose**: Sets or retrieves the status of the background worker.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | Status message to set. If empty, retrieves the current status. Defaults to `NULL`. |

**Return Values**:
- `string` – The current status if `$value` is empty.
- `bool` – `TRUE` if the status was set successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- **Path Handling**: Ensures the daemon directory exists using `mkpath()`.
- **Status File**: Reads or writes the status to `CMS_DATA_PATH . "#daemon/daemon.status"`.
- **Log Rotation**: Limits the status log to the last 25 entries.

**Usage Context**:
- Used to log the status of background tasks.
- Used to retrieve the current status of the background worker.

**Example**:
```php
// Set the status
$success = cms_daemon_status("Task started");

// Retrieve the status
$status = cms_daemon_status();
```
**Explanation**: Sets or retrieves the status of the background worker.

---

### `cms_daemon_exists()`

**Purpose**: Checks if a task is queued or running.

**Parameters**:

| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$id`     | `string` | Task ID to check.                                                           |
| `$running` | `bool`   | If `TRUE`, checks if the task is currently running. Defaults to `FALSE`.    |

**Return Values**: `bool` – `TRUE` if the task exists (and is running if `$running` is `TRUE`), `FALSE` otherwise.

**Inner Mechanisms**:
- **Task File**: Checks if the task file exists and is not empty.
- **Lock File**: If `$running` is `TRUE`, checks if the task lock file exists and is locked.

**Usage Context**:
- Used to check if a task is queued or currently running.

**Example**:
```php
// Check if a task is queued
$exists = cms_daemon_exists("example_task");

// Check if a task is running
$running = cms_daemon_exists("example_task", TRUE);
```
**Explanation**: Checks if a task is queued or running.

---

### `cms_daemon_running()`

**Purpose**: Checks if a task is currently running.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$id`  | `string` | Task ID to check. |

**Return Values**: `bool` – `TRUE` if the task is running, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_daemon_exists($id, TRUE)`.

**Usage Context**:
- Used to check if a task is currently being executed.

**Example**:
```php
$running = cms_daemon_running("example_task");
```
**Explanation**: Checks if a task is currently running.

---

### `cms_daemon_remove()`

**Purpose**: Removes a queued task.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$id`  | `string` | Task ID to remove. |

**Return Values**: `bool` – `TRUE` if the task was removed successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- Checks if the task is running using `cms_daemon_running()`.
- Removes the task file and lock file.

**Usage Context**:
- Used to remove a queued task that is no longer needed.

**Example**:
```php
$success = cms_daemon_remove("example_task");
```
**Explanation**: Removes a queued task from the daemon queue.

---

### `cms_daemon_run()`

**Purpose**: Starts a discrete asynchronous background worker.

**Parameters**: None.

**Return Values**: `bool` – `TRUE` if the background worker was started successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- **Flag Check**: Checks if there are tasks available using the flag file.
- **Lock Handling**: Uses an advisory lock to prevent multiple workers from running simultaneously.
- **HTTP Call**: Invokes the daemon worker script (`daemon.php`) via an HTTP request.

**Usage Context**:
- Used to start the background worker to process queued tasks.

**Example**:
```php
$success = cms_daemon_run();
```
**Explanation**: Starts the background worker to process queued tasks.

---

## Flag

### `cms_flag_set()`

**Purpose**: Sets, retrieves, or deletes a flag.

**Parameters**:

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$key` | `string` | Flag key.                                                                   |
| `$mode` | `int`    | Operation mode: `0` to set, `1` to get, `2` to delete. Defaults to `0`.     |

**Return Values**:
- `bool` – `TRUE` if the operation was successful, `FALSE` otherwise.

**Inner Mechanisms**:
- **Path Handling**: Ensures the flag directory exists using `mkpath()`.
- **File Handling**: Uses a file to represent the flag, with the filename derived from `hash("ripemd128", $key)`.
- **Locking**: Uses file locking to ensure atomic operations.

**Usage Context**:
- Used to set, retrieve, or delete flags for synchronization or state management.

**Example**:
```php
// Set a flag
$success = cms_flag_set("example_flag");

// Check if a flag is set
$isSet = cms_flag_set("example_flag", 1);

// Delete a flag
$success = cms_flag_set("example_flag", 2);
```
**Explanation**: Sets, retrieves, or deletes a flag for synchronization purposes.

---

### `cms_flag_get()`

**Purpose**: Checks if a flag is set.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$key` | `string` | Flag key.         |

**Return Values**: `bool` – `TRUE` if the flag is set, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_flag_set($key, 1)`.

**Usage Context**:
- Used to check if a flag is set.

**Example**:
```php
$isSet = cms_flag_get("example_flag");
```
**Explanation**: Checks if a flag is set.

---

### `cms_flag_del()`

**Purpose**: Deletes a flag.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$key` | `string` | Flag key.         |

**Return Values**: `bool` – `TRUE` if the flag was deleted successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- Calls `cms_flag_set($key, 2)`.

**Usage Context**:
- Used to delete a flag.

**Example**:
```php
$success = cms_flag_del("example_flag");
```
**Explanation**: Deletes a flag.

---

## Miscellaneous

### `cms_set_cookie()`

**Purpose**: Stores or deletes session cookies.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$array` | `array` | Associative array of cookie names and values. Use `NULL` or an empty string to delete a cookie. |

**Return Values**: `bool` – `TRUE` if all cookies were set successfully, `FALSE` otherwise.

**Inner Mechanisms**:
- **Options**: Sets cookie options including `httponly`, `path`, `samesite`, and `secure`.
- **Expiration**: Sets the expiration time to `1` (past) to delete cookies or `0` (session) to store them.
- **Update Check**: Skips setting cookies if the value has not changed.

**Usage Context**:
- Used to manage session cookies for user authentication and preferences.

**Example**:
```php
// Set cookies
$success = cms_set_cookie(["cms_user" => "admin", "cms_language" => "en"]);

// Delete cookies
$success = cms_set_cookie(["cms_user" => NULL, "cms_language" => ""]);
```
**Explanation**: Sets or deletes session cookies.

---

### `cms_salt()`

**Purpose**: Generates or retrieves a salt that changes every 60 minutes after last use.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | Additional value to include in the salt. If `NULL`, uses the client IP. Defaults to `NULL`. |

**Return Values**: `string` – The generated salt.

**Inner Mechanisms**:
- **Salt Storage**: Uses `cms_cache()` to store and retrieve salts.
- **IP-Based Salt**: If `$value` is `NULL`, generates a salt based on the client IP.
- **Custom Salt**: If `$value` is provided, generates a salt based on `CMS_USERID` and `$value`.
- **Expiration**: Changes the salt every 60 minutes if it has not been used.

**Usage Context**:
- Used to generate salts for hashing and security purposes.

**Example**:
```php
// Get an IP-based salt
$salt = cms_salt();

// Get a custom salt
$customSalt = cms_salt("password");
```
**Explanation**: Generates or retrieves a salt that changes periodically.

---

### `cms_email_agent()`

**Purpose**: Initializes the system email address.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **System Configuration**: Retrieves the email address from the `system` class.
- **Fallback**: Uses `mailagent@<domain>` if no email address is configured.
- **Constant**: Defines `CMS_EMAIL_AGENT` with the email address.

**Usage Context**:
- Called during initialization to set the system email address.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Initializes the system email address for use in outgoing emails.

---

### `cms_http_header()`

**Purpose**: Retrieves the value of an HTTP header.

**Parameters**:

| Name   | Type     | Description       |
|--------|----------|-------------------|
| `$name` | `string` | Header name.      |

**Return Values**: `string|null` – The header value or `NULL` if the header does not exist.

**Inner Mechanisms**:
- **Static Cache**: Uses a static array to cache header values.
- **Header Retrieval**: Uses `getallheaders()` if available, otherwise parses `$_SERVER`.

**Usage Context**:
- Used to retrieve HTTP header values.

**Example**:
```php
$userAgent = cms_http_header("user-agent");
```
**Explanation**: Retrieves the value of the `User-Agent` HTTP header.

---

### `cms_mcp_initialize()`

**Purpose**: Initializes the Model Context Protocol (MCP) for stateless API requests.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **Security Check**: Exits if MCP-related constants are already defined.
- **Header Retrieval**: Retrieves MCP headers (`Authorization`, `MCP-Method`, `MCP-Protocol-Version`).
- **Version Check**: Validates the MCP protocol version.
- **JSON Parsing**: Parses the JSON input and validates its structure.
- **Method Handling**: Handles specific MCP methods, including those that do not require authentication.
- **Authentication**: Validates the authentication token for methods that require it.
- **Error Handling**: Sends appropriate error responses for invalid requests or authentication failures.

**Usage Context**:
- Called during initialization to handle MCP API requests.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Initializes the MCP protocol for handling stateless API requests.

---

### `cms_trusted_proxies()`

**Purpose**: Configures trusted proxies to set the correct remote address, protocol, and port.

**Parameters**: None.

**Return Values**: None.

**Inner Mechanisms**:
- **Proxy Headers**: Retrieves the `X-Forwarded-For` or `X-Real-IP` header.
- **Trusted Proxies**: Reads the list of trusted proxies from `CMS_DATA_PATH . "#system/trusted_proxies.txt"`.
- **IP Validation**: Validates the client IP against the list of trusted proxies.
- **Protocol and Port**: Updates `$_SERVER` and environment variables for `HTTPS` and `SERVER_PORT` if the proxy headers are trusted.

**Usage Context**:
- Called during initialization to handle requests behind trusted proxies.

**Example**:
```php
// No direct usage; automatically called during initialization.
```
**Explanation**: Configures trusted proxies to ensure accurate client IP, protocol, and port information.

---

### `cms_ip_in_cidr()`

**Purpose**: Checks if an IP address is contained within a CIDR range.

**Parameters**:

| Name    | Type     | Description       |
|---------|----------|-------------------|
| `$ip`   | `string` | IP address.       |
| `$cidr` | `string` | CIDR range.       |

**Return Values**: `bool` – `TRUE` if the IP is within the CIDR range, `FALSE` otherwise.

**Inner Mechanisms**:
- **CIDR Parsing**: Splits the CIDR range into subnet and bits.
- **Binary Conversion**: Converts the IP and subnet to binary format using `inet_pton()`.
- **Mask Generation**: Generates a binary mask based on the CIDR bits.
- **Comparison**: Compares the masked IP and subnet.

**Usage Context**:
- Used by `cms_trusted_proxies()` to validate client IPs against trusted proxies.

**Example**:
```php
$isInRange = cms_ip_in_cidr("192.168.1.100", "192.168.1.0/24");
// $isInRange = TRUE
```
**Explanation**: Checks if an IP address is within a CIDR range.

---

### `cms_path_urlencode()`

**Purpose**: Encodes non-alphanumerical characters in a string according to RFC 1738, excluding path separators.

**Parameters**:

| Name     | Type     | Description       |
|----------|----------|-------------------|
| `$string` | `string` | String to encode. |

**Return Values**: `string` – The encoded string.

**Inner Mechanisms**:
- Uses `preg_replace_callback()` to encode characters that are not alphanumerical or in the set `$-_.+!*'(),`.

**Usage Context**:
- Used to encode file paths for URLs.

**Example**:
```php
$encoded = cms_path_urlencode("path/to/file with spaces.txt");
// $encoded = "path/to/file%20with%20spaces.txt"
```
**Explanation**: Encodes a file path for use in a URL, preserving path separators.


<!-- HASH:8cf3a509395994dff31628b52321500a -->
