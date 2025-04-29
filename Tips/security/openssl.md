# OpenSSL

## Using SSH public keys to encrypt files

Encrypt like this:

```sh
openssl pkeyutl -encrypt -pubin -inkey public_key.pem -in data.in.txt -out data.enc.txt
```

Decrypt like this:

```sh
openssl pkeyutl -decrypt -inkey private_key.pem -in data.enc.txt -out data.out.txt
cat data.out.txt
```

❕ For error message "Could not find private key of public key ..", make sure to use an SSL public key (instead of RSA): `openssl rsa -in id_rsa.pem -RSAPublicKey_in -pubout > id_pub.pem`
