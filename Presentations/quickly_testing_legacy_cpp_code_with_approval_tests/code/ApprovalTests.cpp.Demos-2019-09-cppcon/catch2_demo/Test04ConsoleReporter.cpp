#include "ApprovalTests.hpp"
#include "Catch.hpp"

#include "include/ConsoleDiffReporter.h"

using namespace ApprovalTests;

TEST_CASE("UseConsoleReporter", "[!shouldfail]")
{
    // The intention of this is to show the text-based diff output next to the
    // output reporting the test failure.
    // However, catch2 separates the two, so that the diff output appears a long
    // way before the FAILED output.
    FirstWorkingReporter diff_reporter(
        {
            // On Windows, SystemLauncher does not yet know how to use the "where" command to find
            // the location of a program that is in the PATH, so we have to give the full
            // path to fc.exe here:
            new ConsoleDiffReporter("C:\\Windows\\System32\\fc.exe"),
            // On Linux and Mac, this one will be used:
            new ConsoleDiffReporter("diff")
        });
    Approvals::verify("Some\nMulti-line\noutput", diff_reporter);
}
