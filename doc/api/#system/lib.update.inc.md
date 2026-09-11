# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.update.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.update.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## update

The `update` class provides a complete self-contained update mechanism for the PWNC Web Platform. It handles checking for new versions, creating backups, downloading update archives, installing new files, and cleaning up temporary artifacts. The class uses a state machine pattern with status constants to track progress and supports rollback on failure.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UPDATE_PATH` | `CMS_ROOT_PATH . "#update/"` | Directory where update-related files are stored |
| `CMS_UPDATE_URL_VERSION` | GitHub raw URL | Remote endpoint for fetching the latest version string |
| `CMS_UPDATE_URL_ARCHIVE` | GitHub archive URL | Remote endpoint for downloading the full update ZIP archive |
| `CMS_UPDATE_TOKEN` | `""` | Optional authentication token for private repositories |
| `CMS_UPDATE_CHECK_INTERVAL` | `3600` | Time in seconds between version checks (1 hour) |
| `CMS_UPDATE_STATUS_ERROR` | `-1` | Update process encountered an error |
| `CMS_UPDATE_STATUS_NONE` | `0` | No update in progress |
| `CMS_UPDATE_STATUS_DONE` | `1` | Update step completed successfully |
| `CMS_UPDATE_STATUS_BACKUP` | `2` | Currently backing up files and database |
| `CMS_UPDATE_STATUS_DOWNLOAD` | `3` | Currently downloading the update archive |
| `CMS_UPDATE_STATUS_INSTALL` | `4` | Currently installing new files |
| `CMS_UPDATE_STATUS_CLEANUP` | `5` | Currently cleaning up temporary files |

### Properties

| Name | Type | Description |
|------|------|-------------|
| *(none)* | — | The class uses file-based state persistence via `update.status` and `update.log` files |

### Methods

#### __construct()

Initializes the update system by ensuring the update directory exists.

**Parameters:** None

**Return Value:** None

**Inner Mechanism:** Calls `mkpath()` to create the `CMS_UPDATE_PATH` directory if it doesn't already exist.

**Usage Example:**
```php
$updater = new \cms\update();
// Update directory is now ready for use
```

#### log($text, $reset = FALSE, $newline = TRUE)

Writes a message to the update log file.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | The text to write to the log |
| `$reset` | `bool` | `FALSE` | If `TRUE`, overwrites the log file instead of appending |
| `$newline` | `bool` | `TRUE` | If `TRUE`, prepends a newline character (unless resetting) |

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure

**Inner Mechanism:** Uses `file_put_contents()` with `FILE_APPEND` and `LOCK_EX` flags to safely append log entries. When `$reset` is `TRUE`, it truncates the file first.

**Usage Example:**
```php
$updater = new \cms\update();
$updater->log("Starting update process");
$updater->log("Step 1 complete", FALSE, TRUE);
```

#### get_log()

Retrieves the entire contents of the update log file.

**Parameters:** None

**Return Value:** `string` — The full log file contents, or an empty string if the file doesn't exist

**Inner Mechanism:** Reads the log file using `read_file()` and casts the result to a string.

**Usage Example:**
```php
$updater = new \cms\update();
echo $updater->get_log(); // Display all logged messages
```

#### progress()

Appends a single dot (`.`) to the log file without a newline, typically used to indicate ongoing progress.

**Parameters:** None

**Return Value:** `bool` — Result of the internal `log()` call

**Inner Mechanism:** Calls `log()` with `$newline = FALSE` to append a progress indicator.

**Usage Example:**
```php
$updater = new \cms\update();
for ($i = 0; $i < 100; $i++) {
    // Process item...
    $updater->progress(); // Show progress in log
}
```

#### status($value = CMS_UPDATE_STATUS_NONE, $text = "")

Updates the current update status and optionally logs a message.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `int` | `CMS_UPDATE_STATUS_NONE` | The new status value (one of the `CMS_UPDATE_STATUS_*` constants) |
| `$text` | `string` | `""` | Optional message to log when status changes |

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure

**Inner Mechanism:** 
1. Determines if this is a status reset (transitioning from `NONE` to another state)
2. Logs the message if provided
3. Writes the status value to `update.status` file
4. Returns `FALSE` if file write fails

**Usage Example:**
```php
$updater = new \cms\update();
$updater->status(CMS_UPDATE_STATUS_BACKUP, "Creating backup...");
```

#### get_status()

Reads the current update status from the status file.

**Parameters:** None

**Return Value:** `int` — The current status value, or `CMS_UPDATE_STATUS_NONE` if no status file exists

**Inner Mechanism:** Reads the `update.status` file using `read_file()` and casts to integer. Falls back to `CMS_UPDATE_STATUS_NONE` if the file is empty or missing.

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->get_status() === CMS_UPDATE_STATUS_NONE) {
    echo "No update in progress";
}
```

#### error($text)

Sets the update status to error and logs an error message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The error message to log |

**Return Value:** `bool` — Result of the internal `status()` call

**Inner Mechanism:** Calls `status()` with `CMS_UPDATE_STATUS_ERROR` and the provided error text.

**Usage Example:**
```php
$updater = new \cms\update();
if (!$someOperation) {
    $updater->error("Failed to perform critical operation");
}
```

