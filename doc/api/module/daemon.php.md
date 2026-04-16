# NUOS API Documentation

[← Index](../README.md) | [`module/daemon.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Daemon Module

The `daemon.php` module implements a task scheduler and executor for the NUOS platform. It processes queued tasks asynchronously in a non-blocking manner, ensuring tasks are executed in the order of their modification time (oldest first). The module uses file-based advisory locking to prevent concurrent execution and manages task lifecycle through file operations.

---

### Overview

The daemon module:
1. Creates a dedicated directory for task storage if it does not exist.
2. Attempts to acquire an exclusive advisory lock to ensure only one instance runs at a time.
3. Scans the task directory for valid, non-empty task files (excluding system files and temporary files).
4. Orders tasks by their last modification time (FIFO).
5. Executes each task in sequence using `register_shutdown_function` to ensure proper cleanup and error handling.
6. Clears or removes task files upon completion.
7. Releases the lock and updates the daemon status upon shutdown.

This module is designed to be invoked via CLI or cron, acting as a lightweight background job processor.

---

### Key Mechanisms

#### File-Based Task Queue
- Tasks are stored as individual files in `CMS_DATA_PATH . "#daemon/"`.
- Filename serves as task identifier.
- File modification time (`filemtime`) determines execution priority.
- Empty or zero-byte files are ignored.

#### Advisory Locking
- A lock file (`daemon.lock`) is used to prevent multiple daemon instances from running simultaneously.
- `flock($hfile, LOCK_EX | LOCK_NB)` attempts to acquire an exclusive, non-blocking lock.
- If lock acquisition fails, the daemon exits immediately.

#### Task Execution
- Each task is loaded via `require()` within a shutdown handler.
- OPCache is invalidated for the task file to ensure fresh execution.
- Time limit is extended to 600 seconds per task.
- Garbage collection is triggered after execution.

#### Task Completion
- Tasks with a modification time of `1` are deleted.
- Other tasks are truncated (emptied) but retained.
- This allows for task persistence or one-time execution based on timestamp.

#### Status Reporting
- `cms_daemon_status()` is called at key points to log daemon activity (start, task completion, shutdown).

---

### Functions and Logic Flow

#### Anonymous Self-Executing Function
```php
(function() { ... })();
```
- Encapsulates the entire daemon logic.
- Runs immediately upon script execution.
- Prevents variable leakage into global scope.

---

### Variables

| Name       | Type      | Description |
|------------|-----------|-------------|
| `$path`    | string    | Absolute path to the daemon task directory (`CMS_DATA_PATH . "#daemon/"`). |
| `$lock`    | string    | Path to the lock file (`$path . "daemon.lock"`). |
| `$hfile`   | resource  | File handle for the lock file. |
| `$flag`    | string    | Path to the "daemon available" flag file (unused in current logic; legacy or future use). |
| `$list`    | array     | Raw directory listing from `scandir()`. |
| `$task`    | array     | Associative array of valid tasks: `filename => filemtime`. |
| `$file`    | string    | Current task filename during iteration. |
| `$_file`   | string    | Full path to the current task file. |
| `$time`    | int       | Modification timestamp of the current task file. |

---

### Inner Workflow

1. **Directory Setup**
   - `mkpath($path)` ensures the task directory exists.
   - If creation fails, the script exits.

2. **Lock Acquisition**
   - Opens `$lock` in `c` (create) mode.
   - Attempts non-blocking exclusive lock.
   - Exits if lock cannot be acquired.

3. **Task Discovery**
   - Scans `$path` and filters out system files (`.`, `..`, `.htaccess`, `daemon.flag`, `daemon.lock`, `daemon.status`).
   - Skips zero-byte files and `.tmp` files.
   - Builds `$task` array with valid files and their `filemtime`.

4. **Task Sorting**
   - `asort($task)` sorts tasks by modification time (ascending), ensuring oldest tasks run first.

5. **Task Registration**
   - For each task, two shutdown functions are registered:
     - **Execution**: Loads and runs the task script; invalidates OPCache; handles exceptions.
     - **Cleanup**: Removes or truncates the task file; logs completion.

6. **Shutdown Sequence**
   - On script termination (normal or error), registered shutdown functions run in reverse order:
     1. Task cleanup.
     2. Task execution.
     3. Lock release and status update.

7. **Lock Release**
   - Updates lock file timestamp.
   - Releases lock with `LOCK_UN`.
   - Closes file handle.
   - Logs completion.

---

### Usage Context

#### When to Use
- For deferred or background processing (e.g., sending emails, generating reports, processing uploads).
- When tasks must run sequentially and without user interaction.
- When task persistence across server restarts is required.

#### How to Use
1. **Enqueue a Task**
   ```php
   $taskData = "<?php\n// Task logic here\n";
   $taskFile = CMS_DATA_PATH . "#daemon/task_" . uniqid() . ".php";
   file_put_contents($taskFile, $taskData);
   touch($taskFile, time() - 3600); // Optional: set older timestamp for priority
   ```

2. **Trigger the Daemon**
   - Invoke `module/daemon.php` via CLI:
     ```bash
     php /path/to/nuos/module/daemon.php
     ```
   - Or via cron (e.g., every 5 minutes):
     ```cron
     */5 * * * * php /path/to/nuos/module/daemon.php
     ```

3. **Monitor Status**
   - Check `CMS_DATA_PATH . "#daemon/daemon.status"` for execution logs.

#### Typical Scenarios
- **Email Queue**: Store email payloads as task files; daemon sends them in order.
- **Image Processing**: Queue image resize or optimization tasks.
- **Data Export**: Generate CSV/Excel reports in the background.
- **Cache Warming**: Preload cache for high-traffic pages.

---

### Error Handling
- Uncaught exceptions in task scripts are re-thrown and will trigger PHP's error handler.
- Fatal errors in task execution may prevent subsequent tasks from running (due to shutdown sequence).
- Locking ensures no race conditions, but task files must be written atomically to avoid corruption.

---

### Notes
- The daemon does **not** run continuously; it processes all available tasks and exits.
- Task files must be valid PHP scripts. Syntax errors will cause the daemon to fail.
- The use of `register_shutdown_function` ensures cleanup even if a task crashes.
- OPCache invalidation is best-effort; may not be available in all environments.
- The daemon is **not** a replacement for a full-fledged job queue (e.g., Redis, RabbitMQ), but is suitable for low-to-medium volume tasks in a zero-dependency environment.


<!-- HASH:c49e193a42f40eab007448f249462b46 -->
