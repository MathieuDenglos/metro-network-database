#include "gui.h"
#include "network.h"
#include "stations.h"
#include "rolling_stock.h"
#include "other.h"

#include <iostream>

namespace GUI
{
    bool execute_query(sql::Connection *con,
                       sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J"
                  << "what do you want to do ?\n\n"
                  << "1) Network/stations\n"
                  << "2) rolling_stock\n"
                  << "3) schedule\n"
                  << "0) exit\n"
                  << std::flush;
        int choice = IO::get_int();

        switch (choice)
        {
        case 1:
            GUI::modify_network(con, stmt);
            break;
        case 2:
            GUI::modify_rolling_stock(con, stmt);
            break;
        case 3:
            std::cout << "to implement" << std::endl;
            break;
        default:
            return false;
        }

        IO::wait_input();
        return true;
    }

    void modify_network(sql::Connection *con,
                        sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J"
                  << "what changes to do in the network ?\n\n"
                  << "1) Add a new line\n"
                  << "2) Show lines in the network\n"
                  << "3) Remove a line\n"
                  << "4) Add a station to an existing line\n"
                  << "5) Show stations of a line\n"
                  << "6) Remove a station from an existing line\n"
                  << "0) cancel\n"
                  << std::flush;
        int choice = IO::get_int();

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
            STATIONS::add_station(con, stmt);
            break;
        case 5:
            STATIONS::show_line_stations(con, stmt);
            break;
        case 6:
            STATIONS::remove_station(con, stmt);
            break;
        }
    }

    void modify_rolling_stock(sql::Connection *con,
                              sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J"
                  << "what changed to do on the rolling_stock ?\n\n"
                  << "1) Add a new train to rolling stock\n"
                  << "2) Show rolling stocks\n"
                  << "3) Remove a train from rolling stock\n"
                  << "0) cancel\n"
                  << std::flush;
        int choice = IO::get_int();

        switch (choice)
        {
        case 1:
            RS::add_material(con, stmt);
            break;
        case 2:
            RS::show_rolling_stock(stmt);
            break;
        case 3:
            RS::remove_material(con, stmt);
            break;
        }
    }
} // namespace GUI
