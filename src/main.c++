#include "constants_includes.h"
#include "initialisation.h"
#include "destruction.h"
#include "gui.h"
#include <iostream>

int main()
{
    sql::mysql::MySQL_Driver *driver = nullptr;
    sql::Connection *con = nullptr;
    sql::Statement *stmt = nullptr;

    try
    {
        //Initialisation (connection et creation tables)
        INI::connection(&driver, &con, &stmt);
        INI::verify_tables_existence(&con, stmt);

        //querries executions
        while (GUI::execute_query(con, stmt))
            ;

        //to destroy the tables (mainly dev purposes)
        //DESTRUCT::remove_tables(stmt);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "\n# ERR: SQLException in " << __FILE__
                  << "(" << __FUNCTION__ << ") on line " << __LINE__ << '\n'
                  << "# ERR: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << " )\n"
                  << std::flush;
    }

    delete con;
    delete stmt;
}