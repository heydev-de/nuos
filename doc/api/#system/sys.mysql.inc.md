# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.mysql.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## MySQL Database Management Class

The `mysql` class provides a comprehensive interface for interacting with MySQL/MariaDB databases in the NUOS platform. It handles connection management, table operations, data manipulation, and import/export functionality while ensuring compatibility with the platform's core principles of performance and simplicity.

### Class Properties

| Name       | Default Value       | Description                                                                 |
|------------|---------------------|-----------------------------------------------------------------------------|
| database   | NULL                | Database name                                                               |
| host       | NULL                | Database server host                                                        |
| user       | NULL                | Database username                                                           |
| password   | NULL                | Database password                                                           |
| software   | NULL                | Database software (MySQL or MariaDB)                                        |
| version    | NULL                | Database version                                                            |
| engine     | "InnoDB"            | Default storage engine                                                      |
| charset    | "utf8mb4"           | Default character set                                                       |
| collation  | "utf8mb4_unicode_ci"| Default collation                                                           |

---

### `__construct()`

**Purpose:**
Initializes the MySQL connection using system configuration values.

**Parameters:**
None

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Retrieves database credentials from the system configuration
2. Establishes the initial database connection via `connection()`

**Usage:**
Automatically called when instantiating the class. Typically used as:
```php
$db = new \cms\mysql();
```

---

### `connection()`

**Purpose:**
Establishes and maintains a persistent database connection, performs initialization tasks, and validates database compatibility.

**Parameters:**
None

**Return Values:**
- `resource|FALSE`: MySQL connection resource on success, FALSE on failure

**Inner Mechanisms:**
1. Uses static `$initialize` flag to prevent redundant initialization
2. Validates database software and version requirements:
   - MySQL ≥ 5.6
   - MariaDB ≥ 10
3. Executes initialization queries:
   - Sets SQL mode to empty
   - Configures character set and collation
   - Sets timezone to UTC (+00:00)

**Usage:**
Automatically called by other methods. Can be manually invoked to verify connection status.

---

### `get($index, $column, $table, $index_key = "id")`

**Purpose:**
Retrieves a single value from a database table by primary key.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $index    | mixed    | Primary key value                    |
| $column   | string   | Column name to retrieve              |
| $table    | string   | Table name                           |
| $index_key| string   | Primary key column name (default: id)|

**Return Values:**
- `mixed|FALSE`: Retrieved value on success, FALSE on failure

**Inner Mechanisms:**
1. Establishes database connection
2. Executes a parameterized SELECT query with LIMIT 1
3. Returns the first column of the first result row

**Usage:**
```php
$value = $db->get(42, 'username', 'users');
```

---

### `set($index, $column, $table, $value, $index_key = "id")`

**Purpose:**
Updates a single value in a database table by primary key.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $index    | mixed    | Primary key value                    |
| $column   | string   | Column name to update                |
| $table    | string   | Table name                           |
| $value    | mixed    | New value to set                     |
| $index_key| string   | Primary key column name (default: id)|

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Establishes database connection
2. Executes a parameterized UPDATE query with LIMIT 1

**Usage:**
```php
$success = $db->set(42, 'username', 'users', 'new_username');
```

---

### `delete($index, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Recursively deletes a record and all its child records from a hierarchical table structure.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $index    | mixed    | Primary key value to delete          |
| $table    | string   | Table name                           |
| $index_key| string   | Primary key column name (default: id)|
| $parent_key| string  | Parent reference column (default: container)|

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Uses recursive deletion to handle hierarchical data
2. First retrieves all child records via the parent reference column
3. Deletes children before deleting the parent record

**Usage:**
```php
$success = $db->delete(42, 'pages');
```

---

### `is_child($index, $parent, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Determines if a record is a descendant of a specified parent record in a hierarchical structure.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| $index    | mixed    | Record to check                      |
| $parent   | mixed    | Potential ancestor record            |
| $table    | string   | Table name                           |
| $index_key| string   | Primary key column name (default: id)|
| $parent_key| string  | Parent reference column (default: container)|

**Return Values:**
- `bool`: TRUE if $index is a descendant of $parent, FALSE otherwise

**Inner Mechanisms:**
1. Uses recursive traversal of the parent reference column
2. Follows the chain of parent references until:
   - The specified parent is found (returns TRUE)
   - The chain ends without finding the parent (returns FALSE)
   - A circular reference is detected (returns FALSE)

**Usage:**
```php
$isDescendant = $db->is_child(42, 5, 'pages');
```

---

### `verify_table($table, $column, $index = NULL, $mapping = NULL)`

**Purpose:**
Ensures a table exists with the specified schema, performing structural synchronization if necessary.

**Parameters:**

| Name    | Type         | Description                                                                 |
|---------|--------------|-----------------------------------------------------------------------------|
| $table  | string       | Table name                                                                 |
| $column | array        | Associative array of column definitions (name => definition)               |
| $index  | array        | Array of index definitions                                                  |
| $mapping| array        | Column mapping for renaming (source => target)                             |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Uses schema hashing to detect changes
2. Performs comprehensive structural comparison:
   - Column definitions
   - Index definitions
   - Auto-increment settings
   - Character set and collation
3. Generates and executes ALTER TABLE statements to synchronize structure
4. Handles column renaming via mapping
5. Preserves data during structural changes

**Usage:**
```php
$success = $db->verify_table('users', [
    'id' => 'int(11) NOT NULL AUTO_INCREMENT',
    'username' => 'varchar(255) NOT NULL',
    'email' => 'varchar(255) NOT NULL'
], [
    'PRIMARY KEY (id)',
    'UNIQUE KEY username (username)',
    'UNIQUE KEY email (email)'
]);
```

