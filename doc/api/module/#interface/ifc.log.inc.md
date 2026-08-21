# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.log.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.log.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Log Interface Module (`ifc.log.inc`)

This file implements the **Log Interface** for the PWNC Web Platform, providing a comprehensive frontend for viewing, filtering, and analyzing access logs. It includes visualization tools (SVG-based graphs), data tables, and configuration options for operators.

The module handles:
- **Log visualization** (time-based graphs for accesses, unique users, and mobile traffic)
- **Data filtering** (by date, user, IP, action, URL, referrer, browser, etc.)
- **Bot detection and management** (thresholds, blocking, and retention policies)
- **User data management** (editing user details and bot status)
- **Statistical reports** (top referrers, accessed content, actions, regions, languages, browsers, and users)

---

### **Constants & Configuration**

| Name | Value/Default | Description |
|------|---------------|-------------|
| **Permissions** | | |
| `CMS_L_ACCESS` | | Base permission to access the log interface. |
| `CMS_L_OPERATOR` | | Operator-level permission for configuration. |
| `CMS_LOG_PERMISSION_OPERATOR` | | Permission key for operator-specific actions. |
| **Database Fields** | | |
| `CMS_DB_LOG_ACCESS_*` | | Fields in the `log_access` table (e.g., `time`, `userid`, `ip`, `action`). |
| `CMS_DB_LOG_USER_*` | | Fields in the `log_user` table (e.g., `userid`, `name`, `bot`). |
| **Log Statuses** | | |
| `CMS_LOG_STATUS_*` | | Bot/user status flags (e.g., `USER_FIXED`, `BOT_PROVISIONAL`). |
| **UI Labels** | | |
| `CMS_L_IFC_LOG_*` | | Localized strings for UI elements (e.g., `CMS_L_IFC_LOG_001` = "Logging Limit"). |

---

### **Class: `log`**
The `log` class manages log-related operations, including configuration, data retrieval, and user management.

#### **Properties**
| Name | Type | Description |
|------|------|-------------|
| `enabled` | `bool` | Whether logging is enabled. |
| `operator` | `bool` | Whether the current user has operator permissions. |
| `limit` | `int` | Retention limit for logs (in days). |
| `anonymize` | `bool` | Whether to anonymize IPs. |
| `privacy` | `bool` | Whether to disable user data logging. |
| `bot_limit` | `int` | Threshold for bot detection (requests per interval). |
| `bad_bot_limit` | `int` | Threshold for bad bot detection. |
| `bad_bot_delay` | `int` | Delay (in seconds) for bad bot countermeasures. |
| `bad_bot_block` | `bool` | Whether to block bad bots. |
| `bot_reset` | `int` | Interval (in days) to reset bot counters. |
| `bot_retention` | `int` | Retention limit for bot logs (in days). |

---

#### **Method: `set_user`**
Updates user data in the `log_user` table.

##### **Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$userid` | `string` | User ID. |
| `$name` | `string` | User name. |
| `$email` | `string` | User email. |
| `$info` | `string` | Additional user info. |
| `$bot` | `int` | Bot status (see `CMS_LOG_STATUS_*` constants). |

##### **Return Value**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

##### **Usage Example**
```php
$log = new log();
$success = $log->set_user(
    "12345",          // User ID
    "John Doe",       // Name
    "john@example.com", // Email
    "Admin user",     // Info
    CMS_LOG_STATUS_USER_FIXED // Bot status
);
if ($success) {
    echo "User updated successfully.";
}
```

---

### **Function: `log_report`**
Generates an HTML report from a SQL query, formatting columns based on provided options.

##### **Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | SQL query to execute. |
| `$columns` | `array` | Associative array of column options (key = column name, value = `log_report_option`). |

##### **Return Value**
| Type | Description |
|------|-------------|
| `int` | Number of rows returned. |

##### **`log_report_option` Class**
Defines how a column is displayed in the report.

