//------------------------------------------------------------------------------
//  Main.cc
//------------------------------------------------------------------------------
#include "Main.h"
#include <iostream>

namespace FBXC {

/// current version
const char* Version = "0.1";

//------------------------------------------------------------------------------
Main::Main(int argc, const char** argv) {
    this->ParseArgs(argc, argv);
}

//------------------------------------------------------------------------------
int
Main::Run() {
    int returnCode = 0;
    this->ValidateArgs();
    if (!this->error.empty()) {
        std::cerr << this->error << std::endl;
        this->showHelp = true;
        returnCode = 5;
    }
    if (this->showVersion) {
        this->ShowVersion();
    }
    else if (this->showHelp) {
        this->ShowHelp();
    }
    else {
        // FIXME
        returnCode = 10;
    }
    return returnCode;
}

//------------------------------------------------------------------------------
void
Main::ShowVersion() {
    std::cout << "fbxc " << Version << std::endl;
}

//------------------------------------------------------------------------------
void
Main::ShowHelp() {
    std::cout <<
        "fbxc [--version] [--help] [--fbx path] [--rules path] [--output path]\n"
        "source and docs: https://github.com/floooh/fbxc\n\n"
        "--version      show version information\n"
        "--help         show this help text\n"
        "--fbx          FBX file path (input)\n"
        "--rules        rules file path (input)\n"
        "--output       output file(s) path\n\n";
}

//------------------------------------------------------------------------------
void
Main::ParseArgs(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "--version") {
            this->showVersion = true;
        }
        else if (arg == "--help") {
            this->showHelp = true;
        }
        else if (arg == "--fbx") {
            if (++i < argc) {
                this->fbxPath = argv[i];
            }
            else {
                this->error = "expected fbx file path after '--fbx'";
            }
        }
        else if (arg == "--rules") {
            if (++i < argc) {
                this->rulesPath = argv[i];
            }
            else {
                this->error = "expected rules file path after '--rules'";
            }
        }
        else if (arg == "--output") {
            if (++i < argc) {
                this->outputPath = argv[i];
            }
            else {
                this->error = "expected output file path after '--output'";
            }
        }
        else {
            this->error = std::string("unknown cmdline arg: ") + argv[i];
        }
    }
}

//------------------------------------------------------------------------------
void
Main::ValidateArgs() {
    if (this->error.empty()) {
        if (!(this->showHelp || this->showVersion)) {
            if (this->fbxPath.empty()) {
                this->error = "--fbx arg missing";
            }
            else if (this->rulesPath.empty()) {
                this->error = "--rules arg missing";
            }
            else if (this->outputPath.empty()) {
                this->error = "--output arg missing";
            }
        }
    }
}

} // namespace FBXC

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    FBXC::Main app(argc, argv);
    return app.Run();
}

