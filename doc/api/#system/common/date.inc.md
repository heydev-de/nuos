# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/date.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/date.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Date Utilities (`date.inc`)

This file provides core date and time utility functions for the PWNC Web Platform. It handles localization, formatting, and human-readable date representations. All functions are part of the `cms` namespace.

---

### **Constants**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_SUNDAY` to `CMS_L_SATURDAY` | Localized strings | Full weekday names. |
| `CMS_L_SUNDAY_ABBR` to `CMS_L_SATURDAY_ABBR` | Localized strings | Abbreviated weekday names (e.g., "Sun"). |
| `CMS_L_JANUARY` to `CMS_L_DECEMBER` | Localized strings | Full month names. |
| `CMS_L_JANUARY_ABBR` to `CMS_L_DECEMBER_ABBR` | Localized strings | Abbreviated month names (e.g., "Jan"). |
| `CMS_L_SHORT_TIME_FORMAT` | `"H:i"` | Default time format (24-hour clock). |
| `CMS_L_DATE_FORMAT` | `"d.m.Y"` | Default date format (day.month.year). |
| `CMS_L_COMMON_002` to `CMS_L_COMMON_027` | Localized strings | Predefined phrases for `friendly_date()` (e.g., "X minutes ago"). |

---

### **Functions**

---

### `weekday(int $number, bool $abbr = TRUE): string|FALSE`

#### **Purpose**
Returns the localized name of a weekday (0 = Sunday, 6 = Saturday) in either full or abbreviated form.

#### **Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$number` | `int` | — | Weekday number (0–6). Values outside this range are normalized using modulo 7. |
| `$abbr` | `bool` | `TRUE` | If `TRUE`, returns abbreviated names (e.g., "Mon"); otherwise, full names (e.g., "Monday"). |

#### **Return Values**
- `string`: Localized weekday name.
- `FALSE`: If `$number` is invalid (unreachable due to modulo normalization).

#### **Inner Mechanisms**
- Uses modulo arithmetic to normalize `$number` to 0–6.
- Maps normalized values to localized constants via a `switch` statement.

#### **Usage Context**
- Displaying dates in user interfaces (e.g., calendars, logs).
- Formatting timestamps for readability.

#### **Example**
```php
echo weekday(1);       // Output: "Mon" (or localized equivalent)
echo weekday(5, FALSE); // Output: "Friday" (or localized equivalent)
```

---

### `month(int $number, bool $abbr = TRUE): string|FALSE`

#### **Purpose**
Returns the localized name of a month (1 = January, 12 = December) in either full or abbreviated form.

#### **Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$number` | `int` | — | Month number (1–12). |
| `$abbr` | `bool` | `TRUE` | If `TRUE`, returns abbreviated names (e.g., "Jan"); otherwise, full names (e.g., "January"). |

#### **Return Values**
- `string`: Localized month name.
- `FALSE`: If `$number` is outside 1–12.

#### **Inner Mechanisms**
- Uses a `switch` statement to map `$number` to localized constants.

#### **Usage Context**
- Date formatting in UIs (e.g., event listings, reports).
- Generating human-readable timestamps.

#### **Example**
```php
echo month(3);       // Output: "Mar" (or localized equivalent)
echo month(11, FALSE); // Output: "November" (or localized equivalent)
```

---

### `local_day_index(int $timestamp): int`

#### **Purpose**
Converts a Unix timestamp into a "day index" representing the number of days since the Unix epoch (1970-01-01), adjusted for the local timezone.

#### **Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$timestamp` | `int` | — | Unix timestamp. |

#### **Return Values**
- `int`: Day index (e.g., `19700` for 2024-01-01 in UTC+0).

#### **Inner Mechanisms**
- Adjusts `$timestamp` by the local timezone offset (`date("Z")`).
- Divides the result by `86400` (seconds in a day) and casts to `int`.

#### **Usage Context**
- Comparing dates without time components (e.g., "same day" checks).
- Grouping events by day in analytics.

#### **Example**
```php
$today = local_day_index(time());
$yesterday = $today - 1;
```

---

### `friendly_date(?int $time = NULL, bool $date_only = FALSE): string`

#### **Purpose**
Generates a human-readable, localized string describing the relative time between `$time` and the current time (e.g., "2 hours ago", "tomorrow at 14:30").

#### **Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$time` | `?int` | `NULL` | Unix timestamp. If `NULL`, uses the current time. |
| `$date_only` | `bool` | `FALSE` | If `TRUE`, omits the time component (e.g., "yesterday" instead of "yesterday at 14:30"). |

#### **Return Values**
- `string`: Localized, relative time description.

#### **Inner Mechanisms**
1. **Time Comparison**:
   - Calculates the absolute difference (`$seconds`) between `$time` and the current time.
   - Determines if `$time` is in the future or past.
2. **Interval Handling**:
   - **0 seconds**: Returns "right now".
   - **< 1 minute**: Returns seconds (e.g., "30 seconds ago").
   - **< 1 hour**: Returns minutes (e.g., "45 minutes ago").
   - **< 6 hours**: Returns hours and minutes (e.g., "2 hours 30 minutes ago").
   - **Same day**: Returns time (e.g., "14:30") or "today".
   - **Yesterday/Tomorrow**: Returns "yesterday at 14:30" or "tomorrow at 14:30".
   - **This week**: Returns weekday and time (e.g., "Monday at 14:30").
   - **This year**: Returns day and month (e.g., "3 March at 14:30").
   - **Other years**: Returns full date (e.g., "03.03.2020 at 14:30").

#### **Usage Context**
- Displaying timestamps in UIs (e.g., comments, notifications).
- Logging events with relative time descriptions.

#### **Example**
```php
echo friendly_date(strtotime("-2 hours")); // Output: "2 hours ago" (or localized equivalent)
echo friendly_date(strtotime("+1 day"), TRUE); // Output: "tomorrow" (or localized equivalent)
```


<!-- HASH:00787df4600e1c4b4d352fad0ce43d28 -->
