#ifndef FS_WORK_H
#define FS_WORK_H
#include "databases.h"

void db_first_write (Database& db);
void db_read (Database& db, std::string name);

#endif
