# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.memory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.memory.inc)

- **Version:** `26.9.21.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Memory Class Documentation

The `memory` class provides a comprehensive key-value storage system for the PWNC Web Platform. It manages structured memory records organized into **regions** (either `scope.*` for shared/global memories or `user.*` for per-user memories), each backed by dynamically created MySQL tables. Records support rich metadata including mnemonics (tags), descriptions, content, binary attachments, ratings, follow-up dates, read-only/safe flags, and access tracking.

## Constants

### Permission Levels

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MEMORY_PERMISSION_OPERATOR` | `"operator"` | Full administrative access to a memory region |
| `CMS_MEMORY_PERMISSION_WRITER` | `"writer"` | Can add, edit, and delete records in a region |
| `CMS_MEMORY_PERMISSION_READER` | `"reader"` | Can read records and search within a region |

### Database Column Names

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_MEMORY` | `CMS_DB_PREFIX . "memory_"` | Table name prefix for memory tables |
| `CMS_DB_MEMORY_INDEX` | `"id"` | Primary key column |
| `CMS_DB_MEMORY_MNEMONIC` | `"mnemonic"` | Mnemonic/tag list column (newline-separated) |
| `CMS_DB_MEMORY_DESCRIPTION` | `"description"` | Short description column |
| `CMS_DB_MEMORY_CONTENT` | `"content"` | Main content/text column |
| `CMS_DB_MEMORY_ATTACHMENT_TYPE` | `"attachment_type"` | MIME type of binary attachment |
| `CMS_DB_MEMORY_OWNER` | `"owner"` | User who created the record |
| `CMS_DB_MEMORY_CREATED` | `"created"` | Creation timestamp |
| `CMS_DB_MEMORY_EDITOR` | `"editor"` | Last user who edited the record |
| `CMS_DB_MEMORY_EDITED` | `"edited"` | Last edit timestamp |
| `CMS_DB_MEMORY_ACCESS_COUNT` | `"access_count"` | Number of times the record was read |
| `CMS_DB_MEMORY_ACCESSED` | `"accessed"` | Last access timestamp |
| `CMS_DB_MEMORY_FOLLOWUP` | `"followup"` | Follow-up reminder date |
| `CMS_DB_MEMORY_READ_ONLY` | `"read_only"` | Lock flag preventing edits |
| `CMS_DB_MEMORY_SAFE` | `"safe"` | Lock flag preventing deletion |
| `CMS_DB_MEMORY_RATING_VALUE` | `"rating_value"` | Average rating (0–5) |
| `CMS_DB_MEMORY_RATING_SUM` | `"rating_sum"` | Sum of all ratings |
| `CMS_DB_MEMORY_RATING_COUNT` | `"rating_count"` | Number of ratings |
| `CMS_DB_MEMORY_MNEM` | `CMS_DB_PREFIX . "memory_mnem_"` | Table name prefix for mnemonic index tables |
| `CMS_DB_MEMORY_MNEM_VALUE` | `"value"` | Mnemonic text column in index table |
| `CMS_DB_MEMORY_MNEM_COUNT` | `"count"` | Usage count of a mnemonic |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | string\|NULL | `NULL` | Current user identifier (defaults to `CMS_SUPERUSER`) |
| `$operator` | bool | `FALSE` | Whether the current user has operator-level memory permissions |
| `$mysql` | mysql | `NULL` | MySQL wrapper instance for table verification |
| `$relevance_rating` | float | `1.0` | Weight factor for rating in relevance scoring |
| `$relevance_recency` | float | `2.0` | Weight factor for recency in relevance scoring |
| `$relevance_limit` | float | `0.25` | Minimum relevance threshold (as fraction of max) for search results |
| `$cleanup_edit_expiry` | int | `180` | Days after last edit before a record is eligible for cleanup (divided by rating) |
| `$cleanup_read_expiry` | int | `90` | Days after last access before a record is eligible for cleanup (divided by rating) |

## Constructor

### `__construct($user = NULL)`

Initializes the memory instance with the given user context. Sets up the MySQL wrapper and checks whether the user has operator-level permissions.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | string\|NULL | `NULL` | User identifier; defaults to `CMS_SUPERUSER` if not provided |

**Inner Mechanisms:**
- Resolves the user to `CMS_SUPERUSER` if `NULL` is passed.
- Calls `cms_permission("memory.operator", FALSE, FALSE)` to determine operator status.
- Instantiates a `mysql` object for table verification operations.

**Usage Example:**
```php
$mem = new memory("alice");
// $mem->user is now "alice"
// $mem->operator is TRUE if alice has memory.operator permission
```

## Record Management

### `add($region, $data, $force = FALSE)`

Creates a new memory record in the specified region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier (e.g., `"scope.notes"` or `"user.alice"`) |
| `$data` | array | — | Record data with keys: `mnemonic`, `description`, `content`, `attachment`, `read_only`, `safe`, `followup` |
| `$force` | bool | `FALSE` | Bypass writer permission check if `TRUE` |

**Return Values:**
- `int` — The new record ID on success
- `FALSE` — If permission denied, invalid data, or table verification fails
- `NULL` — If the follow-up date is invalid

**Inner Mechanisms:**
1. Checks writer permission unless `$force` is `TRUE`.
2. Validates that `$data` is an array.
3. Verifies/creates the region's table via `verify_table()`.
4. Tokenizes mnemonics, truncates description to 500 chars, extracts attachment type.
5. Parses the follow-up date if provided.
6. Inserts the record with owner/editor set to the current user.
7. Indexes mnemonics in the mnemonic table.
8. Writes the attachment file if provided.

**Usage Example:**
```php
$mem = new memory("alice");
$id = $mem->add("scope.notes", [
    "mnemonic" => "project, meeting, urgent",
    "description" => "Team sync meeting notes",
    "content" => "Discussed roadmap and deadlines.",
    "attachment" => [
        "type" => "application/pdf",
        "data" => base64_encode(file_get_contents("/tmp/notes.pdf"))
    ],
    "followup" => "+1 week",
    "safe" => TRUE
]);
// $id is the new record's ID
```

### `get($region, $id)`

Retrieves a memory record by ID, incrementing its access count and clearing any follow-up date.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$id` | int | — | Record ID |

