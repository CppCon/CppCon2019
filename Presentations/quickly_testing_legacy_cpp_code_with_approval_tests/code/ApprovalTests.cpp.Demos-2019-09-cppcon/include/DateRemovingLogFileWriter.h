#ifndef APPROVALTESTS_CPP_DATEREMOVINGLOGFILEWRITER_H
#define APPROVALTESTS_CPP_DATEREMOVINGLOGFILEWRITER_H

#include "ApprovalTests.hpp"
#include "FibonacciCalculator.h"

#include <regex>

using namespace ApprovalTests;

class DateRemovingLogFileWriter : public ApprovalWriter
{
public:
    explicit DateRemovingLogFileWriter(FibonacciCalculator& calculator) : calculator(calculator)
    {
    }

    std::string getFileExtensionWithDot() const override
    {
        return ".log";
    }

    void write(std::string path) const override
    {
        calculator.calculate(path);
        rewriteLogFileRemovingDatesAndTimes(path);
    }

    void cleanUpReceived(std::string receivedPath) const override
    {
        ::remove(receivedPath.c_str());
    }

private:
    void rewriteLogFileRemovingDatesAndTimes(const std::string& path) const
    {
        auto lines = readLines(path);
        stripDatesAndTimes(lines);
        writeLines(lines, path);
    }

    std::vector<std::string> readLines(const std::string& path ) const
    {
        std::ifstream infile(path);
        if( !infile.is_open() )
            throw std::domain_error( "Unable to load input file: " + path );

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(infile, line))
        {
            lines.emplace_back(line);
        }
        return lines;
    }

    std::string stripDateAndTime(const std::string& line) const
    {
        // Example date:
        // Tue Sep  3 16:58:52 2019
        const auto dateRegex = R"(([A-Za-z]{3}) ([A-Za-z]{3}) ([0-9 ]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) ([0-9]{4}))";
        const std::string replacementText = "[date-time-removed]";
        return std::regex_replace(line, std::regex(dateRegex), replacementText);
    }

    void stripDatesAndTimes(std::vector<std::string>& lines) const
    {
        for(auto& line : lines)
        {
            line = stripDateAndTime(line);
        }
    }

    void writeLines(const std::vector<std::string>& lines, const std::string& path ) const
    {
        std::ofstream outfile(path);
        if( !outfile.is_open() )
            throw std::domain_error( "Unable to re-write input file: " + path );

        for( const auto& line : lines)
        {
            outfile << line << '\n';
        }
    }

private:
    FibonacciCalculator& calculator;
};

#endif //APPROVALTESTS_CPP_DATEREMOVINGLOGFILEWRITER_H
