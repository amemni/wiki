# Metasploit

## Why learn Metasploit

Learning Metasploit is essential for ethical hackers during all phases of a penetration testing exercise, from information gathering to post-exploitation.
Metasploit is actually the most widely used exploitation framework.

## Introduction

The main components of the Metasploit framework can be summarized as follows;

- msfconsole: the main command-line interface.
- Modules: supporting modules such as exploits, scanners, payloads, etc.
- Tools: stand-alone tools that will help vulnerability research, vulnerability assessment, or penetration testing, example: `msfvenom`, `pattern_create` and `pattern_offset`.

Important concepts:

- Exploit: a piece of code that uses a vulnerability present on the target system.
- Vulnerability: A design, coding, or logic flaw affecting the target system, which can result in disclosing confidential information or allowing to execute code on the target system.
- Payload: an exploit will take advantage of a vulnerability, payloads are the code that will run on the target system to have the result we want (gaining access to the target system, read confidential information, etc.).

Modules and categories under each one are listed below:

- Auxiliary: any supporting module, such as scanners, crawlers and fuzzers, can be found here.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 auxiliary/
auxiliary/
├── admin
├── analyze
├── bnat
├── client
├── cloud
├── crawler
├── docx
├── dos
├── example.py
├── example.rb
├── fileformat
├── fuzzers
├── gather
├── parser
├── pdf
├── scanner
├── server
├── sniffer
├── spoof
├── sqli
├── voip
└── vsploit
```

- Encoders: will allow you to encode the exploit and payload in the hope that a signature-based antivirus solution may miss them.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 encoders/
encoders/
├── cmd
├── generic
├── mipsbe
├── mipsle
├── php
├── ppc
├── ruby
├── sparc
├── x64
└── x86
```

- Evasion: while encoders will encode the payload, they should not be considered a direct attempt to evade antivirus software. On the other hand, “evasion” modules will try that, with more or less success.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 2 evasion/
evasion/
└── windows
    ├── applocker_evasion_install_util.rb
    ├── applocker_evasion_msbuild.rb
    ├── applocker_evasion_presentationhost.rb
    ├── applocker_evasion_regasm_regsvcs.rb
    ├── applocker_evasion_workflow_compiler.rb
    ├── process_herpaderping.rb
    ├── syscall_inject.rb
    ├── windows_defender_exe.rb
    └── windows_defender_js_hta.rb
```

- Exploits: neatly organized by target system.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 exploits/
exploits/
├── aix
├── android
├── apple_ios
├── bsd
├── bsdi
├── dialup
├── example_linux_priv_esc.rb
├── example.py
├── example.rb
├── example_webapp.rb
├── firefox
├── freebsd
├── hpux
├── irix
├── linux
├── mainframe
├── multi
├── netware
├── openbsd
├── osx
├── qnx
├── solaris
├── unix
└── windows
```

- NOPs (No OPeration): do nothing, literally, they are represented in the Intel x86 CPU family with 0x90, following which the CPU will do nothing for one cycle, they are often used as a buffer to achieve consistent payload sizes.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 nops/
nops/
├── aarch64
├── armle
├── cmd
├── mipsbe
├── php
├── ppc
├── sparc
├── tty
├── x64
└── x86
```

- Payloads: codes that will run on the target system to achieve the desired result, examples could be; getting a shell, loading a malware or backdoor to the target system, running a command, or launching calc.exe as a proof of concept to add to the penetration test report.

```sh
Terminal
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 payloads/
payloads/
├── adapters
├── singles
├── stagers
└── stages
```

You will see four different directories under payloads: adapters, singles, stagers and stages.

  - Adapters: an adapter wraps single payloads to convert them into different formats, for example: a normal single payload can be wrapped inside a Powershell adapter, which will make a single powershell command that will execute the payload.
  - Singles: self-contained payloads (add user, launch notepad.exe, etc.) that do not need to download an additional component to run.
  - Stagers: responsible for setting up a connection channel between Metasploit and the target system, useful when working with staged payloads which will first upload a stager on the target system then download the rest of the payload (stage). (Usually, the initial size of the payload will be relatively small compared to the full payload sent at once.)
  - Stages: downloaded by the stager, this will allow you to use larger sized payloads.

Difference between single (also called “inline”) payloads and staged payloads.

  - `generic/shell_reverse_tcp`: inline (or single) payload, as indicated by the “_” between “shell” and “reverse”
  - `windows/x64/shell/reverse_tcp`: staged payload, as indicated by the “/” between “shell” and “reverse”.


- Post: will be useful on the final stage of the penetration testing process listed above, post-exploitation.

```sh
root@ip-10-10-135-188:/opt/metasploit-framework/embedded/framework/modules# tree -L 1 post/
post/
├── aix
├── android
├── apple_ios
├── bsd
├── firefox
├── hardware
├── linux
├── multi
├── networking
├── osx
├── solaris
└── windows
```

## Exploitation

Port Scanning:

- You can list potential port scanning modules available using the search command:

```sh
msf > search portscan

