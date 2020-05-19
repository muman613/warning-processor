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

using string_vec = std::vector<std::string>;

bool get_warning_type(const string & desc, string & type);

struct inputParms {
    string inputFilename{};
    string outputFilename{};
    bool verbose{false};
};

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
        }
    }

    if (!parms.inputFilename.empty()) {
        bRes = true;
    }

    return bRes;
}

/**
 * Load contents of text file into vector of strings...
 *
 * @param filename
 * @param contents
 * @return
 */
bool load_contents(string filename, string_vec & contents) {
    ifstream ifp(filename.c_str());

    if (!ifp) {
        cerr << "Error opening file..." << endl;
        return false;
    }
    string line;
    while (getline(ifp, line)) {
        contents.push_back(line);
    }

    cout << "Read " << contents.size() << " lines..." << endl;

    return (!contents.empty());
}

/**
 * Extract warnings from string vector using regular expression parsing...
 *
 * @param input
 * @param output
 * @return
 */
bool extract_warnings(string_vec &input, string_vec &output) {
    bool bRes = false;

    for (auto line : input) {
        if (line.find(": warning: ") != std::string::npos) {
            output.push_back(line);
        }
    }

    cout << "Found " << output.size() << " warning lines..." << endl;

    return (!output.empty());
}

bool get_warning_type(const string & desc, string & type) {
    regex matchtype("\\[-W([\\w-]+)]");
    smatch matches;

    if (regex_search(desc, matches, matchtype)) {
        type = matches[1];
    } else {
        type = "undefined";
    }

    return true;
}

bool add_warning_to_file(userfile_vec & vec, string & filename, string & linespec, string & desc) {
    string warning_type;

    get_warning_type(desc, warning_type);

    for (auto & fileObj : vec) {
        if (fileObj.filePath == filename) {
            fileObj.add_warning(warning(linespec, desc));
            return true;
        }
    }
    userFile newFile(filename);
    newFile.add_warning(warning(linespec, desc));
    vec.push_back(newFile);
    return true;
}

/**
 * Display the userFile vector contents...
 *
 * @param vec
 */
void display_stats(userfile_vec & vec, ostream & os = cout) {
    int total_warnings{0};

    os << "Dump of warnings found:" << endl;

    for (const auto & thisFile : vec) {
        os << "Filename      : " << thisFile.filePath << endl;
        os << "Warning Types : " << thisFile.warningmap.size() << endl;

        for (auto warn_iter = thisFile.warningmap.begin() ; warn_iter != thisFile.warningmap.end() ; warn_iter++) {
            os << "\tType : " << warn_iter->first << "\t" << warn_iter->second.size() << endl;
            total_warnings += warn_iter->second.size();
            for (auto this_warning : warn_iter->second) {
                os << "\t\t @ " << this_warning.linespec << " : " << this_warning.type << endl;
            }
        }
    }
    os << "Total warnings found : " << total_warnings << endl;
}

int main(int argc, char * argv[]) {
    inputParms      parms;
    userfile_vec    fileVec;

    parms.outputFilename = "report.txt"; // set default output filename

    if (!parse_args(argc, argv, parms)) {
        cerr << "Invalid arguments..." << endl;
        return 10;
    }

    std::cout << "Parsing input file : " << parms.inputFilename << std::endl;

    if (filesystem::exists(parms.inputFilename)) {
        string_vec inputFileContents;

        cout << "Found input file, processing..." << endl;

        if (load_contents(parms.inputFilename, inputFileContents)) {
            string_vec warningLines;
            regex matchline("([a-zA-Z\\/0-9-_\\.]+):(\\d+:\\d+): warning: (.*)");

            cout << "Extracting warnings from input file..." << endl;
            if (extract_warnings(inputFileContents, warningLines)) {
                for (auto line : warningLines) {
                    smatch matches;

                    if (regex_search(line, matches, matchline)) {
                        string  filename,
                                linespec,
                                warning;
                        filename = matches[1];
                        linespec = matches[2];
                        warning  = matches[3];

                        if (!add_warning_to_file(fileVec, filename, linespec, warning)) {
                            cerr << "Warning : problem adding warning to file!" << endl;
                        }
                    }
                }
            }
            cout << "File processing complete..." << endl;

            std::sort(fileVec.begin(), fileVec.end(), [=](const userFile & a, const userFile & b) {
                return (a.filePath < b.filePath);
            });

            ofstream output_file(parms.outputFilename.c_str());
            display_stats(fileVec, output_file);
            output_file.close();
        }
    } else {
        cerr << "Unable to open input file \"" << parms.inputFilename << "\"." << endl;
    }

    return 0;
}
//([a-zA-Z\/0-9-_\.]+):(\d+:\d+): warning: (.*)
//\[-W([\w-]+)]
