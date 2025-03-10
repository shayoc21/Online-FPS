#include "game.h"

int main(int argc, char** argv)
{
	printf
	(
		   "\n==i hate bsp==\n"
		   "  By Shay O'Connor <github: shayoc21>\n"
		   "=================\n\n"
	);
	Game game;
	initGame(&game);
	runGame(&game);
	closeGame(&game);
}
