# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.note.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.note.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Note Interface

This file handles the desktop note editing interface within the PWNC Web Platform. It provides functionality for viewing, editing, and saving text notes through the desktop interface system.

### Overview

The `desktop.note.inc` file is part of the desktop interface module and manages note objects within the PWNC desktop environment. It allows users to edit note content through a textarea interface and save changes back to the system.

### Message Handling

The file implements a switch statement that processes different message types for the desktop interface:

#### `save` Case

When the `CMS_IFC_MESSAGE` is set to `"save"`, this case handles the saving of note content.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `CMS_IFC_MESSAGE` | string | The message type to process |
| `$object` | string | The object identifier for the note |
| `$ifc_param1` | string | The new text content to save |

**Mechanism:**
1. Sets the "text" property of the specified object using `$desktop->object_set()`
2. Attempts to save the desktop state with `$desktop->save()`
3. Sets the response to either `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure)

**Usage Example:**
```php
// When a user submits the note form, CMS_IFC_MESSAGE is set to "save"
// and $ifc_param1 contains the updated note text
// The system automatically saves the note and returns appropriate feedback
```

### Main Display

The main display section creates an interface for editing notes:

#### Interface Creation

Creates a new interface instance for the note editor:

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_response` | string | The response message from processing |
| `$ifc_page` | string | The current page identifier |
| Command array | array | Available commands with labels and actions |
| `$param` | mixed | Additional parameters |
| `NULL` | null | Reserved parameter |
| `CMS_L_DESKTOP_NOTE_001` | string | Language constant for the interface title |

**Mechanism:**
1. Instantiates a new `ifc` object with response, page, and command configuration
2. The command array maps the save command to its handler
3. Uses language constants for internationalization

#### Textarea Setup

Configures the textarea for note editing:

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| Object name | string | Retrieved via `$desktop->object_get($object, "name")` |
| Field config | string | `"textarea 80x20 102400 f"` - dimensions and constraints |
| Default value | string | Retrieved via `$desktop->object_get($object, "text")` |

**Field Configuration Breakdown:**
- `textarea`: Input type
- `80x20`: Width and height dimensions
- `102400`: Maximum character limit (100KB)
- `f`: Field flag (likely indicating required or formatted)

**Mechanism:**
1. Retrieves the note's display name from the desktop object
2. Sets up a textarea with specified dimensions and constraints
3. Populates the textarea with existing note content
4. Closes the interface with `$ifc->close()`

**Usage Example:**
```php
// The interface displays a note editor with:
// - A save button that triggers the "save" message handler
// - A textarea showing the current note content
// - Character limit enforcement (102400 characters)
// - Proper internationalization through language constants
```

### Integration Points

This file integrates with several core PWNC systems:
- **Desktop System**: Uses `$desktop` object for object management and persistence
- **Interface System**: Leverages the `ifc` class for form generation
- **Messaging System**: Responds to `CMS_IFC_MESSAGE` for action routing
- **Internationalization**: Uses `CMS_L_*` constants for localized text
- **Command System**: Maps UI actions to backend handlers via command arrays


<!-- HASH:67043015d920e2b73976bd6f80027a88 -->
