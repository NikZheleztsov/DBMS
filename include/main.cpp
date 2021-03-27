#include "databases.h"
#include "misc.h"

/* Db types:
   0 - meta db
   1 - common fac
   2 - hybrid fac
*/

int main ()
{
    read_config();
    All_db db;
    // inserting to database_t
    db.insert("schemata 0", 0);
    db.write();
    return 0;
}

