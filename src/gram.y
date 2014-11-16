%include
{
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <stdlib.h>
    #include "json.h"
}

%token_type { const char * }
%token_destructor { free((char *)$$); }

%type object { struct hashmap * }
%type entry_list { struct hashmap * }
%type entry { struct hashmap_entry * }
%type array { struct json_array * }
%type components { struct json_array * }
%type atomic { struct json_data * }

%extra_argument { int *success }

%syntax_error
{
    puts("Error parsing input.");
}

start ::= object.
{
    *success = 0;
}
start ::= error.
{
    *success = 1;
}

object ::= LBRACE entry_list RBRACE.
object ::= LBRACE RBRACE.

entry_list ::= entry COMMA entry_list.
entry_list ::= entry.

entry ::= IDENTIFIER COLON atomic.

atomic ::= JNULL.
atomic ::= BOOLEAN.
atomic ::= NUMBER.
atomic ::= STRING.
atomic ::= array.
atomic ::= object.

array ::= LBRACKET components RBRACKET.

components ::= atomic COMMA components.
components ::= atomic.
