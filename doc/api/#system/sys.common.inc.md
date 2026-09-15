# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.common.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.common.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## sys.common.inc

The `sys.common.inc` file serves as the central entry point for the **Common Library** within the PWNC Web Platform. It resides in the `#system` directory and is responsible for bootstrapping all foundational utility libraries that the platform relies on.

Rather than containing executable logic itself, this file acts as an aggregator — it includes a series of modular `.inc` files from the `common/` subdirectory. Each included file encapsulates a specific domain of functionality (e.g., string handling, database access, URL generation), allowing the system to maintain a clean separation of concerns while ensuring that all core utilities are available globally once this file is loaded.

This design supports lazy loading and modularity: only necessary components are loaded when needed, yet they're all accessible through a single include point.

### Included Libraries

| File | Purpose |
|------|---------|
| `codec.inc` | Encoding/decoding helpers (e.g., base64, JSON) |
| `date.inc` | Date/time manipulation and formatting |
| `file.inc` | Filesystem operations (read/write/move) |
| `hash.inc` | Hashing functions (passwords, checksums) |
| `image.inc` | Image processing utilities |
| `language.inc` | Localization and language support |
| `math.inc` | Mathematical helper functions |
| `misc.inc` | Miscellaneous general-purpose utilities |
| `mysql.inc` | Database abstraction layer using MySQLi |
| `security.inc` | Security-related functions (escaping, validation) |
| `snippet.inc` | Code snippet management |
| `string.inc` | String manipulation and multibyte-safe functions |
| `text.inc` | Text formatting and parsing tools |
| `url.inc` | URL construction, routing, and parameter handling |

### Usage Context

This file should be included early in any request lifecycle where access to the full suite of common utilities is required. Typically, it's part of the system bootstrap process and is automatically included by higher-level system files or application entry points.

#### Example:

```php
// In your main application script or controller
require_once '#system/sys.common.inc';

// Now you can use any utility function from the included libraries
echo cms_url('/home', ['page' => 'about']); // Generates a URL with merged parameters
```

By including `sys.common.inc`, developers gain immediate access to over a dozen specialized utility modules without having to manually require each one individually.


<!-- HASH:d1ed9ce7524aa47be9e88038e9b5fa5a -->
