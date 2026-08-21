# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.setup.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.setup.inc)

- **Version:** `26.8.8.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Interface: System Setup (`ifc.setup.inc`)

This file provides the **setup interface** for the PWNC Web Platform, handling initial configuration tasks such as:
- Administrator password setup
- Database connection configuration
- SMTP email settings
- Database restoration
- System updates and backups

The interface is triggered during the initial installation or when critical system configurations are missing/invalid. It guides the user through mandatory setup steps with validation and feedback.

---

## Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | Dynamic | Determines which setup action to execute. |
| `CMS_IFC_PAGE` | Dynamic | Current interface page identifier. |
| `CMS_SUPERUSER` | `"admin"` | Default superuser username. |
| `CMS_L_*` | Localized strings | Language constants for UI labels and messages. |
| `$ifc_param1` to `$ifc_param6` | Dynamic | Input parameters passed to the interface. |
| `$ifc_response` | Dynamic | Response message (success/error) to display. |

---

## Message Handling

The interface processes actions via a `switch` on `CMS_IFC_MESSAGE`. Each `case` corresponds to a specific setup task.

---

### `setup_password`

**Purpose:**
Sets or updates the administrator password after validating input.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New password. |
| `$ifc_param2` | `string` | Password confirmation. |

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success, or `CMS_MSG_ERROR` with a localized message on failure.

**Inner Mechanisms:**
1. Validates that the password is not empty.
2. Ensures password and confirmation match.
3. Hashes the password using `hash64()` and stores it in the `#system/permission` data store.
4. Sets a secure cookie (`cms_password`) for session persistence.

**Usage Example:**
```php
// Trigger via interface with password "secure123" and confirmation
$ifc_param1 = "secure123";
$ifc_param2 = "secure123";
CMS_IFC_MESSAGE = "setup_password";
include "module/#interface/ifc.setup.inc";
```
> **Note:** This is typically called via a form submission in the setup UI.

---

### `setup_database`

**Purpose:**
Configures the MySQL database connection parameters.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | MySQL host (e.g., `localhost`). |
| `$ifc_param2` | `string` | Database name. |
| `$ifc_param3` | `string` | MySQL username. |
| `$ifc_param4` | `string` | MySQL password. |

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success, or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Stores the parameters in the `system` data store under the `mysql` section.
2. Attempts to establish a connection to validate the credentials.

**Usage Example:**
```php
$ifc_param1 = "localhost";
$ifc_param2 = "pwnc_db";
$ifc_param3 = "db_user";
$ifc_param4 = "db_password";
CMS_IFC_MESSAGE = "setup_database";
include "module/#interface/ifc.setup.inc";
```

---

### `mysql_restore`

**Purpose:**
Restores the database from a backup if the `mysql.restore` flag is set.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | `"yes"` to confirm restoration, otherwise ignored. |

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success, or `CMS_MSG_ERROR` with a localized message on failure.

**Inner Mechanisms:**
1. Checks if `$ifc_param1` is `"yes"`.
2. Uses the `mysql` class to restore the database.
3. Clears the `mysql.restore` flag after restoration.

**Usage Example:**
```php
$ifc_param1 = "yes";
CMS_IFC_MESSAGE = "mysql_restore";
include "module/#interface/ifc.setup.inc";
```

---

### `setup_smtp`

**Purpose:**
Configures SMTP email settings and tests the connection.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Sender email address. |
| `$ifc_param2` | `string` | Reply-to email address. |
| `$ifc_param3` | `string` | Email method (`"mail"` or custom SMTP). |
| `$ifc_param4` | `string` | SMTP host. |
| `$ifc_param5` | `string` | SMTP username. |
| `$ifc_param6` | `string` | SMTP password. |

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success, or `CMS_MSG_ERROR` with an error message on failure.

**Inner Mechanisms:**
1. Validates input (e.g., SMTP host is required if not using `mail`).
2. Tests the SMTP connection by sending a test email.
3. Stores settings in the `system` data store under the `email` section.

**Usage Example:**
```php
$ifc_param1 = "noreply@pwnc.it";
$ifc_param2 = "support@pwnc.it";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.pwnc.it";
$ifc_param5 = "smtp_user";
$ifc_param6 = "smtp_password";
CMS_IFC_MESSAGE = "setup_smtp";
include "module/#interface/ifc.setup.inc";
```

---

### `do_not_show`

**Purpose:**
Marks the setup as complete to hide it after login.

**Parameters:**
None.

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.

**Inner Mechanisms:**
1. Sets `setup.done` to `TRUE` in the `system` data store.
2. Clears the `mysql.restore` flag.

**Usage Example:**
```php
CMS_IFC_MESSAGE = "do_not_show";
include "module/#interface/ifc.setup.inc";
```

---

