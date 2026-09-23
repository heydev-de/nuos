# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.filemanager.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Manager Interface

The `ifc.filemanager.inc` file is the core interface controller for the PWNC file manager module. It handles all file and directory operations including browsing, creating, editing, copying, moving, deleting, compressing, and transferring files. The interface provides a dual-pane view with a directory tree on the left and file listing on the right, along with detailed file information and permission management.

### Key Components

| Component | Description |
|-----------|-------------|
| `ifc` class | Interface controller for rendering forms and handling user interactions |
| `flexview` | Directory tree navigation component |
| `plist` | Persistent list for tracking recently accessed files |
| `filemanager` library | Core file operations (copy, move, delete, zip, unzip) |

### Initialization

The interface begins by loading required libraries (`filemanager`, `flexview`, `plist`) and setting up permissions. It retrieves the currently selected object from cache, sanitizes it to ensure it's within the CMS root path, and initializes variables for file listing and copy/move operations.

### Message Handling

The main logic is driven by a switch statement on `CMS_IFC_MESSAGE`, which determines the current operation:

#### select
Sets the selected object and updates the file list.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | Path to select |

#### save
Renames/moves a file or directory with optional permission changes.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | New name/path |
| `$ifc_param2-10` | mixed | Permission flags (read/write/execute for owner/group/all) |

#### upload
Handles file uploads to the current directory.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | array | Uploaded file data |
| `$ifc_file1_name` | array | Original filenames |

#### mkdir
Creates a new directory through a two-step process (form display then creation).

#### mkfile
Creates a new file with optional content through a two-step process.

#### edit
Provides a code editor for text-based files with syntax highlighting based on MIME type.

#### compress
Creates a ZIP archive of selected files.

#### decompress
Extracts a ZIP archive.

#### copy/cut
Initiates copy or move operations by setting the target directory.

#### copy_insert/cut_insert
Completes copy or move operations to the selected target.

#### download
Triggers a file download.

#### delete
Deletes selected files/directories.

#### transfer
Downloads a file from a remote URL to the server.

### Main Display

After processing messages, the interface renders:

1. **Folder Selection Popover** - Tree view of directories
2. **Shortcuts Popover** - Quick access to common directories
3. **Recently Edited Popover** - List of recently modified files
4. **Recently Selected Popover** - List of recently accessed paths
5. **Main File Table** - Detailed listing with sorting capabilities
6. **Copy/Move Target** - Secondary pane for copy/move operations
7. **Object Data Panel** - File information, permissions, and upload form

### Usage Example

```php
// Navigate to a directory
ifc_post('select', '/var/www/html/images/');

// Create a new file
ifc_post('mkfile');
// Then in the form: set name and content, submit as _mkfile

// Edit a PHP file
ifc_post('edit');
// Edit content in the code editor, save with command_save

// Copy files
ifc_post('copy');
// Select target directory in the right pane
ifc_post('copy_insert', '/var/www/html/backup/');
```

### JavaScript Functions

The interface includes client-side functions for enhanced interaction:

- `s(index)` - Select a file from the list
- `o(value)` - Change sort order

These functions work with the `ifc_post()` mechanism to communicate with the server and update the interface dynamically.


<!-- HASH:293b9c744cbd97d4403d7726af827073 -->
