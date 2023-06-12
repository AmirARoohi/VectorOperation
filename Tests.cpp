#include "Tests.h"
#include "VectorOperations.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <random>
#include<iostream>
#include <iomanip>

// Create random real variable vector size N
std::vector<double> generate_random_vector(std::size_t size,double a=0.0,double b=1.0) {
    // Create a random device
    std::random_device rd;
    // Initialize a random number generator with the random device
    std::default_random_engine generator(rd());
    // Use a uniform_real_distribution to generate floating-point numbers between 0 and 1
    std::uniform_real_distribution<double> distribution(a,b);
    std::vector<double> vec(size);
    for (std::size_t i = 0; i < size; ++i) {
        vec[i] = distribution(generator);
    }
    return vec;
}

namespace
{
    // Returns whether two numbers are close within an epsilon.
    bool close(double a, double b, double epsilon = 1e-6)
    {
        double diff = std::fabs(a - b);

        if (diff < 1.0) {
            return diff < epsilon;
        }
        else {
            // Calculate the absolute values
            double absA = std::fabs(a);
            double absB = std::fabs(b);

            // Check if absB is not zero
            if (absB == 0) {
                throw std::invalid_argument("Division by zero in ratio calculation");
            }

            // Calculate the logarithms
            double logA = std::log(absA);
            double logB = std::log(absB);

            // Calculate the ratio
            double ratio = logA / logB;

            // Check if the ratio is close to 1
            return std::fabs(1.0 - ratio) < epsilon;
        }
    }

    // Returns whether the distance between two points is within an epsilon.
    bool AreVectorsEqual(const std::vector<double>& vec1, const std::vector<double>& vec2) {
        if (vec1.size() != vec2.size()) {
            return false;
        }

        for (std::size_t i = 0; i < vec1.size(); ++i) {
            if (!close(vec1[i], vec2[i])) {
                return false;
            }
        }

        return true;
    }

    bool AreVectorAndArrayEqual(const std::vector<double>& vec1, const std::unique_ptr<double[]>& vec2, std::size_t SizeofArray) {
        if (vec1.size() != SizeofArray) {
            return false;
        }

        for (std::size_t i = 0; i < vec1.size(); ++i) {
            if (!close(vec1[i], vec2[i])) {
                return false;
            }
        }

        return true;
    }
}

void PrintSimpleTimes(const SimpleVectorOperations& operations) {
    std::cout << "(Method 1) The time it takes for the single thread summation using += method:";
    std::cout << "Result of the summation:" << operations.sum1() << "\n";
    std::cout << "(Method 2) The time it takes for the single thread summation using (std::accumulate) method:";
    std::cout << "Result of the summation:" << operations.sum2() << "\n";
    std::cout << "(Method 3) The time it takes for the single thread summation using compensated summation method:";
    std::cout << "Result of the summation:" << operations.KahanSummation() << "\n";
    std::cout << "(Method 4) The time it takes for the single thread product using *= method:";
    std::cout << "Result of the product: " << operations.product1() << "\n";
    std::cout << "(Method 5) The time it takes for the single thread product using (std::accumulate) method:";
    std::cout << "Result of the product: " << operations.product2() << "\n";
}

void PrintSimpleTimesAdjDiff(const SimpleVectorOperations& operations, std::vector<double>& diff1, std::vector<double>& diff2, std::unique_ptr<double[]>& diff3) {
    std::cout << "(Method 6) The time it takes for adjacent difference in a vector:";
    operations.adjacent_difference1(diff1,true);
    std::cout << "(Method 7) The time it takes for adjacent difference in a vector using std::adjacent_difference:";
    operations.adjacent_difference2(diff2,true);
    std::cout << "(Method 8) The time it takes for adjacent difference using pointer to an array:";
    operations.adjacent_difference3(diff3,true);
}

