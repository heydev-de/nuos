# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.memory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.memory.inc)

- **Version:** `26.9.7.10`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The `memory` class in `#system/lib.memory.inc` provides a structured key-value storage system for the PWNC Web Platform. It manages "memory" records—persistent notes, data entries, or documents—organized into **regions** (either `scope.*` for shared/global memories or `user.*` for per-user memories). Each record supports rich metadata including mnemonics (tags), descriptions, content, binary attachments, ratings, follow-up dates, and access tracking.

The class implements full CRUD operations, full-text search with relevance ranking, mnemonic indexing for tag-based discovery, automated cleanup of stale records, and a granular permission model (reader, writer, operator).

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MEMORY_PERMISSION_OPERATOR` | `"operator"` | Permission level for full administrative control over a memory region. |
| `CMS_MEMORY_PERMISSION_WRITER` | `"writer"` | Permission level for creating and editing memory records. |
| `CMS_MEMORY_PERMISSION_READER` | `"reader"` | Permission level for reading and searching memory records. |
| `CMS_DB_MEMORY` | `CMS_DB_PREFIX . "memory_"` | Base table name prefix for memory data tables. |
| `CMS_DB_MEMORY_INDEX` | `"id"` | Primary key column name. |
| `CMS_DB_MEMORY_MNEMONIC` | `"mnemonic"` | Column storing newline-separated mnemonic tags. |
| `CMS_DB_MEMORY_DESCRIPTION` | `"description"` | Column storing a short description (max 500 chars). |
| `CMS_DB_MEMORY_CONTENT` | `"content"` | Column storing the main text content. |
| `CMS_DB_MEMORY_ATTACHMENT_TYPE` | `"attachment_type"` | Column storing the MIME type of the attachment. |
| `CMS_DB_MEMORY_OWNER` | `"owner"` | Column storing the username of the record owner. |
| `CMS_DB_MEMORY_CREATED` | `"created"` | Column storing the creation timestamp. |
| `CMS_DB_MEMORY_EDITOR` | `"editor"` | Column storing the username of the last editor. |
| `CMS_DB_MEMORY_EDITED` | `"edited"` | Column storing the last edit timestamp. |
| `CMS_DB_MEMORY_ACCESS_COUNT` | `"access_count"` | Column storing the number of times the record was accessed. |
| `CMS_DB_MEMORY_ACCESSED` | `"accessed"` | Column storing the last access timestamp. |
| `CMS_DB_MEMORY_FOLLOWUP` | `"followup"` | Column storing a follow-up date (cleared on access). |
| `CMS_DB_MEMORY_READ_ONLY` | `"read_only"` | Column flag (0/1) indicating the record is locked from edits. |
| `CMS_DB_MEMORY_SAFE` | `"safe"` | Column flag (0/1) indicating the record is protected from cleanup. |
| `CMS_DB_MEMORY_RATING_VALUE` | `"rating_value"` | Column storing the average rating (0.00–5.00). |
| `CMS_DB_MEMORY_RATING_SUM` | `"rating_sum"` | Column storing the sum of all ratings. |
| `CMS_DB_MEMORY_RATING_COUNT` | `"rating_count"` | Column storing the number of ratings. |
| `CMS_DB_MEMORY_MNEM` | `CMS_DB_PREFIX . "memory_mnem_"` | Base table name prefix for mnemonic index tables. |
| `CMS_DB_MEMORY_MNEM_VALUE` | `"value"` | Column in mnemonic index table storing the mnemonic string. |
| `CMS_DB_MEMORY_MNEM_COUNT` | `"count"` | Column in mnemonic index table storing usage count. |

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | `string\|NULL` | `NULL` | The current user context (defaults to `CMS_SUPERUSER`). |
| `$operator` | `bool` | `FALSE` | Whether the current user has operator-level permissions. |
| `$mysql` | `mysql` | `NULL` | MySQL wrapper instance for schema verification. |
| `$relevance_rating` | `float` | `1.0` | Weight factor for rating in relevance scoring. |
| `$relevance_recency` | `float` | `2.0` | Weight factor for recency in relevance scoring. |
| `$relevance_limit` | `float` | `0.25` | Minimum relevance threshold (as fraction of max) for search results. |
| `$cleanup_edit_expiry` | `int` | `180` | Days after which unedited records become eligible for cleanup (scaled by rating). |
| `$cleanup_read_expiry` | `int` | `90` | Days after which unread records become eligible for cleanup (scaled by rating). |

## memory

### __construct

Initializes the memory manager with a user context and checks operator-level permissions.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | `string\|NULL` | `NULL` | The username to use as the owner/editor context. Falls back to `CMS_SUPERUSER` if `NULL`. |

**Return Value**

No return value (constructor).

**Inner Mechanisms**

Sets the user context, checks whether the user has the `memory.operator` permission (stored in `$this->operator`), and instantiates a `mysql` wrapper for schema verification operations.

**Usage Example**

```php
$mem = new memory("alice");
// $mem->user is now "alice"
// $mem->operator is TRUE if alice has memory.operator permission
```

---

### add

Creates a new memory record in the specified region with optional attachment.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier (e.g., `"scope.notes"` or `"user.alice"`). |
| `$data` | `array` | Associative array containing record data. Supported keys: `mnemonic` (string, newline/comma-separated tags), `description` (string, short summary), `content` (string, main body), `attachment` (array with `type` and `data` base64-encoded), `read_only` (bool), `safe` (bool), `followup` (int timestamp or date string). |

**Return Value**

| Type | Description |
|------|-------------|
| `int` | The new record's ID on success. |
| `FALSE` | If the user lacks writer permission, data is invalid, or the table cannot be verified. |
| `NULL` | If the follow-up date is invalid. |

**Inner Mechanisms**

1. Checks writer permission via `is_writer()`.
2. Validates that `$data` is an array.
3. Verifies/creates the database table via `verify_table()`.
4. Tokenizes the mnemonic string into an array of tags.
5. Truncates the description to 500 characters.
6. Parses the follow-up date if provided.
7. Inserts the record into the database with all fields.
8. Indexes the mnemonics in the mnemonic index table.
9. Writes the attachment file to disk if provided.

**Usage Example**

```php
$mem = new memory("alice");
$id = $mem->add("scope.notes", [
    "mnemonic" => "project-x, urgent",
    "description" => "Review the Q4 budget proposal",
    "content" => "The budget looks good but needs approval from finance.",
    "attachment" => [
        "type" => "application/pdf",
        "data" => base64_encode(file_get_contents("/tmp/budget.pdf"))
    ],
    "safe" => TRUE,
    "followup" => "+1 week"
]);
// $id is the new record ID, e.g., 42
```

---

### get

Retrieves a memory record by ID and updates its access tracking.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$id` | `int` | The record ID to retrieve. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | The record data as an associative array, including `attachment_path` if an attachment exists. |
| `FALSE` | If the user lacks reader permission or the record is not found. |

