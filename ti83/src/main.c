#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keypadc.h>
#include <graphx.h>

#include "expression_parser.h"
#include "torseur.h"

#define FONT_HEIGHT 8
#define HEADER_HEIGHT 18
#define ASIDE_WIDTH 100
#define INDIVIDUAL_HEIGHT 11
#define DEBOUNCE_DELAY 200
#define MAX_TORSEUR 16

#define REGION_ASIDE 0
#define REGION_MIDDLE 1
#define REGION_INPUT 2

#define GLOBAL_FG_COLOR  0
#define GLOBAL_BG_COLOR  1
#define HEADER_FG_COLOR  2
#define HEADER_BG_COLOR  3
#define SELECT_BG_COLOR  4
#define HOVER_FG_COLOR   5
#define HOVER_BG_COLOR   6
#define ASIDE_FG_COLOR   7
#define ASIDE_BG_COLOR   8
#define BUTTON_BG_COLOR  9
#define HBUTTON_BG_COLOR 10
#define HOVER_BG_COLOR2  11

typedef struct torseur_input TorseurInput;
struct torseur_input {
    char* name;
    char* point;
    char* px;
    char* py;
    char* pz;
    char* rx;
    char* ry;
    char* rz;
    char* mx;
    char* my;
    char* mz;
};

int i;

uint8_t torseur_input_size;
uint8_t aside_select_id;
uint8_t hover_id;
uint8_t hover_region;
TorseurInput** torseur_input_list = NULL;
uint8_t key;
bool alpha;
bool shift;
char** current_input;

void setup(void);
void draw_header(void);
void draw_left_menu(void);
void draw_center(void);
void draw_input(void);
void loop(void);
void do_calculation(void);
void alloc_torseur_input(TorseurInput**);
char input_character(void);

static double PRECISION = 0.0001;
#define MAX_NUMBER_STRING_SIZE 32;

void ftoa(char* buf, double f) {
    int pos=0,ix,dp,num;

    /* Verification, nombre negatif */
    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    
   /* Centaines, dizaines, unites etc ... 
  Bref combien de chiffre avant la virgule ? */
    while (f>=10.0) 
    {
        f=(double)(f/10.0);
        dp++;
    } 

   /* Precision 7 digits en tout */
    for (ix=1;ix<20;ix++)
    {
            /* garde la partie entiere */
            num = f;

            /* le digit traite est enleve de ce qui reste a convertir */
            f=f-num;

            /* Erreur ! num ne peut pas etre > 9 */
            if (num>21)
                buf[pos++]='#';
            else
                /* Astucieux ... 
                '0' => 0x30 ... 
                par ex, si num = 1 alors 0x30 + 1 = 0x31 => '1'
                le compte est bon ;-) */
                buf[pos++]='0'+num;

           /* Positionne la virgule */
            if (dp==0) 
                buf[pos++]='.';

            /* Next digit */
            f=(double)(f*10.0);
            dp--;
    } 
}

/**
 * Double to ASCII
 */
 /*
char* ftoa(char *s, double n) {
    int digit, m, m1 = 0;
    char* c;
    int neg, useExp;
    int i, j;
    // handle special cases
    if (isnan(n)) {
        strcpy(s, "nan");
    } else if (isinf(n)) {
        strcpy(s, "inf");
    } else if (n == 0.0) {
        strcpy(s, "0");
    } else {
        
        c = s;
        neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}*/

char* strcpy_malloc(char* buffer) {
    char* out = malloc(strlen(buffer) + 1);
    strcpy(out, buffer);
    return out;
}

