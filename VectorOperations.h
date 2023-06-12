#ifndef VECTOROPERATIONS_H
#define VECTOROPERATIONS_H

#include "Timer.h"
#include <vector>
#include <atomic>

class VectorOperationsBase {
protected:
    const std::vector<double> vec;
public:
    VectorOperationsBase(const std::vector<double>& vec) : vec(vec) {}
};

class SimpleVectorOperations : public VectorOperationsBase {
public:
    SimpleVectorOperations(const std::vector<double>& vec) : VectorOperationsBase(vec) {}

    double sum1(bool Time = true) const;
    double sum2(bool Time = true) const;
    double KahanSummation(bool Time = true) const;
    double product1(bool Time = true) const;
    double product2(bool Time = true) const;
    void adjacent_difference1(std::vector<double>& diff, bool Time = true) const;
    void adjacent_difference2(std::vector<double>& diff, bool Time = true) const;
    void adjacent_difference3(std::unique_ptr<double[]>& diff, bool Time = true) const;
};

class MultiThreadVectorOperations : public VectorOperationsBase {
private:
    std::atomic<double> sum{0.0};
public:
    MultiThreadVectorOperations(const std::vector<double>& vec) : VectorOperationsBase(vec) {}

    double ComputeSumMultiThreadSum1(bool Time = true);
    double ComputeSumMultiThreadSum2(bool Time = true);
    void CalculatePartialSum(unsigned int start, unsigned int end);
    double ComputeSumMultiThreadSum3(bool Time = true);
    double ComputeSumMultiThreadProduct(bool Time = true);
    void ComputeAdjDiffMultiThread1(std::vector<double>& diff, bool Time = true) const;
    void ComputeAdjDiffMultiThread2(std::vector<double>& diff, bool Time = true) const;
};

#endif
