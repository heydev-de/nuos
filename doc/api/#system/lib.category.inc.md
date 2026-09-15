# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.category.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.category.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## category

The `category` class implements a Bayesian classification system for categorizing text content. It uses n-gram tokenization and statistical probability calculations to classify text into categories (e.g., spam detection). The system maintains training data in two database tables and provides methods for training, updating probabilities, and evaluating text against trained categories.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CATEGORY_CLASS_SPAM` | `"#spam"` | Default class identifier for spam classification |
| `CMS_CATEGORY_LEARNING_THRESHOLD` | `25` | Minimum count of yes/no samples required before evaluation |
| `CMS_CATEGORY_TOKEN_LIMIT` | `15` | Maximum number of tokens used during evaluation |
| `CMS_CATEGORY_DEVIATION_THRESHOLD` | `10` | Minimum deviation from 50% rating to consider a token significant |
| `CMS_CATEGORY_TRAINING_THRESHOLD` | `10` | Minimum total training count for a token to be considered |
| `CMS_CATEGORY_PROBABILITY_DEFAULT` | `40` | Default probability returned when no data is available |

### Database Tables

| Table | Description |
|-------|-------------|
| `CMS_DB_CATEGORY_META` | Stores metadata per class (counts of valid/invalid samples) |
| `CMS_DB_CATEGORY` | Stores token-class combinations with counts and computed probabilities |

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enabled` | `bool\|null` | `NULL` | Indicates whether the category system is enabled after table verification |

### Methods

#### `__construct()`

Initializes the category system by verifying the existence and structure of required database tables.

**Parameters:** None

**Return Value:** None

**Inner Mechanisms:**
1. Creates a new `mysql` instance
2. Verifies the `category_meta` table with columns for class identifier and yes/no counts
3. Verifies the `category` table with columns for token, class reference, counts, probabilities, and rating
4. Sets `$enabled` to `TRUE` if both tables are successfully verified

**Usage Context:** Automatically called when instantiating the `category` class. The system will be disabled if database tables cannot be verified.

```php
$category = new category();
if ($category->enabled) {
    // System is ready for training and evaluation
}
```

#### `train($text, $class = "", $valid = TRUE, $undo = FALSE)`

Trains the classifier with a text sample for a specific class.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to tokenize and train with |
| `$class` | `string` | `""` | Category class identifier |
| `$valid` | `bool` | `TRUE` | Whether this is a valid (TRUE) or invalid (FALSE) sample |
| `$undo` | `bool` | `FALSE` | Whether to undo a previous training operation |

**Return Value:**
- `TRUE` on success
- `FALSE` on failure or if system is disabled

**Inner Mechanisms:**
1. Tokenizes the input text into 3-character n-grams using `category_tokenize_text()`
2. Inserts or retrieves the class index from `category_meta` table
3. Updates the meta count (yes/no) for the class
4. Inserts new token-class combinations if they don't exist
5. Updates token counts (yes/no) for the class
6. All operations are wrapped in conditional checks to ensure atomicity

**Usage Context:** Used to build training data for classification. Call multiple times with different samples to build a robust classifier.

```php
$category = new category();
// Train with valid spam samples
$category->train("Buy cheap viagra now!", "#spam", TRUE);
// Train with valid non-spam samples
$category->train("Meeting scheduled for tomorrow", "#spam", FALSE);
```

#### `update()`

Recalculates probability ratings for all tokens based on current training data.

**Parameters:** None

**Return Value:**
- `TRUE` on success
- `FALSE` on failure or if system is disabled

**Inner Mechanisms:**
1. Updates probability_yes and probability_no for each token using Bayesian formula:
   - `probability_yes = ROUND(100 * count_yes / MAX(meta_count_yes, 1))`
   - `probability_no = ROUND(100 * count_no / MAX(meta_count_no, 1))`
2. Applies Bayes formula to compute final rating:
   - `rating = ROUND(100 * probability_yes / MAX(probability_yes + probability_no, 1))`
3. Cleans up noise by deleting tokens that:
   - Have sufficient training (>10 total samples)
   - Have insignificant ratings (between 40-60, i.e., close to neutral)

**Usage Context:** Should be called periodically after training to refresh probability calculations. Not needed after every single training operation.

```php
$category = new category();
// After training multiple samples
$category->train($spamText, "#spam", TRUE);
$category->train($hamText, "#spam", FALSE);
// Update probabilities
$category->update();
```

#### `evaluate($text, $class = "")`

Evaluates text against a trained class and returns a probability rating.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to evaluate |
| `$class` | `string` | `""` | Category class identifier to evaluate against |

**Return Value:**
- `int` (0-100) representing probability rating
- `FALSE` on error or if system is disabled

**Inner Mechanisms:**
1. Tokenizes input text into n-grams
2. Checks if class has sufficient training data (both yes/no counts > 25)
3. If insufficient data, returns default probability (40)
4. Limits tokens to `CMS_CATEGORY_TOKEN_LIMIT` (15) most significant ones
5. Retrieves ratings for matching tokens that:
   - Belong to the specified class
   - Have sufficient training (>10 samples)
   - Have significant deviation from neutral (≥10)