char out_buffer[32];
int offset;
void do_calculation(void) {
    int i;
    Solutions* s;
    Torseur** tl = malloc(sizeof(Torseur*) * torseur_input_size);
    
    for(i = 0; i < torseur_input_size; i++) {
        tl[i] = malloc(sizeof(Torseur));
        tl[i]->name = strcpy_malloc(torseur_input_list[i]->name);
        tl[i]->point = strcpy_malloc(torseur_input_list[i]->point);
        tl[i]->p.x = EP_parse(torseur_input_list[i]->px);
        tl[i]->p.y = EP_parse(torseur_input_list[i]->py);
        tl[i]->p.z = EP_parse(torseur_input_list[i]->pz);
        tl[i]->r.x = EP_parse(torseur_input_list[i]->rx);
        tl[i]->r.y = EP_parse(torseur_input_list[i]->ry);
        tl[i]->r.z = EP_parse(torseur_input_list[i]->rz);
        tl[i]->m.x = EP_parse(torseur_input_list[i]->mx);
        tl[i]->m.y = EP_parse(torseur_input_list[i]->my);
        tl[i]->m.z = EP_parse(torseur_input_list[i]->mz);
    }
    
    s = T_solve(tl, torseur_input_size);
    
    gfx_FillScreen(GLOBAL_BG_COLOR);
    shift = false;
    alpha = false;
    draw_header();
    
    gfx_SetTextFGColor(GLOBAL_FG_COLOR);
    
    gfx_PrintStringXY("-- Resultats: ", 1, HEADER_HEIGHT + 0 * INDIVIDUAL_HEIGHT + 2);
    
    for(i = 0; i < s->number; i++) {
        offset = 0;
        ftoa(out_buffer, s->values[i]);
        // out_buffer[0] = '\0';
        // sprintf(out_buffer, "%lg", s->values[i]);
        
        gfx_PrintStringXY(s->names[i], 1 + offset, HEADER_HEIGHT + (i+1) * INDIVIDUAL_HEIGHT + 2);
        offset += gfx_GetStringWidth(s->names[i]);
        gfx_PrintStringXY(" = ", 1 + offset, HEADER_HEIGHT + (i+1) * INDIVIDUAL_HEIGHT + 2);
        offset += gfx_GetStringWidth(" = ");
        gfx_PrintStringXY(out_buffer, 1 + offset, HEADER_HEIGHT + (i+1) * INDIVIDUAL_HEIGHT + 2);
    }
    
    for(i = 0; i < torseur_input_size; i++) {
        T_free(tl[i]);
    }
    free(tl);
    ES_free(s);
    
    while(true) {
        key = kb_ScanGroup(kb_group_6);
        if (key & kb_Clear) {
            break;
        }
    }
    
    draw_header();
    draw_left_menu();
    draw_center();
    
    delay(DEBOUNCE_DELAY);
}

