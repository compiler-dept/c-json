#ifndef JSON_H

#define JSON_H

#include "hashmap.h"

enum json_type {JSON_NULL, JSON_BOOL, JSON_NUM, JSON_STRING, JSON_ARRAY, JSON_OBJECT};

struct {
    int size;
    struct json_data *values;
} json_array;

struct json_data {
    enum json_type type;
    union {
        unsigned char bool_value;
        double num_value;
        const char * string_value;
        struct json_array *array_ref;
        struct hashmap *object_ref;
    };
};

#endif /* end of include guard: JSON_H */
