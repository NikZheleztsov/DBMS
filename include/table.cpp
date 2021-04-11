#include <vector>
#include <string>
#include "databases.h"
#include "tuple.h"
#include <unordered_map>
#include <initializer_list>
#include <stdexcept>
#include <ios>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <tuple>
#include "t_print.h"
#include <climits>
#include "table.h"

class Database;
extern Database* current_db;

void Table::select(std::vector<int8_t> col_out, // columns which have to be printed
        int8_t where_col_num, char sign, std::string cond,
        bool is_sort, int8_t sort_col_num, bool dir, bool is_write) const 
{
    T_print tb;
    std::vector<uint8_t> max_w;
    std::vector<std::vector<std::string>> out;

    for (int i = 0; i < col_names.size(); i++)
        max_w.push_back(col_names[i].size());

    for (auto x : tuple_map)
    {
        std::vector<std::string> tup = {std::to_string(x.first)};
        auto temp = x.second->print();
        int k = 1;
        for (auto y : temp)
        {
            if (y.size() > max_w[k])
                max_w[k] = y.size();
            k++;

            tup.push_back(y);
        }
        out.push_back(tup);
    }

    if (col_out[0] != -1)
    {
        for (auto x : out)
        {
            // making vector of columns which have to be deleted
            std::vector<uint8_t> col_del;
            int i = 0;
            for (auto y : col_names)
            {
                col_del.push_back(i);
                i++;
            }

            for (auto y : col_out)
                col_del.erase(std::find(col_del.begin(), col_del.end(), y));

            // deleting
            for (auto y : col_del)
                out.erase(std::find(out.begin(), out.end(), y));
        }
    }

    // need to catch all exceptions !!!
    if (where_col_num != -1)
    {
        switch (sign)
        {
            case '=':
                out.erase(std::remove_if(out.begin(), out.end(), 
                            [&where_col_num, &cond] (std::vector<std::string>& a) {
                        return !(a[where_col_num] == cond);
                            }));
                break;

            case '>':
                if (data_types[where_col_num] != "int")
                    throw;

                out.erase(std::remove_if(out.begin(), out.end(), [&where_col_num, &cond] (std::vector<std::string>& a) {
                        return (std::stoi(a[where_col_num]) <= std::stoi(cond));}));
                break;

            case '<':
                if (data_types[where_col_num] != "int")
                    throw;

                out.erase(std::remove_if(out.begin(), out.end(), [&where_col_num, &cond] (std::vector<std::string>& a) {
                        return (std::stoi(a[where_col_num]) >= std::stoi(cond));}));
                break;

            default:
                throw;
        }
    }

    tb.push_header(col_names, max_w);

    if (is_sort)
    {
        std::sort(out.begin(), out.end(), [&dir, &sort_col_num] (std::vector<std::string>& a,
                                              std::vector<std::string>& b) {
                if (dir)
                    return (a[sort_col_num] < b[sort_col_num]);
                else 
                    return (a[sort_col_num] > b[sort_col_num]);
                });
    } else {
        std::sort(out.begin(), out.end(), [] (std::vector<std::string>& a,
                    std::vector<std::string>& b) {
                    return (a[0] < b[0]);
                });
    }

    if (!out.empty())
    {
        for (auto x : out)
            tb.push_tuple(x);
        tb.print();
    }

    if (is_write && col_out[0] == -1 && table_type == 2)
    {
        // writing to a new file
    }
}

int32_t Table::get_id (std::string name)
{
    for (auto x : tuple_map)
        if (x.second->name == name)
            return x.first;
    return -1;
}

void Table::delete_tuple (uint32_t id) { tuple_map.erase(id); }

DB_table::DB_table ()
{
    table_type = 0;
    table_name = "schemata";
    col_names = {"db_id", "schema_name", "schema_type"};
    data_types = {"str", "int"};
}

void DB_table::insert(std::vector<std::string> l1,
        std::vector<uint32_t> l2)
{
    // schema_name, schema_type
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument ("DB_table");

    // CLEAN !!!
    dbases* ins = new dbases ( l1[0], l2[0]);
    tuple_map.insert({tuple_map.size(), ins });
    row_num ++;
}

/*
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
if (l1.size() != 1 || l2.size() != 2)
throw std::invalid_argument ("Tb_table");;

tb* table = new tb ( l1[0], l2[0], l2[1] );
tuple_map.insert({tuple_map.size(), table});
row_num ++;
}
};
*/


Fac_table::Fac_table ()
{
    table_type = 2;
    table_name = "faculties";
    col_names = {"fac_id", "fac_name", "nuc_name", "num_dep", "is_hybrid"};
    data_types = {"str", "str", "int", "int"};
}

void Fac_table::insert (std::vector<std::string> l1,
        std::vector<uint32_t> l2)
{
    if (l1.size() != 2 || l2.size() != 2)
        throw std::invalid_argument("Fac_table");

    fac* faculty = new fac(l1[0], l1[1], l2[0], l2[1]);
    tuple_map.insert({tuple_map.size(), faculty});
    row_num ++;
}

Dep_table::Dep_table()
{
    table_type = 3;
    table_name = "departments";
    col_names = {"dep_id", "dep_name", "fac_id"};
    data_types = {"str", "int"};
}

void Dep_table::insert (std::vector<std::string> l1,
        std::vector<uint32_t> l2)
{
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument("Dep_table");

    if (!current_db->tb_vec[0]->tuple_map.contains(l2[0]))
        throw;

    dep* department = new dep (l1[0], l2[0]);
    tuple_map.insert({tuple_map.size(), department});
    row_num++;
}

Borg_table::Borg_table()
{
    table_type = 4;
    table_name = "base_org";
    col_names = {"borg_id", "borg_name", "fac_id"};
    data_types = {"str", "int"};
}

void Borg_table::insert (std::vector<std::string> l1,
        std::vector<uint32_t> l2)
{
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument("Borg_table");

    if (!current_db->tb_vec[0]->tuple_map.contains(l2[0]))
        throw;

    borg* bg = new borg (l1[0], l2[0]);
    tuple_map.insert({tuple_map.size(), bg});
    row_num++;
}

// type 5 and 6
Dis_table::Dis_table(uint8_t type)
{
    table_type = type;
    data_types = {"str", "int", "int"};

    if (type == 5)
    {
        table_name = "disciplines_dep";
        col_names = {"dis_id", "dis_name", "num_teach", "dep_id"};
    } else if (type == 6) {
        table_name = "disciplines_borg";
        col_names = {"dis_id", "dis_name", "num_teach", "borg_id"};
    }
}

void Dis_table::insert (std::vector<std::string> l1,
        std::vector<uint32_t> l2)
{
    if (l1.size() != 1 || l2.size() != 2)
        throw std::invalid_argument("Dis_table");

    switch(table_type)
    {
        case 5:
            if (!current_db->tb_vec[1]->tuple_map.contains(l2[1]))
                throw;

        case 6: 
            if (!current_db->tb_vec[3]->tuple_map.contains(l2[1]))
                throw;
    }


    dis* discipline = new dis (l1[0], l2[0], l2[1]);
    tuple_map.insert({tuple_map.size(), discipline});
    row_num++;
}