void PrintThreadedTimes(MultiThreadVectorOperations & MToperations) {
    std::cout << "(Method 9) The time it takes for the multi thread summation using std::thread and lambda function:";
    std::cout << "Result of the summation:" << MToperations.ComputeSumMultiThreadSum1() << "\n";
    std::cout << "(Method 10) The time it takes for the multi thread summation using std::thread and a functor:";
    std::cout << "Result of the summation:" << MToperations.ComputeSumMultiThreadSum2() << "\n";
    std::cout << "(Method 11) The time it takes for the multi thread summation using std::async and std::atomic:";
    std::cout << "Result of the summation: " << MToperations.ComputeSumMultiThreadSum3() << "\n";
    std::cout << "(Method 12) The time it takes for the multi thread product using using std::async and std::mutex:";
    std::cout << "Result of the product: " << MToperations.ComputeSumMultiThreadProduct() << "\n";
}

void PrintThreadedTimesAdjDiff(MultiThreadVectorOperations& MToperations, std::vector<double>& diff4, std::vector<double>& diff5) {
    std::cout << "(Method 13) The time it takes for the multi thread adjacent difference using std::async:";
    MToperations.ComputeAdjDiffMultiThread1(diff4,true);
    std::cout << "(Method 14) The time it takes for the multi thread adjacent difference using OpenMP:";
    MToperations.ComputeAdjDiffMultiThread2(diff5,true) ;
}


void shiftAndPop(std::vector<double>& vec) {
    std::copy(vec.begin() + 1, vec.end(), vec.begin());
    vec.pop_back();
}

void ValidationProcedure(TestData& V, double& Sum, double& Product, bool Assert = true) {
    int SizeOfTheVector = V.TestVector.size();
    std::cout << "For the vector size " << SizeOfTheVector << "\n\n";

    SimpleVectorOperations operations(V.TestVector);
    PrintSimpleTimes(operations);
    if (Assert) {
        assert(close(operations.sum1(false), Sum));
        assert(close(operations.sum2(false), Sum));
        assert(close(operations.KahanSummation(false), Sum));
        assert(close(operations.product1(false), Product));
        assert(close(operations.product2(false), Product));
    }

    {
        std::vector<double> diff1, diff2;
        diff1.reserve(SizeOfTheVector - 1);
        diff2.reserve(SizeOfTheVector);
        std::unique_ptr<double[]> diff3(new double[SizeOfTheVector - 1]);

        PrintSimpleTimesAdjDiff(operations, diff1, diff2, diff3);

        if (Assert) {
            diff2.resize(SizeOfTheVector);
            shiftAndPop(diff2);
            assert(AreVectorsEqual(diff1, diff2));
            assert(AreVectorAndArrayEqual(diff2, diff3, SizeOfTheVector - 1));
        }
    }
    std::cout << "\n--------Multithread--------\n\n";

    MultiThreadVectorOperations MToperations(V.TestVector);
    PrintThreadedTimes(MToperations);

    if (Assert) {
        assert(close(MToperations.ComputeSumMultiThreadSum1(false), Sum));
        assert(close(MToperations.ComputeSumMultiThreadSum2(false), Sum));
        assert(close(MToperations.ComputeSumMultiThreadSum3(false), Sum));
        assert(close(MToperations.ComputeSumMultiThreadProduct(false), Product));
    }

    std::vector<double> diff4(SizeOfTheVector), diff5(SizeOfTheVector);
    PrintThreadedTimesAdjDiff(MToperations, diff4, diff5);

    if (Assert) {
        shiftAndPop(diff4);
        shiftAndPop(diff5);
        assert(AreVectorsEqual(diff4, diff5));
        // assert(AreVectorsEqual(diff4, diff2));
    }
}