char c;
int len;
void loop(void) {
    while(true) {
        
        key = kb_ScanGroup(kb_group_1);
        if (key & kb_2nd && alpha) {
            shift = !shift;
            draw_header();
            delay(DEBOUNCE_DELAY);
        }
        key = kb_ScanGroup(kb_group_2);
        if (key & kb_Alpha) {
            alpha = !alpha;
            
            if (!alpha)
                shift = false;
            draw_header();
            delay(DEBOUNCE_DELAY);
        }
        
        if (hover_region == REGION_ASIDE) {
            key = kb_ScanGroup(kb_group_6);
            if (key & kb_Clear) {
                return;
            } else if (key & kb_Enter) {
                // We selected new...
                if (hover_id == torseur_input_size) {
                    torseur_input_size++;
                    
                    
                    torseur_input_list = realloc(torseur_input_list, sizeof(TorseurInput*) * torseur_input_size);
                    
                    
                    
                    alloc_torseur_input(&(torseur_input_list[torseur_input_size-1]));
                    
                    draw_left_menu();
                    draw_center();
                    delay(DEBOUNCE_DELAY);
                } else if (hover_id == torseur_input_size+1) {
                    // We selected calc button.
                    if (torseur_input_size > 0)
                        do_calculation();
                } else {
                    // We selected a torseur
                    aside_select_id = hover_id;
                    
                    hover_region = REGION_MIDDLE;
                    hover_id = 0;
                    draw_left_menu();
                    draw_center();
                    delay(DEBOUNCE_DELAY);
                    continue;
                }
                

            }
            
            key = kb_ScanGroup(kb_group_7);
            if (key & kb_Down) {
                hover_id++;
                if (hover_id > torseur_input_size + 1)
                    hover_id = torseur_input_size + 1;
                if (torseur_input_size >= MAX_TORSEUR)
                    if (hover_id == torseur_input_size)
                        hover_id++;
                draw_left_menu();
                delay(DEBOUNCE_DELAY);
            } else if (key & kb_Up) {
                if (hover_id != 0)
                    hover_id--;
                if (torseur_input_size >= MAX_TORSEUR)
                    if (hover_id == torseur_input_size)
                        hover_id--;
                draw_left_menu();
                delay(DEBOUNCE_DELAY);
            }
        } else if (hover_region == REGION_MIDDLE) {
            key = kb_ScanGroup(kb_group_6);
            if (key & kb_Clear) {
                hover_region = REGION_ASIDE;
                hover_id = aside_select_id;
                draw_left_menu();
                draw_center();
                delay(DEBOUNCE_DELAY);
                continue;
            }
            if (key & kb_Enter) {
                
                switch(hover_id) {
                    case 0:
                        current_input = &(torseur_input_list[aside_select_id]->name);
                        break;
                    case 1:
                        current_input = &(torseur_input_list[aside_select_id]->point);
                        break;
                    case 2:
                        current_input = &(torseur_input_list[aside_select_id]->px);
                        break;
                    case 3:
                        current_input = &(torseur_input_list[aside_select_id]->py);
                        break;
                    case 4:
                        current_input = &(torseur_input_list[aside_select_id]->pz);
                        break;
                    case 5:
                        current_input = &(torseur_input_list[aside_select_id]->rx);
                        break;
                    case 6:
                        current_input = &(torseur_input_list[aside_select_id]->ry);
                        break;
                    case 7:
                        current_input = &(torseur_input_list[aside_select_id]->rz);
                        break;
                    case 8:
                        current_input = &(torseur_input_list[aside_select_id]->mx);
                        break;
                    case 9:
                        current_input = &(torseur_input_list[aside_select_id]->my);
                        break;
                    case 10:
                        current_input = &(torseur_input_list[aside_select_id]->mz);
                        break;
                }
                
                hover_region = REGION_INPUT;
                draw_center();
                delay(DEBOUNCE_DELAY);
                continue;
            }
        
            key = kb_ScanGroup(kb_group_7);
            if (key & kb_Down) {
                hover_id++;
                if (hover_id > 10)
                    hover_id = 10;
                draw_center();
                delay(DEBOUNCE_DELAY);
            } else if (key & kb_Up) {
                if (hover_id != 0)
                    hover_id--;
                draw_center();
                delay(DEBOUNCE_DELAY);
            }
        } else if (hover_region == REGION_INPUT) {
            c = input_character();
            len = strlen(*current_input);
            if (c == 0) {
                continue;
            } else if (c == '\n') {
                if (len == 0) {
                    continue;
                }
                hover_region = REGION_MIDDLE;
                current_input = NULL;
                draw_center();
                delay(DEBOUNCE_DELAY);
                continue;
            } else if (c == '\b') {
                if (len == 0) {
                    continue;
                }
                *current_input = realloc(*current_input, len);
                (*current_input)[len-1] = '\0';
                draw_input();
                delay(DEBOUNCE_DELAY);
            } else {
                *current_input = realloc(*current_input, len + 2);
                (*current_input)[len] = c;
                (*current_input)[len + 1] = '\0';
                draw_input();
                delay(DEBOUNCE_DELAY);
            }
        }
    }
}

void free_torseur_inputs() {
    for(i = 0; i < torseur_input_size; i++) {
        free(torseur_input_list[i]->name);
        free(torseur_input_list[i]->point);
        free(torseur_input_list[i]->px);
        free(torseur_input_list[i]->py);
        free(torseur_input_list[i]->pz);
        free(torseur_input_list[i]->rx);
        free(torseur_input_list[i]->ry);
        free(torseur_input_list[i]->rz);
        free(torseur_input_list[i]->mx);
        free(torseur_input_list[i]->my);
        free(torseur_input_list[i]->mz);
        free(torseur_input_list[i]);
        torseur_input_list[i] = NULL;
    }
    torseur_input_size = 0;
    free(torseur_input_list);
    torseur_input_list = NULL;
}

void alloc_torseur_input(TorseurInput** ptr) {
    *ptr = malloc(sizeof(TorseurInput));
    (*ptr)->name = malloc(2);
    (*ptr)->name[0] = 'T';
    (*ptr)->name[1] = '\0';
    (*ptr)->point = malloc(2);
    (*ptr)->point[0] = 'P';
    (*ptr)->point[1] = '\0';
    (*ptr)->px = malloc(2);
    (*ptr)->px[0] = '0';
    (*ptr)->px[1] = '\0';
    (*ptr)->py = malloc(2);
    (*ptr)->py[0] = '0';
    (*ptr)->py[1] = '\0';
    (*ptr)->pz = malloc(2);
    (*ptr)->pz[0] = '0';
    (*ptr)->pz[1] = '\0';
    (*ptr)->rx = malloc(2);
    (*ptr)->rx[0] = '0';
    (*ptr)->rx[1] = '\0';
    (*ptr)->ry = malloc(2);
    (*ptr)->ry[0] = '0';
    (*ptr)->ry[1] = '\0';
    (*ptr)->rz = malloc(2);
    (*ptr)->rz[0] = '0';
    (*ptr)->rz[1] = '\0';
    (*ptr)->mx = malloc(2);
    (*ptr)->mx[0] = '0';
    (*ptr)->mx[1] = '\0';
    (*ptr)->my = malloc(2);
    (*ptr)->my[0] = '0';
    (*ptr)->my[1] = '\0';
    (*ptr)->mz = malloc(2);
    (*ptr)->mz[0] = '0';
    (*ptr)->mz[1] = '\0';
}

