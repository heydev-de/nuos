# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.log.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Log Interface Module (`ifc.log.inc`)

This file implements the **Log Interface** for the NUOS web platform, providing a comprehensive analytics dashboard for monitoring and analyzing website access logs. It enables operators to visualize traffic patterns, filter data, and manage user-specific logging information.

---

## Overview

The log interface module serves as the primary tool for:

1. **Traffic Analysis**: Visualizing access patterns over time with customizable time intervals.
2. **Data Filtering**: Applying filters based on user attributes, actions, paths, and other log fields.
3. **Bot Management**: Configuring bot detection thresholds and countermeasures.
4. **User Data Management**: Viewing and editing user-specific logging information.
5. **Reporting**: Generating detailed reports on referrers, content access, technology usage, and more.

The interface is built using the NUOS IFC (Interface Controller) system, which dynamically generates UI elements based on database queries and user permissions.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_ACCESS` | System-defined | Permission level required to access the log interface. |
| `CMS_L_OPERATOR` | System-defined | Permission level required to configure log settings. |
| `CMS_LOG_PERMISSION_OPERATOR` | System-defined | Permission key for operator-level access. |
| `CMS_IFC_MESSAGE` | Dynamic | Determines the current action or sub-display to render. |
| `CMS_IFC_PAGE` | Dynamic | Current interface page identifier. |
| `CMS_DB_LOG_ACCESS_*` | Database column names | Fields in the access log table. |
| `CMS_DB_LOG_USER_*` | Database column names | Fields in the user log table. |
| `CMS_LOG_REPORT_OPTION_TYPE_*` | `TEXT`, `BEAM`, `NONE` | Types of report cell formatting. |
| `CMS_LOG_STATUS_*` | Integer constants | Status codes for bot/user classification. |

---

## Core Logic Flow

1. **Permission Check**: Verifies if the current user has access to the log interface.
2. **Log Instance Initialization**: Creates a `log` object to interact with logging functionality.
3. **Message Handling**: Routes the interface based on `CMS_IFC_MESSAGE` (e.g., `config`, `load_raw`, `save`).
4. **Main Display**: Renders the primary dashboard with a time-series graph and filter controls.
5. **Tabbed Reports**: Provides detailed reports on origins, content, activity, regions, languages, technologies, and user identities.

---

## Key Functions and Methods

---

### `ifc_permission()`

**Purpose**:
Checks if the current user has the required permissions to access the log interface.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$permissions` | `array` | Associative array mapping permission keys to required access levels. |

**Return Values**:
- None. Redirects or terminates execution if permissions are insufficient.

**Inner Mechanisms**:
- Uses the NUOS permission system to validate access.
- Redirects to an inactive page if the log module is disabled.

**Usage Context**:
- Called at the start of the file to enforce access control.

---

### `log_report()`

**Purpose**:
Generates an HTML report table from a SQL query, applying custom formatting to each column.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | SQL query to execute. |
| `$columns` | `array` | Associative array mapping column names to `log_report_option` objects or strings. |

**Return Values**:
- `int`: Number of rows returned by the query.

**Inner Mechanisms**:
- Executes the query and iterates over results.
- For each column, applies the specified formatting (e.g., text, beam, or hidden).
- Supports dynamic callbacks for cell content and click events.
- Escapes all output using `x()` for XML safety.

**Usage Context**:
- Used in all `load_*` message handlers to render tabular reports.

---

### `log_report_option` Class

**Purpose**:
Defines formatting and behavior for a report column.

**Properties**:

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Column header label. |
| `$type` | `int` | Formatting type (`TEXT`, `BEAM`, `NONE`). |
| `$onclick` | `string` | JavaScript click handler (e.g., `javascript:f(0,2,'%s')`). |
| `$encoder` | `string` | Encoder function for values (e.g., `cms\q`). |
| `$id_column` | `string` | Column name to use as ID for click events. |
| `$transform` | `string` | Transform function for values (e.g., `cms\yesno`). |

**Usage Context**:
- Instantiated for each column in `log_report()` to control rendering.

---

## Message Handlers

The interface responds to different `CMS_IFC_MESSAGE` values, each triggering a specific action or display.

---

### `config`

**Purpose**:
Renders the log configuration form for operators.

**Parameters**:
- None (uses `$ifc_param*` for form values).

**Return Values**:
- None. Outputs an IFC form.

**Inner Mechanisms**:
- Creates an `ifc` object with a `_config` subpage.
- Populates form fields for:
  - Logging retention limit.
  - IP anonymization.
  - User data privacy.
  - Bot detection thresholds and countermeasures.
- Uses dynamic arrays for select options (e.g., days, thresholds).

**Usage Context**:
- Accessible only to users with `CMS_L_OPERATOR` permission.

---

### `_config`

**Purpose**:
Saves log configuration settings submitted via the `config` form.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `int` | Logging retention limit in days. |
| `$ifc_param2` | `bool` | IP anonymization flag. |
| `$ifc_param3` | `bool` | User data privacy flag. |
| `$ifc_param4` | `int` | Bot detection threshold. |
| `$ifc_param5` | `int` | Bad bot detection threshold. |
| `$ifc_param6` | `int` | Bad bot countermeasure delay. |
| `$ifc_param7` | `bool` | Bad bot blocking flag. |
| `$ifc_param8` | `int` | Bot reset interval. |
| `$ifc_param9` | `int` | Bot logging retention limit. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
- Uses the `system` class to save settings.
- Updates the log instance to reflect new settings.
- Redirects to an inactive page if logging is disabled.

