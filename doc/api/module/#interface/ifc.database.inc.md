# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.database.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.database.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC Database Interface Module

## Overview

The file `module/#interface/ifc.database.inc` serves as the primary database management interface within the PWNC Web Platform. It provides a comprehensive web-based tool for managing MySQL databases, tables, fields, indexes, and data through a unified interface. This module handles everything from basic table browsing to advanced operations like SQL console execution, table creation/alteration, field management, data import/export, and database maintenance.

The interface operates through a message-driven architecture where different actions are triggered via the `CMS_IFC_MESSAGE` constant. It leverages the platform's core utilities including the `mysql` class for database operations, `cms_cache` for state persistence, and the `ifc` class for rendering interactive forms and controls.

## Core Components

### Database Connection

| Variable | Type | Description |
|----------|------|-------------|
| `$mysql` | `mysql` | Main database connection instance |
| `$object` | `string` | Currently selected database object (table.field.index) |
| `$list` | `array` | Selected objects for batch operations |

### Message Handling

The interface uses a switch statement on `CMS_IFC_MESSAGE` to determine the current operation. Key messages include:

- `select` - Select a database object
- `sql_console` - Execute raw SQL queries
- `edit_table` - Browse and edit table data
- `alter_table` / `create_table` - Modify or create tables
- `add_field` / `change_field` - Manage table fields
- `create_index` - Manage table indexes
- `export_definition` / `export_table` - Export schema or data
- `import_table` - Import data from files
- `delete` - Remove tables, fields, or indexes
- `backup` / `restore` - Database backup and recovery
- `maintain` - Table optimization and repair
- `config` - Database connection configuration

## Detailed Function Documentation

### SQL Console

#### Purpose
Provides an interactive interface for executing arbitrary SQL queries against the database.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | SQL query to execute |
| `$execute` | `boolean` | Flag to execute the query |

#### Mechanism
1. Retrieves last executed query from cache
2. Displays textarea for SQL input
3. On execution, caches the query and runs it via `log_report()`
4. Shows any MySQL errors that occur

#### Usage Example
```php
// User navigates to database/sql_console
// Enters: SELECT * FROM users WHERE active = 1
// Clicks execute button
// Query is cached and executed
// Results displayed in table format
```

### Table Editor

#### Purpose
Provides a spreadsheet-like interface for viewing, adding, editing, and deleting records in a database table.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Table name to edit |
| `$row` | `mixed` | Selected row identifier |
| `$field` | `string` | Field being edited |
| `$value` | `mixed` | New value for field |
| `$list` | `array` | Selected rows for batch operations |
| `$filter_field` | `string` | Column to filter on |
| `$filter_option` | `string` | Filter operator (LIKE, =, etc.) |
| `$filter_value` | `string` | Filter value |
| `$limit` | `int` | Records per page |
| `$offset` | `int` | Current page offset |
| `$order` | `string` | Sort order |

#### Mechanism
1. Retrieves table structure via `SHOW COLUMNS`
2. Determines primary key or uses row numbering
3. Handles CRUD operations (add, update, delete)
4. Implements filtering, sorting, and pagination
5. Renders interactive table with inline editing capabilities

#### Usage Example
```php
// User selects "users" table
// Interface shows all records with pagination
// User clicks on a cell to edit
// Changes are saved via AJAX-like form submission
// Filter applied: WHERE name LIKE '%john%'
```

### Table Creation/Alteration

#### Purpose
Manages creation of new tables and modification of existing table properties.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Table name |
| `$ifc_param2` | `string` | Storage engine |
| `$ifc_param3` | `string` | Character set and collation |
| `$ifc_param4` | `string` | Table comment |
| `$ifc_param5` | `string` | Definition file path |

#### Mechanism
1. Retrieves available character sets, collations, and storage engines
2. For alteration: loads current table status
3. Builds ALTER TABLE or CREATE TABLE query
4. Executes query and reports success/failure

#### Usage Example
```php
// Create new table "products"
// Select engine: InnoDB
// Choose charset: utf8mb4
// Enter comment: "Product catalog"
// Execute CREATE TABLE statement
```

### Field Management

#### Purpose
Handles creation and modification of table fields with full type support.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Field type |
| `$ifc_param2` | `string` | Field name |
| `$ifc_param3` | `boolean` | Allow NULL values |
| `$ifc_param4` | `string` | Default value |
| `$ifc_param5-19` | `mixed` | Type-specific parameters |

#### Mechanism
1. For changes: analyzes existing field structure
2. Builds ALTER TABLE ADD/CHANGE statement
3. Handles all MySQL data types with appropriate parameters
4. Supports character sets, collations, and attributes

