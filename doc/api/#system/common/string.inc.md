# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/string.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## String Utility Functions (`string.inc`)

This file provides a collection of string manipulation utilities for the NUOS platform. These functions handle common tasks such as normalization, formatting, validation, and transformation of strings in a multibyte-safe and locale-aware manner. They are designed to be lightweight, dependency-free, and context-aware for use in both frontend and backend modules.

---

### `strtoalphanum`

| **Purpose**               | Converts a string to alphanumeric characters, replacing non-alphanumeric characters with a specified replacement. Trims leading/trailing replacements and collapses multiple replacements into one. |
|---------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                                                                                             |

| Name          | Type     | Default | Description                                                                                     |
|---------------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$string`     | string   | —       | Input string to process.                                                                        |
| `$replacement`| string   | `" "`   | String used to replace non-alphanumeric characters. If empty, non-alphanumeric chars are removed.|

| **Return Value** | string — The processed string containing only alphanumeric characters and the replacement string. |
|------------------|----------------------------------------------------------------------------------------------------|
| **Mechanism**    | Uses `htmlentities_decode` to decode HTML entities, then applies a Unicode-aware regex (`\p{L}`, `\p{N}`) to retain letters and numbers. Trims and normalizes replacement strings. |
| **Usage**        | Useful for generating clean slugs, filenames, or identifiers from user input (e.g., titles, names). |

---

### `strtonum`

| **Purpose**               | Extracts and converts a numeric value from a string, respecting locale-specific thousand and decimal separators. |
|---------------------------|-------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                   |

| Name      | Type   | Default | Description                     |
|-----------|--------|---------|---------------------------------|
| `$string` | string | —       | Input string containing a number.|

| **Return Value** | float|FALSE — The extracted number as a float, or `FALSE` if no number is found. |
|------------------|--------------------------------------------------------------------------------|
| **Mechanism**    | Uses regex to find a numeric pattern including locale-specific separators (`CMS_L_THOUSAND_SEPARATOR`, `CMS_L_DECIMAL_SEPARATOR`). Strips thousand separators and replaces decimal separator with `.` before converting to float. |
| **Usage**        | Ideal for parsing user-inputted numbers in forms (e.g., prices, quantities) where locale formatting is used. |

---

### `stripspaces`

| **Purpose**               | Normalizes whitespace in a string: trims leading/trailing spaces, collapses internal spaces, and optionally preserves newlines. |
|---------------------------|------------------------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                                    |

| Name                  | Type    | Default | Description                                                                 |
|-----------------------|---------|---------|-----------------------------------------------------------------------------|
| `$string`             | string  | —       | Input string to process.                                                    |
| `$preserve_newlines`  | bool    | `FALSE` | If `TRUE`, preserves newlines and collapses only horizontal whitespace.     |
| `$limit_empty_lines`  | bool    | `TRUE`  | If `TRUE` and `$preserve_newlines` is `TRUE`, limits consecutive newlines to 2. |

| **Return Value** | string — The normalized string with consistent whitespace. |
|------------------|-------------------------------------------------------------|
| **Mechanism**    | Uses `preg_replace` with Unicode-aware patterns to normalize spaces. When newlines are preserved, it ensures clean paragraph separation. |
| **Usage**        | Commonly used for cleaning user-generated content (e.g., comments, articles) before display or storage. |

---

### `nl2br`

| **Purpose**               | Converts newlines (`\n`) to `<br>` tags, with special handling for multiple consecutive newlines. |
|---------------------------|----------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                    |

| Name      | Type   | Default | Description                     |
|-----------|--------|---------|---------------------------------|
| `$string` | string | —       | Input string containing newlines.|

| **Return Value** | string — String with newlines replaced by `<br>` tags. Multiple newlines get a special class (`<br class="multiple">`). |
|------------------|---------------------------------------------------------------------------------------------------------------------------|
| **Mechanism**    | First normalizes `\r\n` to `\n`, then uses regex lookbehind to identify consecutive newlines and apply a distinct class. |
| **Usage**        | Used in rendering text content (e.g., blog posts, forum messages) where line breaks should be preserved in HTML output. |

---

### `limitstr`

| **Purpose**               | Truncates a string to a specified length, intelligently splitting at a separator (e.g., comma) to avoid breaking words. |
|---------------------------|---------------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                           |

| Name        | Type   | Default | Description                                                                 |
|-------------|--------|---------|-----------------------------------------------------------------------------|
| `$string`   | string | —       | Input string to truncate.                                                  |
| `$length`   | int    | 255     | Maximum allowed length of the output string.                               |
| `$separator`| string | `","`   | Character or string used to split the input for intelligent truncation.    |

| **Return Value** | string — The truncated string, possibly shorter than `$length` if a separator is found. |
|------------------|-------------------------------------------------------------------------------------------|
| **Mechanism**    | Splits the string by the separator, then reassembles parts until the length limit is reached. Uses `utf8_*` functions for multibyte safety. |
| **Usage**        | Useful for displaying excerpts (e.g., in lists or previews) where readability is important. |

---

### `first_paragraph`

| **Purpose**               | Extracts the first paragraph from a string, defined as the initial sequence of non-vertical whitespace characters. |
|---------------------------|----------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                      |

| Name      | Type   | Default | Description                     |
|-----------|--------|---------|---------------------------------|
| `$string` | string | —       | Input string containing text.   |

| **Return Value** | string — The first paragraph, or the entire string if no paragraph break is found. |
|------------------|--------------------------------------------------------------------------------------|
| **Mechanism**    | Uses regex with `\V` (non-vertical whitespace) to match the first block of text.    |
| **Usage**        | Ideal for generating lead text or summaries from longer content.                     |

---

### `first_words`

| **Purpose**               | Extracts the first `$length` characters of a string, ensuring the result ends at a word boundary. Appends an ellipsis if truncated. |
|---------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                                                          |

| Name        | Type   | Default | Description                                                                 |
|-------------|--------|---------|-----------------------------------------------------------------------------|
| `$string`   | string | —       | Input string to truncate.                                                  |
| `$length`   | int    | 250     | Maximum number of characters to return.                                    |
| `$ellipsis` | string | `" …"`  | String appended if the text is truncated.                                  |

| **Return Value** | string — The truncated string ending at a word boundary, with ellipsis if applicable. |
|------------------|-----------------------------------------------------------------------------------------|
| **Mechanism**    | Uses regex with a positive lookahead for word separators (`CMS_REGEX_SEPARATOR`) to find a safe truncation point. Uses `utf8_*` functions. |
| **Usage**        | Commonly used for generating teaser text or previews in content listings.              |

---

### `zerofill`

| **Purpose**               | Pads a string with null bytes (`\0`) to reach a specified length. |
|---------------------------|-------------------------------------------------------------------|
| **Parameters**            |                                                                   |

| Name      | Type   | Default | Description                                 |
|-----------|--------|---------|---------------------------------------------|
| `$string` | string | —       | Input string to pad.                        |
| `$length` | int    | —       | Desired total length of the output string.  |

| **Return Value** | string — The input string padded with null bytes to the specified length. |
|------------------|----------------------------------------------------------------------------|
| **Mechanism**    | Uses `str_repeat` to append null bytes.                                   |
| **Usage**        | Used in low-level data formatting (e.g., binary protocols, fixed-width records). |

---

### `stre`, `nstre`, `streq`, `nstreq`

| **Purpose**               | String emptiness and equality checks. |
|---------------------------|---------------------------------------|
| **Parameters**            |                                       |

| Function  | Parameters                     | Return Type | Description                                                                 |
|-----------|---------------------------------|-------------|-----------------------------------------------------------------------------|
| `stre`    | `$string` (string)              | bool        | Returns `TRUE` if the string is empty (`""`).                              |
| `nstre`   | `$string` (string)              | bool        | Returns `TRUE` if the string is **not** empty.                             |
| `streq`   | `$string1`, `$string2` (string) | bool        | Returns `TRUE` if the two strings are identical.                           |
| `nstreq`  | `$string1`, `$string2` (string) | bool        | Returns `TRUE` if the two strings are **not** identical.                   |

| **Mechanism**    | All functions cast input to string before comparison. No multibyte handling — intended for simple checks. |
|------------------|-------------------------------------------------------------------------------------------------------------|
| **Usage**        | Used throughout the codebase for input validation, conditional logic, and state checks.                    |

---

### `strieq`

| **Purpose**               | Case-insensitive string equality check using regex. |
|---------------------------|-----------------------------------------------------|
| **Parameters**            |                                                     |

| Name       | Type   | Default | Description                     |
|------------|--------|---------|---------------------------------|
| `$string1` | string | —       | Pattern string.                 |
| `$string2` | string | —       | String to test against pattern. |

| **Return Value** | bool — `TRUE` if `$string2` matches `$string1` case-insensitively. |
|------------------|--------------------------------------------------------------------|
| **Mechanism**    | Uses `preg_match` with the `i` (case-insensitive) and `u` (Unicode) flags. The pattern is escaped with `preg_quote`. |
| **Usage**        | Useful for comparing user input against known values (e.g., commands, tags) without case sensitivity. |

---

### `substri_count`

| **Purpose**               | Counts the number of occurrences of a substring in a string, case-insensitively. |
|---------------------------|-----------------------------------------------------------------------------------|
| **Parameters**            |                                                                                   |

| Name      | Type   | Default | Description                     |
|-----------|--------|---------|---------------------------------|
| `$source` | string | —       | String to search within.        |
| `$string` | string | —       | Substring to count.             |

| **Return Value** | int — Number of occurrences. |
|------------------|------------------------------|
| **Mechanism**    | Uses `preg_match_all` with case-insensitive and Unicode flags. The substring is escaped with `preg_quote`. |
| **Usage**        | Used in text analysis, tag counting, or keyword density checks. |

---

### `verify_email`

| **Purpose**               | Validates an email address using a strict regex pattern. |
|---------------------------|-----------------------------------------------------------|
| **Parameters**            |                                                           |

| Name    | Type   | Default | Description                     |
|---------|--------|---------|---------------------------------|
| `$email`| string | —       | Email address to validate.      |

| **Return Value** | bool — `TRUE` if the email matches the pattern, `FALSE` otherwise. |
|------------------|--------------------------------------------------------------------|
| **Mechanism**    | Uses a regex that enforces:
- Local part: alphanumeric + `.`, `-`, `_`, and Unicode letters
- Domain: alphanumeric + `-`, `.`, and Unicode letters
- TLD: 2–4 letters
Supports international characters (e.g., `äöü@domain.de`). |
| **Usage**        | Used in user registration, contact forms, and email input validation. |

---

### `unique_id`

| **Purpose**               | Generates a random alphanumeric string of a specified length. |
|---------------------------|---------------------------------------------------------------|
| **Parameters**            |                                                               |

| Name    | Type | Default | Description                     |
|---------|------|---------|---------------------------------|
| `$count`| int  | 8       | Length of the generated ID.     |

| **Return Value** | string — A random string composed of digits and uppercase/lowercase letters. |
|------------------|------------------------------------------------------------------------------|
| **Mechanism**    | Uses `mt_rand` to select characters from a static alphabet. Not cryptographically secure. |
| **Usage**        | Used for generating temporary tokens, session IDs, or unique filenames.      |

---

### `strabridge`

| **Purpose**               | Truncates a string to a specified length, adding an ellipsis. Supports smart truncation (middle or end). |
|---------------------------|----------------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                          |

| Name         | Type    | Default | Description                                                                 |
|--------------|---------|---------|-----------------------------------------------------------------------------|
| `$string`    | string  | —       | Input string to truncate.                                                  |
| `$length`    | int     | 40      | Maximum length of the output string (including ellipsis).                  |
| `$cut_end`   | bool    | `FALSE` | If `TRUE`, truncates from the end; if `FALSE`, truncates from the middle.  |

| **Return Value** | string — The truncated string with ellipsis. |
|------------------|-----------------------------------------------|
| **Mechanism**    | Uses `utf8_*` functions. For middle truncation, 65% of the length is taken from the start, 35% from the end. |
| **Usage**        | Used in UI elements (e.g., breadcrumbs, lists) where space is limited and readability is important. |

---

### `generate_pseudonym`

| **Purpose**               | Generates a random, pronounceable pseudonym using Japanese-inspired syllables. |
|---------------------------|---------------------------------------------------------------------------------|
| **Parameters**            |                                                                                 |

| Name        | Type   | Default | Description                                                                 |
|-------------|--------|---------|-----------------------------------------------------------------------------|
| `$seed`     | string | `NULL`  | Optional seed for reproducible results.                                    |
| `$syllables`| int    | 3       | Number of syllables in the generated name.                                 |

| **Return Value** | string — A capitalized pseudonym (e.g., "Kamiyo", "Sorena"). |
|------------------|---------------------------------------------------------------|
| **Mechanism**    | Uses a predefined array of syllables. If a seed is provided, `srand(crc32($seed))` is used for reproducibility. The last syllable may include "n". |
| **Usage**        | Used for generating usernames, display names, or placeholder text in demos or testing. |

---

### `strtocolor`

| **Purpose**               | Converts a string into a deterministic HSL color, optionally wrapped in a styled `<span>` tag. |
|---------------------------|--------------------------------------------------------------------------------------------------|
| **Parameters**            |                                                                                                  |

| Name         | Type    | Default | Description                                                                 |
|--------------|---------|---------|-----------------------------------------------------------------------------|
| `$string`    | string  | —       | Input string used to generate the color.                                   |
| `$lightness` | int     | 75      | Lightness value (0–100) for the HSL color.                                 |
| `$span`      | bool    | `TRUE`  | If `TRUE`, returns an HTML `<span>` with the color applied; otherwise returns the HSL string. |

| **Return Value** | string — Either an HSL color string (e.g., `hsl(120,75%,75%)`) or an HTML `<span>` element. |
|------------------|-----------------------------------------------------------------------------------------------|
| **Mechanism**    | Uses `crc32` of the string to generate a hue (0–359), then constructs an HSL color. Uses `x()` for XML escaping. |
| **Usage**        | Used for visual differentiation of tags, users, or categories in the UI.                     |


<!-- HASH:63a86fd00c22de8ba4c383d862c5da81 -->
