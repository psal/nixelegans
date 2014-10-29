//
//  sgNumUtils.h
//  sgGeneralUtilities
//
//  Created by Sean Grimes on 09/15/14.
//  Modified continuously
//  Last Modification: 10/28/14
//  Copyright (c) 2014 Grimes. All rights reserved.
//  Contact: spg63@cs.drexel.edu
//
//  You are free to use the code found in this file.
//  It's provided "as-is", no warranties, implicit or otherwise
//  Not distributed under any license, so do as you wish.

#ifndef sgGeneralUtilities_sgNumUtils_h
#define sgGeneralUtilities_sgNumUtils_h

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

/**
 * numUtils functions are placed in namespace numUtils to help make the function
 *      calls unambigious (to the writer/reader).
 * As with any namespace, you can use:
 *      numUtils::maxValue(T a, T b);
 */


// ----- Function Headers ------------------------------------------------------
// ----- Templated Functions for numerics (int, double, float) -----------------

// Count specific objects in a vector
template <typename T>
size_t numOccuranceInVec(std::vector<T> &vec, T &objToCount);

// Return the larger value
template <typename T>
T maxValue(T a, T b);

// Return the smaller value
template <typename T>
T minValue(T a, T b);

// Return the largest value in a vector
template <typename T>
T maxValueVec(const std::vector<T> &vec);

// Return the smallest value in a vector
template <typename T>
T minValueVec(const std::vector<T> &vec);

// Return the min and max elements of a vector
template <typename T>
std::vector<T> minMaxInVec(const std::vector<T> &vec, int begin, int end);

// Recursive replacement for pow()
template <typename T>
T fastPower(T x, T y);

// ----- End Templated Functions -----------------------------------------------

// Return an int from a compatible string
int strToInt(std::string &s);

// Return a double from a compatible string
double strToDouble(std::string &s);

// Recursively sum digits, to sum a string of digits, call strToDouble first
int sumOfDigits(int x);

// Recursively get Fibonacci numbers
int fibonacci(int x);

// --------------- End Function Headers ----------------------------------------
namespace numUtils{
    
    /**
     * Counts the number of times a specific object (number, string, char, user
     *   defined object) occures in a vector of that type of object.
     *
     * Objects passed by reference since some user objects can be quite large
     *
     * @param: vec - The vector to searched
     * @param: objToCount - The object to be searched for in the vector
     * @return: size_t - the number of times the object occures in the vector
     */
    template <typename T>
    size_t numOccuranceInVec(std::vector<T> &vec, T &objToCount){
        size_t total_objects = count(vec.begin(), vec.end(), objToCount);
        return total_objects;
    }
    
    /**
     * Returns the larger value, accepts ints, doubles, floats, chars
     * Returns -1 if the values are equal
     *
     * @param: a - The first value
     * @param: b - The second value
     * @return: T - The larger value
     */
    template <typename T>
    T maxValue(T a, T b){
        if(a > b)
            return a;
        else if (b > a)
            return b;
        else
            return -1;
    }
    
    /**
     * Returns the smaller value, accepts ints, doubles, floats, chars
     * Returns -1 if the values are equal
     *
     * @param: a - The first value
     * @param: b - The second value
     * @return: T - The larger value
     */
    template <typename T>
    T minValue(T a, T b){
        if (a < b)
            return a;
        else if (b < a)
            return b;
        else
            return -1;
    }
    
    /**
     * Returns the largest value in a vector of ints, doubles, floats, or chars
     *      Vector sequentially searched, requires n-1 comparisons
     *
     * @param: vec - the vector to be looked at
     * @return: T - the largest value
     * @return: -1 - If vector has 0 elements
     */
    template <typename T>
    T maxValueVec(std::vector<T> &vec){
        if(vec.size() == 1)
            return vec[0];
        else if(vec.size() == 0)
            return -1;
        else{
            int current_max = vec[0];
            for (int i=0; i < vec.size(); ++i){
                if (vec[i] > current_max)
                    current_max = vec[i];
            }
            return current_max;
        }
    }
    
