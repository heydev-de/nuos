# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/date.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/date.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Date and Time Utilities

This file provides localized date/time formatting and helper functions for the PWNC Web Platform. It includes functions for retrieving weekday and month names, calculating day indices, and generating human-readable "friendly" date strings (e.g., "Today", "Yesterday", "2 hours ago").

### weekday

Returns the localized name of a weekday based on a numeric index.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$number` | int | — | Day number (0 = Sunday through 6 = Saturday). Values are taken modulo 7. |
| `$abbr`   | bool | TRUE    | If TRUE, returns the abbreviated form; otherwise returns the full name. |

**Return Value:**  
- **string** – The localized weekday name (abbreviated or full).
- **FALSE** – If the number doesn't match any valid case (should not occur due to modulo).

**Inner Mechanism:**  
Uses a switch statement to map the integer value to one of several predefined language constants (`CMS_L_SUNDAY`, etc.). The `$abbr` flag selects between short and long forms.

**Usage Example:**
```php
echo weekday(3);           // Outputs: Wed (or localized abbreviation)
echo weekday(0, false);    // Outputs: Sunday (or localized full name)
```

---

### month

Returns the localized name of a month based on a numeric index.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$number` | int | — | Month number (1–12). |
| `$abbr`   | bool | TRUE    | If TRUE, returns the abbreviated form; otherwise returns the full name. |

**Return Value:**  
- **string** – The localized month name (abbreviated or full).
- **FALSE** – If the number doesn't match any valid case.

**Inner Mechanism:**  
Maps the input integer to a corresponding language constant using a switch statement.

**Usage Example:**
```php
echo month(1);             // Outputs: Jan
echo month(12, false);     // Outputs: December
```

---

### local_day_index

Calculates the local day index from a Unix timestamp, accounting for timezone offset.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$timestamp` | int | — | A Unix timestamp. |

**Return Value:**  
- **int** – The number of days since the Unix epoch in the local timezone.

**Inner Mechanism:**  
Adds the timezone offset (in seconds) to the timestamp before dividing by the number of seconds in a day (86400), ensuring correct day boundaries regardless of server timezone settings.

**Usage Example:**
```php
$index = local_day_index(time());
echo $index; // e.g., 19876
```

---

### friendly_date

Generates a human-readable, relative date/time string from a given timestamp.

| Parameter     | Type      | Default | Description |
|---------------|-----------|---------|-------------|
| `$time`       | int/NULL  | NULL    | Timestamp to format. Defaults to current time if NULL. |
| `$date_only`  | bool      | FALSE   | If TRUE, omits time information and returns only date-related text. |

**Return Value:**  
- **string** – A localized, human-friendly representation of the date/time.

**Inner Mechanism:**  
Computes intervals between the provided time and the current time, then selects an appropriate phrase based on how far apart they are:
- Within the same minute → "Just now" or "In X seconds"
- Within the same hour → "X minutes ago" or "In X minutes"
- Up to 6 hours → "X hours ago" or "In X hours"
- Same day → "Today at HH:MM"
- Yesterday/Tomorrow → "Yesterday at HH:MM" or "Tomorrow at HH:MM"
- This week → "Day at HH:MM"
- This year → "Month Day at HH:MM"
- Other years → "Full date at HH:MM"

All output strings use language constants like `CMS_L_COMMON_002`, which should be defined elsewhere in the system for localization support.

**Usage Example:**
```php
echo friendly_date(strtotime('-2 hours')); 
// Outputs something like: "2 hours ago"

echo friendly_date(strtotime('+1 day'), true);
// Outputs something like: "Tomorrow"
```


<!-- HASH:00787df4600e1c4b4d352fad0ce43d28 -->
