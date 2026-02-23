#ifndef SURPRISE_H
#define SURPRISE_H


/* API pubblica del componente */
int surprise_init(void);
void surprise_modify(float action, const char *action_name);

void distraction (void);
void surprise (void);
void amazement (void);

#endif