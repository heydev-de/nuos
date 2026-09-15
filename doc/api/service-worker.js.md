# PWNC API Documentation

[← Index](README.md) | [`service-worker.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/service-worker.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## service-worker.js

### Overview

The `service-worker.js` file is a minimal [Service Worker](https://developer.mozilla.org/en-US/docs/Web/API/Service_Worker_API) implementation for the PWNC Web Platform. Service Workers act as a proxy between the web application and the browser, enabling features like offline support, request interception, and performance optimizations.

This particular Service Worker implements a basic **network-first** strategy: every fetch event is passed through directly to the network without any caching or modification. While simple, this serves as a foundational scaffold that can be extended with more sophisticated caching logic, offline fallbacks, or request/response transformations.

---

### `fetch` Event Listener

#### Description

Registers a handler for the `fetch` event, which is triggered whenever a network request is made from the controlled web page or application.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `event` | `FetchEvent` | The fetch event object containing the request and methods to respond to it. |

#### Return Value

None (void). The function uses `event.respondWith()` to provide a response to the browser.

#### Inner Mechanism

1. The Service Worker listens for all `fetch` events.
2. For each event, it calls `event.respondWith()`.
3. Inside `respondWith()`, it delegates the request to the global `fetch()` function.
4. This means the request is sent to the network as-is, and the response is returned directly to the client without any local processing or caching.

#### Usage Context

This Service Worker is automatically registered by PWNC-enabled websites (typically via JavaScript in the main application). Once registered, it controls all pages under its scope and intercepts every outgoing HTTP request.

Because it simply forwards requests to the network, it does not provide offline capabilities or performance improvements on its own. However, it establishes the infrastructure needed to implement such features later.

#### Example

```javascript
// In your main application JavaScript (e.g., app.js)
if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('/service-worker.js')
        .then(registration => {
            console.log('Service Worker registered with scope:', registration.scope);
        })
        .catch(error => {
            console.error('Service Worker registration failed:', error);
        });
}
```

In this example:
- The browser checks if Service Workers are supported.
- If so, it registers the `service-worker.js` file located at the root of the site.
- Once registered, the Service Worker begins controlling the page and intercepting fetch events according to the logic defined in `service-worker.js`.

#### Extending the Service Worker

To add caching or offline support, you could modify the `fetch` handler like this:

```javascript
self.addEventListener("fetch", (event) => {
    event.respondWith(
        caches.match(event.request).then((cachedResponse) => {
            return cachedResponse || fetch(event.request);
        })
    );
});
```

This would first check the cache for a matching response before falling back to the network.


<!-- HASH:e79ec2bd5ff762c37eaf8ca9e468a3f7 -->
