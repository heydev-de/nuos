# PWNC API Documentation

[← Index](../README.md) | [`module/download.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/download.php)

- **Version:** `26.6.27.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: `module/download.php`

**Overview:**
This file implements the download module for the PWNC Web Platform. It handles the delivery of downloadable files stored in the system, providing both a user interface for download confirmation and direct file streaming. The module ensures proper access control, logging, and compatibility with the platform's content management system.

The module operates in two modes:
1. **Confirmation Page:** Displays file metadata (name, size, description) and initiates the download after a short delay.
2. **Direct Download:** Streams the file to the client if the `download_start` parameter is set.

---

### Global Variables

| Name               | Type      | Description                                                                                     |
|--------------------|-----------|-------------------------------------------------------------------------------------------------|
| `$download_index`  | `string`  | Identifier for the downloadable file, typically passed via URL parameters.                      |
| `$download_start`  | `mixed`   | Flag indicating whether the download should start immediately (non-empty value triggers download). |

---

### Inner Workings

1. **Initialization:**
   - The module checks if `$download_index` is set and loads the `download` library.
   - If valid, it retrieves file metadata (filename, name, description) from the `download` data store.
   - A fallback mechanism ensures backward compatibility by using the index as the filename if metadata is missing.

2. **Confirmation Page:**
   - Displays file details (name, extension, size, description) and a countdown to the download.
   - Uses `cms_url()` to generate a secure download link with CSRF protection.

3. **Direct Download:**
   - Streams the file using the `download()` function if `$download_start` is set.
   - Logs the download action via the `log` class.

4. **Error Handling:**
   - Shows an alert if the file is unavailable or metadata is missing.

---

### Functions and Methods

#### `download($path)`
**Purpose:**
Streams a file to the client with appropriate headers for download.

**Parameters:**

| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$path`| `string` | Absolute path to the file to stream. |

**Return Values:**
- `bool`: `TRUE` if the file was successfully streamed, `FALSE` otherwise.

**Inner Mechanisms:**
- Validates the file path and checks if the file exists.
- Sets HTTP headers for:
  - Content-Type (based on file extension).
  - Content-Disposition (forces download with the original filename).
  - Content-Length (file size).
- Uses `readfile()` to output the file in chunks for memory efficiency.

**Usage Context:**
- Called when `$download_start` is set to stream the file directly to the client.

**Example:**
```php
// Stream a file located at "/var/www/data/downloads/example.pdf"
download(CMS_DATA_PATH . "#download/example.pdf");
```

---

### Classes

#### `download`
**Purpose:**
Manages downloadable files, including metadata storage and retrieval.

**Properties:**

| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `data`    | `object` | Instance of a data store (e.g., database or file-based) for download metadata. |

**Methods:**

##### `data->get($index, $key)`
**Purpose:**
Retrieves metadata for a downloadable file.

**Parameters:**

| Name    | Type     | Description                                      |
|---------|----------|--------------------------------------------------|
| `$index`| `string` | Unique identifier for the downloadable file.     |
| `$key`  | `string` | Metadata key (e.g., "name", "description").      |

**Return Values:**
- `mixed`: The value associated with the key, or `NULL` if not found.

**Usage Context:**
- Used to fetch file metadata (e.g., name, description) for display on the confirmation page.

**Example:**
```php
$name = $download->data->get("example_file", "name");
```

##### `data->remove_prefix($index)`
**Purpose:**
Fallback method to extract a filename from the index if metadata is missing.

**Parameters:**

| Name    | Type     | Description                                      |
|---------|----------|--------------------------------------------------|
| `$index`| `string` | Unique identifier for the downloadable file.     |

**Return Values:**
- `string`: The filename derived from the index.

**Usage Context:**
- Ensures backward compatibility when metadata is not available.

**Example:**
```php
$filename = $download->data->remove_prefix("legacy_file_123");
// Returns "123" if no metadata exists.
```

---

### Utility Functions

#### `file_extension($filename)`
**Purpose:**
Extracts the file extension from a filename.

**Parameters:**

| Name        | Type     | Description              |
|-------------|----------|--------------------------|
| `$filename` | `string` | The filename to process. |

**Return Values:**
- `string`: The file extension in lowercase, or an empty string if none exists.

**Example:**
```php
$ext = file_extension("document.PDF");
// Returns "pdf".
```

#### `format_bytesize($bytes)`
**Purpose:**
Converts a file size in bytes to a human-readable format (e.g., KB, MB).

**Parameters:**

| Name     | Type     | Description              |
|----------|----------|--------------------------|
| `$bytes` | `int`    | File size in bytes.      |

**Return Values:**
- `string`: Formatted size with appropriate unit (e.g., "2.5 MB").

**Example:**
```php
$size = format_bytesize(2621440);
// Returns "2.5 MB".
```

#### `parse_text($text)`
**Purpose:**
Processes text for safe display, including line breaks and HTML escaping.

**Parameters:**

| Name   | Type     | Description              |
|--------|----------|--------------------------|
| `$text`| `string` | Text to process.         |

**Return Values:**
- `string`: Processed text with line breaks converted to `<br>` and special characters escaped.

**Example:**
```php
$description = parse_text("Line 1\nLine 2");
// Returns "Line 1<br>Line 2".
```

---

### Usage Examples

#### Example 1: Displaying a Download Confirmation Page
**Scenario:**
A user clicks a download link for a file with index `example_pdf`. The module displays a confirmation page with metadata.

**Code Flow:**
1. `$download_index` is set to `example_pdf`.
2. The `download` class retrieves metadata:
   ```php
   $name = $download->data->get("example_pdf", "name"); // "Example Document"
   $description = $download->data->get("example_pdf", "description"); // "A sample PDF file."
   ```
3. The confirmation page renders with:
   - Title: "Download"
   - Name: "Example Document"
   - Size: "512 KB"
   - Description: "A sample PDF file."
   - Countdown to download.

**Output:**
```html
<h1>Download</h1>
<div class="p">
  <strong>Example Document</strong> (PDF, 512 KB)<br>
  A sample PDF file.
</div>
<div class="p">
  <strong>Your download will start shortly.</strong><br>
  If it does not start, <a href="?download_index=example_pdf&download_start=1">click here</a>.
</div>
```

---

#### Example 2: Streaming a File Directly
**Scenario:**
A user clicks the direct download link (`download_start=1`). The module streams the file.

**Code Flow:**
1. `$download_start` is set to `1`.
2. The file path is constructed:
   ```php
   $path = CMS_DATA_PATH . "#download/example.pdf";
   ```
3. The `download()` function streams the file:
   ```php
   download($path);
   ```
4. Headers are sent:
   ```http
   Content-Type: application/pdf
   Content-Disposition: attachment; filename="Example Document.pdf"
   Content-Length: 524288
   ```

---

### Error Handling
- If `$download_index` is invalid or the file does not exist, the module displays an alert:
  ```php
  echo(alert(CMS_L_MOD_DOWNLOAD_007)); // "The requested download is not available."
  ```
- If metadata is missing, the module falls back to using the index as the filename.

---

### Security Considerations
1. **CSRF Protection:**
   - Download links are generated using `cms_url()`, which includes a CSRF token.
2. **Path Validation:**
   - The file path is constructed using `CMS_DATA_PATH` to prevent directory traversal attacks.
3. **Logging:**
   - Downloads are logged via the `log` class for auditing.
4. **Escaping:**
   - All dynamic content is escaped using `x()` (XML/HTML) or `q()` (JavaScript/JSON).


<!-- HASH:b1b4e15227f0f8d45ace509547b28c0f -->