**Return Values:**
- `array` — The record data with an added `attachment_path` key if an attachment exists
- `FALSE` — If permission denied or record not found

**Inner Mechanisms:**
1. Checks reader permission.
2. Fetches the record via `get_record()`.
3. Increments `access_count`, updates `accessed` timestamp, and clears `followup`.
4. Adds `attachment_path` to the result if the record has an attachment type.

**Usage Example:**
```php
$mem = new memory("alice");
$record = $mem->get("scope.notes", 42);
if ($record !== FALSE) {
    echo $record["content"];
    if (isset($record["attachment_path"])) {
        // serve the attachment file
    }
}
```

### `get_attachment($region, $id)`

Retrieves attachment metadata (type and file path) for a record.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$id` | int | — | Record ID |

**Return Values:**
- `array` — `["type" => string, "path" => string]` on success
- `FALSE` — If permission denied, no attachment, or file missing

**Inner Mechanisms:**
1. Checks reader permission.
2. Retrieves the attachment type from the database.
3. Constructs the file path via `get_attachment_path()`.
4. Verifies the file exists on disk.

**Usage Example:**
```php
$mem = new memory("alice");
$att = $mem->get_attachment("scope.notes", 42);
if ($att !== FALSE) {
    header("Content-Type: " . $att["type"]);
    readfile($att["path"]);
}
```

### `set($region, $id, $data)`

Updates an existing memory record.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$id` | int | — | Record ID |
| `$data` | array | — | Fields to update: `mnemonic`, `description`, `content`, `attachment`, `followup`, `read_only`, `safe` |

