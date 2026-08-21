# PWNC Instructions

## What it is

PWNC is a PHP-based website runtime and management system bundling applications, tools and libraries with no external dependencies — a web endpoint, IDE, CMS and communication hub for AI agents and human users.

## How it works

PWNC uses **HATEOAS (Hypermedia as the Engine of Application State)** as its primary interaction model.

PWNC provides server-generated web interfaces, primarily HTML forms with JavaScript augmentations, and displays available options depending on context. The interface presented by PWNC is authoritative for the current request, state and accessor.

When working with PWNC, always reason in terms of:

* **What can I see?**
* **What am I allowed to do?**
* **How can I discover what I can do?**
* **How do I execute the required action?**

Do not assume that an operation exists merely because PWNC is capable of performing it. Use the exposed interface to identify the action required to fulfill the user's request. Do not execute actions merely because they are available or exposed by the interface.

Work through HTTP:

1. Use `GET` to retrieve and inspect an endpoint and its currently available interface.
2. Inspect the returned HTML, especially links and `form` elements.
3. Use the available forms, fields and links to determine what actions are currently possible.
4. Submit forms using their declared `method`, `action` and `enctype`.
5. Inspect the resulting response again, because the available actions and state may have changed.
6. Continue following the interface until the requested task is complete.

PWNC uses UTF-8 throughout.

### HTTP and JavaScript

PWNC has extensive JavaScript functionality. Most of it is not relevant to an AI agent when interacting with PWNC and should not be analyzed unnecessarily.

The primary interaction model is the server-generated HTTP interface: `GET`, `POST`, HTML forms, links and HATEOAS.

Inspect JavaScript selectively when it is relevant to understanding an available action. In particular, JavaScript may:

* manipulate form fields or other interface elements,
* provide browser-side convenience functionality,
* derive or transform values before submission, or
* encapsulate HTTP actions, sometimes to reduce the amount of data transferred.

Do not attempt to execute browser-side JavaScript. When a relevant action is implemented or encapsulated by JavaScript, analyze only the relevant code to determine the underlying HTTP interaction and perform that interaction directly.

Do not analyze unrelated JavaScript merely because it is present on the page. Prefer the HTML and exposed HTTP interface and inspect only the JavaScript necessary to understand an otherwise unclear interaction.

### HTTP form submission

`multipart/form-data` **MUST** be sent as raw bytes via binary-safe transport layers only. Multipart request bodies may contain binary data and must not be treated as UTF-8 text or passed through text-only transformations.

## MCP

Every active PWNC endpoint is an **MCP server endpoint**.

An endpoint such as:

`https://example.com/services.php`

is therefore itself an MCP endpoint. MCP clients may communicate with the endpoint using the MCP protocol.

The MCP interface provides a small number of general-purpose operations. The primary PWNC interaction model remains the HTTP interface described above.

PWNC functionality should therefore not be assumed to correspond to individual MCP tools. In general, operations available through a PWNC endpoint are discovered and executed through its normal `GET` and `POST` interfaces, following HATEOAS and the forms and actions exposed by the endpoint.

The MCP interface does not replace PWNC's web interface or introduce a separate representation of every PWNC operation. It provides an additional protocol through which an AI agent can interact with the PWNC environment.

## Frontend, Backend, Desktop

PWNC's architecture has three interrelated main areas defined by the modules `pwnc/module/content.php` (Frontend), `pwnc/module/interface.php` (Backend), and `pwnc/module/desktop.php` (Desktop).

### Frontend

Unified environment serving as public website and visual live compositing workspace.

### Backend

Content and asset management, data access, administration, configuration, analytics, etc.

### Desktop

Utilities for communication (email, messaging, contacts, object exchange), planning (calendar, notes), and links.

`domain.tld/pwnc` always opens the Desktop, which links to Frontend and Backend.

The Frontend is usually called via speaking paths, e.g.:

`https://example.com/services.php`

Every active endpoint supports MCP.

## Authentication

Your platform permissions define what PWNC offers you.

Send your 32-character hex API key with every request via the `Authorization` header:

`Authorization: Bearer <api_key>`

Omit the `Authorization` header only when intentionally accessing PWNC anonymously, for example to discover what an anonymous accessor is presented.

Never guess, reconstruct or disclose an API key.

Only capabilities exposed to the current accessor are available to that accessor. Do not assume permissions or capabilities that are not exposed by the interface.

## IMPORTANT

Unless explicitly asked to, **DO NOT edit any core files in `pwnc/`** — this may break the system irreversibly.

PWNC's architecture is highly interrelated, and the `pwnc/` directory is replaced on every update, so local modifications would be lost.

Prefer the built-in high-level forms and interfaces.

**Do not bypass PWNC's forms, permissions, validation or data-access mechanisms by modifying or directly manipulating core files.**

Use the interfaces exposed by PWNC instead.

## PWNC Text Format

PWNC does not use Markdown for text input.

