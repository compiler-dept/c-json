%include
{
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <stdlib.h>
    #include "json.h"
    #include "json_private.h"
    #include "hashmap.h"
    #include "stack.h"

    struct stack *allocated_json_data = NULL;
    struct stack *allocated_entry_lists = NULL;
    struct stack *allocated_entries = NULL;
    struct stack *allocated_arrays = NULL;
    struct stack *allocated_hashmaps = NULL;
}

%token_type { char * }
%token_destructor { free($$); }

%type object { struct hashmap * }
%type entry_list { struct json_entry_list * }
%type entry { struct json_entry * }
%type array { struct json_array * }
%type components { struct json_array * }
%type atomic { struct json_data * }

%extra_argument { struct json_data **object }

start ::= object(O).
{
    *object = malloc(sizeof(struct json_data));
    (*object)->type = JSON_OBJECT;
    (*object)->object_ref = O;

    while (stack_pop(&allocated_json_data));
    while (stack_pop(&allocated_arrays));
    while (stack_pop(&allocated_entries));
    while (stack_pop(&allocated_entry_lists));
    while (stack_pop(&allocated_hashmaps));
}
start ::= error.
{
    (*object) = NULL;

    struct json_data *json_data = NULL;
    while ((json_data = stack_pop(&allocated_json_data)) != NULL){
        if (json_data->type == JSON_STRING){
            free(json_data->string_value);
        }
        free(json_data);
    }

    struct json_array *json_array = NULL;
    while ((json_array = stack_pop(&allocated_arrays)) != NULL){
        free(json_array);
    }

    struct json_entry *json_entry = NULL;
    while ((json_entry = stack_pop(&allocated_entries)) != NULL){
        free(json_entry);
    }

    struct json_entry_list *json_entry_list = NULL;
    while ((json_entry_list = stack_pop(&allocated_entry_lists)) != NULL){
        free(json_entry_list);
    }

    struct hashmap *hashmap = NULL;
    while ((hashmap = stack_pop(&allocated_hashmaps)) != NULL){
        hashmap_free(hashmap);
    }
}

object(O) ::= LBRACE entry_list(EL) RBRACE.
{
    O = NULL;
    for (int i = 0; i < EL->size; i++){
        hashmap_put(&O, EL->values[i]->key,
            EL->values[i]->value);
        free(EL->values[i]->key);
        free(EL->values[i]);
        stack_pop(&allocated_entries);
    }
    free(EL);
    stack_pop(&allocated_entry_lists);
    stack_push(&allocated_hashmaps, O);
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
        EL2->size * sizeof(struct json_entry *));
    free(EL2);
    stack_push(&allocated_entry_lists, EL1);
}
entry_list(EL) ::= entry(E).
{
    EL = malloc(sizeof(struct json_entry_list) 
        + sizeof(struct json_entry));
    EL->size = 1;
    EL->values[0] = E;
    stack_push(&allocated_entry_lists, EL);
}

entry(E) ::= IDENTIFIER(I) COLON atomic(A).
{
    E = malloc(sizeof(struct json_entry));
    E->key = strdup(I);
    E->value = A;
    free(I);
    stack_push(&allocated_entries, E);
}

atomic(A) ::= JNULL.
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_NULL;
    stack_push(&allocated_json_data, A);
}
atomic(A) ::= BOOLEAN(B).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_BOOL;
    A->bool_value = strncmp(B, "true", 4) == 0;
    free(B);
    stack_push(&allocated_json_data, A);
}
atomic(A) ::= NUMBER(N).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_NUM;
    A->num_value = atof(N);
    free(N);
    stack_push(&allocated_json_data, A);
}
atomic(A) ::= STRING(S).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_STRING;
    int len = strlen(S) - 1;
    A->string_value = malloc(len);
    A->string_value[len - 1] = '\0';
    for (int i = 0; i < len - 1; i++){
        A->string_value[i] = S[i + 1];
    }
    free(S);
    stack_push(&allocated_json_data, A);
}
atomic(A) ::= array(AR).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_ARRAY;
    A->array_ref = AR;
    stack_push(&allocated_json_data, A);
}
atomic(A) ::= object(O).
{
    A = malloc(sizeof(struct json_data));
    A->type = JSON_OBJECT;
    A->object_ref = O;
    stack_push(&allocated_json_data, A);
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
    stack_push(&allocated_arrays, C1);
}
components(C) ::= atomic(A).
{
    C = malloc(sizeof(struct json_array) + sizeof(struct json_data));
    C->size = 1;
    C->values[0] = A;
    stack_push(&allocated_arrays, C);
}
