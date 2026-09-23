# PWNC API Documentation

[← Index](../README.md) | [`javascript/md5.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/md5.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## md5.js

The `md5.js` file is a client-side JavaScript implementation of the MD5 (Message-Digest Algorithm 5) hashing function. It provides a lightweight, dependency-free way to compute MD5 hashes directly in the browser. This is typically used for generating checksums, data integrity verification, or creating identifiers from strings.

### md5_hex

Converts a 32-bit number into its hexadecimal string representation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `num`     | Number | A 32-bit integer to be converted to hex |

**Returns:** A string containing the 8-character hexadecimal representation of the input number.

**Inner Mechanism:** Iterates over each byte of the 32-bit number, extracting and converting each nibble (4 bits) to its corresponding hexadecimal character using bit shifting and masking operations.

**Usage Example:**
```javascript
var hex = md5_hex(1732584193); // Returns "00490000"
```

### md5_convert

Converts a string into an array of 32-bit words, applying MD5 padding rules.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str`     | String | The input string to be converted |

**Returns:** An array of 32-bit integers representing the padded message.

**Inner Mechanism:** Calculates the required number of 16-word blocks based on the string length, initializes the array with zeros, then fills it with the string's character codes. Applies MD5 padding by appending a 0x80 byte and storing the original message length (in bits) in the last two words.

**Usage Example:**
```javascript
var words = md5_convert("hello"); // Returns padded word array
```

### md5_add

Performs 32-bit addition of two numbers, handling overflow correctly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a`       | Number | First 32-bit integer operand |
| `b`       | Number | Second 32-bit integer operand |

**Returns:** A 32-bit integer representing the sum of `a` and `b`.

**Inner Mechanism:** Separates the operands into low and high 16-bit portions, performs addition on each part, and combines them back while properly handling carry-over from the lower 16 bits to the upper 16 bits.

**Usage Example:**
```javascript
var result = md5_add(100, 200); // Returns 300
```

### md5_shift_bit

Performs a left bitwise rotation (circular shift) on a 32-bit number.

| Parameter | Type | Description |
|-----------|------|-------------|
| `num`     | Number | The 32-bit integer to rotate |
| `cnt`     | Number | The number of bits to rotate left |

**Returns:** A 32-bit integer rotated left by `cnt` bits.

**Inner Mechanism:** Uses JavaScript's bitwise operators to perform a left shift and an unsigned right shift, then combines the results to achieve a circular rotation effect.

**Usage Example:**
```javascript
var rotated = md5_shift_bit(0x12345678, 4); // Rotates bits left by 4 positions
```

### md5_cmn

Common operation used across all four MD5 rounds, combining addition, rotation, and another addition.

| Parameter | Type | Description |
|-----------|------|-------------|
| `q`       | Number | Intermediate value from the round function |
| `a`       | Number | Current state value A |
| `b`       | Number | Current state value B |
| `x`       | Number | Input word from the message schedule |
| `s`       | Number | Rotation amount for this step |
| `t`       | Number | Constant value for this step |

**Returns:** A 32-bit integer representing the updated state value.

**Inner Mechanism:** Adds the intermediate value `q` to `a`, adds the input word `x` and constant `t`, rotates the result left by `s` bits, then adds `b` to produce the final result.

**Usage Example:**
```javascript
var result = md5_cmn(0x12345678, 0x9E1E2D2D, 0x3B830B2D, 0x5A827999, 7, -680876936);
```

### md5_ff

First round function of the MD5 algorithm, using the F operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a`       | Number | Current state value A |
| `b`       | Number | Current state value B |
| `c`       | Number | Current state value C |
| `d`       | Number | Current state value D |
| `x`       | Number | Input word from the message schedule |
| `s`       | Number | Rotation amount for this step |
| `t`       | Number | Constant value for this step |

**Returns:** A 32-bit integer representing the updated state value.

**Inner Mechanism:** Applies the F function `(b & c) | ((~b) & d)` to the state values, then passes the result along with other parameters to `md5_cmn`.

**Usage Example:**
```javascript
var result = md5_ff(0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0x12345678, 7, -680876936);
```

### md5_gg

Second round function of the MD5 algorithm, using the G operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a`       | Number | Current state value A |
| `b`       | Number | Current state value B |
| `c`       | Number | Current state value C |
| `d`       | Number | Current state value D |
| `x`       | Number | Input word from the message schedule |
| `s`       | Number | Rotation amount for this step |
| `t`       | Number | Constant value for this step |

**Returns:** A 32-bit integer representing the updated state value.

**Inner Mechanism:** Applies the G function `(b & d) | (c & (~d))` to the state values, then passes the result along with other parameters to `md5_cmn`.

**Usage Example:**
```javascript
var result = md5_gg(0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0x12345678, 5, -165796510);
```

### md5_hh

Third round function of the MD5 algorithm, using the H operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a`       | Number | Current state value A |
| `b`       | Number | Current state value B |
| `c`       | Number | Current state value C |
| `d`       | Number | Current state value D |
| `x`       | Number | Input word from the message schedule |
| `s`       | Number | Rotation amount for this step |
| `t`       | Number | Constant value for this step |

**Returns:** A 32-bit integer representing the updated state value.

**Inner Mechanism:** Applies the H function `b ^ c ^ d` (XOR of all three state values) to the state values, then passes the result along with other parameters to `md5_cmn`.

**Usage Example:**
```javascript
var result = md5_hh(0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0x12345678, 4, -378558);
```

### md5_ii

Fourth round function of the MD5 algorithm, using the I operation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a`       | Number | Current state value A |
| `b`       | Number | Current state value B |
| `c`       | Number | Current state value C |
| `d`       | Number | Current state value D |
| `x`       | Number | Input word from the message schedule |
| `s`       | Number | Rotation amount for this step |
| `t`       | Number | Constant value for this step |

**Returns:** A 32-bit integer representing the updated state value.

**Inner Mechanism:** Applies the I function `c ^ (b | (~d))` to the state values, then passes the result along with other parameters to `md5_cmn`.

**Usage Example:**
```javascript
var result = md5_ii(0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0x12345678, 6, -198630844);
```

### md5

Computes the MD5 hash of a given string and returns it as a hexadecimal string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str`     | String | The input string to hash |

**Returns:** A string containing the 32-character hexadecimal MD5 hash of the input.

**Inner Mechanism:** Initializes the MD5 state variables with standard constants, converts the input string to padded word arrays, processes each 16-word block through four rounds of operations (FF, GG, HH, II), updates the state variables after each block, and finally concatenates the hexadecimal representations of the four state variables.

**Usage Example:**
```javascript
var hash = md5("Hello, World!"); // Returns "65a8e27d8b572f10b8637a5bb8c0c5aa"
```


<!-- HASH:669960f4556194326cdcc8e141241680 -->
