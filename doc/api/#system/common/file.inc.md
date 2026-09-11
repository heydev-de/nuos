# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/file.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/file.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Overview

The file `#system/common/file.inc` is part of the PWNC Web Platform's core system utilities. It provides a comprehensive set of functions for handling filesystem operations, including path manipulation, file reading/writing, filename encoding/decoding, and HTTP file downloads. These functions are designed to be robust, secure, and compatible with both local and remote resources.

Key features include:
- Path normalization and resolution
- Safe file operations with atomic writes
- Filename sanitization for cross-platform compatibility
- HTTP file downloading with proper headers and caching support

---

## Functions

### mkpath

Creates a directory path recursively, ensuring all parent directories exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | The directory path to create |

**Return Value:** `bool` - TRUE on success, FALSE if the path already exists as a file or creation fails.

**Inner Mechanisms:**
1. Checks if the path is already a directory (returns TRUE)
2. Checks if the path exists as a file (returns FALSE)
3. Strips the CMS root path prefix if present
4. Iterates through path segments, creating directories as needed
5. Returns FALSE if any directory creation fails

**Usage Context:** Used when preparing directory structures for file storage, such as uploading assets or creating cache directories.

```php
// Create a nested directory structure
if (mkpath("/var/www/uploads/2023/12")) {
    echo "Directory created successfully";
} else {
    echo "Failed to create directory";
}
```

---

### real_path

Normalizes a filesystem path by resolving `.` and `..` references.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | The path to normalize |

**Return Value:** `string` - The normalized path with resolved relative references.

**Inner Mechanisms:**
1. Preserves leading/trailing slashes
2. Splits the path using a regex that handles both `/` and `\` separators
3. Processes each segment:
   - Empty or `.` segments are ignored
   - `..` segments remove the last element from the result
   - Other segments are added to the result
4. Reassembles the path with preserved prefix/suffix

**Usage Context:** Useful for sanitizing user-provided paths or comparing paths that may contain relative references.

```php
// Normalize a path with relative references
$normalized = real_path("/var/www/../uploads/./images");
// Result: "/var/uploads/images"
```

---

### resolve_path

Resolves a target path relative to a source path, handling both absolute and relative targets.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | string | The base path for resolution |
| `$target` | string | The path to resolve |

**Return Value:** `string` - The resolved path.

**Inner Mechanisms:**
1. If target is absolute (starts with `/` or `\`), uses it directly
2. If target is relative, normalizes the source path first
3. Determines if source is a directory based on trailing slash or `.`/`..` references
4. Splits both paths into segments
5. Processes target segments:
   - `..` removes the last segment from source
   - Other segments are appended to source
6. Preserves trailing slash if target ends with one

**Usage Context:** Essential for resolving include paths, template references, or any situation where paths need to be resolved relative to a base location.

```php
// Resolve a relative path
$resolved = resolve_path("/var/www/templates", "../uploads/image.jpg");
// Result: "/var/uploads/image.jpg"

// Resolve an absolute path
$resolved = resolve_path("/var/www/templates", "/etc/config.ini");
// Result: "/etc/config.ini"
```

---

### read_file

Reads the entire contents of a file with shared locking.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$file` | string | - | Path to the file to read |
| `$length` | int\|NULL | NULL | Maximum length to read (NULL = entire file) |
| `$offset` | int\|NULL | NULL | Starting offset (NULL = beginning) |

**Return Value:** `string\|bool` - File contents on success, FALSE on failure.

**Inner Mechanisms:**
1. Verifies the file exists and is a regular file
2. Opens the file in binary read mode
3. Acquires a shared lock (LOCK_SH)
4. Reads content using stream_get_contents with optional length/offset
5. Closes the file handle
6. Returns FALSE if any step fails

**Usage Context:** Reading configuration files, templates, or any file content that needs to be processed by the application.

```php
// Read a configuration file
$config = read_file("/etc/app/config.json");
if ($config !== FALSE) {
    $settings = json_decode($config, true);
}
```

---

### write_file

Writes data to a file atomically using a temporary file and rename operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$file` | string | Path to the file to write |
| `$data` | string | Data to write to the file |

**Return Value:** `bool` - TRUE on success, FALSE on failure.

**Inner Mechanisms:**
1. Creates the parent directory structure using mkpath
2. Generates a unique temporary filename in the same directory
3. Writes data to the temp file with exclusive locking (LOCK_EX)
4. Atomically renames the temp file to the target filename
5. Cleans up the temp file if rename fails
6. Returns FALSE if any step fails

**Usage Context:** Writing configuration files, saving user data, or any file operation where data integrity is important.

```php
// Write JSON data to a file
$data = json_encode(["user" => "john", "role" => "admin"]);
if (write_file("/var/www/data/users.json", $data)) {
    echo "Data saved successfully";
}
```

---

### ansi_transliteration

Converts non-ASCII characters to their closest ASCII equivalents.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | The string to transliterate |

**Return Value:** `string` - The transliterated string with ASCII characters only.

**Inner Mechanisms:**
1. Uses a static lookup table mapping accented characters to ASCII equivalents
2. Applies strtr() for efficient character replacement
3. Handles common European language characters (German umlauts, French accents, etc.)

**Usage Context:** Preparing strings for use in filenames, URLs, or other contexts that require ASCII-only characters.

```php
// Convert accented characters
$ascii = ansi_transliteration("Café résumé naïve");
// Result: "Cafe resume naive"
```

---

### stringtofilename

Converts a string to a safe, lowercase filename.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$string` | string | - | The string to convert |
| `$replacement` | string | "-" | Character to replace non-alphanumeric sequences |

