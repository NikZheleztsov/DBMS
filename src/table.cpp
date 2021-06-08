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
#include <filesystem>

namespace fs = std::filesystem;
class Database;
extern Database* current_db;
extern fs::path root;
extern std::string db_dir;

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


    // need to catch all exceptions !!!
    if (where_col_num != -1)
    {
        switch (sign)
        {
            case '=':
                std::erase_if(out, [&where_col_num, &cond] (std::vector<std::string>& a) {
                            return !(a[where_col_num] == cond);
                            });
                break;

            case '>':
                if (data_types[where_col_num] != "int")
                    throw;

                std::erase_if(out, [&where_col_num, &cond] (std::vector<std::string>& a) {
                        return (std::stoi(a[where_col_num]) <= std::stoi(cond));});
                break;

            case '<':
                if (data_types[where_col_num] != "int")
                    throw;

                std::erase_if(out, [&where_col_num, &cond] (std::vector<std::string>& a) {
                        return (std::stoi(a[where_col_num]) >= std::stoi(cond));});
                break;

            default:
                throw std::invalid_argument("invalid type");
        }
    }



    if (is_sort)
    {
        auto d_t = data_types[sort_col_num];
        std::sort(out.begin(), out.end(), [&dir, &d_t, &sort_col_num] (std::vector<std::string> a,
                                              std::vector<std::string> b) {
                if (d_t == "str")
                {
                    if (dir)
                        return (a[sort_col_num] < b[sort_col_num]);
                    else 
                        return (a[sort_col_num] > b[sort_col_num]);
                } else {
                    
                    if (dir)
                        return (std::stoi(a[sort_col_num]) < std::stoi(b[sort_col_num]));
                    else
                        return (std::stoi(a[sort_col_num]) > std::stoi(b[sort_col_num]));
                }
                    
                });
    } else {
        std::sort(out.begin(), out.end(), [] (std::vector<std::string>& a,
                    std::vector<std::string>& b) {
                    return (std::stoi(a[0]) < std::stoi(b[0]));
                });
    }

    std::vector<uint32_t> keys;
    for (auto x : out)
        keys.push_back(std::stoi(x[0]));

    // delete some columns
    auto col_names_copy = col_names;
    ///////////////////////////////////////////////////////
    if (col_out[0] != -1)
    {
        std::vector<uint8_t> col_del;

        // making vector of columns which have to be deleted
        int i = 0;
        for (auto y : col_names)
        {
            col_del.push_back(i);
            i++;
        }

        for (auto y : col_out)
            col_del.erase(std::find(col_del.begin(), col_del.end(), y));

        for (auto& x : out)
        {
            // deleting
            int i = 0;
            for (auto y : col_del)
            {
                x.erase(x.begin() + y - i);
                ++i;
            }
        }

        int p = 0;
        for (auto y : col_del)
        {
            max_w.erase(max_w.begin() + y - p);
            p++;
        }

        for (auto y : col_del)
            col_names_copy.erase(col_names_copy.begin() + y);
    }

    ///////////////////////////////////////////////////////

    if (!out.empty())
    {
        tb.push_header(col_names_copy, max_w);
        for (auto x : out)
            tb.push_tuple(x);
        tb.print();
    }

    // don't know why do we need it
    if (is_write)
    {
        std::string name = "Saved_sample_0";
        int i = 1;
        while (fs::exists(root/db_dir/name))
        {
            name.pop_back();
            name += std::to_string(i);
            i++;
        }

        int tb_num = 0;
        for (auto y : current_db->tb_vec)
        {
            if (y->table_name == table_name)
                break;

            tb_num++;
        }

        Database* write_db = new Database (current_db->db_type, name, 1);
        for (auto x : keys)
        {

            // recursive save 
            if (tb_num == 0)
            {
                // saving of faculty
                auto data = tuple_map.find(x)->second->get_data();
                write_db->insert(data.first, data.second, 0);

                std::for_each(current_db->tb_vec[1]->tuple_map.begin(), 
                        current_db->tb_vec[1]->tuple_map.end(), [x, &write_db] 
                        (std::pair<uint32_t, tuple*> t)->void 
                        {
                        if (t.second->for_key == x)
                        {
                        // saving of department
                        auto data = t.second->get_data();
                        write_db->insert (data.first, data.second, 1);

                        auto dep_id = t.first;
                        std::for_each(current_db->tb_vec[2]->tuple_map.begin(),
                                current_db->tb_vec[2]->tuple_map.end(), [dep_id, &write_db]
                                ( std::pair<uint32_t, tuple*> dis)->void
                                {
                                if (dis.second->for_key == dep_id)
                                    write_db->insert( dis.second->get_data().first, dis.second->get_data().second, 2);
                                });
                        }
                        });

                if (current_db->db_type == 2)
                {
                    std::for_each(current_db->tb_vec[3]->tuple_map.begin(), 
                            current_db->tb_vec[3]->tuple_map.end(), [x, &write_db] 
                            (std::pair<uint32_t, tuple*> t)->void 
                            {
                            if (t.second->for_key == x)
                            {
                            auto data = t.second->get_data();
                            write_db->insert (data.first, data.second, 3);

                            auto dep_id = t.first;
                            std::for_each(current_db->tb_vec[4]->tuple_map.begin(),
                                    current_db->tb_vec[4]->tuple_map.end(), [dep_id, &write_db]
                                    ( std::pair<uint32_t, tuple*> dis)->void
                                    {
                                    if (dis.second->for_key == dep_id)
                                        write_db->insert( dis.second->get_data().first, dis.second->get_data().second, 4);
                                    });
                            } 
                            });
                }

            } else if (tb_num == 1)
            {
                auto data = tuple_map.find(x)->second->get_data();
                write_db->insert(data.first, data.second, 1);

                auto dep_id = x;
                std::for_each(current_db->tb_vec[2]->tuple_map.begin(),
                        current_db->tb_vec[2]->tuple_map.end(), [dep_id, &write_db]
                        ( std::pair<uint32_t, tuple*> dis)->void
                        {
                        if (dis.second->for_key == dep_id)
                        write_db->insert( dis.second->get_data().first, dis.second->get_data().second, 2);
                        });

            } else if (tb_num == 2)
            {
                write_db->insert( tuple_map.find(x)->second->get_data().first, 
                        tuple_map.find(x)->second->get_data().second, 2);

            } else if (tb_num == 3)
            {
                auto data = tuple_map.find(x)->second->get_data();
                write_db->insert(data.first, data.second, 3);

                auto dep_id = x;
                std::for_each(current_db->tb_vec[4]->tuple_map.begin(),
                        current_db->tb_vec[4]->tuple_map.end(), [dep_id, &write_db]
                        ( std::pair<uint32_t, tuple*> dis)->void
                        {
                        if (dis.second->for_key == dep_id)
                        write_db->insert( dis.second->get_data().first, dis.second->get_data().second, 4);
                        });

            } else if (tb_num == 4)
            {
                write_db->insert( tuple_map.find(x)->second->get_data().first, 
                        tuple_map.find(x)->second->get_data().second, 4);
            }
        }

        write_db->write();
        delete write_db;
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
        std::vector<uint32_t> l2, int32_t force_id)
{
    if (force_id == -1)
        force_id = tuple_map.size();

    // schema_name, schema_type
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument ("DB_table");

    // CLEAN !!!
    dbases* ins = new dbases ( l1[0], l2[0]);
    tuple_map.insert({force_id, ins });
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
        std::vector<uint32_t> l2, int32_t force_id)
{
    if (l1.size() != 2 || l2.size() != 2)
        throw std::invalid_argument("Fac_table");

    if (force_id == -1)
        force_id = tuple_map.size();

    fac* faculty = new fac(l1[0], l1[1], l2[0], l2[1]);
    tuple_map.insert({force_id, faculty});
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
        std::vector<uint32_t> l2, int32_t force_id)
{
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument("Dep_table");

    if (!current_db->tb_vec[0]->tuple_map.contains(l2[0]))
        throw std::invalid_argument("Wrong foreign key");

    if (force_id == -1)
        force_id = tuple_map.size();

    dep* department = new dep (l1[0], l2[0]);
    tuple_map.insert({force_id, department});
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
        std::vector<uint32_t> l2, int32_t force_id)
{
    if (l1.size() != 1 || l2.size() != 1)
        throw std::invalid_argument("Borg_table");

    if (!current_db->tb_vec[0]->tuple_map.contains(l2[0]))
        throw std::invalid_argument("Wrong foreign key");

    if (force_id == -1)
        force_id = tuple_map.size();

    borg* bg = new borg (l1[0], l2[0]);
    tuple_map.insert({force_id, bg});
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
        std::vector<uint32_t> l2, int32_t force_id)
{
    if (l1.size() != 1 || l2.size() != 2)
        throw std::invalid_argument("Dis_table");

    switch(table_type)
    {
        case 5:
            if (!current_db->tb_vec[1]->tuple_map.contains(l2[1]))
                throw std::invalid_argument("Wrong foreign key");
            else
                break;

        case 6: 
            if (!current_db->tb_vec[3]->tuple_map.contains(l2[1]))
                throw std::invalid_argument("Wrong foreign key");
            else
                break;
    }

    if (force_id == -1)
        force_id = tuple_map.size();

    dis* discipline = new dis (l1[0], l2[0], l2[1]);
    tuple_map.insert({force_id, discipline});
    row_num++;
}
