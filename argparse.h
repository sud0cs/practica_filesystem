#include "utils.h"
#define	MISSING_ARGS_ERROR -1

typedef enum{
  CHAR,
  STRING,
  INT,
  FLOAT,
  BOOL,
  NONE
} arg_type;

typedef struct {
  char *name;
  void *content;
  bool mandatory;
  arg_type type;
  char *description;
} arg;

void init_parser(unsigned int max_args, int _argc, char **_argv);
void add_arg(char *name, bool mandatory, arg_type type, char* description);
void print_help();
void free_args();
int parse_args();
bool arg_exists(char* name);
void* arg_value(char* name);
