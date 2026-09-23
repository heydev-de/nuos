# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.log.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.log.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Log Interface Module

## Overview

The `ifc.log.inc` file is the interface controller for the **Log** module in the PWNC Web Platform. It provides a comprehensive access logging and analytics dashboard, allowing operators to view, filter, and analyze website traffic data. The module supports:

- **Access log visualization** with an interactive SVG graph showing access counts, unique users, and mobile usage over time
- **Detailed raw log data** with filtering capabilities
- **Statistical breakdowns** by referrer domain, content path, action type, region, language, browser technology, user identity, and hourly activity
- **Bot detection and handling** with configurable thresholds and countermeasures
- **User data management** for individual tracked users
- **Configuration settings** for log retention, IP anonymization, and privacy controls

The interface is permission-gated, requiring operator-level access for configuration and user data management.

## Constants and Configuration

| Constant | Description |
|----------|-------------|
| `CMS_LOG_PERMISSION_OPERATOR` | Permission level required for operator access |
| `CMS_L_ACCESS` | Base access permission level |
| `CMS_L_OPERATOR` | Operator permission level |
| `CMS_L_IFC_LOG_*` | Various language strings for UI labels and messages |
| `CMS_DB_LOG_ACCESS_*` | Database column constants for access log table |
| `CMS_DB_LOG_USER_*` | Database column constants for user log table |
| `CMS_LOG_STATUS_*` | Status constants for user/bot classification |
| `CMS_LOG_REPORT_OPTION_TYPE_*` | Report option type constants |

## Main Components

### Log Class

The `log` class manages logging functionality and configuration:

- **`$log->enabled`**: Boolean indicating if logging is active
- **`$log->operator`**: Boolean indicating if current user has operator privileges
- **`$log->limit`**: Log retention period in days
- **`$log->anonymize`**: Boolean for IP anonymization
- **`$log->privacy`**: Boolean for disabling user data logging
- **`$log->bot_limit`**: Bot detection threshold
- **`$log->bad_bot_limit`**: Bad bot detection threshold
- **`$log->bad_bot_delay`**: Delay for bad bot countermeasures
- **`$log->bad_bot_block`**: Boolean for blocking bad bots
- **`$log->bot_reset`**: Bot reset interval
- **`$log->bot_retention`**: Bot log retention period

### Interface Flow

The module handles several message types via `CMS_IFC_MESSAGE`:

1. **`config`**: Displays configuration form for operators
2. **`_config`**: Processes configuration form submission
3. **`save`**: Saves user data modifications
4. **`load_raw`**: Loads raw log data with pagination
5. **`load_origin`**: Loads referrer domain statistics
6. **`load_content`**: Loads content path statistics
7. **`load_activity`**: Loads action type statistics
8. **`load_region`**: Loads region statistics
9. **`load_language`**: Loads language statistics
10. **`load_technology`**: Loads browser technology statistics
11. **`load_identity`**: Loads top user statistics
12. **`load_time`**: Loads hourly activity statistics

## Time Resolution and Intervals

### Resolution Mapping

| Key | SQL Expression | Description |
|-----|----------------|-------------|
| `h` | `YEAR(time) * 10000 + (DAYOFYEAR(time) - 1) * 24 + HOUR(time)` | Hourly resolution |
| `d` | `YEAR(time) * 1000 + DAYOFYEAR(time)` | Daily resolution |
| `w` | `YEARWEEK(time, 3)` | Weekly resolution |
| `m` | `YEAR(time) * 100 + MONTH(time)` | Monthly resolution |
| `q` | `YEAR(time) * 10 + QUARTER(time)` | Quarterly resolution |
| `y` | `YEAR(time)` | Yearly resolution |

### Time Intervals

| Label | Interval | Resolution |
|-------|----------|------------|
| Now | `now` | `h` |
| Last week | `-1 week +1 day` | `d` |
| Last month | `-1 month +1 day` | `d` |
| Last 3 months | `-3 month +1 week` | `w` |
| Last 6 months | `-6 month +1 week` | `w` |
| Last year | `-1 year +1 month` | `m` |
| Last 2 years | `-2 year +1 month` | `m` |
| Last 3 years | `-3 year +3 month` | `q` |
| Last 4 years | `-4 year +3 month` | `q` |
| Last 5 years | `-5 year +3 month` | `q` |
| Last 10 years | `-10 year +1 year` | `y` |

## Filter Configuration

### Filter Fields

| Label | Database Column |
|-------|-----------------|
| User ID | `CMS_DB_LOG_ACCESS_USERID` |
| User | `CMS_DB_LOG_ACCESS_USER` |
| IP | `CMS_DB_LOG_ACCESS_IP` |
| Action | `CMS_DB_LOG_ACCESS_ACTION` |
| URL | `CMS_DB_LOG_ACCESS_URL` |
| Path | `CMS_DB_LOG_ACCESS_PATH` |
| Referrer | `CMS_DB_LOG_ACCESS_REFERRER` |
| Domain | `CMS_DB_LOG_ACCESS_DOMAIN` |
| Agent | `CMS_DB_LOG_ACCESS_AGENT` |
| Browser | `CMS_DB_LOG_ACCESS_BROWSER` |
| Mobile | `CMS_DB_LOG_ACCESS_MOBILE` |
| Language | `CMS_DB_LOG_ACCESS_LANGUAGE` |
| Region | `CMS_DB_LOG_ACCESS_REGION` |
| Error | `CMS_DB_LOG_ACCESS_ERROR` |