Text input fields targeted by the `textcontrol` JS function allow a simple proprietary format where literal `[` and `]` delimit the formatted area.

### Text

`[+ <bold text>]`

`[/ <italic text>]`

`[_ <underlined text>]`

`[< <big text>]`

`[> <small text>]`

### Blocks

`[<- <left-aligned content>]`

`[<-> <centered content>]`

`[-> <right-aligned content>]`

### Images

Basic usage:

`[IMG <any encoded url, e.g. https://example.com/image.png>]`

or:

`[IMG image://<image_id>]`

Left-floating image:

`[<-IMG <url>]`

or:

`[<-IMG image://<image_id>]`

Right-floating image:

`[IMG-> <url>]`

or:

`[IMG-> image://<image_id>]`

Limiting dimensions:

`[IMG <url> <max width in px (number only) or * to skip> <max height in px (number only), optional>]`

Examples:

`[IMG https://example.com/image.png 250 150]`

`[IMG https://example.com/image.png 250]`

`[IMG https://example.com/image.png * 150]`

### Links

`[<any encoded url, e.g. https://example.com/page.php?key=value#anchor> <visible link content>]`

`[content://<content_id> <visible link content>]`

`[directory://<directory_id> <visible link content>]`

`[mailto:info@example.com <visible link content>]`

`content://<content_id>` and `directory://<directory_id>` support query strings and anchors as well.

### Tables

```text id="47299u"
[# row 1 / col 1 | row 1 / col 2 |
 | row 2 / col 1 | row 2 / col 2 ]
```

Formatting can be nested, e.g.:

```text id="v0q3mj"
[+ <bold text> [/ <bold italic text>]]
```

Placeholders extend formatting in most backend cases.

Keep formatting basic; use templates for extensive formatting.

See the Template Manual for details.

## Multilingual values

PWNC uses a simple string format to define multilingual values delimited by the ASCII unit separator (`0x1F`):

`<default/fallback value>[0x1F]<IETF language code>:<language-specific value>[0x1F]<IETF language code>:<language-specific value>` etc.

Replace `[0x1F]` with a raw byte.

If an input field allows multilingual input, its `name` attribute is prefixed with `l_`, e.g.:

`name="l_ifc_param1"`

This field is usually an editing UI element and can be omitted when sending form data. The combined value that is processed by PWNC is stored in a hidden field without the `l_` prefix, e.g.:

`name="ifc_param1"`

Conceptual example of the unencoded string structure used inside a multipart form field:

`default/fallback text[0x1F]en:English text[0x1F]de:Deutscher Text`

The fallback value is used when no specific value exists for the active language.

In multilingual mode, one enabled language is the configured default, so align the fallback with it.

When values match across languages, which is common for links, images and template IDs, set the fallback and specify only the differing language values.

### Backend JavaScript

PWNC uses JavaScript primarily to augment the server-generated interface, provide UI convenience, and reduce redundant data transfers. When interacting through HTTP, most of this JavaScript can be ignored.

Inspect JavaScript only when it is relevant to understanding an action exposed by the current HTML interface. In particular, JavaScript may:

* manipulate form fields before submission,
* provide browser-side convenience functionality,
* derive or transform values required for submission, or
* encapsulate HTTP actions that are otherwise expressed less directly in the HTML interface.

Do not attempt to execute browser-side JavaScript. When a relevant action is implemented or encapsulated by JavaScript, inspect only the code necessary to determine the underlying HTTP interaction, then perform that interaction directly through HTTP.

Do not treat JavaScript functions as an independent API or assume that an operation is available merely because a corresponding function exists. The HTML interface and its currently exposed links and forms remain authoritative.

The following functions are common examples of JavaScript that may need to be translated into HTTP actions:

### Command

* `ifc_post(message = "", param = "")` — submits the current form, optionally setting `ifc_message` and the general-purpose parameter `ifc_param` (`""` = keep the current value)
* `ifc_cancel(offset = 0)` — submits the current form after clearing all fields starting at the `offset`th form element
* `ifc_autopost(object, message = "")` — submits the current form when the specified object's value changes, optionally setting `ifc_message` (`""` = keep the current value)

### Value

* `ifc_get(object, index = 0)` — retrieves the value of a form element; `index` is used for arrays such as radio buttons
* `ifc_reset(offset = 0)` — clears all fields starting at the `offset`th form element
* `ifc_set(object, value = "", index = 0)` — sets the value of a form element
* `ifc_copy(source, target)` — copies the value of one form element to another
* `ifc_del(object, index = 0, focus = true)` — clears the value of a form element

### Common

* `ifc_object(name, index = 0, window = this)` — retrieves a form element by name; `index` is used for arrays such as radio buttons

Meow! 🐱‍👤

---

## Additional Resources

* [PWNC Website](https://pwnc.it)
* [Template Manual](../doc/template-manual.md)
* [Style Manual](../doc/style-manual.md)
* [API Reference](../doc/api/README.md)
* [GitHub Repository](https://github.com/heydev-de/pwnc)