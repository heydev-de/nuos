# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.category.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Category Class Module

This module implements a Bayesian text classification system for categorizing content (e.g., spam detection). It provides mechanisms for training the classifier with labeled text samples, updating probability ratings, and evaluating new text against trained categories.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_CATEGORY_CLASS_SPAM` | `"#spam"` | Default class identifier for spam detection. |
| `CMS_CATEGORY_LEARNING_THRESHOLD` | `25` | Minimum number of positive/negative samples required for evaluation. |
| `CMS_CATEGORY_TOKEN_LIMIT` | `15` | Maximum number of tokens used for evaluation. |
| `CMS_CATEGORY_DEVIATION_THRESHOLD` | `10` | Minimum deviation from 50% to consider a token significant. |
| `CMS_CATEGORY_TRAINING_THRESHOLD` | `10` | Minimum combined count (yes + no) for a token to be retained. |
| `CMS_CATEGORY_PROBABILITY_DEFAULT` | `40` | Default probability rating for untrained tokens. |

#### Database Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_CATEGORY_META` | `CMS_DB_PREFIX . "category_meta"` | Table storing class metadata. |
| `CMS_DB_CATEGORY_META_INDEX` | `"id"` | Primary key for class metadata. |
| `CMS_DB_CATEGORY_META_CLASS` | `"class"` | Class identifier (e.g., `"#spam"`). |
| `CMS_DB_CATEGORY_META_COUNT_YES` | `"count_yes"` | Total positive samples for a class. |
| `CMS_DB_CATEGORY_META_COUNT_NO` | `"count_no"` | Total negative samples for a class. |
| `CMS_DB_CATEGORY` | `CMS_DB_PREFIX . "category"` | Table storing token-class relationships. |
| `CMS_DB_CATEGORY_INDEX` | `"id"` | Primary key for token entries. |
| `CMS_DB_CATEGORY_TOKEN` | `"token"` | 3-character UTF-8 token. |
| `CMS_DB_CATEGORY_CLASS` | `"class"` | Foreign key to `CMS_DB_CATEGORY_META_INDEX`. |
| `CMS_DB_CATEGORY_COUNT_YES` | `"count_yes"` | Positive samples for a token-class pair. |
| `CMS_DB_CATEGORY_COUNT_NO` | `"count_no"` | Negative samples for a token-class pair. |
| `CMS_DB_CATEGORY_PROBABILITY_YES` | `"probability_yes"` | Relative positive probability (0-100). |
| `CMS_DB_CATEGORY_PROBABILITY_NO` | `"probability_no"` | Relative negative probability (0-100). |
| `CMS_DB_CATEGORY_RATING` | `"rating"` | Bayesian probability rating (0-100). |

---

## `category_tokenize_text`

### Purpose
Tokenizes input text into 3-character UTF-8 sequences for classification. Used to break down text into manageable units for Bayesian analysis.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to tokenize. |

### Return Values
| Type | Description |
|------|-------------|
| `array` | Array of unique 3-character tokens. |

### Inner Mechanisms
1. **Preprocessing**: Uses `tokenize_text()` to normalize whitespace and punctuation.
2. **UTF-8 Handling**: Iterates through the text, handling multibyte characters (1-4 bytes) to ensure correct tokenization.
3. **Sliding Window**: Generates all possible 3-character sequences (e.g., `"abc"` → `["abc", "bcd", "cde"]`).
4. **Deduplication**: Returns only unique tokens.

### Usage
- Called internally by `train()` and `evaluate()`.
- Not typically used directly by external code.

---