#### available($enforce = FALSE)

Checks whether a newer version of PWNC is available.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enforce` | `bool` | `FALSE` | If `TRUE`, forces a fresh version check regardless of cache |

**Return Value:** `mixed` — The remote version string if an update is available, `NULL` if current version is up-to-date, `FALSE` on error

**Inner Mechanism:**
1. Checks cached version file (`update.check`) and its modification time
2. If cache is stale or `$enforce` is `TRUE`, fetches the latest version from `CMS_UPDATE_URL_VERSION`
3. Caches the result in `update.check`
4. Compares `CMS_VERSION` with the remote version using `version_compare()`
5. Returns the remote version if newer, `NULL` if current, `FALSE` on fetch error

**Usage Example:**
```php
$updater = new \cms\update();
$newVersion = $updater->available();
if ($newVersion !== NULL && $newVersion !== FALSE) {
    echo "Update available: " . $newVersion;
}
```

#### start($skip_backup = FALSE)

Initiates the full update process: backup → download → install → cleanup.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$skip_backup` | `bool` | `FALSE` | If `TRUE`, skips the backup phase |

**Return Value:** `bool` — `TRUE` if all steps completed successfully, `FALSE` otherwise

**Inner Mechanism:**
1. Verifies no update is currently in progress
2. Optionally skips backup and sets status to `DONE`
3. Executes the sequence: `backup()` → `download()` → `install()` → `cleanup()`
4. Sets final status to `DONE` with completion message
5. Returns `TRUE` only if all steps succeed

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->start()) {
    echo "Update completed successfully";
} else {
    echo "Update failed: " . $updater->get_log();
}
```

#### backup()

Creates a backup of the database and file system before updating.

**Parameters:** None

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure

**Inner Mechanism:**
1. Validates current status allows backup
2. Sets status to `CMS_UPDATE_STATUS_BACKUP`
3. Creates a MySQL database backup using the `mysql` class
4. Creates a ZIP archive of the entire `CMS_ROOT_PATH` directory
5. Excludes update directory, `.git`, `.github`, cache directories, and other non-essential paths
6. Uses a temporary file during creation, then renames to final backup file
7. Maintains a rolling backup (previous backup is preserved as `_backup.zip`)
8. Logs progress every 100 files

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->backup()) {
    echo "Backup created successfully";
} else {
    echo "Backup failed: " . $updater->get_log();
}
```

#### download()

Downloads the update archive from the remote repository.

**Parameters:** None

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure

**Inner Mechanism:**
1. Validates current status allows download
2. Sets status to `CMS_UPDATE_STATUS_DOWNLOAD`
3. Creates HTTP context with user agent and optional auth token
4. Opens remote file stream and verifies HTTP 200 response
5. Downloads archive in 8KB chunks to `update.zip`
6. Uses file locking (`LOCK_EX`) during write
7. Logs progress every 500 chunks
8. Cleans up on error

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->download()) {
    echo "Update archive downloaded";
} else {
    echo "Download failed: " . $updater->get_log();
}
```

#### install()

Extracts and installs the new files from the downloaded archive.

**Parameters:** None

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure

**Inner Mechanism:**
1. Validates current status allows installation
2. Sets status to `CMS_UPDATE_STATUS_INSTALL`
3. Creates extraction directory (`new/`) and rollback directory (`old/`)
4. Opens the ZIP archive and identifies the base directory
5. For each directory in the `$replace` map (currently only `pwnc`):
   - Extracts matching files to the extraction directory
   - Moves original files to the rollback directory
   - Moves new files to their final location
6. On error, attempts to restore original files from rollback directory
7. Logs progress and errors appropriately

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->install()) {
    echo "Files installed successfully";
} else {
    echo "Installation failed: " . $updater->get_log();
}
```

#### cleanup()

Removes temporary files created during the update process.

**Parameters:** None

**Return Value:** `bool` — Always `TRUE`

**Inner Mechanism:**
1. Validates current status allows cleanup
2. Sets status to `CMS_UPDATE_STATUS_CLEANUP`
3. Deletes the downloaded `update.zip` file
4. Loads the `filemanager` library and deletes the `new/` and `old/` directories
5. Sets final status to `DONE` with completion message

**Usage Example:**
```php
$updater = new \cms\update();
$updater->cleanup();
echo "Temporary files removed";
```

### Usage Flow

The typical update workflow follows these steps:

1. **Check availability:**
```php
$updater = new \cms\update();
$newVersion = $updater->available();
```

2. **Start the update process:**
```php
if ($newVersion !== NULL && $newVersion !== FALSE) {
    $success = $updater->start();
    if (!$success) {
        // Check log for details
        error_log($updater->get_log());
    }
}
```

3. **Or run steps individually for more control:**
```php
$updater = new \cms\update();
$updater->backup();
$updater->download();
$updater->install();
$updater->cleanup();
```

### Error Handling

The class uses a file-based status system. After any operation, you can check the status and retrieve logs:

```php
$updater = new \cms\update();
$updater->start();

$status = $updater->get_status();
if ($status === CMS_UPDATE_STATUS_ERROR) {
    echo "Update failed:\n" . $updater->get_log();
}
```


<!-- HASH:b799b4d122c782f9c07d795a1690e145 -->
