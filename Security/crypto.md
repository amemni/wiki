# Crypto

### Why learn Cryptography ?

## Notes

### Udemy course: Master Mathematical Cryptography 2020: Crack Any Code

- Section 2 | Number theory lectures that might help
  - Introduction to congruences:

  ```matlab
  a ≡ b (mod n) iff n | (a - b)
  ```

  - Equivalence classes:

  ```matlab
  S := { a - nq | q ∈ ZZ } = [a]

  Let c ∈ ZZ, let a ≡ b (mod n)
  a + c ≡ b + c (mod n)
  a * c ≡ b * c (mod n)
  ```

  - Introduction to linear congruences:

  ```matlab
  Let c ∈ ZZ, let a ≡ b (mod n)
  a * c ≡ b * c (mod n) => a ≡ b (mod n / (c, n))

  Let k >= 0
  a ≡ b (mod n) => a^k ≡ b^k (mod n)
  ```

  - Systems of residues:

    - Definition: a set of integers such as every integer is congruent to modular m to exactly one of this set

  ```matlab
  If {r1, r2, r3, .. rn} is a complete system of residues (mod n)
  and (a, n) = 1
  then {a * r1, a * r2, r3, .. a * rn} is a complete system of residues (mod n)
  ```

  - Linear congruences:

  ```matlab
  If a * x ≡ b (mod n)
  and (a, n) | b
  then we have a solution
  ```

  - Euclidian algorithm:

  ```matlab
  a = b * q1 + r1 with 0 <= r1 < b
  b = r1 * q2 + r2 with 0 <= r2 <= r1 < b
  r1 = r2 * q2 + r3 with 0 <= r3 <= r2 <= r1 < b
  ...
  rn = rn+1 * qn+1 + rn+2
  rn+1 = rn+2 * qn+2

  Then gcd (a, b) = rn+2
  ```

  - Extended euclidian algorithm:

  ```matlab
  gcd (a, b) = a * x + b * y

  121 = 88 * 1 + 33
  88 = 33 * 2 + 22
  33 = 22 * 1 + 11

  => 11 = 33 - 1 * (88 - 33 * 2)
        = 3 * 33 - 88
  
  => gcd (121, 88) = 11
  ```

    - Solving the multiplicative inverse ?

  ```matlab
  5 * x ≡ 1 (mod 7)

  Using the Euclidian algorithm
  => 1 = 3 * 5 - 2 * 7
  => 1 ≡ 3 * 5 (mod 7)
  => 3 * 5 * x ≡ 3 (mod 7)
  
  Then x ≡ 3 (mod 7) is the solution
  ```