**Inner Mechanisms**

1. Checks reader permission via `is_reader()`.
2. Retrieves the record via `get_record()`.
3. Increments `access_count`, sets `accessed` to `NOW()`, and clears the `followup` date.
4. Adds `attachment_path` to the returned array if the record has an attachment type.

**Usage Example**

```php
$mem = new memory("alice");
$record = $mem->get("scope.notes", 42);
echo $record["description"]; // "Review the Q4 budget proposal"
echo $record["content"];     // "The budget looks good..."
if (isset($record["attachment_path"])) {
    // Serve the attachment file
}
```

---

### get_attachment

Retrieves attachment metadata for a memory record.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$id` | `int` | The record ID. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Associative array with `type` (MIME type) and `path` (filesystem path) keys. |
| `FALSE` | If the user lacks reader permission, the table is invalid, no attachment exists, or the file is missing. |

**Inner Mechanisms**

1. Checks reader permission.
2. Resolves the table name.
3. Retrieves the attachment type from the database.
4. Constructs the filesystem path via `get_attachment_path()`.
5. Verifies the file exists on disk.

**Usage Example**

```php
$mem = new memory("alice");
$attachment = $mem->get_attachment("scope.notes", 42);
if ($attachment !== FALSE) {
    header("Content-Type: " . $attachment["type"]);
    readfile($attachment["path"]);
}
```

---

### set

Updates an existing memory record with new data.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$id` | `int` | The record ID to update. |
| `$data` | `array` | Associative array of fields to update. Supports the same keys as `add()`, plus `mnemonic` (string or array), `description`, `content`, `attachment` (array, `FALSE` to delete, or `NULL` to skip), `read_only`, `safe`, and `followup`. |