## `category` Class

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$mysql` | `mysql` | Database connection handler. |
| `$enabled` | `bool` | Indicates if the classifier is ready (tables exist). |

---

### `__construct`

#### Purpose
Initializes the classifier and verifies database tables.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
1. **Database Setup**: Creates `CMS_DB_CATEGORY_META` and `CMS_DB_CATEGORY` tables if they don’t exist.
2. **Schema Validation**: Ensures required columns and constraints (e.g., unique class-token pairs) are present.
3. **State Check**: Sets `$enabled` to `TRUE` if tables are valid.

#### Usage
```php
$classifier = new \cms\category();
if ($classifier->enabled) {
    // Classifier is ready
}
```

---

### `train`

#### Purpose
Trains the classifier by adjusting token counts for a given class.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to train on. |
| `$class` | `string` | Class identifier (e.g., `"#spam"`). Default: `""`. |
| `$valid` | `bool` | `TRUE` for positive samples, `FALSE` for negative. Default: `TRUE`. |
| `$undo` | `bool` | `TRUE` to decrement counts (undo training). Default: `FALSE`. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Tokenization**: Splits `$text` into 3-character tokens.
2. **Class Handling**:
   - Inserts the class into `CMS_DB_CATEGORY_META` if it doesn’t exist.
   - Retrieves the class’s database index.
3. **Count Adjustment**:
   - Updates `count_yes`/`count_no` in `CMS_DB_CATEGORY_META`.
   - Updates `count_yes`/`count_no` for each token in `CMS_DB_CATEGORY`.
4. **Undo Support**: Decrements counts if `$undo` is `TRUE`.

#### Usage
```php
// Train as valid (positive) sample
$classifier->train("Hello world", "greeting");

// Train as invalid (negative) sample
$classifier->train("Hello world", "greeting", FALSE);

// Undo training
$classifier->train("Hello world", "greeting", TRUE, TRUE);
```

---

### `update`

#### Purpose
Recalculates probability ratings for all tokens using Bayesian statistics.

#### Parameters
None.

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Probability Calculation**:
   - Computes `probability_yes` and `probability_no` as percentages of class totals.
   - Applies Bayesian formula to derive `rating` (0-100).
2. **Noise Reduction**: Deletes tokens with insignificant ratings (close to 50%).

#### Usage
```php
// Rebuild all probabilities after training
$classifier->update();
```

---

### `evaluate`

#### Purpose
Evaluates text against a trained class and returns a probability rating.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to evaluate. |
| `$class` | `string` | Class identifier. Default: `""`. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Probability rating (0-100), or `FALSE` on error. |
| `int` | Defaults to `CMS_CATEGORY_PROBABILITY_DEFAULT` if class is untrained. |

#### Inner Mechanisms
1. **Tokenization**: Splits `$text` into tokens.
2. **Class Validation**: Checks if the class has sufficient training data.
3. **Token Retrieval**: Fetches the most significant tokens (highest deviation from 50%).
4. **Rating Calculation**: Averages ratings of matching tokens, filling missing tokens with the default probability.

#### Usage
```php
$rating = $classifier->evaluate("Buy now!", "#spam");
if ($rating > 70) {
    // Likely spam
}
```

---

### Convenience Methods

| Method | Purpose | Equivalent To |
|--------|---------|---------------|
| `train_valid($text, $class)` | Train as positive sample. | `train($text, $class, TRUE, FALSE)` |
| `undo_valid($text, $class)` | Undo positive training. | `train($text, $class, TRUE, TRUE)` |
| `train_invalid($text, $class)` | Train as negative sample. | `train($text, $class, FALSE, FALSE)` |
| `undo_invalid($text, $class)` | Undo negative training. | `train($text, $class, FALSE, TRUE)` |
| `train_spam($text)` | Train as spam. | `train($text, CMS_CATEGORY_CLASS_SPAM, TRUE, FALSE)` |
| `undo_spam($text)` | Undo spam training. | `train($text, CMS_CATEGORY_CLASS_SPAM, TRUE, TRUE)` |
| `train_nospam($text)` | Train as non-spam. | `train($text, CMS_CATEGORY_CLASS_SPAM, FALSE, FALSE)` |
| `undo_nospam($text)` | Undo non-spam training. | `train($text, CMS_CATEGORY_CLASS_SPAM, FALSE, TRUE)` |
| `evaluate_spam($text)` | Evaluate as spam. | `evaluate($text, CMS_CATEGORY_CLASS_SPAM)` |

#### Usage
```php
// Spam detection shortcuts
$classifier->train_spam("Free offer!");
$rating = $classifier->evaluate_spam("Limited time deal");
```


<!-- HASH:0c2bf9aa8c28cd22e3a70c13a1455228 -->
