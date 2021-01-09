#include "gui.h"
#include "network.h"
#include "stations.h"

#include <iostream>
#include <limits>
#include <limits>

namespace GUI
{
    bool execute_querry(sql::Connection *con,
                        sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";
        int choice;
        std::cout << "que voules-vous faire ?\n\n"
                  << "1) Network/stations\n"
                  << "2) rolling_stocks\n"
                  << "3) schedule\n"
                  << "0) exit\n"
                  << std::flush;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            GUI::modify_network(con, stmt);
            break;
        case 2:
            std::cout << "to implement" << std::endl;
            break;
        case 3:
            std::cout << "to implement" << std::endl;
            break;
        default:
            return false;
        }
        std::cout << "\nPress enter to continue"
                  << std::flush;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        char c = getchar();
        return true;
    }

    void modify_network(sql::Connection *con,
                        sql::Statement *stmt)
    {
        int choice;
        char c;
        std::cout << "quelles modification apporter au network ?\n\n"
                  << "1) Add a new line\n"
                  << "2) Show lines in the network\n"
                  << "3) Remove a line\n"
                  << "4) Add a station to an existing line\n"
                  << "5) Show stations of a line\n"
                  << "6) Remove a station from an existing line\n"
                  << "0) cancel\n"
                  << std::flush;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            NETWORK::add_line(con, stmt);
            break;
        case 2:
            NETWORK::output_lines(stmt);
            break;
        case 3:
            NETWORK::remove_line(con, stmt);
            break;
        case 4:
            STATIONS::add_station(stmt, con);
            break;
        case 5:
            STATIONS::show_line_stations(stmt);
            break;
        case 6:
            STATIONS::remove_station(stmt);
            break;
        }
    }
} // namespace GUI
