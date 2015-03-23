//------------------------------------------------------------------------------
//  Main.cc
//------------------------------------------------------------------------------
#include "Main.h"
#include "Log.h"
#include "cpptoml.h"
#include <iostream>

namespace FBXC {

/// current version
const char* Version = "0.1";

//------------------------------------------------------------------------------
Main::Main(int argc, const char** argv) {
    this->ParseArgs(argc, argv);
}

//------------------------------------------------------------------------------
void
Main::Run() {
    this->ValidateArgs();
    if (this->showVersion) {
        this->ShowVersion();
    }
    else if (this->showHelp) {
        this->ShowHelp();
    }
    else {
        this->fbx.Setup();
        this->fbx.Load(this->fbxPath);
        if (this->dumpFbx) {
            this->fbx.Dump();
        }
        this->fbx.Discard();
    }
}

//------------------------------------------------------------------------------
void
Main::ShowVersion() {
    Log::Info("fbxc %s\n", Version);
}

//------------------------------------------------------------------------------
void
Main::ShowHelp() {
    Log::Info(
        "fbxc [--version] [--help] [--fbx path] [--rules path] [--output path]\n"
        "source and docs: https://github.com/floooh/fbxc\n\n"
        "--version:         show version information\n"
        "--help:            show this help text\n"
        "--fbx path:        FBX file path (input)\n"
        "--rules path:      rules file path (input)\n"
        "--output path:     output file(s) path\n"
        "--fbx-dump:        dump FBX scene structure to stdout\n\n"
    );
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
                Log::Fatal("expected fbx file path after '--fbx\n");
            }
        }
        else if (arg == "--rules") {
            if (++i < argc) {
                this->rulesPath = argv[i];
            }
            else {
                Log::Fatal("expected rules file path after '--rules'\n");
            }
        }
        else if (arg == "--output") {
            if (++i < argc) {
                this->outputPath = argv[i];
            }
            else {
                Log::Fatal("expected output file path after '--output'\n");
            }
        }
        else if (arg == "--fbx-dump") {
            this->dumpFbx = true;
        }
        else {
            Log::Fatal("unknown cmdline arg: %s\n", argv[i]);
        }
    }
}

//------------------------------------------------------------------------------
void
Main::ValidateArgs() {
    if (!(this->showHelp || this->showVersion)) {
        if (this->fbxPath.empty()) {
            Log::Fatal("--fbx arg required\n");
        }
        else if (!this->dumpFbx && this->rulesPath.empty()) {
            Log::Fatal("--rules arg missing\n");
            
        }
        else if (!this->dumpFbx && this->outputPath.empty()) {
            Log::Fatal("--output arg missing\n");
        }
    }
}

} // namespace FBXC

