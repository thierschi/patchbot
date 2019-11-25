#pragma once

/*
	Enumerations fpr Terrain-types, Robot-types and Action-types
*/

enum class Terrain {
	STEEL_PLANKS = ' ',
	PATCHBOT_START = 'p',
	BUGGER_START = '1',
	PUSHER_START = '2',
	DIGGER_START = '3',
	SWIMMER_START = '4',
	FOLLOWER_START = '5',
	HUNTER_START = '6',
	SNIFFER_START = '7',
	ABYSS = 'O',
	WATER = '~',
	MAIN_SERVER = 'P',
	ALIEN_GRASS = 'g',
	GRAVEL = '.',
	SECRET_PASSAGE = 'x',
	MANUAL_DOOR = 'd',
	AUTOMATIC_DOOR = 'D',
	CONCRETE_WALL = '#',
	ROCK_WALL = 'M'
};

enum class Robot {
	PATCHBOT = 'p',
	BUGGER = '1',
	PUSHER = '2',
	DIGGER = '3',
	SWIMMER = '4',
	FOLLOWER = '5',
	HUNTER = '6',
	SNIFFER = '7'
};

enum class Action {
	WALK,
	DIE,
	WIN,
	WALK_AND_WAIT,
	WAIT,
	OBSTRUCTED,
	DIG
};