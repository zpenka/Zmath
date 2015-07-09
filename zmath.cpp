/*

If you're using the GCC compiler, this code should be built with the -std=c++0x or -std=c++11 flag. I don't know how other compilers work.
The -O2 or -O3 flag significantly speeds up execution, by a factor of 4 with 32-bit numbers, and a factor of 2 with 64-bit numbers.

On my computer, using buildSieveTo(n), it generate primes up to:
                                                            32-bit max
                            1 billion       2 billion       4289995710      5 billion       10 billion      15 billion

    in:                     10.3 seconds    22.2 seconds    51.0 seconds    61.8 seconds    145 seconds     232 seconds
    with the -O3 flag:      2.6 seconds     5.5 seconds     12.8 seconds    17.8 seconds    56.4 seconds    96.8 seconds

    At 15.5 billion it throws std::length_error  --  what():  vector<bool>::_M_fill_insert.
    At 26.7 billion it just fails.



Negative numbers will produce undefined behavior. Don't call functions with negative parameters.
If you do, the functions will map your negative number to a really really big positive number and probably hang.

The most efficient way to use this class, if you are making multiple function calls, is:
    Initialize it and then immediately call buildSieveTo(n), when you know that you won't need any primes above n,
    and then use [Unsafe] versions of the functions.
Keep in mind that the factor function only needs primes up to sqrt(n) in the worst case (where n is prime or the square of a prime).
The [Wheel] versions of functions do not need the sieve at all.

If you just directly call the functions, they will only sieve up to whatever they need at the moment.
If you are calling them with increasing inputs, this will sieve a whole bunch of times, a little bit at a time, which is super slow.



The most robust way to test a prime is to use isPrime(n).
However, if you are testing a lot of primes, one at a time, several optimizations are included.
The general format is isPrime[Small][Notx][Unsafe](n). (Don't include the brackets.)

Include [Small] if n is an unsigned int (32 bits, max 4 billion), and not an unsigned long long (64 bits, max a whole bunch).
    Note: Because of performance issues, the cutoff is actually 142857*30030=4289995710, not 2^32-1=4294967296.
Include the [Notx] clause if you already know something about the primes:
    Use NotDiv2 if you know it is not divisible by or equal to 2.
    Use NotDiv23 if you know it is not divisible by or equal to 2 or 3.
    Use NotDiv25 if you know it is not divisible by or equal to 2 or 5.
    Use NotDiv235 if you know it is not divisible by or equal to 2, 3, or 5.
    Use Not23571113 if you know it is not 2, 3, 5, 7, 11, or 13.
    Use Not3571113NotDiv2 if you know it is not 3, 5, 7, 11, or 13, and is not divisible by or equal to 2.
    Use Not571113NotDiv23 if you know it is not 5, 7, 11, or 13, and is not divisible by or equal to 2 or 3.
    Use Not371113NotDiv25 if you know it is not 3, 7, 11, or 13, and is not divisible by or equal to 2 or 5.
    Use Not71113NotDiv235 if you know it is not 7, 11, or 13, and is not divisible by or equal to 2, 3, or 5.
    Use NotDiv23571113 if you know it is not divisible by 2, 3, 5, 7, 11, or 13.
Include [Unsafe] if you have already called buildSeiveTo(n) and you know the sieve is big enough.

So the most optimized version is isPrimeSmallNotDiv23571113Unsafe(n).

Example:
    PrimeData pd;
    pd.buildSieveTo(1000000000);
    for(unsigned int i=999999931; i<999999950; i+=2){
        if(pd.isPrimeSmallNot3571113NotDiv2Unsafe(i))
            std::cout<<i<<" is prime!"<<std::endl;
        else
            std::cout<<i<<" is not prime."<<std::endl;
    }

If you want to use trial division instead of building a sieve and generating all prime numbers up to n, use isPrimeWheel(n).
This is pretty efficient, it will not divide by any composites that are divisible by 2, 3, 5, 7, 11, or 13.
Optimizations are included for this as well, the format is isPrime[Small][Notx]Wheel(n).
[Small] works the same way as in isPrime(n).
The [Notx] clause works almost the same as in isPrime(n), except you can't pass 1 as the parameter:
    Use Not1NotDiv2 if you know it is not 1 and is not divisible by or equal to 2.
    Use Not1NotDiv23 if you know it is not 1 and is not divisible by or equal to 2 or 3.
    Use Not1NotDiv25 if you know it is not 1 and is not divisible by or equal to 2 or 5.
    Use Not1NotDiv235 if you know it is not 1 and is not divisible by or equal to 2, 3, or 5.
    Use Not123571113 if you know it is not 1, 2, 3, 5, 7, 11, or 13.
    Use Not13571113NotDiv2 if you know it is not 1, 3, 5, 7, 11, or 13, and is not divisible by or equal to 2.
    Use Not1571113NotDiv23 if you know it is not 1, 5, 7, 11, or 13, and is not divisible by or equal to 2 or 3.
    Use Not1371113NotDiv25 if you know it is not 1, 3, 7, 11, or 13, and is not divisible by or equal to 2 or 5.
    Use Not171113NotDiv235 if you know it is not 1, 7, 11, or 13, and is not divisible by or equal to 2, 3, or 5.
    Use Not1NotDiv23571113 if you know it is not 1 and is not divisible by 2, 3, 5, 7, 11, or 13.

Example:
    PrimeData pd;
    std::cout<<pd.isPrimeSmallNot171113NotDiv235Wheel(999999937)<<std::endl;



To prime factor a number, use factor(n,x,y), where x and y are arrays to store factors and exponents, respectively.
(The arrays can be called whatever you like.)
The maximum number of prime factors a 64-bit integer can have is 15.
x must be an array of unsigned long long, with size at least 15.
y must be an array of unsigned char, with size at least 15.
Example: if you factor 18, x will be {2, 3, ... } and y will be {1, 2, ... }, representing 2^1 * 3^2.
The return value is the number of prime factors.
Keep in mind that to output the exponents correctly, you need to cast them to ints.
Dont try to factor 0. It's divisible by everything.
1 is not divisible by any primes.

A few optimizations are included. The format is factor[Small][Unsafe/Wheel](n,x,y).
[Small], [Wheel], and [Unsafe] all work the same way as in isPrime(n).
If you use [Small],
    The maximum number of prime factors a 32-bit integer can have is 9.
    x must be an array of unsigned int (not unsigned long long), with size at least 9.
    y must be an array of unsigned char, with size at least 9.
[Wheel] and [Unsafe] cannot be used together.
The sieve in the normal version is pretty optimized and only sieves as high as it needs to go,
    which could be much less than sqrt(n) if n has several factors.
Using wheel factorization is faster than sieving if you're only factoring a couple numbers.
The sieved version becomes much faster after the first sieve (which you should do manually, and use [Unsafe]), and catches up quickly.

Example:
    unsigned long long x[15];
    unsigned char y[15];
    PrimeData pd;
    unsigned char m = pd.factor(8605230063264769013,x,y);
    for(unsigned char i=0; i<m; ++i)
        std::cout<<x[i]<<'^'<<(int)y[i]<<' ';
    std::cout<<std::endl;
    m = pd.factorWheel(12764787846358441471U,x,y);
    for(unsigned char i=0; i<m; ++i)
        std::cout<<x[i]<<'^'<<(int)y[i]<<' ';



To find how many primes exist up to n, use numPrimesUpTo(n).
The optimization numPrimesUpToSmall(n) is included, where [Small] works the same way as above.

To generate the actual primes up to n, use primesUpTo(n,primes), where primes is a vector of unsigned long long.
If you are only calling this once, you don't need to call buildSieveTo(n) first, it will handle it.
This function takes longer than buildSieveTo(n), but it stores the primes in a human-readable format,
    rather than just being queryable with isPrime(n) and factor(n).
Its return value is an unsigned long long that represents the number of primes in the vector.
    Calling primes.size() will return 0 and is not helpful.
The optimization primesUpToSmall(n,primes) is included, where [Small] works the same way as above.
    In this case, n, primes, and the return value are all unsigned ints instead of unsigned long long.
On my computer, calling primesUpTo(n) errors out at 4.6 billion  --  "what():  std::bad_alloc"

Example:
    std::vector<unsigned int> primes;
    PrimeData pd;
    unsigned int x=pd.primesUpToSmall(100000,primes);
    for(unsigned int i=0; i<x; ++i)
        std::cout<<primes[i]<<' ';



To generate the first n primes, use firstPrimes(n,primes), where primes is a vector of unsigned long long.
If you are only calling this once, you don't need to call buildSieveTo(n) first.
The optimization firstPrimesSmall(n,primes) is included. In this case,
    n must be at most 203056267, the number of primes below 4289995710, and
    primes must be a vector of unsigned int instead of unsigned long long.
This function can go a bit further than primesUpTo, because it knows ahead of time how large to make the vector,
    and doesn't have to overcompensate.
    On my computer, I can safely generate the first 230 million primes, up to around 4.9 billion.

Example:
    std::vector<unsigned long long> primes;
    PrimeData pd;
    pd.firstPrimes(100000000,primes);
    std::cout<<primes[99999999]<<endl;



The length of time taken by these functions, in order, is roughly:
    firstPrimes ~ primesUpTo  >  firstPrimesSmall ~ numPrimesUpTo ~ primesUpToSmall ~ numPrimesUpToSmall  >  buildSieveTo
In the 1-4 billion range,
    firstPrimes and primesUpTo take 50~60% longer than buildSieveTo,
     and the other 4 functions take 20~30% longer than buildSieveTo.
Below that range, the ratios are larger but the differences are smaller.
Above that range, I can only test numPrimesUpTo, and the ratio is smaller while the difference gets larger,
    although it does precalculate the number of primes up to 4289995710.

*/



