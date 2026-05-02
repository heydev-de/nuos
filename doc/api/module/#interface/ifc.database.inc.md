# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.database.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Database Interface Module (`ifc.database.inc`)

This file provides a comprehensive web-based interface for managing MySQL databases within the NUOS platform. It enables users to perform database operations such as table creation, modification, data editing, importing/exporting, and maintenance tasks through an intuitive UI.

---

## Interface Overview

The module handles various database operations via message-driven cases (`CMS_IFC_MESSAGE`). It integrates with the NUOS platform's core utilities for database access, caching, and UI rendering.

### Key Features

- **Table Management**: Create, alter, and delete tables.
- **Field Management**: Add, modify, and delete fields.
- **Index Management**: Create and manage indexes.
- **Data Manipulation**: Edit, add, and delete records.
- **Import/Export**: Export table definitions and data; import CSV data.
- **Maintenance**: Backup, restore, repair, and optimize tables.
- **SQL Console**: Execute custom SQL queries.

---

## Core Functions and Message Handlers

### `select`

**Purpose**
Selects a database object (table, field, or index) for detailed inspection or editing.

**Parameters**

| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param`  | `string` | The object identifier (e.g., `table.field`). |

**Return Values**
None. Updates the `$object` variable and caches the selection.

**Inner Mechanisms**
- Updates the current object selection.
- Caches the selection for persistence across requests.

**Usage Context**
Triggered when a user clicks on a database object in the UI.

---

### `sql_console`

**Purpose**
Provides an interactive SQL console for executing custom SQL queries.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | SQL query to execute.                |
| `$execute`     | `bool`    | Flag indicating whether to execute the query. |

**Return Values**
None. Outputs the query result or error message.

**Inner Mechanisms**
- Retrieves the last executed query from cache.
- Displays a textarea for query input.
- Executes the query and displays results in a table format if successful.
- Caches the last query for future use.

**Usage Context**
Used for ad-hoc SQL query execution and debugging.

---

### `edit_table` / `_edit_table`

**Purpose**
Displays and allows editing of table data with pagination, filtering, and sorting.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `int`     | Page offset (default: `0`).          |
| `$ifc_param2`  | `int`     | Rows per page (default: `25`).       |
| `$ifc_param3`  | `int`     | Characters per field (default: `25`). |
| `$ifc_param4`  | `string`  | Field to filter by.                  |
| `$ifc_param5`  | `string`  | Filter operation (e.g., `LIKE`, `=`).|
| `$ifc_param6`  | `string`  | Filter value.                        |
| `$ifc_param7`  | `string`  | Field to sort by.                    |
| `$ifc_param8`  | `mixed`   | New value for field updates.         |
| `$row`         | `string`  | Current row identifier.              |
| `$field`       | `string`  | Current field being edited.          |
| `$list`        | `array`   | List of selected rows for batch operations. |

**Return Values**
None. Renders a table editor UI with data and controls.

**Inner Mechanisms**
- Retrieves table structure (columns and types).
- Determines the primary key or generates a hash-based index.
- Handles pagination, filtering, and sorting.
- Supports inline editing of field values based on data type.
- Provides batch operations (e.g., delete selected rows).

**Usage Context**
Used for viewing and editing table data in a tabular format.

---

### `edit_table_add`

**Purpose**
Adds a new record to the table.

**Parameters**
None (uses `$object` to identify the table).

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Executes an `INSERT` query to add an empty record.
- Returns the ID of the newly inserted row.

**Usage Context**
Triggered when the user clicks the "Add Record" button.

---

### `edit_table_update`

**Purpose**
Updates a field value in a specific record.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param8`  | `mixed`   | New value for the field.             |
| `$row`         | `string`  | Row identifier.                      |
| `$field`       | `string`  | Field to update.                     |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Constructs an `UPDATE` query based on the field type.
- Handles special cases (e.g., `bit`, `enum`, `set`).
- Updates the row and resets the selection.

**Usage Context**
Triggered when the user confirms an inline edit.

---

### `edit_table_delete`

**Purpose**
Deletes selected records from the table.

**Parameters**

| Name   | Type    | Description                          |
|--------|---------|--------------------------------------|
| `$list`| `array` | List of row identifiers to delete.   |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Constructs a `DELETE` query for the selected rows.
- Executes the query and updates the UI.

