# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/file.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/file.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File System Utilities (`file.inc`)

This file provides core file system operations for the PWNC Web Platform. It includes functions for path manipulation, file I/O, filename sanitization, and file downloads. These utilities ensure safe, consistent, and platform-independent file handling across the entire codebase.

---

## Functions

### `mkpath($path)`
Creates a directory path recursively, similar to `mkdir -p`.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Absolute or relative directory path to create. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the path exists (or was created successfully), `FALSE` on failure or if `$path` is a file. |

#### Inner Mechanisms
1. Checks if the path already exists as a directory.
2. If the path is a file, returns `FALSE`.
3. Strips the `CMS_ROOT_PATH` prefix if present.
4. Splits the path into segments and iteratively creates each directory.
5. Returns `FALSE` if any segment cannot be created.

#### Usage Context
Used when creating nested directory structures (e.g., for user uploads, cache directories, or module assets).

#### Example
```php
// Create a nested directory for user uploads
$uploadPath = CMS_ROOT_PATH . "/uploads/users/123/avatars";
if (mkpath($uploadPath)) {
    // Directory created or already exists
} else {
    // Handle error (e.g., path is a file or permission denied)
}
```

---

### `real_path($path)`
Normalizes a path by resolving `.` and `..` segments, similar to PHP’s `realpath()` but without resolving symlinks.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path to normalize (absolute or relative). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Normalized path with `.` and `..` resolved. Preserves leading/trailing slashes. |

#### Inner Mechanisms
1. Uses regex to split the path into segments, ignoring `.` and empty segments.
2. Processes `..` by removing the last valid segment.
3. Reconstructs the path with leading/trailing slashes preserved.

#### Usage Context
Used to sanitize user-provided paths or resolve relative paths in a consistent way.

#### Example
```php
$normalized = real_path("/var/www/../logs/./app.log");
// Returns "/var/logs/app.log"
```

---

### `resolve_path($source, $target)`
Resolves a target path relative to a source path (e.g., for symlinks or file references).

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$source` | `string` | Base path (file or directory). |
| `$target` | `string` | Target path (absolute or relative). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Resolved absolute path. |

#### Inner Mechanisms
1. If `$target` is absolute, returns it directly.
2. Otherwise, resolves `$source` to an absolute path and processes `$target` relative to it.
3. Handles `.` and `..` segments, preserving trailing slashes for directories.

#### Usage Context
Used to resolve file references in templates, symlinks, or module assets.

#### Example
```php
$resolved = resolve_path("/var/www/index.php", "../assets/image.png");
// Returns "/var/assets/image.png"
```

---

### `read_file($file, $length = NULL, $offset = NULL)`
Reads a file’s contents safely with shared locking.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$file` | `string` | | Path to the file. |
| `$length` | `int\|null` | `NULL` | Maximum bytes to read (`NULL` for entire file). |
| `$offset` | `int\|null` | `NULL` | Starting offset (`NULL` for beginning of file). |

#### Return Values
| Type | Description |
|------|-------------|
| `string\|false` | File contents or `FALSE` on failure. |

#### Inner Mechanisms
1. Opens the file in binary mode with shared lock (`LOCK_SH`).
2. Uses `stream_get_contents()` to read the file.
3. Closes the file handle and releases the lock.

#### Usage Context
Used for reading configuration files, templates, or user uploads.

#### Example
```php
$content = read_file("/var/www/config.json");
if ($content !== FALSE) {
    $config = json_decode($content, TRUE);
}
```

---

### `write_file($file, $data)`
Writes data to a file atomically (avoids partial writes).

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$file` | `string` | Target file path. |
| `$data` | `string` | Data to write. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. Creates parent directories if they don’t exist (`mkpath`).
2. Writes data to a temporary file.
3. Atomically renames the temporary file to the target path.
4. Cleans up the temporary file on failure.

#### Usage Context
Used for writing logs, cache files, or user-generated content.

#### Example
```php
if (write_file("/var/www/cache/data.json", json_encode($data))) {
    // File written successfully
}
```

---

### `ansi_transliteration($string)`
Converts non-ASCII characters to their closest ASCII equivalents (e.g., `ä` → `ae`).

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Input string. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Transliterated string. |

#### Inner Mechanisms
Uses a static lookup table to replace characters (e.g., `ß` → `ss`).

#### Usage Context
Used for filename sanitization or URL slugs.

#### Example
```php
$slug = ansi_transliteration("Café Münster");
// Returns "Cafe Muenster"
```

---

### `stringtofilename($string, $replacement = "-")`
Converts a string to a safe filename by transliterating and replacing invalid characters.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | | Input string. |
| `$replacement` | `string` | `"-"` | Character to replace invalid sequences. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Sanitized filename. |

#### Inner Mechanisms
1. Applies `ansi_transliteration`.
2. Strips leading/trailing invalid characters.
3. Replaces invalid sequences with `$replacement`.
4. Converts to lowercase.

#### Usage Context
Used for user-uploaded files or dynamic asset names.

#### Example
```php
$filename = stringtofilename("My Document: Draft #1.pdf");
// Returns "my-document-draft-1.pdf"
```

---

### `encode_filename($string, $maxlength = 255)`
Encodes a filename to be URL-safe and within a length limit.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | | Input filename. |
| `$maxlength` | `int` | `255` | Maximum filename length (including extension). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Encoded filename. |

#### Inner Mechanisms
1. Encodes invalid characters as `%XX` (URL encoding).
2. Truncates the filename if it exceeds `$maxlength`, appending a CRC32 hash to avoid collisions.
3. Preserves the file extension.

#### Usage Context
Used for storing user-uploaded files in a web-accessible directory.

#### Example
```php
$encoded = encode_filename("My File: Image.jpg");
// Returns "My%20File%3A%20Image.jpg"
```

---

### `decode_filename($string)`
Decodes a filename encoded with `encode_filename`.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Encoded filename. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Decoded filename. |

#### Inner Mechanisms
1. Removes CRC32 hashes (if present).
2. Decodes `%XX` sequences back to their original characters.

#### Usage Context
Used to restore original filenames for downloads or display.

#### Example
```php
$decoded = decode_filename("My%20File%3A%20Image.jpg");
// Returns "My File: Image.jpg"
```

---

### `unique_filename($path, $exception = NULL, $reuse_directory = FALSE)`
Generates a unique filename by appending a counter (e.g., `file-1.txt`).

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$path` | `string` | | Target path. |
| `$exception` | `string\|null` | `NULL` | Path to exclude (e.g., original file). |
| `$reuse_directory` | `bool` | `FALSE` | If `TRUE`, allows reusing existing directories. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Unique path. |