#include <vector>
#include <math.h>

class PrimeData {
    public:
        PrimeData(){
            sieve.push_back(0);
            const unsigned short tempconversions[480] = {1,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,169,173,179,181,191,193,197,199,211,221,223,227,229,233,239,241,247,251,257,263,269,271,277,281,283,289,293,299,307,311,313,317,323,331,337,347,349,353,359,361,367,373,377,379,383,389,391,397,401,403,409,419,421,431,433,437,439,443,449,457,461,463,467,479,481,487,491,493,499,503,509,521,523,527,529,533,541,547,551,557,559,563,569,571,577,587,589,593,599,601,607,611,613,617,619,629,631,641,643,647,653,659,661,667,673,677,683,689,691,697,701,703,709,713,719,727,731,733,739,743,751,757,761,767,769,773,779,787,793,797,799,809,811,817,821,823,827,829,839,841,851,853,857,859,863,871,877,881,883,887,893,899,901,907,911,919,923,929,937,941,943,947,949,953,961,967,971,977,983,989,991,997,1003,1007,1009,1013,1019,1021,1027,1031,1033,1037,1039,1049,1051,1061,1063,1069,1073,1079,1081,1087,1091,1093,1097,1103,1109,1117,1121,1123,1129,1139,1147,1151,1153,1157,1159,1163,1171,1181,1187,1189,1193,1201,1207,1213,1217,1219,1223,1229,1231,1237,1241,1247,1249,1259,1261,1271,1273,1277,1279,1283,1289,1291,1297,1301,1303,1307,1313,1319,1321,1327,1333,1339,1343,1349,1357,1361,1363,1367,1369,1373,1381,1387,1391,1399,1403,1409,1411,1417,1423,1427,1429,1433,1439,1447,1451,1453,1457,1459,1469,1471,1481,1483,1487,1489,1493,1499,1501,1511,1513,1517,1523,1531,1537,1541,1543,1549,1553,1559,1567,1571,1577,1579,1583,1591,1597,1601,1607,1609,1613,1619,1621,1627,1633,1637,1643,1649,1651,1657,1663,1667,1669,1679,1681,1691,1693,1697,1699,1703,1709,1711,1717,1721,1723,1733,1739,1741,1747,1751,1753,1759,1763,1769,1777,1781,1783,1787,1789,1801,1807,1811,1817,1819,1823,1829,1831,1843,1847,1849,1853,1861,1867,1871,1873,1877,1879,1889,1891,1901,1907,1909,1913,1919,1921,1927,1931,1933,1937,1943,1949,1951,1957,1961,1963,1973,1979,1987,1993,1997,1999,2003,2011,2017,2021,2027,2029,2033,2039,2041,2047,2053,2059,2063,2069,2071,2077,2081,2083,2087,2089,2099,2111,2113,2117,2119,2129,2131,2137,2141,2143,2147,2153,2159,2161,2171,2173,2179,2183,2197,2201,2203,2207,2209,2213,2221,2227,2231,2237,2239,2243,2249,2251,2257,2263,2267,2269,2273,2279,2281,2287,2291,2293,2297,2309};
            unsigned char j=0;
            unsigned short k=0;
            for(unsigned short i=0; i<5760; ++i){
                unsigned short n=2310*j+tempconversions[k];
                while(!(n%13)){
                    ++k;
                    if(k==480){
                        ++j;
                        k=0;
                    }
                    n=2310*j+tempconversions[k];
                }
                conversions[i]=n;
                indexes[n]=i;
                ++k;
                if(k==480){
                    ++j;
                    k=0;
                }
            }
            for(unsigned short i=0; i<5759; ++i)
                wheel[i]=conversions[i+1]-conversions[i];
            wheel[5759]=2;
        }

