#include "slashLib.h"

void freeTab(void **tab, int tabLen) {
        for (int i = 0; i < tabLen; i++) {
                free(tab[i]);
        }
        free(tab);
}