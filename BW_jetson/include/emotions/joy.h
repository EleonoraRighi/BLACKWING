#ifndef JOY_H
#define JOY_H


/* API pubblica del componente */
int joy_init(void);
void joy_modify(float action, const char *action_name);

void serenity (void);
void joy (void);
void ecstasy (void);

#endif