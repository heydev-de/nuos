# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.category.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.category.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Category Class

The `category` class implements a Bayesian text classification system for categorizing content (e.g., spam detection). It tokenizes text into 3-character sequences, tracks their occurrence in different classes, and calculates probabilities to determine how likely a given text belongs to a specific category.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_CATEGORY_CLASS_SPAM` | `"#spam"` | Default class identifier for spam detection. |
| `CMS_CATEGORY_LEARNING_THRESHOLD` | `25` | Minimum number of positive/negative samples required for evaluation. |
| `CMS_CATEGORY_TOKEN_LIMIT` | `15` | Maximum number of tokens used for evaluation. |
| `CMS_CATEGORY_DEVIATION_THRESHOLD` | `10` | Minimum deviation from 50% to consider a token significant. |
| `CMS_CATEGORY_TRAINING_THRESHOLD` | `10` | Minimum number of samples required for a token to be considered trained. |
| `CMS_CATEGORY_PROBABILITY_DEFAULT` | `40` | Default probability assigned to untrained tokens. |
| `CMS_DB_CATEGORY_META` | `CMS_DB_PREFIX . "category_meta"` | Table storing class metadata (e.g., counts). |
| `CMS_DB_CATEGORY` | `CMS_DB_PREFIX . "category"` | Table storing token-class relationships and probabilities. |

---

### `category_tokenize_text($text)`

#### Purpose
Tokenizes input text into 3-character sequences for classification. Handles multibyte UTF-8 characters safely.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to tokenize. |

#### Return Values
- `array`: List of unique 3-character tokens.

#### Inner Mechanisms
1. Uses `tokenize_text()` to split text into characters (multibyte-safe).
2. Iterates through the text, grouping characters into 3-character sliding windows.
3. Skips incomplete sequences (e.g., at the start/end of text).
4. Returns unique tokens as array keys.

#### Usage Example
```php
$tokens = category_tokenize_text("Hello world");
/* Output: ["Hel", "ell", "llo", "lo ", "o w", " wo", "wor", "orl", "rld"] */
```

---

### `category` Class

#### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$enabled` | `NULL` | `TRUE` if database tables are initialized; `FALSE` otherwise. |

---

### Constructor: `__construct()`

#### Purpose
Initializes the classifier by verifying/creating required database tables.

#### Inner Mechanisms
1. Checks for existence of `CMS_DB_CATEGORY_META` and `CMS_DB_CATEGORY` tables.
2. Creates tables with predefined schemas if they don’t exist.
3. Sets `$enabled` to `TRUE` on success.

---

### `train($text, $class = "", $valid = TRUE, $undo = FALSE)`

#### Purpose
Trains the classifier by adjusting token counts for a given class.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to train on. |
| `$class` | `string` | Class identifier (e.g., `"#spam"`). Default: `""`. |
| `$valid` | `bool` | `TRUE` for valid (positive) samples; `FALSE` for invalid (negative). Default: `TRUE`. |
| `$undo` | `bool` | `TRUE` to decrement counts (undo training). Default: `FALSE`. |

#### Return Values
- `bool`: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Tokenizes text and escapes tokens/SQL values.
2. Inserts/updates the class in `CMS_DB_CATEGORY_META`.
3. Adjusts class-wide counters (`count_yes`/`count_no`).
4. Updates token counts in `CMS_DB_CATEGORY` for the class.

#### Usage Example
```php
$category = new category();
$category->train("Buy cheap pills", "#spam"); // Train as spam
$category->train("Hello user", "#spam", FALSE); // Train as non-spam
```

---

### `update()`

#### Purpose
Recalculates token probabilities and cleans up insignificant data.

#### Return Values
- `bool`: `TRUE` on success; `FALSE` on failure.

#### Inner Mechanisms
1. Updates `probability_yes`/`probability_no` for all tokens (relative to class counts).
2. Applies Bayesian formula to compute `rating` (0–100).
3. Deletes tokens with ratings close to 50% (insignificant).

#### Usage Example
```php
$category = new category();
$category->update(); // Rebuild probabilities after training
```

---

### `evaluate($text, $class = "")`

#### Purpose
Evaluates the probability that a text belongs to a class.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to evaluate. |
| `$class` | `string` | Class identifier. Default: `""`. |

#### Return Values
- `int`: Probability (0–100) or `CMS_CATEGORY_PROBABILITY_DEFAULT` if untrained.
- `bool`: `FALSE` on error.

#### Inner Mechanisms
1. Tokenizes text and checks if the class has sufficient training data.
2. Retrieves the most significant tokens (highest deviation from 50%).
3. Computes average probability across tokens.

#### Usage Example
```php
$probability = $category->evaluate("Free offer", "#spam");
/* Returns e.g., 85 (85% likely to be spam) */
```

---

### Convenience Methods

| Method | Purpose |
|--------|---------|
| `train_valid($text, $class)` | Alias for `train($text, $class, TRUE)`. |
| `undo_valid($text, $class)` | Alias for `train($text, $class, TRUE, TRUE)`. |
| `train_invalid($text, $class)` | Alias for `train($text, $class, FALSE)`. |
| `undo_invalid($text, $class)` | Alias for `train($text, $class, FALSE, TRUE)`. |
| `train_spam($text)` | Trains text as spam (`class = "#spam"`). |
| `undo_spam($text)` | Undoes spam training. |
| `train_nospam($text)` | Trains text as non-spam. |
| `undo_nospam($text)` | Undoes non-spam training. |
| `evaluate_spam($text)` | Evaluates spam probability. |

#### Usage Example
```php
$category->train_spam("Click here now!"); // Train as spam
$isSpam = $category->evaluate_spam("Limited time offer"); // Returns probability
```


<!-- HASH:65778c4c156b2a4ab311b0c93debcfaa -->
