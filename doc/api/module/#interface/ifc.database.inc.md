# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.database.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.database.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Database Interface Module (`ifc.database.inc`)

This file provides a comprehensive **database management interface** for the PWNC Web Platform. It allows developers and administrators to interact with MySQL databases through a web-based UI, supporting operations such as:

- **Table creation, modification, and deletion**
- **Field (column) management**
- **Index management**
- **Data editing and filtering**
- **SQL console for direct query execution**
- **Data import/export (CSV, XLS, HTML)**
- **Database backup and restoration**
- **Configuration of MySQL connection settings**

The interface is **context-aware**, dynamically adjusting available actions based on the selected object (table, field, or index). It uses **caching** to preserve user preferences (e.g., filters, sorting) and **CSRF protection** via the platform's parameter management system.

---

## Core Components

### 1. **Initialization and Permissions**
```php
ifc_permission(["" => CMS_L_ACCESS]);
$mysql = new mysql();
cms_cache_init($object, "database." . CMS_USER . ".object");
```
- **Purpose**: Initializes the database connection and checks user permissions.
- **Mechanism**:
  - `ifc_permission()` ensures the user has access to the database module.
  - `mysql()` establishes a connection to the MySQL server.
  - `cms_cache_init()` loads the last selected database object from cache.

---

### 2. **Message Handling**
The interface processes actions via `CMS_IFC_MESSAGE`, which determines the operation to perform (e.g., `edit_table`, `create_table`). Each case in the `switch` statement corresponds to a specific database operation.

---

## Key Functions and Workflows

### ### `select`
**Purpose**: Selects a database object (table, field, or index) for further operations.
**Parameters**:
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Object identifier (e.g., `table.field`). |

**Usage Example**:
```javascript
// Select the "users" table via JavaScript
ifc_post('select', 'users');
```

---

### ### `sql_console`
**Purpose**: Provides a SQL console for executing raw queries.
**Parameters**:
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | SQL query to execute.                |
| `$execute`    | bool   | Flag to execute the query.           |

**Mechanism**:
- Retrieves the last executed query from cache.
- Displays a textarea for query input and an execute button.
- On execution, logs the query and displays results or errors.

**Usage Example**:
```sql
-- Example query to list all users
SELECT * FROM users;
```

---

