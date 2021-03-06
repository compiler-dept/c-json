#include <stdarg.h>
#include "hashmap.h"
#include "lexer.h"
#include "cjson.h"
#include "cjson_private.h"
#include "parser_signatures.h"

struct json_data *json_parse(const char *str)
{
    struct json_data *json;
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(str, scanner);

    int token = 0;

    void *parser = ParseAlloc(malloc);
    while ((token = yylex(scanner))) {
        if (token == -1) {
            break;
        }
        char *text = strdup(yyget_text(scanner));
        Parse(parser, token, text, &json);
    }
    Parse(parser, 0, NULL, &json);
    ParseFree(parser, free);
    yy_delete_buffer(bufferState, scanner);
    yylex_destroy(scanner);
    return json;
}

struct json_data *json_get(struct json_data *json_object, int keyc, ...)
{
    if (json_object != NULL && json_object->type == JSON_OBJECT) {
        va_list ap;
        va_start(ap, keyc);

        const char *key;
        for (int i = 0; i < keyc; i++) {
            key = va_arg(ap, const char *);
            json_object = hashmap_get(json_object->object_ref, key);
            if (i < keyc - 1 && json_object != NULL
                && json_object->type != JSON_OBJECT) {
                return NULL;
            }
        }

        va_end(ap);

    } else {
        return NULL;
    }
    return json_object;
}

void json_free_array(struct json_array *array)
{
    if (array) {
        for (int i = 0; i < array->size; i++) {
            json_free(array->values[i]);
        }
        free(array);
    }
}

void json_free_object(struct hashmap *object)
{
    if (object) {
        for (int i = 0; i < object->capacity; i++) {
            if (object->values[i].key) {
                json_free(object->values[i].value);
            }
        }
        hashmap_free(object);
    }
}

void json_free(struct json_data *json_object)
{
    if (json_object) {
        switch (json_object->type) {
        case JSON_OBJECT:
            json_free_object(json_object->object_ref);
            break;
        case JSON_ARRAY:
            json_free_array(json_object->array_ref);
            break;
        case JSON_STRING:
            free(json_object->string_value);
            break;
        }
        free(json_object);
    }
}
