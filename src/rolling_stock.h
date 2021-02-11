#ifndef ROLLING_STOCK_H
#define ROLLING_STOCK_H

#include "constants_includes.h"

namespace RS
{
    void add_train(sql::Statement *stmt);

    void show_rolling_stocks(sql::Statement *stmt);

    void remove_train(sql::Statement *stmt);

    void show_train(sql::Statement *stmt);
} // namespace RS

#endif //ROLLING_STOCK_H