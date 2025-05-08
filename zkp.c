#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* Prime number p */
const long long p = 234234163;
/* Previously computed generator g */
const long long g = 2;

/**
 * @param h Commitment: g^r mod p
 * @param s Response: (r + b*x) mod (p-1)
 * @param b Challenge: a random bit
 */
typedef struct
{
    long long h;
    long long s;
    long long b;
} Proof;

/**
 * Generate a random number from 0 to (p-1)
 * @return a random number from 0 to (p-1)
 */
long long gen_rand_num()
{
    unsigned long long r;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
    {
        perror("open /dev/urandom failed");
        exit(1);
    }
    if (read(fd, &r, sizeof(r)) != sizeof(r))
    {
        perror("read /dev/urandom failed");
        exit(1);
    }
    close(fd);

    long long rand_num = r % (unsigned long long)p;
    return rand_num;
}

/**
 * Modular exponentiation
 * 
 * @param x the base
 * @param y the exponent
 * @param p the modulus
 * @return the result of x^y (mod p)
 */
long long mod_pow(long long x, long long y, long long p)
{
    long long result = 1;
    x = x % p;
    while (y > 0)
    {
        if (y % 2 == 1)
            result = (result * x) % p;
        y = y >> 1;
        x = (x * x) % p;
    }
    return result;
}

/**
 * Create a discrete logarithm proof for y = g^x mod p
 *
 * @param x the secret discrete logarithm
 * @param g the generator
 * @param p the prime modulus
 * @param y pointer to store the public key g^x mod p
 * @return the proof
 */
Proof dlogProof(long long x, long long g, long long p, long long *y)
{
    Proof proof;

    /* Calculate public key y = g^x mod p */
    *y = mod_pow(g, x, p);

    /* [Prover] Choose random r, where 0 <= r < p-1 */
    long long r = gen_rand_num();

    /* [Prover] Compute commitment h = g^r mod p */
    proof.h = mod_pow(g, r, p);

    /* [Verifier] Generate a random bit b */
    proof.b = gen_rand_num() % 2;

    /* [Prover] Compute response s = (r + b*x) mod (p-1) */
    proof.s = (r + (proof.b * x)) % (p - 1);

    return proof;
}

/**
 * Verify that g^s ≡ h * y^b (mod p)
 *
 * @param y the public key (g^x mod p)
 * @param g the generator
 * @param p the prime modulus
 * @param proof the proof to verify
 * @return 1 if valid, 0 if invalid
 */
int verify(long long y, long long g, long long p, Proof proof)
{
    /* Calculate left side: g^s mod p */
    long long left_side = mod_pow(g, proof.s, p);

    /* Calculate right side: (h * y^b) mod p */
    long long y_b = mod_pow(y, proof.b, p);
    long long right_side = (proof.h * y_b) % p;
        
    /* Check if they're equal */
    return (left_side == right_side);
}

void proof_of_knowledge()
{
    /* [Prover] Generate private key (x) */
    long long x = gen_rand_num();
    /* [Prover] Create proof */
    long long y;
    Proof proof = dlogProof(x, g, p, &y);

    /* [Verifier] Verify valid proof against the correct public key (y) */
    int valid = verify(y, g, p, proof);
    printf("-------- Verify w/ correct x -------\n");
    printf("Prover's private key (x): %lld\n", x);
    printf("Prover's public key (y = g^x mod p): %lld\n", y);
    printf("Proof: { h: %lld, b: %lld, s: %lld }\n", proof.h, proof.b, proof.s);
    printf("g^s = %lld\n", mod_pow(g, proof.s, p));
    printf("h * y^b = %lld\n", (proof.h * mod_pow(y, proof.b, p)) % p);
    printf("Verification %s\n\n", valid ? "successful" : "failed");

    /* [Fake Prover] Generate private key (x) */
    long long fake_x = gen_rand_num();
    /* [Fake Prover] Create proof with fake x */
    long long fake_y;
    Proof fake_proof = dlogProof(fake_x, g, p, &fake_y);

    /** 
     * [Verifier] Verify fake proof against the correct public key (y) 
     * 
     * This should fail 50% of the times (when b = 1) because the fake prover doesn't know the discrete
     * logarithm of y.
     * 
     * If b = 0, the proof will be valid because the fake prover is essentially proving that h = h and (h) is a known value.
     * For the proof to be valid, g^s ≡ h * y^b (mod p) must hold true.
     * When b = 0, s = r, so g^s = g^r = h. Furthermore, h * y^0 = h.
     */
    int fake_valid = verify(y, g, p, fake_proof);
    printf("-------- Verify w/ wrong x ---------\n");
    printf("Fake prover's private key (fake_x): %lld\n", fake_x);
    printf("Fake prover's public key (fake_y = g^fake_x mod p): %lld\n", fake_y);
    printf("Fake proof: { h: %lld, b: %lld, s: %lld }\n", fake_proof.h, fake_proof.b, fake_proof.s);
    printf("g^s = %lld\n", mod_pow(g, fake_proof.s, p));
    printf("h * y^b = %lld\n", (fake_proof.h * mod_pow(y, fake_proof.b, p)) % p);
    printf("Verification %s\n", fake_valid ? "successful" : "failed");
}

int main()
{
    proof_of_knowledge();
    return 0;
}
