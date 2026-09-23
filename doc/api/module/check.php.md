# PWNC API Documentation

[← Index](../README.md) | [`module/check.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/check.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## module/check.php

### Overview

The `module/check.php` file is a lightweight endpoint within the PWNC Web Platform designed to check and update the bot status of the current user identified by their IP hash. It serves as a background mechanism to transition a user's log status from a provisional bot state to a provisional user state. The script responds with a transparent 1x1 GIF image, making it suitable for use as a tracking pixel or similar background request mechanism.

### Constants Used

| Constant | Description |
| --- | --- |
| `CMS_DB_LOG_USER_BOT` | Database column name for the bot status field in the user log table |
| `CMS_DB_LOG_USER` | Database table name for the user log |
| `CMS_DB_LOG_USER_USERID` | Database column name for the user identifier (IP hash) |
| `CMS_IPHASH` | Current user's IP hash used for identification |
| `CMS_LOG_STATUS_BOT_PROVISIONAL` | Status code indicating a provisional bot state |
| `CMS_LOG_STATUS_USER_PROVISIONAL` | Status code indicating a provisional user state |

### Execution Flow

1. **Error Handling Setup**: A custom error handler is set to suppress all errors, ensuring the script continues execution even if non-fatal errors occur.
2. **Database Connection**: Establishes a connection to the database using the `mysql` class.
3. **Bot Status Check**: Queries the database to retrieve the current bot status for the user identified by `CMS_IPHASH`.
4. **Status Transition**: If the user's bot status matches `CMS_LOG_STATUS_BOT_PROVISIONAL`, it updates the user's log status to `CMS_LOG_STATUS_USER_PROVISIONAL` using the `log` class.
5. **Response**: Outputs a transparent 1x1 GIF image with appropriate headers to prevent indexing.

### Usage Example

This script would typically be called in the background by the platform, perhaps via an image tag in HTML:

```html
<img src="/module/check.php" width="1" height="1" alt="" />
```

When loaded, the script checks if the current visitor (identified by IP hash) is in a provisional bot state and transitions them to a provisional user state if applicable. The GIF response allows it to be used seamlessly as a tracking pixel without affecting page layout.


<!-- HASH:0e0da05941df133da475d444b655613f -->
