#ifndef MISC_H
#define MISC_H
#include <filesystem>
#include "databases.h"

// working with config
void config (std::filesystem::path current);

// first start
void check_for_init();

#endif
