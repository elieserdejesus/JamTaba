#include "BaseScanner.h"

#include "log/Logging.h"

#include <iostream>

BaseScanner::BaseScanner()
{
    //
}

BaseScanner::~BaseScanner()
{
    //
}

void BaseScanner::start(int argc, char *argv[])
{
    initialize(argc, argv);// get the folders to scan and black listed plugin paths
    scan();
}

void BaseScanner::writeToProcessOutput(const QString &string)
{
    // using '\n' here because std::endl don't work well when reading the output from QProcess
    std::cout << '\n' << string.toStdString() << '\n';
    std::flush(std::cout);// necessary to avoid split some outputed lines
}

