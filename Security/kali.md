# Crypto

## Intro

//TODO

## Notes

## Udemy course: Kali Linux Web App Pentesting Labs course

Following this Udemy course: <https://talend.udemy.com/course/kali-linux-web-app-pentesting-labs>

### SQLi labs setup

A collection of PHP files and a script to populate a MySQL database.

Steps:

- Download SQLi labs from here: <https://github.com/skyblueee/sqli-labs-php7>

- Setup instructions are in the README

- MySQL setup tips and tricks:

  - Run this to add a new user in MySQL:

  ```sh
  root@kali:~# mysql -u root -p
  Enter password:
  Welcome to the MariaDB monitor.  Commands end with ; or \g.
  Your MariaDB connection id is 38
  Server version: 10.3.22-MariaDB-1-log Debian buildd-unstable

  Copyright (c) 2000, 2018, Oracle, MariaDB Corporation Ab and others.

  Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

  MariaDB [(none)]> GRANT ALL PRIVILEGES on *.* to 'amemni'@'localhost' IDENTIFIED BY '******';
  Query OK, 0 rows affected (0.001 sec)

  MariaDB [(none)]> Ctrl-C -- exit!
  Aborted
  root@kali:~#
  ```

  - Make sure MySQL logging is enabled:

  ```sh
  root@kali:/etc/mysql/mariadb.conf.d# grep ^general 50-server.cnf
  general_log_file       = /var/log/mysql/mysql.log
  general_log            = 1
  root@kali:/etc/mysql/mariadb.conf.d#
  ```

- You can access labs from here:

![sqli-labs](./pics/kali/sqli-labs.png)

### WebGoat 8 setup

WebGoat is an intentionally vulnerable web application created by OWASP.

Steps:

- Download the latest WebGoat release from here: <https://github.com/WebGoat/WebGoat/releases>

- Now, run this:

  ```sh
    ┌──(kali㉿kali)-[~/Desktop/Tools]
  └─$ java -jar webgoat-server-8.2.2.jar --server.port=9090
  Picked up _JAVA_OPTIONS: -Dawt.useSystemAAFontSettings=on -Dswing.aatext=true
  12:29:50.193 [main] INFO org.owasp.webgoat.StartWebGoat - Starting WebGoat with args: --server.port=9090

    .   ____          _            __ _ _
   /\\ / ___'_ __ _ _(_)_ __  __ _ \ \ \ \
  ( ( )\___ | '_ | '_| | '_ \/ _` | \ \ \ \
   \\/  ___)| |_)| | | | | || (_| |  ) ) ) )
    '  |____| .__|_| |_|_| |_\__, | / / / /
   =========|_|==============|___/=/_/_/_/
   :: Spring Boot ::                (v2.4.3)

  2022-12-11 12:29:51.591  INFO 28163 --- [           main] org.owasp.webgoat.StartWebGoat           : Starting StartWebGoat v8.2.2 using Java 17.0.5 on kali with PID 28163 (/home/kali/Desktop/Tools/webgoat-server-8.2.2.jar started by kali in /home/kali/Desktop/Tools)
  ...
  ```

- You can access the web application from here:

  ![web-goat](./pics/kali/web-goat.png)

## JuiceShop

JuiceShop is an intentionally vulnerable web application written in JavaScript.

Setup:

- Download and run the Docker image like this:

```sh
┌──(root㉿kali)-[~]
└─# docker pull bkimminich/juice-shop
Using default tag: latest
latest: Pulling from bkimminich/juice-shop
8fdb1fc20e24: Pull complete
fda4ba87f6fb: Pull complete
a1f1879bb7de: Pull complete
cc17a9c838e6: Pull complete
5e3b90b638ec: Pull complete
27e3123c4ce5: Pull complete
Digest: sha256:6102f82c1b3ee80aa29dafa2d8d4029312662dce1f4cd8411c2fac274cd5b523
Status: Downloaded newer image for bkimminich/juice-shop:latest
docker.io/bkimminich/juice-shop:latest

┌──(root㉿kali)-[~]
└─# docker run --rm -p 3000:3000 bkimminich/juice-shop
info: All dependencies in ./package.json are satisfied (OK)
info: Chatbot training data botDefaultTrainingData.json validated (OK)
info: Detected Node.js version v16.18.1 (OK)
info: Detected OS linux (OK)
info: Detected CPU x64 (OK)
info: Configuration default validated (OK)
info: Entity models 19 of 19 are initialized (OK)
info: Required file server.js is present (OK)
info: Required file index.html is present (OK)
info: Required file styles.css is present (OK)
info: Required file main.js is present (OK)
info: Required file tutorial.js is present (OK)
info: Required file polyfills.js is present (OK)
info: Required file runtime.js is present (OK)
info: Required file vendor.js is present (OK)
info: Port 3000 is available (OK)
info: Server listening on port 3000
...
```

- You can access the web application from here:

  ![juice-shop](./pics/kali/juice-shop.png)

## bWAPP

bWAPP is a deliberately insecure web application with over 100 web vulnerabilities, including the OWASP top 10.

- Setup: download the latest VM image from here and set it up on VMware: <https://sourceforge.net/projects/bwapp/files/bee-box/>

- You can access the web application from here, using the internal IP address of the Bee-Box VM:

  ![bee-box](./pics/kali/boo-box.png)

## OWASP A1 injection labs

A1 - injection: focuses on the exploitation of SQL, OS, LDAP, XPath or Regex injection attacks, which means executing malicious commands by sending untrusted data to an interpreter in a command or query.

Lab 1:

- We will use <https://www.altoromutual.com/bank/login.aspx>

- Let's configure Burp Suite. First got to Firefox and set up localhost as a proxy:

  ![proxy-to-localhost](./pics/kali/proxy-to-localhost.png)

- To be continued ..