**Return Values:**
- `TRUE` — On successful update
- `FALSE` — If permission denied, invalid data, record not found, or database error
- `NULL` — If the follow-up date is invalid

**Inner Mechanisms:**
1. Checks writer permission.
2. Fetches the existing record to check read-only/ownership constraints.
3. If the record is read-only and the user is neither the owner nor an operator, returns `FALSE`.
4. Builds a dynamic `SET` clause for all provided fields.
5. Handles attachment replacement or deletion.
6. Updates the mnemonic index if mnemonics changed.
7. Executes the `UPDATE` query.

**Usage Example:**
```php
$mem = new memory("alice");
$mem->set("scope.notes", 42, [
    "content" => "Updated content after meeting.",
    "mnemonic" => "project, meeting, completed",
    "read_only" => TRUE
]);
```

### `rate($region, $id, $value)`

Rates a record or deletes it (when rating is 0).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$id` | int | — | Record ID |
| `$value` | int | — | Rating value (1–5) or 0 to delete |

**Return Values:**
- `TRUE` — On successful rating or deletion
- `FALSE` — If permission denied, invalid value, or database error
- `int` (via `mysql_affected_rows()`) — On successful rating update

**Inner Mechanisms:**
- **Deletion (value = 0):** Checks that the record is not safe-locked, the user is the owner or an operator, then deletes the record, its attachment file, and decrements mnemonic counts.
- **Rating (1–5):** Increments `rating_sum` and `rating_count`, then recomputes `rating_value` as the average.

**Usage Example:**
```php
$mem = new memory("alice");
$mem->rate("scope.notes", 42, 5);  // Upvote
$mem->rate("scope.notes", 42, 0);  // Delete (if owner/operator and not safe)
```

## Region Management

### `get_region_list()`

Lists all accessible memory regions with their metadata.

**Return Values:**
- `array` — Associative array keyed by region name, each containing:
  - `mnemonic` — Top 10 mnemonics in the region
  - `reader` — Always `TRUE` (since only accessible regions are listed)
  - `writer` — Whether the user can write
  - `operator` — Whether the user is an operator
- `FALSE` — On database error

**Inner Mechanisms:**
1. Queries `INFORMATION_SCHEMA.TABLES` for all `memory_scope_*` and `memory_user_*` tables.
2. Converts each table name to a region via `table_to_region()`.
3. Filters to only regions where the user has reader permission.
4. For each accessible region, retrieves the top 10 mnemonics.

**Usage Example:**
```php
$mem = new memory("alice");
$regions = $mem->get_region_list();
foreach ($regions as $region => $info) {
    echo "$region: " . ($info["writer"] ? "writable" : "read-only") . "\n";
}
```

### `get_region_overview($region, $limit = 20)`

Retrieves the most relevant records from a region, ordered by a composite relevance score.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$limit` | int | `20` | Maximum number of records to return |

**Return Values:**
- `array` — List of record arrays
- `FALSE` — If permission denied or database error

**Inner Mechanisms:**
1. Checks reader permission.
2. Computes a relevance score: `rating_value * 1.0 + 2.0 / (1 + days_since_last_edit)`.
3. Orders records by this score descending.
4. Limits results to `$limit`.

**Usage Example:**
```php
$mem = new memory("alice");
$records = $mem->get_region_overview("scope.notes", 10);
foreach ($records as $r) {
    echo $r["description"] . " (rating: " . $r["rating_value"] . ")\n";
}
```

### `followup_list()`

Lists all records across all accessible regions that have a follow-up date in the past.

**Return Values:**
- `array` — List of `["region" => string, "id" => int]` entries
- `FALSE` — On database error

**Inner Mechanisms:**
1. Queries `INFORMATION_SCHEMA.TABLES` for all memory tables.
2. For each table, queries records where `followup <= NOW()`.
3. Collects region and ID pairs.

