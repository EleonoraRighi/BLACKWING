#ifndef FEAR_H
#define FEAR_H


/* API pubblica del componente */
int fear_init(void);
void fear_modify(float action, const char *action_name);

void apprehension (void);
void fear (void);
void terror (void);

#endif