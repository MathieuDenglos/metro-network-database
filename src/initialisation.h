#ifndef INITIALISATION_H
#define INITIALISATION_H

#include "constants_includes.h"

namespace INI
{
    void connection(sql::mysql::MySQL_Driver **driver,
                    sql::Connection **con,
                    sql::Statement **stmt);

    void verify_tables_existance(sql::Connection **con,
                                 sql::Statement *stmt);
} // namespace INI

#endif //INITIALISATION_H
