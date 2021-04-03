#include "databases.h"
#include "misc.h"

/* Db types:
   0 - meta db
   1 - common fac
   2 - hybrid fac
*/

Database* db = nullptr;

int main ()
{
    check_for_init();
    Database fac (2, "MGTU Baumana");
    if (db != nullptr)
        db->select({0,1}, {0, 1, 2});
    return 0;
}

