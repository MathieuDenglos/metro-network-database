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
    static const char *select_by_model_and_material_id = "SELECT * FROM rolling_stock "
                                                         "WHERE model = ? "
                                                         "AND material_id = ? ;";

    /**
     * @param manufacturer manufacturer name of the material
     * @param material_id id of the material
     * @return the material with given modemanufacturer name and (material) id
     */
    static const char *select_by_manufacturer_and_material_id = "SELECT * FROM rolling_stock "
                                                                "WHERE manufacturer = ? "
                                                                "AND material_id = ? ;";

    /**
     * @brief Insert a newmaterial in the network
     * 
     * @param material_id id of the new material (must be unique)
     * @param manufacturer name of the material manufacturer
     * @param model 
     */
    static const char *insert_material = "INSERT INTO rolling_stock(material_id, manufacturer, model) "
                                         "VALUES (?, ?, ?)";

    /**
     * @brief Delete a station from a line
     * 
     * @param line_id id of the line where the station to delete is
     * @param station_id id of the station to delete
     */
    static const char *delete_material = "DELETE FROM rolling_stock "
                                         "WHERE material_id = ? ;";

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
    std::size_t ask_material(sql::Connection *con,
                             sql::Statement *stmt,
                             sql::ResultSet **searched_material,
                             bool unique_result);

    /**
     * @brief Complete process for the user to add a new material in the rolling stock
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void add_material(sql::Connection *con,
                      sql::Statement *stmt);

    /**
     * @brief Complete process for the user to remove a material from the rolling stock
     * 
     * @param con Used to execute the prepared statements
     * @param stmt Used to communicate with the database
     */
    void remove_material(sql::Connection *con,
                         sql::Statement *stmt);
} // namespace RS

#endif //ROLLING_STOCK_H