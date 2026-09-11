# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.captcha.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.captcha.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## captcha

The `captcha` class provides a lightweight, self-contained CAPTCHA system for the PWNC Web Platform. It generates image-based challenges using pre-rendered character PNGs, applies random rotations and obfuscation, and verifies user input against a cryptographically derived code. The class relies on PHP's GD extension and PNG support, and uses the platform's caching mechanism to persist a daily secret key.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$secret` | `NULL` | Daily secret string used to generate and verify CAPTCHA codes. |
| `$length` | `4` | Number of characters in the generated CAPTCHA challenge. |
| `$enabled` | `FALSE` | Indicates whether the CAPTCHA system is active and functional. |

### __construct

Initializes the CAPTCHA system by checking for required GD extension capabilities and loading or generating a daily secret key.

#### Parameters

None.

#### Return Values

No explicit return value. Sets internal properties (`$secret`, `$enabled`) based on environment checks.

#### Inner Mechanisms

1. Checks if the `gd` extension is loaded and supports PNG images.
2. Retrieves the current date and cached secret from `cms_cache`.
3. If the cached date does not match today's date or no secret exists, generates a new unique secret and caches it.
4. Sets `$enabled` to `TRUE` if all requirements are met.

#### Usage Example

```php
$captcha = new \cms\captcha();
if ($captcha->enabled) {
    echo "CAPTCHA system is ready.";
}
```

This example creates a new CAPTCHA instance and checks if it's enabled, which would typically be done during form rendering or session initialization.

### create

Generates a CAPTCHA image and corresponding verification code.

#### Parameters

None.

#### Return Values

Returns an associative array with the following keys on success, or `FALSE` if CAPTCHA is disabled:

| Key | Type | Description |
|-----|------|-------------|
| `code` | `string` | Hash-based verification code for the generated challenge. |
| `image` | `string` | Base64-encoded PNG image data URI. |
| `width` | `int` | Width of the generated image in pixels. |
| `height` | `int` | Height of the generated image in pixels. |

#### Inner Mechanisms

1. Generates a random lowercase key of length `$length` using `unique_id`.
2. Creates a verification code by hashing the key concatenated with the secret using `hash32`.
3. Creates a blank image canvas with dimensions based on character count and size.
4. For each character in the key:
   - Loads the corresponding pre-rendered PNG character image.
   - Randomly rotates the character image by up to 45 degrees in either direction.
   - Copies the rotated character onto the main canvas.
5. Applies obfuscation by inverting colors in the bottom half of the image.
6. Converts the entire image to pure black and white to prevent bots from using color shades.
7. Outputs the image as a PNG data URI and returns it along with the verification code.

#### Usage Example

```php
$captcha = new \cms\captcha();
$result = $captcha->create();
if ($result !== false) {
    $_SESSION['captcha_code'] = $result['code'];
    echo '<img src="' . $result['image'] . '" width="' . $result['width'] . '" height="' . $result['height'] . '">';
}
```

This example generates a CAPTCHA image, stores the verification code in the session, and outputs the image to the browser.

### verify

Validates a user-provided key against the expected verification code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | The original CAPTCHA key (typically the visible characters). |
| `$code` | `string` | The verification code provided by the user. |

#### Return Values

Returns `TRUE` if the CAPTCHA is disabled or if the provided code matches the expected hash. Returns `FALSE` otherwise.

#### Inner Mechanisms

1. If CAPTCHA is disabled, returns `TRUE` immediately (allowing all requests).
2. Converts the provided key to lowercase and concatenates it with the secret.
3. Hashes the concatenated string using `hash32`.
4. Compares the resulting hash with the provided code using `streq` (case-sensitive string equality).

#### Usage Example

```php
$captcha = new \cms\captcha();
if ($captcha->verify($_POST['captcha_key'], $_POST['captcha_code'])) {
    // Process form submission
    echo "CAPTCHA verified successfully.";
} else {
    echo "Invalid CAPTCHA. Please try again.";
}
```

This example validates a user-submitted CAPTCHA response by comparing the provided key and code against the internally generated verification hash.


<!-- HASH:6e5e5e1ceca44a8ca543649e59720be3 -->
