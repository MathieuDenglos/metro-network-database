#include "other.h"

#include <iostream>
#include <string>
#include <limits>

namespace IO
{
    const char *get_string()
    {
        std::string reply;
        std::getline(std::cin, reply);
        return reply.c_str();
    }

    int get_int()
    {
        bool is_int = false;
        std::string input;
        int input_int;
        do
        {
            std::getline(std::cin, input);
            try
            {
                input_int = std::stoi(input);
                is_int = true;
            }
            catch (const std::exception &e)
            {
                std::cout << "enter a valid number\n"
                          << std::flush;
            }
        } while (!is_int);

        return input_int;
    }

    void wait_input()
    {
        std::cout << "\nPress enter to continue"
                  << std::flush;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
} // namespace IO