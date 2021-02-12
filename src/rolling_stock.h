#ifndef ROLLING_STOCK_H
#define ROLLING_STOCK_H

#include "constants_includes.h"

#include <string>

namespace RS
{
    std::size_t output_rolling_stocks(sql::Statement *stmt,
                                      std::string model = "",
                                      std::string manufacturer = "");

    std::size_t ask_train(sql::ResultSet **search_train,
                          sql::Statement *stmt,
                          bool unique_result);

    void add_train(sql::Statement *stmt);

    void show_rolling_stocks(sql::Statement *stmt);

    void remove_train(sql::Statement *stmt);

    void show_train(sql::Statement *stmt);
} // namespace RS

#endif //ROLLING_STOCK_H