**Return Value:** `string` - A sanitized filename string.

**Inner Mechanisms:**
1. Applies ANSI transliteration to convert accented characters
2. Uses regex to:
   - Strip leading non-alphanumeric characters
   - Strip trailing non-alphanumeric characters
   - Replace internal non-alphanumeric sequences with the replacement character
3. Converts the result to lowercase using utf8_strtolower

**Usage Context:** Generating filenames from user input, titles, or other arbitrary strings.

```php
// Convert a title to a filename
$filename = stringtofilename("My Great Article!");
// Result: "my-great-article"
```

---

### encode_filename

Encodes a string to be safe for use as a filename across different operating systems.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$string` | string | - | The string to encode |
| `$maxlength` | int | 255 | Maximum length of the resulting filename |

**Return Value:** `string` - An encoded filename safe for cross-platform use.

**Inner Mechanisms:**
1. Sets maximum length constraints
2. Defines regex patterns for:
   - Invalid Windows filenames (aux, com1-9, con, lpt1-9, nul, prn)
   - Non-safe characters (everything except space, dash, underscore, alphanumeric, and Unicode)
   - Any remaining characters
3. Uses a callback function to process each match:
   - Percent-encodes unsafe characters
   - URL-encodes special characters using rawurlencode
   - Tracks total length and truncates if necessary
4. Handles file extensions separately to preserve them
5. Appends a CRC32 hash if truncation occurs

**Usage Context:** Creating filenames from user uploads, generating safe filenames for cross-platform compatibility.

```php
// Encode a filename with special characters
$safe_name = encode_filename("My File (1).txt");
// Result: "My-File-1.txt"

// Encode with length limit
$safe_name = encode_filename("Very long filename that exceeds limits.txt", 20);
// Result: "Very long fil[xxxxxxxx].txt" (where xxxxxxxx is a hash)
```

---

### decode_filename

Decodes a filename previously encoded with encode_filename.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | The encoded filename to decode |

**Return Value:** `string` - The decoded filename.

**Inner Mechanisms:**
1. Removes any appended hash markers ([xxxxxxxx]) that were added during encoding
2. Decodes percent-encoded sequences back to their original characters
3. Returns the decoded string

**Usage Context:** Retrieving original filenames after they've been stored in an encoded format.

```php
// Decode a previously encoded filename
$original = decode_filename("My-File-1.txt");
// Result: "My File (1).txt"
```

---

### unique_filename

Generates a unique filename by appending a counter if the file already exists.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$path` | string | - | The desired file path |
| `$exception` | string\|NULL | NULL | A path to exclude from uniqueness checking |
| `$reuse_directory` | bool | FALSE | Whether to reuse existing directories |

**Return Value:** `string` - A unique file path.

**Inner Mechanisms:**
1. Returns the original path if it doesn't exist
2. Returns the original path if it matches the exception
3. For directories:
   - Optionally reuses existing directories if $reuse_directory is TRUE
   - Appends "-1", "-2", etc. until a unique name is found
4. For files:
   - Preserves the extension
   - Appends "-1", "-2", etc. before the extension until unique

**Usage Context:** Preventing file overwrites when saving user uploads or generated files.

```php
// Get a unique filename
$unique = unique_filename("/var/www/uploads/document.pdf");
// If document.pdf exists, returns "/var/www/uploads/document-1.pdf"

// With exception
$unique = unique_filename("/var/www/uploads/document.pdf", "/var/www/uploads/document.pdf");
// Returns "/var/www/uploads/document.pdf" even if it exists
```

---

### pathinfo

Enhanced version of PHP's pathinfo that can distinguish between files and directories.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$path` | string | - | The path to analyze |
| `$flags` | int | PATHINFO_ALL | What information to return |

**Return Value:** `array\|string` - An array with path components or a specific component based on flags.

**Inner Mechanisms:**
1. Uses a regex to parse the path into components:
   - dirname: Everything up to the last separator
   - basename: The last component
   - filename: The basename without extension
   - extension: The file extension
2. Mimics PHP's native pathinfo behavior for flag handling
3. Can return specific components based on PATHINFO_* constants

**Usage Context:** Analyzing file paths to extract components, especially when distinguishing between files and directories is important.

```php
// Get all path components
$info = pathinfo("/var/www/uploads/image.jpg");
// Returns: ["dirname" => "/var/www/uploads", "basename" => "image.jpg", 
//           "filename" => "image", "extension" => "jpg"]

// Get specific component
$filename = pathinfo("/var/www/uploads/image.jpg", PATHINFO_FILENAME);
// Returns: "image"
```

---

### basename

