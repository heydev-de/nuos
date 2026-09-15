# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.setup.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.setup.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Setup Interface

The `ifc.setup.inc` file is a core interface controller for the PWNC Web Platform's initial setup process. It handles configuration steps such as setting up the administrator password, database connection, SMTP settings, and managing system updates/backups. This interface is typically displayed during the first-time installation or when critical system configurations need attention.

### Permission Declaration

At the top of the file, the interface declares its access level:

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_L_ACCESS` | `""` | Access level required to view this interface |

### Message Handling

The main logic is driven by a `switch` statement based on the `CMS_IFC_MESSAGE` constant, which determines the current setup step or action being performed.

#### `setup_password`

Handles the creation or update of the administrator password.

**Parameters:**
- `$ifc_param1`: New password input
- `$ifc_param2`: Password confirmation input

**Mechanisms:**
1. Validates that the password is not empty using `stre()`.
2. Confirms password match using `nstreq()`.
3. Hashes the password with `hash64()` and stores it in the permission data store.
4. Sets a cookie with the hashed credential using `cms_set_cookie()`.

**Usage Example:**
```php
// Triggered when user submits new admin password form
// ifc_message = "setup_password"
// ifc_param1 = "newpassword123"
// ifc_param2 = "newpassword123"
```

#### `setup_database`

Configures the MySQL database connection parameters.

**Parameters:**
- `$ifc_param1`: Database host
- `$ifc_param2`: Database name
- `$ifc_param3`: Database user
- `$ifc_param4`: Database password

**Mechanisms:**
1. Creates a `system` instance.
2. Stores each parameter using `setval()` with the "mysql" namespace.
3. Saves the configuration.

**Usage Example:**
```php
// Triggered when user submits database configuration form
// ifc_message = "setup_database"
// ifc_param1 = "localhost"
// ifc_param2 = "pwnc_db"
// ifc_param3 = "pwnc_user"
// ifc_param4 = "secure_password"
```

#### `mysql_restore`

Manages database restoration from a backup.

**Parameters:**
- `$ifc_param1`: Confirmation flag ("yes" to proceed)

**Mechanisms:**
1. Checks if a selection was made.
2. If confirmed, establishes a MySQL connection and calls `restore()`.
3. Clears the restore flag from system settings.

**Usage Example:**
```php
// Triggered when user confirms database restore
// ifc_message = "mysql_restore"
// ifc_param1 = "yes"
```

#### `setup_smtp`

Configures SMTP email settings.

**Parameters:**
- `$ifc_param1`: Email address
- `$ifc_param2`: Reply-to address
- `$ifc_param3`: Mail method ("mail" or "smtp")
- `$ifc_param4`: SMTP host
- `$ifc_param5`: SMTP username
- `$ifc_param6`: SMTP password

**Mechanisms:**
1. Stores all email-related parameters in the system configuration.
2. Saves the configuration.

**Usage Example:**
```php
// Triggered when user submits SMTP configuration
// ifc_message = "setup_smtp"
// ifc_param1 = "admin@example.com"
// ifc_param2 = "noreply@example.com"
// ifc_param3 = "smtp"
// ifc_param4 = "smtp.example.com"
// ifc_param5 = "smtp_user"
// ifc_param6 = "smtp_pass"
```

#### `do_not_show`

Allows administrators to dismiss the setup interface after login.

**Mechanisms:**
1. Verifies the current user is an administrator.
2. Sets the "setup done" flag to TRUE.
3. Clears any pending database restore flag.

**Usage Example:**
```php
// Triggered when admin clicks "Don't show again"
// ifc_message = "do_not_show"
```

#### `update`

Initiates a system update process via daemon.

**Mechanisms:**
1. Loads the update library.
2. Starts the update process, optionally skipping backup.
3. Cleans cache after completion.

**Usage Example:**
```php
// Triggered when user initiates system update
// ifc_message = "update"
// Optionally with update_skip_backup checkbox set
```

#### `update_status`

Returns JSON-encoded status of an ongoing update process.

**Mechanisms:**
1. Loads the update library.
2. Retrieves current status and log.
3. Outputs JSON response and exits.

**Usage Example:**
```javascript
// Called via AJAX to poll update progress
// Endpoint returns [status_code, log_text]
```

#### `backup`

Creates a system backup via daemon.

**Mechanisms:**
1. Loads the update library.
2. Executes the backup method.

**Usage Example:**
```php
// Triggered when user initiates manual backup
// ifc_message = "backup"
```

#### `daemon`

Displays the daemon status interface with real-time updates.

**Mechanisms:**
1. Creates an interface object.
2. Outputs HTML/CSS/JavaScript for status display.
3. Uses AJAX polling to update status every 5 seconds.

**Usage Example:**
```php
// Triggered when viewing daemon status
// ifc_message = "daemon"
```

#### `_daemon`

Returns raw daemon status output.

**Mechanisms:**
1. Calls `cms_daemon_status()`.
2. Outputs result and exits.

**Usage Example:**
```javascript
// Called via AJAX to get daemon status text
```

### Main Display Logic

After processing messages, the interface displays appropriate setup forms based on system state:

1. **Password Setup**: Shown if superuser password is empty or default.
2. **Database Setup**: Shown if no database connection can be established.
3. **Database Restore**: Shown if a restore flag is set.
4. **SMTP Setup**: Shown in a loop until valid configuration is provided.
5. **Update Management**: Shows update status, available versions, and backup options.

Each form uses the `ifc` class for rendering and includes "do not show again" options when setup isn't marked as complete.


<!-- HASH:d93f0eb9953c2058ee0adacddff5f705 -->
