#include "game.h"

int main(int argc, char** argv)
{
	printf
	(
		   "\n==aaaaaaaaaaa==\n"
		   "  OpenGL Boilerplate scene, 2 \"trees\" in questionable light\n"
		   "  By Shay O'Connor <github: shayoc21>\n"
		   "=================\n\n"
	);
	Game game;
	initGame(&game);
	runGame(&game);
	closeGame(&game);
}
