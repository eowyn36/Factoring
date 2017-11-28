//
//  main.cpp
//  Factoring
//
//  Created by Nilay Karaoglu on 2017-11-26.
//  Copyright © 2017 Nilay Karaoglu. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>

using std::cout;
using std::cin;
using std::vector;
using std::queue;

const static unsigned MAX_DIGITS = 100;

unsigned getNumberOfDigits (unsigned i) {
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

long gcd(long a, long b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Simple Prime check
bool isPrime (long p) {
    if (p <= 1)
        return false;
    else if ( p <= 3)
        return true;
    else if (p % 2 == 0 || p % 3 == 0)
        return false;
    for (int i = 5; i <= sqrt(p); i += 6)
        if (p % i == 0 || p % (i + 2) == 0)
            return false;
    return true;
}

void FermatsFactorizationStep(long N, queue<long> & queue){
    int NRoot = ceil(sqrt(N)), a, b;
    double bDouble;
    
    for(a = NRoot; a < 10000; a++) {
        bDouble = sqrt(pow(a, 2) - N);
        
        b = (int)bDouble;
        if(bDouble != b) // b is not a perfect square
            continue;
        
        if( a == b || a == abs(b % N) ) // a != ±b mod N
            continue;
                        
        queue.push(gcd(a + b, N));
        queue.push(gcd(a - b, N));
        break;
    }
}

void FermatsFactorization(queue<long> & queue, vector<long> & primes){
    while (!queue.empty() ){
        long N = queue.front();
        queue.pop();
        if( isPrime(N) ){
            primes.push_back(N);
        } else if(N % 2 == 0 && isPrime(N/2)){
            primes.push_back(2);
            primes.push_back(N/2);
        } else {
            FermatsFactorizationStep(N, queue);
        }
    }
}

// STEP 1
// Calculate B (smoothness bound)
// Find all primes smaller than the bound (Sieve of Eratosthenes)
// find legendre's for all the primes (mpz_legendre)
// pick the ones that resulted 1 until and add them to factor base


// STEP 2
// solve for x^2 ≡ n (mod p)
// simple approach:
// r = n (mod p) -> r + p -> check if its a square
// keep adding p until you find another square (Why two x values ?? )
// not-so-simple approach: Tonelli-Shanks algorithm


// STEP 3
// MAGIC

int main(int argc, const char * argv[]) {
    queue<long> queue;
    vector<long> primes;
    vector<long long> Ns;
    long long N;

    //cout << std::numeric_limits<long long>::max() << std::endl;
    while (cin >> N)
        Ns.push_back(N);
    
    /*** Stupid fix for the bug that too big numbers cause ***/
    cin.clear();
    if(Ns.size() < 3)
        Ns.push_back(9223372036854775806);
    while (cin >> N)
        Ns.push_back(N);
    /*** -- ***/
    
    for (int j = 0; j < Ns.size(); j++) {
        primes.clear();
        queue.empty();
        N = Ns[j];
        
        //TODO check with maxsize
        if (N > 9223372036854775805) {
            cout << "fail" << std::endl << std::endl;
            continue;
        }

        queue.push(N);
        FermatsFactorization(queue, primes);
        
        /*** Quadric Sieve ***/
        // TODO is these requirements valid?
        //Requirements
        //1. The number is composite
        //2. The number has no prime factors up to its logarithm base ten
        //3. The number is not a power
        
        std::sort(primes.begin(), primes.end());
        for (int i = 0; i < primes.size(); i++)
            cout << primes[i] << std::endl;
        
        cout << std::endl;
    }
    return 0;
}