Matching Modules
================

   #  Full Name                                         Disclosure Date  Rank    Check  Name
   -  ---------                                         ---------------  ----    -----  ----
   0  auxiliary/scanner/portscan/ftpbounce              .                normal  No     FTP Bounce Port Scanner
   1  auxiliary/scanner/natpmp/natpmp_portscan          .                normal  No     NAT-PMP External Port Scanner
   2  auxiliary/scanner/sap/sap_router_portscanner      .                normal  No     SAPRouter Port Scanner
   3  auxiliary/scanner/portscan/xmas                   .                normal  No     TCP "XMas" Port Scanner
   4  auxiliary/scanner/portscan/ack                    .                normal  No     TCP ACK Firewall Scanner
   5  auxiliary/scanner/portscan/tcp                    .                normal  No     TCP Port Scanner
   6  auxiliary/scanner/portscan/syn                    .                normal  No     TCP SYN Port Scanner
   7  auxiliary/scanner/http/wordpress_pingback_access  .                normal  No     Wordpress Pingback Locator


Interact with a module by name or index. For example info 7, use 7 or use auxiliary/scanner/http/wordpress_pingback_access

msf > use 5
```

- The following module options needs to be set:

```sh
msf auxiliary(scanner/portscan/tcp) > show options

Module options (auxiliary/scanner/portscan/tcp):

   Name         Current Setting  Required  Description
   ----         ---------------  --------  -----------
   CONCURRENCY  10               yes       The number of concurrent ports to check per host
   DELAY        0                yes       The delay between connections, per thread, in mi
                                           lliseconds
   JITTER       0                yes       The delay jitter factor (maximum value by which
                                           to +/- DELAY) in milliseconds.
   PORTS        1-10000          yes       Ports to scan (e.g. 22-25,80,110-900)
   RHOSTS                        yes       The target host(s), see https://docs.metasploit.
                                           com/docs/using-metasploit/basics/using-metasploi
                                           t.html
   THREADS      1                yes       The number of concurrent threads (max one per ho
                                           st)
   TIMEOUT      1000             yes       The socket connect timeout in milliseconds


View the full module info with the info, or info -d command.

msf auxiliary(scanner/portscan/tcp) >
```

UDP Service Identification:

- The `scanner/discovery/udp_sweep` module will allow you to quickly identify services running over UDP. The module will not conduct an extensive scan of all possible UDP services but quicly identifies services such as DNS or NetBIOS:

```sh
msf auxiliary(scanner/discovery/udp_sweep) > run
[*] Sending 13 probes to 10.81.159.26->10.81.159.26 (1 hosts)
[*] Discovered NetBIOS on 10.81.159.26:137 (IP-10-81-159-26:<00>:U :IP-10-81-159-26:<03>:U :IP-10-81-159-26:<20>:U :__MSBROWSE__:<01>:G :ACME IT SUPPORT:<00>:G :ACME IT SUPPORT:<1d>:U :ACME IT SUPPORT:<1e>:G :00:00:00:00:00:00)
[*] Scanned 1 of 1 hosts (100% complete)
[*] Auxiliary module execution completed
msf auxiliary(scanner/discovery/udp_sweep) >
```

SMB Scan:

- There are some useful SMB scanning modules like `smb_enumshares` and `smb_version`:

```sh
msf auxiliary(scanner/smb/smb_version) > run
[*] 10.81.159.26:445      - SMB Detected (versions: 2, 3) (preferred dialect: SMB 3.1.1) (compression capabilities: ) (encryption capabilities: AES-128-GCM) (signatures: optional) (guid: {312d7069-2d30-3138-2d31-35392d323600}) (authentication domain: IP-10-81-159-26)
[+] 10.81.159.26:445      -   Host is running Version 6.1.0 (unknown OS)
[*] 10.81.159.26:445      -   SMB signing is not required
[*] 10.81.159.26          - Scanned 1 of 1 hosts (100% complete)
[*] Auxiliary module execution completed
msf auxiliary(scanner/smb/smb_version) >
```