| Property | Type | Description |
|----------|------|-------------|
| `label` | `string` | Column header label. |
| `type` | `int` | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`). |
| `link` | `string` | JavaScript link template (e.g., `"javascript:f(%d, '%s')"`) |
| `encoder` | `string` | Function to encode values (e.g., `q`, `x`). |
| `data_key` | `string` | Key for additional data (e.g., user ID for color coding). |
| `formatter` | `string` | Function to format values (e.g., `yesno`, `strtocolor`). |

##### **Usage Example**
```php
$query = "SELECT userid, COUNT(*) AS count FROM log_access GROUP BY userid";
$columns = [
    "userid" => new log_report_option(
        "User ID",
        CMS_LOG_REPORT_OPTION_TYPE_TEXT,
        "javascript:f(0, '%s')",
        "q",
        NULL,
        "strtocolor"
    ),
    "count" => new log_report_option(
        "Access Count",
        CMS_LOG_REPORT_OPTION_TYPE_BEAM
    )
];
$rows = log_report($query, $columns);
```

---

### **Interface Workflow**
The interface processes messages (`CMS_IFC_MESSAGE`) to handle different actions:

| Message | Description |
|---------|-------------|
| `config` | Displays configuration form for operators. |
| `_config` | Saves configuration changes. |
| `save` | Saves user data (name, email, info, bot status). |
| `load_raw` | Loads raw log data (paginated). |
| `load_origin` | Loads top referrer domains. |
| `load_content` | Loads top accessed paths. |
| `load_activity` | Loads top actions. |
| `load_region` | Loads top regions. |
| `load_language` | Loads top languages. |
| `load_technology` | Loads top browsers. |
| `load_identity` | Loads top users. |
| `load_time` | Loads hourly activity. |

---

### **Graph Visualization**
The SVG-based graph displays:
- **Accesses** (blue line/path)
- **Unique Users** (red line/path)
- **Mobile Traffic** (green line/path)

#### **Key JavaScript Functions**
| Function | Description |
|----------|-------------|
| `log_vline(date)` | Determines if a vertical line should be drawn (e.g., at month/year boundaries). |
| `log_label(value)` | Formats labels for the x-axis (e.g., "2023-W01" for weeks). |
| `log_index(date)` | Generates a unique index for a date (e.g., `202301` for January 2023). |
| `log_timestamp(value)` | Converts an index back to a timestamp. |

##### **Usage Example**
```javascript
// Navigate to the previous time period
document.getElementById("log-prev").addEventListener("click", () => {
    let date = new Date(ifc_get("ifc_param1"));
    date.setUTCMonth(date.getUTCMonth() - 1); // Go back 1 month
    ifc_set("ifc_param1", date.toISOString().slice(0, 10));
    ifc_post();
});
```

---

### **Filtering & Data Retrieval**
The interface supports filtering logs by:
- **Date range** (e.g., last 7 days, last month)
- **Field** (e.g., IP, user, action, URL)
- **Operation** (e.g., `LIKE`, `NOT LIKE`, `=`)
- **Bot status** (off, only bots, all)

##### **Usage Example**
```javascript
// Filter logs by IP containing "192.168"
ifc_set("ifc_param3", 2); // Field: IP
ifc_set("ifc_param4", 0); // Operation: LIKE
ifc_set("ifc_param5", "192.168"); // Value
ifc_post();
```

---

### **User Data Management**
Operators can edit user details and bot statuses.

##### **Usage Example**
```php
// Display user edit form
$ifc->set(CMS_L_IFC_LOG_063, "text 30 255", $user_name);
$ifc->set(CMS_L_COMMAND_SAVE, "button b", "save");
$ifc->set(CMS_L_IFC_LOG_072, "label");
$ifc->set([
    CMS_L_IFC_LOG_073 => CMS_LOG_STATUS_USER_FIXED,
    CMS_L_IFC_LOG_074 => CMS_LOG_STATUS_USER_PROVISIONAL,
    CMS_L_IFC_LOG_075 => CMS_LOG_STATUS_BOT_PROVISIONAL
], "select", $current_status);
```


<!-- HASH:ae2b4a82d124170500767ec1d1946e00 -->
