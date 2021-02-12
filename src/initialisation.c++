#include "initialisation.h"
#include "informations.h" //to remove in case of a global usage

#include <iostream>

namespace INI
{
    void connection(sql::mysql::MySQL_Driver **driver,
                    sql::Connection **con,
                    sql::Statement **stmt)
    {
        *driver = sql::mysql::get_mysql_driver_instance();
        *con = (*driver)->connect(port, username, password); //port; username; password to change
        *stmt = (*con)->createStatement();
    }

    void verify_tables_existence(sql::Connection **con,
                                 sql::Statement *stmt)
    {
        //first we create the database if it doesn't exists
        stmt->execute("CREATE SCHEMA IF NOT EXISTS " + NETWORKNAME + ";");
        (*con)->setSchema(NETWORKNAME);

        //Now make the verification for each table needed
        stmt->execute("CREATE TABLE IF NOT EXISTS network("
                      "line_id INT PRIMARY KEY,"
                      "line_name VARCHAR(20)"
                      ");");

        stmt->execute("CREATE TABLE IF NOT EXISTS stations("
                      "   line_id INT,"
                      "   station_id INT,"
                      "   station_name VARCHAR(20) NOT NULL,"
                      "   travel_time_to_next_station INT,"
                      "   PRIMARY KEY(line_id, station_id),"
                      "   FOREIGN KEY(line_id) REFERENCES network(line_id) ON DELETE CASCADE"
                      ");");

        stmt->execute("CREATE TABLE IF NOT EXISTS rolling_stock("
                      "   material_id INT PRIMARY KEY,"
                      "   manufacturer VARCHAR(20),"
                      "   model VARCHAR(20)"
                      ");");

        stmt->execute("CREATE TABLE IF NOT EXISTS schedule("
                      "   line_id INT,"
                      "   station_id INT,"
                      "   arrival_time TIMESTAMP,"
                      "   material_id INT,"
                      "   FOREIGN KEY(material_id) REFERENCES " +
                      NETWORKNAME + ".rolling_stock(material_id) ON DELETE CASCADE,"
                                    "   FOREIGN KEY(line_id, station_id) REFERENCES " +
                      NETWORKNAME + ".stations(line_id, station_id) ON DELETE CASCADE"
                                    ");");
    }
} // namespace INI