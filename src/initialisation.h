#ifndef INITIALISATION_H
#define INITIALISATION_H

#include "constants_includes.h"

namespace INI
{
    /**
     * @brief connect to the database using the informations entered in constants_includes.h
     * 
     * @param driver Driver necessary to connect
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void connection(sql::mysql::MySQL_Driver **driver,
                    sql::Connection **con,
                    sql::Statement **stmt);

    /**
     * @brief Make sure that the database and all the tables are created
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void verify_tables_existence(sql::Connection **con,
                                 sql::Statement *stmt);
} // namespace INI

#endif //INITIALISATION_H
