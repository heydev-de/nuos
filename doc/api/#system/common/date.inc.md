# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/date.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Date Utilities (`date.inc`)

This file provides core date and time utility functions for the NUOS platform. It handles localization-aware weekday/month name resolution and human-readable "friendly" date formatting for display purposes.

---

### `weekday`

```php
function weekday($number, $abbr = TRUE)
```

#### Purpose
Resolves a numeric weekday (0–6) into its localized string representation, optionally abbreviated.

#### Parameters

| Name    | Value/Default | Description                                                                 |
|---------|---------------|-----------------------------------------------------------------------------|
| `$number` | `int`         | Weekday number (0 = Sunday, 6 = Saturday). Wraps around via modulo 7.       |
| `$abbr`   | `TRUE`        | If `TRUE`, returns abbreviated weekday names (e.g., "Sun"); otherwise full. |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Localized weekday name (abbreviated or full).                              |
| `FALSE`  | If `$number` is outside 0–6 after modulo (should never occur in practice). |

#### Inner Mechanisms
- Uses modulo 7 to normalize `$number` into the 0–6 range.
- Maps each normalized value to a corresponding CMS language constant (`CMS_L_SUNDAY`, `CMS_L_SUNDAY_ABBR`, etc.).
- Language constants are defined elsewhere in the localization system.

#### Usage Context
- Displaying weekday names in calendars, event lists, or date pickers.
- Used internally by `friendly_date` to format relative dates.

---

### `month`

```php
function month($number, $abbr = TRUE)
```

#### Purpose
Resolves a numeric month (1–12) into its localized string representation, optionally abbreviated.

#### Parameters

| Name    | Value/Default | Description                                                                 |
|---------|---------------|-----------------------------------------------------------------------------|
| `$number` | `int`         | Month number (1 = January, 12 = December).                                  |
| `$abbr`   | `TRUE`        | If `TRUE`, returns abbreviated month names (e.g., "Jan"); otherwise full.   |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Localized month name (abbreviated or full).                                |
| `FALSE`  | If `$number` is outside 1–12 (should never occur in practice).             |

#### Inner Mechanisms
- Maps each month number to a corresponding CMS language constant (`CMS_L_JANUARY`, `CMS_L_JANUARY_ABBR`, etc.).
- Language constants are defined elsewhere in the localization system.

#### Usage Context
- Displaying month names in calendars, date pickers, or reports.
- Used internally by `friendly_date` to format absolute dates.

---

### `friendly_date`

```php
function friendly_date($time = NULL, $date_only = FALSE)
```

#### Purpose
Formats a Unix timestamp into a human-readable, localized, and context-aware string (e.g., "2 hours ago", "Yesterday at 3:45 PM", "Jan 5, 2023").

#### Parameters

| Name         | Value/Default | Description                                                                 |
|--------------|---------------|-----------------------------------------------------------------------------|
| `$time`      | `NULL`        | Unix timestamp. If `NULL`, defaults to current time.                        |
| `$date_only` | `FALSE`       | If `TRUE`, omits time and returns only date-related strings.               |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Localized, human-readable date/time string.                                |

#### Inner Mechanisms
1. **Time Normalization**: If `$time` is `NULL`, uses current time.
2. **Interval Calculation**: Computes the difference between `$time` and current time in seconds, minutes, hours, days, and weeks.
3. **Contextual Formatting**:
   - **Right now**: Returns "Just now" (localized).
   - **Within a minute**: Returns seconds (e.g., "30 seconds ago").
   - **Within an hour**: Returns minutes (e.g., "15 minutes ago").
   - **Within 6 hours**: Returns hours and minutes (e.g., "2 hours 30 minutes ago").
   - **Today**: Returns time only (e.g., "3:45 PM") or "Today" if `$date_only`.
   - **Yesterday/Tomorrow**: Returns "Yesterday at 3:45 PM" or "Tomorrow at 3:45 PM".
   - **This week**: Returns weekday and time (e.g., "Monday at 3:45 PM").
   - **This year**: Returns month and day (e.g., "Jan 5 at 3:45 PM").
   - **Other years**: Returns full date (e.g., "Jan 5, 2023 at 3:45 PM").
4. **Localization**: Uses CMS language constants (`CMS_L_COMMON_*`) for all strings.
5. **Future/Past Handling**: Adjusts strings for future dates (e.g., "in 2 hours").

#### Usage Context
- Displaying timestamps in activity feeds, comments, or logs.
- Reducing cognitive load by showing relative time (e.g., "2 hours ago") instead of absolute timestamps.
- Omitting time when only the date is relevant (e.g., blog post dates).


<!-- HASH:c9705b1b8e744ded8b9f45fd88bf6beb -->