6. Calculates average rating of matched tokens
7. Adds default rating (40) for unmatched tokens
8. Returns rounded average

**Usage Context:** Used to classify new text content. Higher ratings indicate stronger match to the class.

```php
$category = new category();
$text = "Get your free prescription drugs here";
$rating = $category->evaluate($text, "#spam");
if ($rating > 60) {
    echo "This appears to be spam";
} else {
    echo "This appears to be legitimate";
}
```

#### `train_valid($text, $class = "")`

Convenience method to train text as a valid sample for a class.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to train |
| `$class` | `string` | `""` | Category class identifier |

**Return Value:** Result of `train()` method (`TRUE` or `FALSE`)

**Usage Context:** Simplified interface for training valid samples.

```php
$category = new category();
$category->train_valid("Important company announcement", "announcements");
```

#### `undo_valid($text, $class = "")`

Convenience method to undo training of a valid sample.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to untrain |
| `$class` | `string` | `""` | Category class identifier |

**Return Value:** Result of `train()` method with `$undo=TRUE`

**Usage Context:** Removes a previously added valid training sample.

```php
$category = new category();
$category->undo_valid("Outdated announcement text", "announcements");
```

#### `train_invalid($text, $class = "")`

Convenience method to train text as an invalid sample for a class.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to train |
| `$class` | `string` | `""` | Category class identifier |

**Return Value:** Result of `train()` method with `$valid=FALSE`

**Usage Context:** Simplified interface for training invalid samples.

```php
$category = new category();
$category->train_invalid("Spam message content", "#spam");
```

#### `undo_invalid($text, $class = "")`

Convenience method to undo training of an invalid sample.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to untrain |
| `$class` | `string` | `""` | Category class identifier |

**Return Value:** Result of `train()` method with `$valid=FALSE, $undo=TRUE`

**Usage Context:** Removes a previously added invalid training sample.

```php
$category = new category();
$category->undo_invalid("Misclassified content", "#spam");
```

#### `train_spam($text)`

Convenience method to train text as spam.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to train as spam |

**Return Value:** Result of `train()` method with class set to `CMS_CATEGORY_CLASS_SPAM`

**Usage Context:** Simplified interface for spam training.

```php
$category = new category();
$category->train_spam("Viagra cheap prices limited time offer");
```

#### `undo_spam($text)`

Convenience method to undo spam training.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to untrain as spam |

**Return Value:** Result of `train()` method with class set to `CMS_CATEGORY_CLASS_SPAM, $undo=TRUE`

**Usage Context:** Removes a previously added spam training sample.

```php
$category = new category();
$category->undo_spam("Content that was incorrectly marked as spam");
```

#### `train_nospam($text)`

Convenience method to train text as non-spam.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to train as non-spam |

**Return Value:** Result of `train()` method with class set to `CMS_CATEGORY_CLASS_SPAM, $valid=FALSE`

**Usage Context:** Simplified interface for non-spam training.

```php
$category = new category();
$category->train_nospam("Legitimate newsletter subscription");
```

#### `undo_nospam($text)`

Convenience method to undo non-spam training.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to untrain as non-spam |

**Return Value:** Result of `train()` method with class set to `CMS_CATEGORY_CLASS_SPAM, $valid=FALSE, $undo=TRUE`

**Usage Context:** Removes a previously added non-spam training sample.

```php
$category = new category();
$category->undo_nospam("Content that was incorrectly marked as non-spam");
```

#### `evaluate_spam($text)`

Convenience method to evaluate text for spam probability.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Text content to evaluate for spam |

**Return Value:** Probability rating (0-100) from `evaluate()` method

**Usage Context:** Simplified interface for spam detection.

```php
$category = new category();
$text = "Win a million dollars! Click here now!";
$spamRating = $category->evaluate_spam($text);
if ($spamRating > 70) {
    // Likely spam
}
```

## category_tokenize_text($text)

Tokenizes text into 3-character n-grams for use in Bayesian classification.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | Required | Input text to tokenize |

**Return Value:** `array` of unique 3-character n-gram tokens

**Inner Mechanisms:**
1. Calls `tokenize_text($text, TRUE)` to perform initial tokenization
2. Concatenates all tokens with null terminators
3. Iterates through the text character by character
4. Handles multi-byte UTF-8 characters (1-4 bytes)
5. Builds 3-character sliding window n-grams
6. Returns unique tokens using associative array keys

**Usage Context:** Internal function used by the `category` class for text processing. Can also be used independently for n-gram analysis.

```php
$tokens = category_tokenize_text("Hello world");
// Returns array of 3-character sequences like ["Hel", "ell", "llo", ...]
```


<!-- HASH:65778c4c156b2a4ab311b0c93debcfaa -->
