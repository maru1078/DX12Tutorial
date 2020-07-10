#ifndef KEY_TYPE_H_
#define KEY_TYPE_H_

enum class KeyType
{
	Escape = 0x01,
	Num1 = 0x02,
	Num2 = 0x03,
	Num3 = 0x04,
	Num4 = 0x05,
	Num5 = 0x06,
	Num6 = 0x07,
	Num7 = 0x08,
	Num8 = 0x09,
	Num9 = 0x0A,
	Num0 = 0x0B,
	Minus = 0x0C,    /* - on main keyboard */
	Equals = 0x0D,
	Backspace = 0x0E,    /* backspace */
	Tab = 0x0F,
	Q = 0x10,
	W = 0x11,
	E = 0x12,
	R = 0x13,
	T = 0x14,
	Y = 0x15,
	U = 0x16,
	I = 0x17,
	O = 0x18,
	P = 0x19,
	LeftBracket = 0x1A,
	RightBracket = 0x1B,
	Return = 0x1C,    /* Enter on main keyboard */
	LeftControl = 0x1D,
	A = 0x1E,
	S = 0x1F,
	D = 0x20,
	F = 0x21,
	G = 0x22,
	H = 0x23,
	J = 0x24,
	K = 0x25,
	L = 0x26,
	Semicolon = 0x27,
	Apostrophe = 0x28,
	Grave = 0x29,    /* accent grave */
	LeftShift = 0x2A,
	Backslash = 0x2B,
	Z = 0x2C,
	X = 0x2D,
	C = 0x2E,
	V = 0x2F,
	B = 0x30,
	N = 0x31,
	M = 0x32,
	Comma = 0x33,
	Period = 0x34,    /* . on main keyboard */
	Slash = 0x35,    /* / on main keyboard */
	RightShift = 0x36,
	KeypadMultiply = 0x37,    /* * on numeric keypad */
	LeftMenu = 0x38,    /* left Alt */
	Space = 0x39,
	Capital = 0x3A,
	F1 = 0x3B,
	F2 = 0x3C,
	F3 = 0x3D,
	F4 = 0x3E,
	F5 = 0x3F,
	F6 = 0x40,
	F7 = 0x41,
	F8 = 0x42,
	F9 = 0x43,
	F10 = 0x44,
	Numlock = 0x45,
	ScrollLock = 0x46,    /* Scroll Lock */
	Keypad7 = 0x47,
	Keypad8 = 0x48,
	Keypad9 = 0x49,
	KeypadSubtract = 0x4A,    /* - on numeric keypad */
	Keypad4 = 0x4B,
	Keypad5 = 0x4C,
	Keypad6 = 0x4D,
	KeypadPlus = 0x4E,    /* + on numeric keypad */
	Keypad1 = 0x4F,
	Keypad2 = 0x50,
	Keypad3 = 0x51,
	Keypad0 = 0x52,
	KeypadPeriod = 0x53,    /* . on numeric keypad */
	Oem_102 = 0x56,    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	F11 = 0x57,
	F12 = 0x58,
	F13 = 0x64,    /*                     (NEC PC98) */
	F14 = 0x65,    /*                     (NEC PC98) */
	F15 = 0x66,    /*                     (NEC PC98) */
	Kana = 0x70,    /* (Japanese keyboard)            */
	Abnt_C1 = 0x73,    /* /? on Brazilian keyboard */
	Convert = 0x79,    /* (Japanese keyboard)            */
	NoConvert = 0x7B,    /* (Japanese keyboard)            */
	Yen = 0x7D,    /* (Japanese keyboard)            */
	Abnt_C2 = 0x7E,    /* Numpad . on Brazilian keyboard */
	KeypadEquals = 0x8D,    /* = on numeric keypad (NEC PC98) */
	Prevtrack = 0x90,    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	At = 0x91,    /*                     (NEC PC98) */
	Colon = 0x92,    /*                     (NEC PC98) */
	Underline = 0x93,    /*                     (NEC PC98) */
	Kanji = 0x94,    /* (Japanese keyboard)            */
	Stop = 0x95,    /*                     (NEC PC98) */
	Ax = 0x96,    /*                     (Japan AX) */
	Unlabeled = 0x97,    /*                        (J3100) */
	NextTrack = 0x99,    /* Next Track */
	KeypadEnter = 0x9C,    /* Enter on numeric keypad */
	RightControl = 0x9D,
	Mute = 0xA0,    /* Mute */
	Calculator = 0xA1,    /* Calculator */
	PlayPause = 0xA2,    /* Play / Pause */
	MediaStop = 0xA4,    /* Media Stop */
	VolumeDown = 0xAE,    /* Volume - */
	VolumeUp = 0xB0,    /* Volume + */
	Webhome = 0xB2,    /* Web home */
	KeypadComma = 0xB3,    /* , on numeric keypad (NEC PC98) */
	KeypadDivide = 0xB5,    /* / on numeric keypad */
	Sysrq = 0xB7,
	RightAlt = 0xB8,    /* right Alt */
	Pause = 0xC5,    /* Pause */
	Home = 0xC7,    /* Home on arrow keypad */
	Up = 0xC8,    /* UpArrow on arrow keypad */
	PgUp = 0xC9,    /* PgUp on arrow keypad */
	Left = 0xCB,    /* LeftArrow on arrow keypad */
	Right = 0xCD,    /* RightArrow on arrow keypad */
	End = 0xCF,    /* End on arrow keypad */
	Down = 0xD0,    /* DownArrow on arrow keypad */
	PgDn = 0xD1,    /* PgDn on arrow keypad */
	Insert = 0xD2,    /* Insert on arrow keypad */
	Delete = 0xD3,    /* Delete on arrow keypad */
	LeftWindows = 0xDB,    /* Left Windows key */
	RightWindows = 0xDC,    /* Right Windows key */
	Apps = 0xDD,    /* AppMenu key */
	Power = 0xDE,    /* System Power */
	Sleep = 0xDF,    /* System Sleep */
	Wake = 0xE3,    /* System Wake */
	WebSearch = 0xE5,    /* Web Search */
	WebFavorites = 0xE6,    /* Web Favorites */
	WebRefresh = 0xE7,    /* Web Refresh */
	WebStop = 0xE8,    /* Web Stop */
	WebForward = 0xE9,    /* Web Forward */
	WebBack = 0xEA,    /* Web Back */
	MyComputer = 0xEB,    /* My Computer */
	Mail = 0xEC,    /* Mail */
	MediaSelect = 0xED,    /* Media Select */
};

#endif // !KEY_TYPE_H_

