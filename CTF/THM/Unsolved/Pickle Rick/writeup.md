# Pickle Rick

## Statement

https://tryhackme.com/room/picklerick

## Solution

If you inspect the main page, you will find a comment containing a possible user name: `R1ckRul3s`

If you check the 'robots.txt', you will find a possible password: `Wubbalubbadubdub`

Dirbuster using a simple wordlist will give you a list of hidden directories:

```sh
root@ip-10-81-123-98:~# gobuster dir -w /usr/share/wordlists/dirb/common.txt -u http://10.81.157.160/ 
===============================================================
Gobuster v3.6
by OJ Reeves (@TheColonial) & Christian Mehlmauer (@firefart)
===============================================================
[+] Url:                     http://10.81.157.160/
[+] Method:                  GET
[+] Threads:                 10
[+] Wordlist:                /usr/share/wordlists/dirb/common.txt
[+] Negative Status codes:   404
[+] User Agent:              gobuster/3.6
[+] Timeout:                 10s
===============================================================
Starting gobuster in directory enumeration mode
===============================================================
/.hta                 (Status: 403) [Size: 278]
/.htaccess            (Status: 403) [Size: 278]
/.htpasswd            (Status: 403) [Size: 278]
/assets               (Status: 301) [Size: 315] [--> http://10.81.157.160/assets/]
/index.html           (Status: 200) [Size: 1062]
/robots.txt           (Status: 200) [Size: 17]
/server-status        (Status: 403) [Size: 278]
Progress: 4614 / 4615 (99.98%)
===============================================================
Finished
root@ip-10-81-123-98:~#
``