# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/ice.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## File System Security Wrapper (`ice.inc`)

This file provides secure wrappers around PHP's native file system functions. All functions enforce path validation to prevent directory traversal attacks before delegating to the original PHP functions. Additional security measures include automatic permission handling and `.htaccess` protection for sensitive directories.

---

### Core Security Function

#### `ice_check_path($path)`
Validates a file system path to block directory traversal attempts.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$path`   | `string` | File or directory path to validate.  |

**Mechanism:**
- Uses regex to detect sequences of `.` or `..` followed or preceded by directory separators (`/`, `\`).
- Terminates script execution (`exit()`) if traversal patterns are detected.

**Usage Context:**
- Called internally by all file system wrappers before any operation.
- Should not be used directly in application code.

---

### File System Operation Wrappers

All wrappers follow a consistent pattern:
1. Validate paths via `ice_check_path()`.
2. Delegate to the native PHP function.
3. Apply additional security measures (e.g., permissions, `.htaccess`).

#### `chgrp($filename, $group, ...$args)`
Changes group ownership of a file/directory.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | Target file/directory.               |
| `$group`    | `string` | Group name or ID.                    |
| `...$args`  | `mixed`  | Additional arguments for `chgrp()`.  |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `chmod($filename, $mode, ...$args)`
Changes file/directory permissions.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | Target file/directory.               |
| `$mode`     | `int`    | Permission mode (e.g., `0755`).      |
| `...$args`  | `mixed`  | Additional arguments for `chmod()`.  |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `chown($filename, $user, ...$args)`
Changes owner of a file/directory.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | Target file/directory.               |
| `$user`     | `string` | User name or ID.                     |
| `...$args`  | `mixed`  | Additional arguments for `chown()`.  |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `copy($source, $dest, ...$args)`
Copies a file while preserving permissions.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$source`   | `string` | Source file path.                    |
| `$dest`     | `string` | Destination file path.               |
| `...$args`  | `mixed`  | Additional arguments for `copy()`.   |

**Mechanism:**
- Copies permissions from the source file to the destination.
- Falls back to `0666` (Apache) or `0644` (CGI) if source permissions are unavailable.

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `file_get_contents($filename, ...$args)`
Reads a file into a string.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `...$args`  | `mixed`  | Additional arguments for `file_get_contents()`. |

**Return:**
- `string|false`: File contents or `FALSE` on failure.

---

#### `file_put_contents($filename, $data, ...$args)`
Writes data to a file with secure permissions.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `$data`     | `mixed`  | Data to write.                       |
| `...$args`  | `mixed`  | Additional arguments for `file_put_contents()`. |

**Mechanism:**
- Sets permissions to `0666` (Apache) or `0644` (CGI) for new files.

**Return:**
- `int|false`: Number of bytes written or `FALSE` on failure.

---

#### `file($filename, ...$args)`
Reads a file into an array of lines.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `...$args`  | `mixed`  | Additional arguments for `file()`.   |

**Return:**
- `array|false`: Array of file lines or `FALSE` on failure.

---

#### `fopen($filename, $mode, ...$args)`
Opens a file or URL with secure permissions.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `$mode`     | `string` | File mode (e.g., `'r'`, `'w'`).      |
| `...$args`  | `mixed`  | Additional arguments for `fopen()`.  |

**Mechanism:**
- Sets permissions to `0666` (Apache) or `0644` (CGI) for new files.

**Return:**
- `resource|false`: File handle or `FALSE` on failure.

---

#### `mkdir($pathname, ...$args)`
Creates a directory with secure permissions and optional `.htaccess` protection.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$pathname` | `string` | Directory path.                      |
| `...$args`  | `mixed`  | Additional arguments for `mkdir()`.  |

**Mechanism:**
- **Permissions:**
  - `0777` (Apache) or `0755` (CGI) by default.
  - `0700` for protected directories (names starting with `#` or `!`).
- **`.htaccess`:**
  - Blocks all access (`Deny from all`) for protected directories.
  - Enables `ExecCGI` for directories starting with `!`.

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `move_uploaded_file($filename, $destination, ...$args)`
Moves an uploaded file with secure permissions.

| Parameter      | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$filename`    | `string` | Temporary file path.                 |
| `$destination` | `string` | Destination file path.               |
| `...$args`     | `mixed`  | Additional arguments for `move_uploaded_file()`. |

**Mechanism:**
- Sets permissions to `0666` (Apache) or `0644` (CGI).

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `opendir($path, ...$args)`
Opens a directory handle.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$path`   | `string` | Directory path.                      |
| `...$args`| `mixed`  | Additional arguments for `opendir()`.|

**Return:**
- `resource|false`: Directory handle or `FALSE` on failure.

---

#### `readfile($filename, ...$args)`
Outputs a file directly to the browser.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `...$args`  | `mixed`  | Additional arguments for `readfile()`. |

**Return:**
- `int|false`: Number of bytes read or `FALSE` on failure.

---

#### `rename($oldname, $newname, ...$args)`
Renames a file/directory.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$oldname`  | `string` | Current file/directory path.         |
| `$newname`  | `string` | New file/directory path.             |
| `...$args`  | `mixed`  | Additional arguments for `rename()`. |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `rmdir($dirname, ...$args)`
Removes an empty directory.

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$dirname` | `string` | Directory path.                      |
| `...$args` | `mixed`  | Additional arguments for `rmdir()`.  |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `unlink($filename, ...$args)`
Deletes a file.

| Parameter   | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$filename` | `string` | File path.                           |
| `...$args`  | `mixed`  | Additional arguments for `unlink()`. |

**Return:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

### Constants

| Name         | Value       | Description                          |
|--------------|-------------|--------------------------------------|
| `CMS_APACHE` | `bool`      | `TRUE` if running as Apache module.  |

**Usage Context:**
- Determines default permissions for files/directories.
- Automatically set by the NUOS platform.


<!-- HASH:d202a819fe321803c3835f117f3d4f69 -->