void main(void) {
    setup();
    
    draw_header();
    draw_left_menu();
    draw_input();
    
    loop();
    free_torseur_inputs();
    gfx_End();
}

void draw_center(void) {
    gfx_SetColor(GLOBAL_BG_COLOR);
    gfx_FillRectangle(ASIDE_WIDTH, HEADER_HEIGHT, gfx_lcdWidth - ASIDE_WIDTH, gfx_lcdHeight - HEADER_HEIGHT);
    
    gfx_SetTextFGColor(GLOBAL_FG_COLOR);
    gfx_SetColor(HOVER_BG_COLOR2);
    
    offset = 0;
    gfx_PrintStringXY("Torseur ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 0 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth("Torseur ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 0) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 0 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->name), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->name, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 0 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY("-- Point: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 2 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth("-- Point:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 1) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 2 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->point), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->point, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 2 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" px: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 3 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" px:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 2) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 3 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->px), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->px, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 3 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" py: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 4 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" py:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 3) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 4 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->py), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->py, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 4 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" pz: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 5 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" pz:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 4) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 5 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->pz), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->pz, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 5 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY("-- Resultante: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 7 * INDIVIDUAL_HEIGHT + 2);

    offset = 0;
    gfx_PrintStringXY(" x: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 8 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" x:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 5) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 8 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->rx), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->rx, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 8 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" y: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 9 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" y:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 6) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 9 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->ry), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->ry, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 9 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" z: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 10 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" z:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 7) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 10 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->rz), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->rz, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 10 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY("-- Moment: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 12 * INDIVIDUAL_HEIGHT + 2);

    offset = 0;
    gfx_PrintStringXY(" l: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 13 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" l:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 8) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 13 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->mx), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->mx, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 13 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" m: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 14 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" m:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 9) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 14 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->my), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->my, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 14 * INDIVIDUAL_HEIGHT + 2);
    
    offset = 0;
    gfx_PrintStringXY(" n: ", ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 15 * INDIVIDUAL_HEIGHT + 2);
    offset += gfx_GetStringWidth(" n:  ");
    
    if (hover_region == REGION_MIDDLE && hover_id == 10) {
        gfx_FillRectangle(ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 15 * INDIVIDUAL_HEIGHT, gfx_GetStringWidth(torseur_input_list[aside_select_id]->mz), INDIVIDUAL_HEIGHT);
    }
    gfx_PrintStringXY(torseur_input_list[aside_select_id]->mz, ASIDE_WIDTH + 1 + offset, HEADER_HEIGHT + 15 * INDIVIDUAL_HEIGHT + 2);
    
    draw_input();
    
}

