%include
{
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <stdlib.h>
    #include "json.h"
}

%token_type { char * }
%token_destructor { free($$); }

%type object { struct hashmap * }
%type entry_list { struct json_entry_list * }
%type entry { struct json_entry * }
%type array { struct json_array * }
%type components { struct json_array * }
%type atomic { struct json_data * }

%extra_argument { struct json_data *object }

%syntax_error
{
    puts("Error parsing input.");
}

start ::= object(O).
{
    object->type = JSON_OBJECT;
    object->object_ref = O;
}
start ::= error.
{
    object->type = JSON_NULL;
}

object(O) ::= LBRACE entry_list(EL) RBRACE.
{
    O = NULL;
    for (int i = 0; i < EL->size; i++){
        hashmap_put(&O, EL->values[i]->key,
            EL->values[i]->value);
    }
    free(EL);
}
object(O) ::= LBRACE RBRACE.
{
    O = NULL;
}

entry_list(EL1) ::= entry(E) COMMA entry_list(EL2).
{
    EL1 = malloc(sizeof(struct json_entry_list) + 
        EL2->size * sizeof(struct json_entry));
    EL1->size = EL2->size + 1;
    EL1->values[0] = E;
    memcpy(EL1->values + 1, EL2->values,
        EL2->size * sizeof(struct json_entry));
    free(EL2);
}
entry_list(EL) ::= entry(E).
{
    EL = malloc(sizeof(struct json_entry_list) 
        + sizeof(struct json_entry));
    EL->size = 1;
    EL->values[0] = E;
}

entry(E) ::= IDENTIFIER(I) COLON atomic(A).
{
    E = malloc(sizeof(struct json_entry));
    E->key = strdup(I);
    E->value = A;
    free(I);
}

atomic(A) ::= JNULL.
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_NULL;
}
atomic(A) ::= BOOLEAN(B).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_BOOL;
    A->bool_value = strncmp(B, "true", 4) == 0;
    free(B);
}
atomic(A) ::= NUMBER(N).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_NUM;
    A->num_value = atof(N);
    free(N);
}
atomic(A) ::= STRING(S).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_STRING;
    A->string_value = malloc(strlen(S) + 1);
    strcpy(A->string_value, S);
    free(S);
}
atomic(A) ::= array(AR).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_ARRAY;
    A->array_ref = AR;
}
atomic(A) ::= object(O).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_OBJECT;
    A->object_ref = O;
}

array(A) ::= LBRACKET components(C) RBRACKET.
{
    A = C;
}

components(C1) ::= atomic(A) COMMA components(C2).
{
    C1 = malloc(sizeof(struct json_array) 
        + C2->size * sizeof(struct json_data));
    C1->size = C2->size + 1;
    C1->values[0] = A;
    memcpy(C1->values + 1, C2->values, 
        C2->size * sizeof(struct json_data *));
    free(C2);
}
components(C) ::= atomic(A).
{
    C = malloc(sizeof(struct json_array) + sizeof(struct json_data));
    C->size = 1;
    C->values[0] = A;
}
