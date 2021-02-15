#include "stations.h"
#include "network.h"
#include "other.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <limits>

namespace STATIONS
{
    std::size_t output_stations(sql::Connection *con,
                                int line_id)
    {
        //first collect all the stations in given line
        std::cout << "\e[1;1H\e[2J";
        sql::ResultSet *existing_stations;
        sql::PreparedStatement *select_by_line_id = con->prepareStatement(SPS::select_by_line_id);
        select_by_line_id->setInt(1, line_id);
        existing_stations = select_by_line_id->executeQuery();
        std::size_t stations_count = existing_stations->rowsCount();

        //if none exists, tell the user
        if (stations_count == 0)
            std::cout << "line " << line_id << " has no stations yet\n"
                      << std::flush;
        //else output existing lines
        else
        {
            std::cout << "List of stations in the line " << line_id << "\n\n"
                      << "line  station  station_name          travel time to the next station\n";
            while (existing_stations->next())
            {
                std::cout << " " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("line_id")
                          << "      " << std::setw(3) << std::right << std::setfill('0') << existing_stations->getInt("station_id")
                          << "  " << std::setw(20) << std::left << std::setfill(' ') << existing_stations->getString("station_name")
                          << "  " << std::setw(4) << std::right << std::setfill('0') << existing_stations->getString("travel_time_to_next_station") << "s\n";
            }
            std::cout << std::flush;
        }

        //delete the ResultSet, PreparedStatement and return
        delete existing_stations;
        delete select_by_line_id;
        return stations_count;
    }

