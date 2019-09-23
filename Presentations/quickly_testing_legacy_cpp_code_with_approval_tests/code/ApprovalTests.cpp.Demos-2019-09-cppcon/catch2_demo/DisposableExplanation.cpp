#include "ApprovalTests.hpp"
#include "Catch.hpp"

using namespace ApprovalTests;

TEST_CASE("DisposableExplanation")
{
    {
        auto disposer = Approvals::useAsDefaultReporter(
                std::make_shared<Mac::BeyondCompareReporter>());
        // Your tests here will use Mac::BeyondCompareReporter...

    } // as soon as the code passes this }, the disposer is destroyed
    // and Approvals reinstates the previous default reporter
}
