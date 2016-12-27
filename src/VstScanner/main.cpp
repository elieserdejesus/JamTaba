#include "VstPluginScanner.h"
#include "vst/VstHost.h"

int main(int argc, char *argv[])
{
    VstPluginScanner scanner;
    scanner.start(argc, argv);
    return 0;
}
