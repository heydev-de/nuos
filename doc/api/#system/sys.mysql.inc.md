# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mysql.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MySQL Database Class

The `mysql` class provides a comprehensive interface for interacting with MySQL/MariaDB databases in the PWNC Web Platform. It handles connection management, CRUD operations, schema verification, data export/import, and backup/restore functionality. The class ensures secure, efficient, and consistent database operations with built-in escaping and multibyte support.

---

### Properties

| Name        | Default Value       | Description                                                                 |
|-------------|---------------------|-----------------------------------------------------------------------------|
| `database`  | `NULL`              | Database name retrieved from system configuration.                          |
| `host`      | `NULL`              | Database host retrieved from system configuration.                          |
| `user`      | `NULL`              | Database user retrieved from system configuration.                          |
| `password`  | `NULL`              | Database password retrieved from system configuration.                      |
| `software`  | `NULL`              | Detected database software (MySQL or MariaDB).                              |
| `version`   | `NULL`              | Detected database version.                                                  |
| `engine`    | `"InnoDB"`          | Default storage engine for tables.                                          |
| `charset`   | `"utf8mb4"`         | Default character set for tables.                                           |
| `collation` | `"utf8mb4_unicode_ci"` | Default collation for tables.                                           |

---

### Constructor

#### `__construct()`

**Purpose:**
Initializes the MySQL connection by retrieving credentials from the system configuration and establishing a connection.

**Parameters:**
None.

**Return Values:**
None (constructor).

**Inner Mechanisms:**
- Retrieves database credentials (`database`, `host`, `user`, `password`) using the `system` class.
- Calls `connection()` to establish a database connection.

**Usage Context:**
Automatically invoked when creating a new instance of the `mysql` class. Typically used at the start of database operations.

**Example:**
```php
$db = new \cms\mysql();
```

---

### Methods

#### `connection()`

**Purpose:**
Establishes and manages a persistent database connection. Validates software/version compatibility and initializes session settings.

**Parameters:**
None.

**Return Values:**
- **`mysqli` object** – Active database connection.
- **`FALSE`** – If connection fails or credentials are missing.

**Inner Mechanisms:**
- Uses a static `$init` array to track initialized connections by thread ID.
- Validates MySQL (≥5.6) or MariaDB (≥10) compatibility.
- Sets session parameters: `sql_mode`, `NAMES`, and `time_zone`.
- Selects the database after initialization.

**Usage Context:**
Called internally by other methods. Can be used to verify or re-establish a connection.

**Example:**
```php
if ($db->connection() === FALSE) {
    die("Database connection failed.");
}
```

---

#### `get($index, $column, $table, $index_key = "id")`

**Purpose:**
Retrieves a single value from a table by matching a column with a given index.

**Parameters:**

| Name        | Type     | Description                                      |
|-------------|----------|--------------------------------------------------|
| `$index`    | `mixed`  | Value to match against the index key.            |
| `$column`   | `string` | Column name to retrieve.                         |
| `$table`    | `string` | Table name.                                      |
| `$index_key`| `string` | Column name to use as the index (default: `"id"`).|

**Return Values:**
- **`string`** – Retrieved value.
- **`FALSE`** – If query fails or no result is found.

**Inner Mechanisms:**
- Escapes all parameters using `sqlesc()`.
- Executes a `SELECT` query with `LIMIT 1`.

**Usage Context:**
Fetching a single field (e.g., user email, configuration value).

**Example:**
```php
$email = $db->get(42, "email", "users");
if ($email !== FALSE) {
    echo "User email: " . x($email);
}
```

---

#### `set($index, $column, $table, $value, $index_key = "id")`

**Purpose:**
Updates a single value in a table by matching a column with a given index.

**Parameters:**

| Name        | Type     | Description                                      |
|-------------|----------|--------------------------------------------------|
| `$index`    | `mixed`  | Value to match against the index key.            |
| `$column`   | `string` | Column name to update.                           |
| `$table`    | `string` | Table name.                                      |
| `$value`    | `mixed`  | New value to set.                                |
| `$index_key`| `string` | Column name to use as the index (default: `"id"`).|

**Return Values:**
- **`TRUE`** – If update succeeds.
- **`FALSE`** – If query fails.

**Inner Mechanisms:**
- Escapes all parameters using `sqlesc()`.
- Executes an `UPDATE` query with `LIMIT 1`.

