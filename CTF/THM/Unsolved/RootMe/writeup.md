# RootMe

## Statement

https://tryhackme.com/room/rrootme

## Solution

You can get the open ports and Apache version with nmap:

```sh
root@ip-10-66-109-9:~# nmap -sC -sV 10.66.136.168
Starting Nmap 7.80 ( https://nmap.org ) at 2026-02-13 20:59 GMT
mass_dns: warning: Unable to open /etc/resolv.conf. Try using --system-dns or specify valid servers with --dns-servers
mass_dns: warning: Unable to determine any DNS servers. Reverse DNS is disabled. Try using --system-dns or specify valid servers with --dns-servers
Nmap scan report for 10.66.136.168
Host is up (0.00023s latency).
Not shown: 998 closed ports
PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.13 (Ubuntu Linux; protocol 2.0)
80/tcp open  http    Apache httpd 2.4.41 ((Ubuntu))
| http-cookie-flags: 
|   /: 
|     PHPSESSID: 
|_      httponly flag not set
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: HackIT - Home
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 7.91 seconds
root@ip-10-66-109-9:~#
```

Dirbuster using a simple wordlist will give you a list of hidden directories:

```sh
root@ip-10-66-109-9:~# gobuster dir -w /usr/share/wordlists/dirb/big.txt -u http://10.66.136.168/
===============================================================
Gobuster v3.6
by OJ Reeves (@TheColonial) & Christian Mehlmauer (@firefart)
===============================================================
[+] Url:                     http://10.66.136.168/
[+] Method:                  GET
[+] Threads:                 10
[+] Wordlist:                /usr/share/wordlists/dirb/big.txt
[+] Negative Status codes:   404
[+] User Agent:              gobuster/3.6
[+] Timeout:                 10s
===============================================================
Starting gobuster in directory enumeration mode
===============================================================
/.htaccess            (Status: 403) [Size: 278]
/.htpasswd            (Status: 403) [Size: 278]
/css                  (Status: 301) [Size: 312] [--> http://10.66.136.168/css/]
/js                   (Status: 301) [Size: 311] [--> http://10.66.136.168/js/]
/panel                (Status: 301) [Size: 314] [--> http://10.66.136.168/panel/]
/server-status        (Status: 403) [Size: 278]
/uploads              (Status: 301) [Size: 316] [--> http://10.66.136.168/uploads/]
Progress: 20469 / 20470 (100.00%)
===============================================================
Finished
===============================================================
root@ip-10-66-109-9:~# 
```

For getting a reverse shell, I need to try something like this: <https://medium.com/@tareshsharma17/simple-php-reverse-shell-061d4a6bd18d>

To be continued ..
