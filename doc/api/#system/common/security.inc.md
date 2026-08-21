# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/security.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/security.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File System Security Utilities (`security.inc`)

This file provides secure wrappers around PHP's native file system functions. Each wrapper enforces path traversal protection and integrates additional security measures such as permission management, `.htaccess` protection for sensitive directories, and automatic cache invalidation. These functions are designed to be drop-in replacements for their native counterparts while mitigating common security risks.

---

## Functions

### `pta_block($path)`

**Purpose:**
Prevents path traversal attacks by blocking any path containing `.` or `..` segments. Terminates script execution if an invalid path is detected.

**Parameters:**

| Name  | Type     | Description                          |
|-------|----------|--------------------------------------|
| $path | `string` | File or directory path to validate.  |

**Return Values:**
- **None** – Script execution halts on invalid paths.

**Inner Mechanisms:**
- Uses `preg_match` to detect `.` or `..` segments at the start, middle, or end of the path.
- Dies with "Invalid path." if a traversal attempt is detected.

**Usage Context:**
- Called internally by all other functions in this file before performing file operations.
- Should **not** be called directly in application code.

**Example:**
```php
// This will terminate the script with "Invalid path."
pta_block("../../../etc/passwd");
```

---

### `chgrp($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `chgrp()`. Changes the group ownership of a file or directory after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file or directory.       |
| ...$args  | `mixed`  | Additional arguments for `chgrp()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Calls `pta_block()` to validate the path.
- Executes native `chgrp()` and clears the stat cache for the file if successful.

**Usage Context:**
- Used when changing group ownership of files or directories in a secure manner.

**Example:**
```php
// Change group ownership of a file to 'www-data'
chgrp("/var/www/html/config.php", "www-data");
```

---

### `chmod($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `chmod()`. Changes file or directory permissions after path validation.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file or directory.       |
| ...$args  | `mixed`  | Additional arguments for `chmod()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `chmod()` and clears the stat cache if successful.

**Usage Context:**
- Used to set file permissions securely, e.g., after file creation or upload.

**Example:**
```php
// Set file permissions to 0644
chmod("/var/www/html/upload.txt", 0644);
```

---

### `chown($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `chown()`. Changes the owner of a file or directory after path validation.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file or directory.       |
| ...$args  | `mixed`  | Additional arguments for `chown()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `chown()` and clears the stat cache if successful.

**Usage Context:**
- Used to change file ownership securely, e.g., after file operations.

**Example:**
```php
// Change ownership of a file to 'www-data'
chown("/var/www/html/temp.log", "www-data");
```

---

### `copy($from, $to, ...$args)`

**Purpose:**
Secure wrapper for PHP's `copy()`. Copies a file from `$from` to `$to` after validating both paths. Automatically sets permissions on the destination file to match the source or a default (0666 for Apache, 0644 otherwise).

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| $from   | `string` | Source file path.                    |
| $to     | `string` | Destination file path.               |
| ...$args| `mixed`  | Additional arguments for `copy()`.   |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates both paths via `pta_block()`.
- Executes native `copy()` and clears the stat cache for the destination.
- Sets permissions on the destination file if the copy succeeds.

**Usage Context:**
- Used for secure file copying, e.g., during asset management or backups.

**Example:**
```php
// Copy a file securely
copy("/var/www/uploads/image.jpg", "/var/www/backups/image_backup.jpg");
```

---

### `rename($from, $to, ...$args)`

**Purpose:**
Secure wrapper for PHP's `rename()`. Renames or moves a file or directory after validating both paths.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| $from   | `string` | Source path.                         |
| $to     | `string` | Destination path.                    |
| ...$args| `mixed`  | Additional arguments for `rename()`. |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates both paths via `pta_block()`.
- Executes native `rename()` and clears the stat cache for both paths if successful.

**Usage Context:**
- Used for secure file/directory renaming or moving, e.g., during file organization.

**Example:**
```php
// Rename a file securely
rename("/var/www/uploads/old_name.txt", "/var/www/uploads/new_name.txt");
```

---

### `move_uploaded_file($from, $to, ...$args)`

**Purpose:**
Secure wrapper for PHP's `move_uploaded_file()`. Moves an uploaded file to a new location after validating both paths. Sets default permissions (0666 for Apache, 0644 otherwise) on the destination file.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| $from   | `string` | Temporary uploaded file path.        |
| $to     | `string` | Destination file path.               |
| ...$args| `mixed`  | Additional arguments for `move_uploaded_file()`. |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates both paths via `pta_block()`.
- Executes native `move_uploaded_file()` and sets permissions on the destination file if successful.

**Usage Context:**
- Used for secure file upload handling, e.g., in user upload forms.

**Example:**
```php
// Move an uploaded file securely
move_uploaded_file($_FILES["file"]["tmp_name"], "/var/www/uploads/user_file.jpg");
```

---

### `file($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `file()`. Reads a file into an array after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `file()`.   |

**Return Values:**
- **`array|false`** – Array of file lines on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `file()`.

**Usage Context:**
- Used for secure file reading, e.g., reading configuration files.

**Example:**
```php
// Read a file securely into an array
$lines = file("/var/www/config/settings.conf");
```

---

### `readfile($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `readfile()`. Outputs a file after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `readfile()`. |

**Return Values:**
- **`int|false`** – Number of bytes read on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `readfile()`.

**Usage Context:**
- Used for secure file output, e.g., serving static assets.

**Example:**
```php
// Output a file securely
readfile("/var/www/assets/logo.png");
```

---

### `file_get_contents($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `file_get_contents()`. Reads a file into a string after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `file_get_contents()`. |

**Return Values:**
- **`string|false`** – File contents on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `file_get_contents()`.

**Usage Context:**
- Used for secure file reading, e.g., reading JSON or XML files.

**Example:**
```php
// Read a file securely into a string
$content = file_get_contents("/var/www/config/config.json");
```

---

### `file_put_contents($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `file_put_contents()`. Writes data to a file after validating the path. Sets default permissions (0666 for Apache, 0644 otherwise) on new files.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `file_put_contents()`. |

**Return Values:**
- **`int|false`** – Number of bytes written on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `file_put_contents()` and clears the stat cache.
- Sets permissions on new files if the write succeeds.

**Usage Context:**
- Used for secure file writing, e.g., logging or configuration updates.

**Example:**
```php
// Write data to a file securely
file_put_contents("/var/www/logs/access.log", "User accessed the system.\n", FILE_APPEND);
```

---

### `opendir($directory, ...$args)`

**Purpose:**
Secure wrapper for PHP's `opendir()`. Opens a directory handle after validating the path.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| $directory | `string` | Path to the directory.               |
| ...$args   | `mixed`  | Additional arguments for `opendir()`. |

**Return Values:**
- **`resource|false`** – Directory handle on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `opendir()`.

**Usage Context:**
- Used for secure directory traversal, e.g., listing files in a directory.

**Example:**
```php
// Open a directory securely
$handle = opendir("/var/www/uploads");
```

---

### `mkdir($directory, ...$args)`

**Purpose:**
Secure wrapper for PHP's `mkdir()`. Creates a directory after validating the path. Supports protected directories (prefixed with `#` or `!`) by creating an `.htaccess` file to deny access. Executable directories (prefixed with `!`) also enable CGI execution.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| $directory | `string` | Path to the directory.               |
| ...$args   | `mixed`  | Additional arguments for `mkdir()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Determines directory type (protected/executable) based on the first character of the basename:
  - `#` or `!`: Protected directory (denies all access via `.htaccess`).
  - `!`: Also enables CGI execution (`Options +ExecCGI`).
- Sets default permissions (0777 for Apache, 0700 for protected directories, 0755 otherwise).
- Creates an `.htaccess` file for protected directories if the directory is created successfully.

**Usage Context:**
- Used for secure directory creation, e.g., during user uploads or cache management.

**Example:**
```php
// Create a protected directory (denies all access)
mkdir("/var/www/uploads/#private");

// Create an executable directory (denies access and enables CGI)
mkdir("/var/www/cgi-bin/!scripts");
```

---

### `rmdir($directory, ...$args)`

**Purpose:**
Secure wrapper for PHP's `rmdir()`. Removes a directory after validating the path.

**Parameters:**

| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| $directory | `string` | Path to the directory.               |
| ...$args   | `mixed`  | Additional arguments for `rmdir()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `rmdir()` and clears the stat cache if successful.

**Usage Context:**
- Used for secure directory removal, e.g., during cleanup tasks.

**Example:**
```php
// Remove a directory securely
rmdir("/var/www/temp/old_cache");
```

---

### `fopen($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `fopen()`. Opens a file or URL after validating the path. Sets default permissions (0666 for Apache, 0644 otherwise) on new files.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `fopen()`.  |

**Return Values:**
- **`resource|false`** – File handle on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `fopen()` and clears the stat cache.
- Sets permissions on new files if the open succeeds.

**Usage Context:**
- Used for secure file operations, e.g., reading/writing files in chunks.

**Example:**
```php
// Open a file securely for writing
$handle = fopen("/var/www/logs/debug.log", "a");
```

---

### `fclose($stream)`

**Purpose:**
Secure wrapper for PHP's `fclose()`. Closes an open file handle and clears the stat cache for the associated file.

**Parameters:**

| Name    | Type       | Description                          |
|---------|------------|--------------------------------------|
| $stream | `resource` | File handle to close.                |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves the file URI from the stream metadata.
- Executes native `fclose()` and clears the stat cache for the file if the URI is local.

**Usage Context:**
- Used to securely close file handles, e.g., after file operations.

**Example:**
```php
// Close a file handle securely
fclose($handle);
```

---

### `touch($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `touch()`. Updates the access/modification time of a file after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `touch()`.  |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `touch()` and clears the stat cache if successful.

**Usage Context:**
- Used to update file timestamps securely, e.g., during cache management.

**Example:**
```php
// Update file timestamps securely
touch("/var/www/cache/data.dat");
```

---

### `unlink($filename, ...$args)`

**Purpose:**
Secure wrapper for PHP's `unlink()`. Deletes a file after validating the path.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $filename | `string` | Path to the file.                    |
| ...$args  | `mixed`  | Additional arguments for `unlink()`. |

**Return Values:**
- **`bool`** – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates path via `pta_block()`.
- Executes native `unlink()` and clears the stat cache if successful.

**Usage Context:**
- Used for secure file deletion, e.g., during cleanup tasks.

**Example:**
```php
// Delete a file securely
unlink("/var/www/temp/old_file.txt");
```


<!-- HASH:c5e46e3991d6b3f5919c56e85b2abd71 -->
