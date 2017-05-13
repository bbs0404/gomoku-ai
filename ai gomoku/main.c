#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AI 'O'
#define PLA 'X'
#define NONE ' '
int pointMap[19][19] = { 0 };
char map[19][20] = { 0 };
int activeMap[19][19] = { 0 };
int turn = 0;

typedef struct Vec2 {
	int x;
	int y;
} Vec2;

typedef struct Node {
	char map[19][20];
	int value;
	Vec2 action;
	struct Node * parent;
	struct Node * children;
	struct Node * next;
	int isTerminal;
	int isOrigin;
} Node;

int isEnded(Vec2 action) {
	int i = action.x, j = action.y;
	int player, count = 0;
	if (turn % 2 == 0)
		player = AI;
	else
		player = PLA;
	while (j > 0 && map[i][j - 1] == player) {
		--j;
	}
	while (j < 19 && map[i][j] == player) {
		++count;
		++j;
	}
	if (count == 5)
		return 1;

	j = action.y;
	count = 0;

	while (i > 0 && map[i - 1][j] == player) {
		--i;
	}
	while (i < 19 && map[i][j] == player) {
		++count;
		++i;
	}
	if (count == 5)
		return 1;

	i = action.x;
	count = 0;

	while (i > 0 && j > 0 && map[i - 1][j - 1] == player) {
		--i;
		--j;
	}
	while (i < 19 && j < 19 && map[i][j] == player) {
		++count;
		++i;
		++j;
	}
	if (count == 5)
		return 1;

	i = action.x;
	j = action.y;
	count = 0;

	while (i > 0 && j < 18 && map[i - 1][j + 1] == player) {
		--i;
		++j;
	}
	while (i < 19 && j >= 0 && map[i][j] == player) {
		++count;
		++i;
		--j;
	}
	if (count == 5)
		return 1;
	return 0;
}

int evaluatePattern(int count, int open, int turn, int * three, int * four) {
	if (open == 0)
		return 0;
	switch (count) {
	case 4:
		if (!turn) {
			if (open == 2) {
				*four = 1;
				return 500000;
			}
			else {
				if (*three || *four)
					return 500000;
				else {
					*four = 1;
					return 50;
				}
			}
		}
		else {
			*four = 1;
			return 100000000;
		}
		break;
	case 3:
		if (!turn) {
			if (open == 2) {
				*three = 1;
				if (*four)
					return 500000;
				return 50;
			}
			else
				return 5;
		}
		else {
			if (open == 2) {
				*three = 1;
				return 10000;
			}
			else
				return 7;
		}
	case 2:
		if (open == 2)
			return 5;
		else
			return 2;		
	case 1:
		if (open == 2)
			return 2;
		else
			return 1;
	default:
		return 1;
	}
}

int evaluateEnemyPattern(int count, int open, int turn, int * three, int * four) {
	if (open == 0)
		return 0;
	switch (count) {
	case 4:
		if (turn) {
			if (open == 2) {
				*four = 1;
				return -750000;
			}
			else {
				if (*four || *three)
					return -750000;
				else {
					*four = 1;
					return -75;
				}
			}
		}
		else {
			*four = 1;
			return -150000000;
		}
		break;
	case 3:
		if (turn) {
			if (open == 2) {
				*three = 1;
				if (*four)
					return -750000;
				return -75;
			}
			else
				return -7;
		}
		else {
			if (open == 2) {
				*three = 1;
				return -150000;
			}
			else
				return -7;
		}
	case 2:
		if (open == 2)
			return -8;
		else
			return -3;
	case 1:
		if (open == 2)
			return -2;
		else
			return -1;
	default:
		return -1;
	}
}

