# PWNC API Documentation

[← Index](../README.md) | [`module/daemon.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/daemon.php)

- **Version:** `26.8.11.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Daemon Module

The Daemon module is a background task processor for the PWNC Web Platform. It manages and executes queued tasks stored as files in a designated directory, ensuring sequential processing based on modification time. The module implements advisory file locking to prevent concurrent execution and provides status updates during task processing.

### Core Functionality

The module operates as a self-contained anonymous function that:

1. Creates a dedicated task directory if it does not exist
2. Acquires an exclusive advisory lock to prevent multiple daemon instances
3. Scans the task directory for valid task files
4. Processes tasks in order of their modification time (oldest first)
5. Handles task execution, error reporting, and cleanup
6. Releases all locks upon completion

---

### Constants and Paths

| Name | Value | Description |
|------|-------|-------------|
| `$path` | `CMS_DATA_PATH . "#daemon/"` | Base directory for daemon tasks and control files |
| `$lock` | `$path . "daemon.lock"` | Advisory lock file path |
| `$flag` | `$path . "daemon.flag"` | Availability flag file (removed during execution) |

---

### Task Processing Flow

#### 1. **Directory and Lock Initialization**
- Creates the task directory if missing using `mkpath()`
- Opens and locks the daemon lock file (`daemon.lock`) in non-blocking exclusive mode
- Removes the availability flag (`daemon.flag`) to signal the daemon is active

#### 2. **Task Discovery**
- Scans the task directory using `scandir()`
- Filters out system files (`.`, `..`, `.htaccess`) and control files (`daemon.flag`, `daemon.lock`, `daemon.status`)
- Skips files with:
  - Zero size
  - `.lock` extension (indicating active processing)
  - `.tmp` extension (temporary files)
- Collects valid task files with their modification times

#### 3. **Task Prioritization**
- Sorts tasks by modification time (ascending) using `asort()`
- Ensures oldest tasks are processed first

#### 4. **Task Execution**
- For each task, registers two shutdown functions:
  - **Pre-execution**: Acquires a task-specific lock, invalidates OPcache, and includes the task script
  - **Post-execution**: Clears or truncates the task file, releases the lock, and logs status

#### 5. **Error Handling**
- Catches exceptions during task execution
- Logs errors via `cms_error()` and stores messages in `$error`
- Reports task failures via `cms_daemon_status()`

#### 6. **Cleanup**
- Releases the daemon lock and updates the status on shutdown
- Ensures all file handles are closed and temporary files are removed

---

### Key Mechanisms

#### Advisory Locking
- Uses `flock()` with `LOCK_EX | LOCK_NB` to prevent concurrent access
- Locks are held for the daemon (`daemon.lock`) and individual tasks (`<task>.lock`)
- Locks are released via shutdown functions to ensure cleanup even if errors occur

#### Task Lifecycle
- **Pending**: Task file exists with content
- **Active**: Task file is locked (`.lock` file present)
- **Completed**: Task file is truncated or deleted
- **Failed**: Error message is logged, and status is updated

#### OPcache Invalidation
- Uses `opcache_invalidate()` (if available) to ensure task scripts are reloaded
- Prevents stale bytecode from affecting task execution

#### Resource Management
- Calls `gc_collect_cycles()` to minimize memory usage
- Sets a 600-second timeout per task via `set_time_limit()`

---

### Usage Context

#### When to Use
- **Background Processing**: Offload long-running or non-critical tasks (e.g., report generation, data exports)
- **Scheduled Jobs**: Execute tasks at specific intervals (e.g., cleanup, backups)
- **Decoupled Workflows**: Process user-submitted jobs without blocking the main application

#### How to Queue a Task
1. Write a PHP script containing the task logic
2. Save the script to `$path` (e.g., `CMS_DATA_PATH . "#daemon/my_task.php"`)
3. The daemon will automatically detect and execute the task on its next run

---

### Example: Queueing a Task

#### Task Script (`my_task.php`)
```php
<?php
// Example: Generate a report and email it
require("../pwnc.inc");

$data = cms_db_query("SELECT * FROM users WHERE last_login > NOW() - INTERVAL 30 DAY");
$report = "Active Users Report\n\n";
foreach ($data as $user) {
    $report .= "- {$user['username']} (Last Login: {$user['last_login']})\n";
}

cms_mail("admin@example.com", "Active Users Report", $report);
cms_daemon_status("Report generated and sent.");
```

#### Queueing the Task
```php
<?php
// In your application code:
$task_path = CMS_DATA_PATH . "#daemon/my_task_" . time() . ".php";
file_put_contents($task_path, file_get_contents("my_task.php"));
```

#### Explanation
1. The task script queries active users and sends an email report.
2. The application queues the task by copying the script to the daemon directory.
3. The daemon processes the task in the background, logging status updates.
4. The task file is automatically removed upon completion.

---

### Error Handling and Debugging

#### Common Issues
| Issue | Cause | Solution |
|-------|-------|----------|
| Task not executed | Daemon not running or locked | Check `daemon.lock` and `daemon.status` |
| Task stuck | Lock file not released | Manually delete `<task>.lock` |
| OPcache issues | Stale bytecode | Restart PHP or disable OPcache for the daemon directory |

#### Debugging Tools
- **Status Log**: Monitor `daemon.status` for execution updates
- **Error Log**: Check `cms_error()` output for task-specific errors
- **Lock Files**: Verify no orphaned `.lock` files exist

---

### Integration with PWNC Utilities

| Utility | Purpose |
|---------|---------|
| `cms_daemon_status()` | Logs daemon and task status updates |
| `cms_error()` | Reports errors during task execution |
| `cms_db_query()` | Database access (example usage) |
| `cms_mail()` | Email functionality (example usage) |


<!-- HASH:f94b265f87227717fd7142bace41ec10 -->
