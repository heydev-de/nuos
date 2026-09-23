# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.update.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.update.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## update

The `update` class provides a complete self-contained update mechanism for the PWNC Web Platform. It handles checking for new versions, creating backups, downloading update archives, installing new files, and cleaning up temporary artifacts. The class uses a status file to track progress across multiple steps and logs all actions to a log file.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UPDATE_PATH` | `CMS_ROOT_PATH . "#update/"` | Directory where update-related files are stored. |
| `CMS_UPDATE_URL_VERSION` | `https://raw.githubusercontent.com/heydev-de/pwnc/refs/heads/main/pwnc/version.txt` | Remote URL to fetch the latest version string. |
| `CMS_UPDATE_URL_ARCHIVE` | `https://github.com/heydev-de/pwnc/archive/main.zip` | Remote URL to download the update archive. |
| `CMS_UPDATE_TOKEN` | `""` | Optional authentication token for private repositories. |
| `CMS_UPDATE_CHECK_INTERVAL` | `3600` | Time in seconds between version checks (1 hour). |
| `CMS_UPDATE_STATUS_ERROR` | `-1` | Status indicating an error occurred. |
| `CMS_UPDATE_STATUS_NONE` | `0` | Initial or idle status. |
| `CMS_UPDATE_STATUS_DONE` | `1` | Status indicating a step completed successfully. |
| `CMS_UPDATE_STATUS_BACKUP` | `2` | Status indicating backup is in progress. |
| `CMS_UPDATE_STATUS_DOWNLOAD` | `3` | Status indicating download is in progress. |
| `CMS_UPDATE_STATUS_INSTALL` | `4` | Status indicating installation is in progress. |
| `CMS_UPDATE_STATUS_CLEANUP` | `5` | Status indicating cleanup is in progress. |

### Properties

| Name | Type | Description |
|------|------|-------------|
| *(none)* | — | The class does not declare any explicit properties. All state is persisted via files in `CMS_UPDATE_PATH`. |

### Methods

#### __construct

Initializes the update system by ensuring the update directory exists.

**Parameters:** None.

**Return Value:** None.

**Inner Mechanism:** Calls `mkpath()` to create the `CMS_UPDATE_PATH` directory if it doesn't already exist.

**Usage Example:**
```php
$updater = new \cms\update();
// The update directory is now ready for use.
```

---

#### log

Appends or writes text to the update log file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The text to write to the log. |
| `$reset` | `bool` | If `TRUE`, overwrites the log file instead of appending. |
| `$newline` | `bool` | If `TRUE` and not resetting, prepends a newline before the text. |

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:** Uses `file_put_contents()` with `LOCK_EX` to safely write to the log file. When `$reset` is `FALSE`, it appends with a leading newline (if `$newline` is `TRUE`).

**Usage Example:**
```php
$updater = new \cms\update();
$updater->log("Starting update process...");
// Appends "Starting update process..." to update.log
```

---

#### get_log

Reads the entire contents of the update log file.

**Parameters:** None.

**Return Value:** `string` — The full log file contents, or an empty string if the file doesn't exist.

**Inner Mechanism:** Uses `read_file()` to read the log file and casts the result to a string.

**Usage Example:**
```php
$updater = new \cms\update();
echo $updater->get_log();
// Outputs the full update log
```

---

#### progress

Writes a single dot (`.`) to the log file without a newline, used as a visual progress indicator.

**Parameters:** None.

**Return Value:** `bool` — Result of the internal `log()` call.

**Inner Mechanism:** Calls `log(".", FALSE, FALSE)` to append a dot without a newline.

**Usage Example:**
```php
$updater = new \cms\update();
for ($i = 0; $i < 100; $i++) {
    // Perform some work...
    $updater->progress();
}
// Logs 100 dots as a progress indicator
```

---

#### status

Sets the current update status and optionally logs a message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | One of the `CMS_UPDATE_STATUS_*` constants. |
| `$text` | `string` | Optional message to log alongside the status change. |

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:** Checks if the status is transitioning from `CMS_UPDATE_STATUS_NONE` to determine whether to reset the log. Logs the message if provided, then writes the status value to `update.status`.

**Usage Example:**
```php
$updater = new \cms\update();
$updater->status(CMS_UPDATE_STATUS_BACKUP, "Creating backup...");
// Sets status to BACKUP and logs the message
```

---

#### get_status

Reads the current update status from the status file.

**Parameters:** None.

**Return Value:** `int` — The current status value, or `CMS_UPDATE_STATUS_NONE` if the file doesn't exist or is empty.

**Inner Mechanism:** Reads `update.status` using `read_file()` and casts to integer. Falls back to `CMS_UPDATE_STATUS_NONE` if the file is empty or unreadable.

**Usage Example:**
```php
$updater = new \cms\update();
$currentStatus = $updater->get_status();
if ($currentStatus === CMS_UPDATE_STATUS_NONE) {
    echo "No update in progress.";
}
```

---

#### error

Sets the status to `CMS_UPDATE_STATUS_ERROR` and logs an error message.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The error message to log. |

**Return Value:** `bool` — Result of the internal `status()` call.

**Inner Mechanism:** Calls `status(CMS_UPDATE_STATUS_ERROR, $text)` to set the error state and log the message.

**Usage Example:**
```php
$updater = new \cms\update();
$updater->error("Failed to connect to update server.");
// Sets status to ERROR and logs the message
```

---

#### available

