#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include "tuple.h"
#include <unordered_map>

// interface
class Table
{
protected:
    uint8_t col_num;
    std::vector <std::string> col_names;
    uint32_t row_num;

public:
    virtual void print() const = 0;
    virtual void del (uint32_t key) = 0;
    virtual void sort_n_print (std::string col_n, bool dir) = 0;

    // all kind of interface to one table
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

class DB_table : public Table
{
    std::unordered_map<uint32_t, dbases> db_map;

public: 

    DB_table () : 

    void insert(std::string sch_name, bool sch_type)
    {
        dbases ins = { sch_name, sch_type };
        db_map.insert( {db_map.size() + 1, ins });
        row_num ++;
    }

    void print() const
};

class Tb_table : public Table
{
    std::unordered_map<uint32_t, tb> tb_map;
};

#endif 
