#ifndef NETWORK_H
#define NETWORK_H

#include "constants_includes.h"

namespace
{
    void new_line_station_adding(sql::Connection *con,
                                 sql::Statement *stmt,
                                 int line_id,
                                 int station_id,
                                 int seconds_to_station);
} // namespace

namespace NETWORK
{
    std::size_t output_lines(sql::Statement *stmt);

    void new_line(sql::Connection *con,
                  sql::Statement *stmt);

    void remove_line(sql::Connection *con,
                     sql::Statement *stmt);
} // namespace NETWORK

#endif //NETWORK_H