**Return Value**

| Type | Description |
|------|-------------|
| `TRUE` | On successful update. |
| `FALSE` | If the user lacks writer permission, data is invalid, the record is not found, or the record is read-only and the user is neither owner nor operator. |
| `NULL` | If the follow-up date is invalid. |

**Inner Mechanisms**

1. Checks writer permission.
2. Retrieves the existing record.
3. Enforces read-only protection: if the record is read-only and the user is neither the owner nor an operator, returns `FALSE`.
4. Builds a dynamic `SET` clause for the `UPDATE` query, handling:
   - Editor and edited timestamp
   - Follow-up date (parsed or set to `NULL`)
   - Mnemonic (tokenized and re-indexed)
   - Description (truncated to 500 chars)
   - Text fields (mnemonic, description, content)
   - Attachment (write new file, delete existing file, or clear type)
   - Flags (read_only, safe)
5. Executes the `UPDATE` query.
6. Updates the mnemonic index if mnemonics changed.

**Usage Example**

```php
$mem = new memory("alice");
$mem->set("scope.notes", 42, [
    "content" => "Updated: The budget has been approved.",
    "followup" => "+2 weeks",
    "safe" => FALSE
]);
// Record 42 is now updated
```

---

### rate

Rates a memory record or deletes it (when rating is 0).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$id` | `int` | The record ID. |
| `$value` | `int` | The rating value (1–5). A value of `0` deletes the record. |

**Return Value**

| Type | Description |
|------|-------------|
| `TRUE` | On successful rating or deletion. |
| `FALSE` | If the user lacks writer permission, the rating value is invalid, or the record is safe/locked and the user is neither owner nor operator. |

**Inner Mechanisms**

- **Deletion (value = 0):** Checks that the record is not safe, the user is the owner or an operator, then deletes the record, its attachment file, and decrements the mnemonic index.
- **Rating (value 1–5):** Increments `rating_sum` and `rating_count`, and recomputes `rating_value` as the average. Returns whether any rows were affected.

**Usage Example**

```php
$mem = new memory("alice");
$mem->rate("scope.notes", 42, 5); // Upvote with 5 stars
$mem->rate("scope.notes", 42, 0); // Delete the record (if owner or operator)
```

---

### get_region_list

Retrieves a list of all accessible memory regions with their metadata.

**Parameters**

None.

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Associative array keyed by region name. Each value contains: `mnemonic` (top 10 mnemonics), `reader` (bool), `writer` (bool), `operator` (bool). |
| `FALSE` | On database error. |

**Inner Mechanisms**

1. Queries `INFORMATION_SCHEMA.TABLES` for all tables matching the memory pattern (`scope_*` or `user_*`).
2. Converts each table name to a region identifier via `table_to_region()`.
3. Filters to only regions where the user has reader permission.
4. For each accessible region, retrieves the top 10 mnemonics and permission flags.

**Usage Example**

```php
$mem = new memory("alice");
$regions = $mem->get_region_list();
foreach ($regions as $region => $info) {
    echo "$region: " . ($info["writer"] ? "writable" : "read-only") . "\n";
}
```

---

### get_region_overview

Retrieves the most relevant records from a region, ordered by a composite relevance score.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | `string` | — | The memory region identifier. |
| `$limit` | `int` | `20` | Maximum number of records to return. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of record associative arrays, ordered by relevance. |
| `FALSE` | If the user lacks reader permission or on database error. |

**Inner Mechanisms**

1. Checks reader permission.
2. Constructs a relevance formula: `(rating_value * relevance_rating / 5) + (relevance_recency / (1 + days_since_last_edit))`.
3. Queries all records ordered by this formula in descending order, limited to `$limit`.

**Usage Example**

```php
$mem = new memory("alice");
$records = $mem->get_region_overview("scope.notes", 10);
foreach ($records as $record) {
    echo $record["description"] . "\n";
}
```

---

### followup_list

Retrieves all records across all accessible regions that have a follow-up date in the past.

**Parameters**

None.

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of associative arrays, each with `region` and `id` keys. |
| `FALSE` | On database error. |

**Inner Mechanisms**

1. Queries `INFORMATION_SCHEMA.TABLES` for all memory tables.
2. For each table, converts to a region and checks reader permission.
3. Queries for records where `followup <= NOW()`.
4. Collects all matching record IDs with their region.

**Usage Example**

```php
$mem = new memory("alice");
$followups = $mem->followup_list();
foreach ($followups as $item) {
    $record = $mem->get($item["region"], $item["id"]);
    echo "Follow-up: " . $record["description"] . "\n";
}
```

---

### cleanup

Removes stale, low-rated memory records and their associated data. Intended for daemon/cron execution.

**Parameters**

None.

**Return Value**

| Type | Description |
|------|-------------|
| `TRUE` | On completion (even if some tables had errors). |
| `FALSE` | If the user lacks daemon permission. |

**Inner Mechanisms**

1. Checks daemon permission.
2. Queries all memory tables.
3. For each table, selects records that are:
   - Not safe (`safe = 0`)
   - Last edited more than `(cleanup_edit_expiry / 5 * rating_value)` days ago
   - Last accessed more than `(cleanup_read_expiry / 5 * rating_value)` days ago
4. Deletes matching records, their attachment files, and decrements the mnemonic index.
5. Drops tables that become empty after cleanup.

**Usage Example**

```php
// Typically called from a daemon/cron job
$mem = new memory();
$mem->cleanup();
```

---

### search

Searches for records within a single region using full-text search.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$value` | `string` | The search query string. |
| `&$mnemonic` | `array\|NULL` | Reference parameter that receives the mnemonic counts for the search results. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of matching record associative arrays (with `relevance` field). |
| `FALSE` | If the user lacks reader permission or on database error. |

