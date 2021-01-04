#include "stations.h"

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