char out;
char input_character(void) {
    out = 0;
    key = kb_ScanGroup(kb_group_2);
    if (key & kb_Math)
        if (alpha)
            if (shift)
                out = 'A';
            else
                out = 'a';
    if (key & kb_Recip)
        if (alpha)
            if (shift)
                out = 'D';
            else
                out = 'd';
    if (key & kb_Square)
        if (alpha)
            if (shift)
                out = 'I';
            else
                out = 'i';
    if (key & kb_Log)
        if (alpha)
            if (shift)
                out = 'N';
            else
                out = 'n';
    if (key & kb_Ln)
        if (alpha)
            if (shift)
                out = 'S';
            else
                out = 's';
    if (key & kb_Sto)
        if (alpha)
            if (shift)
                out = 'X';
            else
                out = 'x';
    
    key = kb_ScanGroup(kb_group_3);
    
    if (key & kb_0)
        if (alpha)
            out = ' ';
        else
            out = '0';
    if (key & kb_1)
        if (alpha)
            if (shift)
                out = 'Y';
            else
                out = 'y';
        else
            out = '1';
    if (key & kb_4)
        if (alpha)
            if (shift)
                out = 'T';
            else
                out = 't';
        else
            out = '4';
    if (key & kb_7)
        if (alpha)
            if (shift)
                out = 'O';
            else
                out = 'o';
        else
            out = '7';
    if (key & kb_Comma)
        if (alpha)
            if (shift)
                out = 'J';
            else
                out = 'j';
    if (key & kb_Sin)
        if (alpha)
            if (shift)
                out = 'E';
            else
                out = 'e';
    if (key & kb_Apps)
        if (alpha)
            if (shift)
                out = 'B';
            else
                out = 'b';
    
    key = kb_ScanGroup(kb_group_4);
    
    if (key & kb_DecPnt)
        if (!alpha)
            out = '.';
    if (key & kb_2)
        if (alpha)
            if (shift)
                out = 'Z';
            else
                out = 'z';
        else
            out = '2';
    if (key & kb_5)
        if (alpha)
            if (shift)
                out = 'U';
            else
                out = 'u';
        else
            out = '5';
    if (key & kb_8)
        if (alpha)
            if (shift)
                out = 'P';
            else
                out = 'p';
        else
            out = '8';
    if (key & kb_LParen)
        if (alpha)
            if (shift)
                out = 'K';
            else
                out = 'k';
    if (key & kb_Cos)
        if (alpha)
            if (shift)
                out = 'F';
            else
                out = 'f';
    if (key & kb_Prgm)
        if (alpha)
            if (shift)
                out = 'C';
            else
                out = 'c';
    
    key = kb_ScanGroup(kb_group_5);
    
    if (key & kb_Chs)
        if (alpha)
            out = '?';
        else
            out = '-';
    if (key & kb_3)
        if (!alpha)
            out = '3';
    if (key & kb_6)
        if (alpha)
            if (shift)
                out = 'V';
            else
                out = 'v';
        else
            out = '6';
    if (key & kb_9)
        if (alpha)
            if (shift)
                out = 'Q';
            else
                out = 'q';
        else
            out = '9';
    if (key & kb_RParen)
        if (alpha)
            if (shift)
                out = 'L';
            else
                out = 'l';
    if (key & kb_Tan)
        if (alpha)
            if (shift)
                out = 'G';
            else
                out = 'g';
    
    key = kb_ScanGroup(kb_group_6);
    
    if (key & kb_Enter)
        out = '\n';
    if (key & kb_Add)
        if (!alpha)
            out = '+';
    if (key & kb_Sub)
        if (alpha)
            if (shift)
                out = 'W';
            else
                out = 'w';
        else
            out = '-';
    if (key & kb_Mul)
        if (alpha)
            if (shift)
                out = 'R';
            else
                out = 'r';
    if (key & kb_Div)
        if (alpha)
            if (shift)
                out = 'M';
            else
                out = 'm';
        else
            out = '/';
    if (key & kb_Power)
        if (alpha)
            if (shift)
                out = 'H';
            else
                out = 'h';
    if (key & kb_Clear)
        out = '\b';
    return out;
}

void draw_input(void) {
    gfx_SetColor(HOVER_BG_COLOR2);
    gfx_FillRectangle(ASIDE_WIDTH, gfx_lcdHeight - INDIVIDUAL_HEIGHT, gfx_lcdWidth - ASIDE_WIDTH, INDIVIDUAL_HEIGHT);
    
    if (current_input != NULL) {
        gfx_SetTextFGColor(GLOBAL_FG_COLOR);
        gfx_PrintStringXY(*current_input, ASIDE_WIDTH + 3, gfx_lcdHeight - INDIVIDUAL_HEIGHT + 2);
        // gfx_PrintStringXY(" |", ASIDE_WIDTH + 3 + gfx_GetStringWidth(*current_input), gfx_lcdHeight - INDIVIDUAL_HEIGHT + 2);
    }
}

