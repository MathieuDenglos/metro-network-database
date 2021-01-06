#include "gui.h"
#include "network.h"
#include "stations.h"

#include <iostream>

namespace GUI
{
    bool execute_querry(sql::Connection *con,
                        sql::Statement *stmt)
    {
        int choice;

        std::cout << "que voules-vous faire ?\n\n"
                  << "1) modify network\n"
                  << "2) modify rolling_stocks\n"
                  << "3) modify schedule\n"
                  << "0) exit\n"
                  << std::flush;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            GUI::modify_network(con, stmt);
            return true;
        case 2:
            std::cout << "to implement" << std::endl;
            return true;
        case 3:
            std::cout << "to implement" << std::endl;
            return true;
        case 0:
            return false;
        default:
            return true;
        }
    }

    void modify_network(sql::Connection *con,
                        sql::Statement *stmt)
    {
        int choice;

        std::cout << "quelles modification apporter au network ?\n\n"
                  << "1) Add a new line\n"
                  << "2) See lines in the network\n"
                  << "3) Remove a line\n"
                  << "4) Add a station station\n"
                  << "5) See stations of a line\n"
                  << "6) Remove a station\n"
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
            std::cout << "to implement" << std::endl;
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
