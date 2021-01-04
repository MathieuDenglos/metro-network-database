#include "stations.h"

#include <string>

namespace STATIONS
{
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