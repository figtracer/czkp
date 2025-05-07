# Zero-Knowledge Proof: Discrete Logarithm Implementation

This is an educational implementation of a zero-knowledge proof (ZKP) system for discrete logarithms, based on the Schnorr identification protocol. This code demonstrates how a prover can convince a verifier that they know a secret value `x` (the discrete logarithm) without revealing `x` itself.

> This is my solution to https://hackmd.io/@gubsheep/Hy57lluOs

## Theoretical Background

**Discrete Logarithm Problem**: Given values `g`, `y`, and `p`, find `x` such that `y = g^x mod p`. This is computationally difficult for large primes `p`.

**Zero-Knowledge Proof**: A method where one party (prover) proves to another party (verifier) that they know a value `x` without conveying any information about `x` apart from the fact that they know it.

## The Schnorr Identification Protocol

This implementation uses a simplified version of the Schnorr protocol:

1. **Setup**: Public parameters are prime `p` and generator `g`. Prover has secret `x` and public key `y = g^x mod p`.

2. **Protocol**:
   - Prover chooses random `r` and computes commitment `h = g^r mod p`
   - Verifier sends challenge bit `b` (0 or 1)
   - Prover computes response `s = (r + b*x) mod (p-1)`
   - Verifier checks if `g^s = h * y^b mod p`

3. **Security**:
   - If `b = 0`: Verifier checks if `g^s = h`, which doesn't depend on `x`
   - If `b = 1`: Verifier checks if `g^s = h * y`, which requires knowledge of `x`

## Important Limitations

This implementation has several limitations that make it educational but not suitable for production use:

1. **1-bit Challenge**: With only a single-bit challenge, an impersonator has a 50% chance of successfully creating a valid proof without knowing `x` (when `b = 0`). A secure implementation would use multiple rounds or a larger challenge space.

2. **Key Size**: The prime `p` is relatively small. For real-world applications, much larger primes would be needed.

3. **Random Number Generation**: While `/dev/urandom` is used, a production system would need more careful treatment of randomness.

## How to Run

```
gcc -o zkp zkp.c
./zkp
```
