# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.note.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Note Module (`desktop.note.inc`)

This file implements the interface for displaying and saving desktop notes in the NUOS web platform. Desktop notes are user-created text objects that persist across sessions, providing a simple way to store and retrieve textual information directly from the desktop interface.

---

### Overview

The module handles two primary operations:
1. **Message Handling** – Processes incoming interface commands (e.g., saving a note).
2. **Main Display** – Renders the note editing interface with appropriate controls and data.

The module relies on the `$desktop` object (assumed to be pre-instantiated) for data persistence and the `ifc` class for interface construction.

---

### Message Handling

#### Context: `CMS_IFC_MESSAGE` Switch

The module uses a `switch` statement to respond to interface commands. Currently, only the `"save"` command is implemented.

#### Case: `"save"`

```php
case "save":
    $desktop->object_set($object, "text", $ifc_param1);
    $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

| Parameter | Type   | Description |
|-----------|--------|-------------|
| `$object` | string | Identifier of the desktop note object being modified. Typically passed via global or request context. |
| `$ifc_param1` | string | The new text content of the note, submitted via the interface. |

**Purpose**:
Updates the text content of a specified desktop note and attempts to save it. Sets the interface response to indicate success (`CMS_MSG_DONE`) or failure (`CMS_MSG_ERROR`).

**Return Value**:
- None (sets `$ifc_response` for use in the interface layer).

**Inner Mechanism**:
- Calls `$desktop->object_set()` to update the `"text"` property of the note identified by `$object`.
- Calls `$desktop->save()` to persist changes. The result determines the response code.

**Usage Context**:
Triggered when the user submits the note editing form. The `"save"` command is linked to the form via the `ifc` control definition (see Main Display).

---

### Main Display

#### Interface Construction

```php
$ifc = new ifc($ifc_response,
    $ifc_page,
    [CMS_L_COMMAND_SAVE . "|desktop/command_save" => "save"],
    $param,
    NULL,
    CMS_L_DESKTOP_NOTE_001);
```

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_response` | string | Response code from message handling (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`). Used to display feedback. |
| `$ifc_page` | string | Current page identifier. Used for navigation and state management. |
| Control Array | array | Associative array defining interface controls. Key: Display label + URL; Value: Command name (`"save"`). |
| `$param` | array | Additional parameters (e.g., URL or state parameters) passed to the interface. |
| `NULL` | — | Reserved for secondary controls (not used here). |
| `CMS_L_DESKTOP_NOTE_001` | string | Localized title for the interface (e.g., "Edit Note"). |

**Purpose**:
Initializes the interface context (`ifc` object) for the note editor. Defines a single control: a save button labeled with `CMS_L_COMMAND_SAVE` that triggers the `"save"` command.

**Inner Mechanism**:
- The `ifc` class is responsible for rendering forms, controls, and feedback messages.
- The control array maps user-visible actions to backend commands.

---

#### Setting Form Fields

```php
$ifc->set($desktop->object_get($object, "name"),
    "textarea 80x20 2048 f",
    $desktop->object_get($object, "text"));
```

| Parameter | Type   | Description |
|-----------|--------|-------------|
| `$desktop->object_get($object, "name")` | string | The name/title of the note. Used as the field label. |
| `"textarea 80x20 2048 f"` | string | Field definition: textarea with 80 columns, 20 rows, max length 2048, and `f` (likely "focus" or "full-width"). |
| `$desktop->object_get($object, "text")` | string | Current content of the note. Populates the textarea. |

**Purpose**:
Defines the main input field for the note. The textarea allows users to view and edit the note's content.

**Inner Mechanism**:
- `object_get($object, "name")` retrieves the note's title.
- `object_get($object, "text")` retrieves the current content.
- The `set()` method of `ifc` binds these values to a form field.

---

#### Finalizing the Interface

```php
$ifc->close();
```

**Purpose**:
Renders the interface to the output buffer and terminates the interface context. This is the final step in interface generation.

**Inner Mechanism**:
- Outputs all form elements, controls, and feedback messages.
- Closes any open HTML tags or containers.

---

### Usage Scenarios

#### Editing a Note
1. User navigates to the desktop note interface (e.g., via a desktop icon or menu).
2. The module loads the note's current name and text.
3. The `ifc` object renders a form with a textarea and a save button.
4. User edits the text and clicks save.
5. The `"save"` command is triggered, updating and persisting the note.
6. Feedback (success or error) is displayed.

#### Integration with Desktop
- Notes are typically accessed via the desktop environment.
- The `$object` identifier is passed via URL or session state (e.g., `?object=note123`).
- The module assumes `$desktop` is available in the global or object context.

---

### Dependencies

| Dependency | Role |
|-----------|------|
| `$desktop` | Core object for managing desktop objects (including notes). Must support `object_get()`, `object_set()`, and `save()`. |
| `ifc` class | Interface construction utility. Must support `new ifc()`, `set()`, and `close()`. |
| Localization constants (`CMS_L_*`) | Used for UI labels and titles. |
| `CMS_IFC_MESSAGE`, `CMS_MSG_DONE`, `CMS_MSG_ERROR` | System constants for command and response handling. |

---

### Notes

- This module follows NUOS's zero-dependency, high-performance philosophy: no external libraries are used.
- All escaping and encoding should be handled by the calling context or the `ifc` class.
- The module is designed for integration into the desktop environment, not as a standalone page.


<!-- HASH:19c93befdb7e8eeb82fe8a184b407a09 -->
