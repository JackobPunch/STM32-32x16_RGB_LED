#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#ifdef __cplusplus
extern "C"
{
#endif

    void countdown_init(void);
    void countdown(void);
    void countdown_start(void);
    void countdown_stop(void);
    void countdown_reset(void);

#ifdef __cplusplus
}

// C++ functions
void update_display(void);

#endif

#endif