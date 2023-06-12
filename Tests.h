#ifndef TESTS_H
#define TESTS_H
#include <vector>

struct TestData
{
    std::vector<double> TestVector;
};

// These are four test cases for the vector operations algorithm.
// Each has a hard-coded vector.  The
// cooresponding object is then built, and the vector operations
// computation is invoked.  The results are tested with explicit code.

void test1();
void test2();
void test3();
void test4();
void test5();
#endif
