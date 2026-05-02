# NUOS API Documentation

[← Index](../README.md) | [`javascript/md5.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## MD5 Hashing Utility

This file implements the MD5 hashing algorithm in pure JavaScript. It provides functions to generate an MD5 hash (128-bit fingerprint) of any given string input. The implementation follows the RFC 1321 specification and is used for data integrity checks, password hashing, and other cryptographic applications within the NUOS platform.

---

### `md5_hex(num)`

Converts a 32-bit number into an 8-character hexadecimal string representation.

#### Parameters

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| num  | number | 32-bit integer to be converted to hex |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| string | 8-character lowercase hexadecimal string         |

#### Inner Mechanisms

- Iterates over each byte (4 bytes total) of the 32-bit number.
- Extracts the high and low nibble (4 bits) of each byte.
- Maps each nibble to its corresponding hexadecimal character using a lookup table.
- Concatenates the results to form the final 8-character string.

#### Usage

Used internally by `md5()` to convert the final hash state (four 32-bit words) into a human-readable hexadecimal string.

---

### `md5_convert(str)`

Converts an input string into an array of 32-bit words suitable for MD5 processing.

#### Parameters

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| str  | string | Input string to be converted         |

#### Return Value

| Type     | Description                                      |
|----------|--------------------------------------------------|
| number[] | Array of 32-bit integers representing the input  |

#### Inner Mechanisms

- Calculates the required number of 512-bit blocks (16 × 32-bit words per block) needed to store the input, including padding.
- Initializes an array of zeros with the appropriate length.
- Copies each character of the input string into the array, packing 4 characters into each 32-bit word (little-endian).
- Appends the mandatory `0x80` bit (128 in decimal) to mark the end of the data.
- Stores the original bit length of the input in the last two words of the final block (little-endian).

#### Usage

Called by `md5()` to prepare the input string for hashing. Ensures proper padding and length encoding as per MD5 specification.

---

### `md5_add(a, b)`

Performs 32-bit unsigned addition with wrap-around (modulo 2³²).

#### Parameters

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| a    | number | First 32-bit operand                 |
| b    | number | Second 32-bit operand                |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| number | Result of (a + b) mod 2³²                        |

#### Inner Mechanisms

- Splits both operands into low and high 16-bit words.
- Adds the low words, capturing any carry.
- Adds the high words along with the carry from the low addition.
- Combines the results into a 32-bit integer using bitwise operations.

#### Usage

Used throughout the MD5 algorithm to perform safe 32-bit arithmetic, preventing overflow in JavaScript’s native number type.

---

### `md5_shift_bit(num, cnt)`

Performs a circular left shift (rotate) of a 32-bit number by a specified number of bits.

#### Parameters

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| num  | number | 32-bit number to rotate              |
| cnt  | number | Number of bits to shift (0–31)       |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| number | Result of rotating `num` left by `cnt` bits      |

#### Inner Mechanisms

- Uses bitwise left shift (`<<`) and unsigned right shift (`>>>`) to simulate a circular rotation.
- Ensures the result remains within 32 bits.

#### Usage

Core operation in MD5 round functions to mix bits during hashing.

---

### `md5_cmn(q, a, b, x, s, t)`

General-purpose MD5 round function used by all four main round functions.

#### Parameters

| Name | Type   | Description                                      |
|------|--------|--------------------------------------------------|
| q    | number | Function-specific logic (e.g., bitwise operation)|
| a    | number | Accumulator (current hash state word)            |
| b    | number | Current hash state word                          |
| x    | number | Current message word from the block              |
| s    | number | Number of bits to rotate                         |
| t    | number | Constant value for this step                     |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| number | Updated value of the accumulator after the step  |

#### Inner Mechanisms

- Combines `a`, `q`, `x`, and `t` using addition.
- Rotates the result left by `s` bits.
- Adds `b` to the result.
- Returns the new value for the accumulator.

#### Usage

Called by `md5_ff`, `md5_gg`, `md5_hh`, and `md5_ii` to perform each step in the MD5 compression function.

---

### `md5_ff(a, b, c, d, x, s, t)`

Round 1 function of the MD5 algorithm.

#### Parameters

| Name | Type   | Description                                      |
|------|--------|--------------------------------------------------|
| a    | number | Accumulator (current hash state word)            |
| b    | number | Current hash state word                          |
| c    | number | Current hash state word                          |
| d    | number | Current hash state word                          |
| x    | number | Current message word from the block              |
| s    | number | Number of bits to rotate                         |
| t    | number | Constant value for this step                     |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| number | Updated value of the accumulator                 |

#### Inner Mechanisms

- Implements the logic: `(b & c) | ((~b) & d)`
- Calls `md5_cmn` with the computed value.

#### Usage

Used in the first 16 steps of each 512-bit block during MD5 processing.

---

### `md5_gg(a, b, c, d, x, s, t)`

Round 2 function of the MD5 algorithm.

#### Parameters

Same structure as `md5_ff`.

#### Return Value

Same as `md5_ff`.

#### Inner Mechanisms

- Implements the logic: `(b & d) | (c & (~d))`
- Calls `md5_cmn` with the computed value.

#### Usage

Used in the second 16 steps of each 512-bit block.

---

### `md5_hh(a, b, c, d, x, s, t)`

Round 3 function of the MD5 algorithm.

#### Parameters

Same structure as `md5_ff`.

#### Return Value

Same as `md5_ff`.

#### Inner Mechanisms

- Implements the logic: `b ^ c ^ d`
- Calls `md5_cmn` with the computed value.

#### Usage

Used in the third 16 steps of each 512-bit block.

---

### `md5_ii(a, b, c, d, x, s, t)`

Round 4 function of the MD5 algorithm.

#### Parameters

Same structure as `md5_ff`.

#### Return Value

Same as `md5_ff`.

#### Inner Mechanisms

- Implements the logic: `c ^ (b | (~d))`
- Calls `md5_cmn` with the computed value.

#### Usage

Used in the final 16 steps of each 512-bit block.

---

### `md5(str)`

Main function: computes the MD5 hash of a string.

#### Parameters

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| str  | string | Input string to hash                 |

#### Return Value

| Type   | Description                                      |
|--------|--------------------------------------------------|
| string | 32-character lowercase hexadecimal MD5 hash     |

#### Inner Mechanisms

- Initializes the four 32-bit hash state variables (`a`, `b`, `c`, `d`) with standard MD5 constants.
- Converts the input string into a padded array of 32-bit words using `md5_convert`.
- Processes each 512-bit block (16 words) through 64 steps (16 per round), using the four round functions.
- After each block, adds the result to the previous hash state (mod 2³²).
- Converts the final hash state into a 32-character hexadecimal string using `md5_hex`.

#### Usage

Primary entry point for generating MD5 hashes. Can be called directly:

```javascript
var hash = md5("Hello, NUOS!");
// Returns "6cd3556deb0da54bca060b4c39479839"
```

Used in NUOS for:
- Password hashing (with salt)
- File integrity checks
- Unique identifier generation
- Data fingerprinting in caching and asset management


<!-- HASH:5e57d30f177082c56890dc5fe7dce0b4 -->
