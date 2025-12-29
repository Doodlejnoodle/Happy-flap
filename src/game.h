#ifndef GAME_H
#define GAME_H

void flappy_init(void);
void flappy_update(int jump_pressed);
int flappy_check_collision(void);
int flappy_get_score(void);

#endif
