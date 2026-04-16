# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/file.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## File System Utilities (`file.inc`)

Core file system operations for the NUOS platform. Provides functions for path manipulation, file reading, downloading, name sanitization, and remote file retrieval. All functions are namespaced under `cms\`.

---

### `mkpath($path)`

Creates a directory path recursively.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$path`   | `string` | Absolute or relative path to create. Relative paths are resolved from `CMS_ROOT_PATH`. |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the path already exists or was created successfully. `FALSE` on failure. |

**Inner Mechanisms:**
- Checks if the path already exists as a directory.
- Strips `CMS_ROOT_PATH` from the input path to handle relative paths.
- Iterates through each directory segment, creating directories as needed.
- Uses `mkdir()` with default permissions (0777 masked by `umask`).

**Usage Context:**
- Used during module installation, asset uploads, or any operation requiring directory creation.
- Safe to call multiple times on the same path.

---

### `read_file($file)`

Reads the entire contents of a file into a string.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$file`   | `string` | Path to the file to read.                                                   |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | File contents as a binary-safe string.                                      |
| `bool`    | `FALSE` if the file does not exist or cannot be opened.                     |

**Inner Mechanisms:**
- Opens the file in binary mode (`"rb"`).
- Locks the file with `LOCK_SH` to prevent concurrent writes.
- Uses `stream_get_contents()` for efficient reading.
- Closes the file handle before returning.

**Usage Context:**
- Reading configuration files, templates, or static assets.
- Not suitable for very large files (use streaming instead).

---

### `download($file, $name = NULL)`

Forces a file download to the client with proper HTTP headers.

| Parameter | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$file`   | `string`  | Path to the file to download.                                               |
| `$name`   | `string`  | Optional. Custom filename for the download. If empty, uses the original filename. |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | Number of bytes sent on success.                                            |
| `bool`    | `FALSE` if the file does not exist, headers were already sent, or an error occurred. |

**Inner Mechanisms:**
- Validates file existence and header status.
- Disables output buffering to ensure headers are sent first.
- Generates an `ETag` from file modification time and size for caching.
- Sends `304 Not Modified` if the client’s `If-None-Match` matches the `ETag`.
- Streams the file in 512KB chunks to avoid memory issues.
- Sets `Content-Disposition`, `Content-Length`, `Content-Type`, `Last-Modified`, and `ETag` headers.
- Handles Excel files (`xls`) with `utf-16le` charset; others use `utf-8`.

**Usage Context:**
- Serving user-uploaded files, reports, or generated documents.
- Must be called before any output is sent to the browser.

---

### `ansi_transliteration($string)`

Converts common accented and special characters to their ASCII equivalents.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string containing non-ASCII characters.                              |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Transliterated string with accents and special characters replaced.         |

**Inner Mechanisms:**
- Uses a static associative array (`$translation`) mapping Unicode characters to ASCII.
- Applies `strtr()` for efficient character replacement.

**Usage Context:**
- Used internally by `stringtofilename()` and `safe_filename()`.
- Can be used for SEO-friendly URLs or search normalization.

---

### `stringtofilename($string, $replacement = "-")`

Converts a string into a URL-safe filename.

| Parameter      | Type     | Description                                                                 |
|----------------|----------|-----------------------------------------------------------------------------|
| `$string`      | `string` | Input string to convert.                                                    |
| `$replacement` | `string` | Character to replace non-alphanumeric sequences (default: `"-"`).           |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Sanitized filename in lowercase with non-alphanumeric sequences replaced.   |

**Inner Mechanisms:**
- Applies `ansi_transliteration()` to remove accents.
- Uses Unicode-aware regex (`\p{L}`, `\p{N}`) to strip non-letter/number characters.
- Trims leading/trailing separators and converts to lowercase.

**Usage Context:**
- Generating filenames for uploaded assets (images, documents).
- Creating slugs for URLs or database keys.

---

### `basename($path, $suffix = NULL)`

Extracts the filename component from a path, optionally removing a suffix.

| Parameter | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$path`   | `string`  | File path (e.g., `/var/www/file.txt`).                                      |
| `$suffix` | `string`  | Optional. Suffix to remove from the filename (e.g., `.txt`).                |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Filename without path or suffix (if provided).                              |

**Inner Mechanisms:**
- Trims trailing slashes from the path.
- Uses `strrchr()` to find the last path separator.
- Removes the suffix if it matches the end of the filename.

**Usage Context:**
- Extracting filenames from full paths for display or processing.

---

### `file_size($path)`

Retrieves the size of a local or remote file.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$path`   | `string` | Path or URL to the file.                                                    |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | File size in bytes.                                                         |
| `bool`    | `FALSE` if the file does not exist or size cannot be determined.            |

**Inner Mechanisms:**
- For local files, uses `filesize()`.
- For remote files, sends a `HEAD` request and reads `Content-Length` header.

**Usage Context:**
- Validating file uploads or checking remote asset sizes.

---

### `file_name($path, $extension = FALSE)`

Extracts the filename (without extension) or extension from a path.

| Parameter    | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$path`      | `string`  | Path to the file.                                                           |
| `$extension` | `bool`    | If `TRUE`, returns the extension; otherwise, returns the filename.          |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Filename or extension (lowercase).                                          |
| `bool`    | `FALSE` if the path is invalid or cannot be analyzed.                       |

**Inner Mechanisms:**
- Uses a static cache (`$array`) to avoid repeated parsing of the same path.
- For local files, uses `pathinfo()`.
- For remote files, uses `analyze_url()` (custom URL parser).
- Converts extensions to lowercase for consistency.

**Usage Context:**
- Extracting filenames or extensions for display, storage, or MIME type detection.

---

### `file_extension($path)`

Alias for `file_name($path, TRUE)`.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$path`   | `string` | Path to the file.                                                           |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | File extension in lowercase.                                                |
| `bool`    | `FALSE` if the path is invalid.                                             |

---

### `safe_filename($string)`

Generates a filesystem-safe filename with a CRC32 hash suffix for uniqueness.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string to convert.                                                    |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Sanitized filename with a hash suffix (e.g., `my-file-1a2b3c4d`).           |

**Inner Mechanisms:**
- Checks if the string is already safe (only alphanumeric, `.`, or `_`).
- Applies `ansi_transliteration()` and replaces non-safe characters with `_`.
- Trims leading/trailing `.` or `_`.
- Appends a CRC32 hash of the original string for uniqueness.
- Limits the filename to 246 characters (plus hash) to avoid filesystem limits.

**Usage Context:**
- Storing user-uploaded files with predictable but unique names.
- Preventing path traversal or special character issues.

---

### `retrieve_file($source_url, $target_path, $timeout = 60)`

Downloads a file from a remote URL to a local path.

| Parameter      | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$source_url`  | `string`  | URL of the file to download.                                                |
| `$target_path` | `string`  | Local path to save the file.                                                |
| `$timeout`     | `int`     | Timeout in seconds for the download (default: 60).                          |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the download and save were successful. `FALSE` otherwise.         |

**Inner Mechanisms:**
- Uses a temporary file (`.unique_id()`) to avoid partial downloads.
- Creates a stream context with a custom `User-Agent` and timeout.
- Uses `copy()` to download the file.
- Renames the temporary file to the target path on success.
- Cleans up the temporary file on failure.

**Usage Context:**
- Downloading remote assets (e.g., images, libraries) during module installation.
- Caching remote resources locally.


<!-- HASH:501c66f0b2420e6358217de609830601 -->
