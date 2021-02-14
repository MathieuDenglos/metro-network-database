#ifndef NETWORK_H
#define NETWORK_H

#include "constants_includes.h"

namespace NPS //Network Prepared Statement
{
    /**
     * @return all lines in the network
     */
    static const char *select_all_lines = "SELECT * FROM network "
                                          "ORDER BY line_id ASC";

    /**
     * @param line_id id of the searched line
     * @return all the lines in the network with the given line id
     */
    static const char *select_by_line_id = "SELECT * FROM network "
                                           "WHERE line_id = ? ;";

    /**
     * @param line_id name of the searched line
     * @return all the lines in the network with the given line name
     */
    static const char *select_by_line_name = "SELECT * FROM network "
                                             "WHERE line_name = ? ;";

    /**
     * @brief Insert a new line in the network
     * 
     * @param line_id id of the new line (must be unique)
     * @param line_name name of the new line (must be unique)
     */
    static const char *insert_line = "INSERT INTO network(line_id, line_name) "
                                     "VALUES (?, ?)";

    /**
     * @brief Delete a line from the network
     * 
     * @param line_id id of the line to delete
     */
    static const char *delete_line = "DELETE FROM network "
                                     "WHERE line_id = ? ;";

} // namespace NPS

namespace
{
    /**
     * @brief Complete process for the user to add the first stations of a newly created line.
     *  Works recursively, calls itself until the user added all the needed stations
     * SHOULD ONLY BE CALLED WITH AN EMPTY LINE.
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     * @param line_id id of the line to add stations in
     * @param station_id id of the station (starts at 1, automatically increment at each recursive call)
     * @param seconds_to_station time in seconds to the station that's about to be added (0 for the first call)
     */
    void add_stations_in_new_line(sql::Connection *con,
                                  sql::Statement *stmt,
                                  int line_id,
                                  int station_id = 1,
                                  int seconds_to_station = 0);
} // namespace

namespace NETWORK
{
    /**
     * @brief output all the lines of the network
     * 
     * @param stmt Used to communicate with the database
     * @return the amount of lines in the network
     */
    std::size_t output_lines(sql::Statement *stmt);

    /**
     * @brief Ask the user for a specific line. 
     * search work with station name and id which are both unique
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     * @param searched_line a pointer to the ResultSet to store the result of the query
     * @return the amount of lines found with entered criteria (normally 0 or 1)
    */
    std::size_t ask_line(sql::Connection *con,
                         sql::Statement *stmt,
                         sql::ResultSet **searched_line);

    /**
     * @brief Complete process for the user to add a new line to the network
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void add_line(sql::Connection *con,
                  sql::Statement *stmt);

    /**
     * @brief Complete process for the user to remove a line from the network
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void remove_line(sql::Connection *con,
                     sql::Statement *stmt);
} // namespace NETWORK

#endif //NETWORK_H