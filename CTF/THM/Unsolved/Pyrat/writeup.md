# Pyrat

## Statement

https://tryhackme.com/dashboard

## Solution

Nmap shows a SimpleHTTP server listening on port 8000:

```sh
8000/tcp open  http-alt SimpleHTTP/0.6 Python/3.11.2
| fingerprint-strings: 
|   DNSStatusRequestTCP, DNSVersionBindReqTCP, JavaRMI, LANDesk-RC, NotesRPC, Socks4, X11Probe, afp, giop: 
|     source code string cannot contain null bytes
|   FourOhFourRequest, LPDString, SIPOptions: 
|     invalid syntax (<string>, line 1)
|   GetRequest: 
|     name 'GET' is not defined
|   HTTPOptions, RTSPRequest: 
|     name 'OPTIONS' is not defined
|   Help: 
|_    name 'HELP' is not defined
|_http-open-proxy: Proxy might be redirecting requests
|_http-server-header: SimpleHTTP/0.6 Python/3.11.2
|_http-title: Site doesn't have a title (text/html; charset=utf-8).
1 service unrecognized despite returning data. If you know the service/version, please submit the following fingerprint at https://nmap.org/cgi-bin/submit.cgi?new-service :
SF-Port8000-TCP:V=7.80%I=7%D=2/11%Time=698CF3BF%P=x86_64-pc-linux-gnu%r(Ge
SF:nericLines,1,"\n")%r(GetRequest,1A,"name\x20'GET'\x20is\x20not\x20defin
SF:ed\n")%r(X11Probe,2D,"source\x20code\x20string\x20cannot\x20contain\x20
SF:null\x20bytes\n")%r(FourOhFourRequest,22,"invalid\x20syntax\x20\(<strin
SF:g>,\x20line\x201\)\n")%r(Socks4,2D,"source\x20code\x20string\x20cannot\
SF:x20contain\x20null\x20bytes\n")%r(HTTPOptions,1E,"name\x20'OPTIONS'\x20
SF:is\x20not\x20defined\n")%r(RTSPRequest,1E,"name\x20'OPTIONS'\x20is\x20n
SF:ot\x20defined\n")%r(DNSVersionBindReqTCP,2D,"source\x20code\x20string\x
SF:20cannot\x20contain\x20null\x20bytes\n")%r(DNSStatusRequestTCP,2D,"sour
SF:ce\x20code\x20string\x20cannot\x20contain\x20null\x20bytes\n")%r(Help,1
SF:B,"name\x20'HELP'\x20is\x20not\x20defined\n")%r(LPDString,22,"invalid\x
SF:20syntax\x20\(<string>,\x20line\x201\)\n")%r(SIPOptions,22,"invalid\x20
SF:syntax\x20\(<string>,\x20line\x201\)\n")%r(LANDesk-RC,2D,"source\x20cod
SF:e\x20string\x20cannot\x20contain\x20null\x20bytes\n")%r(NotesRPC,2D,"so
SF:urce\x20code\x20string\x20cannot\x20contain\x20null\x20bytes\n")%r(Java
SF:RMI,2D,"source\x20code\x20string\x20cannot\x20contain\x20null\x20bytes\
SF:n")%r(afp,2D,"source\x20code\x20string\x20cannot\x20contain\x20null\x20
SF:bytes\n")%r(giop,2D,"source\x20code\x20string\x20cannot\x20contain\x20n
SF:ull\x20bytes\n");
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Using the web browser, I get a message saying "Try a more basic connection!".

In Python, I try to open a raw TCP socket connection:

```py
Python 2.7.18 (default, Jan 31 2024, 16:23:13) 
[GCC 9.4.0] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import socket
>>> sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
>>> sock.connect((host, port))
>>> sock.sendall("GET /index.html HTTP/1.1\r\nHost:10.64.162.243\r\n\r\n".encode('utf-8'))
>>> sock.recv(4096).decode('utf-8')
u'HTTP/1.0 200 OK\nServer: SimpleHTTP/0.6 Python/3.11.2\nDate: Wed Feb 11 21:42:28  2026\nContent-type: text/html; charset=utf-8\nContent-Length: 27\n\nTry a more basic connection!\n\n'
>>> 
```

Then, the server appears to be running a Python REPL:

```py
>>> sock.sendall("print('yo')\r\n".encode('utf-8'))
>>> sock.recv(4096).decode('utf-8')
u'yo\n\n'
>>> 
```

To be continued ..
