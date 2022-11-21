#ifndef PROJET_SY5_TOKEN_H
#define PROJET_SY5_TOKEN_H

enum tok_type{
    arg,
    cmd,
    ope,
    redirect
};

typedef struct {
    string *name;
    enum tok_type type;
} token;
#endif
