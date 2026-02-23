#ifndef TRUST_H
#define TRUST_H


/* API pubblica del componente */
int trust_init(void);
void trust_modify(float action, const char *action_name);

void acceptance (void);
void trust (void);
void admiration (void);

#endif