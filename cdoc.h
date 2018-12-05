
#if !defined(CDOC_H)
#define CDOC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Basic Type Definitions
typedef enum
{
    FALSE, TRUE
} Boolean;
typedef struct DocumentationUnit DocumentationUnit;
typedef DocumentationUnit * DocumentationUnitPtr;

// Struct Modeling
/**
 * DocumetantionUnit holds type of documentation, its proper value and a pointer to another DocumentationUnitPtr
 * Current types: @Name @Macro @Function @Definition @Params @Return
 */ 
struct DocumentationUnit
{
    char type[20];
    char value[1000];
    DocumentationUnitPtr next_document_unit_ptr;
};

//Global Variables
Boolean IS_SINGLELINE_DOCUMENT_PROTOCOL_ENABLED;
Boolean IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED;

// Function Prototypes
DocumentationUnitPtr init_cdoc(char program_name[]);
void analyse_program_for_documentation(char program_name[], DocumentationUnitPtr *documentation_unit_header_ptr);
void scan_documentation_status(char data[]);
DocumentationUnitPtr create_new_documentation_unit();
void update_documentation_unit_list(DocumentationUnitPtr *documentation_unit_header_ptr, DocumentationUnitPtr new_documentation_unit_ptr);
void create_HTML(DocumentationUnitPtr documentation_unit_header);

// Initialises the documentation
DocumentationUnitPtr init_cdoc(char program_name[])
{
    DocumentationUnitPtr documentation_unit_header = NULL;
    analyse_program_for_documentation(program_name, &documentation_unit_header);
    create_HTML(documentation_unit_header);
    return documentation_unit_header;
}

// Reads c program to analyse documentation structure
void analyse_program_for_documentation(char program_name[], DocumentationUnitPtr *documentation_unit_header_ptr)
{
    DocumentationUnitPtr new_documentation_unit_ptr;
    int data_type_counter = 0;
    char value[1000], data[1000], type[20];
    FILE *program_fptr = fopen(program_name, "r");
    while (fscanf(program_fptr, "%s", data) != EOF)
    {
        if (data_type_counter == 0)
        {
            new_documentation_unit_ptr = create_new_documentation_unit();
            new_documentation_unit_ptr->next_document_unit_ptr = NULL;
        }
        if (IS_SINGLELINE_DOCUMENT_PROTOCOL_ENABLED)
        {
            switch (data_type_counter)
            {
                case 0: 
                {
                    sscanf(data, "@%s", type);
                    strcpy(new_documentation_unit_ptr->type, type);
                    break;
                }
                case 1:
                {
                    strcpy(value, data);
                    fscanf(program_fptr, "%[^$]", data);
                    strcat(value, data);
                    strcpy(new_documentation_unit_ptr->value, value);
                }
            }
            ++data_type_counter;
        }
        else if (IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED && strcmp(data, "*"))
        {
            switch (data_type_counter)
            {
                case 0: 
                {
                    sscanf(data, "@%s", type);
                    strcpy(new_documentation_unit_ptr->type, type);
                    break;
                }
                case 1:
                {
                    strcpy(value, data);
                    fscanf(program_fptr, "%[^*]", data);
                    strcat(value, data);
                    value[strlen(value) - 2] = '\0'; 
                    strcpy(new_documentation_unit_ptr->value, value);
                }
            }
            ++data_type_counter;
        }
        if (data_type_counter == 2)
        {
            IS_SINGLELINE_DOCUMENT_PROTOCOL_ENABLED = FALSE;
            IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED = FALSE;
            data_type_counter = 0;
            update_documentation_unit_list(documentation_unit_header_ptr, new_documentation_unit_ptr);
        }
        if (!strcmp(data, "*"))
        {
            IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED = TRUE;
        }
        scan_documentation_status(data);
    }
}

// Scans the data and decides the documentation status
void scan_documentation_status(char data[])
{
    if (!strcmp(data, "/*$")) 
    {
        IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED = TRUE;
    } 
    else if (!strcmp(data, "//$"))
    {
        IS_SINGLELINE_DOCUMENT_PROTOCOL_ENABLED = TRUE;
    }
    else if(!strcmp(data, "*/"))
    {
        IS_MULTILINE_DOCUMENT_PROTOCOL_ENABLED = FALSE;
    }
}

DocumentationUnitPtr create_new_documentation_unit()
{
    DocumentationUnitPtr new_documentation_unit = (DocumentationUnitPtr)malloc(sizeof(DocumentationUnit));
    if (new_documentation_unit != NULL)
    {
        return new_documentation_unit;
    }
    create_new_documentation_unit();
}

// Updates documentation unit list adding the new node
void update_documentation_unit_list(DocumentationUnitPtr *documentation_unit_header_ptr, DocumentationUnitPtr new_documentation_unit_ptr)
{
    if (*documentation_unit_header_ptr == NULL)
    {
        *documentation_unit_header_ptr = new_documentation_unit_ptr;
    }
    else 
    {
        DocumentationUnitPtr current_documentation_unit_ptr = *documentation_unit_header_ptr;
        while (current_documentation_unit_ptr->next_document_unit_ptr != NULL)
        {
            current_documentation_unit_ptr = current_documentation_unit_ptr->next_document_unit_ptr;
        }
        current_documentation_unit_ptr->next_document_unit_ptr = new_documentation_unit_ptr;
    }
}

// Creates an HTML file styled with CSS for documentation on localhost
// A webserver for documentation is planned
void create_HTML(DocumentationUnitPtr documentation_unit_header)
{
    char filename[50];
    strcpy(filename, documentation_unit_header->value);
    strcat(filename, ".html");
    FILE *html_fptr = fopen(filename, "w");
    DocumentationUnitPtr current_documentation_unit_ptr = documentation_unit_header;
    while (current_documentation_unit_ptr != NULL)
    {
        fprintf(html_fptr, "<code><h2 style=\"display:inline\">%s:</h2></code>\n", current_documentation_unit_ptr->type);
        fprintf(html_fptr, "<code>%s</code><br>\n", current_documentation_unit_ptr->value);
        current_documentation_unit_ptr = current_documentation_unit_ptr->next_document_unit_ptr;
    }
    fclose(html_fptr);
}

#endif // CDOC_H
