#ifndef FIR_RENDER_H
#define FIR_RENDER_H

#include <stdio.h>

#include "ast.h"

int fir_render(FILE *out, const FirNode *root);

#endif