void evaluate(Node * state) {
	int t = (turn % 2 == 0);
	int three = 0, four = 0;
	int ethree = 0, efour = 0;
	char tmpMap[19][20];
	for (int i = 0; i < 19; ++i) { //→
		for (int j = 0; j < 19; ++j) {
			if (state->map[i][j] == NONE)
				continue;
			int count = 0, openEnd = 0;
			if (state->map[i][j] == AI) {
				if (j > 0 && state->map[i][j - 1] == NONE)
					++openEnd;
				while (j < 19 && state->map[i][j] == AI) {
					++j;
					++count;
				}
				if (j < 19 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = 1000000000;
					return;
				}
				state->value += evaluatePattern(count, openEnd, t, &three, &four);
				--j;
			}
			else {
				if (j > 0 && state->map[i][j - 1] == NONE)
					++openEnd;
				while (j < 19 && state->map[i][j] == PLA) {
					++j;
					++count;
				}
				if (j < 19 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = -200000000;
					return;
				}
				state->value += evaluateEnemyPattern(count, openEnd, t, &ethree, &efour);
				--j;
			}
		}
	}

	for (int j = 0; j < 19; ++j) { //↓
		for (int i = 0; i < 19; ++i) {
			if (state->map[i][j] == NONE)
				continue;

			int count = 0, openEnd = 0;
			if (state->map[i][j] == AI) {

				if (i > 0 && state->map[i - 1][j] == NONE)
					++openEnd;
				while (i < 19 && state->map[i][j] == AI) {
					++i;
					++count;
				}
				if (i < 19 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = 1000000000;
					return;
				}

				state->value += evaluatePattern(count, openEnd, t, &three, &four);
				--i;
			}
			else {

				if (i > 0 && state->map[i - 1][j] == NONE)
					++openEnd;
				while (i < 19 && state->map[i][j] == PLA) {
					++i;
					++count;
				}
				if (i < 19 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = -2000000000;
					return;
				}

				state->value += evaluateEnemyPattern(count, openEnd, t, &ethree, &efour);
				--i;
			}
		}

	}

	for (int i = 0; i < 19; ++i) {
		for (int j = 0; j < 20; ++j) {
			tmpMap[i][j] = state->map[i][j];
		}
	}

	for (int i = 0; i < 19; ++i) { // ↘
		for (int j = 0; j < 19; ++j) {
			if (tmpMap[i][j] == NONE)
				continue;
			int count = 0, openEnd = 0, tmpI = i, tmpJ = j;
			if (tmpMap[i][j] == AI) {
				if (j > 0 && i > 0 && tmpMap[i - 1][j - 1] == NONE)
					++openEnd;
				while (i < 19 && j < 19 && tmpMap[i][j] == AI) {
					tmpMap[i][j] = NONE;
					++i;
					++j;
					++count;
				}
				if (i < 19 && j < 19 && tmpMap[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = 1000000000;
					return;
				}

				state->value += evaluatePattern(count, openEnd, t, &three, &four);
			}
			else {
				if (j > 0 && i > 0 && tmpMap[i - 1][j - 1] == NONE)
					++openEnd;
				while (i < 19 && j < 19 && tmpMap[i][j] == PLA) {
					++i;
					++j;
					++count;
				}
				if (i < 19 && j < 19 && tmpMap[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = -2000000000;
					return;
				}

				state->value += evaluateEnemyPattern(count, openEnd, t, &ethree, &efour);
			}
			i = tmpI;
			j = tmpJ;
		}
	}

	for (int k = 0; k < 19; ++k) { // ↙
		for (int i = 0, j = k - i; j >= 0; ++i, --j) {
			if (state->map[i][j] == NONE)
				continue;
			int count = 0, openEnd = 0;
			if (state->map[i][j] == AI) {
				if (j < 19 && i > 0 && state->map[i - 1][j + 1] == NONE)
					++openEnd;
				while (i < 19 && j >= 0 && state->map[i][j] == AI) {
					++i;
					--j;
					++count;
				}
				if (i < 19 && j >= 0 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = 1000000000;
					return;
				}

				state->value += evaluatePattern(count, openEnd, t, &three, &four);
				--i;
				++j;
			}
			else {
				if (j < 19 && i > 0 && state->map[i - 1][j + 1] == NONE)
					++openEnd;
				while (i < 19 && j >= 0 && state->map[i][j] == PLA) {
					++i;
					--j;
					++count;
				}
				if (i < 19 && j >= 0 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = -2000000000;
					return;
				}

				state->value += evaluateEnemyPattern(count, openEnd, t, &ethree, &efour);
				--i;
				++j;
			}
		}
	}

	for (int k = 19; k < 37; ++k) {
		for (int j = 18, i = k - j; i < 19; ++i, --j) {
			if (state->map[i][j] == NONE)
				continue;
			int count = 0, openEnd = 0;
			if (state->map[i][j] == AI) {
				if (j < 19 && i > 0 && state->map[i - 1][j + 1] == NONE)
					++openEnd;
				while (i < 19 && j >= 0 && state->map[i][j] == AI) {
					++i;
					--j;
					++count;
				}
				if (i < 19 && j >= 0 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = 1000000000;
					return;
				}

				state->value += evaluatePattern(count, openEnd, t, &three, &four);
				--i;
				++j;
			}
			else {
				if (j < 19 && i > 0 && state->map[i - 1][j + 1] == NONE)
					++openEnd;
				while (i < 19 && j >= 0 && state->map[i][j] == PLA) {
					++i;
					--j;
					++count;
				}
				if (i < 19 && j >= 0 && state->map[i][j] == NONE)
					++openEnd;

				if (count == 5) {
					state->isTerminal = 1;
					state->value = -2000000000;
					return;
				}

				state->value += evaluateEnemyPattern(count, openEnd, t, &ethree, &efour);
				--i;
				++j;
			}
		}
	}
}

void updateActiveMap() {
	for (int i = 0; i < 19; ++i)
		for (int j = 0; j < 19; ++j)
			if (map[i][j] != NONE)
				for (int k = max(i - 1, 0); k <= min(i + 1, 18); ++k)
					for (int l = max(j - 1, 0); l <= min(j + 1, 18); ++l)
						activeMap[k][l] = 1;
	for (int i = 0; i < 19; ++i)
		for (int j = 0; j < 19; ++j)
			if (map[i][j] != NONE)
				activeMap[i][j] = 0;
}

void printMap() {
	printf("\t");
	for (int i = 0; i < 10; ++i)
		printf("%d", i);
	for (int i = 0; i < 9; ++i)
		printf("%c", 'A' + i);
	printf("\n");
	for (int i = 0; i < 19; ++i) {
		if (i < 10)
			printf("%d\t", i);
		else
			printf("%c\t", 'A' + i - 10);
		printf("%s\n", map[i]);
	}
	printf("\n");
}

void init_Node(Node * node, char map[19][20], Vec2 act) {
	for (int i = 0; i < 19; ++i) {
		for (int j = 0; j < 20; ++j) {
			node->map[i][j] = map[i][j];
		}
	}
	node->parent = NULL;
	node->children = NULL;
	node->next = NULL;
	node->isTerminal = 0;
	node->action = act;
	node->value = 0;
}

void findActions(Node * state, int player) {
	Node * child = NULL;
	for (int i = 0; i < 19; ++i) {
		for (int j = 0; j < 19; ++j) {
			if (state->map[i][j] != NONE)
				continue;
			if (!activeMap[i][j])
				continue;
			if (child == NULL) {
				child = malloc(sizeof(Node));
				state->children = child;
			}
			else {
				child->next = malloc(sizeof(Node));
				child = child->next;
			}
			Vec2 action;
			action.x = i;
			action.y = j;
			init_Node(child, state->map, action);
			child->map[i][j] = player;
			child->parent = state;
		}
	}
}

void freeTree(Node * node) {
	Node * n = node->children;
	free(node);
	while (n) {
		Node * tmp = n->next;
		freeTree(n);
		n = tmp;
	}
}

Vec2 alpha_beta_search(Node * state, int depth) {
	Vec2 action;
	action.x = action.y = -1;
	int v = max_value(state, -2000000001, 2000000001, depth);
	for (Node * s = state->children; s; s = s->next) {
		if (s->value == v) {
			action = s->action;
			break;
		}
	}

	if (action.x == -1)
		printf("Error\n");

	freeTree(state);
	return action;
}

int max_value(Node * state, int alpha, int beta, int depth) {
	if (state->isTerminal || depth == 0) {
		evaluate(state);
		int tmp = state->value;
		if (!state->isOrigin && !state->parent->isOrigin)
			freeTree(state);
		return tmp;
	}
	findActions(state,AI);
	int v = -10000000;
	Node * n = state->children;
	while (n) {
		Node * tmp = n->next;
		int minValue = min_value(n, alpha, beta, depth - 1);
		v = max(v, minValue);
		if (v >= beta) {
			state->value = v;
			if (!state->isOrigin && !state->parent->isOrigin)
				freeTree(state);
			else if (!state->isOrigin) {
				Node * s = state->children;
				while (s) {
					Node * tmp = s->next;
					freeTree(s);
					s = tmp;
				}
			}
			return v;
		}
		alpha = max(alpha, v);
		n = tmp;
	}
	state->value = v;
	if (!state->isOrigin && !state->parent->isOrigin)
		freeTree(state);
	else if (!state->isOrigin) {
		Node * s = state->children;
		while (s) {
			Node * tmp = s->next;
			freeTree(s);
			s = tmp;
		}
	}
	return v;
}

int min_value(Node * state, int alpha, int beta, int depth) {
	if (state->isTerminal || depth == 0) {
		evaluate(state);
		int tmp = state->value;
		if (!state->isOrigin && !state->parent->isOrigin)
			freeTree(state);
		return tmp;
	}
	findActions(state,PLA);
	int v = 10000000;
	Node * n = state->children;
	while (n) {
		Node * tmp = n->next;
		int maxValue = max_value(n, alpha, beta, depth - 1);
		v = min(v, maxValue);
		if (v <= alpha) {
			state->value = v;
			if (!state->isOrigin && !state->parent->isOrigin)
				freeTree(state);
			else if (!state->isOrigin) {
				Node * s = state->children;
				while (s) {
					Node * tmp = s->next;
					freeTree(s);
					s = tmp;
				}
			}
			return v;
		}
		beta = min(beta, v);
		n = tmp;
	}
	state->value = v;
	if (!state->isOrigin && !state->parent->isOrigin)
		freeTree(state);
	else if (!state->isOrigin) {
		Node * s = state->children;
		while (s) {
			Node * tmp = s->next;
			freeTree(s);
			s = tmp;
		}
	}
	return v;
}

int main() {

	int i, j;
	Vec2 action;

	for (int i = 0; i < 19; ++i) {
		for (int j = 0; j < 19; ++j)
			map[i][j] = ' ';
		map[i][19] = '\0';
	}

	printf("1 : 플레이어 시작, 0 : AI 시작\n");
	scanf("%d", &turn);

	while (1) {
		if (turn % 2 == 0) {
			Node * origin = malloc(sizeof(Node));
			Vec2 act;
			act.x = act.y = 0;
			init_Node(origin, map, act);
			if (turn != 0) {
				Vec2 action = alpha_beta_search(origin, 4);
				map[action.x][action.y] = AI;
				printf("%d %d\n",action.x,action.y);
				if (isEnded(action)) {
					printMap();
					printf("AI WINS\n");
					break;
				}
			}
			else {
				printf("9 9\n");
				map[9][9] = AI;
			}
		}
		else {
			scanf("%d %d", &i, &j);
			if (map[i][j] != NONE) {
				printf("Invalid Input\n");
				continue;
			}
			map[i][j] = PLA;
			Vec2 action;
			action.x = i;
			action.y = j;
			if (isEnded(action)) {
				printMap();
				printf("PLAYER WINS\n");
				break;
			}
		}
		printMap();
		updateActiveMap();
		++turn;
	}
	scanf("%d", &i);
}