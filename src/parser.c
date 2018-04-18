//
// Created by ivan on 18.04.18.
//


#include <assert.h>
#include <string.h>

#include <mCc/parser.h>

struct mCc_parser_error_array* new_parse_error_array() {
    struct mCc_parser_error_array *parser_error_array =
            malloc(sizeof(*parser_error_array));

    parser_error_array->errors = malloc(sizeof( struct mCc_parser_error *));
    parser_error_array->counter = 0;

    return parser_error_array;
}

struct mCc_parser_error_array* add_parse_error(struct mCc_parser_error_array* array, struct mCc_parser_error *error) {
    assert(array);
    assert(error);
    if(array->counter == 0)
    {
        memcpy(array->errors,error, sizeof(*error));

    } else
    {
        struct mCc_parser_error * temp = realloc(array->errors, sizeof(*error) * (array->counter + 1));
        if(temp == NULL)
        {
            //TODO throw error
            return NULL;
        }
        array->errors = temp;
        memcpy(&(array->errors[array->counter]),error, sizeof(*error));
    }
    free(error);
    array->counter++;
    return array;
}

void mCc_delete_result(struct mCc_parser_result * result)
{
    free(result->errors->errors);
    free(result->errors);
}