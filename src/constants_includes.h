#ifndef CONSTANTS_INCLUDES_H
#define CONSTANTS_INCLUDES_H

#include <cppconn/driver.h>
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

static const std::string NETWORKNAME = "metro_system"; //cannot contain spaces
static const int STATIONSTOPTIME = 45;                 //station stop time in seconds
// static const char *port = "";
// static const char *username = "";
// static const char *password = "";
//to add in a case of a global use

#endif //CONSTANTS_INCLUDES_H