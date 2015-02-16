#include "clar.h"

#include <cjson.h>

void test_cjson__get_object_atomic(void)
{
    struct json_data *json =
        json_parse("{ \"jnull\" : null, \"bool1\" : true, \"bool2\" : false, \"number\" : -23.4, \"string\" : \"foo\"}");

    cl_assert(json->type == JSON_OBJECT);

    struct json_data *needle;

    needle = json_get(json, 1, "jnull");
    cl_assert(needle != NULL);
    cl_assert(needle->type == JSON_NULL);

    needle = json_get(json, 1, "bool1");
    cl_assert(needle != NULL);
    cl_assert(needle->type == JSON_BOOL);
    cl_assert(needle->bool_value > 0);

    needle = json_get(json, 1, "bool2");
    cl_assert(needle != NULL);
    cl_assert(needle->type == JSON_BOOL);
    cl_assert(needle->bool_value == 0);

    needle = json_get(json, 1, "number");
    cl_assert(needle != NULL);
    cl_assert(needle->type == JSON_NUM);
    cl_assert(needle->num_value == -23.4);

    needle = json_get(json, 1, "string");
    cl_assert(needle != NULL);
    cl_assert(needle->type == JSON_STRING);
    cl_assert(strcmp(needle->string_value, "foo") == 0);

    json_free(json);
}

void test_cjson__get_object_object(void)
{
    struct json_data *json =
        json_parse("{ \"object\" : { \"object\" : { \"string\" : \"foo\" } } }");

    cl_assert(json->type == JSON_OBJECT);

    struct json_data *needle;

    // test step by step
    needle = json_get(json, 1, "object");
    cl_assert(needle != NULL);
    cl_assert(needle->type = JSON_OBJECT);

    needle = json_get(needle, 1, "object");
    cl_assert(needle != NULL);
    cl_assert(needle->type = JSON_OBJECT);

    needle = json_get(needle, 1, "string");
    cl_assert(needle != NULL);
    cl_assert(needle->type = JSON_STRING);
    cl_assert(strcmp(needle->string_value, "foo") == 0);

    // test automatic search
    needle = json_get(json, 3, "object", "object", "string");
    cl_assert(needle != NULL);
    cl_assert(needle->type = JSON_STRING);
    cl_assert(strcmp(needle->string_value, "foo") == 0);

    json_free(json);
}

void test_cjson__get_array(void)
{
    struct json_data *json = json_parse("{ \"array\" : [ \"foo\", \"bar\", \"spam\" ] }");

    cl_assert(json->type == JSON_OBJECT);

    struct json_data *array = json_get(json, 1, "array");
    cl_assert(array->type == JSON_ARRAY);
    cl_assert(array->array_ref->size == 3);
    cl_assert(array->array_ref->values[0]->type == JSON_STRING);
    cl_assert(strcmp(array->array_ref->values[0]->string_value, "foo") == 0);
    cl_assert(strcmp(array->array_ref->values[1]->string_value, "bar") == 0);
    cl_assert(strcmp(array->array_ref->values[2]->string_value, "spam") == 0);

    json_free(json);
}

void test_cjson__syntax_error(void)
{
    struct json_data *json =
        json_parse("{ \"string\" : \"foos\", s\" : null }");

    if (json){
        json_free(json);
    }
}
