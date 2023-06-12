#include "VectorOperations.h"
#include "Tests.h"

int main() {
	// (Sanity Check) Vector of size 7 with positive and negative floating point of the same order of magnitude
	// Making sure all 14 Methods Pass it
	test1();
	// (Overflow Check) We consider a vector of size 20 with both positive and negative floating-point numbers 
	// in the range (-100, 100), excluding zeros. Due to the presence of both positive and negative numbers, 
	// using the logarithm method for multiplication is not feasible as log is undefined for non-positive numbers. 
	// As such, there's a potential for overflow or underflow when multiplying large or small numbers. 
	// To counter this, our check method uses std::assert() to compare the logarithms when dealing with large values
	// and directly compares the values for small ones. This is not entirely accurate due to potential precision loss 
	// when comparing logarithms, but without an assurance of only positive values, this is a pragmatic approach.
	test2();
	// (Overflew Check) A set with higher ratios between numbers
	// Vector of size 20 with positive and negative floating point in the range (-1200,1000) , excluding zeros.
	test3();
	// (Perfomance Check) Vector of size 30,000,000 to check which method is more suitable for a large data
	// This test can be used to determine the right algorithm in terms of performance for a given size of vector.
	// Overall reults: N =  1000 | 10000 | 100000 | 1000000 | 10000000 | 
	// Best Method (No) Sum : 1	 | 1or2	 | 1 or 2 | 2 or 11	|    11    |
	// Best Method (No)  *	: 4	 |   4	 |    4   |   12    |    12    |
	// Best Method (No)AdDif: 8	 |   14	 |    13  |   13    |    13    |
	test4();
	// (Summation accuracy check) 3 Vectors, one costume made with 11 memebers of very small numbers with many
	// significant digit, one randomized 1,000,0000 elements between (0,1), and one randomized 10,000,0000 elements between (-1,1)
	// the goal is to compare the results of three single thread summation methods (1,2,3)
	// Results of all three agree for the V(11), but for V(10e6) and V(10e7) Methods 1 and 2 agree, but Method 3 is a bit different 
	// which is the more acurrate version!
	test5();
}	
