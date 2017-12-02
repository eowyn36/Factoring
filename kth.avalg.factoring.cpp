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
#include <map>
#include <queue>
#include <algorithm>
#include <gmpxx.h>
#include "Matrix.cpp"

using std::cout;
using std::cin;
using std::vector;
using std::queue;
using std::string;
using std::map;

typedef std::vector<mpz_class> mpz_vector;

// TODO shouldnt it be 100?
// 175891579187581657617 is 68
const static unsigned MAX_DIGITS = 65;
const static unsigned B_SMOOTH = 40;
const static unsigned SIEVE_LENGTH = 100; //TODO how to calcualte sieve length ??

template<typename T>
void print(string title, vector<T> vector) {
    cout << title << ": " << std::endl;
    for (int i = 0; i < vector.size(); i++)
        cout << vector[i] << std::endl;
    cout << std::endl;
}

long gcd(long a, long b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Simple Prime check
bool isPrime (mpz_class p) {
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

void FermatsFactorizationStep(mpz_class N, queue<mpz_class> & queue){
    mpz_class NRoot = sqrt(N);
    long a, b;
    double bDouble;
    
    for(a = NRoot.get_si(); a < 10000; a++) {
        bDouble = sqrt(pow(a, 2) - N.get_si());
        
        b = (long)bDouble;
        if(bDouble != b) // b is not a perfect square
            continue;
        
        if( a == b || a == abs(b % N) ) // a != ±b mod N
            continue;
                        
        queue.push(gcd(a + b, N));
        queue.push(gcd(a - b, N));
        break;
    }
}

void FermatsFactorization(queue<mpz_class> & queue, vector<mpz_class> & primes){
    while (!queue.empty() ){
        mpz_class factor = queue.front();
        queue.pop();
        if( isPrime(factor) ){
            primes.push_back(factor);
        } else if(factor % 2 == 0 && isPrime(factor/2)){
            primes.push_back(2);
            primes.push_back(factor/2);
        } else {
            FermatsFactorizationStep(factor, queue);
        }
    }
}


/*** Quadratic Sieve ***/
// TODO is these requirements valid?
//Requirements
//1. The number is composite
//2. The number has no prime factors up to its logarithm base ten
//3. The number is not a power


// STEP 1
// Calculate B (smoothness bound) TODO
// Find all primes smaller than the bound (Sieve of Eratosthenes) DONE
// find legendre's for all the primes (mpz_legendre) DONE
// pick the ones that resulted 1 and add them to factor base DONE

// STEP 1.5?
// Construct the initial sieve, V DONE

// STEP 2
// solve for x^2 ≡ n (mod p)
// 2.1 simple approach: DONE
// 2.1.1 r = n (mod p) -> r + p -> check if its a square DONE
// 2.1.2 keep adding p until you find another square DONE
// 2.2 not-so-simple approach: Tonelli-Shanks algorithm

// STEP 3
// MAGIC

void getFactorBase(mpz_class N, vector<int> & factorBase){
    // Sieve of Eratosthenes
    long B = B_SMOOTH;
    vector<bool> sieve(B + 1, false);
    for (int i = 2; i <= B; i++) {
        for (int j = i+i; j <= B; j += i) {
            sieve[j] = true;
        }
    }
    
    // Pick the primes which legendre is 1 for the factor base
    // TODO not sure about this
    // TODO well this BREAKS stuff !!
    //if ( N % 2 == 1)
    //    factorBase.push_back(2);
    
    for (int i = 2; i <= B; i++) {
        if(sieve[i] == false && mpz_legendre(N.get_mpz_t(), mpz_class(i).get_mpz_t()) == 1 )
            factorBase.push_back(i);
    }
    
    print("Factor Base", factorBase);
}

void processSieve(vector<mpz_class> & V, map<long, vector<int>> & exponents, long firstIndex, int p){
    std::vector<int> temp;
    long Vi;
    for(long i = firstIndex; i < V.size(); i += p) {
        Vi = V[i].get_si();
        while(Vi % p == 0) {
            Vi /= p;
            V[i] = Vi;
            if(exponents.find(i) == exponents.end()) {
                temp.clear();
                temp.push_back(p);
                exponents.insert(std::pair<long, vector<int>>(i, temp));
            } else {
                temp = exponents.at(i);
                temp.push_back(p);
                exponents[i] = temp;
            }
        }
    }
}

void contructSieve(mpz_class N, vector<int> & factorBase){
    mpz_class NRoot = floor(sqrt(N.get_si()));
    vector<mpz_class> V;
    map<long, vector<int>> exponents;
    
    int sieveLenght = SIEVE_LENGTH;
    for(int x = 0; x < sieveLenght; x++) {
        V.push_back( pow(x + NRoot.get_si(), 2) - N ); // Q(x) = (x + √n)^2 − N
         /*
         x: 1 vi: 1    153 = 3^2 17
         x: 4 vi: 1
         x: 12 vi: 1
         x: 21 vi: 1
         x: 52 vi: 1
         */
    }
    
    for(int i = 0; i < factorBase.size(); i++) {
        int numberOfXs = 0, p = factorBase[i];
        mpz_class firstIndex, r = N % p;
        while(true) {
            if(numberOfXs == 2 || (numberOfXs == 1 && p == 2))
                break;
            
            // TODO no need to calculate for x2 since x2 = p - x1 
            if(mpz_perfect_square_p(r.get_mpz_t()) != 0) {
                numberOfXs++;
                firstIndex = (((sqrt(r) - NRoot) % p) + p) % p;
                cout << p << ": " << sqrt(r) << " findex: " << firstIndex << std::endl;
                processSieve(V, exponents, firstIndex.get_si(), p);
            }
            r +=p;
        }
    }
    
    for(int x = 0; x < V.size(); x++) {
        if(V[x] == 1)
            print(std::to_string(x), exponents.at(x));
    }
}
/*
int main()
{
    double M[4][3] = {
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 1, 0, 1 }
    };
    
    to_reduced_row_echelon_form(M);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 3; ++j)
            std::cout << M[i][j] << '\t';
        std::cout << "\n";
    }
    
    return EXIT_SUCCESS;
}*/

int main(int argc, const char * argv[]) {
    //FermatsFactorization
    queue<mpz_class> queue;
    vector<mpz_class> primes;
    
    //Quadratic Sieve
    vector<int> factorBase;
    
    //Common
    vector<mpz_class> Ns;
    string N;
    while (cin >> N)
        Ns.push_back(mpz_class(N, 10));
    
    for (int j = 0; j < Ns.size(); j++) {
        primes.clear();
        queue.empty();
        
        getFactorBase(Ns[j], factorBase);
        contructSieve(Ns[j], factorBase);
        
        if (mpz_sizeinbase(Ns[j].get_mpz_t(), 2) > MAX_DIGITS) {
            cout << "fail" << std::endl << std::endl;
            continue;
        }
        /*
        queue.push(Ns[j]);
        FermatsFactorization(queue, primes);

        std::sort(primes.begin(), primes.end());
        for (int i = 0; i < primes.size(); i++)
            cout << primes[i] << std::endl;
         */
        cout << std::endl;
    }
    return 0;
}

/*
 2: 1 1 index0 0
 3: 1 2 index0 1
 13: 8 5 index12 9
 17: 7 10 index1 4
 19: 5 14 index14 4
 29: 12 17 index7 12
 
 41: 16 25 index8 17
 43: 1 42 index7 5
 53: 15 38 index38 8
 59: 5 54 index5 54
 73: 3 70 index0 67
 79: 22 57 index43 78
 */
