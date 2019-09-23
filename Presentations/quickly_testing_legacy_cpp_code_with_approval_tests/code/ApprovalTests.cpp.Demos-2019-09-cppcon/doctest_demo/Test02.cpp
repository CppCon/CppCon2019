#include "ApprovalTests.hpp"
#include "doctest.h"

using namespace ApprovalTests;

// Approvals test - test static value, for demo purposes
TEST_CASE("TestFixedInput")
{
    Approvals::verify("Some\nMulti-line\noutput");
}

/*
 * Things to note here:
 * 
 * Running the tests
 * -----------------
 * 
 * The call to Approvals::verify() is the core of Approval testing.
 * Approvals code verifies lots of things.
 * 
 * The first time we run this test, the following things happen:
 * 1. Approvals writes the text "Some\nMulti-line\noutput" to a file
 *    called "Test02.TestFixedInput.received.txt" in the source directory.
 * 2. Approvals then looks in the same directory for a file called 
 *    "Test02.TestFixedInput.approved.txt", i.e. an already-approved output
 *    for this test,
 * 3. As that file doesn't exist yet, approvals provides a convenient way for
 *    the developer to create it. It looks for one of a long list of differencing
 *    tools it knows about, and when it finds an available tool, it opens
 *    the tool showing the "received" file on one side, and the (empty) "approved"
 *    one on the other.
 * 4. The developer then reviews the two files, and does one of two things:
 *    a) likes the content of the "received" file, so copies it over the
 *       "approved" one, and saves it, meaning that this is now the right content
 *    b) closes the differencing tool without saving any changes, and fixes their
 *       code.
 * 5. Once the developer has approved all outputs, she commits all the changed
 *    source files *and* all the approved files to version control.
 * 
 * On subsequent runs of the tests, any time the received and approved files
 * match, no differencing tool is shown, as no action is required from the 
 * developer.
 * 
 * The differencing tool is only shown if the received file differs from the
 * approved one, i.e. if action is required on the part of the developer.
 * 
 * Convenience
 * -----------
 * 
 * Approvals saves the developer from a whole bunch of work, by just having
 * sensible defaults
 * 
 * - No need to supply names of output files - sensible filenames are generated 
 *   automatically, based on the source code and test names
 * - No need to supply the name of a diff-ing tool (unless you do not have
 *   any of the ones supported by default)
 * - No need to delete the received files - they are deleted automatically
 *   after successful tests runs.
 *   
 * Verifications
 * -------------
 * 
 * Note that if you look at the statistics from your test framework, it
 * may appear that your Approvals tests do not add any new verifications
 * or assertions.
 * 
 * When an Approvals test fails, with catch as the testing framework, the output 
 * looks something like this:
 
    ...cppp2019/catch2_demo/test02.cpp(5): FAILED:
    due to unexpected exception with message:
      Failed Approval:
      Received does not match approved
      Received : ".../cppp2019/catch2_demo/Test02.TestFixedInput.received.txt"
      Approved : ".../cppp2019/catch2_demo/Test02.TestFixedInput.approved.txt"

    ===============================================================================
    test cases: 2 | 1 passed | 1 failed
    assertions: 3 | 2 passed | 1 failed

* This is because Approvals is silent if the file comparison passes, and throws
* an exception to indication either a missing approved file, or mismatch
* between received and approved files.
*/
