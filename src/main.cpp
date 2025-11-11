#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "Data.h"

enum Solvers
{
    None,
    ILS,
    All
};

std::vector<std::string> inputs;
Solvers active_solver = Solvers::ILS;
bool benchmark = false;

// argument parsing stuff
std::vector<std::string> cmd_tokens;

const std::string& cmd_get_option(const std::string& option)
{
    std::vector<std::string>::const_iterator it = std::find(cmd_tokens.begin(), cmd_tokens.end(), option);
    if(it != cmd_tokens.end() && ++it != cmd_tokens.end())
        return *it;

    static const std::string empty = "";
    return empty;
}

bool cmd_exists(const std::string& option)
{
    return std::find(cmd_tokens.begin(), cmd_tokens.end(), option) != cmd_tokens.end();
}

void display_help_message()
{
    std::cout   << "Usage: " << cmd_tokens[0] << " [options]\n"
                << "Options:\n"
                << "  --help         Shows this help message\n"
                << "  -s <solver>    Specify the solver to use (default: ILS) (Possibles: ILS/All)\n"
                << "  -i <file>      File to use as input\n"
                << "  -d <directory> Use all files from directory as input\n"
                << "  --benchmark    Show runtime benchmark\n";

}
//

Solvers get_solver_from_name(const std::string& solver)
{
    if(solver == "ILS")
    {
        return Solvers::ILS;
    }
    else if(solver == "All")
    {
        return Solvers::All;
    }
    return Solvers::None;
}

int main(int argc, const char* argv[])
{
    // get string tokens
    for(int i = 0; i < argc; i++)
    {
        cmd_tokens.push_back(std::string(argv[i]));
    }

    // check for help option
    if(cmd_exists("--help"))
    {
        display_help_message();
    }

    // check if benchmark should be activated
    if(cmd_exists("--benchmark"))
    {
        benchmark = true;        
    }

    // get chosen solver
    const std::string& chosen_solver = cmd_get_option("-s");
    if(!chosen_solver.empty())
    {
        active_solver = get_solver_from_name(chosen_solver);
        if(active_solver == Solvers::None)
        {
            // invalid
            std::cerr << "unknown solver " << chosen_solver << "\nuse --help option for more info\n";
            return -1;
        }
    }

    // get file input
    const std::string& input_file = cmd_get_option("-i");
    if(!input_file.empty())
    {
        inputs.push_back(input_file);
    }

    // get files from directory
    const std::string& input_directory = cmd_get_option("-d");
    if(!input_directory.empty())
    {
        for(const auto& file : std::filesystem::directory_iterator(input_directory))
        {
            if(!file.is_directory())
            {
                inputs.push_back(file.path());
            }
        }
    }

    if(inputs.size() == 0)
    {
        std::cerr << "no input files\nuse --help option for more info" << std::endl;
        return -1;
    }

    // TODO: Benchmarking
    for(std::string& path : inputs)
    {
        Data d = Data(2, path.c_str());
        d.read();
        size_t n = d.getDimension();

        std::cout << "Dimension: " << n << std::endl;
        std::cout << "DistanceMatrix: " << std::endl;
        d.printMatrixDist();
    }
        
    return 0;
}
