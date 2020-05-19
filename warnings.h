//
// Created by developer on 5/19/20.
//

#ifndef WARNING_PROCESSOR_WARNINGS_H
#define WARNING_PROCESSOR_WARNINGS_H

#include <string>
#include <vector>
#include <map>

class warning {
public:
    warning(std::string & _linespec, std::string & _type) : linespec(_linespec), type(_type) {};
    warning(const warning & copy) = default;

//protected:
    std::string linespec;
    std::string type;
};

using warning_map = std::map<std::string, std::vector<warning>>;

extern bool get_warning_type(const std::string & desc, std::string & type);

class userFile {
public:
    userFile(std::string & _filepath) : filePath(_filepath), warningmap{} {};

    void add_warning(const warning & warn) {
        std::string warning_type;
        get_warning_type(warn.type, warning_type);
        warningmap[warning_type].push_back(warn);
    }

//protected:
    std::string          filePath;
    warning_map     warningmap;
};

using userfile_vec = std::vector<userFile>;


#endif //WARNING_PROCESSOR_WARNINGS_H
