# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/math.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/math.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Math Utilities (`math.inc`)

This file provides core mathematical and formatting utilities for the PWNC Web Platform. It includes functions for percentage calculations, number formatting, byte size conversion, bitmask operations, sign determination, CSS unit conversion, base62 encoding, and string similarity measurement.

---

### `diffpercent`

Calculates the percentage difference between two values.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value1` | float/int | Reference value (must not be zero).  |
| `$value2` | float/int | Value to compare against reference.  |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted percentage difference (e.g., `"5.2%"` or `"-3.7%"`).              |
| FALSE   | If `$value1` is zero (invalid reference).                                  |

#### Inner Mechanisms

1. Validates `$value1` is non-zero.
2. Computes absolute percentage difference: `100 - (100 / $value1 * $value2)`.
3. Applies negative sign if `$value2 < $value1`.
4. Formats result with 1 decimal place using `CMS_L_DECIMAL_SEPARATOR`.

#### Usage Context

- **Typical Scenario**: Displaying growth/decline metrics (e.g., "Revenue increased by 12.5%").
- **Edge Case**: Returns `FALSE` for zero denominators to avoid division errors.

#### Example

```php
$oldPrice = 50;
$newPrice = 60;
echo diffpercent($oldPrice, $newPrice); // Output: "20.0%"
```

---

### `format_number`

Formats a number with locale-aware decimal and thousand separators.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | float/int | Number to format.                    |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted number (e.g., `"1,234.56"` or `"1234"`).                         |

#### Inner Mechanisms

1. Checks if `$value` has a fractional part using `fmod()`.
2. Uses `number_format()` with:
   - 2 decimal places if fractional part exists.
   - 0 decimal places if integer.
   - Locale-specific separators (`CMS_L_DECIMAL_SEPARATOR`, `CMS_L_THOUSAND_SEPARATOR`).

#### Usage Context

- **Typical Scenario**: Displaying prices, statistics, or measurements in user interfaces.
- **Edge Case**: Handles both integers and floats gracefully.

#### Example

```php
echo format_number(1234.56); // Output: "1,234.56" (en_US locale)
echo format_number(1234);    // Output: "1,234"
```

---

### `format_bytesize`

Converts a byte value into a human-readable string with appropriate units (KB, MB, GB).

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | int     | Byte size to convert.                |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted size with unit (e.g., `"1.23 MB"`).                              |

#### Inner Mechanisms

1. Checks value against thresholds (1024, 1048576, 1073741824) to determine unit.
2. Divides value by the appropriate factor and formats with 2 decimal places.
3. Uses `CMS_L_DECIMAL_SEPARATOR` for locale compatibility.

#### Usage Context

- **Typical Scenario**: Displaying file sizes, memory usage, or storage metrics.
- **Edge Case**: Values < 1024 return as `"X Byte"`.

#### Example

```php
echo format_bytesize(1500);      // Output: "1.46 KB"
echo format_bytesize(3000000);   // Output: "2.86 MB"
```

---

### `flag`

Checks if a specific bit flag is set in a bitmask.

#### Parameters

| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$bitmask` | int     | Bitmask to test.                     |
| `$flag`    | int     | Flag to check (single bit).          |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| bool    | `TRUE` if flag is set, `FALSE` otherwise.                                  |

#### Inner Mechanisms

1. Uses bitwise AND (`&`) to test if `$flag` is set in `$bitmask`.
2. Casts result to boolean.

#### Usage Context

- **Typical Scenario**: Checking user permissions, feature flags, or configuration options.
- **Edge Case**: Works with any integer bitmask.

#### Example

```php
$permissions = 0b1010; // Binary: 1010 (flags 2 and 8 are set)
echo flag($permissions, 0b1000) ? "Allowed" : "Denied"; // Output: "Allowed"
```

---

### `sgn`

Determines the sign of a numeric value.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | float/int | Value to evaluate.                   |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | `1` (positive), `-1` (negative), or `0` (zero).                            |

#### Inner Mechanisms

1. Compares `$value` against zero.
2. Returns `1`, `-1`, or `0` based on comparison.

#### Usage Context

- **Typical Scenario**: Sorting algorithms, mathematical operations requiring direction.
- **Edge Case**: Returns `0` for zero input.

#### Example

```php
echo sgn(-5); // Output: -1
echo sgn(0);  // Output: 0
```

---

### `dimension_to_px`

Converts CSS dimension strings (e.g., `"12em"`, `"5cm"`) to pixels.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | string  | CSS dimension string (e.g., `"12em"`). |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | Pixel value (rounded to nearest integer).                                  |
| FALSE   | If input is invalid or unit is unsupported.                                |

#### Inner Mechanisms

1. Uses regex to split input into numeric value and unit.
2. Validates numeric value and unit.
3. Multiplies value by unit-specific conversion factor (stored in static `$list`).
4. Rounds result to nearest integer.

#### Supported Units

| Unit | Conversion Factor (to pixels) | Description               |
|------|-------------------------------|---------------------------|
| `px` | 1                             | Pixels (no conversion).   |
| `em` | 16                            | Font-relative (1em = 16px). |
| `cm` | 16 * 6 / 2.54                 | Centimeters.              |
| `in` | 16 * 6                        | Inches.                   |
| `pt` | 16 * 6 / 72                   | Points (1/72 inch).       |
| ...  | ...                           | See static `$list` for all. |

#### Usage Context

- **Typical Scenario**: Dynamic CSS calculations, responsive design adjustments.
- **Edge Case**: Returns `FALSE` for invalid inputs (e.g., `"abc"`, `"12"`).

#### Example

```php
echo dimension_to_px("2em");   // Output: 32
echo dimension_to_px("5cm");   // Output: 118 (approx)
```

---

### `base62`

Encodes an integer into a base62 string (0-9, a-z, A-Z).

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | int     | Integer to encode.                   |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Base62-encoded string (e.g., `"a3fG"`).                                    |

#### Inner Mechanisms

1. Uses modulo division to extract digits from least to most significant.
2. Maps digits to characters using `$char` string (`012...XYZ`).
3. Builds result string in reverse order (corrected by prepending).

#### Usage Context

- **Typical Scenario**: Generating short, URL-safe identifiers (e.g., URL shorteners, unique keys).
- **Edge Case**: Input must be non-negative integer.

#### Example

```php
echo base62(123456); // Output: "w7e"
```

---

### `hamming_distance`

Calculates the Hamming distance between two strings (number of differing characters).

#### Parameters

| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$string1` | string  | First string.                        |
| `$string2` | string  | Second string.                       |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | Number of differing characters.                                            |

#### Inner Mechanisms

1. Determines maximum string length.
2. Iterates through each character position, comparing characters.
3. Counts mismatches (including positions where one string is shorter).

#### Usage Context

- **Typical Scenario**: String similarity analysis, error detection, or fuzzy matching.
- **Edge Case**: Handles strings of unequal length by treating missing characters as empty.

#### Example

```php
echo hamming_distance("karolin", "kathrin"); // Output: 3
echo hamming_distance("1011101", "1001001"); // Output: 2
```


<!-- HASH:46b59141383d031583ce18f989d6edf4 -->
