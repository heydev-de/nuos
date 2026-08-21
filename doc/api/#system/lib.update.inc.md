# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.update.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.update.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Update Class

The `update` class provides a comprehensive system for managing platform updates in the PWNC Web Platform. It handles version checking, downloading, installing, and cleaning up updates while maintaining system integrity through backup and rollback mechanisms.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UPDATE_PATH` | `CMS_ROOT_PATH . "#update/"` | Base directory for update operations |
| `CMS_UPDATE_URL_VERSION` | GitHub raw URL for version file | Remote location of current version information |
| `CMS_UPDATE_URL_ARCHIVE` | GitHub ZIP archive URL | Remote location of update package |
| `CMS_UPDATE_TOKEN` | Empty string | Optional authentication token for private repositories |
| `CMS_UPDATE_CHECK_INTERVAL` | 3600 (1 hour) | Minimum interval between version checks |
| `CMS_UPDATE_STATUS_ERROR` | -1 | Update process encountered an error |
| `CMS_UPDATE_STATUS_NONE` | 0 | No update in progress |
| `CMS_UPDATE_STATUS_DONE` | 1 | Current update step completed successfully |
| `CMS_UPDATE_STATUS_BACKUP` | 2 | Backup phase in progress |
| `CMS_UPDATE_STATUS_DOWNLOAD` | 3 | Download phase in progress |
| `CMS_UPDATE_STATUS_INSTALL` | 4 | Installation phase in progress |
| `CMS_UPDATE_STATUS_CLEANUP` | 5 | Cleanup phase in progress |

### Constructor

```php
function __construct()
```

**Purpose:**
Initializes the update system by ensuring the update directory exists.

**Inner Mechanisms:**
- Uses `mkpath()` to create the update directory if it doesn't exist
- No parameters or return values

---

### `log()`

```php
function log($text, $reset = FALSE, $newline = TRUE)
```

**Purpose:**
Writes messages to the update log file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Message to log |
| `$reset` | bool | If TRUE, overwrites existing log; if FALSE, appends |
| `$newline` | bool | If TRUE, adds newline before message (unless resetting) |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
- Uses `file_put_contents()` with `LOCK_EX` flag for atomic writes
- Handles newline insertion based on `$reset` and `$newline` parameters
- Log file location: `CMS_UPDATE_PATH . "update.log"`

**Usage Example:**
```php
$update = new update();
$update->log("Starting update process", TRUE);  // Creates new log
$update->log("Downloading update package");     // Appends to log
```

---

### `get_log()`

```php
function get_log()
```

**Purpose:**
Retrieves the entire contents of the update log.

**Return Values:**
- `string`: Contents of the log file, or empty string if file doesn't exist

**Inner Mechanisms:**
- Uses `read_file()` helper function
- Log file location: `CMS_UPDATE_PATH . "update.log"`

**Usage Example:**
```php
$update = new update();
$logContents = $update->get_log();
echo "<pre>$logContents</pre>";
```

---

### `progress()`

```php
function progress()
```

**Purpose:**
Adds a progress indicator (dot) to the log without newline.

**Return Values:**
- `bool`: Result of the underlying `log()` call

**Inner Mechanisms:**
- Calls `log(".", FALSE, FALSE)` to append a dot
- Used during long operations to show progress

**Usage Example:**
```php
// During a long operation
for ($i = 0; $i < 1000; $i++) {
    // Process item
    $update->progress();  // Adds dot to log every iteration
}
```

---

### `status()`

```php
function status($value = CMS_UPDATE_STATUS_NONE, $text = "")
```

**Purpose:**
Gets or sets the current update status.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | int | Status constant (CMS_UPDATE_STATUS_*) |
| `$text` | string | Optional message to log with status change |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
- Status file location: `CMS_UPDATE_PATH . "update.status"`
- If `$value` is not `CMS_UPDATE_STATUS_NONE`, writes to status file
- If `$text` is provided, logs the message
- Resets log file when transitioning from `CMS_UPDATE_STATUS_NONE`

**Usage Example:**
```php
$update = new update();
$update->status(CMS_UPDATE_STATUS_DOWNLOAD, "Starting download");  // Sets status and logs
$currentStatus = $update->status();  // Gets current status
```

---

### `get_status()`

```php
function get_status()
```

**Purpose:**
Retrieves the current update status.

**Return Values:**
- `int`: Current status constant (CMS_UPDATE_STATUS_*)

**Inner Mechanisms:**
- Reads from status file: `CMS_UPDATE_PATH . "update.status"`
- Returns `CMS_UPDATE_STATUS_NONE` if file doesn't exist or is empty

**Usage Example:**
```php
$update = new update();
switch ($update->get_status()) {
    case CMS_UPDATE_STATUS_NONE:
        echo "No update in progress";
        break;
    case CMS_UPDATE_STATUS_DOWNLOAD:
        echo "Update is downloading";
        break;
    // ... other cases
}
```

---

### `error()`

```php
function error($text)
```

**Purpose:**
Sets the update status to error and logs the error message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Error message to log |

**Return Values:**
- `bool`: Result of the underlying `status()` call

**Inner Mechanisms:**
- Calls `status(CMS_UPDATE_STATUS_ERROR, $text)`

**Usage Example:**
```php
$update = new update();
if (!file_exists($requiredFile)) {
    $update->error("Required file not found: $requiredFile");
    return FALSE;
}
```

---

### `available()`

```php
function available($enforce = FALSE)
```

**Purpose:**
Checks if a newer version of the platform is available.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$enforce` | bool | If TRUE, ignores cache and checks immediately |

