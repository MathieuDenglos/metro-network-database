#include "network.h"
#include "stations.h"
#include <iostream>
#include <iomanip>
#include <string>

namespace NETWORK
{
    void new_line(sql::Connection *con,
                  sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";

        //first grab the already existing lines
        sql::ResultSet *existing_lanes;
        existing_lanes = stmt->executeQuery("SELECT line_id, line_name FROM network ORDER BY line_id ASC");

        //output the existing lines or a message if there aren't any
        if (existing_lanes->rowsCount() == 0)
            std::cout << "the network have no lines yet!\n\n";

        else
            std::cout << "The network already have : " << existing_lanes->rowsCount() << " lines\n\n"
                      << "number  line_name\n";
        while (existing_lanes->next())
        {
            std::cout << "   " << std::setw(3) << std::right << std::setfill('0') << existing_lanes->getInt("line_id")
                      << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_lanes->getString("line_name") << '\n';
        }
        delete existing_lanes;

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

        //Ask for the station name (has to be under 20 characters)
        std::cout << "Provide a name for this new line\n"
                  << std::flush;
        std::string new_line_name;
        std::cin >> new_line_name;
        if (new_line_name.size() > 20)
            new_line_name.erase(new_line_name.begin() + 19, new_line_name.end());

        //Insert the new line
        sql::PreparedStatement *insert_line_stmt;
        insert_line_stmt = con->prepareStatement("INSERT INTO network(line_id, line_name)"
                                                 "   VALUES (?, ?)");
        insert_line_stmt->setInt(1, new_line_id);
        insert_line_stmt->setString(2, new_line_name);
        insert_line_stmt->execute();
        delete insert_line_stmt;

        //Insert all the stations for the new line
        new_line_station_adding(con, stmt, new_line_id, 1, 0);
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
        {
            std::cout << "List of already existing stations in the line\n\n"
                      << "line  station  station_name          seconds to next station\n";
            sql::ResultSet *existing_stations;
            existing_stations = stmt->executeQuery("SELECT * FROM stations WHERE line_id = " + std::to_string(line_id) + " ORDER BY station_id ASC");
            while (existing_stations->next())
            {
                std::cout << " " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("line_id")
                          << "      " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("station_id")
                          << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_stations->getString("station_name")
                          << "  " << std::setw(4) << std::right << std::setfill('0') << existing_stations->getString("travel_time_to_next_station") << "s\n";
            }
            delete existing_stations;
        }

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
        STATIONS::append_station(con, stmt, line_id, station_id, new_station_name.c_str(), seconds_to_station);

        if (seconds_to_next_station > 0)
            new_line_station_adding(con, stmt, line_id, station_id + 1, seconds_to_next_station);
    }
} // namespace