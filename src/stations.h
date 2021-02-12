#ifndef STATIONS_H
#define STATIONS_H

#include "constants_includes.h"

namespace STATIONS
{
    /**
     * @brief output all stations from a given line
     * 
     * @param stmt Used to communicate with the database
     * @param line_id id of the line
     * @return amount of stations in the given line 
     */
    std::size_t output_stations(sql::Statement *stmt,
                                int line_id);

    /**
     * @brief ask for a specific station of a given line
     * 
     * @param stmt Used to communicate with the database
     * @param searched_station Resultset to store the stations found
     * @param line_id id of the line to search a station in
     * @return the amount of stations in given line with entered station name or id (normally 0 or 1)
     */
    std::size_t ask_station(sql::Statement *stmt,
                            sql::ResultSet **searched_station,
                            int line_id);

    /**
     * @brief insert a valid station in a line.
     * STATION MUST BE VALID TO PREVENT UNEXPECTED TERMINATION
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     * @param line_id line id of the line to add a station in (must be an already existing line)
     * @param station_id id of the station to add (stations within a line must be unique and counting)
     * @param station_name name of the station (must be unique within a line)
     * @param seconds_to_station time in seconds to station (0 if first station)
     * @param seconds_to_next_station time in seconds to next station (0 if last station)
     */
    void insert_valid_station(sql::Connection *con,
                              sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              const char *station_name,
                              int seconds_to_station,
                              int seconds_to_next_station);
                            
    /**
     * @brief remove a valid existing station from a line
     * STATION MUST BE VALID TO PREVENT UNEXPECTED TERMINATION
     * 
     * @param stmt Used to communicate with the database
     * @param line_id line id of the line from which to remove a station
     * @param station_id station id of the station to remove in the line
     * @param is_last_station whether the station to remove is the last of the line
     * @param is_first_station whether the station to remove is the first of the list
     * @param seconds_from_last_to_next time in seconds to from previous station to station afterward (only if the station is neither the first nor last one)
     */
    void remove_valid_station(sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              bool is_last_station,
                              bool is_first_station,
                              int seconds_from_last_to_next);

    /**
     * @brief ask for a line and show all the stations from entered line
     * 
     * @param stmt Used to communicate with the database
     */
    void show_line_stations(sql::Statement *stmt);

    /**
     * @brief Complete process for the user to add a new station in a line
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void add_station(sql::Connection *con,
                     sql::Statement *stmt);

    /**
     * @brief Complete process for the user to remove a station from the network
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void remove_station(sql::Connection *con,
                        sql::Statement *stmt);

} // namespace STATIONS

#endif //STATIONS_H