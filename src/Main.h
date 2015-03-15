#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::Main
    @brief fbxc main class
*/
#include <string>
#include "FBX.h"

namespace FBXC {
class Main {
public:
    /// setup from cmd line args
    Main(int argc, const char** argv);
    /// run the app
    void Run();
    
private:
    /// parse cmd line args
    void ParseArgs(int argc, const char** argv);
    /// check args, set error message on error
    void ValidateArgs();
    /// show version
    void ShowVersion();
    /// display help
    void ShowHelp();

    bool showHelp = false;
    bool showVersion = false;
    bool dumpFbx = false;
    std::string fbxPath;
    std::string rulesPath;
    std::string outputPath;
    FBX fbx;
};
} // namespace FBXC
