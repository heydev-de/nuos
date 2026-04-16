# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.common.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Common Library Loader (`sys.common.inc`)

### Overview
This file serves as the central loader for NUOS's core utility libraries. It aggregates all fundamental system modules into a single namespace (`cms`) to provide consistent access to essential functions across the platform. The file does not contain executable logic but acts as a manifest for required dependencies.

---

### Included Libraries
The following table lists all libraries loaded by this file, their primary purpose, and typical use cases.

| Library File            | Purpose                                                                                     | Key Functional Areas                                                                 |
|-------------------------|---------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------|
| `common/codec.inc`      | Encoding, decoding, and escaping utilities                                                  | String escaping (SQL, JS, XML, URL), multibyte-safe operations, nested encoders      |
| `common/comm.inc`       | Communication and request handling                                                          | HTTP request/response management, CSRF protection, session handling                  |
| `common/date.inc`       | Date and time manipulation                                                                  | Formatting, parsing, timezone handling, relative time calculations                   |
| `common/file.inc`       | File system operations                                                                      | File I/O, path resolution, directory traversal, MIME type detection                  |
| `common/hash.inc`       | Cryptographic and non-cryptographic hashing                                                 | Password hashing, checksums, token generation                                        |
| `common/ice.inc`        | Incremental cache engine                                                                    | Dual-layer caching (RAM + persistent storage), cache invalidation                    |
| `common/image.inc`      | Image processing                                                                            | Thumbnail generation, format conversion, EXIF metadata handling                      |
| `common/language.inc`   | Multilingual support                                                                        | Locale management, translation lookups, language detection                           |
| `common/math.inc`       | Mathematical operations                                                                     | Number formatting, rounding, statistical functions, random number generation         |
| `common/misc.inc`       | Miscellaneous utilities                                                                     | System diagnostics, environment checks, fallback mechanisms                          |
| `common/mysql.inc`      | Database abstraction                                                                        | MySQL wrapper (mysqli-based), query building, transaction management                 |
| `common/snippet.inc`    | Reusable code snippets                                                                      | Templating helpers, conditional logic, data transformation                           |
| `common/string.inc`     | String manipulation                                                                         | Multibyte-safe string operations, validation, sanitization                           |
| `common/text.inc`       | Text processing                                                                             | Markdown parsing, typography correction, text normalization                         |
| `common/url.inc`        | URL and parameter management                                                                | URL generation, querystring manipulation, logical-to-physical URL resolution         |

---

### Namespace
All loaded libraries are scoped under the `cms` namespace. Functions from these libraries are accessible globally after inclusion but are logically grouped under this namespace for organizational clarity.

---

### Usage Context
#### When to Include
- **Core Initialization**: This file is included during the bootstrap phase of the NUOS platform. It should **not** be included manually in application code.
- **Dependency Management**: Modules or applications requiring any of the core utilities should rely on this file being pre-loaded by the system.

#### Typical Scenarios
1. **Platform Bootstrapping**:
   - Loaded by the entry point (`index.php`) to initialize all core utilities before routing or application logic execution.
2. **Module Development**:
   - Developers can assume these libraries are available when writing custom modules or extensions.
3. **Testing**:
   - Test scripts may include this file to simulate the full runtime environment.

#### Example
```php
// No direct usage; libraries are auto-loaded by the system.
// Functions from included libraries are available globally:
$escaped = sqlesc("user input"); // From common/codec.inc
$url = u("content://home", ["page" => 1]); // From common/url.inc
```

---

### Inner Mechanisms
- **Order of Inclusion**: Libraries are loaded in a specific order to ensure dependencies are resolved (e.g., `codec.inc` is loaded first as other libraries may depend on its escaping functions).
- **Namespace Isolation**: The `namespace cms;` declaration ensures all loaded functions are logically grouped, though they remain globally callable for backward compatibility.
- **Error Handling**: If a required library file is missing, the system will trigger a fatal error during bootstrap.

---

### Notes
- **No Direct Execution**: This file contains no executable code beyond `require` statements.
- **Zero Dependencies**: The libraries included here are designed to be self-contained with no external dependencies (e.g., Composer packages).
- **Performance**: Loading all libraries upfront ensures consistent performance and avoids autoloading overhead.


<!-- HASH:c70bc0488cc2994bd1823de449326ff4 -->
