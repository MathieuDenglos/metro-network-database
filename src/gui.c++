#include "gui.h"
#include "network.h"
#include "stations.h"
#include "rolling_stock.h"

#include <iostream>
#include <limits>
#include <limits>

namespace GUI
{
    bool execute_query(sql::Connection *con,
                        sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";
        int choice;
        std::cout << "what do you want to do ?\n\n"
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
            GUI::modify_rolling_stocks(con, stmt);
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
        std::cout << "what changed to do on the network ?\n\n"
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
            STATIONS::add_station(con, stmt);
            break;
        case 5:
            STATIONS::show_line_stations(stmt);
            break;
        case 6:
            STATIONS::remove_station(stmt);
            break;
        }
    }

    void modify_rolling_stocks(sql::Connection *con,
                               sql::Statement *stmt)
    {
        int choice;
        char c;
        std::cout << "what changed to do on the rolling_stocks ?\n\n"
                  << "1) Add a new train to rolling stock\n"
                  << "2) Show rolling stocks\n"
                  << "3) Remove a train from rolling stock\n"
                  << "4) Show train data and schedule\n"
                  << std::flush;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            RS::add_train(stmt);
            break;
        case 2:
            RS::show_rolling_stocks(stmt);
            break;
        case 3:
            RS::remove_train(stmt);
            break;
        case 4:
            RS::show_train(stmt);
            break;
        }
    }
} // namespace GUI