### Filter Operations

| Label | SQL Pattern |
|-------|-------------|
| Contains | ` LIKE '%s'` |
| Not contains | ` NOT LIKE '%s'` |
| Equals | `='%s'` |

## JavaScript Functions

### Date Utilities

#### `Date.prototype.getISOYear()`
Returns the ISO year for a date, accounting for week-based year boundaries.

#### `Date.prototype.getISOWeek()`
Returns the ISO week number (1-53) for a date.

#### `Date.prototype.getTimeFromISOWeek(week)`
Converts an ISO week number to a timestamp.

#### `Date.prototype.getWeekdayAbbr()`
Returns the abbreviated weekday name based on the current locale.

### Graph Functions

#### `log_vline(date)`
Determines whether to display a vertical reference line for the given date based on the current resolution.

#### `log_label(value)`
Generates a human-readable label for a time index value.

#### `log_index(date)`
Calculates the time index for a given date based on the current resolution.

#### `log_timestamp(value)`
Converts a time index back to a timestamp.

### Interaction Functions

#### `d(value)`
Sets the date parameter and submits the form.

#### `f(field, operator, value)`
Sets filter parameters and submits the form.

#### `l(value)`
Navigates to a URL, adding CSRF token for same-origin links.

#### `b(value)`
Toggles bot filtering (0=off, 1=only bots, 2=all).

#### `log_load_raw(offset, append)`
Loads raw log data with pagination support.

#### `log_load_stats(object, target, message)`
Loads statistical data into a target element when a tab is activated.

## Usage Examples

### Viewing Access Logs

```php
// The main display is automatically rendered when accessing the log interface
// No direct function call needed - handled by the interface framework
```

### Filtering by Date Range

```javascript
// Set a specific date and resolution
ifc_set("ifc_param1", "2024-01-15");
ifc_set("ifc_param2", 3); // Monthly resolution
ifc_post();
```

### Filtering by Field

```javascript
// Filter by user ID containing "admin"
f(0, 0, "admin"); // field=userid, operation=contains, value=admin
```

### Loading Raw Data

```javascript
// Load first 100 raw log entries
log_load_raw(0, false);

// Load next 100 entries
log_load_raw(100, true);
```

### Configuring Bot Detection

```php
// In the config form, operators can set:
// - Bot detection threshold (5-15 requests)
// - Bad bot threshold (10-20 requests)
// - Bad bot delay (1-10 seconds)
// - Bad bot blocking (enabled/disabled)
// - Bot reset interval
// - Bot log retention
```

## Security Considerations

1. **Permission Checks**: All configuration and user data operations require operator privileges
2. **SQL Injection Prevention**: Uses `sqlesc()` for manual SQL escaping
3. **CSRF Protection**: URL generation includes CSRF tokens via `cms_url()`
4. **Timezone Handling**: Properly sets MySQL timezone and converts to UTC for consistent data processing
5. **Input Validation**: Parameters are validated and sanitized before use in queries

## Performance Features

1. **Pagination**: Raw log data loads in batches of 100 entries
2. **Lazy Loading**: Statistical data loads only when tabs are activated
3. **Caching**: Uses `cms_cache()` for efficient data retrieval
4. **Timeout Management**: Sets extended timeout (600 seconds) for large data operations
5. **User Abort Handling**: Allows script continuation even if user disconnects

## SVG Graph Features

The interactive SVG graph provides:

- **Three data series**: Access counts (blue), unique users (red), mobile usage (green)
- **Zoom controls**: Previous/next buttons for time navigation
- **Zoom out**: Button to increase time resolution
- **Click interactions**: Clicking data points sets date filters
- **Vertical reference lines**: Mark period boundaries (weeks, months, etc.)
- **Dynamic labels**: Time labels adjust based on resolution
- **Responsive sizing**: ViewBox adjusts based on label width

## Tab Structure

The interface uses a tabbed layout with the following sections:

1. **Graph**: Main visualization with statistics
2. **Raw Data**: Detailed log entries with filtering
3. **Origin**: Referrer domain statistics
4. **Content**: Content path statistics
5. **Activity**: Action type statistics
6. **Region**: Geographic region statistics
7. **Language**: Language statistics
8. **Technology**: Browser technology statistics
9. **Identity**: Top user statistics
10. **Time**: Hourly activity patterns
11. **User Data**: Individual user information (when selected)


<!-- HASH:ae2b4a82d124170500767ec1d1946e00 -->
