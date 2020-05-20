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
    warning(std::string & _linespec, std::string & _type);
    warning(const warning & copy);

    [[nodiscard]] std::string gettype() const;
    [[nodiscard]] std::string getspec() const;

    friend std::ostream & operator <<(std::ostream & out, const warning & w);

protected:
    std::string linespec;
    std::string type;
};

using warning_map = std::map<std::string, std::vector<warning>>;

class userFile {
public:
    explicit userFile(std::string & _filepath);

    void add_warning(const warning & warn);

//protected:
    std::string     filePath;
    warning_map     warningmap;
};

using userfile_vec = std::vector<userFile>;


#endif //WARNING_PROCESSOR_WARNINGS_H
