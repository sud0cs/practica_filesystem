#include <stdbool.h>
typedef enum{
    DEFAULT = -1,
    BLACK = 0,
    ERROR_COLOR = 1,
    RED = 1,
    SUCCESS_COLOR = 2,
    GREEN = 2,
    WARNING_COLOR = 3,
    YELLOW = 3,
    HINT_COLOR = 4,
    BLUE = 4,
    WHITE = 7,
    LIGHT_BLUE = 81,
    PURPLE = 165,
    ORANGE = 172,
    LIGHT_GREEN = 192,
    PINK = 205,
    LIGHT_RED = 210,
    SALMON = 216,
    LIGHT_YELLOW = 228,
    GRAY = 245
} Color;

typedef struct{
    Color color;
    Color bgcolor;
    bool bold;
    bool underline;
} style;

#define ERROR_STYLE (style){RED, DEFAULT, true, false}

char *strpl(char *str, char *fnd, char *rpl, int lim);
char *prettify(char *str,style *s);
void pprint(char *str, style *s, ...);
void xpprint(char *str, Color fg, Color bg, bool bold, bool underline, ...);
void pperror(char *str, style *s, ...);
void xpperror(char *str, Color fg, Color bg, bool bold, bool underline, ...);
