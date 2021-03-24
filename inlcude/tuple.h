#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>

class Tuple  // Кортеж
{
protected:
    uint8_t attr_num = 0;
    uint8_t id = 1; // autoincrement primary key

};

// faculty
class Fac : public Tuple
{
protected:

    std::string name_fac;
    std::string name_nuc;
    uint8_t num_dep;
    bool is_spec;

    template <typename T> friend class Table;
};

// department
class Dep : public Tuple
{
protected:
    std::string dep_name;
    uint8_t fac_id;

    template <typename T> friend class Table;
};

// base organization
class Borg : public Tuple
{
protected:
    std::string borg_name;
    // (optional foreign key for preformance increase)
    uint8_t fac_id;

    template <typename T> friend class Table;
};

// discipline
class Dis : public Tuple
{
protected:
    std::string dis_name;
    uint8_t dis_teach_num;
    uint8_t dep_id;
    uint8_t borg_id;

    template <typename T> friend class Table;
};
#endif

