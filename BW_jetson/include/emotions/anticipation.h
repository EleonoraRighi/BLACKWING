#ifndef ANTICIPATION_H
#define ANTICIPATION_H


/* API pubblica del componente */
int anticipation_init(void);
void anticipation_modify(float action, const char *action_name);

void interest (void);
void anticipation (void);
void vigilance (void);

#endif