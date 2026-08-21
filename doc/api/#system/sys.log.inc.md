# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.log.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.log.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Log System Module (`sys.log.inc`)

This module provides logging and reporting functionality for the PWNC Web Platform. It consists of two main components:

1. **`log_report_option` class** – Defines display options for log reports, including text formatting, links, and visual beam representations.
2. **`log` class** – Handles access logging, user tracking, bot detection, and log cleanup.

---

## `log_report_option` Class

Defines customization options for log report columns, enabling enhanced data visualization (e.g., links, beams).

### Properties

| Name              | Value/Default | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$name`           | `NULL`        | Custom column name. Overrides the default field name.                      |
| `$type`           | `NULL`        | Display type: `CMS_LOG_REPORT_OPTION_TYPE_NONE`, `TEXT`, or `BEAM`.        |
| `$link`           | `NULL`        | URL pattern for linked values (e.g., `"user.php?id=%s"`).                  |
| `$link_encoding`  | `NULL`        | Callable function to encode link values (default: `x()` for XML escaping). |
| `$link_source`    | `NULL`        | Field name providing the value for the link (defaults to the column name). |
| `$value_function` | `NULL`        | Callable function to transform the displayed value.                        |

### Constructor

```php
function __construct(
    $name = NULL,
    $type = NULL,
    $link = NULL,
    $link_encoding = NULL,
    $link_source = NULL,
    $value_function = NULL
)
```

#### Parameters

| Parameter         | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$name`           | `string`   | Custom column name.                                                         |
| `$type`           | `int`      | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`).                             |
| `$link`           | `string`   | URL pattern for linked values.                                              |
| `$link_encoding`  | `callable` | Function to encode link values (default: `x()`).                           |
| `$link_source`    | `string`   | Field name for link values (defaults to the column name).                  |
| `$value_function` | `callable` | Function to transform the displayed value.                                  |

#### Usage Example

```php
$option = new log_report_option(
    "User ID",                          // Custom column name
    CMS_LOG_REPORT_OPTION_TYPE_TEXT,    // Text display
    "user.php?id=%s",                   // Link pattern
    NULL,                               // Default encoding (x())
    "userid"                            // Link source field
);
```

---

## `log_report` Function

Generates an HTML table from an SQL query, applying custom display options for each column.

### Purpose
Transforms query results into a formatted table with support for:
- Text columns (with optional links).
- Beam columns (visual bar charts for numeric values).
- Custom value transformations.

### Parameters

| Parameter | Type                          | Description                                                                 |
|-----------|-------------------------------|-----------------------------------------------------------------------------|
| `$sql`    | `string`                      | SQL query to execute.                                                       |
| `$options`| `array\|log_report_option`    | Associative array of column names to `log_report_option` objects or strings.|

### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `FALSE` on query failure.                                                   |
| `array`   | Raw result rows if `$options === FALSE`.                                    |
| `int`     | Number of rows displayed (success).                                         |

### Inner Mechanisms
1. **Query Execution**: Measures execution time and checks for errors.
2. **Column Processing**: Maps field names to display options, resolving duplicates (e.g., `"name (1)"`).
3. **Beam Calculation**: For numeric columns, computes max/sum values and scaling factors.
4. **HTML Generation**: Renders a table with headers, data rows, and optional beam footers.

### Usage Example

```php
$sql = "SELECT userid, name, access_count FROM log_users";
$options = [
    "userid" => new log_report_option(
        "ID",
        CMS_LOG_REPORT_OPTION_TYPE_TEXT,
        "user.php?id=%s"
    ),
    "access_count" => new log_report_option(
        "Accesses",
        CMS_LOG_REPORT_OPTION_TYPE_BEAM
    )
];
log_report($sql, $options);
```

---

## `log` Class

Manages access logging, user tracking, bot detection, and log maintenance.

### Properties

| Name               | Value/Default       | Description                                                                 |
|--------------------|---------------------|-----------------------------------------------------------------------------|
| `$limit`           | `NULL`              | Max log retention in days (`-1` to disable).                               |
| `$anonymize`       | `NULL`              | Anonymizes IP addresses if `TRUE`.                                         |
| `$privacy`         | `NULL`              | Enables privacy mode (pseudonyms, no email storage).                       |
| `$bot_limit`       | `5`                 | Max allowed requests in 10 seconds before bot flagging.                    |
| `$bad_bot_limit`   | `10`                | Max 404 errors in 10 seconds before bad bot flagging.                      |
| `$bad_bot_delay`   | `5`                 | Seconds to wait before rechecking bad bot status.                          |
| `$bad_bot_block`   | `FALSE`             | Blocks bad bots if `TRUE`.                                                 |
| `$bot_reset`       | `14`                | Days after which bot status resets to provisional.                         |
| `$bot_retention`   | `30`                | Days to retain bot access logs.                                            |
| `$operator`        | `FALSE`             | `TRUE` if the current user has `CMS_LOG_PERMISSION_OPERATOR` permission.   |
| `$enabled`         | `NULL`              | `TRUE` if logging is enabled.                                              |

### Constructor

Initializes database tables and loads configuration from the `system` module.

---

### `access` Method

Logs an access event, detects bots, and updates user records.

#### Parameters

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$action` | `string` | Action description (e.g., `"login"`).                                       |
| `$info`   | `string` | Additional info (e.g., `"success"`).                                        |

