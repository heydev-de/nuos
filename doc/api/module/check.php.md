# NUOS API Documentation

[← Index](../README.md) | [`module/check.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: `check.php`

### Overview
This module serves as a **bot verification endpoint** for the NUOS web platform. Its primary purpose is to:

1. **Detect provisional bot status** for the current visitor (based on IP hash).
2. **Update the visitor's status** from "provisional bot" to "provisional user" if detected.
3. **Return a 1x1 transparent GIF pixel** (black pixel) with headers instructing search engines to ignore the resource.

This is typically used in scenarios where automated systems (e.g., search engine crawlers) need to be identified and their status updated in the system without disrupting their operation. The endpoint is designed to be lightweight, non-intrusive, and silent (no output except the pixel).

---

## Core Logic Flow

### Anonymous Function (Immediately Invoked)
The entire module logic is wrapped in an anonymous function that executes immediately upon inclusion. This ensures:
- **Isolation**: No variables leak into the global scope.
- **Cleanup**: Error handling and output buffering are managed locally.

---

### Error Handling
```php
set_error_handler(function() { return TRUE; });
```
| Name | Value/Default | Description |
|------|---------------|-------------|
| **Handler** | Anonymous function | Silently suppresses all PHP errors during execution. This prevents any error output from interfering with the pixel response. |

**Usage Context**:
- Used in production environments where silent failure is preferred over error leakage.

---

### Database Connection
```php
$mysql = new mysql();
while ($mysql->connection())
```
| Name | Type | Description |
|------|------|-------------|
| `$mysql` | `mysql` | Instance of the NUOS database wrapper. The `connection()` method establishes a MySQL connection. The `while` loop ensures the connection is active before proceeding. |

**Inner Mechanisms**:
- The `mysql` class internally uses `mysqli` but provides a simplified, consistent interface.
- The loop exits if the connection fails.

---

### User Identification
```php
$userid = sqlesc(CMS_IPHASH);
```
| Name | Type | Description |
|------|------|-------------|
| `$userid` | `string` | SQL-escaped hash of the visitor's IP address, used as a unique identifier in the log table. |

**Inner Mechanisms**:
- `CMS_IPHASH` is a constant representing the hashed IP of the current visitor.
- `sqlesc()` recursively escapes strings/arrays for SQL safety (prevents SQL injection).

---

### Bot Status Check
```php
$query = "SELECT " . CMS_DB_LOG_USER_BOT . " " .
         "FROM " . CMS_DB_LOG_USER . " " .
         "WHERE " . CMS_DB_LOG_USER_USERID . "='$userid' " .
         "LIMIT 1";
$result = mysql_query($query);
```
| Constant | Description |
|----------|-------------|
| `CMS_DB_LOG_USER_BOT` | Column name storing the bot status flag. |
| `CMS_DB_LOG_USER` | Table name for user/visitor logging. |
| `CMS_DB_LOG_USER_USERID` | Column name for the user identifier (IP hash). |

**Query Logic**:
- Fetches the bot status for the current visitor.
- `LIMIT 1` ensures only one row is returned (optimization).

---

### Status Validation
```php
if (
(! $result) //error
||
(($resultrow = mysql_fetch_row($result)) === FALSE) //no entry
||
((int)$resultrow[0] !== CMS_LOG_STATUS_BOT_PROVISIONAL)
)
break;
```
| Condition | Description |
|-----------|-------------|
| `! $result` | Query execution failed (e.g., database error). |
| `mysql_fetch_row($result) === FALSE` | No record exists for the visitor. |
| `(int)$resultrow[0] !== CMS_LOG_STATUS_BOT_PROVISIONAL` | Visitor's status is not "provisional bot". |

**Behavior**:
- If any condition is met, the loop breaks, and no further action is taken (pixel is returned as-is).

---

### Status Update
```php
$log = new log();
$log->user(NULL, NULL, NULL, CMS_LOG_STATUS_USER_PROVISIONAL, NULL, FALSE);
```
| Parameter | Type | Description |
|-----------|------|-------------|
| `NULL` (1st) | `mixed` | User ID (unused, defaults to current visitor). |
| `NULL` (2nd) | `mixed` | Session ID (unused). |
| `NULL` (3rd) | `mixed` | Action (unused). |
| `CMS_LOG_STATUS_USER_PROVISIONAL` | `int` | New status: "provisional user". |
| `NULL` (5th) | `mixed` | Additional data (unused). |
| `FALSE` | `bool` | Whether to log the action (suppressed here). |

**Inner Mechanisms**:
- The `log` class updates the visitor's status in the database.
- `CMS_LOG_STATUS_USER_PROVISIONAL` transitions the visitor from "bot" to "user" status.

---

### Pixel Response
```php
while (ob_get_level()) ob_end_clean();
header("Content-Type: image/gif");
header("X-Robots-Tag: noindex");
echo(base64_decode("R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="));
exit();
```
| Component | Description |
|-----------|-------------|
| `ob_end_clean()` | Clears all output buffers to ensure no extraneous data is sent. |
| `Content-Type: image/gif` | Sets the response MIME type to GIF. |
| `X-Robots-Tag: noindex` | Instructs search engines to ignore this resource. |
| `base64_decode(...)` | Decodes a 1x1 transparent GIF pixel (base64-encoded). |

**Usage Context**:
- Used in `<img>` tags or as a tracking pixel in emails/HTML.
- Example:
  ```html
  <img src="https://example.com/module/check.php" width="1" height="1" alt="" />
  ```

---

## Constants Reference
| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_IPHASH` | Hashed IP | Unique identifier for the current visitor. |
| `CMS_DB_LOG_USER_BOT` | Column name | Stores the bot status flag in the log table. |
| `CMS_DB_LOG_USER` | Table name | Log table for visitor tracking. |
| `CMS_DB_LOG_USER_USERID` | Column name | User identifier column (IP hash). |
| `CMS_LOG_STATUS_BOT_PROVISIONAL` | `int` | Status code for "provisional bot". |
| `CMS_LOG_STATUS_USER_PROVISIONAL` | `int` | Status code for "provisional user". |

---

## Typical Usage Scenarios
1. **Bot Detection in Templates**:
   Embed the pixel in a template to silently check and update bot status:
   ```html
   <img src="/module/check.php" style="display:none;" />
   ```

2. **Crawler Verification**:
   Use in conjunction with `robots.txt` to verify compliant crawlers:
   ```
   User-agent: *
   Allow: /module/check.php
   ```

3. **Analytics Tracking**:
   Track non-human visitors (e.g., search engine bots) without disrupting their behavior.

---

## Security Considerations
- **SQL Injection**: Mitigated via `sqlesc()`.
- **Error Leakage**: Suppressed via custom error handler.
- **Output Integrity**: Output buffering ensures only the pixel is returned.
- **CSRF**: Not applicable (no state modification via user input).

---

## Performance Notes
- **Minimal Overhead**: Only executes database queries if the visitor is a provisional bot.
- **No Dependencies**: Relies only on core NUOS utilities (`mysql`, `log`, `sqlesc`).
- **Stateless**: No session or cookie requirements.


<!-- HASH:25920bc40c7d5e73deaff13aec6b411d -->