Checks whether a newer version of PWNC is available.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$enforce` | `bool` | If `TRUE`, forces a fresh version check regardless of cache age. |

**Return Value:** `string|bool|null` — The new version string if an update is available, `NULL` if the current version is up-to-date, or `FALSE` on failure.

**Inner Mechanism:**
1. Checks the cached version file (`update.check`) and its modification time.
2. If the cache is older than `CMS_UPDATE_CHECK_INTERVAL` or `$enforce` is `TRUE`, fetches the latest version from `CMS_UPDATE_URL_VERSION` using an HTTP GET request with the configured user agent and optional bearer token.
3. Caches the fetched version string.
4. Compares the current `CMS_VERSION` with the fetched version using `version_compare()`.

**Usage Example:**
```php
$updater = new \cms\update();
$newVersion = $updater->available();
if ($newVersion !== NULL && $newVersion !== FALSE) {
    echo "Update available: " . $newVersion;
} elseif ($newVersion === FALSE) {
    echo "Failed to check for updates.";
} else {
    echo "You are up to date.";
}
```

---

#### start

Begins the full update process: backup, download, install, and cleanup.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$skip_backup` | `bool` | If `TRUE`, skips the backup step. |

**Return Value:** `bool` — `TRUE` if all steps completed successfully, `FALSE` otherwise.

**Inner Mechanism:**
1. Checks that no update is already in progress (status must be `CMS_UPDATE_STATUS_NONE`).
2. If `$skip_backup` is `TRUE`, sets status to `CMS_UPDATE_STATUS_DONE` with a skip message.
3. Executes the sequence: `backup()` → `download()` → `install()` → `cleanup()`.
4. Sets final status to `CMS_UPDATE_STATUS_DONE`.

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->start()) {
    echo "Update completed successfully.";
} else {
    echo "Update failed. Check the log: " . $updater->get_log();
}
```

---

#### backup

Creates a backup of the database and the entire PWNC installation.

**Parameters:** None.

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:**
1. Validates that the current status allows backup (`CMS_UPDATE_STATUS_NONE` or `CMS_UPDATE_STATUS_DONE`).
2. Sets status to `CMS_UPDATE_STATUS_BACKUP`.
3. Creates a MySQL database backup using the `mysql` class.
4. Creates a `backup/` directory inside the update path.
5. Iterates through all files in `CMS_ROOT_PATH`, excluding specified directories (update path, `.git/`, cache directories, etc.).
6. Adds each file to a temporary ZIP archive using `ZipArchive`.
7. Renames the temporary archive to `backup.zip`, preserving any previous backup as `_backup.zip`.
8. Logs progress every 100 files.

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->backup()) {
    echo "Backup created successfully.";
} else {
    echo "Backup failed: " . $updater->get_log();
}
```

---

#### download

Downloads the update archive from the remote repository.

**Parameters:** None.

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:**
1. Validates that the current status allows download (`CMS_UPDATE_STATUS_NONE` or `CMS_UPDATE_STATUS_DONE`).
2. Sets status to `CMS_UPDATE_STATUS_DOWNLOAD`.
3. Ensures the update directory exists.
4. Opens an HTTP connection to `CMS_UPDATE_URL_ARCHIVE` with the configured user agent and optional bearer token.
5. Checks the HTTP response code for a 200 OK status.
6. Streams the archive content to `update.zip` in 8KB chunks.
7. Uses file locking (`LOCK_EX`) during the write operation.
8. Logs progress every 500 chunks.

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->download()) {
    echo "Update archive downloaded successfully.";
} else {
    echo "Download failed: " . $updater->get_log();
}
```

---

#### install

Extracts and installs the new version from the downloaded archive.

**Parameters:** None.

**Return Value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:**
1. Validates that the current status allows installation (`CMS_UPDATE_STATUS_NONE` or `CMS_UPDATE_STATUS_DONE`).
2. Sets status to `CMS_UPDATE_STATUS_INSTALL`.
3. Creates extraction and rollback directories.
4. Opens the downloaded ZIP archive.
5. Determines the base directory name from the archive.
6. For each directory specified in the `$replace` array (currently only `pwnc`):
   - Extracts matching files to the extraction directory.
   - Moves the original directory to the rollback directory.
   - Moves the new directory from the extraction path to the root path.
7. On error, attempts to roll back by restoring original directories from the rollback path.
8. Logs progress and errors appropriately.

**Usage Example:**
```php
$updater = new \cms\update();
if ($updater->install()) {
    echo "Update installed successfully.";
} else {
    echo "Installation failed: " . $updater->get_log();
}
```

---

#### cleanup

Removes temporary files created during the update process.

**Parameters:** None.

**Return Value:** `bool` — Always `TRUE`.

**Inner Mechanism:**
1. Validates that the current status allows cleanup (`CMS_UPDATE_STATUS_NONE` or `CMS_UPDATE_STATUS_DONE`).
2. Sets status to `CMS_UPDATE_STATUS_CLEANUP`.
3. Deletes the downloaded `update.zip` file.
4. Loads the `filemanager` library and uses `filemanager_delete()` to remove the extraction and rollback directories.
5. Sets final status to `CMS_UPDATE_STATUS_DONE`.

**Usage Example:**
```php
$updater = new \cms\update();
$updater->cleanup();
echo "Temporary files cleaned up.";
```


<!-- HASH:d5dadd5d45f167e6968eb4b5d3101ddc -->
