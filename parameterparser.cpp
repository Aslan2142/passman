#include "parameterparser.h"
#include <iostream>
parameterparser::parameterparser(int argc, char *argv[])
{
    parse(argc, argv);
}

void parameterparser::parse(int argc, char *argv[])
{
    arg_names.emplace_back("program");
    arg_values.emplace_back("");

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        {
            //Parameter ex.(--parameter)
            arg_names.emplace_back(argv[i]);
            arg_values.emplace_back("");
        } else if (argv[i][0] == '-') {
            //Parameter ex.(-p)
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                arg_names.emplace_back(std::string(1, argv[i][j]));
                arg_values.emplace_back("");
            }
        } else {
            //Parameter Value
            arg_values[arg_values.size() - 1] = argv[i];
        }
    }
}


bool parameterparser::has_parameter(std::string parameter_name, char parameter_abbr)
{
    return get_value(parameter_name, parameter_abbr) == "-" ? false : true;
}

std::string parameterparser::get_value(std::string parameter_name, char parameter_abbr)
{
    parameter_name = "--" + parameter_name;

    for (unsigned long i = 0; i < arg_names.size(); i++)
    {
        if (arg_names[i].compare(parameter_name) == 0)
        {
            return arg_values[i];
        }
        if (arg_names[i].compare(std::string(1, parameter_abbr)) == 0)
        {
            return arg_values[i];
        }
    }

    return "-";
}
