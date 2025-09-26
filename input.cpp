struct Button_State {
	bool is_down, is_changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,

	BUTTON_COUNT,
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};