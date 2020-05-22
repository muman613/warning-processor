//
// Created by muman on 5/19/2020.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>
#include <regex>

#include "warnings.h"

using namespace std;
using namespace experimental;

extern bool get_warning_type(const std::string & desc, std::string & type);


warning::warning(std::string &_linespec, std::string &_type)
    : linespec(_linespec), type(_type)
{

}
warning::warning(const warning &copy)
    : linespec(copy.linespec), type(copy.type)
{

}

std::string warning::gettype() const
{
    return type;
}

std::string warning::getspec() const
{
    return linespec;
}

std::ostream &operator<<(ostream &out, const warning &w) {
    out << "warning(" << w.linespec << ", '" << w.type << "')";
    return out;
}

//-----------------------------------------------------------------------------

userFile::userFile(std::string &_filepath)
    : filePath(_filepath), warningmap{}
{

}

/**
 * Add a warning to a file...
 *
 * @param warn - warning object to add to file.
 */
void userFile::add_warning(const warning & warn) {
    cout << "adding warning " << warn << endl;

    std::string warning_type;
    warning_processor::get_warning_type(warn.gettype(), warning_type);
    for (auto w : warningmap[warning_type]) {
        if (warn.getspec() == w.getspec()) {
            // the warning is already in the vector
            return;
        }
    }
    // add the warning to the map.
    warningmap[warning_type].push_back(warn);
}

warning_processor::warning_processor() {
}

warning_processor::warning_processor(inputParms & inpParameters)
    : parms(inpParameters)
{

}

warning_processor::~warning_processor()  {

}



/**
 * Load contents of text file into vector of strings...
 *
 * @param filename
 * @param contents
 * @return
 */
bool warning_processor::load_contents() {
    ifstream  ifp(parms.inputFilename.c_str());

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
bool warning_processor::extract_warnings(string_vec &input, string_vec &output) {
    for (const auto & line : input) {
        if (line.find(": warning: ") != std::string::npos) {
            output.push_back(line);
        }
    }

    cout << "Found " << output.size() << " warning lines..." << endl;

    return (!output.empty());
}

bool warning_processor::get_warning_type(const string & desc, string & type) {
    regex matchtype("\\[-W([\\w-]+)]");
    smatch matches;

    if (regex_search(desc, matches, matchtype)) {
        type = matches[1];
    } else {
        type = "undefined";
    }

    return true;
}

bool warning_processor::add_warning_to_file(userfile_vec & vec, string & filename, string & linespec, string & desc) {

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
void warning_processor::display_stats(userfile_vec & vec, ostream & os) {
    int total_warnings{0};

    os << "Dump of warnings found:" << endl;

    for (const auto & thisFile : vec) {
        os << "Filename      : " << thisFile.filePath << endl;
        os << "Warning Types : " << thisFile.warningmap.size() << endl;

        for (const auto & warn_iter : thisFile.warningmap) {
            os << "\tType : " << warn_iter.first << "\t" << warn_iter.second.size() << endl;
            total_warnings += warn_iter.second.size();
            for (const auto& this_warning : warn_iter.second) {
                os << "\t\t @ " << this_warning.getspec() << " : " << this_warning.gettype() << endl;
            }
        }
    }
    os << "Total warnings found : " << total_warnings << endl;
}

/**
 * Generate warning report using parameters passed in through inputParms.
 *
 * @param parms - reference to inputParms structure.
 * @return integer result code.
 */
int warning_processor::generate_warning_report() {
    userfile_vec    fileVec;
    int             nResult{0};

    std::cout << "Parsing input file : " << parms.inputFilename << std::endl;

    if (filesystem::exists(parms.inputFilename)) {
        string_vec inputFileContents;

        cout << "Found input file, processing..." << endl;

        if (load_contents()) {
            string_vec warningLines;
            regex matchline(R"(([a-zA-Z\/0-9-_\.]+):(\d+:\d+): warning: (.*))");

            cout << "Extracting warnings from input file..." << endl;
            if (extract_warnings(inputFileContents, warningLines)) {
                for (auto const & line : warningLines) {
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
        nResult = 10;
    }

    return nResult;
}
