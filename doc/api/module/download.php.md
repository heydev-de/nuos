# PWNC API Documentation

[← Index](../README.md) | [`module/download.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/download.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## download.php

The `download.php` module handles file downloads within the PWNC Web Platform. It serves two primary purposes:

1. **Display a download page** with file information and an automatic redirect to initiate the download after a short delay.
2. **Serve the actual file** for download when triggered via the `download_start` parameter.

The module uses the `download` library to retrieve file metadata, validates the file's existence, and provides appropriate user feedback. It also logs download actions for access tracking.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `$download_index` | `NULL` | Identifier for the download entry in the database |
| `$download_start` | `NULL` | Flag to trigger the actual file download (set to `1`) |

### Main Logic Flow

The module is wrapped in an immediately-invoked function expression (IIFE) to avoid polluting the global namespace. It performs the following steps:

1. **Verify download index**: Checks if `$download_start` is not set and `$download_index` is provided.
2. **Load download library**: Uses `cms_load("download")` to load the required library.
3. **Retrieve file data**: Fetches file metadata using the `download` class.
4. **Validate file existence**: Confirms the file exists on disk.
5. **Display or serve**: Either renders the download page or serves the file directly.

### Usage Example

```php
// Access the download page for a file with index "example_file"
// URL: /module/download.php?download_index=example_file

// After 2.5 seconds, the browser is automatically redirected to:
// /module/download.php?download_index=example_file&download_start=1
// Which triggers the actual file download.
```

### Key Functions and Methods Used

#### `cms_load("download")`

Loads the `download` library which provides the `download` class for managing download entries.

#### `$download->data->get($download_index, "filename")`

Retrieves the filename associated with the given download index from the data store.

#### `$download->data->remove_prefix($download_index)`

Compatibility fallback that removes a prefix from the download index to derive the filename.

#### `l($name)`

Localizes the given string for internationalization support.

#### `file_extension($filename)`

Extracts the file extension from the filename.

#### `format_bytesize($size)`

Formats a byte count into a human-readable string (e.g., "1.5 MB").

#### `cms_url([...])`

Generates a URL with the specified parameters, merging with the current request state.

#### `q($string)`

Encodes a string for safe inclusion in JavaScript/JSON contexts.

#### `x($string)`

Escapes special characters for safe HTML output.

#### `alert($message)`

Generates an alert box HTML element with the given message.

#### `insert($location)`

Inserts content at predefined template locations (e.g., "top", "bottom").

#### `permission([...])`

Displays permission-related information or access controls.

#### `download($path)`

Serves the file at the given path for download, setting appropriate HTTP headers.

#### `parse_text($text)`

Parses and formats text content, potentially handling markup or shortcodes.

### Conditional Behavior

- **When `$download_start` is empty**: Renders an HTML page with file information and a JavaScript redirect to start the download after 2.5 seconds.
- **When `$download_start` is set and `$flag` is true**: Serves the file directly using the `download()` function and logs the action.

### Error Handling

If the download index is invalid or the file does not exist, the module displays an alert message (`CMS_L_MOD_DOWNLOAD_007`) indicating the download is not available.

### Logging

When a file is successfully downloaded, a log entry is created using the `log` class:

```php
$log = new log();
$log->access("downloaded", $name);
```

This records the download action with the file name for audit or analytics purposes.


<!-- HASH:b1b4e15227f0f8d45ace509547b28c0f -->
