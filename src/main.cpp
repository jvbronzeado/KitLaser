#include <iostream>

void display_help_message()
{
    std::cout << "wip" << std::endl;
}

int main(int argc, const char* argv[])
{
    if(argc == 1)
    {
        display_help_message();
        return 0;
    }
    
    std::cout << "Hello World!" << std::endl;
    return 0;
}
