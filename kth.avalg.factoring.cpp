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
    //cout << "in FermatsFactorizationStep \n";
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
        //cout << "Proccessing queue \n";
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

void getFactorBase(mpz_class N, long B, vector<int> & factorBase){
    // Sieve of Eratosthenes
    vector<bool> sieve(B + 1, false);
    for (int i = 2; i <= B; i++) {
        for (int j = i+i; j <= B; j += i) {
            sieve[j] = true;
        }
    }
    
    // Pick the primes which legendre is 1 for the factor base
    // TODO not sure about this
    if ( N % 2 == 1)
        factorBase.push_back(2);
    
    for (int i = 2; i <= B; i++) {
        if(sieve[i] == false && mpz_legendre(N.get_mpz_t(), mpz_class(i).get_mpz_t()) == 1 )
            factorBase.push_back(i);
    }
    
    print("Factor Base", factorBase);
}

void processSieve(vector<mpz_class> & V, map<long, vector<int>> & exponents, long firstIndex, int p){
    std::vector<int> temp;
    for(long i = firstIndex; i < V.size(); i += p) {
        V[i] /= p;
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

void contructSieve(mpz_class N, vector<int> & factorBase){
    mpz_class NRoot = ceil(sqrt(N.get_si()));
    vector<mpz_class> V;
    map<long, vector<int>> exponents;
    
    //TODO how to calcualte sieve length ??
    int sieveLenght = 100;
    for(int x = 0; x < sieveLenght; x++) {
        V.push_back( pow(x + NRoot.get_si(), 2) - N );
    }
    
    //Process once for prime 2
    processSieve(V, exponents, 1, 2);
    
    for(int i = 1; i < factorBase.size(); i++) {
        int numberOfXs = 0, p = factorBase[i];
        mpz_class firstIndex, r = N % p;
        while(true) {
            if(numberOfXs == 2)
                break;
            r +=p;
            if(mpz_perfect_square_p(r.get_mpz_t()) != 0) {
                numberOfXs++;
                firstIndex = (sqrt(r) - NRoot) % p;
                if(firstIndex < 0)
                    firstIndex += p;
                processSieve(V, exponents, firstIndex.get_si(), p);
            }
        }
    }
    
    for(int x = 0; x < V.size(); x++) {
        if(V[x] == 1)
            print(std::to_string(x), exponents.at(x));
    }
    //print("Vs", V);
}

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
        
        getFactorBase(Ns[j], 29, factorBase);
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
