//
// Created by muman on 5/19/2020.
//
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "warnings.h"

using namespace std;

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
    get_warning_type(warn.gettype(), warning_type);
    for (auto w : warningmap[warning_type]) {
        if (warn.getspec() == w.getspec()) {
            // the warning is already in the vector
            return;
        }
    }
    // add the warning to the map.
    warningmap[warning_type].push_back(warn);
}

/*
class userFile {
public:
    userFile(std::string & _filepath) : filePath(_filepath), warningmap{} {};

    void add_warning(const warning & warn) {
        std::string warning_type;
        get_warning_type(warn.gettype(), warning_type);
        warningmap[warning_type].push_back(warn);
    }

//protected:
    std::string          filePath;
    warning_map     warningmap;
};
class warning {
public:
    warning(std::string & _linespec, std::string & _type) : linespec(_linespec), type(_type) {};
    warning(const warning & copy);

//protected:
    std::string linespec;
    std::string type;
};
 */