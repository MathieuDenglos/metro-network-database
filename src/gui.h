#ifndef GUI_H
#define GUI_H

#include "constants_includes.h"

namespace GUI
{
    bool execute_query(sql::Connection *con,
                       sql::Statement *stmt);

    void modify_network(sql::Connection *con,
                        sql::Statement *stmt);

    void modify_rolling_stock(sql::Connection *con,
                              sql::Statement *stmt);
} // namespace GUI

#endif //GUI_H