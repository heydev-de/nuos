# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.setup.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Interface Setup Module (`ifc.setup.inc`)

This file implements the NUOS web platform's initial setup interface. It handles critical first-time configuration tasks including administrator password setup, database connection configuration, SMTP email settings, and system updates/backups. The module is permission-protected and only accessible to users with `CMS_L_ACCESS` privileges.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | Dynamic | Determines which setup operation to perform (e.g., `setup_password`, `setup_database`). |
| `CMS_SUPERUSER` | `"admin"` | Default superuser account name. |
| `CMS_L_ACCESS` | Permission constant | Required permission level to access setup interface. |
| `$ifc_param1` - `$ifc_param5` | Dynamic | Input parameters for setup operations (e.g., passwords, database credentials). |
| `$ifc_response` | Dynamic | Response message to display after an operation. |
| `$setup_done` | Boolean | Flag indicating whether setup has been marked as complete. |

---

## Message Handling / Sub-Display

The module uses a `switch` statement to route `CMS_IFC_MESSAGE` to the appropriate setup operation.

---

### `setup_password`

**Purpose:**
Sets or updates the administrator password.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New password. |
| `$ifc_param2` | `string` | Password confirmation. |

**Return Values:**
- `CMS_MSG_DONE`: Success.
- `CMS_MSG_ERROR . CMS_L_IFC_SETUP_020`: Password is empty.
- `CMS_MSG_ERROR . CMS_L_IFC_SETUP_021`: Passwords do not match.

**Inner Mechanisms:**
1. Validates that the password is not empty and matches the confirmation.
2. Hashes the password using `hash64()`.
3. Stores the hashed password in the `#system/permission` data store under the superuser key.
4. Sets a secure cookie (`cms_password`) using a salted hash.
5. Saves the data store and returns success or failure.

**Usage Context:**
- Triggered during initial setup or when the default administrator password is still in use.
- Used to secure the superuser account.

---

### `setup_database`

**Purpose:**
Configures the MySQL database connection.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | MySQL host. |
| `$ifc_param2` | `string` | Database name. |
| `$ifc_param3` | `string` | MySQL username. |
| `$ifc_param4` | `string` | MySQL password. |

**Return Values:**
- `CMS_MSG_DONE`: Success.
- `CMS_MSG_ERROR`: Failure to save settings.

**Inner Mechanisms:**
1. Stores the provided MySQL credentials in the system configuration.
2. Saves the system configuration and returns success or failure.

**Usage Context:**
- Required for establishing a connection to the MySQL database.
- Used during initial setup or when reconfiguring the database.

---

### `mysql_restore`

**Purpose:**
Restores the database from a backup if the restore flag is set.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | `"yes"` or `"no"` to confirm or cancel restore. |

**Return Values:**
- `CMS_MSG_DONE`: Success.
- `CMS_MSG_ERROR . CMS_L_IFC_SETUP_031`: Restore failed.

**Inner Mechanisms:**
1. Checks if `$ifc_param1` is set to `"yes"`.
2. If confirmed, initializes a `mysql` object and calls its `restore()` method.
3. Clears the `mysql.restore` flag in the system configuration.
4. Returns success or an error message if the restore fails.

**Usage Context:**
- Used when the system detects a need to restore the database (e.g., after a failed update).

---

### `setup_smtp`

**Purpose:**
Configures SMTP email settings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Email address. |
| `$ifc_param2` | `string` | Email method (`"mail"` or custom SMTP). |
| `$ifc_param3` | `string` | SMTP host. |
| `$ifc_param4` | `string` | SMTP username. |
| `$ifc_param5` | `string` | SMTP password. |

**Return Values:**
- `CMS_MSG_DONE`: Success.
- `CMS_MSG_ERROR`: Failure to save settings or invalid input.

**Inner Mechanisms:**
1. Stores the provided SMTP settings in the system configuration.
2. Validates the settings by attempting to send a test email.
3. Returns success or an error message if the test fails.

**Usage Context:**
- Used to configure email delivery for system notifications and user communications.

---

