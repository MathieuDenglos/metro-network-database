#ifndef STATIONS_H
#define STATIONS_H

#include "constants_includes.h"

namespace STATIONS
{
    std::size_t output_stations(sql::Statement *stmt,
                                int line_id);

    void append_station(sql::Connection *con,
                        sql::Statement *stmt,
                        int line_id,
                        int station_id,
                        const char *station_name,
                        int seconds_to_station);
} // namespace STATIONS

#endif //STATIONS_H