**Usage Context**
Triggered when the user clicks the "Delete Selected" button.

---

### `alter_table` / `_create_table`

**Purpose**
Alters an existing table or creates a new one.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | New table name.                      |
| `$ifc_param2`  | `string`  | Storage engine (e.g., `InnoDB`).     |
| `$ifc_param3`  | `string`  | Collation (e.g., `utf8mb4_general_ci`).|
| `$ifc_param4`  | `string`  | Table comment.                       |
| `$ifc_param5`  | `string`  | Path to a SQL definition file.       |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- For `alter_table`: Modifies table properties (name, engine, collation, comment).
- For `_create_table`: Creates a table from scratch or using a definition file.
- Handles SQL file parsing and execution.

**Usage Context**
Used for table creation and structural modifications.

---

### `add_field` / `_add_field` / `change_field`

**Purpose**
Adds a new field to a table or modifies an existing one.

**Parameters**

| Name            | Type      | Description                          |
|-----------------|-----------|--------------------------------------|
| `$ifc_param1`   | `string`  | Field type (e.g., `int`, `varchar`). |
| `$ifc_param2`   | `string`  | Field name.                          |
| `$ifc_param3`   | `bool`    | Whether the field allows `NULL`.     |
| `$ifc_param4`   | `string`  | Default value.                       |
| `$ifc_param5`   | `int`     | Length for `bit` fields.             |
| `$ifc_param6`   | `int`     | Length for `char`/`varchar` fields.  |
| `$ifc_param7`   | `string`  | Collation for text fields.           |
| `$ifc_param8`   | `int`     | Length for integer fields.           |
| `$ifc_param9`   | `string`  | Attributes (e.g., `UNSIGNED`).       |
| `$ifc_param10`  | `bool`    | Auto-increment flag.                 |
| `$ifc_param11`  | `string`  | Collation for text fields.           |
| `$ifc_param12`  | `int`     | Length for decimal fields.           |
| `$ifc_param13`  | `int`     | Decimals for decimal fields.         |
| `$ifc_param14`  | `string`  | Attributes for decimal fields.       |
| `$ifc_param15`  | `int`     | Length for binary fields.            |
| `$ifc_param16`  | `int`     | Length for `year` fields.            |
| `$ifc_param17`  | `string`  | Values for `enum`/`set` fields.      |
| `$ifc_param18`  | `string`  | Collation for `enum`/`set` fields.   |
| `$ifc_param19`  | `string`  | Field comment.                       |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Constructs an `ALTER TABLE` query to add or modify a field.
- Handles type-specific properties (e.g., length, collation, attributes).
- Executes the query and updates the UI.

**Usage Context**
Used for field creation and modification.

---

### `create_index` / `_create_index`

**Purpose**
Creates a new index on a table.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | Index type (`INDEX`, `UNIQUE`, `PRIMARY KEY`, `FULLTEXT`). |
| `$ifc_param2`  | `string`  | Index name.                          |
| `$ifc_param3`  | `string`  | Index format (`BTREE`, `HASH`).      |
| `$list`        | `array`   | List of fields to include in the index. |
| `$length`      | `array`   | Length for each field in the index.  |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Constructs an `ALTER TABLE` query to add the index.
- Handles field selection and length specification.
- Executes the query and updates the UI.

**Usage Context**
Used for index creation and management.

---

### `export_definition` / `_export_definition`

**Purpose**
Exports a table's definition to a SQL file.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | Output file name.                    |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` and a download button on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Uses the `mysql` class to export the table definition.
- Provides a download link for the generated SQL file.

**Usage Context**
Used for backing up table schemas.

---

### `export_table` / `_export_table`

