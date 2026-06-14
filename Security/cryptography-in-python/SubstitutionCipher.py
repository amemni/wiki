import random

def generate_key():
    letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    cletters = list(letters)
    key = {}
    for c in letters:
        key[c] = cletters.pop(random.randint(0, len(cletters) - 1))
    return key

def encrypt(key, message):
    cipher = ""
    for c in message:
        if c in key:
            cipher += key[c]
        else:
            cipher += c
    return cipher

key = generate_key()
print("Key: {}".format(key))
message = "YOU ARE AWESOME"
cipher = encrypt(key, message)
print("Cipher: {}".format(cipher))
