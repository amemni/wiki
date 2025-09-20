# SSH and SSHD

## Upload your public SSH key to a server

The easier way is to run the following command:

```sh
ssh-copy-id user@machine
```

## Remove all keys for a hostname from a known_hosts file

Usually needed after rotating SSH host keys, you can run this command:

```sh
ssh-keygen -R machine
```
