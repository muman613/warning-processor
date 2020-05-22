//
// Created by developer on 5/19/20.
//

#ifndef WARNING_PROCESSOR_WARNINGS_H
#define WARNING_PROCESSOR_WARNINGS_H

#include <string>
#include <vector>
#include <map>

using string_vec = std::vector<std::string>;

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

struct inputParms {
    std::string inputFilename{};
    std::string outputFilename{};
    bool verbose{false};
};

using userfile_vec = std::vector<userFile>;

class warning_processor {
public:
    warning_processor();
    warning_processor(inputParms & inpParameters);

    ~warning_processor();

    int generate_warning_report();
    static bool get_warning_type(const std::string & desc, std::string & type);

protected:
    bool load_contents();
    bool extract_warnings(string_vec &input, string_vec &output);
    bool add_warning_to_file(userfile_vec & vec, std::string & filename, std::string & linespec, std::string & desc);
    void display_stats(userfile_vec & vec, std::ostream & os = std::cout);

private:
    inputParms          parms;
    string_vec          contents;
};

#endif //WARNING_PROCESSOR_WARNINGS_H