    std::size_t ask_station(sql::Connection *con,
                            sql::Statement *stmt,
                            sql::ResultSet **searched_station,
                            int line_id)
    {
        //ask the user to enter the station he's searching for (either the station_id or the station name)
        std::cout << "\nEnter the station id or name\n"
                  << std::flush;
        std::string reply = IO::get_string();

        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            sql::PreparedStatement *select_by_station_id_and_line_id = con->prepareStatement(SPS::select_by_station_id_and_line_id);
            select_by_station_id_and_line_id->setInt(1, line_id);
            select_by_station_id_and_line_id->setInt(2, searched_id);
            *searched_station = select_by_station_id_and_line_id->executeQuery();
            delete select_by_station_id_and_line_id;
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            sql::PreparedStatement *select_by_station_name_and_line_id = con->prepareStatement(SPS::select_by_station_name_and_line_id);
            select_by_station_name_and_line_id->setInt(1, line_id);
            select_by_station_name_and_line_id->setString(2, reply);
            *searched_station = select_by_station_name_and_line_id->executeQuery();
            delete select_by_station_name_and_line_id;
        }
        catch (const std::out_of_range &e)
        {
            //If invalid, create an empty resultset
            *searched_station = stmt->executeQuery("SELECT NULL limit 0;");
        }
        return (*searched_station)->rowsCount();
    }

    void add_station(sql::Connection *con,
                     sql::Statement *stmt)
    {
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
            std::size_t stations_count = STATIONS::output_stations(con, searched_line->getInt("line_id"));

            //grabs the station_id
            std::cout << "indicate the id of the station after which you want to insert the station\n"
                      << "(0 = begining ; " << stations_count << " = end)\n\n"
                      << std::flush;
            int new_station_id = IO::get_int();

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
            sql::PreparedStatement *stations_with_same_name = con->prepareStatement(SPS::select_by_station_name_and_line_id);
            stations_with_same_name->setInt(1, searched_line->getInt("line_id"));
            sql::ResultSet *same_station_name;
            do
            {
                new_station_name = IO::get_string();
                if (new_station_name.size() > 20)
                    new_station_name.erase(new_station_name.begin() + 19, new_station_name.end());

                stations_with_same_name->setString(2, new_station_name);
                same_station_name = stations_with_same_name->executeQuery();
                if (same_station_name->rowsCount() != 0)
                    std::cout << "This line already has a station with the name : " << new_station_name << std::endl;
            } while (same_station_name->rowsCount() != 0);
            delete same_station_name;

            //Now need to change some travel time to stations depending on where the station is located in the line
            int seconds_to_station = 0, seconds_to_next_station = 0;

            //if the station is not at the begining, a time to station must be given to update the old one
            if (!is_begining)
            {
                std::cout << "Insert the new time from the previous station to this station\nminutes (enter) seconds\n"
                          << std::flush;
                do
                {
                    seconds_to_station = IO::get_int() * 60 + IO::get_int();
                    seconds_to_station *= (seconds_to_station >= 0); //to prevent negative travel times
                    if (seconds_to_station == 0)
                        std::cout << "travel time between stations cannot be null, teleportation doesn't exist yet\n"
                                  << std::flush;
                } while (seconds_to_station == 0);
            }

            //if the station is not at the end, a time to the next station must be given to update the old one
            if (!is_end)
            {
                std::cout << "Insert the time to the next station\nminutes (enter) seconds\n"
                          << std::flush;
                do
                {
                    seconds_to_next_station = IO::get_int() * 60 + IO::get_int();
                    seconds_to_next_station *= (seconds_to_next_station >= 0); //to prevent negative travel times
                    if (seconds_to_next_station == 0)
                        std::cout << "travel time between stations cannot be null, teleportation doesn't exist yet\n"
                                  << std::flush;
                } while (seconds_to_next_station == 0);
            }

            //Modify the append valid station to make it an insert valid station. (send the time_to and time_to_next and execute more queries depending on where the station needs to be inserted)
            STATIONS::insert_valid_station(con, searched_line->getInt("line_id"), new_station_id, new_station_name.c_str(), seconds_to_station, seconds_to_next_station);
        }
        delete searched_line;
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
            searched_line->next();
            STATIONS::output_stations(con, searched_line->getInt("line_id"));
        }
    }

    void remove_station(sql::Connection *con,
                        sql::Statement *stmt)
    {
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
            if (STATIONS::output_stations(con, searched_line->getInt("line_id")) == 0)
            {
                delete searched_line;
                return;
            }

            //ask the user to select a station within the line
            sql::ResultSet *searched_station;
            if (STATIONS::ask_station(con, stmt, &searched_station, searched_line->getInt("line_id")) == 0)
                std::cout << "station doesn't exist, please try again later\n"
                          << std::flush;
            else
            {
                searched_station->next();
                //Unless we're removing the first or last station; ask for the time from previous station to next station
                int time_from_last_to_next = 0;
                const bool is_last_station = (stmt->executeQuery("SELECT station_id FROM stations WHERE line_id = " + std::to_string(searched_station->getInt("line_id")) + " AND station_id > " + std::to_string(searched_station->getInt("station_id")) + " ;")->rowsCount() == 0);
                const bool is_first_station = searched_station->getInt("station_id") == 1;

                //If this is neither the first station or last station, ask the new time from station_id -1 to station_id
                if (!is_first_station && !is_last_station)
                {
                    std::cout << "Insert the new commute time from the previous station to the new one\nminutes (enter) seconds"
                              << std::flush;
                    time_from_last_to_next = IO::get_int() * 60 + IO::get_int();
                    time_from_last_to_next *= (time_from_last_to_next >= 0); //to prevent negative travel times
                }

                STATIONS::remove_valid_station(con, searched_station->getInt("line_id"), searched_station->getInt("station_id"), is_last_station, is_first_station, time_from_last_to_next);
            }
            delete searched_station;
        }
        delete searched_line;
    }

    void insert_valid_station(sql::Connection *con,
                              int line_id,
                              int station_id,
                              const char *station_name,
                              int seconds_to_station,
                              int seconds_to_next_station)
    {
        if (seconds_to_next_station > 0)
        {
            sql::PreparedStatement *range_station_id_increase = con->prepareStatement(SPS::range_station_id_increase);
            range_station_id_increase->setInt(1, line_id);
            range_station_id_increase->setInt(2, station_id);
            range_station_id_increase->execute();
            delete range_station_id_increase;
        }

        //Append a new station in the stations table, only filling :
        //line_id, station_id and station_name while setting to NULL the time_to_next_station (which doesn't exist)
        sql::PreparedStatement *insert_station = con->prepareStatement(SPS::insert_station);
        insert_station->setInt(1, line_id);
        insert_station->setInt(2, station_id);
        insert_station->setString(3, station_name);
        if (seconds_to_next_station > 0)
            insert_station->setInt(4, seconds_to_next_station);
        else
            insert_station->setNull(4, 0);
        insert_station->execute();
        delete insert_station;

        //update the previous station (unless it's the first one) with the new seconds to station
        if (seconds_to_station > 0)
        {
            sql::PreparedStatement *update_travel_time = con->prepareStatement(SPS::update_travel_time);
            update_travel_time->setInt(1, seconds_to_station);
            update_travel_time->setInt(2, line_id);
            update_travel_time->setInt(3, station_id - 1);
            update_travel_time->execute();
            delete update_travel_time;
        }
    }

    void remove_valid_station(sql::Connection *con,
                              int line_id,
                              int station_id,
                              bool is_last_station,
                              bool is_first_station,
                              int seconds_from_last_to_next)
    {
        //Remove the station from the stations table
        sql::PreparedStatement *delete_station = con->prepareStatement(SPS::delete_station);
        delete_station->setInt(1, line_id);
        delete_station->setInt(2, station_id);
        delete_station->execute();
        delete delete_station;

        //if it isn't the last station, decreases all the station_id above by one to still have an incrementing list.
        if (!is_last_station)
        {
            sql::PreparedStatement *range_station_id_decrease = con->prepareStatement(SPS::range_station_id_decrease);
            range_station_id_decrease->setInt(1, line_id);
            range_station_id_decrease->setInt(2, station_id + 1);
            range_station_id_decrease->execute();
            delete range_station_id_decrease;
        }

        //Update the travel time from the station before the one deleted to the one after
        if (!is_first_station)
        {
            sql::PreparedStatement *update_travel_time = con->prepareStatement(SPS::update_travel_time);

            //if the station deleted was the last one, just update travel time to NULL
            //otherwise set it to the given time
            if (is_last_station && !is_first_station)
                update_travel_time->setNull(1, 0);
            else
                update_travel_time->setInt(1, seconds_from_last_to_next);

            update_travel_time->setInt(2, line_id);
            update_travel_time->setInt(3, station_id - 1); //cannot cause issue because code doesn't execute if fir station
            update_travel_time->execute();
            delete update_travel_time;
        }
    }

    void add_stations_in_new_line(sql::Connection *con,
                                  int line_id,
                                  int station_id /*= 1*/,
                                  int seconds_to_station /*= 0*/)
    {
        //print all previously added stations of the line
        if (station_id != 1)
            STATIONS::output_stations(con, line_id);

        std::cout << "\e[1;1H\e[2J"
                  << "\n\nAdding station " << station_id << " of line " << line_id << '\n';

        //Ask for the station name (need to be unique)
        std::cout << "Provide a name for this new station (has to be unique within the line)\n"
                  << std::flush;
        std::string new_station_name;
        sql::PreparedStatement *select_by_station_name_and_line_id = con->prepareStatement(SPS::select_by_station_name_and_line_id);
        select_by_station_name_and_line_id->setInt(1, line_id);
        sql::ResultSet *same_station_name;
        do
        {
            new_station_name = IO::get_string();
            //if station name is over 20 characters (column limit), remove the end
            if (new_station_name.size() > 20)
                new_station_name.erase(new_station_name.begin() + 19, new_station_name.end());

            //verify if another station doesn't have the same name (all stations must have unique name within the line)
            select_by_station_name_and_line_id->setString(2, new_station_name);
            same_station_name = select_by_station_name_and_line_id->executeQuery();
            if (same_station_name->rowsCount() != 0)
                std::cout << "This line already has a station with the name : " << new_station_name << std::endl;
        } while (same_station_name->rowsCount() != 0);
        delete same_station_name;

        //ask for the time to the next station (0 if last station of the line)

        std::cout << "indicates the time to the next station minutes (enter) seconds (0 (enter) 0) if last station" << std::endl;
        int seconds_to_next_station = IO::get_int() * 60 + IO::get_int();
        seconds_to_next_station *= (seconds_to_next_station >= 0); //to prevent negative travel times

        STATIONS::insert_valid_station(con, line_id, station_id, new_station_name.c_str(), seconds_to_station, 0);

        //calls the method again if the user still want to add some stations to the line
        if (seconds_to_next_station > 0)
            STATIONS::add_stations_in_new_line(con, line_id, station_id + 1, seconds_to_next_station);
    }
} // namespace STATIONS