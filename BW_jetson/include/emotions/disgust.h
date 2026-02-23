#ifndef DISGUST_H
#define DISGUST_H


/* API pubblica del componente */
int disgust_init(void);
void disgust_modify(float action, const char *action_name);

void boredom (void);
void disgust (void);
void loathing (void);

#endif