#### Inner Mechanisms
1. If `$path` doesn’t exist, returns it directly.
2. Otherwise, appends `-1`, `-2`, etc., until a unique path is found.
3. Skips `$exception` if provided.

#### Usage Context
Used for file uploads to avoid overwriting existing files.

#### Example
```php
$uniquePath = unique_filename("/uploads/image.jpg");
// Returns "/uploads/image-1.jpg" if "/uploads/image.jpg" exists
```

---

### `pathinfo($path, $flags = PATHINFO_ALL)`
Enhanced version of PHP’s `pathinfo()` that correctly handles directories.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$path` | `string` | | Path to analyze. |
| `$flags` | `int` | `PATHINFO_ALL` | Bitmask of `PATHINFO_*` constants. |

#### Return Values
| Type | Description |
|------|-------------|
| `array\|string` | Associative array (or string if `$flags` is not `PATHINFO_ALL`). |

#### Inner Mechanisms
1. Uses regex to split the path into `dirname`, `basename`, `filename`, and `extension`.
2. Handles edge cases (e.g., paths ending in `.` or `..`).

#### Usage Context
Used for path manipulation in modules or file management.

#### Example
```php
$info = pathinfo("/var/www/index.php");
// Returns ["dirname" => "/var/www", "basename" => "index.php", ...]
```

---

### `basename($path, $suffix = NULL)`
Enhanced version of PHP’s `basename()` with multibyte support.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$path` | `string` | | Path to process. |
| `$suffix` | `string\|null` | `NULL` | Suffix to remove (e.g., `.php`). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Basename of the path. |

#### Inner Mechanisms
Uses regex to extract the last segment of the path.

#### Example
```php
$name = basename("/var/www/index.php", ".php");
// Returns "index"
```

---

### `file_name($path, $extension = FALSE)`
Extracts the filename (without extension) or extension from a path.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$path` | `string` | | Path to analyze. |
| `$extension` | `bool` | `FALSE` | If `TRUE`, returns the extension instead. |

#### Return Values
| Type | Description |
|------|-------------|
| `string\|false` | Filename or extension, or `FALSE` on failure. |

#### Inner Mechanisms
1. Caches results for performance.
2. Uses `pathinfo` for local files or `analyze_url` for remote files.

#### Example
```php
$name = file_name("/var/www/image.png");
// Returns "image"
$ext = file_name("/var/www/image.png", TRUE);
// Returns "png"
```

---

### `file_extension($path)`
Alias for `file_name($path, TRUE)`.

#### Example
```php
$ext = file_extension("/var/www/image.png");
// Returns "png"
```

---

### `file_size($path)`
Gets the size of a local or remote file.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | File path or URL. |

#### Return Values
| Type | Description |
|------|-------------|
| `int\|false` | File size in bytes, or `FALSE` on failure. |

#### Inner Mechanisms
1. For local files, uses `filesize()`.
2. For remote files, checks the `Content-Length` header.

#### Example
```php
$size = file_size("https://example.com/image.jpg");
// Returns the file size in bytes
```

---

### `retrieve_file($source_url, $target_path, $timeout = 60)`
Downloads a remote file to a local path.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$source_url` | `string` | | Remote file URL. |
| `$target_path` | `string` | | Local destination path. |
| `$timeout` | `int` | `60` | Download timeout in seconds. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. Uses a temporary file to avoid partial downloads.
2. Sets a custom `User-Agent` and follows redirects.
3. Renames the temporary file on success.

#### Example
```php
if (retrieve_file("https://example.com/image.jpg", "/var/www/downloads/image.jpg")) {
    // File downloaded successfully
}
```

---

### `download($file, $name = NULL)`
Forces a file download with proper headers and ETag support.

#### Parameters
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$file` | `string` | | Path to the file. |
| `$name` | `string\|null` | `NULL` | Custom filename for the download. |

#### Return Values
| Type | Description |
|------|-------------|
| `int\|false` | Bytes sent on success, `FALSE` on failure. |

#### Inner Mechanisms
1. Checks ETag for caching (returns `304 Not Modified` if unchanged).
2. Sends `Content-Disposition` and `Content-Type` headers.
3. Streams the file in chunks to avoid memory issues.

#### Example
```php
download("/var/www/reports/report.pdf", "Monthly Report.pdf");
```


<!-- HASH:5b967c4487e27b7b2d4a40f0653e41bc -->
