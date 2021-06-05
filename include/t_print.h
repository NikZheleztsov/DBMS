#ifndef Table_print
#define Table_print
#include <vector>
#include <string>

class T_print {
private:

    size_t m_width = 0; // width of table 
    std::vector<std::pair<std::string, uint8_t>> header;
    std::vector<std::vector<std::string>> tuples;

    void print_hor_line ();

public:

    void push_header (std::vector<std::string> names, 
            std::vector<uint8_t> width = {0});
    void push_tuple (std::vector<std::string> tup);
    void print();
};

#endif
