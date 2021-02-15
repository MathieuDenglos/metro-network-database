#include "rolling_stock.h"
#include "other.h"

#include <iostream>
#include <iomanip>

namespace RS
{
    std::size_t output_rolling_stock(sql::Connection *con,
                                     sql::Statement *stmt,
                                     std::string model /* = "" */,
                                     std::string manufacturer /* = "" */)
    {
        //first grab all the rolling stock
        sql::ResultSet *existing_rolling_stock;
        if (manufacturer != "")
        {
            sql::PreparedStatement *select_by_manufacturer = con->prepareStatement(RSPS::select_by_manufacturer);
            select_by_manufacturer->setString(1, manufacturer);
            existing_rolling_stock = select_by_manufacturer->executeQuery();
            delete select_by_manufacturer;
        }
        else if (model != "")
        {
            sql::PreparedStatement *select_by_model = con->prepareStatement(RSPS::select_by_model);
            select_by_model->setString(1, model);
            existing_rolling_stock = select_by_model->executeQuery();
            delete select_by_model;
        }
        else
            existing_rolling_stock = stmt->executeQuery(RSPS::select_all_rolling_stock);
        std::size_t train_count = existing_rolling_stock->rowsCount();

        std::cout << "\e[1;1H\e[2J" << std::flush;

        //output the existing lines or a message if there aren't any/any with given research
        if (train_count == 0 && model != "")
            std::cout
                << "No rolling stocks of given model\n\n";
        else if (train_count == 0 && manufacturer != "")
            std::cout
                << "No rolling stocks of given manufacturer name\n\n";
        else if (train_count == 0)
            std::cout
                << "No material in the rolling stock yet\n\n";
        else
            std::cout << "material_id   model            manufacturer\n";

        while (existing_rolling_stock->next())
            std::cout << "    " << std::setw(4) << std::right << std::setfill('0') << existing_rolling_stock->getInt("material_id")
                      << "      " << std::setw(16) << std::left << std::setfill(' ') << existing_rolling_stock->getString("model")
                      << " " << std::setw(12) << std::left << std::setfill(' ') << existing_rolling_stock->getString("manufacturer") << '\n';

        std::cout << std::flush;

        delete existing_rolling_stock;
        return train_count;
    }

    std::size_t ask_material(sql::Connection *con,
                             sql::Statement *stmt,
                             sql::ResultSet **searched_material,
                             bool unique_result)
    {
        //Ask for the searched train
        std::cout << "Enter the model, or train ID\n"
                  << std::flush;
        std::string reply = IO::get_string();

        try
        {
            //convert the reply to an int and search by rolling_stock_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            sql::PreparedStatement *select_by_material_id = con->prepareStatement(RSPS::select_by_material_id);
            select_by_material_id->setInt(1, searched_id);
            *searched_material = select_by_material_id->executeQuery();
            delete select_by_material_id;
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by model or manufacturer
            sql::PreparedStatement *select_by_manufacturer = con->prepareStatement(RSPS::select_by_manufacturer),
                                   *select_by_model = con->prepareStatement(RSPS::select_by_model);
            select_by_manufacturer->setString(1, reply);
            select_by_model->setString(1, reply);
            *searched_material = select_by_model->executeQuery();
            if ((*searched_material)->rowsCount() == 0)
                *searched_material = select_by_manufacturer->executeQuery();
            delete select_by_manufacturer, select_by_model;
        }
        catch (const std::out_of_range &e)
        {
            //if the given value is too big, return an empty list
            *searched_material = stmt->executeQuery("SELECT NULL LIMIT 0;");
        }

        //if we search for a single result and we found multiple, ask the user to input the material id
        if (unique_result && (*searched_material)->rowsCount() > 1)
        {
            (*searched_material)->next();

            //output all the rolling_stock with the searched model or manufacturer
            RS::output_rolling_stock(con, stmt,
                                     ((*searched_material)->getString("model") == reply) ? reply : "",
                                     ((*searched_material)->getString("manufacturer") == reply) ? reply : "");

            std::cout << "\nmultiple trains selected in a unique query, here are the different trains selected"
                      << "\nplease enter the train id from above list!\n"
                      << std::flush;
            int material_id = IO::get_int();

            //If the user did a research by model → ask for a material id and search by the model and the material id
            if ((*searched_material)->getString("model") == reply)
            {
                sql::PreparedStatement *select_by_model_and_material_id = con->prepareStatement(RSPS::select_by_model_and_material_id);
                select_by_model_and_material_id->setString(1, reply);
                select_by_model_and_material_id->setInt(2, material_id);
                *searched_material = select_by_model_and_material_id->executeQuery();
                delete select_by_model_and_material_id;
            }
            //If the user did a research by manufacturer → ask for a material id and search by the manufacturer and the material id
            else
            {
                sql::PreparedStatement *select_by_manufacturer_and_material_id = con->prepareStatement(RSPS::select_by_manufacturer_and_material_id);
                select_by_manufacturer_and_material_id->setString(1, reply);
                select_by_manufacturer_and_material_id->setInt(2, material_id);
                *searched_material = select_by_manufacturer_and_material_id->executeQuery();
                delete select_by_manufacturer_and_material_id;
            }
        }
        return (*searched_material)->rowsCount();
    }

    void add_material(sql::Connection *con,
                      sql::Statement *stmt)
    {
        //output the rolling stock
        RS::output_rolling_stock(con, stmt);

        std::cout << "\n\nPlease provide an id to the new material (has to be unique\n"
                  << std::flush;

        //ask for the id of the new material
        int new_material_id;
        sql::PreparedStatement *select_by_material_id = con->prepareStatement(RSPS::select_by_material_id);
        sql::ResultSet *material_with_id;
        do
        {
            new_material_id = IO::get_int();
            select_by_material_id->setInt(1, new_material_id);
            material_with_id = select_by_material_id->executeQuery();
            if (material_with_id->rowsCount() != 0)
                std::cout << "this material already exists please select another material id\n"
                          << std::flush;
        } while (material_with_id->rowsCount() != 0);
        delete select_by_material_id;
        delete material_with_id;

        //ask for the model of the new material
        std::cout << "\ninput the model of the material with id : " << new_material_id
                  << std::endl;
        std::string new_model = IO::get_string();

        //ask for the manufacturer of the new material
        std::cout << "\ninput the manufacturer of the material with id : " << new_material_id
                  << std::endl;
        std::string new_manufacturer = IO::get_string();

        //Insert the new material with all the user input data
        sql::PreparedStatement *insert_material = con->prepareStatement(RSPS::insert_material);
        insert_material->setInt(1, new_material_id);
        insert_material->setString(2, new_manufacturer);
        insert_material->setString(3, new_model);
        insert_material->execute();
        delete insert_material;
    }

    void remove_material(sql::Connection *con,
                         sql::Statement *stmt)
    {
        sql::ResultSet *searched_material;

        //ask for the material to remove from the list
        RS::output_rolling_stock(con, stmt);
        std::cout << "\n\nSelect the material to delete\n"
                  << std::flush;
        if (RS::ask_material(con, stmt, &searched_material, true))
        {
            //if a material was found delete it
            searched_material->next();
            sql::PreparedStatement *delete_material = con->prepareStatement(RSPS::delete_material);
            delete_material->setInt(1, searched_material->getInt("material_id"));
            delete_material->execute();
            delete delete_material;
        }
        //otherwise just give an error message and return
        else
            std::cout << "No material to delete with given information"
                      << std::flush;

        delete searched_material;
    }
} // namespace RS