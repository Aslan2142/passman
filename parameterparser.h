#ifndef PARAMETERPARSER_H
#define PARAMETERPARSER_H

#include <string>
#include <vector>

//Help to manage the program input parameters
class parameterparser
{
public:
    parameterparser(int argc, char *argv[]);
    bool has_parameter(std::string parameter_name, char parameter_abbr);
    std::string get_value(std::string parameter_name, char parameter_abbr);
protected:
    std::vector<std::string> arg_names;
    std::vector<std::string> arg_values;
    void parse(int argc, char *argv[]); //Parse the parameters and insert values into vectors of string
};

#endif // PARAMETERPARSER_H
