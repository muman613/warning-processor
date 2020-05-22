#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <experimental/filesystem>
#include <regex>
#include "warnings.h"

using namespace std;
using namespace experimental;


/**
 * Parse arguments from commandline...
 *
 * @param argc - Count of arguments
 * @param argv - Array of pointers to argument strings.
 * @param parms
 * @return
 */
bool parse_args(int argc, char * argv[], inputParms & parms) {
    int opt;
    bool bRes = false;

    while ((opt = getopt(argc, argv, "i:o:v")) != -1) {
        switch (opt) {
            case 'i':
                parms.inputFilename = optarg;
                break;
            case 'o':
                parms.outputFilename = optarg;
                break;
            case 'v':
                parms.verbose = true;
                break;
            default:
                cerr << "Invalid parameter " << opt << endl;
                break;
        }
    }

    if (!parms.inputFilename.empty()) {
        bRes = true;
    }

    return bRes;
}

int main(int argc, char * argv[]) {
    inputParms      parms;

    parms.outputFilename = "report.txt"; // set default output filename

    if (!parse_args(argc, argv, parms)) {
        cerr << "Invalid arguments..." << endl;
        return 10;
    }

    int nResult{0};
    warning_processor proc(parms);

    if ((nResult = proc.generate_warning_report()) != 0) {
        switch (nResult) {
            case 10:
                cout << "Terminating due to nonexistent input file." << endl;
                break;
            default:
                break;
        }
    }
    return nResult;
}
//([a-zA-Z\/0-9-_\.]+):(\d+:\d+): warning: (.*)
//\[-W([\w-]+)]