void draw_left_menu(void) {
    // gfx_FillScreen(gfx_white);
    // Draw background
    gfx_SetColor(ASIDE_BG_COLOR);
    gfx_FillRectangle(0, HEADER_HEIGHT, ASIDE_WIDTH, gfx_lcdHeight - HEADER_HEIGHT);
    
    for(i = 0; i < torseur_input_size; i++) {
        gfx_SetTextFGColor(ASIDE_FG_COLOR);
        if (i == aside_select_id) {
            gfx_SetColor(SELECT_BG_COLOR);
            gfx_SetTextFGColor(HOVER_FG_COLOR);
            gfx_FillRectangle(0, HEADER_HEIGHT + i * INDIVIDUAL_HEIGHT, ASIDE_WIDTH, INDIVIDUAL_HEIGHT);
        }
        if (hover_region == REGION_ASIDE && i == hover_id) {
            gfx_SetColor(HOVER_BG_COLOR);
            gfx_SetTextFGColor(HOVER_FG_COLOR);
            gfx_FillRectangle(0, HEADER_HEIGHT + i * INDIVIDUAL_HEIGHT, ASIDE_WIDTH, INDIVIDUAL_HEIGHT);
        }
        gfx_PrintStringXY(torseur_input_list[i]->name, 3, HEADER_HEIGHT + i * INDIVIDUAL_HEIGHT + 2);
    }
    
    if (torseur_input_size < MAX_TORSEUR) {
        gfx_SetTextFGColor(ASIDE_FG_COLOR);
        if (hover_region == REGION_ASIDE && hover_id == torseur_input_size) {
            gfx_SetColor(HOVER_BG_COLOR);
            gfx_SetTextFGColor(HOVER_FG_COLOR);
            gfx_FillRectangle(0, HEADER_HEIGHT + hover_id * INDIVIDUAL_HEIGHT, ASIDE_WIDTH, INDIVIDUAL_HEIGHT);
        }
        gfx_PrintStringXY("New...", 3, HEADER_HEIGHT + torseur_input_size * INDIVIDUAL_HEIGHT + 2);
    }

    
    gfx_SetColor(BUTTON_BG_COLOR);
    if (hover_region == REGION_ASIDE && hover_id == torseur_input_size + 1) {
        gfx_SetColor(HBUTTON_BG_COLOR);
    }
    gfx_FillRectangle(0, gfx_lcdHeight - INDIVIDUAL_HEIGHT, ASIDE_WIDTH, INDIVIDUAL_HEIGHT);
    gfx_SetTextFGColor(HOVER_FG_COLOR);
    gfx_PrintStringXY("Calc...", 3, gfx_lcdHeight - INDIVIDUAL_HEIGHT + 2);
}

const char* title = "Torseurs v0.1 - M4x1m3";

void setup(void) {
    alpha = false;
    shift = false;
    
    current_input = NULL;
    
    torseur_input_size = 0;
    aside_select_id = 0;
    hover_region = REGION_ASIDE;
    hover_id = 0;
    torseur_input_list = NULL;
    
    gfx_Begin(gfx_8bpp);
    
    gfx_palette[GLOBAL_FG_COLOR] = gfx_RGBTo1555(0, 0, 0);
    gfx_palette[GLOBAL_BG_COLOR] = gfx_RGBTo1555(255, 255, 255);
    gfx_palette[HEADER_FG_COLOR] = gfx_RGBTo1555(255, 255, 255);
    gfx_palette[HEADER_BG_COLOR] = gfx_RGBTo1555(81, 110, 123);
    gfx_palette[SELECT_BG_COLOR] = gfx_RGBTo1555(17, 24, 27);
    gfx_palette[HOVER_FG_COLOR]  = gfx_RGBTo1555(255, 255, 255);
    gfx_palette[HOVER_BG_COLOR]  = gfx_RGBTo1555(28, 39, 43);
    gfx_palette[ASIDE_FG_COLOR]  = gfx_RGBTo1555(197, 252, 232);
    gfx_palette[ASIDE_BG_COLOR]  = gfx_RGBTo1555(35, 49, 54);
    gfx_palette[BUTTON_BG_COLOR]  = gfx_RGBTo1555(0, 119, 174);
    gfx_palette[HBUTTON_BG_COLOR] = gfx_RGBTo1555(0, 80, 118);
    gfx_palette[HOVER_BG_COLOR2]  = gfx_RGBTo1555(180, 180, 180);
    
    gfx_FillScreen(GLOBAL_BG_COLOR);
}

void draw_header(void) {
    gfx_SetColor(HEADER_BG_COLOR);
    gfx_FillRectangle(0, 0, gfx_lcdWidth, HEADER_HEIGHT);

    gfx_SetTextFGColor(HEADER_FG_COLOR);
    gfx_PrintStringXY(title, (LCD_WIDTH - gfx_GetStringWidth(title)) / 2, (HEADER_HEIGHT - FONT_HEIGHT) / 2);
    
    if (alpha) {
        if (shift) {
            gfx_PrintStringXY("A", 2, (HEADER_HEIGHT - FONT_HEIGHT) / 2);
        } else {
            gfx_PrintStringXY("a", 2, (HEADER_HEIGHT - FONT_HEIGHT) / 2);
        }
    }
}