**Inner Mechanisms**

1. Checks reader permission.
2. Delegates to `search_table()` for the actual full-text query.
3. Tokenizes the mnemonics from all results.
4. Retrieves mnemonic counts from the region's mnemonic index via `mnemonic_count()`.

**Usage Example**

```php
$mem = new memory("alice");
$results = $mem->search("scope.notes", "budget", $mnemonics);
foreach ($results as $record) {
    echo "[$record[relevance]] " . $record["description"] . "\n";
}
print_r($mnemonics); // Tag counts from results
```

---

### search_all

Searches for records across all accessible regions, with cross-region relevance normalization.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The search query string. |
| `&$mnemonic` | `array\|NULL` | Reference parameter that receives aggregated mnemonic counts across all regions. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of matching records, each augmented with a `region` key, sorted by normalized relevance. |
| `FALSE` | If `get_region_list()` fails. |
| `array` (empty) | If no results or no search terms. |

**Inner Mechanisms**

1. Retrieves the list of accessible regions via `get_region_list()`.
2. Extracts unique words from the search query.
3. For each region, checks if any mnemonic matches the search terms.
4. If matches exist, performs a full-text search via `search_table()` with a limit of 20.
5. Normalizes relevance scores per region (divides by the max relevance in that region).
6. Sorts all results globally by relevance.
7. Filters out results below the `relevance_limit` threshold.
8. Aggregates mnemonic counts across all regions.

**Usage Example**

```php
$mem = new memory("alice");
$results = $mem->search_all("budget", $mnemonics);
foreach ($results as $record) {
    echo "[{$record["region"]}] {$record["description"]}\n";
}
print_r($mnemonics); // Aggregated tag counts
```

---

### search_table (private)

