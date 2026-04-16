# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.captcha.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## CAPTCHA Class (`lib.captcha.inc`)

Core class for generating and verifying CAPTCHA challenges. Provides a simple, secure, and lightweight mechanism to prevent automated form submissions. Relies on the GD library for image generation and uses a daily-rotated secret key for verification.

---

### Class Properties

| Name      | Default | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| `secret`  | `NULL`  | Daily-rotated secret key used to generate and verify CAPTCHA codes.         |
| `length`  | `4`     | Number of characters in the CAPTCHA string.                                 |
| `enabled` | `FALSE` | Indicates whether CAPTCHA generation is enabled (GD library requirements). |

---

### Constructor: `__construct()`

#### Purpose
Initializes the CAPTCHA system. Checks for GD library support, rotates the secret key daily, and ensures the image storage directory exists.

#### Parameters
None.

#### Return Value
None (constructor).

#### Inner Mechanisms
1. **GD Library Check**: Verifies that the GD extension is loaded and supports PNG images.
2. **Secret Key Rotation**: Loads the current secret key from cache. If the key is outdated (older than the current day), generates a new one using `unique_id()` and updates the cache.
3. **Directory Setup**: Creates the `CMS_DATA_PATH . "captcha"` directory if it does not exist.
4. **Cleanup**: If the secret key is rotated, deletes all existing CAPTCHA images to prevent stale challenges.

#### Usage Context
Automatically invoked when the `captcha` class is instantiated. Typically used once per request to ensure the CAPTCHA system is ready.

---

### Method: `create()`

#### Purpose
Generates a CAPTCHA image and returns its verification code.

#### Parameters
None.

#### Return Value
| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `string`| Verification code (filename of the generated CAPTCHA image). Returns `FALSE` if CAPTCHA is disabled. |

#### Inner Mechanisms
1. **Key Generation**: Creates a random lowercase string of length `$this->length`.
2. **Verification Code**: Computes a 32-bit hash of the key concatenated with the secret (`hash32("$key|" . $this->secret)`).
3. **Image Creation**:
   - Allocates a blank image with dimensions `($this->length * 70) x 70` pixels.
   - For each character in the key:
     - Loads a pre-rendered PNG image of the character from `CMS_IMAGES_PATH . "captcha/"`.
     - Rotates the character image randomly by up to ±45 degrees.
     - Copies the character into the main image.
4. **Obfuscation**:
   - Inverts pixel colors in the bottom half of the image to disrupt automated recognition.
   - Converts the image to pure black and white to eliminate intermediate shades that could aid bot recognition.
5. **Storage**: Saves the image as `CMS_DATA_PATH . "captcha/$code.png"` and returns the verification code.

#### Usage Context
Called when a CAPTCHA challenge is needed (e.g., during form rendering). The returned verification code should be stored in a hidden form field or session for later verification.

---

### Method: `verify($key, $code)`

#### Purpose
Validates a user-provided CAPTCHA response against the expected verification code.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `$key` | `string` | User-provided CAPTCHA response (e.g., from a form submission).              |
| `$code`| `string` | Verification code (returned by `create()` and stored server-side).          |

#### Return Value
| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` if the response is valid; `FALSE` otherwise. Returns `TRUE` if CAPTCHA is disabled. |

#### Inner Mechanisms
1. **Hash Comparison**: Computes the expected verification code by hashing the lowercase `$key` concatenated with the secret (`hash32(strtolower($key) . "|" . $this->secret)`).
2. **Validation**: Compares the computed hash with the provided `$code` using `streq()` for timing-safe equality.

#### Usage Context
Called during form submission to validate the user's CAPTCHA response. The `$code` parameter should match the value returned by `create()` and stored server-side (e.g., in a hidden form field or session).

---


<!-- HASH:e9f59e7949cbb7845aff26a150c66f1c -->