**Usage Example:**
```php
$mem = new memory("alice");
$followups = $mem->followup_list();
foreach ($followups as $f) {
    $record = $mem->get($f["region"], $f["id"]);
    echo "Follow-up: " . $record["description"] . "\n";
}
```

### `cleanup()`

Removes old, low-rated records and their associated data. Intended for daemon/cron execution.

**Return Values:**
- `TRUE` — On completion (even if some tables had errors)
- `FALSE` — If the user lacks daemon permission or the initial table query fails

**Inner Mechanisms:**
1. Requires `daemon` permission.
2. For each memory table:
   - Selects records that are not safe-locked and whose last edit/access dates exceed age limits scaled by rating.
   - Deletes attachment files for matched records.
   - Deletes the records from the database.
   - Decrements mnemonic counts and removes unused mnemonics.
   - Drops the table if it becomes empty.

**Usage Example:**
```php
// Typically called from a cron job or daemon
$mem = new memory();
$mem->cleanup();
```

## Search

### `search($region, $value, &$mnemonic = NULL)`

Searches for records within a single region using full-text search.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$value` | string | — | Search query |
| `$mnemonic` | array\|NULL | `NULL` | Output parameter: mnemonic counts for matched records |

**Return Values:**
- `array` — List of matching record arrays (with `relevance` field)
- `FALSE` — If permission denied or database error

**Inner Mechanisms:**
1. Checks reader permission.
2. Delegates to `search_table()` for the actual full-text query.
3. Tokenizes mnemonics from all matched records.
4. Retrieves mnemonic counts via `mnemonic_count()`.

**Usage Example:**
```php
$mem = new memory("alice");
$results = $mem->search("scope.notes", "meeting");
$mnemonics = NULL;
$results = $mem->search("scope.notes", "meeting", $mnemonics);
echo "Found " . count($results) . " records\n";
print_r($mnemonics);
```

### `search_all($value, &$mnemonic = NULL)`

Searches across all accessible regions and merges results by relevance.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Search query |
| `$mnemonic` | array\|NULL | `NULL` | Output parameter: aggregated mnemonic counts across all regions |

**Return Values:**
- `array` — List of matching records (each with `region` and normalized `relevance`)
- `array` — Empty array if no results
- `FALSE` — If region list retrieval fails

**Inner Mechanisms:**
1. Retrieves the accessible region list.
2. Extracts unique lowercase words from the search query.
3. For each region, checks if any mnemonic matches via the mnemonic index.
4. If matched, performs a full-text search in that region (limited to 20 results).
5. Normalizes relevance scores per region (divides by max relevance in that region).
6. Sorts all results globally by relevance.
7. Filters out results below the relevance threshold (`$relevance_limit` fraction of the top score).
8. Aggregates mnemonic counts across all regions.

**Usage Example:**
```php
$mem = new memory("alice");
$mnemonics = NULL;
$results = $mem->search_all("project meeting", $mnemonics);
foreach ($results as $r) {
    echo "[{$r["region']}] {$r["description"]} (relevance: {$r["relevance"]})\n";
}
```

### `search_table($table, $value, $limit = 200)` (private)

Executes a full-text search query against a single memory table.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$table` | string | — | Database table name |
| `$value` | string | — | Search query |
| `$limit` | int | `200` | Maximum results |

**Return Values:**
- `array` — List of matching records with a computed `relevance` field
- `FALSE` — On database error

**Inner Mechanisms:**
1. Sanitizes the search value for boolean mode (removes special characters).
2. Creates a wildcard-enhanced version for partial matching.
3. Builds a composite relevance score:
   - `MATCH(mnemonic) AGAINST(value IN NATURAL LANGUAGE MODE) * 4`
   - `MATCH(description) AGAINST(value IN NATURAL LANGUAGE MODE) * 2`
   - `MATCH(content) AGAINST(value IN NATURAL LANGUAGE MODE)`
   - Plus the same in BOOLEAN MODE with wildcard matching