Executes a full-text search query against a single memory table.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$table` | `string` | — | The database table name to search. |
| `$value` | `string` | — | The raw search query string. |
| `$limit` | `int` | `200` | Maximum number of results. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of matching record associative arrays (with `relevance` field). |
| `FALSE` | On database error. |

**Inner Mechanisms**

1. Sanitizes the search value for boolean mode (removes special characters).
2. Appends `*` to each word for prefix matching.
3. Constructs a composite relevance score:
   - `MATCH(mnemonic) AGAINST (natural) * 4`
   - `MATCH(description) AGAINST (natural) * 2`
   - `MATCH(content) AGAINST (natural) * 1`
   - `MATCH(mnemonic) AGAINST (boolean) * 4`
   - `MATCH(description) AGAINST (boolean) * 2`
   - `MATCH(content) AGAINST (boolean) * 1`
4. Queries with `WHERE MATCH(...) AGAINST (boolean)` and `HAVING relevance > 0`.
5. Filters results below the `relevance_limit` threshold.

**Usage Example**

```php
// Called internally by search() and search_all()
// Not typically called directly by application code
```

---

### mnemonic_list

Retrieves the most frequently used mnemonics from a region.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | `string` | — | The memory region identifier. |
| `$limit` | `int` | `100` | Maximum number of mnemonics to return. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Associative array mapping mnemonic strings to their usage counts, sorted by count (descending) then alphabetically. |
| `FALSE` | If the user lacks reader permission or the table is invalid. |

**Inner Mechanisms**

1. Checks reader permission.
2. Resolves the mnemonic index table name.
3. Queries the top `$limit` mnemonics ordered by count descending, value ascending.
4. Sorts the result via `mnemonic_sort()`.

**Usage Example**

```php
$mem = new memory("alice");
$tags = $mem->mnemonic_list("scope.notes", 20);
foreach ($tags as $tag => $count) {
    echo "$tag: $count\n";
}
```

---

### mnemonic_count

Retrieves usage counts for specific mnemonics in a region.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$list` | `array\|string` | An array of mnemonic strings, or a single mnemonic string (which will be tokenized). |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Associative array mapping mnemonic strings to their counts, sorted by count (descending) then alphabetically. |
| `FALSE` | If the user lacks reader permission or the table is invalid. |
| `array` (empty) | If the input list is empty. |

**Inner Mechanisms**

1. Checks reader permission.
2. Resolves the mnemonic index table name.
3. If `$list` is a string, tokenizes it via `mnemonic_tokenize()`.
4. Queries the mnemonic index for the specified values.
5. Sorts the result via `mnemonic_sort()`.

**Usage Example**

```php
$mem = new memory("alice");
$counts = $mem->mnemonic_count("scope.notes", ["urgent", "project-x"]);
echo "urgent: " . ($counts["urgent"] ?? 0) . "\n";
echo "project-x: " . ($counts["project-x"] ?? 0) . "\n";
```

---

### mnemonic_sort (private)

Sorts a mnemonic-count array by descending count, then by natural ascending mnemonic string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Associative array mapping mnemonic strings to integer counts. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | The sorted array (same keys and values, reordered). |

**Inner Mechanisms**

Uses `uksort()` with a comparator that first compares counts in descending order (`$array[$b] <=> $array[$a]`), then falls back to `strnatcmp()` for natural string comparison of the keys.

**Usage Example**

```php
// Called internally by mnemonic_list() and mnemonic_count()
// Not typically called directly
```

---

### mnemonic_tokenize

Normalizes and tokenizes a mnemonic string into an array of clean, unique, sorted tags.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A mnemonic string with tags separated by commas, newlines, or whitespace. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Array of unique, lowercased, trimmed mnemonic strings, sorted naturally. |
| `array` (empty) | If the input is empty or contains no valid tokens. |

**Inner Mechanisms**

1. Strips unnecessary whitespace.
2. Replaces delimiters (commas, newlines) with line breaks.
3. Limits total length to 500 characters.
4. Lowercases using multibyte-safe `utf8_strtolower()`.
5. Splits by newline.
6. Truncates each mnemonic to 100 characters.
7. Removes duplicates.
8. Sorts naturally.

**Usage Example**

```php
$mem = new memory("alice");
$tags = $mem->mnemonic_tokenize("Project-X, URGENT, project-x, review");
// Result: ["project-x", "review", "urgent"]
```

---

### mnemonic_index (private)

Updates the mnemonic index table when mnemonics are added or removed from a record.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | `string` | — | The memory region identifier. |
| `$in` | `array` | — | Mnemonics to add (increment count). |
| `$out` | `array` | `[]` | Mnemonics to remove (decrement count). |

**Return Value**

| Type | Description |
|------|-------------|
| `TRUE` | On successful index update. |
| `FALSE` | If the table name cannot be resolved. |

**Inner Mechanisms**

1. Resolves the mnemonic index table name.
2. Computes `$add` (in `$in` but not in `$out`) and `$del` (in `$out` but not in `$in`).
3. For additions: uses `INSERT ... ON DUPLICATE KEY UPDATE` to increment counts.
4. For deletions: decrements counts, then deletes entries with count ≤ 0.

**Usage Example**

```php
// Called internally by add(), set(), and rate()
// Not typically called directly
```

---

### mnemonic_rebuild

