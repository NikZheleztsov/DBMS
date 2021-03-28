#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>
#include <fstream>

class tuple 
{
protected:
    uint32_t size;
    std::string name; // 32 byte

public:
    tuple(uint32_t s, std::string n) : size(s), name(n) {}
    virtual uint32_t write(char* out) const = 0;

    friend class data_block;
};


//schemata
class dbases : public tuple
{
    uint8_t schema_type;

public:

    dbases (std::string n, uint8_t type) : tuple(33, n), schema_type(type) {}

    // name <= 32
    uint32_t write(char* out) const override
    {
        int i = 0;
        for (i = 0; i < name.size(); i++)
            out[i] = name[i];

        for (; i < 32; i++)
            out[i] = '\0';

        out[i] = schema_type;

        // 1 - sizeof(schema_type)
        return i + 1;
    }
};

class tb : public tuple
{
    //std::string table_name;
    uint32_t rows_num;
    uint32_t db_id;

public: 
    tb (std::string n, uint32_t rows, uint32_t id) : tuple(40, n), rows_num(rows), db_id(id) {}

    uint32_t write(char* out) const override
    {
        int i = 0;
        for (i = 0; i < name.size(); i++)
            out[i] = name[i];

        for (; i < 32; i++)
            out[i] = '\0';

        out[i] = rows_num;
        out[i+4] = db_id;

        return i + 8;
    }
};

// faculty
class fac : public tuple // 0 type
{
    //std::string name_fac;
    std::string name_nuc;
    uint32_t num_dep;
    bool is_spec;
};

// department
class dep : public tuple// 1 type
{
    //std::string dep_name;
    uint32_t fac_id;
};

// base organization
class borg : public tuple// 2 type
{
    //std::string borg_name;
    // (optional foreign key for preformance increase)
    uint32_t fac_id;
};

// discipline
class dis : public tuple // 3 type
{
    // std::string dis_name;
    uint32_t dis_teach_num;
    uint32_t foreign_id;
};

#endif

