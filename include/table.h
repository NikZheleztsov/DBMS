#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <string>
#include <unordered_map>
#include "tuple.h"

class Database;
extern Database* current_db;

// interface
class Table
{
protected:
    // Database Meta
    // num of data blocks
    // <= 64 bytes
    // <= 32 bytes
    std::vector <uint8_t> pointers;
    uint32_t row_num = 0;
    std::vector<std::string> data_types;
    std::unordered_map<uint32_t, tuple*> tuple_map;

    Table () : pointers(128) {};

public:

    uint8_t table_type;
    std::string table_name;
    std::vector <std::string> col_names;

    virtual void insert (std::vector<std::string>,
                         std::vector<uint32_t> ) = 0;

    void select(std::vector<int8_t>, int8_t, char, std::string,
            bool is_sort, int8_t col, bool dir, bool) const;

    int32_t get_id (std::string name);
    void delete_tuple (uint32_t id);

    friend void parsing_in (std::vector<std::string>);
    friend void db_full_write (Database&);
    friend Database* db_meta_read (std::string name);
    friend void show_databases ();
    friend void check_for_init();

    friend class Database;
    friend class Dep_table;
    friend class Borg_table;
    friend class Dis_table;

    virtual ~Table() {};
};

class DB_table : public Table
{
    // dbases
public: 

    DB_table ();
    void insert(std::vector<std::string> l1,
                std::vector<uint32_t> l2) override;
};


class Fac_table : public Table
{
    //fac

public: 

    Fac_table ();

    void insert (std::vector<std::string> l1,
            std::vector<uint32_t> l2) override;
};

class Dep_table : public Table
{
    // dep

public:

    Dep_table();

    void insert (std::vector<std::string> l1,
            std::vector<uint32_t> l2) override;
};

class Borg_table : public Table
{
    // borg

public:

    Borg_table();
    
    void insert (std::vector<std::string> l1,
            std::vector<uint32_t> l2) override;
};

// type 5 and 6
class Dis_table : public Table
{
    // dis
public:

    Dis_table(uint8_t type);
    void insert (std::vector<std::string> l1,
            std::vector<uint32_t> l2) override;
};

#endif 
