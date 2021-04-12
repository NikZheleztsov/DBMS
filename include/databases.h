#ifndef DATABASE_H
#define DATABASE_H
#include "tuple.h"
#include "table.h"
#include <cstdint>
#include <string>
#include <vector>

extern uint32_t block_size;

class Database 
{
    int8_t change_tuple (uint8_t tb_num, uint32_t key, tuple* new_tup);

protected:

    // -1 = custom database
    int8_t db_type = -1;
    uint32_t bl_size = block_size;

public:

    std::string db_name = "";
    std::vector<Table*> tb_vec;

    Database () {};

    Database (uint8_t type, std::string name = "", 
            bool is_first_create = false);

    void insert (std::vector<std::string> l1, 
                 std::vector<uint32_t> l2, uint8_t tb_num, int32_t force_id = -1);

    int32_t get_id (uint8_t tb_num, std::string name);

    tuple* get_tuple (uint8_t tb_num, uint32_t key);

    void select (uint32_t tb_num, std::vector<int8_t> col_out = {-1},
            int8_t where_str = -1 , 
            char sign = ' ', std::string cond = "", 
            bool is_sort = false, int8_t col = -1, bool dir = false,
            bool is_write = false);


    /*
    void update_tup (uint8_t tb_num, uint32_t id,
            std::vector<std::string> v1, std::vector<uint32_t> v2);
            */

    void rename (std::string new_name);

    void delete_id (uint8_t tb_num, uint32_t id);
    
    void write ();

    friend void db_full_write (Database&);
    friend Database* db_meta_read (std::string name);
    friend Database& init_tables(Database& db);

    ~Database ();
};

#endif
