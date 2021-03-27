#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <string>
#include "tuple.h"
#include <unordered_map>

// interface
class Table
{
protected:
    // Database Meta
    // num of data blocks

    std::string table_name;
    std::vector <std::string> col_names;
    uint8_t col_num;
    uint32_t row_num = 0;

public:
//    virtual void print() const = 0;
//   virtual void del (uint32_t key) = 0;
//    virtual void sort_n_print (std::string col_n, bool dir) = 0;
    virtual void insert (std::string data) = 0;

    // all kind of interface to one table
};

class DB_table : public Table
{
    std::unordered_map<uint32_t, dbases> db_map;

public: 

    DB_table ()
    {
        table_name = "schemata";
        col_num = 3;
        col_names = {"db_id", "schema_name", "schema_type"};
    }

    void insert(std::string data) override
    {
        // parsing
        int space = data.find(' ');
        std::string sch_name = data.substr(0, space);
        uint8_t sch_type= std::stoi(data.substr(space + 1, data.size() - space - 1));

        dbases ins = { sch_name, sch_type };
        db_map.insert({db_map.size() + 1, ins });
        row_num ++;
    }

//    void print() const
};

class Tb_table : public Table
{
    std::unordered_map<uint32_t, tb> tb_map;

    public:
        void insert(std::string data) override {};
};


class Fac_table : public Table
{
    std::unordered_map<uint32_t, fac> fac_map;
};

class Dep_table : public Table
{
    std::unordered_map<uint32_t, dep> dep_map;
};

class Borg_table : public Table
{
    std::unordered_map<uint32_t, borg> borg_map;
};

class Dis_table : public Table
{
    std::unordered_map<uint32_t, dis> dis_map;
};

#endif 