Rebuilds the entire mnemonic index for a region from scratch.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |

**Return Value**

| Type | Description |
|------|-------------|
| `TRUE` | On successful rebuild. |
| `FALSE` | If the user is not an operator, the table is invalid, or the truncate fails. |

**Inner Mechanisms**

1. Checks operator permission.
2. Resolves both the data table and mnemonic index table names.
3. Truncates the mnemonic index table.
4. Retrieves all mnemonics from the data table.
5. Tokenizes each mnemonic string and counts occurrences.
6. Bulk-inserts the aggregated counts into the mnemonic index table.

**Usage Example**

```php
$mem = new memory("alice");
$mem->mnemonic_rebuild("scope.notes");
// Mnemonic index for scope.notes is now rebuilt from all records
```

---

### has_permission (private)

Checks whether the current user has a specific permission level for a memory region.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$permission` | `string` | The permission level to check (`"reader"`, `"writer"`, or `"operator"`). |

**Return Value**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has the permission, `FALSE` otherwise. |

**Inner Mechanisms**

1. Parses the region into type and identifier via `parse_region()`.
2. If the user is an operator, returns `TRUE` immediately.
3. Otherwise, checks the permission via `cms_permission()`:
   - For user regions (`user.<username>`), checks `memory` permission.
   - For scope regions, checks `memory.<type>.<identifier>.<permission>`.

**Usage Example**

```php
// Called internally by is_reader(), is_writer(), is_operator()
// Not typically called directly
```

---

### is_reader

Checks whether the current user has reader permission for a region.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |

**Return Value**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user can read from the region. |

**Inner Mechanisms**

Delegates to `has_permission()` with `CMS_MEMORY_PERMISSION_READER`.

**Usage Example**

```php
$mem = new memory("alice");
if ($mem->is_reader("scope.notes")) {
    $records = $mem->get_region_overview("scope.notes");
}
```

---

### is_writer

Checks whether the current user has writer permission for a region.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |

**Return Value**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user can create and edit records in the region. |

**Inner Mechanisms**

Delegates to `has_permission()` with `CMS_MEMORY_PERMISSION_WRITER`.

**Usage Example**

```php
$mem = new memory("alice");
if ($mem->is_writer("scope.notes")) {
    $mem->add("scope.notes", ["mnemonic" => "test", "content" => "Hello"]);
}
```

---

### is_operator

Checks whether the current user has operator permission for a region.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |

**Return Value**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the user has full administrative control over the region. |

**Inner Mechanisms**

Delegates to `has_permission()` with `CMS_MEMORY_PERMISSION_OPERATOR`.

**Usage Example**

```php
$mem = new memory("alice");
if ($mem->is_operator("scope.notes")) {
    $mem->mnemonic_rebuild("scope.notes");
}
```

---

### parse_region (private)

Parses a region identifier into its type and identifier components.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The region identifier (e.g., `"scope.notes"` or `"user.alice"`). |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | Associative array with `type` (`"scope"` or `"user"`) and `identifier` keys. |
| `FALSE` | If the region format is invalid or the identifier fails validation. |

**Inner Mechanisms**

1. Matches the region against the regex `/^(scope|user)\.(.+)$/s`.
2. Validates the identifier via `verify_identifier()`.
3. Returns the parsed components or `FALSE`.

**Usage Example**

```php
// Called internally by has_permission(), table_name(), get_attachment_path()
// Not typically called directly
```

---

### verify_identifier (private)

Validates that a region identifier contains only safe characters.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The identifier to validate. |

**Return Value**

| Type | Description |
|------|-------------|
| `int` | `1` if valid (from `preg_match`), `0` if invalid. |

**Inner Mechanisms**

Uses `preg_match("/^[-0-9a-z_]{1,40}$/", $value)` to ensure the identifier is 1–40 characters of lowercase letters, digits, hyphens, and underscores.

**Usage Example**

```php
// Called internally by parse_region() and table_to_region()
// Not typically called directly
```

---

### table_name (private)

Resolves a region identifier to its corresponding database table name.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The region identifier. |
| `$mnemonic` | `bool` | `FALSE` | If `TRUE`, returns the mnemonic index table name instead of the data table. |

**Return Value**

| Type | Description |
|------|-------------|
| `string` | The database table name. |
| `FALSE` | If the region is invalid. |

**Inner Mechanisms**

1. Parses the region via `parse_region()`.
2. Returns `CMS_DB_MEMORY . type . "_" . identifier` for data tables, or `CMS_DB_MEMORY_MNEM . type . "_" . identifier` for mnemonic index tables.

**Usage Example**

```php
// Called internally by add(), get(), set(), etc.
// Not typically called directly
```

---

### table_to_region (private)

Converts a database table name back to a region identifier.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The database table name. |

**Return Value**

| Type | Description |
|------|-------------|
| `string` | The region identifier (e.g., `"scope.notes"`). |
| `FALSE` | If the table name doesn't match the memory pattern or the identifier is invalid. |

**Inner Mechanisms**

1. Matches the table name against the regex `/^CMS_DB_MEMORY(scope|user)_(.+)$/`.
2. Validates the identifier via `verify_identifier()`.
3. Returns `type . "." . identifier` or `FALSE`.

**Usage Example**

```php
// Called internally by get_region_list() and followup_list()
// Not typically called directly
```

---

### get_attachment_path (private)

Constructs the filesystem path for a memory record's attachment file.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |
| `$id` | `int` | The record ID. |

**Return Value**

| Type | Description |
|------|-------------|
| `string` | The filesystem path to the attachment file. |
| `FALSE` | If the region is invalid. |

**Inner Mechanisms**

1. Parses the region to get type and identifier.
2. Constructs a sharded path: `CMS_DATA_PATH . "#memory/" . type . "/" . identifier . "/" . floor(id/1000000) . "/" . floor(id/1000) . "/" . id`.
3. The sharding prevents too many files in a single directory.

**Usage Example**

```php
// Called internally by add(), get(), get_attachment(), set(), rate(), cleanup()
// Not typically called directly
```

---

### verify_table (private)

Ensures that the data table and mnemonic index table for a region exist with the correct schema.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$region` | `string` | The memory region identifier. |

