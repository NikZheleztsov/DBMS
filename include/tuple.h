#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>

//schemata
struct dbases
{
    std::string schema_name;
    uint8_t schema_type; // 0 or 1
};

struct tb
{
    std::string table_name;
    uint32_t rows_num;
    uint32_t db;
};

// faculty
struct fac // 0 type
{
    std::string name_fac;
    std::string name_nuc;
    uint32_t num_dep;
    bool is_spec;
};

// department
struct dep // 1 type
{
    std::string dep_name;
    uint32_t fac_id;
};

// base organization
struct borg // 2 type
{
    std::string borg_name;
    // (optional foreign key for preformance increase)
    uint32_t fac_id;
};

// discipline
struct dis // 3 type
{
    std::string dis_name;
    uint32_t dis_teach_num;
    uint32_t foreign_id;
};
#endif

