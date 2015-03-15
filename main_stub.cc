// simple main stub for fbxc
#include "Main.h"

int main(int argc, const char** argv) {
    FBXC::Main app(argc, argv);
    app.Run();
    // NOTE: on fatal error, exit(10) will be called from Log::Fatal()
    return 0;
}