#### Inner Mechanisms
1. **Bot Detection**: Checks access patterns against thresholds.
2. **User Tracking**: Updates or creates user records with pseudonyms/emails.
3. **Privacy Handling**: Anonymizes IPs and limits stored data if `$privacy` is enabled.
4. **Bad Bot Blocking**: Terminates requests from flagged bots if `$bad_bot_block` is `TRUE`.

#### Usage Example

```php
$log = new log();
$log->access("page_view", "homepage");
```

---

### `user` Method

Updates or creates a user record.

#### Parameters

| Parameter   | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$name`     | `string`  | User name (appended to existing names if `$append` is `TRUE`).              |
| `$email`    | `string`  | Email address (appended if `$append` is `TRUE`).                            |
| `$info`     | `string`  | Additional info.                                                            |
| `$bot`      | `int`     | Bot status (`CMS_LOG_STATUS_*`).                                            |
| `$userid`   | `string`  | User ID (defaults to `CMS_IPHASH`).                                         |
| `$append`   | `bool`    | If `TRUE`, appends new names/emails to existing data.                       |

#### Return Values
| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the user record was updated/created.                              |

#### Usage Example

```php
$log->user("John Doe", "john@example.com", "Premium User", CMS_LOG_STATUS_USER_FIXED);
```

---

### `set_user` Method

Alias for `user()` with `$append = FALSE`.

---

### `cleanup` Method

Maintains log tables by:
1. Deleting old bot/access records.
2. Removing orphaned users.
3. Resetting bot statuses after `$bot_reset` days.

#### Usage Example

```php
$log->cleanup(); // Run during low-traffic periods
```

---

### `browser` Method

Parses a user agent string and returns a browser identifier (e.g., `"Chrome-Mobile"`).

#### Parameters

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | User agent string.                                                          |
| `$count`  | `int`    | Number of tokens to include in the result (default: `1`).                   |

#### Return Values
| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Browser identifier (e.g., `"Firefox"`).                                     |

#### Inner Mechanisms
1. **Tokenization**: Extracts browser names from the user agent string.
2. **Frequency Analysis**: Uses a database table (`CMS_DB_LOG_UA_FREQ`) to rank tokens by popularity.
3. **Caching**: Stores unique token combinations in `CMS_DB_LOG_UA_LIST`.

#### Usage Example

```php
$browser = $log->browser($_SERVER["HTTP_USER_AGENT"], 2);
```

---

## Constants

### Permission
| Name                          | Value       | Description                          |
|-------------------------------|-------------|--------------------------------------|
| `CMS_LOG_PERMISSION_OPERATOR` | `"operator"`| Permission to manage logs.           |

### `log_report_option` Types
| Name                                  | Value | Description                          |
|---------------------------------------|-------|--------------------------------------|
| `CMS_LOG_REPORT_OPTION_TYPE_NONE`     | `0`   | Hide column.                         |
| `CMS_LOG_REPORT_OPTION_TYPE_TEXT`     | `1`   | Display as text (default).           |
| `CMS_LOG_REPORT_OPTION_TYPE_BEAM`     | `2`   | Display as a visual beam.            |

### Bot Statuses
| Name                                  | Value | Description                          |
|---------------------------------------|-------|--------------------------------------|
| `CMS_LOG_STATUS_USER_FIXED`           | `-1`  | Confirmed human user.                |
| `CMS_LOG_STATUS_USER_PROVISIONAL`     | `0`   | Provisional human user.              |
| `CMS_LOG_STATUS_BOT_PROVISIONAL`      | `1`   | Provisional bot.                     |
| `CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED`   | `2`   | Bot flagged for high access rate.    |
| `CMS_LOG_STATUS_BOT_FIXED`            | `3`   | Confirmed bot.                       |
| `CMS_LOG_STATUS_BAD_BOT`              | `4`   | Malicious bot (excessive 404 errors).|

### Database Tables
| Name                     | Value                     | Description                          |
|--------------------------|---------------------------|--------------------------------------|
| `CMS_DB_LOG_ACCESS`      | `"{prefix}log_access"`    | Access logs.                         |
| `CMS_DB_LOG_USER`        | `"{prefix}log_user"`      | User records.                        |
| `CMS_DB_LOG_UA_LIST`     | `"{prefix}log_ua_list"`   | Unique user agent hashes.            |
| `CMS_DB_LOG_UA_FREQ`     | `"{prefix}log_ua_freq"`   | Token frequencies for browser detection.|


<!-- HASH:8cf404d2e055e4724423db2b822daec7 -->
