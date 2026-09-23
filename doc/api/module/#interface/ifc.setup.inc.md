# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.setup.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.setup.inc)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Setup Interface Controller

The `ifc.setup.inc` file is a critical component of the PWNC Web Platform's installation and configuration system. It serves as the primary interface controller for the initial setup process, handling tasks such as setting up administrator passwords, configuring database connections, restoring databases, configuring SMTP settings, managing system updates, creating backups, and monitoring background daemon processes.

This file operates within the `cms` namespace and uses a switch statement driven by the `CMS_IFC_MESSAGE` constant to determine which setup action to perform. It interacts heavily with core PWNC classes like `data`, `system`, `mysql`, `smtp`, `mime`, and `update`, leveraging their methods to persist configuration changes and manage system state.

### Key Concepts

| Constant | Description |
|----------|-------------|
| `CMS_IFC_MESSAGE` | Determines the current setup action to execute |
| `CMS_SUPERUSER` | Identifier for the superuser account (typically "admin") |
| `CMS_MSG_DONE` | Success message indicator |
| `CMS_MSG_ERROR` | Error message indicator |
| `CMS_L_IFC_SETUP_*` | Language constants for setup interface labels and messages |
| `CMS_UPDATE_STATUS_NONE` | Indicates no update is in progress |
| `CMS_VERSION` | Current platform version string |
| `CMS_EMAIL_AGENT` | Default email address used when none is provided |
| `CMS_UPDATE_PATH` | Filesystem path where update-related files are stored |

---

## Message Handling Switch

The main logic of this file is organized around a `switch` statement that evaluates `CMS_IFC_MESSAGE`. Each case corresponds to a specific setup operation:

### Case: `setup_password`

Handles the initial setup of the administrator password.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | The new password entered by the user |
| `$ifc_param2` | string | Confirmation of the new password |

#### Logic

1. Checks if the password field is empty using `stre()`.
2. Verifies that both password fields match using `nstreq()`.
3. Hashes the password with `hash64()` and stores it in the permission data store under the superuser key.
4. Sets a cookie containing the salted password hash for authentication.
5. Saves the data and returns success or error status.

#### Usage Example

When a user first installs PWNC and needs to set the admin password:
```php
// Triggered via form submission with ifc_message=setup_password
// User enters "mySecurePass123" in both password fields
// System hashes and saves the password, sets auth cookie
```

---

### Case: `setup_database`

Configures the MySQL database connection parameters.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Database host (e.g., "localhost") |
| `$ifc_param2` | string | Database name |
| `$ifc_param3` | string | Database username |
| `$ifc_param4` | string | Database password |

#### Logic

1. Creates a `system` instance.
2. Stores each parameter in the system configuration under the "mysql" section.
3. Saves the configuration and returns success or error status.

#### Usage Example

During installation, after entering database credentials:
```php
// Form submits with ifc_message=setup_database
// Values: localhost, pwnc_db, dbuser, secretpass
// System persists these values for future database connections
```

---

### Case: `mysql_restore`

Manages the restoration of a database from a backup.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Either "yes" to proceed with restore or any other value to skip |

#### Logic

1. If no selection is made, exits early.
2. If "yes" is selected, creates a `mysql` instance and calls its `restore()` method.
3. Clears the restore flag from system configuration regardless of outcome.
4. Returns success or error status.

#### Usage Example

After uploading a database backup file:
```php
// User selects "Yes" to restore database
// System calls mysql->restore() to import the backup
// Restore flag is cleared from configuration
```

---

### Case: `setup_smtp`

Configures SMTP email settings.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Email address (sender) |
| `$ifc_param2` | string | Reply-to email address |
| `$ifc_param3` | string | Mail delivery method ("mail" or "smtp") |
| `$ifc_param4` | string | SMTP server hostname |
| `$ifc_param5` | string | SMTP username |
| `$ifc_param6` | string | SMTP password |

#### Logic

1. Creates a `system` instance.
2. Stores all email-related parameters in the system configuration.
3. Saves the configuration and returns success or error status.

#### Usage Example

Configuring email notifications:
```php
// User fills out SMTP form with Gmail settings
// System saves: smtp.gmail.com, user@gmail.com, app_password
// Future emails will be sent through configured SMTP server
```

---

### Case: `do_not_show`

Allows administrators to hide the setup interface after login.

#### Logic

1. Restricts access to the admin user only.
2. Sets the "setup.done" flag to TRUE in system configuration.
3. Clears the "mysql.restore" flag.
4. Saves configuration and returns success or error status.

#### Usage Example

After completing initial setup:
```php
// Admin clicks "Don't show setup again"
// System marks setup as complete, hides setup UI on subsequent logins
```

---

### Case: `update`

Triggers a system update process via a background daemon.

#### Logic

1. Loads the update library.
2. Starts the update process, optionally skipping backup based on `$update_skip_backup`.
3. Cleans the content cache upon completion.

#### Usage Example

Initiating a platform update:
```php
// User clicks "Update Now"
// System runs update daemon, cleans cache afterward
// Progress is monitored through update_status case
```

---

### Case: `update_status`

Provides real-time status information about ongoing updates.

#### Logic

1. Loads the update library.
2. Retrieves current update status and log.
3. Outputs JSON-encoded status and log data.
4. Exits immediately to prevent further rendering.

#### Usage Example

