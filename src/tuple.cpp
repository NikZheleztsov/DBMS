#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <utility>
#include <iomanip>
#include "tuple.h"

class Database;

dbases::dbases(std::fstream& in) {

    size = 33;
    char* name = new char [32];
    in.read(name, 32);
    uint8_t type = 0;
    in.read(reinterpret_cast<char*>(&type), 1);

    this->name = name;
    schema_type = type;
    delete [] name;
}

void dbases::write(std::fstream& out)
{
    out.write(name.c_str(), name.size());
    char* bl = new char [32 - name.size()] ();
    out.write(bl, 32 - name.size());
    delete [] bl;
    out.write(reinterpret_cast<char*>(&schema_type), 1);
}

std::pair<std::vector<std::string>,
    std::vector<uint32_t>> dbases::get_data() const
{
    std::vector<std::string> l1 = {name};
    std::vector<uint32_t> l2 = {schema_type};
    return std::make_pair(l1,l2);
}

std::vector<std::string> dbases::print () const 
{
    std::vector<std::string> out = 
    {name, std::to_string(schema_type)};

    return out;
}

/*
   class tb : public tuple
   {
//std::string table_name;
uint32_t rows_num;
uint32_t db_id;

public: 

tb (std::fstream& in) {

size = 40;
char* name = new char [32];
in.read(name, 32);
uint32_t rows = 0, id = 0;
in.read(reinterpret_cast<char*>(&rows), 4);
in.read(reinterpret_cast<char*>(&id), 4);

this->name = name;
rows_num = rows;
db_id = id;
delete [] name;
}

tb (std::string n, uint32_t rows, uint32_t id) : tuple(40, n), rows_num(rows), db_id(id) {}

void write(std::fstream& out) override
{
out.write(name.c_str(), name.size());
char* bl = new char [32 - name.size()] ();
out.write(bl, 32 - name.size());
delete [] bl;
out.write(reinterpret_cast<char*>(&rows_num), 4);
out.write(reinterpret_cast<char*>(&db_id), 4);
}

void print (uint8_t width, std::vector <uint8_t> num, int8_t num_of_col, 
char sign, int64_t cond) const override
{
if (num_of_col == -1 || sign == ' ' || cond == -1)
std::cout << std::setw(width) << name <<
std::setw(width) << rows_num << 
std::setw(width) << db_id << std::endl;
}
};
*/

// faculty
fac::fac (std::fstream& in) {

    size = 69;
    char* name = new char [32];
    in.read(name, 32);
    this->name = name;

    in.read(name, 32);
    name_nuc = name;
    delete[] name;

    uint32_t num_dep = 0;
    bool spec = false;
    in.read(reinterpret_cast<char*>(&num_dep), 4);
    in.read(reinterpret_cast<char*>(&spec), 1);
    this->num_dep = num_dep;
    is_spec = spec;
}

void fac::write(std::fstream& out)
{
    out.write(name.c_str(), name.size());
    char* bl = new char [32 - name.size()] ();
    out.write(bl, 32 - name.size());
    delete [] bl;

    out.write(name_nuc.c_str(), name.size());
    bl = new char [32 - name_nuc.size()] ();
    out.write(bl, 32 - name_nuc.size());
    delete [] bl;

    out.write(reinterpret_cast<char*>(&num_dep), 4);
    out.write(reinterpret_cast<char*>(&is_spec), 1);
}

std::pair<std::vector<std::string>,
    std::vector<uint32_t>> fac::get_data() const 
{
    std::vector<std::string> l1 = {name, name_nuc};
    std::vector<uint32_t> l2 = {num_dep, is_spec};
    return std::make_pair(l1,l2);
}

std::vector<std::string> fac::print () const 
{
    std::vector<std::string> out = 
    {name, name_nuc, std::to_string(num_dep),
        std::to_string(is_spec)};

    return out;
}

dep::dep (std::fstream& in) {

    size = 36;
    char* name = new char [32];
    in.read(name, 32);
    uint32_t fac = 0;
    in.read(reinterpret_cast<char*>(&fac), 4);

    this->name = name;
    for_key = fac;
    delete [] name;
}

void dep::write(std::fstream& out) 
{
    out.write(name.c_str(), name.size());
    char* bl = new char [32 - name.size()] ();
    out.write(bl, 32 - name.size());
    delete [] bl;

    out.write(reinterpret_cast<char*>(&for_key), 4);
}

std::pair<std::vector<std::string>,
    std::vector<uint32_t>> dep::get_data() const 
{
    uint32_t key = for_key;
    std::vector<std::string> l1 = {name};
    std::vector<uint32_t> l2 = {key};
    return std::make_pair(l1,l2);
}

std::vector<std::string> dep::print () const 
{
    std::vector<std::string> out = 
    {name, std::to_string(for_key)};

    return out;
}

borg::borg (std::fstream& in) {

    size = 36;
    char* name = new char [32];
    in.read(name, 32);
    uint32_t fac = 0;
    in.read(reinterpret_cast<char*>(&fac), 4);

    this->name = name;
    for_key = fac;
    delete [] name;
}

void borg::write(std::fstream& out)
{
    out.write(name.c_str(), name.size());
    char* bl = new char [32 - name.size()] ();
    out.write(bl, 32 - name.size());
    delete [] bl;

    out.write(reinterpret_cast<char*>(&for_key), 4);
}

std::pair<std::vector<std::string>,
    std::vector<uint32_t>> borg::get_data() const 
{
    uint32_t key = for_key;
    std::vector<std::string> l1 = {name};
    std::vector<uint32_t> l2 = {key};
    return std::make_pair(l1,l2);
}

std::vector<std::string> borg::print () const
{
    std::vector<std::string> out = 
    {name, std::to_string(for_key)};

    return out;
}

dis::dis (std::fstream& in) {

    size = 40;
    char* name = new char [32];
    in.read(name, 32);
    uint32_t teach = 0, id = 0;
    in.read(reinterpret_cast<char*>(&teach), 4);
    in.read(reinterpret_cast<char*>(&id), 4);

    this->name = name;
    dis_teach_num = teach;
    for_key = id;
    delete [] name;
}

void dis::write(std::fstream& out)
{
    out.write(name.c_str(), name.size());
    char* bl = new char [32 - name.size()] ();
    out.write(bl, 32 - name.size());
    delete [] bl;

    out.write(reinterpret_cast<char*>(&dis_teach_num), 4);
    out.write(reinterpret_cast<char*>(&for_key), 1);
}

std::pair<std::vector<std::string>,
    std::vector<uint32_t>> dis::get_data() const 
{
    uint32_t key = for_key;
    std::vector<std::string> l1 = {name};
    std::vector<uint32_t> l2 = {dis_teach_num, key};
    return std::make_pair(l1,l2);
}

std::vector<std::string> dis::print () const 
{
    std::vector<std::string> out = 
    {name, std::to_string(dis_teach_num),
        std::to_string(for_key)};

    return out;
}
