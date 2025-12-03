#ifndef READ_FILE_H_
#define READ_FILE_H_

#include <stdio.h>

#include "string_view.h"

StringView read_file(FILE *fp);

#endif
