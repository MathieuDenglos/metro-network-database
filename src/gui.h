#ifndef GUI_H
#define GUI_H

#include "constants_includes.h"

namespace GUI
{
    bool execute_querry(sql::Connection *con,
                        sql::Statement *stmt);

    void modify_network(sql::Connection *con,
                        sql::Statement *stmt);
} // namespace GUI

#endif //GUI_H