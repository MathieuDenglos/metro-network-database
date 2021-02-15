#ifndef ROLLING_STOCK_H
#define ROLLING_STOCK_H

#include "constants_includes.h"

#include <string>

namespace RSPS //Rolling Stock Prepared Statements
{
    /**
     * @return all rolling stock
     */
    static const char *select_all_rolling_stock = "SELECT * FROM rolling_stock "
                                                  "ORDER BY material_id ASC";

    /**
     * @param manufacturer manufacturer name of the material
     * @return all the material with given manufacturer name
     */
    static const char *select_by_manufacturer = "SELECT * FROM rolling_stock "
                                                "WHERE manufacturer = ? "
                                                "ORDER BY material_id ASC";

    /**
     * @param model model of the material
     * @return all the material with given model
     */
    static const char *select_by_model = "SELECT * FROM rolling_stock "
                                         "WHERE model = ? "
                                         "ORDER BY material_id ASC";

    /**
     * @param material_id id of the material
     * @return the material with given id
     */
    static const char *select_by_material_id = "SELECT * FROM rolling_stock "
                                               "WHERE material_id = ? "
                                               "ORDER BY material_id ASC";

    /**
     * @param model model of the material
     * @param material_id id of the material
     * @return the material with given model and id
     */
    static const char *select_by_model_and_material_id = "SELECT * FROM network "
                                                         "WHERE model = ? "
                                                         "AND material_id = ? ;";

    /**
     * @param manufacturer manufacturer name of the material
     * @param material_id id of the material
     * @return the material with given modemanufacturer name and (material) id
     */
    static const char *select_by_manufacturer_and_material_id = "SELECT * FROM network "
                                                                "WHERE manufacturer = ? "
                                                                "AND material_id = ? ;";

} // namespace RSPS

namespace RS
{
    /**
     * @brief output all rolling_stock
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     * @param model specific model to search ("" if not a research by model)
     * @param manufacturer specific manufacturer to search ("" if not a research by manufacturer)
     * @return amount of stations in the given line 
     */
    std::size_t output_rolling_stock(sql::Connection *con,
                                     sql::Statement *stmt,
                                     std::string model = "",
                                     std::string manufacturer = "");

    /**
     * @brief ask for a list of (or a specific) train in the rolling_stock
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     * @param searched_material_id Resultset to store the trains found
     * @param unique_result whether we're searching for a unique train or potentially multiple
     * @return the amount of stations in given line with entered station name or id (normally 0 or 1)
     */
    std::size_t ask_material_id(sql::Connection *con,
                                sql::Statement *stmt,
                                sql::ResultSet **searched_material,
                                bool unique_result);

    void add_material_id(sql::Statement *stmt);

    void show_rolling_stock(sql::Statement *stmt);

    void remove_material_id(sql::Statement *stmt);

    void show_material_id(sql::Statement *stmt);
} // namespace RS

#endif //ROLLING_STOCK_H