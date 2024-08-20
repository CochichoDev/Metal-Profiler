#ifndef TUI_H
#define TUI_H

#include <stdint.h>

#define TRUE                1
#define FALSE               0

#define TERMESC             "\x1B"
#define TERMHIDECURSOR      TERMESC"[?25l"
#define TERMSHOWCURSOR      TERMESC"[?25h"
#define TERMCLEAR           TERMESC"[2J"
#define TERMCURSORHOME      TERMESC"[H"
#define TERMMODE            TERMESC"[=19h"
#define TERMCURSORMOVE      TERMESC"[r;cH"
#define TERMSETBACKCOLOR(ID)    \
                            TERMESC"[48;5;"#ID"m"
#define TERMSETFORECOLOR(ID)    \
                            TERMESC"[38;5;"#ID"m"

#define LEFTARROW           TERMESC"[D"
#define RIGHTARROW          TERMESC"[C"
#define DOWNARROW           TERMESC"[B"

#define TAB_KEYCODE         9

#define POS_TO_GRID(P)      ( (T_POSGRID){(T_USHORT) ( ( (P).X / 2 + .5f ) * (window.WIDTH-1) ), \
                                            (T_USHORT) ( ( (P).Y / 2 + .5f ) * (window.HEIGHT-1) )} )

#define RETURN_CODE         0
#define RIGHT_CODE          1
#define LEFT_CODE           2
#define DOWN_CODE           3

#define BACKGROUNDCOLOR     TERMSETBACKCOLOR(0)


#define GET_CHILD_ACTION(ACTION, SELECTOR)      \
                            __get_child_action_##ACTION(SELECTOR);

/*      TYPE DEFINITIONS        */
typedef uint16_t        T_USHORT;
typedef void            T_VOID;
typedef int8_t          T_ERROR;


typedef struct s_posgrid {
    T_USHORT        X;
    T_USHORT        Y;
} T_POSGRID;

typedef struct s_node T_NODE;

extern T_NODE schema;

/***************FUNCTION DECLARATION*********************/
// TUI
T_VOID *init_tui();
T_VOID exit_tui(T_VOID *old);
T_VOID event_handler();

// Nodes
T_NODE *get_child(T_NODE *parent, T_USHORT child_idx);
T_NODE *add_node(T_NODE *parent, T_NODE *new);
T_ERROR hook_return(T_NODE *node, T_VOID (*hook)(T_NODE *, T_VOID **), T_VOID **data);
T_ERROR hook_hover(T_NODE *node, T_VOID (*hook)(T_NODE *, T_VOID **), T_VOID **data);
T_ERROR hook_unhover(T_NODE *node, T_VOID (*hook)(T_NODE *, T_VOID **), T_VOID **data);
T_ERROR hook_draw(T_NODE *node, T_VOID (*hook)(T_NODE *, T_VOID **), T_VOID **data);
T_ERROR hook_default(T_NODE *node, T_VOID (*hook)(T_NODE *, T_VOID *, T_VOID **), T_VOID **data);
T_VOID print_schema_tree(T_NODE *parent);

// Draw
T_ERROR draw();
T_ERROR draw_node(T_NODE *node);
T_ERROR draw_rectangle(T_POSGRID p1, T_USHORT width, T_USHORT height);
T_ERROR draw_char(T_POSGRID point, const char chr);

// State
T_USHORT WINDOW_WIDTH();
T_USHORT WINDOW_HEIGHT();

// Button
T_NODE *create_node_button(T_POSGRID p1, T_USHORT width, T_USHORT height, const char *text);

// Panel
T_NODE *create_node_panel(T_POSGRID p1, T_USHORT width, T_USHORT height);

// Terminal
T_NODE *create_node_term (T_POSGRID p1, T_USHORT width, T_USHORT height);
int term_get_descriptor(T_NODE *term_node);

// Textbox
T_NODE *create_node_textb(T_POSGRID p1, T_USHORT width);
const char *textb_get_text(T_NODE *textb);

// Multinode
T_NODE *create_node_multi(T_POSGRID p1, T_USHORT width, T_USHORT height);
T_NODE *multi_add_item(T_NODE *multi, T_NODE *new_item);
T_VOID multi_clean_items(T_NODE *multi_node);
T_USHORT multi_get_index(T_NODE *multi);

#endif
