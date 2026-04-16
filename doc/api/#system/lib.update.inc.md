# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.update.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Update Class

The `update` class provides a self-contained, atomic update mechanism for the NUOS platform. It handles version checks, backup creation, download, installation, and cleanup of update packages. The class maintains a state machine to ensure operations are performed in the correct order and can be safely resumed after interruptions.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_UPDATE_PATH` | `CMS_ROOT_PATH . "#update/"` | Filesystem path to the update working directory. |
| `CMS_UPDATE_URL_VERSION` | `"https://raw.githubusercontent.com/heydev-de/nuos/refs/heads/main/nuos/version.txt"` | Remote URL to fetch the latest version string. |
| `CMS_UPDATE_URL_ARCHIVE` | `"https://github.com/heydev-de/nuos/archive/main.zip"` | Remote URL to download the update archive. |
| `CMS_UPDATE_TOKEN` | `""` | Optional bearer token for authenticated requests. |
| `CMS_UPDATE_CHECK_INTERVAL` | `3600` | Minimum interval (seconds) between version checks. |
| `CMS_UPDATE_STATUS_ERROR` | `-1` | Update failed; error state. |
| `CMS_UPDATE_STATUS_NONE` | `0` | No update in progress; idle state. |
| `CMS_UPDATE_STATUS_DONE` | `1` | Current step completed successfully. |
| `CMS_UPDATE_STATUS_BACKUP` | `2` | Backup phase active. |
| `CMS_UPDATE_STATUS_DOWNLOAD` | `3` | Download phase active. |
| `CMS_UPDATE_STATUS_INSTALL` | `4` | Installation phase active. |
| `CMS_UPDATE_STATUS_CLEANUP` | `5` | Cleanup phase active. |

---

### Constructor

#### `__construct()`

**Purpose**
Initializes the update environment by ensuring the working directory (`CMS_UPDATE_PATH`) exists.

**Parameters**
None.

**Return Values**
None.

**Inner Mechanisms**
Calls `mkpath()` to create the directory if it does not exist.

**Usage Context**
Instantiate the class to prepare the update environment.

---

### Logging Methods

#### `log($text, $reset = FALSE, $newline = TRUE)`

**Purpose**
Appends or resets the update log file (`update.log`) with the provided text.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | Text to log. |
| `$reset` | `bool` | `FALSE` | If `TRUE`, overwrites the log; otherwise appends. |
| `$newline` | `bool` | `TRUE` | If `TRUE` and not resetting, prepends a newline. |

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
Uses `file_put_contents()` with `FILE_APPEND` and `LOCK_EX` flags for atomic writes.

**Usage Context**
Used internally to record progress, errors, and status changes.

---

#### `get_log()`

**Purpose**
Retrieves the entire content of the update log.

**Parameters**
None.

**Return Values**
`string` – Log content; empty string if the log does not exist.

**Inner Mechanisms**
Checks for the log file and reads it using `file_get_contents()`.

**Usage Context**
Used to display the log to administrators or for debugging.

---

#### `progress()`

**Purpose**
Appends an ellipsis (`…`) to the log to indicate ongoing activity without a newline.

**Parameters**
None.

**Return Values**
`bool` – Result of the underlying `log()` call.

**Inner Mechanisms**
Calls `log("…", FALSE, FALSE)`.

**Usage Context**
Used during long-running operations (e.g., backup, download) to signal activity.

---

### Status Management

#### `status($value = CMS_UPDATE_STATUS_NONE, $text = "")`

**Purpose**
Sets the current update state and optionally logs a message.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `int` | `CMS_UPDATE_STATUS_NONE` | One of the `CMS_UPDATE_STATUS_*` constants. |
| `$text` | `string` | `""` | Optional message to log. |

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Resets the log if transitioning from `CMS_UPDATE_STATUS_NONE` to any other state.
- Writes the state value to `update.status` with an exclusive lock.
- Logs the message if provided.

**Usage Context**
Used to advance the state machine and record progress.

---

#### `get_status()`

**Purpose**
Retrieves the current update state.

**Parameters**
None.

**Return Values**
`int` – One of the `CMS_UPDATE_STATUS_*` constants.

**Inner Mechanisms**
- Opens `update.status` with a shared lock to prevent race conditions.
- Returns `CMS_UPDATE_STATUS_NONE` if the file does not exist or cannot be read.

**Usage Context**
Used to check the current state before proceeding to the next step.

---

#### `error($text)`

**Purpose**
Sets the state to `CMS_UPDATE_STATUS_ERROR` and logs an error message.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | Error message to log. |

**Return Values**
`bool` – Result of the underlying `status()` call.

**Inner Mechanisms**
Calls `status(CMS_UPDATE_STATUS_ERROR, $text)`.

**Usage Context**
Used to signal and record failures during update steps.

---

### Version Check

#### `available($enforce = FALSE)`

**Purpose**
Checks if a newer version of NUOS is available.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enforce` | `bool` | `FALSE` | If `TRUE`, bypasses the check interval and forces a remote request. |

