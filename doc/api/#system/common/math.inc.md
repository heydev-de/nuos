# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/math.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Math Utilities (`math.inc`)

Core mathematical and formatting utilities for the NUOS platform. Provides functions for percentage calculations, number formatting, byte size conversion, bitmask operations, sign determination, CSS unit conversion, base62 encoding, and string distance measurement.

---

### `diffpercent`

Calculates the percentage difference between two values.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value1` | `float`  | Base value (denominator)             |
| `$value2` | `float`  | Value to compare against the base    |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Formatted percentage difference (e.g., `-12.3%`) with 1 decimal place       |
| `FALSE`   | If `$value1` is zero or falsy (division by zero protection)                 |

**Inner Mechanisms:**
- Computes relative difference: `(100 / $value1 * $value2)`
- Adjusts for direction: negative if `$value2 < $value1`
- Formats result using `number_format` with locale-specific decimal separator

**Usage Context:**
- Displaying growth/decline metrics (e.g., sales, traffic)
- Comparative analytics dashboards

---

### `format_number`

Formats a number with locale-specific separators, omitting decimals for integers.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `float`  | Number to format                     |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Formatted number (e.g., `1,234.56` or `1,234`)                              |

**Inner Mechanisms:**
- Checks for fractional part using `fmod`
- Uses `number_format` with dynamic decimal places (0 for integers, 2 for floats)
- Respects `CMS_L_DECIMAL_SEPARATOR` and `CMS_L_THOUSAND_SEPARATOR` constants

**Usage Context:**
- User-facing number display (e.g., financial reports, statistics)

---

### `format_bytesize`

Converts a byte value into a human-readable string with appropriate unit (KB, MB, GB).

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `int`    | Byte size to convert                 |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Formatted size with unit (e.g., `1.23 MB`)                                  |

**Inner Mechanisms:**
- Threshold-based unit selection (1024, 1048576, 1073741824)
- Uses `number_format` with 2 decimal places for non-byte units

**Usage Context:**
- File size display (e.g., upload limits, storage usage)

---

### `flag`

Checks if a specific bit flag is set in a bitmask.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$bitmask`| `int`    | Bitmask to test                      |
| `$flag`   | `int`    | Flag to check                        |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if flag is set, `FALSE` otherwise                                    |

**Inner Mechanisms:**
- Uses bitwise AND (`&`) to test flag presence

**Usage Context:**
- Permission checks (e.g., user roles, feature toggles)
- Configuration option validation

---

### `sgn`

Determines the sign of a numeric value.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `float`  | Value to evaluate                    |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | `1` (positive), `-1` (negative), or `0` (zero)                              |

**Usage Context:**
- Directional calculations (e.g., sorting, vector math)

---

### `dimension_to_px`

Converts CSS dimension strings (e.g., `1.5em`, `10px`) to pixel values.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `string` | CSS dimension (e.g., `12pt`, `2em`)  |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | Pixel value (rounded)                                                       |
| `FALSE`   | Invalid input format or unknown unit                                        |

**Constants/Variables:**

| Name      | Value/Default | Description                          |
|-----------|---------------|--------------------------------------|
| `$unit`   | Static array  | Conversion factors for CSS units (e.g., `1em = 16px`) |

**Inner Mechanisms:**
- Regex parsing: `^(\d*(?:\.\d*)?)([a-z]*)$`
- Falls back to integer if no unit is specified
- Returns `FALSE` for invalid formats or unknown units

**Usage Context:**
- Dynamic CSS value conversion (e.g., responsive layouts, theme calculations)

---

### `base62`

Encodes an integer into a base62 string (0-9, a-z, A-Z).

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `int`    | Integer to encode                    |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Base62-encoded string (e.g., `1aB`)                                         |

**Inner Mechanisms:**
- Character set: `0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`
- Modulo-based encoding loop

**Usage Context:**
- Short URL generation
- Compact ID encoding (e.g., database keys)

---

### `hamming_distance`

Calculates the Hamming distance between two strings (number of differing characters).

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$string1` | `string` | First string                         |
| `$string2` | `string` | Second string                        |

**Return Values:**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `int`     | Number of differing characters (0 = identical)                              |

**Inner Mechanisms:**
- Iterates up to the length of the longer string
- Uses null coalescing (`??`) for out-of-bounds indices

**Usage Context:**
- String similarity analysis (e.g., typo detection, fuzzy matching)


<!-- HASH:bf7b19e0e024d27ec30de3162db3d2ca -->
