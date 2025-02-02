#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <assert.h>

#define NUM_THREADS     32

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

enum { NOUGHTS, CROSSES, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW };
const int Directions[4] = { 1, 5, 4, 6 };
const int ConvertTo25[9] = { 6,7,8,11,12,13,16,17,18, };
const int InMiddle = 4;
const int Corners[4] = { 0, 2, 6, 8 };
int ply = 0;
int positions = 0;
int maxPly = 0;
typedef struct _thread_data_t
{
	int tid;
	double stuff;
} thread_data_t;
int GetNumForDir(int startSq, const int dir, const int* board, const int us) {
	int found = 0;
	while (board[startSq] != BORDER) {
		if (board[startSq] != us) {
			break;
		}
		found++;
		startSq += dir;
	}
	return found;
}
int FindThreeInARow(const int* board, const int ourindex, const int us)
{
	int DirIndex = 0;
	int Dir = 0;
	int threeCount = 1;
	for (DirIndex - 0; DirIndex < 4; ++DirIndex)
	{
		Dir = Directions[DirIndex];
		threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
		threeCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
		if (threeCount == 3)
		{
			break;
		}
		threeCount = 1;
	}
	return threeCount;
}

int FindThreeInARowAllBoard(const int* board, const int us)
{
	int threeFound = 0;
	int index;
	for (index = 0; index < 9; ++index)
	{
		if (board[ConvertTo25[index]] == us)
		{
			if (FindThreeInARow(board, ConvertTo25[index], us) == 3)
			{
				threeFound = 1;
				break;
			}
		}
	}
	return threeFound;
}

int EvalForWin(const int* board, const int us)
{
	if (FindThreeInARowAllBoard(board, us) != 0)
		return 1;
	if (FindThreeInARowAllBoard(board, us ^ 1) != 0)
		return -1;
	return 0;
}

int MinMax(int* board, int side)
{
	int MoveList[9];
	int MoveCount = 0;
	int bestScore = -2;
	int score = -2;
	int bestMove = -1;
	int Move;
	int index;
	pthread_t thr[NUM_THREADS];
	int i, rc;
	thread_data_t thr_data[NUM_THREADS];
	for (i = 0; i < NUM_THREADS; ++i)
	{
		if (ply > maxPly)
		{
			maxPly = ply;
			positions++;
		}
		if (ply > 0)
		{
			score = EvalForWin(board, side);
			if (score != 0)
			{
				return score;
			}
		}
	}
	for (index = 0; index < 9; ++index)
	{
		if (board[ConvertTo25[index]] == EMPTY)
		{
			MoveList[MoveCount++] = ConvertTo25[index];
		}
	}
	for (index = 0; index < MoveCount; ++index)
	{
		Move = MoveList[index];
		board[Move] = side;
		ply++;
		score = -MinMax(board, side ^ 1);
		if (score > bestScore)
		{
			bestScore = score;
			bestMove = Move;
		}
		board[Move] = EMPTY;
		ply--;
	}
	for (i = 0; i < NUM_THREADS; ++i)
	{
		if (MoveCount == 0)
		{
			bestScore = FindThreeInARowAllBoard(board, side);
		}
		for (i = 0; i < NUM_THREADS; ++i)
		{
			if (ply != 0)
				return bestScore;
			else
				return bestMove;
		}
	}
	for (i = 0; i < NUM_THREADS; ++i)
	{
		pthread_join(thr[i], NULL);
	}
	return EXIT_SUCCESS;
	for (i = 0; i < NUM_THREADS; ++i)
	{
		pthread_join(thr[i], NULL);
	}
	return EXIT_SUCCESS;
}

void InitialiseBoard(int* board)
{
	int index = 0;
	for (index = 0; index < 25; ++index)
	{
		board[index] = BORDER;
	}
	for (index = 0; index < 9; ++index)
	{
		board[ConvertTo25[index]] = EMPTY;
	}
}

void PrintBoard(const int* board)
{
	int index = 0;
	char pceChars[] = "OX|-";
	printf("\n\nBoard:\n\n");
	for (index = 0; index < 9; ++index)
	{
		if (index != 0 && index % 3 == 0)
		{
			printf("\n\n");
		}
		printf("%4c", pceChars[board[ConvertTo25[index]]]);
	}
	printf("\n");
}

