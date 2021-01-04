#include "destruction.h"

namespace DESTRUCT
{
    void remove_tables(sql::Statement *stmt)
    {
        //deletes all table one by one before deleting the schema
        stmt->execute("DROP TABLE IF EXISTS " + NETWORKNAME + ".schedule;");
        stmt->execute("DROP TABLE IF EXISTS " + NETWORKNAME + ".rolling_stock;");
        stmt->execute("DROP TABLE IF EXISTS " + NETWORKNAME + ".stations;");
        stmt->execute("DROP TABLE IF EXISTS " + NETWORKNAME + ".network;");

        //delete the schema
        stmt->execute("DROP SCHEMA IF EXISTS " + NETWORKNAME + ";");
    }
} // namespace DESTRUCT