**Return Value**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if both tables are verified/created successfully. |
| `FALSE` | If either table verification fails. |

**Inner Mechanisms**

1. Resolves both table names via `table_name()`.
2. Checks if the MySQL version supports n-gram parser (MySQL ≥ 5.7.6).
3. Calls `mysql->verify_table()` for the data table with:
   - Column definitions (id, mnemonic, description, content, attachment_type, owner, created, editor, edited, access_count, accessed, followup, read_only, safe, rating_value, rating_sum, rating_count)
   - Indexes (owner index, fulltext indexes on mnemonic, description, content, and a composite fulltext)
4. Calls `mysql->verify_table()` for the mnemonic index table with:
   - Column definitions (value as primary key, count)
   - Index on count

**Usage Example**

```php
// Called internally by add()
// Not typically called directly
```

---

### get_record (private)

Retrieves a single record from a database table by ID.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$table` | `string` | The database table name. |
| `$id` | `int` | The record ID. |

**Return Value**

| Type | Description |
|------|-------------|
| `array` | The record as an associative array. |
| `FALSE` | If the table is invalid, the query fails, or no record is found. |

**Inner Mechanisms**

Executes a `SELECT * FROM table WHERE id = ? LIMIT 1` query and returns the first row as an associative array.

**Usage Example**

```php
// Called internally by get(), set(), and rate()
// Not typically called directly
```

---

### parse_date

Parses a date value into a standardized MySQL datetime string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int\|string` | An integer Unix timestamp or a date string (e.g., `"+1 week"`, `"2026-12-25 10:00:00"`). |

**Return Value**

| Type | Description |
|------|-------------|
| `string` | A MySQL datetime string in `Y-m-d H:i:s` format (UTC). |
| `NULL` | If the value is invalid (negative timestamp, unparseable string, or unsupported type). |

**Inner Mechanisms**

1. For integers: validates that the value is non-negative.
2. For strings: uses `strtotime()` to parse the date string.
3. Converts the timestamp to a UTC datetime string via `gmdate()`.

**Usage Example**

```php
$mem = new memory("alice");
$datetime = $mem->parse_date("+1 week");
// Returns something like "2026-04-15 14:30:00"
$datetime = $mem->parse_date(1713168000);
// Returns "2024-04-15 12:00:00"
```


<!-- HASH:c9321aca4f8fb86483c073ad9ea80858 -->