int GetNextBest(const int* board)
{
	int ourMove = ConvertTo25[InMiddle];
	if (board[ourMove] == EMPTY)
	{
		return ourMove;
	}
	int index = 0;
	ourMove = -1;
	for (index = 0; index < 4; index++)
	{
		ourMove = ConvertTo25[Corners[index]];
		if (board[ourMove] == EMPTY)
		{
			break;
		}
		ourMove = -1;
	}
	return ourMove;
}

int GetWinningMove(int* board, const int side)
{
	int ourMove = -1;
	int winFound = 0;
	int index = 0;
	for (index = 0; index < 9; ++index)
	{
		if (board[ConvertTo25[index]] == EMPTY)
		{
			ourMove = ConvertTo25[index];
			board[ourMove] = side;
			if (FindThreeInARow(board, ourMove, side) == 3)
			{
				winFound = 1;
			}
			board[ourMove] = EMPTY;
			if (winFound == 1)
			{
				break;
			}
			ourMove = -1;
		}
	}
	return ourMove;
}

int GetComputerMove(int* board, const int side)
{
	ply = 0;
	positions = 0;
	maxPly = 0;
	int best = MinMax(board, side);
	printf("Finished searching through positions in tree:%d max depth:%d best move:%d\n", positions, maxPly, best);
	return best;
}

int GetHumanMove(const int* board)
{
	char userInput[4];
	int moveOk = 0;
	int move = -1;
	while (moveOk == 0)
	{
		printf("Please enter a move from 1 to 9:");
		fgets(userInput, 3, stdin);
		fflush(stdin);
		if (strlen(userInput) != 2)
		{
			printf("Shucks! You entered an invalid strlen()! \n");
			continue;
		}
		if (sscanf(userInput, "%d", &move) != 1)
		{
			move = -1;
			printf("Shucks! You entered an invalid sscanf()! \n");
			continue;
		}
		if (move < 1 || move > 9)
		{
			move = -1;
			printf("Shucks! You entered an invalid range! \n");
			continue;
		}
		move--;
		if (board[ConvertTo25[move]] != EMPTY)
		{
			move = -1;
			printf("Shucks! Square not available\n");
			continue;
		}
		moveOk = 1;
	}
	printf("You are selecting position...%d\n", (move + 1));
	return ConvertTo25[move];
}

int HasEmpty(const int* board)
{
	int index = 0;
	for (index = 0; index < 9; ++index)
	{
		if (board[ConvertTo25[index]] == EMPTY) return 1;
	}
	return 0;
}

void MakeMove(int* board, const int sq, const int side)
{
	board[sq] = side;
}

void RunGame()
{
	printf("%s TIC TAC TOE \n", KRED);
	int GameOver = 0;
	int Side = NOUGHTS;
	int LastMoveMade = 0;
	int board[25];
	InitialiseBoard(&board[0]);
	PrintBoard(&board[0]);
	while (!GameOver)
	{
		if (Side == NOUGHTS)
		{
			LastMoveMade = GetHumanMove(&board[0]);
			MakeMove(&board[0], LastMoveMade, Side);
			Side = CROSSES;
			printf("%s COMPUTER MOVE \n", KBLU);
		}
		else
		{
			LastMoveMade = GetComputerMove(&board[0], Side);
			MakeMove(&board[0], LastMoveMade, Side);
			Side = NOUGHTS;
			PrintBoard(&board[0]);
			printf("%s PLAYER MOVE \n", KNRM);
		}
		if (FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3)
		{
			printf("Game over!\n");
			GameOver = 1;
			if (Side == NOUGHTS)
			{
				printf("Computer Wins\n");
			}
			else
			{
				printf("Human Wins\n");
			}
		}
		if (!HasEmpty(board))
		{
			printf("Game Over! I know, it's a shame it can't last forever! \n");
			GameOver = 1;
			printf("It's a draw! Come on, try harder for the win next time!");
		}
	}
}

int main()
{
	srand(time(NULL));
	RunGame();
	return 0;
}