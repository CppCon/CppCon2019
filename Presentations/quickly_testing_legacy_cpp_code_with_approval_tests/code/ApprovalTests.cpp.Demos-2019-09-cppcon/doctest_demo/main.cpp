#define APPROVALS_DOCTEST
#include "ApprovalTests.hpp"

using namespace ApprovalTests;

// Put output files in a sub-directory
auto subDirectoryDisposer = Approvals::useApprovalsSubdirectory("approval_tests");

// Optionally, override the Reporter in all cases where code did not specify what to use:
// auto defaultReportDisposer = Approvals::useAsDefaultReporter( std::make_shared<Windows::AraxisMergeReporter>() );
