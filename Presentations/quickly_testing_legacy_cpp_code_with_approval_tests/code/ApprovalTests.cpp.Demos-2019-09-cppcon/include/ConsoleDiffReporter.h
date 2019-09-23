#ifndef CPPP2019_CONSOLEDIFFREPORTER_H
#define CPPP2019_CONSOLEDIFFREPORTER_H

#include "ApprovalTests.hpp"

#include "ForegroundSystemLauncher.h"

using namespace ApprovalTests;

// Based on GenericDiffReporter, but using a different launcher
class ConsoleDiffReporter : public CommandReporter {
private:
    ForegroundSystemLauncher launcher;

public:
    explicit ConsoleDiffReporter(const std::string& program) : CommandReporter(program, &launcher) {};
    explicit ConsoleDiffReporter(const DiffInfo& info) : CommandReporter(info.getProgramForOs(), &launcher) {};
};

#endif //CPPP2019_CONSOLEDIFFREPORTER_H