### `do_not_show`

**Purpose:**
Marks the setup as complete to prevent it from showing after login.

**Return Values:**
- `CMS_MSG_DONE`: Success.
- `CMS_MSG_ERROR`: Failure to save settings.

**Inner Mechanisms:**
1. Sets the `setup.done` flag in the system configuration to `TRUE`.
2. Clears the `mysql.restore` flag.
3. Saves the system configuration and returns success or failure.

**Usage Context:**
- Used to hide the setup interface after initial configuration is complete.

---

### `update`

**Purpose:**
Triggers a system update in the background using a daemon process.

**Parameters:**
None (uses `$update_skip_backup` global variable).

**Inner Mechanisms:**
1. Calls `cms_daemon()` to start an update process in the background.
2. The daemon loads the `update` library and initiates the update.
3. If the update succeeds, the content cache is cleared.

**Usage Context:**
- Used to update the system to the latest version.

---

### `update_status`

**Purpose:**
Retrieves the status of an ongoing update and returns it as JSON.

**Return Values:**
- JSON-encoded array containing:
  - `status`: Current update status (`CMS_UPDATE_STATUS_NONE` or other status codes).
  - `log`: Update log or a default message if no update is in progress.

**Inner Mechanisms:**
1. Loads the `update` library.
2. Retrieves the update status and log using `get_status()` and `get_log()`.
3. Outputs the result as JSON and exits.

**Usage Context:**
- Used by the frontend to poll the update status and display progress.

---

### `backup`

**Purpose:**
Triggers a system backup in the background using a daemon process.

**Inner Mechanisms:**
1. Calls `cms_daemon()` to start a backup process in the background.
2. The daemon loads the `update` library and initiates the backup.

**Usage Context:**
- Used to create a backup of the system before performing updates.

---

### `daemon` and `_daemon`

**Purpose:**
Displays and retrieves the status of background daemon processes (e.g., updates, backups).

**Inner Mechanisms:**
- `daemon`: Renders a `<div>` to display daemon status and starts a JavaScript polling loop.
- `_daemon`: Outputs the current daemon status using `cms_daemon_status()` and exits.

**Usage Context:**
- Used to monitor the progress of background tasks.

---

## Main Display Logic

The main display logic renders the setup interface based on the system's current state:

1. **Administrator Password Setup:**
   - Checks if the superuser password is empty or still set to the default (`"admin"`).
   - Displays a form to set a new password.

2. **Database Connection Setup:**
   - Checks if a MySQL connection can be established.
   - Displays a form to configure database credentials if no connection exists.

3. **Database Restore:**
   - Checks if the `mysql.restore` flag is set.
   - Displays a confirmation form to restore the database.

4. **SMTP Configuration:**
   - Displays a form to configure email settings.
   - Validates the settings by sending a test email.

5. **Update and Backup:**
   - Checks for available updates and displays update options.
   - Provides options to create and download backups.
   - Displays progress bars and status updates for ongoing updates/backups.

---

## Helper Functions and UI Components

- **`ifc_table_open()` / `ifc_table_close()`:** Opens and closes HTML tables for form layout.
- **`$ifc->set()`:** Renders form fields (e.g., text inputs, buttons, radio buttons).
- **`ifc_inactive()`:** Displays an error if a required library is not loaded.
- **`download()`:** Downloads a file (e.g., backup archives).

---

## JavaScript Functions

- **`setup_update_status()`:** Polls the server for update status and updates the UI.
- **`setup_update_response()`:** Updates the UI with the latest log messages from the update process.

---

## Usage Scenarios

1. **Initial Setup:**
   - The module guides the user through setting up the administrator password, database connection, and SMTP settings.

2. **Post-Setup Configuration:**
   - Allows reconfiguration of database or SMTP settings if needed.

3. **System Maintenance:**
   - Provides options to update the system, create backups, and restore the database.

4. **Monitoring:**
   - Displays the status of background tasks (e.g., updates, backups) in real-time.


<!-- HASH:b59ef7b2bea405c66a9b0ee01882005c -->