**Usage Context:**
Modifying a single field (e.g., updating a user's status).

**Example:**
```php
if ($db->set(42, "status", "users", "active")) {
    echo "User status updated.";
}
```

---

#### `delete($index, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Recursively deletes a record and all its child records in a hierarchical table structure.

**Parameters:**

| Name         | Type     | Description                                      |
|--------------|----------|--------------------------------------------------|
| `$index`     | `mixed`  | Value to match against the index key.            |
| `$table`     | `string` | Table name.                                      |
| `$index_key` | `string` | Column name to use as the index (default: `"id"`).|
| `$parent_key`| `string` | Column name referencing the parent (default: `"container"`).|

**Return Values:**
- **`TRUE`** – If deletion succeeds.
- **`FALSE`** – If query fails.

**Inner Mechanisms:**
- Uses recursion to delete child records before deleting the parent.
- Escapes all parameters using `sqlesc()`.

**Usage Context:**
Deleting a record and its descendants (e.g., removing a category and all subcategories).

**Example:**
```php
if ($db->delete(12, "categories")) {
    echo "Category and subcategories deleted.";
}
```

---

#### `is_child($index, $parent, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Checks if a record is a descendant of a given parent record in a hierarchical structure.

**Parameters:**

| Name         | Type     | Description                                      |
|--------------|----------|--------------------------------------------------|
| `$index`     | `mixed`  | Value of the record to check.                    |
| `$parent`    | `mixed`  | Value of the potential ancestor.                 |
| `$table`     | `string` | Table name.                                      |
| `$index_key` | `string` | Column name to use as the index (default: `"id"`).|
| `$parent_key`| `string` | Column name referencing the parent (default: `"container"`).|

**Return Values:**
- **`TRUE`** – If the record is a descendant.
- **`FALSE`** – If not or if query fails.

**Inner Mechanisms:**
- Traverses the hierarchy using a `do-while` loop until the parent is found or the chain breaks.

**Usage Context:**
Checking relationships (e.g., verifying if a page belongs to a specific section).

**Example:**
```php
if ($db->is_child(45, 10, "pages")) {
    echo "Page is a descendant of section 10.";
}
```

---

#### `verify_table($table, $column, $index = NULL, $mapping = NULL)`

**Purpose:**
Ensures a table exists and matches a specified schema. Creates or alters the table as needed.

**Parameters:**

| Name      | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| `$table`  | `string`           | Table name.                                                                 |
| `$column` | `array`            | Associative array of column definitions (e.g., `["name" => "VARCHAR(255)"]`).|
| `$index`  | `array` (optional) | Array of index definitions (e.g., `["PRIMARY KEY (id)"]`).                 |
| `$mapping`| `array` (optional) | Column name mappings for renaming (e.g., `[0 => "new_name"]`).             |

**Return Values:**
- **`TRUE`** – If table matches schema or was successfully altered.
- **`FALSE`** – If operation fails.

**Inner Mechanisms:**
- Uses a schema hash (`hash32`) to detect changes.
- Creates a temporary table to compare structures.
- Generates and executes `ALTER TABLE` statements for modifications.
- Handles column renaming, index changes, and auto-increment adjustments.

**Usage Context:**
Schema migration or ensuring tables exist during module installation.

**Example:**
```php
$columns = [
    "id" => "INT UNSIGNED AUTO_INCREMENT PRIMARY KEY",
    "name" => "VARCHAR(255) NOT NULL",
    "created" => "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
];
$indices = ["INDEX (name)"];
if ($db->verify_table("products", $columns, $indices)) {
    echo "Table verified or created.";
}
```

---

#### `export_sql($table, $file = NULL)`

**Purpose:**
Exports a table's schema (CREATE TABLE statement) to a `.sql` file.

**Parameters:**

| Name     | Type     | Description                                      |
|----------|----------|--------------------------------------------------|
| `$table` | `string` | Table name.                                      |
| `$file`  | `string` (optional) | Output file path (default: `CMS_DATA_PATH/#database/$table.sql`).|

**Return Values:**
- **`TRUE`** – If export succeeds.
- **`FALSE`** – If query or file operation fails.

**Inner Mechanisms:**
- Uses `SHOW CREATE TABLE` to retrieve the schema.
- Writes the output to a file using `write_file()`.

**Usage Context:**
Backing up table structures or migrating schemas.

**Example:**
```php
if ($db->export_sql("users")) {
    echo "SQL export completed.";
}
```

---

#### `export_csv($table, $separator = ",", $delimiter = "\"", $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to a CSV file.

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name.                                      |
| `$separator` | `string`  | Field separator (default: `,`).                  |
| `$delimiter` | `string`  | Text delimiter (default: `"`).                   |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH/#database/$table.csv`).|
| `$set_fields`| `boolean` | Whether to include a header row (default: `FALSE`).|

**Return Values:**
- **`TRUE`** – If export succeeds.
- **`FALSE`** – If query or file operation fails.

**Inner Mechanisms:**
- Uses `CONCAT_WS` and `REPLACE` to escape delimiters and separators.
- Writes data in chunks to handle large tables.

**Usage Context:**
Data backup or exchange with external systems.

**Example:**
```php
if ($db->export_csv("orders", ",", "\"", NULL, TRUE)) {
    echo "CSV export completed with headers.";
}
```

---

#### `export_excel($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an Excel-compatible `.xls` file (UTF-16LE encoded TSV).

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name.                                      |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH/#database/$table.xls`).|
| `$set_fields`| `boolean` | Whether to include a header row (default: `FALSE`).|

**Return Values:**
- **`TRUE`** – If export succeeds.
- **`FALSE`** – If operation fails.

**Inner Mechanisms:**
- Uses `export_csv()` to generate a temporary TSV file.
- Converts the file to UTF-16LE with a BOM using a stream filter.

**Usage Context:**
Generating Excel-compatible reports.

**Example:**
```php
if ($db->export_excel("products")) {
    echo "Excel export completed.";
}
```

---

#### `export_html($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an HTML file with a structured table.

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name.                                      |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH/#database/$table.htm`).|
| `$set_fields`| `boolean` | Whether to include a header row (default: `FALSE`).|

**Return Values:**
- **`TRUE`** – If export succeeds.
- **`FALSE`** – If query or file operation fails.

**Inner Mechanisms:**
- Escapes HTML content using `x()` and `nl2br()`.
- Generates a complete HTML document with `<table>`, `<thead>`, and `<tbody>`.

**Usage Context:**
Generating human-readable reports or documentation.

**Example:**
```php
if ($db->export_html("customers")) {
    echo "HTML export completed.";
}
```

---

#### `import_csv($file, $separator = ",", $delimiter = "\"", $table = NULL, $get_fields = FALSE, $ignore_first_row = FALSE, $ignore_existing = TRUE, $mapping = NULL)`

**Purpose:**
Imports data from a CSV file into a table.

**Parameters:**

| Name                | Type               | Description                                      |
|---------------------|--------------------|--------------------------------------------------|
| `$file`             | `string`           | Path to the CSV file.                            |
| `$separator`        | `string`           | Field separator (default: `,`).                  |
| `$delimiter`        | `string`           | Text delimiter (default: `"`).                   |
| `$table`            | `string` (optional)| Target table name.                               |
| `$get_fields`       | `boolean`          | If `TRUE`, returns the first row as field names (default: `FALSE`).|
| `$ignore_first_row` | `boolean`          | If `TRUE`, skips the first row (default: `FALSE`).|
| `$ignore_existing`  | `boolean`          | If `TRUE`, uses `INSERT IGNORE` (default: `TRUE`).|
| `$mapping`          | `array` (optional) | Column index to field name mapping (e.g., `[0 => "name"]`).|

**Return Values:**
- **`TRUE`** – If import succeeds.
- **`FALSE`** – If operation fails.
- **`array`** – If `$get_fields` is `TRUE`, returns the first row as field names.

**Inner Mechanisms:**
- Parses CSV manually for performance (avoids `fgetcsv` overhead).
- Buffers 100 records per query to optimize performance.
- Supports column mapping and conditional insertion.

**Usage Context:**
Bulk data import (e.g., importing user lists or product catalogs).

**Example:**
```php
$mapping = [0 => "id", 1 => "name", 2 => "email"];
if ($db->import_csv("users.csv", ",", "\"", "users", FALSE, TRUE, TRUE, $mapping)) {
    echo "CSV import completed.";
}
```

---

#### `backup()`

**Purpose:**
Backs up all non-backup tables to the `#database/backup/` directory.

**Parameters:**
None.

**Return Values:**
- **`TRUE`** – If backup succeeds.
- **`FALSE`** – If operation fails.

**Inner Mechanisms:**
- Uses `export_sql()` and `export_csv()` to create `.sql` and `.csv` backups.
- Skips tables prefixed with `#backup_`.

**Usage Context:**
Automated or manual database backups.

**Example:**
```php
if ($db->backup()) {
    echo "Database backup completed.";
}
```

---

#### `restore()`

**Purpose:**
Restores tables from the `#database/backup/` directory.

**Parameters:**
None.

**Return Values:**
- **`TRUE`** – If restore succeeds.
- **`FALSE`** – If operation fails.

**Inner Mechanisms:**
- Reads `.sql` files to recreate tables.
- Uses `import_csv()` to restore data from `.csv` files.
- Creates backups of existing tables before restoration.

**Usage Context:**
Disaster recovery or migrating data between environments.

**Example:**
```php
if ($db->restore()) {
    echo "Database restore completed.";
}
```

---

#### `drop_table($table)`

**Purpose:**
Drops a table from the database.

**Parameters:**

| Name     | Type     | Description       |
|----------|----------|-------------------|
| `$table` | `string` | Table name.       |

**Return Values:**
- **`TRUE`** – If drop succeeds.
- **`FALSE`** – If query fails.

**Inner Mechanisms:**
- Executes a `DROP TABLE` query.

**Usage Context:**
Cleanup or schema changes.

**Example:**
```php
if ($db->drop_table("temp_logs")) {
    echo "Table dropped.";
}
```


<!-- HASH:3ca2c27c1a7a981193674ddf4378e254 -->
