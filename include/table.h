#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <string>
#include "tuple.h"
#include <unordered_map>
#include <initializer_list>

class Database;

// interface
class Table
{
protected:
    // Database Meta
    // num of data blocks

    // <= 64 bytes
    uint32_t table_type;
    std::string table_name;
    // <= 32 bytes
    std::vector <std::string> col_names;
    std::unordered_map<uint32_t, tuple*> tuple_map;
    uint32_t row_num = 0;

public:
//    virtual void print() const = 0;
//   virtual void del (uint32_t key) = 0;
//    virtual void sort_n_print (std::string col_n, bool dir) = 0;
    virtual void insert (std::vector<std::string>,
                         std::vector<uint32_t> ) = 0;

    // all kind of interface to one table
    
    friend void db_first_write (Database&);
};

class DB_table : public Table
{
    // dbases

public: 

    DB_table ()
    {
        table_type = 0;
        table_name = "SCHEMATA";
        col_names = {"db_id", "schema_name", "schema_type"};
    }

    void insert(std::vector<std::string> l1,
                std::vector<uint32_t> l2) override
    {
        // schema_name, schema_type
        if (l1.size() != 1 && l2.size() != 1)
            throw -1;

        // CLEAN !!!
        dbases* ins = new dbases ( l1[0], l2[0]);
        tuple_map.insert({tuple_map.size(), ins });
        row_num ++;
    }

//    void print() const
};

class Tb_table : public Table
{
    //tb
    
public:

    Tb_table ()
    {
        table_type = 1;
        table_name = "TABLES";
        col_names = {"table_id", "table_name", "rows_num", "db_id"};
    }


    void insert(std::vector<std::string> l1,
                std::vector<uint32_t> l2) override
    {
        if (l1.size() != 1 && l2.size() != 2)
            throw -1;

        // + 1 row
        if (l1[0] == table_name)
            l2[0]++; 

        tb* table = new tb ( l1[0], l2[0], l2[1] );
        tuple_map.insert({tuple_map.size(), table});
        row_num ++;
    }
};


class Fac_table : public Table
{
    //fac
};

class Dep_table : public Table
{
    // dep
};

class Borg_table : public Table
{
    // borg
};

class Dis_table : public Table
{
    // dis
};

#endif 
