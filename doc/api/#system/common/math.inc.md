# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/math.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/math.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Math Utility Functions

This file provides a collection of mathematical and formatting utility functions used throughout the PWNC Web Platform. These functions handle percentage calculations, number formatting, byte size representation, bitmask operations, sign detection, CSS unit conversion, base62 encoding, and string comparison metrics.

### Constants

| Name | Default | Description |
|------|---------|-------------|
| `CMS_L_DECIMAL_SEPARATOR` | Locale-dependent | Character used as decimal separator in formatted numbers |
| `CMS_L_THOUSAND_SEPARATOR` | Locale-dependent | Character used as thousands separator in formatted numbers |

### Helper Functions (Referenced)

| Name | Description |
|------|-------------|
| `stre($v)` | Returns `TRUE` if the value is empty |
| `nstre($v)` | Returns `TRUE` if the value is not empty |

---

### diffpercent

Calculates the percentage difference between two values, indicating whether the second value is higher or lower than the first.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `int\|float` | The reference (baseline) value |
| `$value2` | `int\|float` | The value to compare against the reference |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | A formatted percentage string (e.g., `"25.0%"`, `"-10.0%"`) |
| `FALSE` | If `$value1` is zero or falsy (to prevent division by zero) |

#### Inner Mechanisms

1. If `$value1` is falsy, returns `FALSE` immediately to avoid division by zero.
2. Computes the absolute difference: `abs(100 - (100 / $value1 * $value2))`.
3. If `$value2` is less than `$value1`, the difference is negated to indicate a decrease.
4. The result is formatted to one decimal place using `number_format()` with the locale-specific decimal separator and no thousands separator, then suffixed with `%`.

#### Usage Example

```php
// Calculate how much a price changed
echo diffpercent(200, 250);  // Output: "25.0%" (25% increase)
echo diffpercent(200, 150);  // Output: "-25.0%" (25% decrease)
echo diffpercent(0, 100);    // Output: FALSE (division by zero prevented)
```

---

### format_number

Formats a numeric value with locale-appropriate separators, automatically choosing the number of decimal places based on whether the value has a fractional component.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int\|float` | The number to format |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The formatted number string with locale-specific decimal and thousands separators |

#### Inner Mechanisms

1. Uses `fmod($value, 1)` to check if the value has a fractional part.
2. If it does, formats with 2 decimal places; otherwise, formats with 0 decimal places.
3. Applies `number_format()` with `CMS_L_DECIMAL_SEPARATOR` and `CMS_L_THOUSAND_SEPARATOR` for locale-aware output.

#### Usage Example

```php
echo format_number(1234.5);   // Output: "1,234.50" (or locale equivalent)
echo format_number(1234);     // Output: "1,234" (or locale equivalent)
echo format_number(999999.99); // Output: "999,999.99" (or locale equivalent)
```

---

### format_bytesize

Converts a byte count into a human-readable string with the appropriate unit (Byte, KB, MB, or GB).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int\|float` | The size in bytes to format |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | A human-readable size string with unit suffix (e.g., `"1.50 KB"`, `"250 MB"`) |

#### Inner Mechanisms

1. If the value is less than 1024, returns the raw value followed by `" Byte"`.
2. If less than 1,048,576 (1 MB), divides by 1024 and formats as KB.
3. If less than 1,073,741,824 (1 GB), divides by 1,048,576 and formats as MB.
4. Otherwise, divides by 1,073,741,824 and formats as GB.
5. All larger units use `number_format()` with 2 decimal places and the locale-specific decimal separator. The thousands separator is `NULL` (no separator).

#### Usage Example

```php
echo format_bytesize(512);           // Output: "512 Byte"
echo format_bytesize(1536);          // Output: "1.50 KB"
echo format_bytesize(2621440);       // Output: "2.50 MB"
echo format_bytesize(1073741824);    // Output: "1.00 GB"
```

---

### flag

Checks whether a specific flag (bit) is set within a bitmask.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$bitmask` | `int` | The combined bitmask value containing multiple flags |
| `$flag` | `int` | The specific flag (single bit) to check for |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the flag is set in the bitmask; `FALSE` otherwise |

#### Inner Mechanisms

Performs a bitwise AND operation (`&`) between `$bitmask` and `$flag`. If the result is non-zero, the flag is present. The result is cast to `bool` for a clean boolean return.

#### Usage Example

```php
define('PERM_READ', 1);
define('PERM_WRITE', 2);
define('PERM_EXECUTE', 4);

