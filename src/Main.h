#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::Main
    @brief fbxc main class
*/
#include <string>

namespace FBXC {
class Main {
public:
    /// setup from cmd line args
    Main(int argc, const char** argv);
    /// run the app
    int Run();
    
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
    std::string error;
    std::string fbxPath;
    std::string rulesPath;
    std::string outputPath;
};
} // namespace FBXC
