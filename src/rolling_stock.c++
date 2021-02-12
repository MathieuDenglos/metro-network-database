#include "rolling_stock.h"

#include <iostream>
#include <iomanip>

namespace RS
{
    std::size_t output_rolling_stocks(sql::Statement *stmt,
                                      std::string model /* = "" */,
                                      std::string manufacturer /* = "" */)
    {
        //first grab all the rolling stock
        sql::ResultSet *existing_rolling_stock;
        if (manufacturer != "")
            existing_rolling_stock = stmt->executeQuery("SELECT * FROM rolling_stock WHERE manufacturer = " + manufacturer + " ORDER BY material_id ASC");
        else if (model != "")
            existing_rolling_stock = stmt->executeQuery("SELECT * FROM rolling_stock WHERE model = " + model + " ORDER BY material_id ASC");
        else
            existing_rolling_stock = stmt->executeQuery("SELECT * FROM rolling_stock ORDER BY material_id ASC");
        std::size_t train_count = existing_rolling_stock->rowsCount();

        //output the existing lines or a message if there aren't any
        if (train_count == 0)
            std::cout << "No rolling stocks yet\n\n";

        else
            std::cout << "The rolling_stock already have " << train_count << " trains\n\n"
                      << "train_id   model            manufacturer\n";

        while (existing_rolling_stock->next())
            std::cout << "    " << std::setw(5) << std::right << std::setfill(' ') << existing_rolling_stock->getInt("material_id")
                      << "  " << std::setw(16) << std::left << std::setfill(' ') << existing_rolling_stock->getString("model")
                      << "  " << std::setw(12) << std::left << std::setfill(' ') << existing_rolling_stock->getString("manufacturer") << '\n';

        std::cout << std::flush;

        delete existing_rolling_stock;
        return train_count;
    }

    std::size_t ask_train(sql::ResultSet **search_train,
                          sql::Statement *stmt,
                          bool unique_result)
    {
        //Ask for the searched train
        std::string reply;
        std::cout << "Enter the model, or train ID\n"
                  << std::flush;
        std::cin >> reply;

        try
        {
            //convert the reply to an int and search by line_id if std::stoi() doesn't throw
            int searched_id = std::stoi(reply);
            *search_train = stmt->executeQuery("SELECT * FROM network WHERE material_id = " + reply + " ;");
        }
        catch (const std::invalid_argument &e)
        {
            //if the function throw, it means that the user did a research by line_name
            *search_train = stmt->executeQuery("SELECT * FROM network WHERE model = \'" + reply + "\' ;");
        }
        catch (const std::out_of_range &e)
        {
            //if the given value is too big, return an empty list
            *search_train = stmt->executeQuery("SELECT NULL LIMIT 0;");
        }

        if (unique_result && (*search_train)->rowsCount() <= 1)
        {
            (*search_train)->next();
            int material_id;
            std::cout << "multiple trains selected in a unique query, here are the different trains selected\n"
                      << std::flush;

            RS::output_rolling_stocks(stmt,
                                      ((*search_train)->getString("model") == reply) ? reply : "",
                                      ((*search_train)->getString("manufacturer") == reply) ? reply : "");

            std::cout << "please enter the train id from above list!\n"
                      << std::flush;

            std::cin >> material_id;

            if ((*search_train)->getString("model") == reply)
                *search_train = stmt->executeQuery("SELECT * FROM network WHERE model = \'" + reply + "\' AND material_id = " + std::to_string(material_id) + " ;");
            else
                *search_train = stmt->executeQuery("SELECT * FROM network WHERE manufacturer = \'" + reply + "\' AND material_id = " + std::to_string(material_id) + " ;");
        }

        return (*search_train)->rowsCount();
    }

    void add_train(sql::Statement *stmt)
    {
    }

    void show_rolling_stocks(sql::Statement *stmt)
    {
    }

    void remove_train(sql::Statement *stmt)
    {
    }

    void show_train(sql::Statement *stmt)
    {
    }
} // namespace RS