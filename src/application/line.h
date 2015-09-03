enum LineState {
	NO_LINE;
	LEFT_EDGE;
	RIGHT_EDGE;
	T_EDGE;
	IN_LINE;
};

void init_line(void);
enum LineState get_line_state(void);
int get_line_position(void);
