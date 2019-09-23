#ifndef CPPP2019_FOREGROUNDSYSTEMLAUNCHER_H
#define CPPP2019_FOREGROUNDSYSTEMLAUNCHER_H

#include "ApprovalTests.hpp"

using namespace ApprovalTests;

// Based on SystemLauncher, and differs in that it runs the command in
// the foreground instead of the background, so that any text output is
// interleaved in with the output from the test framework.
class ForegroundSystemLauncher : public CommandLauncher
{
public:
    bool launch(std::vector<std::string> argv) override
    {
        SystemLauncher temp_launcher;
        if (!temp_launcher.exists(argv.front()))
        {
            return false;
        }

        // Surround each of the arguments by double-quotes:
        const std::string command = std::accumulate(
            argv.begin(), argv.end(), std::string(""),
            [](std::string a, std::string b) {return a + " " + "\"" + b + "\""; });

        // See https://stackoverflow.com/a/9965141/104370 for why the Windows string is so complex:
        const std::string launch = SystemUtils::isWindowsOs() ?
                                   (std::string("cmd /S /C ") + "\"" + command + "\"") :
                                   (command);
        system(launch.c_str());
        return true;
    }
};

#endif //CPPP2019_FOREGROUNDSYSTEMLAUNCHER_H