### ### `edit_table`
**Purpose**: Edits table data with filtering, sorting, and pagination.
**Parameters**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$object`       | string   | Table name.                          |
| `$row`          | mixed    | Selected row identifier.             |
| `$field`        | string   | Field being edited.                  |
| `$filter_field` | string   | Field to filter by.                  |
| `$filter_option`| string   | Filter operator (e.g., `LIKE`, `=`). |
| `$filter_value` | string   | Filter value.                        |
| `$limit`        | int      | Rows per page.                       |
| `$length`       | int      | Characters per field.                |
| `$offset`       | int      | Pagination offset.                   |
| `$order`        | string   | Sorting field and direction.         |

**Mechanism**:
- Retrieves table structure (columns, types, primary keys).
- Applies filters, sorting, and pagination to the query.
- Supports inline editing of field values.
- Caches filter and pagination settings for the user.

**Usage Example**:
```javascript
// Edit the "email" field of row 5 in the "users" table
ifc_post('edit_table', {
    object: 'users',
    row: 5,
    field: 'email'
});
```

---

### ### `alter_table` / `create_table`
**Purpose**: Modifies or creates a table with customizable settings (engine, collation, comment).
**Parameters**:
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | Table name.                          |
| `$ifc_param2` | string | Storage engine (e.g., `InnoDB`).     |
| `$ifc_param3` | string | Collation (e.g., `utf8mb4_general_ci`). |
| `$ifc_param4` | string | Table comment.                       |
| `$ifc_param5` | string | SQL definition file (for creation).  |

**Mechanism**:
- For `alter_table`, retrieves current table settings and allows modification.
- For `create_table`, supports creation from scratch or via an SQL definition file.
- Validates inputs and executes the `ALTER TABLE` or `CREATE TABLE` query.

**Usage Example**:
```sql
-- Example SQL definition file for table creation
CREATE TABLE users (
    id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

---

### ### `add_field` / `change_field`
**Purpose**: Adds or modifies a field in a table.
**Parameters**:
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$ifc_param1` | string   | Field type (e.g., `VARCHAR`, `INT`). |
| `$ifc_param2` | string   | Field name.                          |
| `$ifc_param3` | bool     | `NOT NULL` flag.                     |
| `$ifc_param4` | string   | Default value.                       |
| `$ifc_param5` | int      | Length (for `BIT`, `CHAR`, etc.).    |
| `$ifc_param6` | int      | Length (for `VARCHAR`).              |
| `$ifc_param7` | string   | Collation (for text fields).         |
| `$ifc_param8` | int      | Length (for integers).               |
| `$ifc_param9` | string   | Attributes (e.g., `UNSIGNED`).       |
| `$ifc_param10`| bool     | `AUTO_INCREMENT` flag.               |
| `$ifc_param11`| string   | Collation (for `TEXT` fields).       |
| `$ifc_param12`| int      | Length (for decimals).               |
| `$ifc_param13`| int      | Decimals (for decimals).             |
| `$ifc_param14`| string   | Attributes (for decimals).           |
| `$ifc_param15`| int      | Length (for binary fields).          |
| `$ifc_param16`| int      | Length (for `YEAR`).                 |
| `$ifc_param17`| string   | Values (for `ENUM`/`SET`).           |
| `$ifc_param18`| string   | Collation (for `ENUM`/`SET`).        |
| `$ifc_param19`| string   | Field comment.                       |

**Mechanism**:
- For `change_field`, retrieves the current field definition and populates the form.
- Validates inputs and constructs the `ALTER TABLE` query.
- Supports all MySQL data types and attributes.

**Usage Example**:
```javascript
// Add a new "age" field to the "users" table
ifc_post('add_field', {
    object: 'users',
    ifc_param1: 'INT',
    ifc_param2: 'age',
    ifc_param3: true, // NOT NULL
    ifc_param8: 3     // Length
});
```

---

### ### `create_index`
**Purpose**: Creates an index on one or more fields.
**Parameters**:
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$ifc_param1` | string   | Index type (`INDEX`, `UNIQUE`, `PRIMARY KEY`, `FULLTEXT`). |
| `$ifc_param2` | string   | Index name.                          |
| `$ifc_param3` | string   | Index format (`BTREE`, `HASH`).      |
| `$list`       | array    | Fields to include in the index.      |
| `$length`     | array    | Field lengths (for partial indexes). |

**Mechanism**:
- Displays a list of fields in the table for selection.
- Constructs and executes the `ALTER TABLE ... ADD INDEX` query.

**Usage Example**:
```javascript
// Create a unique index on the "email" field
ifc_post('create_index', {
    object: 'users.email',
    ifc_param1: 'UNIQUE',
    list: ['email']
});
```

---

### ### `export_definition` / `export_table`
**Purpose**: Exports table definitions or data in various formats (SQL, HTML, XLS, CSV).
**Parameters**:
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$ifc_param1` | string   | File name.                           |
| `$ifc_param2` | int      | Export type (`0`=HTML, `1`=XLS, `2`=CSV). |
| `$ifc_param3` | bool     | Include field names in export.       |
| `$ifc_param4` | string   | CSV separator.                       |
| `$ifc_param5` | string   | CSV delimiter.                       |

**Mechanism**:
- For `export_definition`, generates an SQL file with the `CREATE TABLE` statement.
- For `export_table`, exports data in the selected format and provides a download link.

**Usage Example**:
```javascript
// Export the "users" table as CSV
ifc_post('export_table', {
    object: 'users',
    ifc_param1: 'users_export',
    ifc_param2: 2, // CSV
    ifc_param4: ',', // Separator
    ifc_param5: '"'  // Delimiter
});
```

---

### ### `import_table`
**Purpose**: Imports data from a CSV file into a table.
**Parameters**:
| Name               | Type     | Description                          |
|--------------------|----------|--------------------------------------|
| `$ifc_param1`       | string   | File path.                           |
| `$ifc_param2`       | string   | CSV separator.                       |
| `$ifc_param3`       | string   | CSV delimiter.                       |
| `$ifc_param4`       | bool     | Ignore first row (field names).      |
| `$ifc_param5`       | bool     | Ignore existing records.             |
| `$mapping`          | array    | Field mapping (column → field).      |

**Mechanism**:
- For existing tables, maps CSV columns to table fields.
- For new tables, creates a table with `BLOB` fields and imports data.
- Supports ignoring the first row (for field names) and existing records.

**Usage Example**:
```javascript
// Import data from "users.csv" into the "users" table
ifc_post('import_table', {
    object: 'users',
    ifc_param1: 'users.csv',
    ifc_param2: ',', // Separator
    ifc_param3: '"', // Delimiter
    ifc_param4: true // Ignore first row
});
```

---

### ### `delete`
**Purpose**: Deletes selected objects (tables, fields, or indexes).
**Parameters**:
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `$list` | array  | List of objects to delete.           |

**Mechanism**:
- Splits each object identifier into table, field, and index components.
- Constructs and executes the appropriate `DROP` or `ALTER TABLE` query.

**Usage Example**:
```javascript
// Delete the "users" table
ifc_post('delete', {
    list: ['users']
});
```

---

### ### `backup` / `restore`
**Purpose**: Backs up or restores the entire database.
**Mechanism**:
- `backup()`: Dumps the database to a file.
- `restore()`: Restores the database from a backup file.

**Usage Example**:
```javascript
// Backup the database
ifc_post('backup');
```

---

### ### `maintain`
**Purpose**: Repairs and optimizes all tables in the database.
**Mechanism**:
- Executes `REPAIR TABLE` and `OPTIMIZE TABLE` for each table.

**Usage Example**:
```javascript
// Repair and optimize all tables
ifc_post('maintain');
```

---

### ### `config`
**Purpose**: Configures MySQL connection settings.
**Parameters**:
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | MySQL host.                          |
| `$ifc_param2` | string | Database name.                       |
| `$ifc_param3` | string | MySQL username.                      |
| `$ifc_param4` | string | MySQL password.                      |

**Mechanism**:
- Retrieves current settings from the `system` class.
- Updates settings and reinitializes the MySQL connection.

**Usage Example**:
```javascript
// Update MySQL configuration
ifc_post('config', {
    ifc_param1: 'localhost',
    ifc_param2: 'pwnc_db',
    ifc_param3: 'admin',
    ifc_param4: 'password123'
});
```

---

## Helper Functions

### `bitstring($value, $length)`
**Purpose**: Converts a bit value to a binary string.
**Parameters**:
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value` | string | Bit value (e.g., `1010`).            |
| `$length`| int    | Length of the bit field.             |

**Return Value**: Binary string representation.

**Usage Example**:
```php
echo bitstring('1010', 4); // Output: "1010"
```

---

### `pagination($link, $page, $pages, $next, $class)`
**Purpose**: Generates pagination controls.
**Parameters**:
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$link`  | string | JavaScript function for page links.  |
| `$page`  | int    | Current page.                        |
| `$pages` | int    | Total pages.                         |
| `$next`  | string | Label for the "next" button.         |
| `$class` | string | CSS class for the pagination div.    |

**Usage Example**:
```php
pagination("javascript:p(%page%);", 2, 10, "Next", "pagination");
```

---

## JavaScript Functions

### `c()`
**Purpose**: Cancels field editing and resets the form.
**Usage**:
```javascript
c(); // Resets the row and field selection
```

---

### `o(value)`
**Purpose**: Sets the sorting field and direction.
**Parameters**:
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | string | Field name and direction (e.g., `` `name` DESC ``). |

**Usage**:
```javascript
o('`name` DESC'); // Sort by "name" in descending order
```

---

### `p(number)`
**Purpose**: Navigates to a specific page.
**Parameters**:
| Name    | Type | Description                          |
|---------|------|--------------------------------------|
| `number`| int  | Page number.                         |

**Usage**:
```javascript
p(3); // Go to page 3
```

---

### `s(row, field)`
**Purpose**: Selects a row and field for editing.
**Parameters**:
| Name    | Type | Description                          |
|---------|------|--------------------------------------|
| `row`   | int  | Row index.                           |
| `field` | int  | Field index.                         |

**Usage**:
```javascript
s(1, 2); // Select row 1, field 2 for editing
```

---

## Constants and Labels
The interface uses language constants (e.g., `CMS_L_IFC_DATABASE_001`) for labels and messages. These are defined in the platform's language files.

---

## Best Practices
1. **Permissions**: Ensure users have the `CMS_L_ACCESS` permission before accessing the module.
2. **Caching**: Leverage caching (`cms_cache`) to preserve user preferences (e.g., filters, sorting).
3. **Escaping**: Always use `sqlesc()` for SQL values and `x()` for HTML output to prevent injection.
4. **Error Handling**: Check `mysql_error()` after queries and display errors to the user.
5. **Backup**: Use the `backup` function before performing destructive operations (e.g., `DELETE`, `DROP`).

---

## Example Workflow: Creating a Table
1. **Navigate to the Database Module**:
   ```javascript
   load_page(cms_url({ ifc_page: CMS_IFC_PAGE }));
   ```
2. **Create a Table**:
   ```javascript
   ifc_post('create_table', {
       ifc_param1: 'products', // Table name
       ifc_param2: 'InnoDB',  // Engine
       ifc_param3: 'utf8mb4_general_ci', // Collation
       ifc_param4: 'Product catalog' // Comment
   });
   ```
3. **Add Fields**:
   ```javascript
   ifc_post('add_field', {
       object: 'products',
       ifc_param1: 'INT', // Type
       ifc_param2: 'id',  // Name
       ifc_param3: true,  // NOT NULL
       ifc_param10: true  // AUTO_INCREMENT
   });
   ifc_post('add_field', {
       object: 'products',
       ifc_param1: 'VARCHAR',
       ifc_param2: 'name',
       ifc_param3: true,
       ifc_param6: 255    // Length
   });
   ```
4. **Create an Index**:
   ```javascript
   ifc_post('create_index', {
       object: 'products.id',
       ifc_param1: 'PRIMARY KEY',
       list: ['id']
   });
   ```
5. **Import Data**:
   ```javascript
   ifc_post('import_table', {
       object: 'products',
       ifc_param1: 'products.csv',
       ifc_param2: ',',
       ifc_param3: '"',
       ifc_param4: true
   });
   ```


<!-- HASH:7181dc3920a14fe8625d2672ec6a7c0d -->
