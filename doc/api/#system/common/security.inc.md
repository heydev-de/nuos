# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/security.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/security.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Security File System Wrappers

This file provides a security-hardened wrapper layer around PHP's native filesystem functions. All functions reside in the `cms` namespace and intercept potentially dangerous operations by validating paths against directory traversal attacks before delegating to the underlying PHP functions. Additionally, several wrappers apply consistent permission management and integrate with the platform's deferred data system.

### pta_block

Validates a filesystem path to prevent directory traversal attacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | The filesystem path to validate |

**Return Value:** No explicit return value. Terminates execution with `die()` if the path is invalid.

**Inner Mechanism:** Uses a regular expression to detect paths containing `.` or `..` segments that could escape the intended directory structure. The pattern matches these segments at the start, end, or between path separators (`/` or `\`).

**Usage Context:** Called internally by all other wrapper functions before performing any filesystem operation.

```php
// This will terminate execution
pta_block("../secret/config.php");

// This is valid
pta_block("uploads/image.jpg");
```

### chgrp

Wrapper for PHP's native `chgrp()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file or directory |
| `...$args` | mixed | Additional arguments passed to native `chgrp()` |

**Return Value:** `bool` — Result of the native `chgrp()` call.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\chgrp()`. On success, clears the stat cache for the affected file.

**Usage Context:** Changing group ownership of files within the application's controlled directories.

```php
chgrp("uploads/document.pdf", "www-data");
```

### chmod

Wrapper for PHP's native `chmod()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file or directory |
| `...$args` | mixed | Additional arguments passed to native `chmod()` |

**Return Value:** `bool` — Result of the native `chmod()` call.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\chmod()`. On success, clears the stat cache.

**Usage Context:** Modifying file permissions after ensuring the path is safe.

```php
chmod("config/settings.php", 0644);
```

### chown

Wrapper for PHP's native `chown()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file or directory |
| `...$args` | mixed | Additional arguments passed to native `chown()` |

**Return Value:** `bool` — Result of the native `chown()` call.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\chown()`. On success, clears the stat cache.

**Usage Context:** Changing file ownership within controlled paths.

```php
chown("uploads/file.txt", "www-data");
```

### copy

Wrapper for PHP's native `copy()` function with path validation and permission management.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$from` | string | Source file path |
| `$to` | string | Destination file path |
| `...$args` | mixed | Additional arguments passed to native `copy()` |

**Return Value:** `bool` — Result of the native `copy()` call.

**Inner Mechanism:**
1. Validates both source and destination paths with `pta_block()`
2. If the `data` class exists, applies any deferred data associated with the source file
3. Delegates to `\copy()`
4. On success:
   - Clears stat cache for the destination
   - Discards any deferred data for the replaced destination file
   - Sets appropriate permissions (preserving source file permissions or using defaults based on `CMS_APACHE`)

**Usage Context:** Copying files while maintaining security and proper permission handling.

```php
copy("templates/default.html", "cache/rendered.html");
```

### rename

Wrapper for PHP's native `rename()` function with path validation and deferred data relocation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$from` | string | Original file or directory path |
| `$to` | string | New file or directory path |
| `...$args` | mixed | Additional arguments passed to native `rename()` |

**Return Value:** `bool` — Result of the native `rename()` call.

**Inner Mechanism:**
1. Validates both paths with `pta_block()`
2. If the `data` class exists, applies deferred data for the source (or NULL for directories)
3. On Windows, implements a retry mechanism (up to 10 attempts with 20ms delays) to handle file locking issues
4. Delegates to `\rename()`
5. On success:
   - Clears stat cache for both paths
   - Relocates deferred data cache from old to new path

**Usage Context:** Moving or renaming files and directories with cross-platform compatibility.

```php
rename("temp/upload.tmp", "uploads/final.jpg");
```

### move_uploaded_file

Wrapper for PHP's native `move_uploaded_file()` function with path validation and MCP support.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$from` | string | Temporary uploaded file path |
| `$to` | string | Destination file path |
| `...$args` | mixed | Additional arguments passed to native `move_uploaded_file()` |

**Return Value:** `bool` — Result of the file move operation.

**Inner Mechanism:**
1. Validates both paths with `pta_block()`
2. If MCP (Multi-Client Platform) is active and the source is a tracked temporary file, uses `rename()` instead and cleans up the temp tracking
3. Otherwise delegates to `\move_uploaded_file()`
4. On success:
   - Sets permissions (0666 for Apache, 0644 otherwise)
   - Clears stat cache

**Usage Context:** Securely handling uploaded files in both standard and MCP environments.

```php
move_uploaded_file($_FILES['avatar']['tmp_name'], "uploads/avatar.jpg");
```

### file

Wrapper for PHP's native `file()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to read |
| `...$args` | mixed | Additional arguments passed to native `file()` |

**Return Value:** `array|false` — Array of file lines or `false` on failure.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\file()`.

**Usage Context:** Reading file contents into an array with security validation.

```php
$lines = file("logs/app.log");
```

### readfile

Wrapper for PHP's native `readfile()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to read |
| `...$args` | mixed | Additional arguments passed to native `readfile()` |

**Return Value:** `int|false` — Number of bytes read or `false` on failure.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\readfile()`.

**Usage Context:** Outputting file contents directly to the response stream.

```php
readfile("downloads/manual.pdf");
```

### file_get_contents

Wrapper for PHP's native `file_get_contents()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to read |
| `...$args` | mixed | Additional arguments passed to native `file_get_contents()` |

**Return Value:** `string|false` — File contents as a string or `false` on failure.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\file_get_contents()`.

**Usage Context:** Reading entire file contents into a string with security validation.

```php
$config = file_get_contents("config/database.json");
```

### file_put_contents

Wrapper for PHP's native `file_put_contents()` function with path validation and permission management.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to write |
| `...$args` | mixed | Additional arguments passed to native `file_put_contents()` |

**Return Value:** `int|false` — Number of bytes written or `false` on failure.

**Inner Mechanism:**
1. Validates the path with `pta_block()`
2. Tracks whether the file is new (doesn't exist yet)
3. Delegates to `\file_put_contents()`
4. On success:
   - Clears stat cache
   - If the file was newly created, sets appropriate permissions (0666 for Apache, 0644 otherwise)

**Usage Context:** Writing data to files with automatic permission management for new files.

```php
file_put_contents("cache/data.json", json_encode($data));
```

### opendir

Wrapper for PHP's native `opendir()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$directory` | string | Path to the directory to open |
| `...$args` | mixed | Additional arguments passed to native `opendir()` |

**Return Value:** `resource|false` — Directory handle or `false` on failure.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\opendir()`.

**Usage Context:** Opening directory handles for iteration with security validation.

```php
$dir = opendir("uploads/");
```

### mkdir

Wrapper for PHP's native `mkdir()` function with path validation, permission management, and directory protection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$directory` | string | Path to the directory to create |
| `...$args` | mixed | Additional arguments passed to native `mkdir()` |

**Return Value:** `bool` — Result of the native `mkdir()` call.

**Inner Mechanism:**
1. Validates the path with `pta_block()`
2. Examines the directory basename's first character:
   - `!` prefix: Executable directory (CGI execution enabled)
   - `#` prefix: Protected directory (access denied via .htaccess)
3. Sets appropriate permissions based on `CMS_APACHE` flag and protection level
4. Delegates to `\mkdir()`
5. On success:
   - Clears stat cache
   - For protected directories, creates a `.htaccess` file with access restrictions
   - For executable directories, adds `Options +ExecCGI` to the `.htaccess`

**Usage Context:** Creating directories with automatic security configuration.

```php
// Create a protected directory
mkdir("private/uploads");

// Create an executable directory
mkdir("!cgi-bin/scripts");
```

### rmdir

Wrapper for PHP's native `rmdir()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$directory` | string | Path to the directory to remove |
| `...$args` | mixed | Additional arguments passed to native `rmdir()` |

**Return Value:** `bool` — Result of the native `rmdir()` call.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\rmdir()`. On success, clears the stat cache.

**Usage Context:** Removing empty directories with security validation.

```php
rmdir("temp/cache");
```

### fopen

Wrapper for PHP's native `fopen()` function with path validation and permission management.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to open |
| `...$args` | mixed | Additional arguments passed to native `fopen()` |

**Return Value:** `resource|false` — File handle or `false` on failure.

**Inner Mechanism:**
1. Validates the path with `pta_block()`
2. Tracks whether the file is new
3. Delegates to `\fopen()`
4. On success:
   - Clears stat cache
   - If the file was newly created, sets appropriate permissions (0666 for Apache, 0644 otherwise)

**Usage Context:** Opening file handles with automatic permission management for new files.

```php
$handle = fopen("logs/app.log", "a");
```

### fclose

Wrapper for PHP's native `fclose()` function with stat cache management.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$stream` | resource | File handle to close |

**Return Value:** `bool` — Result of the native `fclose()` call.

**Inner Mechanism:**
1. Retrieves stream metadata before closing
2. Delegates to `\fclose()`
3. On success, if the stream URI is a local path (no scheme), clears the stat cache for that path

**Usage Context:** Properly closing file handles while maintaining accurate filesystem metadata.

```php
fclose($handle);
```

### touch

Wrapper for PHP's native `touch()` function with path validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to touch |
| `...$args` | mixed | Additional arguments passed to native `touch()` |

**Return Value:** `bool` — Result of the native `touch()` call.

**Inner Mechanism:** Validates the path with `pta_block()`, then delegates to `\touch()`. On success, clears the stat cache.

**Usage Context:** Setting file access and modification times with security validation.

```php
touch("cache/last_run");
```

### unlink

Wrapper for PHP's native `unlink()` function with path validation and deferred data cleanup.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$filename` | string | Path to the file to delete |
| `...$args` | mixed | Additional arguments passed to native `unlink()` |

**Return Value:** `bool` — Result of the native `unlink()` call.

**Inner Mechanism:**
1. Validates the path with `pta_block()`
2. Delegates to `\unlink()`
3. On success:
   - Clears stat cache
   - If the `data` class exists, discards any deferred data associated with the file

**Usage Context:** Deleting files with security validation and cleanup of associated deferred data.

```php
unlink("temp/session_data.tmp");
```>


<!-- HASH:9ff7170348974a05978208897e135ec0 -->
