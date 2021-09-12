# Shift cipher

### Statement
https://www.root-me.org/en/Challenges/Cryptanalysis/Shift-cipher

### Solution
1. One can simply understand from the challenge description or related resources that this is a Shift (or Ceaser) cipher. One genious way to get the shift offset is by looking at the last character from the hexdump of the ciphertext and comparing it to hex character "0x00" (Null character) corresponding to the end of the text inside the binary file ch7.bin:
```
amemni:~/Desktop/CTF/Root-me/Crypt/ch7$ xxd -p -s -1 ch7.bin 
0a
```

2. Hence it's obvious taht the shift offset is 10 ("0x0a" must be shifted by -1 to get to "0x00"). You can then shift all characters from the ciphertext by 10 to get the original text. Here's how it can be done:
```
with open("ch7.bin", "rb") as f:
    msg = f.read()
    print ''.join([chr((ord(x)-10) % 256) for x in msg])
```

3. Put that in a python script and execute it, you will get the flag:
```
amemni:~/Desktop/CTF/Root-me/Crypt/ch7$ python decrypt.py 
Bravo! Tu peux valider avec le pass Y****
```