Frontend polling for update progress:
```javascript
// JavaScript periodically calls this endpoint
// Receives JSON with status code and log text
// Updates progress bar and status display accordingly
```

---

### Case: `backup`

Creates a system backup via a background daemon.

#### Logic

1. Loads the update library.
2. Calls the backup method on the update instance.

#### Usage Example

Creating a manual backup:
```php
// User clicks "Create Backup"
// System runs backup daemon, creates backup.zip file
// Backup appears in download list for retrieval
```

---

### Case: `daemon`

Displays the daemon status monitoring interface.

#### Logic

1. Creates an interface object with response, page, and title parameters.
2. Outputs HTML/CSS/JavaScript for a live-updating daemon status display.
3. Uses AJAX polling to fetch daemon status every 5 seconds.

#### Usage Example

Monitoring background processes:
```html
<!-- Shows live status of update/backup daemons -->
<!-- Automatically refreshes every 5 seconds -->
<!-- Displays formatted log output in real-time -->
```

---

### Case: `_daemon`

Returns raw daemon status information.

#### Logic

1. Outputs the result of `cms_daemon_status()`.
2. Exits immediately.

#### Usage Example

Backend endpoint for daemon status polling:
```php
// Called by JavaScript in daemon case
// Returns plain text status of all running daemons
```

---

## Main Display Section

After processing the message switch, the file renders the main setup interface based on current system state.

### Administrator Password Setup

Checks if the current user is the administrator and whether their password is empty or default. If so, displays a form to change the password.

#### Logic

1. Loads permission data for the superuser.
2. Checks if password is empty or matches the default hash.
3. If condition is met, displays password change form with validation fields.
4. Includes option to mark setup as complete.

#### Usage Example

First-time login with default credentials:
```php
// System detects default password hash
// Displays password change form
// User must set new password before proceeding
```

---

### Database Connection Setup

Attempts to establish a database connection and displays configuration form if connection fails.

#### Logic

1. Attempts to create a `mysql` instance.
2. If connection fails, displays database configuration form.
3. Pre-fills form with existing values from system configuration.
4. Includes option to mark setup as complete.

#### Usage Example

Database not yet configured:
```php
// System cannot connect to database
// Displays form with host, database, user, password fields
// User enters correct credentials to proceed
```

---

### Database Restore Prompt

Checks if a database restore is pending and displays confirmation form.

#### Logic

1. Checks system configuration for "mysql.restore" flag.
2. If set, displays restore confirmation form with yes/no options.
3. Includes option to mark setup as complete.

#### Usage Example

Backup file uploaded but not yet restored:
```php
// System detects restore flag is set
// Displays confirmation dialog
// User chooses to restore or skip
```

---

### SMTP Configuration

Handles SMTP setup with validation and testing capabilities.

#### Logic

1. Enters a loop to handle SMTP configuration display and validation.
2. If message is "setup_smtp", loads existing values from system configuration.
3. Validates that required fields are present for non-mail methods.
4. Tests SMTP connection by sending a test email.
5. Displays configuration form with all SMTP parameters.
6. Includes option to mark setup as complete.

#### Usage Example

Configuring email notifications:
```php
// User enters SMTP details
// System tests connection by sending test email
// On success, saves configuration and proceeds
// On failure, displays error and prompts for correction
```

---

### Update Management Interface

Displays update status, available versions, and backup options.

#### Logic

1. Checks if update library is available.
2. Determines if update/backup daemons are scheduled or running.
3. If not running, checks for available updates.
4. Displays appropriate interface based on update availability:
   - No connection: Shows retry button
   - No new version: Shows info message
   - New version: Shows update button with backup option
5. Always shows backup creation button.
6. Lists available backup files for download.
7. If update/backup is running, shows progress bar and status polling.

#### Usage Example

Checking for platform updates:
```php
// System checks for new version
// If available, shows "Update Now" button
// User can choose to skip backup or create one first
// Progress bar shows real-time update status
```

---

## Helper Functions Used

Throughout this file, several PWNC utility functions are employed:

| Function | Purpose |
|----------|---------|
| `stre($v)` | Checks if a value is empty |
| `nstreq($a, $b)` | Checks if two values are not equal |
| `streq($a, $b)` | Checks if two values are equal |
| `hash64($v)` | Generates a 64-character hash of a value |
| `cms_salt($type)` | Generates a salt for a given type |
| `cms_set_cookie($data)` | Sets cookies with provided data |
| `cms_error_silent($mode)` | Toggles error reporting |
| `cms_load($lib)` | Loads a library class |
| `cms_daemon($code, $id, $priority, $label)` | Executes code in a background daemon |
| `cms_daemon_exists($id)` | Checks if a daemon exists |
| `cms_daemon_running($id)` | Checks if a daemon is running |
| `cms_daemon_status()` | Gets status of all daemons |
| `cms_url($params)` | Generates URLs with current state |
| `q($s)` | Encodes strings for JavaScript output |
| `x($s)` | Escapes XML special characters |
| `download($path)` | Forces file download |
| `ifc_table_open($style)` | Opens an interface table |
| `ifc_table_close()` | Closes an interface table |
| `ifc_inactive($page)` | Marks interface as inactive |
| `init($value, $label)` | Initializes a value with a label |

This file represents the complete setup workflow for the PWNC platform, guiding users through essential configuration steps while providing robust error handling and real-time feedback mechanisms.


<!-- HASH:3c3bbf0419547fed30f1278106f651a3 -->
