#include "ApprovalTests.hpp"

#include "include/FibonacciCalculator.h"
#include "include/DateRemovingLogFileWriter.h"

#include <gtest/gtest.h>

#include <ostream>


using namespace ApprovalTests;

namespace
{
    class LegacyThing;
    std::ostream &operator<<(std::ostream &os, const LegacyThing &result);
    LegacyThing doLegacyOperation();

    class LegacyThing
    {
    };

    std::ostream &operator<<(std::ostream &os, const LegacyThing &result)
    {
        os << "Some complicated output\nfrom my legacy code\n";
        return os;
    }

    LegacyThing doLegacyOperation()
    {
        return LegacyThing{};
    }
}

TEST(Test07TestScenarios, New_test_of_legacy_feature)
{
    // Wrap up your legacy feature that needs testing in a function call
    // that returns some state that can be written to a text file
    const LegacyThing result = doLegacyOperation();
    Approvals::verify(result);
}

//--------------------------------------------------------------------------------

// Problem: Output contains data that changes each run, e.g. date-and-time

TEST(Test07TestScenarios, Deal_with_dates_and_times_in_output)
{
    // Could refactor the system being tested to allow use of fixed dates and times
    // but that risks breaking the system before you have tests. It would be
    // a useful later step.

    // Or could write a custom diff mechanism that recognises dates and times,
    // and ignores differences in them. Non-trivial.

    // Instead we will post-process our log files when running in test, and
    // strip out any dates and times, so that the files are expected to be
    // identical.

    FibonacciCalculator calculator(42);
    DateRemovingLogFileWriter datelessWriter(calculator);

    Approvals::verify(datelessWriter);
}

//--------------------------------------------------------------------------------

// Problem: Output is not suitable for writing to a text file

namespace
{
    // Simulation for a Qt's class for representing images:
    class QImage
    {
    public:
        bool save(std::string path) const
        {
            // Glossing over detail that the real QImage save method takes a QString
            return true;
        }
    };

    // An implementation of Approval's ApprovalWriter class, that takes
    // an image, and when needed, writes it out as a correctly-named .png file.
    class QImageWriter : public ApprovalWriter
    {
    public:
        explicit QImageWriter(const QImage& image) : image(image)
        {
        }

        std::string getFileExtensionWithDot() const override
        {
            return ".png";
        }

        void write(std::string path) const override
        {
            image.save(path);
        }

        void cleanUpReceived(std::string receivedPath) const override
        {
        }

    private:
        QImage image;
    };

    QImage getImage()
    {
        return QImage{};
    }
}

TEST(Test07TestScenarios, DISABLED_DUE_TO_INTENTIONAL_FAILURE_Test_an_image)
{
    // Approvals is based on saving text files

    // If we are testing a non-text type, we have some options.
    // 1. We could write a text representation of the image, e.g. in pseudo-code
    //      for each pixel:
    //          write xpos, ypos, redvalue, greenvalue, bluevalue
    //    This would work, but if we get a test failure, it would be hard for a user
    //    to interpret the results/
    // 2. We could write the image in a standard graphics format, and if we
    //    have a diffing tool that supports images, the results will be easy to 
    //    interpret.

    QImage image = getImage();
    QImageWriter imageWriter(image);

    // We're using QuietReporter here due to using dummy image-writing code.
    // In a real-world case, we'd use the standard DiffReporter{}, to
    // review the results interactively.
    Approvals::verify(imageWriter, QuietReporter{});
}