**Purpose**
Exports table data to HTML, XLS, or CSV format.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | Output file name.                    |
| `$ifc_param2`  | `int`     | Export format (`0`: HTML, `1`: XLS, `2`: CSV). |
| `$ifc_param3`  | `bool`    | Whether to include all fields.       |
| `$ifc_param4`  | `string`  | CSV separator.                       |
| `$ifc_param5`  | `string`  | CSV delimiter.                       |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` and a download button on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Uses the `mysql` class to export data in the specified format.
- Provides a download link for the generated file.

**Usage Context**
Used for data backup and reporting.

---

### `import_table` / `_import_table` / `__import_table` / `___import_table`

**Purpose**
Imports data from a CSV file into a table.

**Parameters**

| Name                | Type      | Description                          |
|---------------------|-----------|--------------------------------------|
| `$ifc_param1`       | `string`  | Path to the CSV file.                |
| `$ifc_param2`       | `string`  | CSV separator.                       |
| `$ifc_param3`       | `string`  | CSV delimiter.                       |
| `$ifc_param4`       | `bool`    | Whether to ignore the first row.     |
| `$ifc_param5`       | `bool`    | Whether to ignore existing records.  |
| `$mapping`          | `array`   | Field mapping for import.            |
| `$file`             | `string`  | Path to the CSV file.                |
| `$separator`        | `string`  | CSV separator.                       |
| `$delimiter`        | `string`  | CSV delimiter.                       |
| `$ignore_first_row` | `bool`    | Whether to ignore the first row.     |
| `$ignore_existing`  | `bool`    | Whether to ignore existing records.  |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- For `___import_table`: Creates a new table from the CSV file.
- For `__import_table`: Imports data into an existing table with field mapping.
- Uses the `mysql` class to parse and import CSV data.

**Usage Context**
Used for data migration and bulk imports.

---

### `delete`

**Purpose**
Deletes selected database objects (tables, fields, or indexes).

**Parameters**

| Name   | Type    | Description                          |
|--------|---------|--------------------------------------|
| `$list`| `array` | List of objects to delete.           |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Constructs and executes `DROP` or `ALTER TABLE` queries for each object.
- Handles errors and updates the UI.

**Usage Context**
Triggered when the user clicks the "Delete Selected" button.

---

### `backup` / `restore` / `maintain`

**Purpose**
Performs database maintenance operations.

**Parameters**
None.

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- **Backup**: Uses the `mysql` class to create a database backup.
- **Restore**: Uses the `mysql` class to restore a database from backup.
- **Maintain**: Repairs and optimizes all tables.

**Usage Context**
Used for database maintenance and recovery.

---

### `config` / `_config`

**Purpose**
Configures database connection settings.

**Parameters**

| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param1`  | `string`  | MySQL host.                          |
| `$ifc_param2`  | `string`  | Database name.                       |
| `$ifc_param3`  | `string`  | MySQL username.                      |
| `$ifc_param4`  | `string`  | MySQL password.                      |

**Return Values**
- Updates `$ifc_response` with `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**
- Uses the `system` class to save connection settings.
- Reinitializes the `mysql` class with new settings.

**Usage Context**
Used for configuring database access.

---

## Main Display Logic

**Purpose**
Renders the main database interface, displaying tables, fields, and indexes.

**Inner Mechanisms**
- Caches the selected object for persistence.
- Displays a hierarchical view of database objects (tables, fields, indexes).
- Provides context-sensitive menus for operations.
- Shows detailed information about the selected object.

**Usage Context**
Default view when accessing the database interface.

---

## Helper Functions

### `bitstring($value, $length)`

**Purpose**
Converts a bit field value to a binary string representation.

**Parameters**

| Name     | Type     | Description                          |
|----------|----------|--------------------------------------|
| `$value` | `string` | Bit field value.                     |
| `$length`| `int`    | Length of the bit field.             |

**Return Values**
- `string`: Binary string representation of the bit field.

**Usage Context**
Used for displaying and editing `bit` fields.

---

### `format_bytesize($bytes)`

**Purpose**
Formats a byte size into a human-readable string (e.g., `1.2 KB`).

**Parameters**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$bytes`| `int`    | Size in bytes.                       |

**Return Values**
- `string`: Formatted size string.

**Usage Context**
Used for displaying file and table sizes.

---

### `strabridge($string, $length)`

**Purpose**
Truncates a string and adds an ellipsis if it exceeds the specified length.

**Parameters**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$string` | `string` | Input string.                        |
| `$length` | `int`    | Maximum length.                      |

**Return Values**
- `string`: Truncated string with ellipsis if necessary.

**Usage Context**
Used for displaying long strings in limited space.

---

### `yesno($value)`

**Purpose**
Converts a boolean value to a localized "Yes" or "No" string.

**Parameters**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$value`| `bool`   | Boolean value.                       |

**Return Values**
- `string`: "Yes" or "No" based on the input.

**Usage Context**
Used for displaying boolean values in the UI.


<!-- HASH:d2f89bc3fd65e1124ac2256890ec829f -->
