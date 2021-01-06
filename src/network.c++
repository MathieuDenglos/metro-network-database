#include "network.h"
#include "stations.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <exception>

namespace NETWORK
{
    //general input, output functions
    std::size_t output_lines(sql::Statement *stmt)
    {
        //first grab the already existing lines
        sql::ResultSet *existing_lines;
        existing_lines = stmt->executeQuery("SELECT line_id, line_name FROM network ORDER BY line_id ASC");
        std::size_t lines_count = existing_lines->rowsCount();

        //output the existing lines or a message if there aren't any
        if (lines_count == 0)
            std::cout << "the network have no lines yet!\n\n";

        else
            std::cout << "The network already have " << lines_count << " lines\n\n"
                      << "number  line_name\n";
        while (existing_lines->next())
        {
            std::cout << "   " << std::setw(3) << std::right << std::setfill('0') << existing_lines->getInt("line_id")
                      << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_lines->getString("line_name") << '\n';
        }

        delete existing_lines;
        return lines_count;
    }

    std::size_t ask_line(sql::ResultSet **searched_line,
                         sql::Statement *stmt)
    {
        //Ask for the searched line
        std::string reply;
        std::cout << "Entre the name or number of the line you want to remove\n"
                  << std::flush;
        std::cin >> reply;

        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            *searched_line = stmt->executeQuery("SELECT * FROM network WHERE line_id = " + reply + " ;");
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            *searched_line = stmt->executeQuery("SELECT * FROM network WHERE line_name = \'" + reply + "\' ;");
        }
        catch (const std::out_of_range &e)
        {
            //if the given value is too big, return an empty list
            *searched_line = stmt->executeQuery("SELECT NULL LIMIT 0;");
        }
        return (*searched_line)->rowsCount();
    }

    //callable functions
    void add_line(sql::Connection *con,
                  sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";

        //output all the lines
        NETWORK::output_lines(stmt);

        //Ask for the line number (has to be unique)
        std::cout << "\n\nPlease provide the line number (has to be unique)\n"
                  << std::flush;
        int new_line_id;
        sql::ResultSet *lines_with_id;
        do
        {
            std::cin >> new_line_id;
            lines_with_id = stmt->executeQuery("SELECT COUNT(line_id) FROM network WHERE line_id = " + std::to_string(new_line_id) + ";");
            lines_with_id->next();
            if (lines_with_id->getInt(1) != 0)
                std::cout << "this line already exists please select another line id\n"
                          << std::flush;
        } while (lines_with_id->getInt(1) != 0);
        delete lines_with_id;

        //Ask for the station name (has to be under 20 characters and unique)
        std::cout << "Provide a name for this new line\n"
                  << std::flush;
        std::string new_line_name;
        sql::ResultSet *lines_with_name;
        do
        {
            std::cin >> new_line_name;
            if (new_line_name.size() > 20)
                new_line_name.erase(new_line_name.begin() + 19, new_line_name.end());
            lines_with_name = stmt->executeQuery("SELECT COUNT(line_id) FROM network WHERE line_name = \'" + new_line_name + "\';");
            lines_with_name->next();
            if (lines_with_name->getInt(1) != 0)
                std::cout << "Another line already have this name, please select another name for the line\n"
                          << std::flush;
        } while (lines_with_name->getInt(1) != 0);
        delete lines_with_name;

        //create the prepared statement to prevent SQLinjections while insertine the line
        sql::PreparedStatement *insert_line_stmt;
        insert_line_stmt = con->prepareStatement("INSERT INTO network(line_id, line_name)"
                                                 "   VALUES (?, ?)");

        //Fill the different elements and execute the insertion
        insert_line_stmt->setInt(1, new_line_id);
        insert_line_stmt->setString(2, new_line_name);
        insert_line_stmt->execute();
        delete insert_line_stmt;

        //Insert all the stations for the new line
        new_line_station_adding(con, stmt, new_line_id, 1, 0);
    }

    void remove_line(sql::Connection *con,
                     sql::Statement *stmt)
    {
        //prints all the lines and return if there are none
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask for the line to search, and either output an error message or continue
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(&searched_line, stmt) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            //print all the stations within the line
            searched_line->next();
            std::cout << "\e[1;1H\e[2J";
            int station_in_line = STATIONS::output_stations(stmt, searched_line->getInt("line_id"));
            std::cout << "\nBy deleting the line " << searched_line->getString("line_name") << " (" << searched_line->getInt("line_id")
                      << "), you will also remove : " << station_in_line
                      << " stations.\nTo confirm deletion write : yes\n"
                      << std::flush;

            //ask for confirmation to prevent mistakes
            std::string confirmation;
            std::cin >> confirmation;
            if (std::toupper(confirmation[0]) == 'Y')
            {
                stmt->execute("DELETE FROM network WHERE line_id = " + std::to_string(searched_line->getInt("line_id")) + " ;");
                std::cout << "line deleted\n\n"
                          << std::flush;
            }
            else
                std::cout << "action canceled\n\n"
                          << std::flush;
        }
        delete searched_line;
    }

} // namespace NETWORK

namespace
{
    void new_line_station_adding(sql::Connection *con,
                                 sql::Statement *stmt,
                                 int line_id,
                                 int station_id,
                                 int seconds_to_station)
    {
        std::cout << "\e[1;1H\e[2J";

        //print all previously added stations of the line
        if (station_id != 1)
            STATIONS::output_stations(stmt, line_id);

        std::cout << "\n\nAdding station " << station_id << " of line " << station_id << '\n';

        //Ask for the station name
        std::cout << "Provide a name for this new station (has to be unique within the line)\n"
                  << std::flush;
        std::string new_station_name;
        sql::ResultSet *same_station_name;
        do
        {
            std::cin >> new_station_name;
            if (new_station_name.size() > 20)
                new_station_name.erase(new_station_name.begin() + 19, new_station_name.end());
            same_station_name = stmt->executeQuery("SELECT COUNT(line_id) FROM stations WHERE line_id = " + std::to_string(line_id) + " AND station_name LIKE \'" + new_station_name + "\';");
            same_station_name->next();
            if (same_station_name->getInt(1) != 0)
                std::cout << "This line already has a station with the name : " << new_station_name << std::endl;
        } while (same_station_name->getInt(1) != 0);
        delete same_station_name;

        //ask for the time to the next station (0) if last station of the line
        unsigned int seconds_to_next_station, temp_min, temp_sec;
        std::cout << "indicates the time to the next station minutes (enter) seconds (0 (enter) 0) if last station" << std::endl;
        std::cin >> temp_min >> temp_sec;
        seconds_to_next_station = temp_min * 60 + temp_sec;
        std::cout << temp_min << ' ' << temp_sec << ' ' << seconds_to_next_station << std::endl;
        STATIONS::append_valid_station(con, stmt, line_id, station_id, new_station_name.c_str(), seconds_to_station);

        if (seconds_to_next_station > 0)
            new_line_station_adding(con, stmt, line_id, station_id + 1, seconds_to_next_station);
    }
} // namespace