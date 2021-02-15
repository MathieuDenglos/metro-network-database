#include "network.h"
#include "stations.h"
#include "other.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <exception>

namespace NETWORK
{
    std::size_t output_lines(sql::Statement *stmt)
    {
        //grab the list of lines in the network
        std::cout << "\e[1;1H\e[2J";
        sql::ResultSet *existing_lines;
        existing_lines = stmt->executeQuery(NPS::select_all_lines);
        std::size_t lines_count = existing_lines->rowsCount();

        //if none exists, tell the user
        if (lines_count == 0)
            std::cout << "the network have no lines yet!\n\n";
        //else output existing lines
        else
        {
            std::cout << "The network already has " << lines_count << " lines\n\n"
                      << "number  line_name\n";

            while (existing_lines->next())
                std::cout << "   " << std::setw(3) << std::right << std::setfill('0') << existing_lines->getInt("line_id")
                          << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_lines->getString("line_name") << '\n';
        }
        std::cout << std::flush;

        //delete the result query and return
        delete existing_lines;
        return lines_count;
    }

    std::size_t ask_line(sql::Connection *con,
                         sql::Statement *stmt,
                         sql::ResultSet **searched_line)
    {
        //Ask for the searched line

        std::cout << "Entre the name or number of the line you want to remove\n"
                  << std::flush;
        std::string reply = IO::get_string();

        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            sql::PreparedStatement *select_by_line_id = con->prepareStatement(NPS::select_by_line_id);
            select_by_line_id->setInt(1, searched_id);
            *searched_line = select_by_line_id->executeQuery();
            delete select_by_line_id;
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            sql::PreparedStatement *select_by_line_name = con->prepareStatement(NPS::select_by_line_name);
            select_by_line_name->setString(1, reply);
            *searched_line = select_by_line_name->executeQuery();
            delete select_by_line_name;
        }
        catch (const std::out_of_range &e)
        {
            //if the given value is too big, fill the ResultSet with an empty list
            *searched_line = stmt->executeQuery("SELECT NULL LIMIT 0;");
        }
        return (*searched_line)->rowsCount();
    }

    void add_line(sql::Connection *con,
                  sql::Statement *stmt)
    {
        //output all the lines
        NETWORK::output_lines(stmt);

        //Ask for the line number (has to be unique)
        std::cout << "\n\nPlease provide the line number (has to be unique)\n"
                  << std::flush;
        int new_line_id;
        sql::PreparedStatement *select_by_line_id = con->prepareStatement(NPS::select_by_line_id);
        sql::ResultSet *lines_with_id;
        do
        {
            new_line_id = IO::get_int();
            select_by_line_id->setInt(1, new_line_id);
            lines_with_id = select_by_line_id->executeQuery();
            if (lines_with_id->rowsCount() != 0)
                std::cout << "this line already exists please select another line id\n"
                          << std::flush;
        } while (lines_with_id->rowsCount() != 0);
        delete select_by_line_id;
        delete lines_with_id;

        //Ask for the station name (has to be under 20 characters and unique)
        std::cout << "Provide a name for this new line\n"
                  << std::flush;
        std::string new_line_name;
        sql::PreparedStatement *select_by_line_name = con->prepareStatement(NPS::select_by_line_name);
        sql::ResultSet *lines_with_name;
        do
        {
            new_line_name = IO::get_string();
            //if station name is over 20 characters (column limit), remove the end
            if (new_line_name.size() > 20)
                new_line_name.erase(new_line_name.begin() + 19, new_line_name.end());

            //verify if another line doesn't have the same name (all lines must have unique names)
            select_by_line_name->setString(1, new_line_name);
            lines_with_name = select_by_line_name->executeQuery();
            if (lines_with_id->rowsCount() != 0)
                std::cout << "Another line already have this name, please select another name for the line\n"
                          << std::flush;
        } while (lines_with_name->rowsCount() != 0);
        delete select_by_line_name;
        delete lines_with_name;

        sql::PreparedStatement *insert_line;
        insert_line = con->prepareStatement(NPS::insert_line);

        //Fill the different elements and execute the insertion
        insert_line->setInt(1, new_line_id);
        insert_line->setString(2, new_line_name);
        insert_line->execute();
        delete insert_line;

        //Insert all the stations for the new line
        STATIONS::add_stations_in_new_line(con, new_line_id);
    }

    void remove_line(sql::Connection *con,
                     sql::Statement *stmt)
    {
        //prints all the lines and return if there are none
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask for the line to search, and either output an error message or continue
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(con, stmt, &searched_line) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            //print all the stations within the line
            searched_line->next();
            int station_in_line = STATIONS::output_stations(con, searched_line->getInt("line_id"));
            std::cout << "\nBy deleting the line " << searched_line->getString("line_name") << " (" << searched_line->getInt("line_id")
                      << "), you will also remove : " << station_in_line
                      << " stations.\nTo confirm deletion write : yes\n"
                      << std::flush;

            //ask for confirmation to prevent mistakes
            std::string confirmation = IO::get_string();
            if (std::toupper(confirmation[0]) == 'Y')
            {
                sql::PreparedStatement *delete_line = con->prepareStatement(NPS::delete_line);
                delete_line->setInt(1, searched_line->getInt("line_id"));
                delete_line->execute();
                std::cout << "line deleted\n\n"
                          << std::flush;

                delete delete_line;
            }
            else
                std::cout << "action canceled\n\n"
                          << std::flush;
        }
        delete searched_line;
    }

} // namespace NETWORK