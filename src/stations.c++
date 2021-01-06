#include "stations.h"
#include "network.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace STATIONS
{
    //general input, output functions
    std::size_t output_stations(sql::Statement *stmt,
                                int line_id)
    {
        //first collect all the stations in given line
        sql::ResultSet *existing_stations;
        existing_stations = stmt->executeQuery("SELECT * FROM stations WHERE line_id = " + std::to_string(line_id) + " ORDER BY station_id ASC");
        std::size_t stations_count = existing_stations->rowsCount();

        //if stations are found, print them, otherwise tell the user that the line has no stations
        if (stations_count > 0)
        {
            std::cout << "List of stations in the line\n\n"
                      << "line  station  station_name          seconds to next station\n";
            while (existing_stations->next())
            {
                std::cout << " " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("line_id")
                          << "      " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("station_id")
                          << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_stations->getString("station_name")
                          << "  " << std::setw(4) << std::right << std::setfill('0') << existing_stations->getString("travel_time_to_next_station") << "s\n";
            }
            std::cout << std::flush;
        }
        else
            std::cout << "line " << line_id << " has no stations yet\n"
                      << std::flush;

        delete existing_stations;
        return stations_count;
    }

    std::size_t ask_station(sql::Statement *stmt,
                            sql::ResultSet **searched_station,
                            int line_id)
    {
        //ask the user to enter the station he's searching for (either the station_id or the station name)
        std::cout << "\nEnter the station id or name\n"
                  << std::flush;
        std::string reply;
        std::cin >> reply;

        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            *searched_station = stmt->executeQuery("SELECT * FROM stations WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + reply + " ;");
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            *searched_station = stmt->executeQuery("SELECT * FROM stations WHERE line_id = " + std::to_string(line_id) + " AND station_name = \'" + reply + "\' ;");
        }
        catch (const std::out_of_range &e)
        {
            //If invalid, create an empty resultset
            *searched_station = stmt->executeQuery("SELECT NULL limit 0;");
        }
        return (*searched_station)->rowsCount();
    }

    void append_valid_station(sql::Connection *con,
                              sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              const char *station_name,
                              int seconds_to_station)
    {
        //Append a new station in the stations table, only filling :
        //line_id, station_id and station_name while setting to NULL the time_to_next_station (which doesn't exist)
        sql::PreparedStatement *append_station_stmt;
        append_station_stmt = con->prepareStatement("INSERT INTO stations(line_id, station_id, station_name)"
                                                    "   VALUES (?, ?, ?)");
        append_station_stmt->setInt(1, line_id);
        append_station_stmt->setInt(2, station_id);
        append_station_stmt->setString(3, station_name);
        append_station_stmt->execute();
        delete append_station_stmt;

        //update the previous station (unless it's the first one) with the new seconds to station
        stmt->execute("UPDATE stations"
                      "   SET travel_time_to_next_station = " +
                      std::to_string(seconds_to_station) + "   WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id - 1));
    }

    void remove_valid_station(sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              bool is_last_station,
                              bool is_first_station,
                              int secons_from_last_to_next)
    {
        //Remove the station from the stations table
        stmt->execute("DELETE FROM stations where line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id) + " ;");

        //if it isn't the last station, decreases all the station_id above by one to still have an incrementing list.
        if (!is_last_station)
            stmt->execute("UPDATE stations"
                          "   SET station_id = station_id - 1"
                          "   WHERE line_id = " +
                          std::to_string(line_id) + " AND station_id > " + std::to_string(station_id) + " ORDER BY station_id ASC;");

        //if it's the last station that got removed and the line still have stations, set the previous station time to next station to NULL
        if (is_last_station && !is_first_station)
            stmt->execute("UPDATE stations SET travel_time_to_next_station = NULL WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id - 1) + ";");

        //otherwise, set the time to the next station to secons_from_last_to_next
        if (!is_first_station && !is_last_station)
            stmt->execute("UPDATE stations SET travel_time_to_next_station = " + std::to_string(secons_from_last_to_next) + " WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id - 1) + ";");
    }

    void show_line_stations(sql::Statement *stmt)
    {
        //outputs all the line and returns if none exists
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask for the line and
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(&searched_line, stmt) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            std::cout << "\e[1;1H\e[2J";
            searched_line->next();
            std::cout << "\n\n\nStations of line " << searched_line->getString("line_name")
                      << " (" << searched_line->getInt("line_id") << "\n\n"
                      << std::flush;
            STATIONS::output_stations(stmt, searched_line->getInt("line_id"));
        }
    }

    void remove_station(sql::Statement *stmt)
    {
        //first output all the lines and return if none are being displayed
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask the user to select a line
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(&searched_line, stmt) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            //if the search is successful, move on and outputs all the stations of the line
            searched_line->next();
            if (STATIONS::output_stations(stmt, searched_line->getInt("line_id")) == 0)
            {
                delete searched_line;
                return;
            };

            //ask the user to select a station within the line
            sql::ResultSet *searched_station;
            if (STATIONS::ask_station(stmt, &searched_station, searched_line->getInt("line_id")) == 0)
                std::cout << "station doesn't exist, please try again later\n"
                          << std::flush;
            else
            {
                searched_station->next();
                //Unless we're removing the first or last station; ask for the time from previous station to next station
                int time_from_last_to_next = 0, temp_min, temp_sec;
                const bool is_last_station = (stmt->executeQuery("SELECT station_id FROM stations WHERE line_id = " + std::to_string(searched_station->getInt("line_id")) + " AND station_id > " + std::to_string(searched_station->getInt("station_id")) + " ;")->rowsCount() == 0);
                const bool is_first_station = searched_station->getInt("station_id") == 1;

                //If this is neither the first station or last station, ask the new time from station_id -1 to station_id
                if (!is_first_station && !is_last_station)
                {
                    std::cout << "Insert the new commute time from the previous station to the new one\nminutes (enter) seconds"
                              << std::flush;
                    std::cin >> temp_min >> temp_sec;
                    time_from_last_to_next = 60 * temp_min + temp_sec;
                }

                STATIONS::remove_valid_station(stmt, searched_station->getInt("line_id"), searched_station->getInt("station_id"), is_last_station, is_first_station, time_from_last_to_next);
            }
            delete searched_station;
        }
        delete searched_line;
    }
} // namespace STATIONS