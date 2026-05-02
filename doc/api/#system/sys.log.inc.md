# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.log.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Log System Module (`sys.log.inc`)

This file provides logging and reporting functionality for the NUOS platform. It consists of two main components:

1. **`log_report_option` class** – Defines display options for log reports.
2. **`log` class** – Handles access logging, user tracking, and bot detection.

---

## `log_report_option` Class

Defines customization options for log report columns.

### Properties

| Name              | Value/Default | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$name`           | `NULL`        | Custom column name.                                                         |
| `$type`           | `NULL`        | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`).                              |
| `$link`           | `NULL`        | URL pattern for linked values (e.g., `"user.php?id=%s"`).                   |
| `$link_encoding`  | `NULL`        | Callable function to encode link values (default: `cms\x`).                 |
| `$link_source`    | `NULL`        | Source field for link values (defaults to current field).                   |
| `$value_function` | `NULL`        | Callable function to transform displayed values.                            |

### Constructor

#### `__construct()`

Initializes a `log_report_option` instance.

**Parameters:**

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$name`           | `string`   | Custom column name.                                                         |
| `$type`           | `int`      | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`).                              |
| `$link`           | `string`   | URL pattern for linked values.                                              |
| `$link_encoding`  | `callable` | Function to encode link values.                                             |
| `$link_source`    | `string`   | Source field for link values.                                               |
| `$value_function` | `callable` | Function to transform displayed values.                                     |

---

## `log_report()` Function

Generates an HTML table from an SQL query with customizable column options.

### Purpose
Renders query results as an interactive table with support for:
- Text columns
- Beam (bar chart) columns
- Linked values
- Custom value transformations

### Parameters

| Name      | Type                          | Description                                                                 |
|-----------|-------------------------------|-----------------------------------------------------------------------------|
| `$sql`    | `string`                      | SQL query to execute.                                                       |
| `$options`| `array\|log_report_option\|FALSE` | Column display options. `FALSE` returns raw results.                       |

### Return Values

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if successful, `FALSE` on query error.                              |
| `array`   | Raw query results if `$options === FALSE`.                                 |
| `int`     | Number of rows displayed.                                                   |

### Inner Mechanisms
1. **Query Execution**: Measures execution time and handles errors.
2. **Result Processing**:
   - For non-SELECT queries: Displays affected rows.
   - For SELECT queries: Renders an HTML table with optional beams/links.
3. **Beam Calculation**: Computes relative widths for numeric columns.
4. **Link Handling**: Applies custom encoding to link values.

### Usage Context
- **Debugging**: Display query results with visual formatting.
- **Analytics**: Create dashboards with beam charts for numeric data.
- **Data Exploration**: Quickly inspect database contents.

**Example:**
```php
$options = [
    "username" => new log_report_option("User", CMS_LOG_REPORT_OPTION_TYPE_TEXT, "user.php?id=%s"),
    "visits"   => new log_report_option("Visits", CMS_LOG_REPORT_OPTION_TYPE_BEAM)
];
log_report("SELECT username, COUNT(*) as visits FROM log_access GROUP BY username", $options);
```

---

## `log` Class

Handles access logging, user tracking, and bot detection.

### Properties

| Name               | Value/Default       | Description                                                                 |
|--------------------|---------------------|-----------------------------------------------------------------------------|
| `$limit`           | `NULL`              | Log retention period in days (`-1` = disabled).                            |
| `$anonymize`       | `NULL`              | Whether to anonymize IP addresses.                                          |
| `$privacy`         | `NULL`              | Whether to enforce privacy (pseudonyms for users).                          |
| `$bot_limit`       | `5`                 | Max allowed requests in 10 seconds before bot detection.                    |
| `$bad_bot_limit`   | `10`                | Max allowed 404 errors in 10 seconds before bad bot classification.        |
| `$bad_bot_delay`   | `5`                 | Delay in seconds before re-checking bad bot status.                         |
| `$bad_bot_block`   | `FALSE`             | Whether to block bad bots.                                                  |
| `$bot_reset`       | `14`                | Days before resetting bot status to provisional.                            |
| `$bot_retention`   | `30`                | Days to retain bot access logs.                                             |
| `$operator`        | `FALSE`             | Whether current user has operator permissions.                              |
| `$enabled`         | `NULL`              | Whether logging is enabled.                                                 |

### Constructor

#### `__construct()`
Initializes the log system and verifies database tables.

---

### Methods

#### `access()`

Logs an access event with detailed metadata.

