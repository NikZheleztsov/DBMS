#include <exception>
#include <cstdint>
#include <string>

class dbms_except : public std::exception 
{
    uint8_t err_num;
    std::string output;

public:

    dbms_except (uint8_t num = 0, std::string msg = "") : err_num (num)
    {
        switch (err_num)
        {
            case 1: 
                output = "Error while creating TB_table. ";

            default :
                output = "Default exception. ";
        }
            if (msg != "")
                output += "Additional info: " + msg;
    }

    virtual const char* what() const noexcept
    {
        return output.c_str();
    }

    uint8_t get_err_num ()
    {
        return err_num;
    }
};

