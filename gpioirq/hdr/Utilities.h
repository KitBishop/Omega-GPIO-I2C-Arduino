#ifndef Utilities_H
#define Utilities_H

#include <string>
#include <sstream>

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

#endif 