**Usage Context**:
- Triggered by form submission from the `config` display.

---

### `save`

**Purpose**:
Saves user-specific logging data (name, email, info, bot status).

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | User ID. |
| `$ifc_param7` | `string` | User name. |
| `$ifc_param8` | `string` | User email. |
| `$ifc_param9` | `string` | User info. |
| `$ifc_param10` | `int` | Bot status. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
- Calls `$log->set_user()` to update the user record.

**Usage Context**:
- Triggered by form submission in the user data tab.

---

### `load_raw`

**Purpose**:
Loads raw access log data for display in a paginated table.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$offset` | `int` | Pagination offset. |

**Return Values**:
- None. Outputs HTML table rows.

**Inner Mechanisms**:
- Constructs a complex SQL query joining `CMS_DB_LOG_ACCESS` and `CMS_DB_LOG_USER`.
- Uses `log_report()` to render the table with clickable cells for filtering.
- Includes a "More" button for pagination.

**Usage Context**:
- Called via AJAX when the raw data tab is active or when paginating.

---

### `load_origin`, `load_content`, `load_activity`, `load_region`, `load_language`, `load_technology`, `load_identity`, `load_time`

**Purpose**:
Loads aggregated log data for specific dimensions (e.g., referrers, paths, actions).

**Parameters**:
- None (uses global filter state).

**Return Values**:
- None. Outputs HTML table rows.

**Inner Mechanisms**:
- Each function constructs a `GROUP BY` query for its dimension.
- Uses `log_report()` to render the top 100 results.
- Applies beam formatting to count columns.

**Usage Context**:
- Called via AJAX when their respective tabs are activated.

---

## Time Series Graph

**Purpose**:
Renders an SVG-based time series graph of access data.

**Key Features**:
- **Three Metrics**: Accesses (blue), unique users (red), mobile accesses (green).
- **Dynamic Resolution**: Supports hourly, daily, weekly, monthly, quarterly, and yearly views.
- **Interactive Controls**: Previous, next, and zoom-out buttons.
- **Clickable Data Points**: Allows drilling down into specific time periods.

**Inner Mechanisms**:
1. **Data Aggregation**: Groups log data by time index and user ID.
2. **SVG Generation**: Dynamically creates SVG elements for lines, labels, and data points.
3. **JavaScript Integration**: Uses inline scripts to:
   - Calculate time indices based on resolution.
   - Render vertical lines and labels.
   - Handle click events for navigation and filtering.
4. **Responsive Design**: Adjusts view box based on label width.

**Usage Context**:
- Rendered in the main tab of the log interface.

---

## JavaScript Functions

| Name | Purpose |
|------|---------|
| `d(value)` | Sets the date filter and reloads the interface. |
| `f(field, operator, value)` | Applies a field filter and reloads the interface. |
| `l(value)` | Navigates to a URL (used for referrers and paths). |
| `b(value)` | Filters by bot status and reloads the interface. |
| `log_load_raw(offset, append)` | Loads raw log data via AJAX. |
| `log_load_stats(object, target, message)` | Loads tabular report data when the tab is activated. |
| `log_vline()`, `log_label()`, `log_index()`, `log_timestamp()` | Resolution-specific helper functions for the graph. |

---

## User Data Tab

**Purpose**:
Allows operators to view and edit user-specific logging information.

**Fields**:
- **Name**: User display name.
- **Email**: User email address.
- **Info**: Additional notes.
- **Bot Status**: Classification (e.g., user, bot, bad bot).

**Usage Context**:
- Displayed when a user ID is selected in the main interface.

---

## Best Practices and Usage Scenarios

1. **Traffic Monitoring**:
   - Use the main graph to identify traffic spikes or drops.
   - Adjust the time interval to zoom in/out of specific periods.

2. **Bot Management**:
   - Configure bot detection thresholds in the config tab.
   - Use the bot filter to analyze bot vs. human traffic.

3. **User Analysis**:
   - Filter by user ID or name to track specific users.
   - Use the user data tab to update user information or bot status.

4. **Content Performance**:
   - Use the content tab to identify the most accessed paths.
   - Filter by action to analyze specific interactions (e.g., downloads).

5. **Geographic Analysis**:
   - Use the region tab to identify traffic sources by country or region.

6. **Technology Trends**:
   - Use the technology tab to monitor browser and device usage.

7. **Privacy Compliance**:
   - Enable IP anonymization and user data privacy in the config tab to comply with regulations like GDPR.

---

## Error Handling

- **Permission Errors**: Redirects to an inactive page if the user lacks permissions.
- **Database Errors**: Silently handles MySQL errors (e.g., timezone setting) with fallbacks.
- **AJAX Errors**: Relies on the `asr_send()` function for error handling in client-side requests.

---

## Performance Considerations

- **Pagination**: Raw data is loaded in chunks of 100 records to avoid memory issues.
- **Caching**: Aggregated reports are generated on-demand and not cached.
- **Time Zone Handling**: Adjusts for server-client time zone differences.
- **Query Optimization**: Uses `ORDER BY NULL` with `GROUP BY` to disable file sorting for better performance.


<!-- HASH:156e880ec9c2cc7ac1fd45db315dadeb -->