Enhanced version of PHP's basename that properly handles suffixes.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$path` | string | - | The path to process |
| `$suffix` | string\|NULL | NULL | Suffix to remove from the basename |

**Return Value:** `string` - The basename with optional suffix removed.

**Inner Mechanisms:**
1. Quotes the suffix for regex safety if provided
2. Uses a regex to extract the basename:
   - Matches everything up to the last path separator
   - Optionally removes the specified suffix
   - Strips trailing path separators

**Usage Context:** Extracting filenames from paths, especially when removing known extensions.

```php
// Get basename
$name = basename("/var/www/uploads/image.jpg");
// Returns: "image.jpg"

// Get basename without extension
$name = basename("/var/www/uploads/image.jpg", ".jpg");
// Returns: "image"
```

---

### file_name

Extracts the filename (without extension) or extension from a path.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$path` | string | - | The path to analyze |
| `$extension` | bool | FALSE | Whether to return the extension instead of the name |

**Return Value:** `string\|bool` - The filename or extension, or FALSE on failure.

**Inner Mechanisms:**
1. Uses a static cache to avoid repeated processing
2. For local streams, uses the custom pathinfo function
3. For remote URLs, uses analyze_url
4. Caches the result as an array containing both filename and extension
5. Returns either the filename or extension based on the $extension parameter

**Usage Context:** Extracting filenames or extensions from paths, especially when dealing with both local and remote resources.

```php
// Get filename without extension
$name = file_name("/var/www/uploads/image.jpg");
// Returns: "image"

// Get extension
$ext = file_name("/var/www/uploads/image.jpg", TRUE);
// Returns: "jpg"
```

---

### file_extension

Convenience function to get the file extension from a path.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | The path to analyze |

**Return Value:** `string\|bool` - The file extension, or FALSE on failure.

**Inner Mechanisms:**
Simply calls file_name with the extension parameter set to TRUE.

**Usage Context:** Quickly getting file extensions for MIME type detection or validation.

```php
// Get file extension
$ext = file_extension("/var/www/uploads/image.jpg");
// Returns: "jpg"
```

---

### file_size

Gets the size of a file, supporting both local and remote files.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$path` | string | Path to the file |

**Return Value:** `int\|bool` - File size in bytes, or FALSE on failure.

**Inner Mechanisms:**
1. For local files:
   - Checks if the file exists
   - Returns the file size using filesize()
2. For remote files:
   - Retrieves HTTP headers
   - Extracts Content-Length header
   - Returns the size as an integer

**Usage Context:** Determining file sizes for display, validation, or progress tracking.

```php
// Get local file size
$size = file_size("/var/www/uploads/large_file.zip");
// Returns: 10485760 (size in bytes)

// Get remote file size
$size = file_size("https://example.com/file.zip");
// Returns: 5242880 (size in bytes)
```

---

### retrieve_file

Downloads a remote file to a local path.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source_url` | string | - | URL of the file to download |
| `$target_path` | string | - | Local path to save the file |
| `$timeout` | int | 60 | Timeout in seconds |

**Return Value:** `bool` - TRUE on success, FALSE on failure.

**Inner Mechanisms:**
1. Returns FALSE if the source is a local path
2. Creates a temporary file in the target directory
3. Sets up an HTTP context with:
   - Custom timeout
   - User-Agent header
   - Follow redirects
4. Attempts to copy the remote file to the temp location
5. Renames the temp file to the target path
6. Cleans up the temp file if the operation fails
7. Extends the time limit to accommodate the timeout

**Usage Context:** Downloading remote assets, importing files from external sources, or caching remote content.

```php
// Download a remote image
if (retrieve_file("https://example.com/image.jpg", "/var/www/uploads/image.jpg")) {
    echo "File downloaded successfully";
} else {
    echo "Download failed";
}
```

---

### download

Sends a file to the browser as a download with proper HTTP headers.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$file` | string | - | Path to the file to download |
| `$name` | string\|NULL | NULL | Custom filename for the download |

**Return Value:** `int\|bool` - Number of bytes sent on success, FALSE on failure.

**Inner Mechanisms:**
1. Validates that the file exists
2. Checks that headers haven't been sent
3. Silences errors and disables user abort checking
4. Implements ETag-based caching:
   - Generates an ETag from modification time and file size
   - Returns 304 Not Modified if the client already has the current version
5. Opens the file with shared locking
6. Clears all output buffers
7. Optionally starts a new buffer for MCP (Management Control Panel)
8. Sets appropriate HTTP headers:
   - Content-Disposition with filename
   - Content-Length
   - Content-Type with charset
   - Last-Modified
   - ETag
9. Sends the file in 512KB chunks with periodic time limit resets
10. Returns the total bytes sent or FALSE on failure

**Usage Context:** Serving files for download, such as user documents, generated reports, or media files.

```php
// Force download of a file
$bytes = download("/var/www/uploads/report.pdf", "Annual Report.pdf");
if ($bytes !== FALSE) {
    echo "Sent $bytes bytes";
} else {
    echo "Download failed";
}
```


<!-- HASH:6198269ad06a008d919bd3bfeda08807 -->
