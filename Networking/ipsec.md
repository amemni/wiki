# Leaning IPSec

This document collects notes, findings and interesting links I encounter whilst trying to learn IPSec.

## Some theory

Introduction: <https://docs.strongswan.org/docs/latest/howtos/ipsecProtocol.html>

Cipher suites: <https://docs.strongswan.org/docs/latest/config/proposals.html>

Components of HAGLE:

- H - Hash Algorithm: Identifies the algorithm (e.g., MD5, SHA1, SHA256) used for data integrity.
- A - Authentication Method: Determines how peers prove identity, such as Pre-Shared Keys (PSK) or RSA digital certificates.
- G - Group (Diffie-Hellman): Specifies the Diffie-Hellman group (e.g., Group 2, 5, 14) used to generate shared secret keys for encryption.
- L - Lifetime: Determines how long the IKE Phase 1 SA (Security Association) is valid before being renegotiated, often in seconds or kilobytes.
- E - Encryption Algorithm: Defines the method used to encrypt data, such as DES, 3DES, or AES.

## Writing a simulator

Here I'm following along this Udemy course: [Introduction to Cryptography](https://tryhackme.com/room/cryptographyintro)

Simulator code: <github.com/aseemsethi/ipsec>