$permissions = PERM_READ | PERM_WRITE; // 3

var_dump(flag($permissions, PERM_READ));    // Output: true
var_dump(flag($permissions, PERM_EXECUTE)); // Output: false
```

---

### sgn

Returns the sign (signum) of a numeric value.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int\|float` | The value whose sign to determine |

#### Return Values

| Type | Description |
|------|-------------|
| `int` | `1` if positive, `-1` if negative, `0` if zero |

#### Inner Mechanisms

A simple conditional check:
- If `$value > 0`, returns `1`.
- If `$value < 0`, returns `-1`.
- Otherwise (zero), returns `0`.

#### Usage Example

```php
echo sgn(42);    // Output: 1
echo sgn(-7);    // Output: -1
echo sgn(0);     // Output: 0
```

---

### dimension_to_px

Converts a CSS dimension string (e.g., `"10em"`, `"2.5cm"`) into an equivalent pixel value based on a reference root font size of 16px.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A CSS dimension string containing a number and optional unit (e.g., `"10px"`, `"2em"`, `"1.5cm"`) |

#### Return Values

| Type | Description |
|------|-------------|
| `int` | The equivalent pixel value (rounded) |
| `FALSE` | If the input string does not match the expected format or contains no valid number |

#### Inner Mechanisms

1. Uses a regular expression to parse the input into a numeric part and a unit part. The regex supports scientific notation.
2. If no number is found, returns `FALSE`.
3. If no unit is found, treats the number as pixels and returns it rounded.
4. Uses a static lookup table (`$list`) that maps CSS units to their pixel conversion factors, all based on a 16px root font size:
   - Absolute units: `px`, `pt`, `pc`, `in`, `cm`, `mm`, `Q`
   - Relative units: `em`, `ex`, `ch`, `ic`, `lh`
   - Root-relative units: `rem`, `rex`, `rch`, `ric`, `rlh`, `rcap`
5. If the unit is recognized, multiplies the number by the conversion factor and returns the rounded result.
6. If the unit is not recognized, returns `FALSE`.

#### Usage Example

```php
echo dimension_to_px("10px");   // Output: 10
echo dimension_to_px("2em");    // Output: 32 (2 * 16)
echo dimension_to_px("1in");    // Output: 96 (1 * 16 * 6)
echo dimension_to_px("2.54cm"); // Output: 96 (2.54 * 16 * 6 / 2.54)
echo dimension_to_px("abc");    // Output: FALSE
```

---

### base62

Encodes a non-negative integer into a base62 string using digits `0-9`, lowercase letters `a-z`, and uppercase letters `A-Z`.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | The non-negative integer to encode |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The base62-encoded representation of the input value |

#### Inner Mechanisms

1. Defines the character set: `"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"` (62 characters).
2. Uses a `do-while` loop to repeatedly divide the value by 62:
   - The remainder (`$value % 62`) selects a character from the set.
   - The character is prepended to the result string.
   - The value is updated to the integer quotient (`$value / 62`).
3. The loop continues until the value reaches zero.
4. This ensures that even an input of `0` produces the output `"0"`.

#### Usage Example

```php
echo base62(0);     // Output: "0"
echo base62(61);    // Output: "z"
echo base62(62);    // Output: "10"
echo base62(12345); // Output: "d7d"
```

---

### hamming_distance

Calculates the Hamming distance between two strings—the number of positions at which the corresponding characters differ.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string1` | `string` | The first string to compare |
| `$string2` | `string` | The second string to compare |

#### Return Values

| Type | Description |
|------|-------------|
| `int` | The number of differing character positions between the two strings |

#### Inner Mechanisms

1. Determines the maximum length of the two strings using `max(strlen(...))`.
2. Iterates over each character position from `0` to the maximum length.
3. At each position, compares the characters from both strings using the null coalescing operator (`??`) to handle strings of unequal length (missing characters are treated as empty strings).
4. Increments a counter for each position where the characters differ.
5. Returns the total count of differing positions.

#### Usage Example

```php
echo hamming_distance("karolin", "kathrin"); // Output: 3
echo hamming_distance("hello", "hello");     // Output: 0
echo hamming_distance("abc", "abcd");        // Output: 1 (the extra 'd' differs)
echo hamming_distance("", "abc");            // Output: 3 (all positions differ)
```


<!-- HASH:46b59141383d031583ce18f989d6edf4 -->
