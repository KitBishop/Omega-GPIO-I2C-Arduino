#include <iostream>
#include <cstdlib>

#include "Utilities.h"

using namespace std;

void prterr(const string &err) {
    cerr << "**ERROR** " << err << endl;
}

string trim(const string& str,
        const string& whitespace) {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

bool getInteger(string s, int &i) {
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   i = strtol(s.c_str(), &p, 0) ;

   return (*p == 0) ;
}

bool getLongInteger(string s, long int &i) {
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   i = strtol(s.c_str(), &p, 0) ;

   return (*p == 0) ;
}

void replaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

std::string replaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

string i2cResultStr(I2C_Result r) {
    switch (r) {
        case I2C_OK:
            return "I2C_OK";
            
        case I2C_BAD_ACCESS:
            return "I2C_BAD_ACCESS";
            
        case I2C_BAD_DEV:
            return "I2C_BAD_DEV";
            
        case I2C_BAD_WRITE:
            return "I2C_BAD_WRITE";
            
        case I2C_BAD_READ:
            return "I2C_BAD_READ";
            
        case I2C_BAD_READ_ADDR:
            return "I2C_BAD_READ_ADDR";
            
        case I2C_BAD_READ_LEN:
            return "I2C_BAD_READ_LEN";
            
        case I2C_BAD_PROBE:
            return "I2C_BAD_PROBE";
    }
    
    return "I2C unknown";
}

string gpioResultStr(GPIO_Result r) {
    switch (r) {
        case GPIO_OK:
            return "GPIO_OK";
            
        case GPIO_BAD_ACCESS:
            return "GPIO_BAD_ACCESS";
            
        case GPIO_INVALID_PIN:
            return "GPIO_INVALID_PIN";
            
        case GPIO_INVALID_OP:
            return "GPIO_INVALID_OP";
            
        case GPIO_TIME_OUT:
            return "GPIO_TIME_OUT";
    }
    
    return "GPIO unknown";
}