4. Filters results to those above the relevance threshold.

## Mnemonic Management

### `mnemonic_list($region, $limit = 100)`

Retrieves the most frequently used mnemonics in a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$limit` | int | `100` | Maximum number of mnemonics |

**Return Values:**
- `array` — Associative array of `mnemonic => count`, sorted by count descending
- `FALSE` — If permission denied, invalid region, or database error

**Usage Example:**
```php
$mem = new memory("alice");
$tags = $mem->mnemonic_list("scope.notes", 20);
foreach ($tags as $tag => $count) {
    echo "$tag: $count\n";
}
```

### `mnemonic_count($region, $list)`

Retrieves usage counts for specific mnemonics in a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$list` | array\|string | — | Mnemonic(s) to look up |

**Return Values:**
- `array` — Associative array of `mnemonic => count` for found mnemonics
- `array` — Empty array if list is empty
- `FALSE` — If permission denied, invalid region, or database error

**Usage Example:**
```php
$mem = new memory("alice");
$counts = $mem->mnemonic_count("scope.notes", ["project", "meeting", "urgent"]);
```

### `mnemonic_sort($array)` (private)

Sorts a mnemonic array by count descending, then by name ascending.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$array` | array | — | Associative array of `mnemonic => count` |

**Return Values:**
- `array` — The sorted array (modified in place)

### `mnemonic_tokenize($value)`

Splits a mnemonic string into an array of normalized tokens.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Raw mnemonic string (comma/newline separated) |

**Return Values:**
- `array` — Sorted, unique, lowercased mnemonic tokens

**Inner Mechanisms:**
1. Strips extra whitespace.
2. Replaces commas and newlines with line breaks.
3. Limits total length to 500 characters.
4. Lowercases using multibyte-safe `utf8_strtolower()`.
5. Splits into individual tokens.
6. Truncates each token to 100 characters.
7. Removes duplicates and sorts naturally.

**Usage Example:**
```php
$mem = new memory("alice");
$tokens = $mem->mnemonic_tokenize("Project, Meeting, URGENT, project");
// Result: ["meeting", "project", "urgent"]
```

### `mnemonic_index($region, $in, $out = [])` (private)

Updates the mnemonic index table by adding new mnemonics and removing old ones.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$in` | array | — | New mnemonics to add |
| `$out` | array | `[]` | Old mnemonics to remove |

**Return Values:**
- `TRUE` — On success
- `FALSE` — If the mnemonic table name is invalid

**Inner Mechanisms:**
1. Computes the difference between `$in` and `$out` to find additions and removals.
2. Inserts new mnemonics with count 1 (using `ON DUPLICATE KEY UPDATE` to increment).
3. Decrements counts for removed mnemonics.
4. Deletes mnemonics with count ≤ 0.

### `mnemonic_rebuild($region)`

Rebuilds the entire mnemonic index for a region from scratch.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |

**Return Values:**
- `TRUE` — On success
- `FALSE` — If not an operator, invalid region, or database error

**Inner Mechanisms:**
1. Requires operator permission.
2. Truncates the mnemonic index table.
3. Scans all records in the region's main table.
4. Tokenizes and counts all mnemonics.
5. Bulk-inserts the aggregated counts.

**Usage Example:**
```php
$mem = new memory("alice");
$mem->mnemonic_rebuild("scope.notes");
```

## Permission System

### `has_permission($region, $permission)` (private)

