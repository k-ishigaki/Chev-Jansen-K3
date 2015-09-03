#include <stdbool.h>

typedef struct {
	int r;
	int phi1;
	int phi2;
} PoleCood;

typedef struct {
	int x;
	int y;
	int theta;
} RectCood;

void init_motion(void);
void move(int inv_curv, int distance, int velocity);
void move_to_pole(PoleCood* pc, int velocity);
void move_to_rect(RectCood* rc, int velocity);
bool is_moving(void);
void set_checkpoint(int num);
PoleCood* get_pole_cood(int num);
RectCood* get_rect_cood(int num);

