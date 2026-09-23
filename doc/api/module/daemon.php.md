# PWNC API Documentation

[← Index](../README.md) | [`module/daemon.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/daemon.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Daemon Task Processor

The `module/daemon.php` file implements a background task processing daemon for the PWNC Web Platform. It scans a designated directory for pending task scripts, acquires exclusive locks to prevent concurrent execution, and processes each task sequentially. The daemon ensures only one instance runs at a time using advisory file locking, handles errors gracefully, and maintains status logs throughout its lifecycle.

### Key Features

- **Single Instance Enforcement**: Uses advisory file locking (`flock`) to ensure only one daemon process runs at a time.
- **Task Discovery**: Scans the daemon data directory for task files, filtering out system files and lock files.
- **Prioritization**: Orders tasks by modification time, processing older tasks first.
- **Error Handling**: Catches exceptions during task execution and logs them appropriately.
- **Resource Management**: Invalidates OPcache entries, sets time limits, and performs garbage collection before executing tasks.
- **Cleanup**: Removes or empties processed task files and releases all locks upon completion.

### Constants and Paths

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DATA_PATH` | (Defined elsewhere) | Base path for CMS data storage |
| `$path` | `CMS_DATA_PATH . "#daemon/"` | Directory containing daemon task files |
| `$lock` | `$path . "daemon.lock"` | Lock file for single-instance enforcement |
| `$flag` | `$path . "daemon.flag"` | Flag file indicating task availability |

### Execution Flow

1. Creates the daemon directory if it doesn't exist.
2. Attempts to acquire an exclusive lock on the daemon lock file.
3. Removes the task availability flag if present.
4. Scans the directory for valid task files.
5. Sorts tasks by modification time (oldest first).
6. For each task:
   - Registers a shutdown function to execute the task.
   - Registers a shutdown function to clean up after the task.
7. Registers a final shutdown function to release the daemon lock.
8. Exits immediately after registering all shutdown functions.

### Usage Example

Tasks are placed in the daemon directory as PHP scripts. For example, a task file named `send_emails.php` might contain:

```php
<?php
// Send queued emails
$mailer = new EmailQueue();
$mailer->process();
```

When the daemon runs, it will discover this file, acquire a lock on it, invalidate any cached version, and execute it. If the script throws an exception, the error is logged via `cms_error()` and the daemon continues with the next task.

### Internal Mechanisms

#### Lock Acquisition

```php
$hfile = fopen($lock, "c");
if (! flock($hfile, LOCK_EX | LOCK_NB)) {
    fclose($hfile);
    exit();
}
```

The daemon opens the lock file in create mode (`"c"`) and attempts a non-blocking exclusive lock. If another process holds the lock, the daemon exits immediately.

#### Task Filtering

```php
$list = array_diff($list, [".", "..", ".htaccess", "daemon.flag", "daemon.lock", "daemon.status"]);
```

System files and daemon-specific files are excluded from the task list. Additionally, files with `.lock` or `.tmp` extensions, or zero-byte files, are skipped.

#### Task Execution

Each task is executed within a shutdown function that:
1. Acquires a per-task lock to prevent concurrent execution of the same task.
2. Invalidates OPcache for the task file.
3. Sets a 600-second time limit.
4. Performs garbage collection.
5. Includes and executes the task script.
6. Catches any `Throwable` exceptions and logs them.

#### Cleanup

After task execution, another shutdown function:
1. Removes the task file if its modification time is `1` (indicating it should be deleted).
2. Empties the task file otherwise.
3. Releases the per-task lock.
4. Logs success or failure status.

### Helper Functions Used

| Function | Purpose |
|----------|---------|
| `mkpath($path)` | Recursively creates directories |
| `blank($var)` | Checks if a variable is empty |
| `cms_daemon_status($msg)` | Logs daemon status messages |
| `cms_error(...)` | Logs errors with context information |

### Typical Scenarios

- **Scheduled Tasks**: Running periodic maintenance scripts like log rotation or cache cleanup.
- **Queue Processing**: Processing email queues, image processing jobs, or data synchronization tasks.
- **Background Jobs**: Performing long-running operations without blocking web requests.

The daemon is typically invoked via a cron job or system service that periodically executes this script, allowing the web application to offload heavy processing to the background.


<!-- HASH:f94b265f87227717fd7142bace41ec10 -->
