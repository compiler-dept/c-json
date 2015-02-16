#include "clar.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cjson.h>

void test_spaceapi__parse_directory(void)
{
    char *str = NULL;
    int len = 0;

    FILE *f = fopen(cl_fixture("directory.json"), "r");
    cl_assert(f != NULL);

    char buf[1024];
    int count = 0;
    while ((count = fread(buf, 1, 1024, f)) > 0){
        str = realloc(str, len + count);
        memcpy(str + len, buf, count);
        len += count;
    }
    fclose(f);

    str = realloc(str, len + 1);
    str[len] = '\0';

    struct json_data *json = json_parse(str);
    free(str);
    cl_assert(json->type == JSON_OBJECT);
    json_free(json);
}
