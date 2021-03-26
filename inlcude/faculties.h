#ifndef FACULTIES_H
#define FACULTIES_H
#include <vector>
#include "table.h"
#include "tuple.h"

class Virtual_faculty
{
protected:
    Fac_table fac_t;
    Dep_table dep_t;
    Dis_table dis_dep_t;
};

class Common_faculty : public Virtual_faculty
{

};

class Special_faculty : public Virtual_faculty
{
    Borg_table borg_t;
    Dis_table dis_borg_t;
};

#endif
