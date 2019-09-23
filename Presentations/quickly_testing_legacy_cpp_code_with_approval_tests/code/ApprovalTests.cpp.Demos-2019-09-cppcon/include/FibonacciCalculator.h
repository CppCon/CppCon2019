#ifndef APPROVALTESTS_CPP_FIBONACCICALCULATOR_H
#define APPROVALTESTS_CPP_FIBONACCICALCULATOR_H

#include <ctime>
#include <chrono>
#include <iostream>

class FibonacciCalculator
{
public:
    explicit FibonacciCalculator(unsigned value) : value(value)
    {
    }

    void calculate(const std::string& path)
    {
        std::ofstream stream(path);

        stream << "f(" << value << ") = " << fibonacci(value) << '\n';
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);

        stream << "finished computation at " << std::ctime(&end_time) << '\n';
    }

private:
    static long fibonacci(unsigned n)
    {
        if (n < 2) return n;
        return fibonacci(n-1) + fibonacci(n-2);
    }

    unsigned value;
};

#endif //APPROVALTESTS_CPP_FIBONACCICALCULATOR_H
