enum IRSensorID {
	LEFT_IR = 0;
	CENTER_LEFT_IR = 1;
	CENTER_RIGHT_IR = 2;
	RIGHT_IR = 3;
	SIZE_OF_IRSensorID = 4;
};

void init_distance(void);
int get_distance(enum IRSensorID num);