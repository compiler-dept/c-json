#ifndef JSON_PRIVATE_H
#define JSON_PRIVATE_H

struct json_entry_list {
    int size;
    struct json_entry *values[];
};

struct json_entry {
    char *key;
    struct json_data *value;
};

#endif /* end of include guard: JSON_PRIVATE_H */