### `update`

**Purpose:**
Triggers a system update in the background via a daemon.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$update_skip_backup` | `bool` | If `TRUE`, skips backup during update. |

**Return/Output:**
- Launches a daemon process to perform the update.

**Inner Mechanisms:**
1. Uses `cms_daemon()` to run the update asynchronously.
2. Cleans the cache after a successful update.

**Usage Example:**
```php
$update_skip_backup = TRUE;
CMS_IFC_MESSAGE = "update";
include "module/#interface/ifc.setup.inc";
```

---

### `update_status`

**Purpose:**
Fetches the current update status and log for display.

**Parameters:**
None.

**Return/Output:**
- Outputs a JSON array `[status, log]` where:
  - `status`: Update status code (e.g., `CMS_UPDATE_STATUS_NONE`).
  - `log`: Update log or localized message.

**Inner Mechanisms:**
1. Loads the `update` library.
2. Retrieves the status and log from the `update` class.
3. Outputs the result as JSON.

**Usage Example:**
```php
CMS_IFC_MESSAGE = "update_status";
include "module/#interface/ifc.setup.inc";
// Output: [0, "No updates available"]
```

---

### `backup`

**Purpose:**
Triggers a system backup in the background via a daemon.

**Parameters:**
None.

**Return/Output:**
- Launches a daemon process to perform the backup.

**Inner Mechanisms:**
1. Uses `cms_daemon()` to run the backup asynchronously.

**Usage Example:**
```php
CMS_IFC_MESSAGE = "backup";
include "module/#interface/ifc.setup.inc";
```

---

### `daemon` and `_daemon`

**Purpose:**
- `daemon`: Displays the daemon status UI.
- `_daemon`: Returns the current daemon status for AJAX polling.

**Parameters:**
None.

**Return/Output:**
- `daemon`: Renders a `<div>` for status updates and starts a JavaScript polling loop.
- `_daemon`: Returns the output of `cms_daemon_status()`.

**Inner Mechanisms:**
1. Uses `setTimeout` to poll `_daemon` every 5 seconds.
2. Updates the UI with the latest daemon status.

**Usage Example:**
```php
// Display daemon status UI
CMS_IFC_MESSAGE = "daemon";
include "module/#interface/ifc.setup.inc";

// Fetch daemon status (AJAX endpoint)
CMS_IFC_MESSAGE = "_daemon";
include "module/#interface/ifc.setup.inc";
```

---

## Main Display Logic

The file also contains UI rendering logic for each setup step, triggered when no specific `CMS_IFC_MESSAGE` is set. Key scenarios:

1. **Administrator Password Setup:**
   - Checks if the default password is still in use.
   - Displays a form to set a new password.

2. **Database Configuration:**
   - Checks if the database connection is missing/failed.
   - Displays a form to configure MySQL settings.

3. **Database Restoration:**
   - Checks if the `mysql.restore` flag is set.
   - Displays a confirmation dialog.

4. **SMTP Configuration:**
   - Validates and tests SMTP settings.
   - Displays a form for email configuration.

5. **Update/Backup Management:**
   - Checks for available updates.
   - Displays update/backup controls and progress.

---

## Helper Classes

| Class | Purpose |
|-------|---------|
| `data` | Manages key-value data storage (e.g., user permissions). |
| `system` | Manages system-wide configuration settings. |
| `mysql` | Handles database connections and restoration. |
| `smtp` | Manages email sending via SMTP. |
| `mime` | Constructs MIME email messages. |
| `update` | Handles system updates and backups. |
| `ifc` | Renders the interface UI components. |

---

## Example: Full Setup Workflow

1. **Set Administrator Password:**
   ```php
   $ifc_param1 = "new_password";
   $ifc_param2 = "new_password";
   CMS_IFC_MESSAGE = "setup_password";
   include "module/#interface/ifc.setup.inc";
   ```

2. **Configure Database:**
   ```php
   $ifc_param1 = "localhost";
   $ifc_param2 = "pwnc_db";
   $ifc_param3 = "db_user";
   $ifc_param4 = "db_password";
   CMS_IFC_MESSAGE = "setup_database";
   include "module/#interface/ifc.setup.inc";
   ```

3. **Test SMTP:**
   ```php
   $ifc_param1 = "noreply@pwnc.it";
   $ifc_param3 = "smtp";
   $ifc_param4 = "smtp.pwnc.it";
   CMS_IFC_MESSAGE = "setup_smtp";
   include "module/#interface/ifc.setup.inc";
   ```

4. **Mark Setup as Complete:**
   ```php
   CMS_IFC_MESSAGE = "do_not_show";
   include "module/#interface/ifc.setup.inc";
   ```


<!-- HASH:d93f0eb9953c2058ee0adacddff5f705 -->