**Return Values:**
- `string|FALSE|NULL`: Version string if update available, FALSE on error, NULL if no update

**Inner Mechanisms:**
- Checks local cache file (`CMS_UPDATE_PATH . "update.check"`) for last check time
- If cache is older than `CMS_UPDATE_CHECK_INTERVAL` or `$enforce` is TRUE:
  - Makes HTTP request to `CMS_UPDATE_URL_VERSION`
  - Uses custom User-Agent and optional Bearer token
  - Updates cache file with new version
- Compares current version (`CMS_VERSION`) with remote version
- Returns version string if remote version is newer

**Usage Example:**
```php
$update = new update();
if (($newVersion = $update->available()) !== NULL) {
    echo "Update available: $newVersion";
    if ($update->start()) {
        echo "Update completed successfully!";
    }
}
```

---

### `start()`

```php
function start($skip_backup = FALSE)
```

**Purpose:**
Initiates the complete update process.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$skip_backup` | bool | If TRUE, skips backup phase |

**Return Values:**
- `bool`: TRUE if update completed successfully, FALSE otherwise

**Inner Mechanisms:**
- Checks current status is `CMS_UPDATE_STATUS_NONE`
- If `$skip_backup` is FALSE, performs backup
- Downloads update package
- Installs update
- Performs cleanup
- Sets final status to `CMS_UPDATE_STATUS_DONE`

**Usage Example:**
```php
$update = new update();
if ($update->start()) {
    echo "System updated successfully!";
} else {
    echo "Update failed. Check logs for details.";
}
```

---

### `backup()`

```php
function backup()
```

**Purpose:**
Creates a complete backup of the current system (database and files).

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. **Database Backup:**
   - Uses `mysql` class to create database dump
   - Logs success/failure

2. **File Backup:**
   - Creates backup directory: `CMS_UPDATE_PATH . "backup/"`
   - Includes: Entire `CMS_ROOT_PATH` (with exclusions)
   - Excludes: Git directories, cache directories, update directory
   - Creates ZIP archive with all files
   - Maintains original directory structure
   - Handles large files with periodic progress updates
   - Manages existing backup files (renames old backup)

3. **Error Handling:**
   - Detailed error messages for each failure point
   - Comprehensive rollback on failure

**Usage Example:**
```php
$update = new update();
if ($update->backup()) {
    echo "Backup completed successfully!";
} else {
    echo "Backup failed. System not updated.";
    exit;
}
```

---

### `download()`

```php
function download()
```

**Purpose:**
Downloads the update package from the remote server.

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. **Preparation:**
   - Ensures update directory exists
   - Sets up HTTP context with User-Agent and optional Bearer token

2. **Download Process:**
   - Opens remote file (`CMS_UPDATE_URL_ARCHIVE`)
   - Verifies HTTP 200 response
   - Creates local file (`CMS_UPDATE_PATH . "update.zip"`)
   - Uses file locking to prevent concurrent access
   - Streams download in chunks (8KB)
   - Shows progress with periodic dots
   - Handles time limits for long downloads

3. **Error Handling:**
   - Verifies HTTP response code
   - Checks file creation and writing
   - Cleans up partial downloads on failure

**Usage Example:**
```php
$update = new update();
if ($update->download()) {
    echo "Download completed successfully!";
} else {
    echo "Download failed. Check logs for details.";
}
```

---

### `install()`

```php
function install()
```

**Purpose:**
Installs the downloaded update package.

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. **Preparation:**
   - Creates directories for new files and rollback
   - Opens downloaded ZIP archive (`CMS_UPDATE_PATH . "update.zip"`)

2. **Installation Process:**
   - Identifies base directory in ZIP archive
   - Processes specific directories (`pwnc/` and `nuos/`)
   - For each directory:
     - Extracts files to temporary location
     - Moves current version to rollback directory
     - Moves new version to production location

3. **Migration Handling:**
   - Handles temporary migration from `/nuos/` to `/pwnc/`
   - Creates redirect from `/pwnc/nuos/` to `/nuos/`

4. **Rollback:**
   - On failure, attempts to restore original files from rollback directory
   - Provides detailed error messages

**Usage Example:**
```php
$update = new update();
if ($update->install()) {
    echo "Installation completed successfully!";
} else {
    echo "Installation failed. System may be in inconsistent state.";
}
```

---

### `cleanup()`

```php
function cleanup()
```

**Purpose:**
Removes temporary files after update completion.

**Return Values:**
- `bool`: Always returns TRUE

**Inner Mechanisms:**
- Deletes downloaded ZIP archive (`CMS_UPDATE_PATH . "update.zip"`)
- Uses `filemanager` module to delete:
  - Temporary extraction directory (`CMS_UPDATE_PATH . "new/"`)
  - Rollback directory (`CMS_UPDATE_PATH . "old/"`)

**Usage Example:**
```php
$update = new update();
$update->cleanup();  // Typically called automatically by start()
echo "Temporary files removed";
```


<!-- HASH:53e49c1966592103b8bae1fbc24253dc -->
