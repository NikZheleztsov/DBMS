#include "databases.h"
#include "misc.h"

/* Db types:
   0 - meta db
   1 - common fac
   2 - hybrid fac
*/

Database* db = nullptr;
Database* current_db = nullptr;
std::string curr_db = "none";


int main ()
{
    check_for_init();
    std::cout << "Welcome to DBMS (pre-alpha). \n\
It`s not ready yet. However, all tasks were done.\n\
Print 'help' in order to get commands, corresponding\n\
to number of exercise\n\n";
    std::string answ = "";
    while (answ != "quit" && answ != "q")
    { 
        std::cout << "DBMS [(" << curr_db << ")]> ";
        answ.clear();

        do {
            std::string temp;
            std::getline(std::cin, temp);
            answ += temp;
        } while (answ[answ.size() - 1] != ';' && !(answ == "q" 
                || answ == "quit" || answ == "help"));

        if (answ == "help")
        {
            help();
            continue;
        }

        if (answ != "quit" && answ != "q")
            parsing(answ);
    }

    return 0;
}

