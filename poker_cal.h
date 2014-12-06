#ifndef __POKER_CAL_H__
#define __POKER_CAL_H__

#include <stdbool.h>

#define SUITED_CARDS_NUM 13
#define TOTAL_CARD_NUM (SUITED_CARDS_NUM * 4)
#define DEFAULT_PRINT_LINE_WRAP 13

#define POCKET_CARD_NUM 2
#define COMMUNITY_CARD_NUM 5
#define AVAIL_CARD_NUM (POCKET_CARD_NUM + COMMUNITY_CARD_NUM)
#define EFFECTIVE_CARD_NUM 5

enum suit
{
	SPADE,
	HEART,
	CLUB,
	DIAM,
	JOKER
};

enum final_hand_type
{
	HIGH_CARD,
	ONE_PAIR,
	TWO_PAIRS,
	SET,
	STRAIGHT,
	FLUSH,
	FULLHOUSE,
	KINGKONG,
	STRAIGHT_FLUSH
};

enum game_proc
{
	PREFLOP,
	FLOP,
	TURN,
	RIVER
};

struct card
{
	enum suit suit;

	/**
	  From 2 to 14. Use 14 to stand for Ace.
	  Big joker = 1, small joker = 2;
	  Of course it's useless in Texas Poker.
	  */
	int point;
};

char k_suit_index[4] = {'s', 'h', 'c', 'd'};
char k_type_index[9][32] = {
	"high_card"
	"one_pair",
	"two_pairs",
	"set",
	"straight",
	"flush",
	"fullhouse",
	"kingkong",
	"straight_flush"
};

struct card k_smallest[5] = {
	{0, 2},
	{1, 3},
	{2, 4},
	{1, 5},
	{0, 7},
};

int proc_com_card_num[4] = {0, 3, 4, 5};

struct card poker[TOTAL_CARD_NUM];

void init_poker(struct card *cards, int size);
void print_card(struct card *c, int);
void print_cards(struct card *c, int size, int line_wrap);

void sort_cards(struct card *c, int size);
void copy_card(struct card *src, struct card *dest);

int cmp_comp_card(struct card *c1, struct card *c2);

void deter_final_card_type(struct card *card, struct card *result_card);

void win_percent_recur(int *arr, int size, int n, int start, int selected, int *result, 
		struct card (*pocket_card)[2], int pocket_num, struct card *com_card, enum game_proc proc,
		struct card *rest_card, int rest_size, double *sttt);

#endif
