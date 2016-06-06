#ifndef Utilities_H
#define Utilities_H

#include <string>
#include <sstream>

#include "GPIOTypes.h"
#include "I2CTypes.h"

using namespace std;

void prterr(const string &err);

string trim(const string& str,
        const string& whitespace = " \t");

template <typename T> string to_string(T val) {
    stringstream stream;
    stream << val;
    return stream.str();
}

bool getInteger(string s, int &i);
bool getLongInteger(string s, long int &i);

void replaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);

std::string replaceString(std::string subject, const std::string& search,
                          const std::string& replace);

string i2cResultStr(I2C_Result r);
string gpioResultStr(GPIO_Result r);

#endif 
