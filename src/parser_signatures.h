#include "cjson.h"

void *ParseAlloc(void *(*allocProc) (size_t));
void Parse(void *, int, const char *, struct json_data **object);
void ParseFree(void *, void (*freeProc) (void *));
