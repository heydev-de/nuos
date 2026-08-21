# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.common.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.common.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Common Library (`sys.common.inc`)

### Overview
The `sys.common.inc` file serves as the central **common library loader** for the PWNC Web Platform. It aggregates all core utility modules into a single namespace (`\cms`) to provide consistent access to essential functions across the platform.

This file **does not implement logic directly** but acts as a **dependency hub** for the platform’s foundational utilities. It ensures that all required subsystems (e.g., database, security, URL handling, string manipulation) are loaded and available globally under the `\cms` namespace.

---

### Required Modules
The following modules are loaded via `require()` and are made available to the entire codebase:

| Module Path               | Description |
|---------------------------|-------------|
| `common/codec.inc`        | Encoding/decoding utilities (e.g., base64, JSON, serialization). |
| `common/date.inc`         | Date/time manipulation and formatting. |
| `common/file.inc`         | File system operations (read, write, upload, permissions). |
| `common/hash.inc`         | Cryptographic hashing (e.g., SHA, bcrypt) and random token generation. |
| `common/image.inc`        | Image processing (resizing, cropping, format conversion). |
| `common/language.inc`     | Multilingual support (translations, locale handling). |
| `common/math.inc`         | Mathematical operations and number formatting. |
| `common/misc.inc`         | Miscellaneous utilities (e.g., debugging, logging). |
| `common/mysql.inc`        | Database abstraction layer (MySQL wrappers, query building). |
| `common/security.inc`     | Security utilities (CSRF, input validation, sanitization). |
| `common/snippet.inc`      | Reusable code snippets (e.g., HTML templates, macros). |
| `common/string.inc`       | String manipulation (multibyte-safe operations, escaping). |
| `common/text.inc`         | Text processing (parsing, formatting, diffing). |
| `common/url.inc`          | URL/parameter management (routing, query string generation). |

---

### Namespace
All loaded modules are scoped under the `\cms` namespace, ensuring no collisions with user-defined or third-party code.

---

### Usage Context
#### When to Use
- **Entry Points**: Include this file at the **start of every PHP script** (e.g., `index.php`, CLI tools) to ensure all core utilities are available.
- **Module Development**: When building new modules, rely on this file to access platform utilities without manual `require` statements.

#### When *Not* to Use
- **Performance-Critical Loops**: Avoid including this file in tight loops or performance-sensitive code (e.g., image processing). Instead, require only the specific modules needed.
- **Isolated Scripts**: For standalone scripts (e.g., cron jobs), include only the necessary modules to reduce overhead.

---

### Example Usage
#### Scenario: Bootstrapping a PWNC Application
```php
<?php
// Load the common library (all core utilities become available under \cms)
require_once __DIR__ . '/system/sys.common.inc';

// Use a utility from the loaded modules (e.g., URL generation)
$url = \cms\cms_url('user/profile', ['id' => 42]);
echo "Redirecting to: " . $url;

// Access database utilities
$result = \cms\mysql_query("SELECT * FROM users WHERE id = 1");
?>
```
**Explanation**:
1. The `sys.common.inc` file loads all core modules, making them accessible via the `\cms` namespace.
2. The example demonstrates using `cms_url()` (from `common/url.inc`) to generate a URL and `mysql_query()` (from `common/mysql.inc`) to interact with the database.

---

### Inner Mechanisms
1. **Autoloading**: The file uses `require` (not `require_once`) to ensure modules are loaded exactly once per request. This is safe because the platform’s architecture prevents duplicate inclusions.
2. **Namespace Isolation**: By wrapping utilities in the `\cms` namespace, the platform avoids conflicts with user code or third-party libraries.
3. **Dependency Order**: Modules are loaded in a logical order (e.g., `security.inc` before `mysql.inc` to ensure escaping functions are available for database operations).

---

### Notes
- **Extensibility**: To add a new utility module, place it in the `common/` directory and add a `require` statement here.
- **Backward Compatibility**: Removing or renaming modules requires updates to this file to avoid runtime errors.
- **Testing**: The `cms_load()` function (from `common/misc.inc`) can be used to test if a module is loaded:
  ```php
  if (!\cms\cms_load('mysql', false, true)) {
      die("Database module failed to load!");
  }
  ```


<!-- HASH:d1ed9ce7524aa47be9e88038e9b5fa5a -->
