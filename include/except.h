#ifndef EXCEPT_H
#define EXCEPT_H

#include <exception>
#include <string>

class dbms_except : public std::exception 
{
    std::string output;

public:

    dbms_except (std::string msg = "") : 
        output(msg) {}

    virtual const char* what() 
        const noexcept { return output.c_str(); }
};

#endif
