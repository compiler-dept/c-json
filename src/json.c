#include <stdarg.h>
#include "json.h"

struct json_data *json_object_get(struct json_data *json_object, int keyc, ...)
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
