#include "clar.h"

#include <cjson.h>
#include <gram.h>
#include <parser_signatures.h>

void test_parser__empty_object(void)
{
    struct json_data *json;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);
    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

    cl_assert(json->type == JSON_OBJECT);
    cl_assert(json->object_ref == NULL);
    cl_assert(json_get(json, 1, "needle") == NULL);

    json_free(json);
}

void test_parser__get_nonexisting(void)
{
    struct json_data *json = NULL;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);
    Parse(parser, STRING, strdup("\"string\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);
    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

    cl_assert(json->type == JSON_OBJECT);
    cl_assert(json_get(json, 1, "needle") == NULL);

    json_free(json);
}

void test_parser__get_object_atomic(void)
{
    struct json_data *json = NULL;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);

    Parse(parser, STRING, strdup("\"null\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, JNULL, strdup("null"), &json);

    Parse(parser, COMMA, strdup(","), &json);

    Parse(parser, STRING, strdup("\"bool1\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, BOOLEAN, strdup("true"), &json);

    Parse(parser, COMMA, strdup(","), &json);

    Parse(parser, STRING, strdup("\"bool2\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, BOOLEAN, strdup("false"), &json);

    Parse(parser, COMMA, strdup(","), &json);

    Parse(parser, STRING, strdup("\"number\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, NUMBER, strdup("-23.4"), &json);

    Parse(parser, COMMA, strdup(","), &json);

    Parse(parser, STRING, strdup("\"string\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);

    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

    cl_assert(json->type == JSON_OBJECT);

    struct json_data *needle;

    needle = json_get(json, 1, "null");
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

void test_parser__get_object_object(void)
{
    struct json_data *json = NULL;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);

    Parse(parser, STRING, strdup("\"object\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
        Parse(parser, LBRACE, strdup("{"), &json);
        Parse(parser, STRING, strdup("\"object\""), &json);
        Parse(parser, COLON, strdup(":"), &json);
            Parse(parser, LBRACE, strdup("{"), &json);
            Parse(parser, STRING, strdup("\"string\""), &json);
            Parse(parser, COLON, strdup(":"), &json);
            Parse(parser, STRING, strdup("\"foo\""), &json);
            Parse(parser, RBRACE, strdup("}"), &json);
        Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

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

void test_parser__get_array(void)
{
    struct json_data *json = NULL;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);
    Parse(parser, STRING, strdup("\"array\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, LBRACKET, strdup("["), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);
    Parse(parser, COMMA, strdup(","), &json);
    Parse(parser, STRING, strdup("\"bar\""), &json);
    Parse(parser, RBRACKET, strdup("]"), &json);
    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

    cl_assert(json->type == JSON_OBJECT);

    struct json_data *array = json_get(json, 1, "array");
    cl_assert(array->type == JSON_ARRAY);
    cl_assert(array->array_ref->size == 2);
    cl_assert(array->array_ref->values[0]->type == JSON_STRING);
    cl_assert(strcmp(array->array_ref->values[0]->string_value, "foo") == 0);
    cl_assert(strcmp(array->array_ref->values[1]->string_value, "bar") == 0);

    json_free(json);
}

void test_parser__syntax_error(void)
{
    struct json_data *json = NULL;
    void *parser = ParseAlloc(malloc);

    Parse(parser, LBRACE, strdup("{"), &json);

    Parse(parser, STRING, strdup("\"string\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);

    Parse(parser, COMMA, strdup(","), &json);

    Parse(parser, STRING, strdup("\"array\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, LBRACKET, strdup("["), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);
    Parse(parser, COMMA, strdup(","), &json);
    Parse(parser, STRING, strdup("\"bar\""), &json);
    Parse(parser, RBRACKET, strdup("]"), &json);

    Parse(parser, STRING, strdup("\"object\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, LBRACE, strdup("{"), &json);
    Parse(parser, STRING, strdup("\"string\""), &json);
    Parse(parser, COLON, strdup(":"), &json);
    Parse(parser, STRING, strdup("\"foo\""), &json);
    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, RBRACE, strdup("}"), &json);

    Parse(parser, RBRACE, strdup("}"), &json); // syntax error

    Parse(parser, 0, 0, &json);
    ParseFree(parser, free);

    cl_assert(json == NULL);
}