Checks whether the current user has a specific permission level for a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$permission` | string | — | Permission level (`"reader"`, `"writer"`, or `"operator"`) |

**Return Values:**
- `TRUE` — If the user has the permission
- `FALSE` — If the region is invalid or permission is denied

**Inner Mechanisms:**
1. Parses the region into type and identifier.
2. If the user is a global operator, returns `TRUE`.
3. For user-owned regions (`user.<username>`), checks the `memory` permission.
4. For scope regions, checks `memory.<type>.<identifier>.<permission>`.

### `is_reader($region)`

Checks if the user can read from a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |

**Return Values:**
- `TRUE` — If the user has reader permission
- `FALSE` — Otherwise

### `is_writer($region)`

Checks if the user can write to a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |

**Return Values:**
- `TRUE` — If the user has writer permission
- `FALSE` — Otherwise

### `is_operator($region)`

Checks if the user is an operator for a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |

**Return Values:**
- `TRUE` — If the user has operator permission
- `FALSE` — Otherwise

## Helper Methods

### `parse_region($value)` (private)

Parses a region string into its type and identifier components.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Region string (e.g., `"scope.notes"` or `"user.alice"`) |

**Return Values:**
- `array` — `["type" => "scope"|"user", "identifier" => string]` on success
- `FALSE` — If the format is invalid or the identifier fails validation

### `verify_identifier($value)` (private)

Validates that an identifier contains only safe characters.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Identifier to validate |

**Return Values:**
- `int` — `1` if valid (matches `/^[-0-9a-z_]{1,40}$/`)
- `int` — `0` if invalid

### `table_name($value, $mnemonic = FALSE)` (private)

Generates the database table name for a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Region identifier |
| `$mnemonic` | bool | `FALSE` | If `TRUE`, returns the mnemonic index table name |

**Return Values:**
- `string` — Table name (e.g., `"cms_memory_scope_notes"` or `"cms_memory_mnem_scope_notes"`)
- `FALSE` — If the region is invalid

### `table_to_region($value)` (private)

Converts a database table name back to a region identifier.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | string | — | Table name |

**Return Values:**
- `string` — Region identifier (e.g., `"scope.notes"`)
- `FALSE` — If the table name doesn't match the expected pattern

### `get_attachment_path($region, $id)` (private)

Constructs the filesystem path for a record's attachment.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |
| `$id` | int | — | Record ID |

**Return Values:**
- `string` — Full filesystem path to the attachment file
- `FALSE` — If the region is invalid

**Inner Mechanisms:**
Uses a sharded directory structure based on the record ID to avoid filesystem limits:
```
CMS_DATA_PATH/#memory/<type>/<identifier>/<id/1000000>/<id/1000>/<id>
```

### `verify_table($region)` (private)

Creates or verifies the database tables for a region.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$region` | string | — | Region identifier |

**Return Values:**
- `TRUE` — If both tables are verified/created successfully
- `FALSE` — If either table verification fails

**Inner Mechanisms:**
1. Checks if the MySQL version supports the `ngram` parser (MySQL ≥ 5.7.6).
2. Creates the main memory table with all columns, indexes, and full-text indexes.
3. Creates the mnemonic index table with value/count columns.

### `get_record($table, $id)` (private)

Fetches a single record from a memory table.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$table` | string | — | Database table name |
| `$id` | int | — | Record ID |

**Return Values:**
- `array` — Associative array of the record's columns
- `FALSE` — If the table is invalid or the query fails

### `parse_date($value)`

Parses various date formats into a standardized MySQL datetime string.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | int\|string | — | Unix timestamp, date string, or relative date expression |

**Return Values:**
- `string` — Formatted datetime string (`"Y-m-d H:i:s"`) in UTC
- `NULL` — If the value is invalid or cannot be parsed

**Inner Mechanisms:**
1. For integers: validates non-negative, uses as Unix timestamp.
2. For strings: uses `strtotime()` to parse.
3. Converts to UTC via `gmdate()`.

**Usage Example:**
```php
$mem = new memory("alice");
$ts = $mem->parse_date("+1 week");     // "2026-02-08 12:00:00"
$ts = $mem->parse_date(1700000000);    // "2023-11-14 22:13:20"
$ts = $mem->parse_date("invalid");     // NULL
```


<!-- HASH:87f8d822a08d2ae309051c4a9f12df94 -->
