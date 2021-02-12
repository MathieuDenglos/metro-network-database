#ifndef DESTRUCTION_H
#define DESTRUCT_H

#include "constants_includes.h"

namespace DESTRUCT
{
    /**
     * @brief function to remove all the tables and database.
     * /!\ CALLING THIS FUNCTION ERASE ALL DATA FOREVER.
     * Function mainly for test purposes
     * 
     * @param stmt Used to communicate with the database
     */
    void remove_tables(sql::Statement *stmt);
} // namespace DESTRUCT

#endif //DESTRUCT_H