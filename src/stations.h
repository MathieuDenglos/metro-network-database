#ifndef STATIONS_H
#define STATIONS_H

#include "constants_includes.h"

namespace STATIONS
{
    //general input, output function
    std::size_t output_stations(sql::Statement *stmt,
                                int line_id);
    std::size_t ask_station(sql::Statement *stmt,
                            sql::ResultSet **searched_station,
                            int line_id);

    //station modifications (add, alter, delete)
    void append_valid_station(sql::Connection *con,
                              sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              const char *station_name,
                              int seconds_to_station);
    void remove_valid_station(sql::Statement *stmt,
                              int line_id,
                              int station_id,
                              bool is_last_station,
                              bool is_first_station,
                              int secons_from_last_to_next);

    //connector functions
    void show_line_stations(sql::Statement *stmt);
    void remove_station(sql::Statement *stmt);

} // namespace STATIONS

#endif //STATIONS_H