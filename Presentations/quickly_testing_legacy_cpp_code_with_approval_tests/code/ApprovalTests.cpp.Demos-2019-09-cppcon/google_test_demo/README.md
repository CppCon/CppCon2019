# google_test_demo/

Some example uses of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp) with the [Google Test framework](https://github.com/google/googletest), with detailed explanatory comments.

* [main.cpp](main.cpp) - Shows how to set up your `main()`
* [Test01.cpp](Test01.cpp) - This is an ordinary Google Test test
* [Test02.cpp](Test02.cpp) - Simplest possible Approval Tests example
* [Test03CustomReporters.cpp](Test03CustomReporters.cpp) - Demos some of the supplied Reporter classes, which act on Approval Test failures
* [Test04ConsoleReporter.cpp](Test04ConsoleReporter.cpp) - Demos a custom Reporter, that may be useful when running on Build Servers
* [Test05VerifyAll.cpp](Test05VerifyAll.cpp) - Demos the various overloads of `Approvals::verifyAll()`
* [Test06VerifyAllCombinations.cpp](Test06VerifyAllCombinations.cpp) - Demos the various overloads of `CombinationApprovals::verifyAllCombinations()`
* [Test07TestScenarios.cpp](Test07TestScenarios.cpp) - Demos:
    * use with legacy code
    * logging that includes dates and times
    * and a simulated QImage example, showing how writing of non-text files may be handled.