    /**
     * Returns the smallest value in a vector of ints, doubles, floats, or chars
     *      Vector sequentially searched, requires n-1 comparisons
     *
     * @param: vec - the vector to be looked at
     * @return: T - the smallest value
     * @return: -1 - iIf vector has 0 elements
     */
    template <typename T>
    T minValueVec(std::vector<T> &vec){
        if(vec.size() == 1)
            return vec[0];
        else if(vec.size() == 0)
            return -1;
        else{
            int current_min = vec[0];
            for(int i = 0; i < vec.size(); ++i){
                if(vec[i] < current_min)
                    current_min = vec[i];
            }
            return current_min;
        }
    }
    
    /**
     * Divide and conquere approach to finding the min and max element in a
     *      vector
     *
     * @param: vec - an int, double, or float std::vector
     * @param: l - The starting range to search, typically 0
     * @param: u - The end range to search, typically v.size() - 1
     * @return: A vector of type 'vec', holding 2 entries:
     *      M[0] - holds the min value of vec
     *      M[1] - holds the max value of vec
     */
    template <typename T>
    std::vector<T> minMaxInVec(const std::vector<T> &vec, int begin, int end){
        int m;
        std::vector<T> M(2), M1(2), M2(2);
        if (end == begin){
            M[0] = vec[begin];  M[1] = vec[end];
            return M;
        }
        // two elements - only one comparison
        if (end - begin == 1){
            if (vec[begin] <= vec[end]){
                M[0] = vec[begin];  M[1] = vec[end];
            }
            else{
                M[0] = vec[end];
                M[1] = vec[begin];
            }
            return M;
        }
        m = (end + begin)/2;
        M1 = minMaxInVec(vec, begin, m);
        M2 = minMaxInVec(vec, m+1, end);
        
        M[0] = minValue(M1[0],M2[0]);
        M[1] = maxValue(M1[1],M2[1]);
        return M;
    }
    
    /**
     * Recursive replacement for pow(), faster for large exponents
     *
     * @param: x - int or double as the base
     * @param: n - The exponent
     * @return: double - The base raised to the exponent
     */
    template <typename T>
    double fastPower(T x, int n){
        if( x==0 || x==1 || n==1 ) // no calculation here
            return x;
        if( n==0 ) // no calculation here
            return 1;
        if( n < 0 ) // negative exponent means reciprocal
            return( 1.0 / fastPower( x, -n ));
        if ( n%2 == 0 ){ // n is even
            double temp = fastPower( x , n/2 );
            return temp * temp;
        }
        else // n is odd
            return x * fastPower( x, n-1 );
    }
    
    /**
     * Returns an int from a string, using stringstream
     *
     * @param: s - The string to be converted to an int
     * @return: int - The converted string
     */
    int strToInt(std::string &s){
        std::stringstream vert;
        int i;
        vert << s;
        vert >> i;
        return i;
    }
    
    /**
     * Returns a double from a string, using stringstream
     *
     * WARNING: May cause a rounding error, it shouldn't, but it might.
     *      Check with cout << setprecision(WHATEVER_YOU_NEED) << var;
     *
     * @param: s - The string to be converted to a double
     * @return: double - The converted string
     */
    
    double strToDouble(std::string &s){
        std::stringstream vert;
        double d;
        vert << s;
        vert >> d;
        return d;
    }
    
    /**
     * Recursively sum the digits of a number
     *
     * @param: x - int
     * @return: T - returns an int
     */
    int sumOfDigits(int x){
        if(x < 0)
            return -sumOfDigits(-x);
        if(x < 10)
            return x;
        return ( x % 10 + sumOfDigits( x/10 ));
    }
    
    /**
     * Recursively get fibonacci numbers based on inputted int
     *
     * @param: x - int
     * @return: int - returns the fib #
     */
    int fibonacci(int x){
        if(x <= 1)
            return x;
        return (fibonacci(x - 1) + fibonacci(x - 2));
    }
    
} //namespace numUtils



#endif














