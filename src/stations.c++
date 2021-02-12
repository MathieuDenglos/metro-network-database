#include "stations.h"
#include "network.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <limits>

namespace STATIONS
{
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
            std::cout << "List of stations in the line " << line_id << "\n\n"
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

    void remove_valid_station(sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              bool is_last_station,
                              bool is_first_station,
                              int seconds_from_last_to_next)
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

        //otherwise, set the time to the next station to seconds_from_last_to_next
        if (!is_first_station && !is_last_station)
            stmt->execute("UPDATE stations SET travel_time_to_next_station = " + std::to_string(seconds_from_last_to_next) + " WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id - 1) + ";");
    }

    void show_line_stations(sql::Connection *con,
                            sql::Statement *stmt)
    {
        //outputs all the line and returns if none exists
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask for the line and shows all the stations of said line
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(con, stmt, &searched_line) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            std::cout << "\e[1;1H\e[2J";
            searched_line->next();
            STATIONS::output_stations(stmt, searched_line->getInt("line_id"));
        }
    }

    void remove_station(sql::Connection* con,
    sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";

        //first output all the lines and return if none are being displayed
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask the user to select a line
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(con, stmt, &searched_line) == 0)
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
            }

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

    void add_station(sql::Connection *con,
                     sql::Statement *stmt)
    {
        std::cout << "\e[1;1H\e[2J";

        //first output all the lines and return if none are being displayed
        if (NETWORK::output_lines(stmt) == 0)
            return;

        //ask the user to select a line
        sql::ResultSet *searched_line;
        if (NETWORK::ask_line(con, stmt, &searched_line) == 0)
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
        else
        {
            searched_line->next();
            //if the search is successful, move on and outputs all the stations of the line
            std::size_t stations_count = STATIONS::output_stations(stmt, searched_line->getInt("line_id"));

            //grabs the station_id
            int new_station_id;
            std::cout << "indicate the id of the station after which you want to insert the station\n"
                      << "(0 = begining ; " << stations_count << " = end)\n\n"
                      << std::flush;
            std::cin >> new_station_id;

            //normalize the given value, so the station_id always are incrementing
            const bool is_begining = new_station_id <= 0;
            const bool is_end = new_station_id >= stations_count;
            new_station_id = 1 * is_begining +
                             (stations_count + 1) * (is_end && !is_begining) +
                             (new_station_id + 1) * (!is_begining && !is_end);

            //Ask for the station name
            std::string new_station_name;
            std::cout << "\e[1;1H\e[2J";
            std::cout << "Provide a name for this new station (has to be unique within the line)\n"
                      << std::flush;
            sql::ResultSet *same_station_name;
            do
            {
                std::cin >> new_station_name;
                if (new_station_name.size() > 20)
                    new_station_name.erase(new_station_name.begin() + 19, new_station_name.end());
                same_station_name = stmt->executeQuery("SELECT COUNT(line_id) FROM stations WHERE line_id = " + std::to_string(searched_line->getInt("line_id")) + " AND station_name LIKE \'" + new_station_name + "\';");
                same_station_name->next();
                if (same_station_name->getInt(1) != 0)
                    std::cout << "This line already has a station with the name : " << new_station_name << std::endl;
            } while (same_station_name->getInt(1) != 0);
            delete same_station_name;

            //Now need to change some travel time to stations depending on where the station is at
            unsigned int seconds_to_station = 0, seconds_to_next_station = 0, temp_min, temp_sec;

            //if the station is not at the begining, a time to station must be given to update the old one
            if (!is_begining)
            {
                std::cout << "Insert the new time from the previous station to this station\nminutes (enter) seconds"
                          << std::flush;
                do
                {
                    std::cin >> temp_min >> temp_sec;
                    if (temp_min == 0 && temp_sec == 0)
                        std::cout << "travel time between stations cannot be null, teleportation doesn't exist yet\n"
                                  << std::flush;
                } while (temp_min == 0 && temp_sec == 0);
                seconds_to_station = temp_min * 60 + temp_sec;
            }

            //if the station is not at the end, a time to the next station must be given to update the old one
            if (!is_end)
            {
                std::cout << "Insert the time to the next station\nminutes (enter) seconds"
                          << std::flush;
                do
                {
                    std::cin >> temp_min >> temp_sec;
                    if (temp_min == 0 && temp_sec == 0)
                        std::cout << "travel time between stations cannot be null, teleportation doesn't exist yet\n"
                                  << std::flush;
                } while (temp_min == 0 && temp_sec == 0);
                seconds_to_next_station = temp_min * 60 + temp_sec;
            }

            //Modify the append valid station to make it an insert valid station. (send the time_to and time_to_next and execute more querries depending on where the station needs to be inserted)
            STATIONS::insert_valid_station(con, stmt, searched_line->getInt("line_id"), new_station_id, new_station_name.c_str(), seconds_to_station, seconds_to_next_station);
        }
        delete searched_line;
    }

    void insert_valid_station(sql::Connection *con,
                              sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              const char *station_name,
                              int seconds_to_station,
                              int seconds_to_next_station)
    {
        if (seconds_to_next_station > 0)
            stmt->execute("UPDATE stations"
                          "   SET station_id = station_id + 1"
                          "   WHERE line_id = " +
                          std::to_string(line_id) + " AND station_id >= " + std::to_string(station_id) + " ORDER BY station_id DESC;");

        //Append a new station in the stations table, only filling :
        //line_id, station_id and station_name while setting to NULL the time_to_next_station (which doesn't exist)
        sql::PreparedStatement *append_station_stmt;
        if (seconds_to_next_station > 0)
            append_station_stmt = con->prepareStatement("INSERT INTO stations(line_id, station_id, station_name, travel_time_to_next_station)"
                                                        "   VALUES (?, ?, ?, ?)");
        else
            append_station_stmt = con->prepareStatement("INSERT INTO stations(line_id, station_id, station_name)"
                                                        "   VALUES (?, ?, ?)");
        append_station_stmt->setInt(1, line_id);
        append_station_stmt->setInt(2, station_id);
        append_station_stmt->setString(3, station_name);
        if (seconds_to_next_station > 0)
            append_station_stmt->setInt(4, seconds_to_next_station);
        append_station_stmt->execute();
        delete append_station_stmt;

        //update the previous station (unless it's the first one) with the new seconds to station
        if (seconds_to_station > 0)
            stmt->execute("UPDATE stations"
                          "   SET travel_time_to_next_station = " +
                          std::to_string(seconds_to_station) + "   WHERE line_id = " + std::to_string(line_id) + " AND station_id = " + std::to_string(station_id - 1));
    }
} // namespace STATIONS