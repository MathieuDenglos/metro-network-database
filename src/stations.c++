#include "stations.h"
#include "network.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace STATIONS
{
    std::size_t output_stations(sql::Statement *stmt,
                                int line_id)
    {
        //first collect the list of the already existing stations
        sql::ResultSet *existing_stations;
        existing_stations = stmt->executeQuery("SELECT * FROM stations WHERE line_id = " + std::to_string(line_id) + " ORDER BY station_id ASC");
        std::size_t stations_count = existing_stations->rowsCount();
        //print all previously added stations of the line
        if (stations_count > 0)
        {
            std::cout << "List of already existing stations in the line\n\n"
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

    void show_line_stations(sql::Statement *stmt)
    {
        //outputs all the line and returns if none exists
        if (NETWORK::output_lines(stmt) == 0)
            return;

        std::cout << "\nPlease select enter the line id or name to show the stations\n"
                  << std::flush;
        std::string reply;
        std::cin >> reply;

        sql::ResultSet *searched_line;
        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            searched_line = stmt->executeQuery("SELECT * FROM network WHERE line_id = " + reply + " ;");
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            searched_line = stmt->executeQuery("SELECT * FROM network WHERE line_name = \'" + reply + "\' ;");
        }
        catch (const std::out_of_range &e)
        {
            //if the given value is too big, send an error messqage and return
            std::cout << "\nThe entered line doesn't exist\n"
                      << std::flush;
            delete searched_line;
            return;
        }

        //if the query wasn't successful return
        if (searched_line->rowsCount() == 0)
        {
            std::cout << "\nNo line found with given research, try again later\n"
                      << std::flush;
            delete searched_line;
            return;
        }
        std::cout << "\e[1;1H\e[2J";

        //if the query was succesful, ask the user a verification for each
        while (searched_line->next())
        {
            std::cout << "\n\n\nStations of line " << searched_line->getString("line_name")
                      << " (" << searched_line->getInt("line_id") << "\n\n"
                      << std::flush;
            STATIONS::output_stations(stmt, searched_line->getInt("line_id"));
        }
        delete searched_line;
    }

    void append_station(sql::Connection *con,
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
} // namespace STATIONS