# PWNC API Documentation

[← Index](../README.md) | [`javascript/md5.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/md5.js)

- **Version:** `26.6.19.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MD5 Hashing Utility

This file provides a pure JavaScript implementation of the MD5 hashing algorithm. It is used for generating 128-bit (16-byte) hash values, typically rendered as 32-character hexadecimal strings. The implementation is self-contained and does not rely on external libraries, aligning with PWNC's zero-dependency philosophy.

### Core Functions

#### `md5_hex(num)`
Converts a 32-bit number into an 8-character hexadecimal string.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `num`     | number | 32-bit integer to be converted       |

**Return Value:**
- `string`: 8-character hexadecimal representation of `num`.

**Inner Mechanisms:**
- Uses a character map (`0123456789abcdef`) to convert each 4-bit segment of the 32-bit number into its corresponding hexadecimal character.
- Processes the number in 4-byte chunks, extracting each nibble (4 bits) via bitwise operations.

**Usage Context:**
- Internal utility for converting the final MD5 hash state into a human-readable string.
- Used by `md5()` to format the output.

**Example:**
```javascript
console.log(md5_hex(255)); // Output: "000000ff"
```

---

#### `md5_convert(str)`
Converts an input string into an array of 32-bit integers, padding it according to the MD5 specification.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `str`     | string | Input string to be converted         |

**Return Value:**
- `Array<number>`: Array of 32-bit integers representing the padded input string.

**Inner Mechanisms:**
- Calculates the required block count (each block is 512 bits or 16 32-bit words).
- Initializes an array of zeros with length `c * 16`.
- Fills the array by placing each character's ASCII value into the appropriate 32-bit word, using bitwise OR and shifts.
- Appends the `0x80` byte (binary `10000000`) to mark the end of the string.
- Sets the last two words to the original string length in bits (little-endian).

**Usage Context:**
- Prepares the input string for MD5 processing by converting it into the required 512-bit block format.

**Example:**
```javascript
const blocks = md5_convert("hello");
console.log(blocks); // Output: [1819043144, 1867980912, 1684828783, 0, 0x28, ...]
```

---

#### `md5_add(a, b)`
Performs 32-bit addition with wrap-around (modulo 2³²).

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | number | First 32-bit integer                 |
| `b`       | number | Second 32-bit integer                |

**Return Value:**
- `number`: Result of `a + b` as a 32-bit integer.

**Inner Mechanisms:**
- Splits both numbers into lower and upper 16-bit halves.
- Adds the lower halves, carrying over any overflow to the upper halves.
- Combines the results into a 32-bit integer.

**Usage Context:**
- Core arithmetic operation for MD5, ensuring results stay within 32 bits.

**Example:**
```javascript
console.log(md5_add(0xFFFFFFFF, 1)); // Output: 0 (32-bit wrap-around)
```

---

#### `md5_shift_bit(num, cnt)`
Performs a circular left shift (rotate) of a 32-bit number by `cnt` bits.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `num`     | number | 32-bit integer to rotate             |
| `cnt`     | number | Number of bits to shift              |

**Return Value:**
- `number`: Result of rotating `num` left by `cnt` bits.

**Inner Mechanisms:**
- Uses bitwise left shift (`<<`) and unsigned right shift (`>>>`) to achieve rotation.
- Combines the shifted parts to form the rotated 32-bit integer.

**Usage Context:**
- Fundamental operation in MD5 for mixing bits during hash computation.

**Example:**
```javascript
console.log(md5_shift_bit(0x80000000, 1).toString(16)); // Output: "00000001" (rotated left by 1)
```

---

#### `md5_cmn(q, a, b, x, s, t)`
Core MD5 round function. Combines inputs using bitwise operations and modular addition.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `q`       | number | Round-specific function result       |
| `a`       | number | Current hash state variable          |
| `b`       | number | Current hash state variable          |
| `x`       | number | Current 32-bit word from input       |
| `s`       | number | Shift amount                         |
| `t`       | number | Constant for the round               |

**Return Value:**
- `number`: Updated hash state variable.

**Inner Mechanisms:**
- Computes `a + q + x + t`, then rotates the result left by `s` bits, and adds `b`.
- Used as a building block for the four MD5 round functions (`md5_ff`, `md5_gg`, `md5_hh`, `md5_ii`).

**Usage Context:**
- Internal utility for MD5 rounds. Not typically called directly.

---

#### `md5_ff(a, b, c, d, x, s, t)`
MD5 round 1 function.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | number | Current hash state variable          |
| `b`       | number | Current hash state variable          |
| `c`       | number | Current hash state variable          |
| `d`       | number | Current hash state variable          |
| `x`       | number | Current 32-bit word from input       |
| `s`       | number | Shift amount                         |
| `t`       | number | Constant for the round               |

**Return Value:**
- `number`: Updated hash state variable.

**Inner Mechanisms:**
- Implements the boolean function `(b & c) | ((~b) & d)`.
- Passes the result to `md5_cmn` along with other parameters.

**Usage Context:**
- First of four rounds in the MD5 algorithm. Processes 16 words per block.

---

#### `md5_gg(a, b, c, d, x, s, t)`
MD5 round 2 function.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | number | Current hash state variable          |
| `b`       | number | Current hash state variable          |
| `c`       | number | Current hash state variable          |
| `d`       | number | Current hash state variable          |
| `x`       | number | Current 32-bit word from input       |
| `s`       | number | Shift amount                         |
| `t`       | number | Constant for the round               |

**Return Value:**
- `number`: Updated hash state variable.

**Inner Mechanisms:**
- Implements the boolean function `(b & d) | (c & (~d))`.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Second round of the MD5 algorithm.

---

#### `md5_hh(a, b, c, d, x, s, t)`
MD5 round 3 function.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | number | Current hash state variable          |
| `b`       | number | Current hash state variable          |
| `c`       | number | Current hash state variable          |
| `d`       | number | Current hash state variable          |
| `x`       | number | Current 32-bit word from input       |
| `s`       | number | Shift amount                         |
| `t`       | number | Constant for the round               |

**Return Value:**
- `number`: Updated hash state variable.

**Inner Mechanisms:**
- Implements the boolean function `b ^ c ^ d`.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Third round of the MD5 algorithm.

---

#### `md5_ii(a, b, c, d, x, s, t)`
MD5 round 4 function.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | number | Current hash state variable          |
| `b`       | number | Current hash state variable          |
| `c`       | number | Current hash state variable          |
| `d`       | number | Current hash state variable          |
| `x`       | number | Current 32-bit word from input       |
| `s`       | number | Shift amount                         |
| `t`       | number | Constant for the round               |

**Return Value:**
- `number`: Updated hash state variable.

**Inner Mechanisms:**
- Implements the boolean function `c ^ (b | (~d))`.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Fourth and final round of the MD5 algorithm.

---

#### `md5(str)`
Generates the MD5 hash of an input string.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `str`     | string | Input string to hash                 |

**Return Value:**
- `string`: 32-character hexadecimal MD5 hash of `str`.

**Inner Mechanisms:**
1. Converts the input string into 32-bit word blocks using `md5_convert`.
2. Initializes the hash state variables (`a`, `b`, `c`, `d`) with MD5 constants.
3. Processes each 512-bit block through four rounds of 16 operations each, using `md5_ff`, `md5_gg`, `md5_hh`, and `md5_ii`.
4. Updates the hash state variables after each round.
5. Adds the final state to the initial state to produce the hash.
6. Converts the final state into a hexadecimal string using `md5_hex`.

**Usage Context:**
- Primary function for generating MD5 hashes in the PWNC platform.
- Used for checksums, data integrity verification, or lightweight security applications (e.g., gravatar URLs, cache keys).

**Example:**
```javascript
// Generate MD5 hash of a password (for demonstration; not secure for passwords)
const passwordHash = md5("myPassword123");
console.log(passwordHash); // Output: "482c811da5d5b4bc6d497ffa98491e38"

// Generate a gravatar URL
const email = "user@example.com";
const gravatarUrl = `https://www.gravatar.com/avatar/${md5(email.trim().toLowerCase())}`;
console.log(gravatarUrl); // Output: "https://www.gravatar.com/avatar/b58996c504c5638798eb6b511e6f49af"
```


<!-- HASH:669960f4556194326cdcc8e141241680 -->