**Return Values**
- `string` – The latest version string if an update is available.
- `NULL` – No update available.
- `FALSE` – Error during the check.

**Inner Mechanisms**
- Uses a local cache file (`update.check`) to avoid excessive remote requests.
- Fetches the latest version from `CMS_UPDATE_URL_VERSION` if the cache is stale or `$enforce` is `TRUE`.
- Compares the local version (`CMS_VERSION`) with the remote version using `version_compare()`.

**Usage Context**
Used to determine if an update should be offered to the administrator.

---

### Update Workflow

#### `start($skip_backup = FALSE)`

**Purpose**
Initiates the full update workflow: backup, download, install, and cleanup.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$skip_backup` | `bool` | `FALSE` | If `TRUE`, skips the backup step. |

**Return Values**
`bool` – `TRUE` if all steps succeed, `FALSE` otherwise.

**Inner Mechanisms**
- Checks the current state to ensure no update is in progress.
- Calls `backup()`, `download()`, `install()`, and `cleanup()` in sequence.
- Sets the state to `CMS_UPDATE_STATUS_DONE` on completion.

**Usage Context**
Called by the administrator to perform a full update.

---

#### `backup()`

**Purpose**
Creates a backup of the current installation, including the database and filesystem.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
1. **Database Backup**: Uses the `mysql` class to dump the database.
2. **Filesystem Backup**:
   - Creates a ZIP archive of the entire installation, excluding cache directories, `.git`, and the update directory.
   - Uses a recursive directory traversal with `opendir()`/`readdir()`.
   - Writes files in chunks to avoid memory exhaustion.
   - Rotates previous backups (renames `backup.zip` to `_backup.zip`).
3. **Progress Tracking**: Calls `progress()` every 100 files to update the log.

**Usage Context**
Called automatically by `start()` unless `$skip_backup` is `TRUE`.

---

#### `download()`

**Purpose**
Downloads the update archive from `CMS_UPDATE_URL_ARCHIVE`.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Uses `stream_context_create()` to set HTTP headers (User-Agent, Authorization).
- Downloads the archive in chunks (8 KB) to `update.zip`.
- Calls `progress()` every 500 chunks to update the log.
- Uses file locks to prevent concurrent access.

**Usage Context**
Called automatically by `start()` after a successful backup.

---

#### `install()`

**Purpose**
Installs the downloaded update archive.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
1. **Extraction**:
   - Opens `update.zip` and extracts only files under the `nuos/` directory to `new/`.
2. **Replacement**:
   - Moves the current installation (`CMS_PATH`) to a timestamped backup directory (`old/`).
   - Moves the extracted files (`new/nuos/`) to `CMS_PATH`.
   - Rolls back if the move fails.
3. **Error Handling**: Logs errors and sets the state to `CMS_UPDATE_STATUS_ERROR` on failure.

**Usage Context**
Called automatically by `start()` after a successful download.

---

#### `cleanup()`

**Purpose**
Removes temporary files and directories after a successful update.

**Parameters**
None.

**Return Values**
`bool` – Always `TRUE`.

**Inner Mechanisms**
- Deletes `update.zip` and the `new/` directory.
- Uses `filemanager_delete()` (if loaded) to recursively delete the `old/` backup directory.

**Usage Context**
Called automatically by `start()` after a successful installation.


<!-- HASH:eefdb73fc340f8d8a731f72c5b36a7b9 -->