        void buildSieveTo(unsigned long long n){
            if(n<=4289995710U)
                doSieve(n);
            else{
                doSieve(4289995710U);
                doBigSieve(n);
            }
        }

        bool isPrime(unsigned long long n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv2(unsigned long long n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv23(unsigned long long n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv25(unsigned long long n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv235(unsigned long long n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot23571113(unsigned long long n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot3571113NotDiv2(unsigned long long n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot571113NotDiv23(unsigned long long n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot371113NotDiv25(unsigned long long n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot71113NotDiv235(unsigned long long n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv23571113(unsigned long long n){
            buildSieveTo(n);
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }

        bool isPrimeUnsafe(unsigned long long n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv2Unsafe(unsigned long long n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv23Unsafe(unsigned long long n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv25Unsafe(unsigned long long n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv235Unsafe(unsigned long long n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot23571113Unsafe(unsigned long long n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot3571113NotDiv2Unsafe(unsigned long long n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot571113NotDiv23Unsafe(unsigned long long n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot371113NotDiv25Unsafe(unsigned long long n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNot71113NotDiv235Unsafe(unsigned long long n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }
        bool isPrimeNotDiv23571113Unsafe(unsigned long long n){
            if(n<=4289995710U)
                return sieve[n/30030*5760+indexes[n%30030]];
            return bigsieve[n/30030*5760+indexes[n%30030]-822856320];
        }

        bool isPrimeSmall(unsigned int n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv2(unsigned int n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv23(unsigned int n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv25(unsigned int n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv235(unsigned int n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot23571113(unsigned int n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot3571113NotDiv2(unsigned int n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot571113NotDiv23(unsigned int n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot371113NotDiv25(unsigned int n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot71113NotDiv235(unsigned int n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv23571113(unsigned int n){
            doSieve(n);
            return sieve[n/30030*5760+indexes[n%30030]];
        }

        bool isPrimeSmallUnsafe(unsigned int n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv2Unsafe(unsigned int n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv23Unsafe(unsigned int n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv25Unsafe(unsigned int n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv235Unsafe(unsigned int n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot23571113Unsafe(unsigned int n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot3571113NotDiv2Unsafe(unsigned int n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot571113NotDiv23Unsafe(unsigned int n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot371113NotDiv25Unsafe(unsigned int n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNot71113NotDiv235Unsafe(unsigned int n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            return sieve[n/30030*5760+indexes[n%30030]];
        }
        bool isPrimeSmallNotDiv23571113Unsafe(unsigned int n){
            return sieve[n/30030*5760+indexes[n%30030]];
        }

        bool isPrimeWheel(unsigned long long n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(n==1 || !(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1NotDiv2Wheel(unsigned long long n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1NotDiv23Wheel(unsigned long long n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1NotDiv25Wheel(unsigned long long n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1NotDiv235Wheel(unsigned long long n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot123571113Wheel(unsigned long long n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot13571113NotDiv2Wheel(unsigned long long n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1571113NotDiv23Wheel(unsigned long long n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1371113NotDiv25Wheel(unsigned long long n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot171113NotDiv235Wheel(unsigned long long n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeNot1NotDiv23571113Wheel(unsigned long long n){
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }

        bool isPrimeSmallWheel(unsigned int n){
            if(n==2 || n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(n==1 || !(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1NotDiv2Wheel(unsigned int n){
            if(n==3 || n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1NotDiv23Wheel(unsigned int n){
            if(n==5 || n==7 || n==11 || n==13)
                return true;
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1NotDiv25Wheel(unsigned int n){
            if(n==3 || n==7 || n==11 || n==13)
                return true;
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1NotDiv235Wheel(unsigned int n){
            if(n==7 || n==11 || n==13)
                return true;
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot123571113Wheel(unsigned int n){
            if(!(n&1U) || !(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot13571113NotDiv2Wheel(unsigned int n){
            if(!(n%3) || !(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1571113NotDiv23Wheel(unsigned int n){
            if(!(n%5) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1371113NotDiv25Wheel(unsigned int n){
            if(!(n%3) || !(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot171113NotDiv235Wheel(unsigned int n){
            if(!(n%7) || !(n%11) || !(n%13))
                return false;
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }
        bool isPrimeSmallNot1NotDiv23571113Wheel(unsigned int n){
            unsigned int p=17;
            unsigned short w=1;
            unsigned int sqrtn=sqrt(n);
            while(p<=sqrtn){
                if(!(n%p))
                    return false;
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            return true;
        }

        unsigned char factor(unsigned long long n, unsigned long long* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned int b = 1;
            unsigned long long p = 17;
            if((unsigned long long)maxsieve*maxsieve<n){
                while(b<maxbool && p*p<=n){
                    while(!(n%p)){
                        ++exp;
                        n/=p;
                    }
                    if(exp){
                        factors[numfacs]=p;
                        exponents[numfacs]=exp;
                        ++numfacs;
                        exp=0;
                    }
                    ++b;
                    while(!sieve[b])
                        ++b;
                    p = b/5760*30030+conversions[b%5760];
                }
                if(p*p<=n)
                    buildSieveTo(sqrt(n));
            }
            if(p*p<=n){
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                factors[numfacs+1]=0;
                ++numfacs;
            }
            return numfacs;
        }
        unsigned char factorUnsafe(unsigned long long n, unsigned long long* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned int b = 1;
            unsigned long long p = 17;
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                ++numfacs;
            }
            return numfacs;
        }
        unsigned char factorWheel(unsigned long long n, unsigned long long* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned long long p=17;
            unsigned short w=1;
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                ++numfacs;
            }
            return numfacs;
        }
        unsigned char factorSmall(unsigned int n, unsigned int* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned int b = 1;
            unsigned int p = 17;
            if(maxsieve*maxsieve<n){
                while(b<maxbool && p*p<=n){
                    while(!(n%p)){
                        ++exp;
                        n/=p;
                    }
                    if(exp){
                        factors[numfacs]=p;
                        exponents[numfacs]=exp;
                        ++numfacs;
                        exp=0;
                    }
                    ++b;
                    while(!sieve[b])
                        ++b;
                    p = b/5760*30030+conversions[b%5760];
                }
                if(p*p<=n)
                    doSieve(sqrt(n));
            }
            if(p*p<=n){
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                ++numfacs;
            }
            return numfacs;
        }
        unsigned char factorSmallUnsafe(unsigned int n, unsigned int* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned int b = 1;
            unsigned int p = 17;
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                ++numfacs;
            }
            return numfacs;
        }
        unsigned char factorSmallWheel(unsigned int n, unsigned int* factors, unsigned char* exponents){
            unsigned char exp=0;
            unsigned char numfacs=0;
            while(!(n&1U)){
                ++exp;
                n>>=1;
            }
            if(exp){
                factors[0]=2;
                exponents[0]=exp;
                numfacs=1;
                exp=0;
            }
            while(!(n%3)){
                ++exp;
                n/=3;
            }
            if(exp){
                factors[numfacs]=3;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%5)){
                ++exp;
                n/=5;
            }
            if(exp){
                factors[numfacs]=5;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%7)){
                ++exp;
                n/=7;
            }
            if(exp){
                factors[numfacs]=7;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%11)){
                ++exp;
                n/=11;
            }
            if(exp){
                factors[numfacs]=11;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            while(!(n%13)){
                ++exp;
                n/=13;
            }
            if(exp){
                factors[numfacs]=13;
                exponents[numfacs]=exp;
                ++numfacs;
                exp=0;
            }
            unsigned int p=17;
            unsigned short w=1;
            while(p*p<=n){
                while(!(n%p)){
                    ++exp;
                    n/=p;
                }
                if(exp){
                    factors[numfacs]=p;
                    exponents[numfacs]=exp;
                    ++numfacs;
                    exp=0;
                }
                p+=wheel[w];
                ++w;
                w%=5760;
            }
            if(n>1){
                factors[numfacs]=n;
                exponents[numfacs]=1;
                ++numfacs;
            }
            return numfacs;
        }

        unsigned long long primesUpTo(unsigned long long n, std::vector<unsigned long long>& primes){
            buildSieveTo(n);
            if(n>=60184)
                primes.reserve((unsigned long long)(n/(log(n)-1.1)));
            else if(n>=17)
                primes.reserve((unsigned int)(1.25506*n/log(n)));
            else
                primes.reserve(6);
            unsigned long long t=0;
            if(n>=2){
                primes[0]=2;
                t=1;
            }
            if(n>=3){
                primes[1]=3;
                t=2;
            }
            if(n>=5){
                primes[2]=5;
                t=3;
            }
            if(n>=7){
                primes[3]=7;
                t=4;
            }
            if(n>=11){
                primes[4]=11;
                t=5;
            }
            if(n>=13){
                primes[5]=13;
                t=6;
            }
            if(n>=17){
                unsigned long long b=1;
                unsigned long long p=17;
                if(n<=4289995710U){
                    while(p<=n){
                        primes[t]=p;
                        ++t;
                        ++b;
                        while(!sieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760];
                    }
                }else{
                    while(b<822856320){
                        primes[t]=p;
                        ++t;
                        ++b;
                        while(!sieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760];
                    }
                    b=3;
                    p=4289995733U;
                    while(p<=n){
                        primes[t]=p;
                        ++t;
                        ++b;
                        while(!bigsieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760]+4289995710U;
                    }
                }
            }
            return t;
        }
        unsigned int primesUpToSmall(unsigned int n, std::vector<unsigned int>& primes){
            if(n>=60184){
                primes.reserve((unsigned int)(n/(log(n)-1.1)));
                doSieve(n);
            }else if(n>=17){
                primes.reserve((unsigned int)(1.25506*n/log(n)));
                doSieve(n);
            }else
                primes.reserve(6);
            unsigned int t=0;
            if(n>=2){
                primes[0]=2;
                t=1;
            }
            if(n>=3){
                primes[1]=3;
                t=2;
            }
            if(n>=5){
                primes[2]=5;
                t=3;
            }
            if(n>=7){
                primes[3]=7;
                t=4;
            }
            if(n>=11){
                primes[4]=11;
                t=5;
            }
            if(n>=13){
                primes[5]=13;
                t=6;
            }
            if(n>=17){
                unsigned int b=1;
                unsigned int p=17;
                while(p<=n){
                    primes[t]=p;
                    ++t;
                    ++b;
                    while(!sieve[b])
                        ++b;
                    p=b/5760*30030+conversions[b%5760];
                }
            }
            return t;
        }

        unsigned long long numPrimesUpTo(unsigned long long n){
            buildSieveTo(n);
            if(n>=17){
                unsigned long long t=6;
                unsigned long long b=1;
                unsigned long long maxb=n/1001*192; // this can put maxb up to 2 greater than where it should be
                if(n%30030){
                    if(maxb/5760*30030+conversions[maxb%5760]>n) // maxb is at most 1 greater than where it should be
                        --maxb;
                    while(maxb/5760*30030+conversions[maxb%5760]<n) // force maxb up to equal (prime) or 1 greater (composite) than where it should be
                        ++maxb;
                    if(maxb/5760*30030+conversions[maxb%5760]>n) // put maxb where it should be
                        --maxb;
                }else if(maxb)
                    --maxb;
                if(n<=4289995710U){
                    while(b<=maxb){
                        ++t;
                        ++b;
                        while(!sieve[b])
                            ++b;
                    }
                }else{
                    t=203056267;
                    b=3;
                    maxb-=822856320;
                    while(b<=maxb){
                        ++t;
                        ++b;
                        while(!bigsieve[b])
                            ++b;
                    }
                }
                return t;
            }else if(n>=13)
                return 6;
            else if(n>=11)
                return 5;
            else if(n>=7)
                return 4;
            else if(n>=5)
                return 3;
            else if(n>=3)
                return 2;
            else if(n>=2)
                return 1;
            else
                return 0;
        }

        unsigned int numPrimesUpToSmall(unsigned int n){
            doSieve(n);
            if(n>=17){
                unsigned int t=6;
                unsigned int b=1;
                unsigned int maxb=n/1001*192; // this can put maxb up to 2 greater than where it should be
                if(n%30030){
                    if(maxb/5760*30030+conversions[maxb%5760]>n) // maxb is at most 1 greater than where it should be
                        --maxb;
                    while(maxb/5760*30030+conversions[maxb%5760]<n) // force maxb up to equal (prime) or 1 greater (composite) than where it should be
                        ++maxb;
                    if(maxb/5760*30030+conversions[maxb%5760]>n) // put maxb where it should be
                        --maxb;
                }else if(maxb)
                    --maxb;
                if(n<=4289995710U){
                    while(b<=maxb){
                        ++t;
                        ++b;
                        while(!sieve[b])
                            ++b;
                    }
                }else{
                    t=203056267;
                    b=3;
                    maxb-=822856320;
                    while(b<=maxb){
                        ++t;
                        ++b;
                        while(!bigsieve[b])
                            ++b;
                    }
                }
                return t;
            }else if(n>=13)
                return 6;
            else if(n>=11)
                return 5;
            else if(n>=7)
                return 4;
            else if(n>=5)
                return 3;
            else if(n>=3)
                return 2;
            else if(n>=2)
                return 1;
            else
                return 0;
        }

        void firstPrimes(unsigned long long n, std::vector<unsigned long long>& primes){
            primes.reserve(n);
            buildSieveTo(n*log(n*log(n)));
            unsigned int t=0;
            if(n>=1){
                primes[0]=2;
                t=1;
            }
            if(n>=2){
                primes[1]=3;
                t=2;
            }
            if(n>=3){
                primes[2]=5;
                t=3;
            }
            if(n>=4){
                primes[3]=7;
                t=4;
            }
            if(n>=5){
                primes[4]=11;
                t=5;
            }
            if(n>=6){
                primes[5]=13;
                t=6;
            }
            if(n>=7){
                unsigned long long b=1;
                unsigned long long p=17;
                if(n<=203056267){
                    while(t<n){
                        primes[t]=p;
                        ++t;
                        if(t<n){
                            ++b;
                            while(!sieve[b])
                                ++b;
                            p=b/5760*30030+conversions[b%5760];
                        }
                    }
                }else{
                    while(b<822856320){
                        primes[t]=p;
                        ++t;
                        ++b;
                        while(!sieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760];
                    }
                    b=3;
                    p=4289995733U;
                    while(t<n){
                        primes[t]=p;
                        ++t;
                        ++b;
                        while(!bigsieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760]+4289995710U;
                    }
                }
            }
        }
        void firstPrimesSmall(unsigned int n, std::vector<unsigned int>& primes){
            primes.reserve(n);
            if(n<=194682290)
                doSieve((unsigned int)(n*log(n*log(n))));
            else
                doSieve(4289995710U);
            unsigned int t=0;
            if(n>=1){
                primes[0]=2;
                t=1;
            }
            if(n>=2){
                primes[1]=3;
                t=2;
            }
            if(n>=3){
                primes[2]=5;
                t=3;
            }
            if(n>=4){
                primes[3]=7;
                t=4;
            }
            if(n>=5){
                primes[4]=11;
                t=5;
            }
            if(n>=6){
                primes[5]=13;
                t=6;
            }
            if(n>=7){
                unsigned int b=1;
                unsigned int p=17;
                while(t<n){
                    primes[t]=p;
                    ++t;
                    if(t<n){
                        ++b;
                        while(!sieve[b])
                            ++b;
                        p=b/5760*30030+conversions[b%5760];
                    }
                }
            }
        }

    private:
        unsigned short conversions[5760];
        unsigned char wheel[5760];
        unsigned short indexes[30030] = {0};
        unsigned int maxsieve = 0;
        unsigned long long maxbigsieve = 4289995710U;
        unsigned int maxbool = 1;
        unsigned long long maxbigbool = 0;
        std::vector<bool> sieve;
        std::vector<bool> bigsieve;

        void doSieve(unsigned int n){
            if(n<=maxsieve)
                return;
            if(n%30030)
                n+=30030-n%30030; // Round up to nearest multiple of 2*3*5*7*11*13
            maxbool=n/1001*192;
            sieve.resize(maxbool+1,1);
            unsigned int sqrtn=sqrt(n);

            unsigned int b=1;
            unsigned int p=17;
            while(p<=sqrtn){
                // no need to sieve below p^2
                unsigned int mins=std::max(maxsieve,p*p);

                // s starts out at mins rounded down to a multiple of 30030*p, plus p,
                // and then hits all multiples of p not divisible by 2, 3, 5, 7, 11, or 13
                unsigned int s=mins-mins%(30030*p)+p;
                unsigned short w=0;
                if(s==p){
                    s+=16*p;
                    w=1;
                }
                while(s<mins){
                    s+=p*wheel[w];
                    ++w;
                }
                w%=5760;
                while(s<n){
                    sieve[s/30030*5760+indexes[s%30030]]=0;
                    s+=p*wheel[w];
                    ++w;
                    w%=5760;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            maxsieve = n;
        }
        void doBigSieve(unsigned long long n){
            if(n<=maxbigsieve)
                return;
            if(n%30030)
                n+=30030-n%30030; // Round up to nearest multiple of 2*3*5*7*11*13
            maxbigbool=n/1001*192-822856320;
            bigsieve.resize(maxbigbool+1,1);
            unsigned int sqrtn=sqrt(n);

            unsigned long long b=1;
            unsigned int p = 17;
            while(p<=sqrtn){
                // no need to sieve below p^2
                unsigned long long mins=std::max(maxbigsieve,(unsigned long long)p*p);

                // s starts out at mins rounded down to a multiple of 30030*p, plus p,
                // and then hits all multiples of p not divisible by 2, 3, 5, 7, 11, or 13
                unsigned long long s=mins-mins%(30030*p)+p;
                unsigned short w=0;
                while(s<mins){
                    s+=p*wheel[w];
                    ++w;
                }
                w%=5760;

                // Using this block instead of the block directly above should optimize getting s above mins,
                // because it is bidrectional, cutting the time in half.
                // However, testing revealed no difference, and if anything, a slight increase.
/*                unsigned short w;
                unsigned long long multp = 30030*p;
                unsigned long long testhalf = mins%(multp);
                unsigned long long s=mins-testhalf;
                if(testhalf*2<multp){
                    s+=p;
                    w=0;
                    while(s<mins){
                        s+=p*wheel[w];
                        ++w;
                    }
                }else{
                    s+=multp-p;
                    w=5759;
                    while(s>=mins){
                        --w;
                        s-=p*wheel[w];
                    }
                    s+=p*wheel[w];
                    ++w;
                }*/

                while(s<n){
                    bigsieve[s/30030*5760+indexes[s%30030]-822856320]=0;
                    s+=p*wheel[w];
                    ++w;
                    w%=5760;
                }
                ++b;
                while(!sieve[b])
                    ++b;
                p = b/5760*30030+conversions[b%5760];
            }
            maxbigsieve = n;
        }
};