**Parameters:**

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$action`| `string` | Action description (e.g., `"login"`).                                       |
| `$info`  | `string` | Additional information.                                                     |

**Inner Mechanisms:**
1. **User Identification**: Uses `CMS_IPHASH` for anonymous users.
2. **Bot Detection**:
   - Checks access frequency against `$bot_limit`.
   - Classifies bad bots based on 404 error frequency.
3. **Metadata Collection**:
   - IP address (anonymized if `$anonymize` is `TRUE`).
   - Referrer, user agent, language, and region.
   - Browser detection via `browser()` method.
4. **Blocking**: Terminates requests from bad bots if `$bad_bot_block` is `TRUE`.

**Usage Context:**
- **Automatic Logging**: Called on every request to track access.
- **Manual Logging**: Used to log specific actions (e.g., `"user_edit"`).

---

#### `user()`

Updates or creates a user record.

**Parameters:**

| Name       | Type      | Description                                                                 |
|------------|-----------|-----------------------------------------------------------------------------|
| `$name`    | `string`  | User name (comma-separated for multiple).                                   |
| `$email`   | `string`  | Email address (comma-separated for multiple).                               |
| `$info`    | `string`  | Additional information.                                                     |
| `$bot`     | `int`     | Bot status (`CMS_LOG_STATUS_*`).                                            |
| `$userid`  | `string`  | User ID (defaults to `CMS_IPHASH`).                                         |
| `$append`  | `bool`    | Whether to append to existing values (`TRUE`) or replace (`FALSE`).        |

**Return Values:**

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| `bool` | `TRUE` if successful, `FALSE` otherwise.                                    |

**Inner Mechanisms:**
1. **Privacy Handling**: Generates pseudonyms if `$privacy` is `TRUE`.
2. **Bot Status Propagation**: Updates historical access logs if bot status changes.
3. **Data Merging**: Appends new values to existing ones if `$append` is `TRUE`.

**Usage Context:**
- **User Registration**: Create a new user record.
- **Profile Updates**: Modify existing user data.
- **Bot Classification**: Manually set bot status.

---

#### `set_user()`

Convenience wrapper for `user()` with `$append = FALSE`.

**Parameters:** Same as `user()` except `$append` is always `FALSE`.

---

#### `cleanup()`

Removes old log entries and resets bot statuses.

**Inner Mechanisms:**
1. **Log Retention**: Deletes access logs older than `$limit` days.
2. **Bot Cleanup**: Removes bot logs older than `$bot_retention` days.
3. **Orphaned Users**: Deletes users without recent access logs.
4. **Bot Status Reset**: Resets bot status to provisional after `$bot_reset` days.

**Usage Context:**
- **Scheduled Maintenance**: Run periodically to manage database size.

---

#### `browser()`

Detects browser from user agent string.

**Parameters:**

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$string`| `string` | User agent string.                                                          |
| `$count` | `int`    | Number of tokens to return (default: `1`).                                  |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Browser name (e.g., `"Firefox-Mobile"`).                                    |

**Inner Mechanisms:**
1. **Tokenization**: Extracts browser tokens from user agent string.
2. **Frequency Analysis**: Uses database to determine most common tokens.
3. **Mobile Detection**: Appends `"-Mobile"` if applicable.

**Usage Context:**
- **Analytics**: Track browser usage statistics.
- **Logging**: Store browser information in access logs.

---

## Constants

### Permission
| Name                          | Value       | Description                     |
|-------------------------------|-------------|---------------------------------|
| `CMS_LOG_PERMISSION_OPERATOR` | `"operator"`| Permission to manage logs.      |

### Report Option Types
| Name                                  | Value | Description                     |
|---------------------------------------|-------|---------------------------------|
| `CMS_LOG_REPORT_OPTION_TYPE_NONE`     | `0`   | Hide column.                    |
| `CMS_LOG_REPORT_OPTION_TYPE_TEXT`     | `1`   | Display as text.                |
| `CMS_LOG_REPORT_OPTION_TYPE_BEAM`     | `2`   | Display as beam chart.          |

### Log Statuses
| Name                                  | Value | Description                     |
|---------------------------------------|-------|---------------------------------|
| `CMS_LOG_STATUS_USER_FIXED`           | `-1`  | Confirmed human user.           |
| `CMS_LOG_STATUS_USER_PROVISIONAL`     | `0`   | Provisional human user.         |
| `CMS_LOG_STATUS_BOT_PROVISIONAL`      | `1`   | Provisional bot.                |
| `CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED`   | `2`   | Bot exceeding access limits.    |
| `CMS_LOG_STATUS_BOT_FIXED`            | `3`   | Confirmed bot.                  |
| `CMS_LOG_STATUS_BAD_BOT`              | `4`   | Malicious bot.                  |

### Database Tables
| Name                          | Value                     | Description                     |
|-------------------------------|---------------------------|---------------------------------|
| `CMS_DB_LOG_ACCESS`           | `"{prefix}log_access"`    | Access log table.               |
| `CMS_DB_LOG_USER`             | `"{prefix}log_user"`      | User information table.         |
| `CMS_DB_LOG_UA_LIST`          | `"{prefix}log_ua_list"`   | User agent hash list.           |
| `CMS_DB_LOG_UA_FREQ`          | `"{prefix}log_ua_freq"`   | User agent token frequencies.   |

### Database Fields
See code for field definitions (e.g., `CMS_DB_LOG_ACCESS_USERID`, `CMS_DB_LOG_USER_NAME`).


<!-- HASH:50b46b354c404c3c87ff72ee2eb220e0 -->
