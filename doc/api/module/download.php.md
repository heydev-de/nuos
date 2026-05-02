# NUOS API Documentation

[← Index](../README.md) | [`module/download.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: `download.php`

**Overview:**
This file implements the download module for the NUOS web platform. It handles the delivery of downloadable files stored in the system, providing both a user interface for previewing download details and the actual file transfer mechanism. The module ensures secure access, logs download actions, and maintains compatibility with the platform's content management system.

---

### Global Variables

| Name               | Type     | Description                                                                                     |
|--------------------|----------|-------------------------------------------------------------------------------------------------|
| `$download_index`  | `string` | Identifier for the downloadable file, passed via URL parameters.                                |
| `$download_start`  | `int`    | Flag indicating whether the download should start immediately (`1`) or show a preview (`NULL`). |

---

### Anonymous Function (Immediately Invoked)

The entire module logic is wrapped in an anonymous function to avoid polluting the global namespace.

#### Inner Mechanisms
1. **Initialization:**
   - Loads the `download` library if `$download_index` is set.
   - Retrieves file metadata (e.g., `filename`, `name`, `description`) from the `download` data store.
   - Validates the existence of the physical file in `CMS_DATA_PATH/#download/`.

2. **Preview Mode (`$download_start` is empty):**
   - Renders an HTML page with download details (name, extension, size, description).
   - Automatically redirects to the download URL after 2.5 seconds via JavaScript.
   - Displays an error message if the download is unavailable.

3. **Download Mode (`$download_start` is `1`):**
   - Initiates the file transfer using the `download()` function.
   - Logs the download action via the `log` class.
   - Terminates script execution after the download starts.

---

### Key Functions and Logic

#### File Metadata Retrieval
- **Purpose:**
  Fetches metadata for the requested downloadable file from the `download` data store.
- **Parameters:**
  - `$download_index` (string): Unique identifier for the file.
- **Return Values:**
  - `$name` (string): Human-readable name of the file.
  - `$filename` (string): Physical filename on disk (fallback to `$download_index` if not set).
  - `$description` (string): Optional description of the file.
- **Usage Context:**
  Used to populate the preview page with file details.

---

#### `download($path)`
- **Purpose:**
  Streams a file to the client with appropriate headers for download.
- **Parameters:**
  | Name   | Type     | Description                          |
  |--------|----------|--------------------------------------|
  | `$path`| `string` | Absolute path to the file on disk.   |
- **Return Values:**
  - `bool`: `TRUE` if the file was successfully streamed, `FALSE` otherwise.
- **Inner Mechanisms:**
  - Sets HTTP headers for file transfer (`Content-Type`, `Content-Disposition`, `Content-Length`).
  - Uses `readfile()` to output the file in chunks for memory efficiency.
- **Usage Context:**
  Called when `$download_start` is `1` to initiate the file transfer.

---

#### Logging
- **Purpose:**
  Records download actions in the system log for auditing.
- **Mechanism:**
  - Instantiates the `log` class and calls `access("downloaded", $name)`.
- **Usage Context:**
  Executed after a successful file transfer to track user activity.

---

### Helper Functions (Implicit Usage)

| Function               | Description                                                                                     |
|------------------------|-------------------------------------------------------------------------------------------------|
| `cms_load("download")` | Loads the `download` library, which provides access to the `download` data store.              |
| `l($string)`           | Localizes a string (translates it based on the user's language settings).                       |
| `x($string)`           | Escapes a string for safe output in HTML/XML contexts.                                          |
| `q($string)`           | Escapes a string for safe output in JavaScript/JSON contexts.                                   |
| `cms_url()`            | Generates a URL with the specified parameters, merging them with the current global state.      |
| `format_bytesize()`    | Converts a file size in bytes to a human-readable format (e.g., "1.2 MB").                      |
| `parse_text()`         | Processes text for display (e.g., converting Markdown or BBCode to HTML).                       |
| `alert()`              | Generates an HTML alert box for error messages.                                                 |
| `permission()`         | Checks user permissions and displays an access denied message if necessary.                     |
| `insert()`             | Injects HTML content (e.g., headers, footers) from template files.                              |

---

### Typical Usage Scenarios

1. **Previewing a Download:**
   - User clicks a link with `download_index` set (e.g., `?download_index=123`).
   - The module renders a preview page with file details and a countdown to the download.

2. **Direct Download:**
   - User clicks a link with both `download_index` and `download_start=1` set.
   - The module streams the file directly to the client and logs the action.

3. **Error Handling:**
   - If the file is missing or metadata is invalid, the module displays an error message.

---

### Security Considerations
- **CSRF Protection:**
  URLs generated via `cms_url()` include CSRF tokens to prevent unauthorized downloads.
- **Path Validation:**
  The physical file path is constructed using `CMS_DATA_PATH` and validated with `is_file()`.
- **Escaping:**
  All dynamic output is escaped using `x()`, `q()`, or other context-aware functions.

---

### Dependencies
- **Libraries:**
  - `download`: Provides access to file metadata and data store.
  - `log`: Handles logging of download actions.
- **Constants:**
  - `CMS_DATA_PATH`: Base path for file storage.
  - `CMS_L_MOD_DOWNLOAD_*`: Localized strings for UI messages.
  - `CMS_DOCTYPE_HTML`, `CMS_HTML_HEADER`, etc.: HTML template fragments.


<!-- HASH:fe0ae0a502c06bced419ff311fb601c3 -->
