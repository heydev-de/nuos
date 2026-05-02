# NUOS API Documentation

[← Index](../README.md) | [`javascript/asr.js`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## ASR (Asynchronous Server Request) Module

JavaScript utility module for handling asynchronous server requests in NUOS. Provides functions for sending GET/POST requests, binding/unbinding form submission handlers, and processing responses without page reloads. Designed for seamless integration with NUOS's PHP backend while maintaining CSRF protection and cache-busting mechanisms.

---

### `asr_send(url, callback = null)`

Sends an asynchronous GET request to the specified URL with cache-busting and optional callback handling.

#### Parameters

| Name       | Type       | Default | Description                                                                                     |
|------------|------------|---------|-------------------------------------------------------------------------------------------------|
| `url`      | `string`   | -       | Target URL. May include a fragment (`#hash`) and/or existing query parameters.                 |
| `callback` | `function` | `null`  | Optional callback function. Receives the server response text (or `false` on failure) as input. |

#### Return Value
`void`

#### Inner Mechanisms
1. **Cache-Busting**:
   - Generates an 8-character random alphanumeric string (e.g., `_aB3x9Y2z`).
   - Appends it as a query parameter to the URL (e.g., `?_aB3x9Y2z=1`).
   - Preserves existing query parameters and URL fragments.

2. **Request Handling**:
   - Uses the Fetch API with `cache: "no-store"` to bypass HTTP caching.
   - Resolves the response to text if the status is `200-299`; otherwise returns `false`.
   - Invokes the callback with the response text or `false` on failure.

3. **Error Handling**:
   - Silently catches network errors and invokes the callback with `false`.

#### Usage Context
- **Typical Scenarios**:
  - Fetching dynamic content (e.g., search results, live previews).
  - Triggering server-side actions (e.g., toggling settings, loading more data).
  - Polling for updates (e.g., notifications, progress bars).
- **Integration**:
  - Works with NUOS's `cms_url()` and `translate_url()` for URL generation.
  - Compatible with NUOS's CSRF protection (random token is automatically included in `cms_param()`).

#### Example
```javascript
asr_send("content://article/123", function(response) {
    if (response !== false) {
        document.getElementById("preview").innerHTML = response;
    }
});
```

---

### `asr_form_bind(object, callback = null)`

Binds an asynchronous submission handler to a form element, overriding the default synchronous submission.

#### Parameters

| Name       | Type       | Default | Description                                                                                     |
|------------|------------|---------|-------------------------------------------------------------------------------------------------|
| `object`   | `HTMLFormElement` | -       | The form element to bind.                                                                      |
| `callback` | `function` | `null`  | Optional callback function. Receives the server response text (or `false` on failure) as input. |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Override**:
   - Creates a custom `submit` event handler (`asr_submit_function1`) that:
     - Prevents the default form submission.
     - Calls `asr_form_post()` with the form and callback.
   - Attaches the handler to the form's `submit` event.

2. **Native Submit Preservation**:
   - Stores the original `submit` method (`asr_submit_function2`).
   - Overrides the form's `submit` method to dispatch a synthetic `submit` event, ensuring the custom handler is triggered.

#### Usage Context
- **Typical Scenarios**:
  - Submitting forms without page reloads (e.g., login, contact forms, comments).
  - Validating form data asynchronously before submission.
  - Processing file uploads in the background.
- **Integration**:
  - Works with NUOS's form modules (e.g., `form_validate()`).
  - Ensures CSRF tokens (via `cms_param()`) are included in the form data.

#### Example
```javascript
var form = document.getElementById("login-form");
asr_form_bind(form, function(response) {
    if (response === "success") {
        window.location.href = "/dashboard";
    } else {
        alert("Login failed!");
    }
});
```

---

### `asr_form_unbind(object)`

Removes the asynchronous submission handler from a form and restores the original behavior.

#### Parameters

| Name     | Type               | Default | Description                     |
|----------|--------------------|---------|---------------------------------|
| `object` | `HTMLFormElement`  | -       | The form element to unbind.     |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Cleanup**:
   - Removes the custom `submit` event handler (`asr_submit_function1`).
   - Deletes the handler reference from the form object.

2. **Native Submit Restoration**:
   - Restores the original `submit` method (`asr_submit_function2`).
   - Deletes the stored reference.

#### Usage Context
- **Typical Scenarios**:
  - Temporarily disabling asynchronous submission (e.g., during maintenance).
  - Switching between synchronous and asynchronous submission modes.
  - Cleaning up event listeners to prevent memory leaks.

#### Example
```javascript
var form = document.getElementById("login-form");
asr_form_unbind(form); // Revert to default form submission
```

---

### `asr_form_post(object, callback = "")`

Sends an asynchronous POST request with form data to the server.

#### Parameters

| Name       | Type               | Default | Description                                                                                     |
|------------|--------------------|---------|-------------------------------------------------------------------------------------------------|
| `object`   | `HTMLFormElement`  | -       | The form element containing the data to submit.                                                |
| `callback` | `function`         | `""`    | Optional callback function. Receives the server response text (or `false` on failure) as input. |

#### Return Value
`void`

#### Inner Mechanisms
1. **Data Handling**:
   - Creates a `FormData` object from the form, automatically including all input fields (including files).

2. **Request Handling**:
   - Uses the Fetch API to send a POST request to the form's `action` URL.
   - Resolves the response to text if the status is `200-299`; otherwise returns `false`.
   - Invokes the callback with the response text or `false` on failure.

#### Usage Context
- **Typical Scenarios**:
  - Submitting forms with large payloads (e.g., file uploads, rich text editors).
  - Processing form data without page reloads (e.g., AJAX form submissions).
- **Integration**:
  - Works with NUOS's `cms_param()` for CSRF protection (token is included in `FormData`).
  - Compatible with NUOS's backend form handlers (e.g., `form_process()`).

#### Example
```javascript
var form = document.getElementById("upload-form");
asr_form_post(form, function(response) {
    if (response !== false) {
        document.getElementById("status").textContent = "Upload successful!";
    }
});
```


<!-- HASH:540253170e109dc54aace0382d1cf329 -->
