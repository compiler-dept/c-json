#ifndef JSON_H

#define JSON_H

#include "hashmap.h"

enum json_type {JSON_NULL, JSON_BOOL, JSON_NUM, JSON_STRING, JSON_ARRAY, JSON_OBJECT};


struct json_array {
    int size;
    struct json_data *values[];
};

struct json_data {
    enum json_type type;
    union {
        unsigned char bool_value;
        double num_value;
        char *string_value;
        struct json_array *array_ref;
        struct hashmap *object_ref;
    };
};

struct json_data *json_parse(const char *str);
struct json_data *json_get(struct json_data *json_object, int keyc, ...);
void json_free(struct json_data *json_object);

#endif /* end of include guard: JSON_H */