void SumValidation(TestData &V, bool Assert = true) {
    int SizeOfTheVector = V.TestVector.size();
    std::cout << "For the vector size " << SizeOfTheVector << "\n\n";
    SimpleVectorOperations operations(V.TestVector);
    std::cout << "(Method 1) The time it takes for the single thread summation using += method:";
    std::cout << "Result of the summation:" << std::setprecision(std::numeric_limits<double>::max_digits10) << operations.sum1() << "\n";
    std::cout << "(Method 2) The time it takes for the single thread summation using (std::accumulate) method:";
    std::cout << "Result of the summation:" << std::setprecision(std::numeric_limits<double>::max_digits10) << operations.sum2() << "\n";
    std::cout << "(Method 3) The time it takes for the single thread summation using compensated summation method:";
    std::cout << "Result of the summation:" << std::setprecision(std::numeric_limits<double>::max_digits10) << operations.KahanSummation() << "\n\n";
    if (Assert) {
        assert(close(operations.sum1(false), operations.sum2(false)));
        assert(close(operations.sum2(false), operations.KahanSummation(false)));
        assert(close(operations.KahanSummation(false), operations.sum1(false)));

    }
}

void test1()
{
    std::cout << "\n---- Test 1 results: Given the vector V = {-17.3401, 2.01, -3.01, 4.10, -5.07 ,6.70,-7.01} ----\n " << std::endl;
    TestData V1 = {{-17.3401, 2.01, -3.01, 4.10, -5.07 ,6.70,-7.01}};
    double Sum1 = -19.6201;
    double Product1 = 102423.30544585084;
    ValidationProcedure(V1, Sum1, Product1);
}

void test2()
{
    std::cout << "\n---- Test 2 results: Given the vector V = { 91.27, -8.873, -36.07, -17.14, 45.57,...,-86.13} ----\n " << std::endl;
    TestData V2 = {{91.27, -8.873, -36.07, -17.14, 45.57, 52.20, -98.25, 78.38, -70.11, -79.78, 90.97, -88.55, -23.21, -50.60, 14.80, -67.67, -49.44, 98.45, -76.33, -86.13}};
    double Sum2 = -280.5130;
    double Product2 = -1.5553604175567974e+34;
    ValidationProcedure(V2, Sum2, Product2);
}

void test3()
{
    std::cout << "\n---- Test 3 results: Given the vector V =  { 1.27, -0.00873, -6.017, -1017.14, 0.59, 12.000105,...,-6.15} ----\n " << std::endl;
    TestData V3 = {{1.27, -0.00873, -6.017, -1017.14, 0.59, 12.000105, -0.1225, 781.308, -7.1, -9.3, -0.102, -0.515, -232.201, -510.660, 104.080, -607.657, -41.0404, 8.41, -7.38, -6.15}};
    double Sum3 = -1537.735525;
    double Product3 = 1.873628997950631e+19;
    ValidationProcedure(V3, Sum3, Product3);
}


void test4()
{
    std::cout << "\n---- Performance ---- Test 4 results: Random vector of size 30,000,000 with elements (0,1] ----\n" << std::endl;
    TestData V4;
    size_t TestVectorSize = 10000000;
    V4.TestVector = generate_random_vector(TestVectorSize);
    double Sum4 = 0.0; // irrelevent - No asseration is invoked 
    double Product4 = 0.0;
    ValidationProcedure(V4, Sum4, Product4, false);
}

void test5(){

    std::cout << "\n ---- Sum Accuracy ---- Test 5 results: Given the vector V = {-1.5553604175567974e-14, 0.00002763,...,31.45765e-12}----\n" << std::endl;
    TestData V5 = { {-1.5553604175567974e-14, 0.00002763,102423.30544585084e-18,0.12390273,198253.7893e-19, 1.098e-13, 60058692.02e-23,-1290.34526e-17,+1.555567974e-14,13.20e-10,31.45765e-12} };
    SumValidation(V5, true);
    std::cout << "\n ---- Sum Accuracy ---- Test 5 results: Random vector of size 30,000,000 with elements (-1,1) ----\n" << std::endl;
    TestData V6;
    size_t TestVectorSize = 1000000;
    V6.TestVector = generate_random_vector(TestVectorSize,-1.0,1.0);
    SumValidation(V6, true);
    TestData V7;
    TestVectorSize = 10000000;
    V7.TestVector = generate_random_vector(TestVectorSize, -1.0, 1.0);
    SumValidation(V7, true);
}