---

### `export_sql($table, $file = NULL)`

**Purpose:**
Exports a table's structure (CREATE TABLE statement) to a SQL file.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| $table| string | Table name                           |
| $file | string | Output file path (default: #database/$table.sql)|

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Retrieves the CREATE TABLE statement via SHOW CREATE TABLE
2. Writes the statement to the specified file
3. Creates parent directories if they don't exist

**Usage:**
```php
$success = $db->export_sql('users');
```

---

### `export_csv($table, $separator = ",", $delimiter = "\"", $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to a CSV file with proper character encoding and escaping.

**Parameters:**

| Name        | Type    | Description                          |
|-------------|---------|--------------------------------------|
| $table      | string  | Table name                           |
| $separator  | string  | Field separator (default: ",")       |
| $delimiter  | string  | Text delimiter (default: "\"")       |
| $file       | string  | Output file path (default: #database/$table.csv)|
| $set_fields | bool    | Include field names as first row     |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Uses CONCAT_WS and REPLACE to properly escape CSV data
2. Handles NULL values by casting to empty strings
3. Processes data in a single query to minimize memory usage
4. Creates parent directories if they don't exist

**Usage:**
```php
$success = $db->export_csv('users', ',', '"', NULL, TRUE);
```

---

### `export_excel($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an Excel-compatible file (UTF-16LE encoded tab-delimited).

**Parameters:**

| Name        | Type    | Description                          |
|-------------|---------|--------------------------------------|
| $table      | string  | Table name                           |
| $file       | string  | Output file path (default: #database/$table.xls)|
| $set_fields | bool    | Include field names as first row     |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Uses `export_csv()` to generate a temporary tab-delimited file
2. Converts the file to UTF-16LE encoding with BOM
3. Uses stream filters for efficient conversion
4. Cleans up temporary files

**Usage:**
```php
$success = $db->export_excel('users');
```

---

### `export_html($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an HTML file with proper escaping.

**Parameters:**

| Name        | Type    | Description                          |
|-------------|---------|--------------------------------------|
| $table      | string  | Table name                           |
| $file       | string  | Output file path (default: #database/$table.htm)|
| $set_fields | bool    | Include field names as table header  |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Generates a complete HTML document with proper structure
2. Uses the platform's `x()` function for HTML escaping
3. Uses `nl2br()` for proper newline handling
4. Creates parent directories if they don't exist

**Usage:**
```php
$success = $db->export_html('users');
```

---

### `import_csv($file, $separator = ",", $delimiter = "\"", $table = NULL, $get_fields = FALSE, $ignore_first_row = FALSE, $ignore_existing = TRUE, $mapping = NULL)`

**Purpose:**
Imports data from a CSV file into a database table.

**Parameters:**

| Name               | Type         | Description                                                                 |
|--------------------|--------------|-----------------------------------------------------------------------------|
| $file              | string       | Input CSV file path                                                         |
| $separator         | string       | Field separator (default: ",")                                              |
| $delimiter         | string       | Text delimiter (default: "\"")                                              |
| $table             | string       | Target table name                                                           |
| $get_fields        | bool         | Return field names from first row instead of importing                      |
| $ignore_first_row  | bool         | Skip first row (typically field names)                                      |
| $ignore_existing   | bool         | Use INSERT IGNORE instead of REPLACE                                        |
| $mapping           | array        | Field mapping (CSV column index => database column name)                   |

**Return Values:**
- `bool|array`: TRUE on success, FALSE on failure, or array of field names if $get_fields is TRUE

**Inner Mechanisms:**
1. Uses custom CSV parsing for performance (avoids fgetcsv overhead)
2. Processes files in chunks to minimize memory usage
3. Supports column mapping for non-matching CSV structures
4. Uses buffered inserts (100 records at a time) for efficiency
5. Handles escaped delimiters and proper character encoding

**Usage:**
```php
// Basic import
$success = $db->import_csv('users.csv', ',', '"', 'users');

// Import with column mapping
$success = $db->import_csv('users.csv', ',', '"', 'users', FALSE, TRUE, TRUE, [
    0 => 'username',
    1 => 'email',
    2 => 'created_at'
]);

// Get field names
$fields = $db->import_csv('users.csv', ',', '"', NULL, TRUE);
```

---

### `backup()`

**Purpose:**
Creates a complete backup of all database tables (excluding backup tables).

**Parameters:**
None

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Retrieves list of all tables via SHOW TABLES
2. Exports each table's structure and data using `export_sql()` and `export_csv()`
3. Stores backups in #database/backup/ directory
4. Skips tables with names beginning with "#backup_"

**Usage:**
```php
$success = $db->backup();
```

---

### `restore()`

**Purpose:**
Restores database tables from a backup created by the `backup()` method.

**Parameters:**
None

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Scans the #database/backup/ directory for backup files
2. For each table:
   - Creates a backup of the existing table (if it exists)
   - Executes the SQL file to recreate the table structure
   - Imports data from the CSV file
3. Handles failures by restoring the original table

**Usage:**
```php
$success = $db->restore();
```

---

### `drop_table($table)`

**Purpose:**
Drops a database table.

**Parameters:**

| Name  | Type   | Description  |
|-------|--------|--------------|
| $table| string | Table name   |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure

**Inner Mechanisms:**
1. Executes a parameterized DROP TABLE statement

**Usage:**
```php
$success = $db->drop_table('temp_users');
```


<!-- HASH:96a7d0e2f882bf06c2b01c550fc62634 -->
