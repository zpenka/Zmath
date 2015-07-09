This is a library I wrote to assist with problems on [Project Euler](https://projecteuler.net/). It's mostly a heavily optimized prime number generator, with some functions on top to query it. It uses both sieve and wheel factorization methods, and in both cases removes the first 6 primes from consideration; testing revealed this to be the optimal number. On my current computer, it generates primes up to 1 billion in 2 seconds, and up to the 32-bit max of 4.29 billion in 10 seconds. Documentation is in the file.

![](https://i.imgur.com/qrHKIiG.jpg)