#### Usage Example
```php
// Add field to "users" table
// Type: VARCHAR(255)
// Name: email
// Allow NULL: No
// Default: empty string
// Execute: ALTER TABLE users ADD email VARCHAR(255) NOT NULL DEFAULT ''
```

### Index Management

#### Purpose
Creates and manages database indexes for performance optimization.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Index type (INDEX, UNIQUE, etc.) |
| `$ifc_param2` | `string` | Index name |
| `$ifc_param3` | `string` | Index method (BTREE, HASH) |
| `$list` | `array` | Fields to include in index |
| `$length` | `array` | Field length specifications |

#### Mechanism
1. Retrieves available table columns
2. Allows selection of multiple fields
3. Builds CREATE INDEX statement
4. Supports prefix lengths for indexed fields

#### Usage Example
```php
// Create index on "users" table
// Type: INDEX
// Name: idx_email
// Method: BTREE
// Fields: email
// Execute: CREATE INDEX idx_email ON users USING BTREE (email)
```

### Data Export

#### Purpose
Exports database schema definitions and table data in various formats.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Export filename |
| `$ifc_param2` | `int` | Export format (0=HTML, 1=XLS, 2=CSV) |
| `$ifc_param3` | `boolean` | Include field names |
| `$ifc_param4` | `string` | CSV separator |
| `$ifc_param5` | `string` | CSV delimiter |

#### Mechanism
1. For schema: uses `mysql->export_sql()`
2. For data: uses format-specific export methods
3. Generates downloadable file
4. Provides download link via JavaScript

#### Usage Example
```php
// Export "users" table data
// Format: CSV
// Include headers: Yes
// Separator: comma
// Delimiter: double quote
// File generated: users_2023-01-15.csv
```

### Data Import

#### Purpose
Imports data from CSV/TXT files into database tables.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Source file |
| `$ifc_param2` | `string` | Field separator |
| `$ifc_param3` | `string` | Field delimiter |
| `$ifc_param4` | `boolean` | Ignore first row |
| `$ifc_param5` | `boolean` | Ignore existing records |
| `$mapping` | `array` | Field-to-column mapping |

#### Mechanism
1. Scans data directory for importable files
2. Parses first row to determine structure
3. Allows field mapping configuration
4. Executes import with specified options
5. For new tables: creates table automatically

#### Usage Example
```php
// Import "customers.csv"
// Separator: comma
// Delimiter: double quote
// Ignore first row: Yes (contains headers)
// Map columns: name->customer_name, email->email_address
// Execute import into "customers" table
```

### Database Maintenance

#### Purpose
Performs optimization and repair operations on database tables.

#### Mechanism
1. Retrieves list of all tables
2. Executes REPAIR TABLE on each
3. Executes OPTIMIZE TABLE on each
4. Reports any errors encountered

#### Usage Example
```php
// User clicks "Repair/Optimize" button
// System processes all tables:
// REPAIR TABLE users, products, orders...
// OPTIMIZE TABLE users, products, orders...
// Reports completion status
```

### Configuration Management

#### Purpose
Manages database connection parameters.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | MySQL host |
| `$ifc_param2` | `string` | Database name |
| `$ifc_param3` | `string` | Username |
| `$ifc_param4` | `string` | Password |

#### Mechanism
1. Reads current values from system configuration
2. Allows modification through form
3. Saves to persistent storage
4. Attempts new connection to verify

#### Usage Example
```php
// View current config:
// Host: localhost
// Database: pwnc_cms
// User: cms_user
// Password: ********
// Modify and save new credentials
// System reconnects with new settings
```

## JavaScript Integration

The interface includes several JavaScript functions for enhanced interactivity:

- `c()` - Clears selection and posts form
- `o(value)` - Sets sort order and posts
- `p(number)` - Navigates to page number
- `s(row, field)` - Selects cell for editing
- `ifc_list_activate/invert/deactivate()` - Batch selection controls

## Security Considerations

1. All SQL queries use `sqlesc()` for proper escaping
2. Object names are validated before use in queries
3. File operations are restricted to designated data directories
4. CSRF protection via `cms_param()` state management
5. Input validation for all user-provided parameters

## Error Handling

The interface consistently follows this pattern:
1. Execute database operation
2. Check for errors using `mysql_error()`
3. Set `$ifc_response` to `CMS_MSG_ERROR` with escaped error message
4. Display error in interface via `ifc` component


<!-- HASH:0071fdf267ae6f4ab9c63b646a3e66ae -->
