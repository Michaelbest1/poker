#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <memory.h>

#include "poker_cal.h"

int search_card(struct card *c, int size, struct card *dest)
{
	int i;

	for (i = 0; i < size; i++) {
		if (cmp_cards(&c[i], dest) == 0) {
			return i;
		}
	}

	return -1;
}

void init_poker(struct card *cards, int size)
{
	int i = 0;
	int s = 0;
	int j = 0;

	for (s = 0; s < 4; s++) {
		for (i = 0; i < SUITED_CARDS_NUM; i++) {
			cards[j].suit = (enum suit) s;
			cards[j].point = i + 2;
			j++;
		}
	}
}

void init_card_arr(struct card *dest, int size, struct card *except, int exsize)
{
	struct card poker[TOTAL_CARD_NUM];
	int i, j;

	init_poker(poker, TOTAL_CARD_NUM);

	for (i = 0, j = 0; i < size, j < TOTAL_CARD_NUM; j++) {
		if (search_card(except, exsize, &poker[j]) < 0) {
			copy_card(&poker[j], &dest[i++]);
		}
	}
}

void print_card(struct card *c, int wrap)
{
	if (! c) {
		printf("Error when print card! NUll pointer!\n");
		return;
	}

	printf("%d%c ", c->point, k_suit_index[c->suit]);
	if (wrap) {
		printf("\n");
	}
}

void print_cards(struct card *c, int size, int line_wrap)
{
	int i = 0;
	int j = 1;

	if (line_wrap <= 0) {
		line_wrap = DEFAULT_PRINT_LINE_WRAP;
	}

	for (i = 0; i < size; i++,j++) {
		print_card(&c[i], 0);

		if (j >= line_wrap) {
			printf("\n");
			j = 0;
		}
	}

	printf("\n");
}

/**
  The following functions check if the effective cards contain certain card type.
  The caller must ensure the cards are sorted before and the array size is 5.
  */

/**
  return the start pair position. -1 if no pair.
  */
int contain_pair(struct card *c)
{
	int i = 1;

	for (i = 1; i < EFFECTIVE_CARD_NUM; i++){
		if (c[i - 1].point == c[i].point) {
			return i - 1;
		}
	}

	return -1;
}

/**
  return the start set position. -1 if no set.
  */
int contain_set(struct card *c)
{
	if (c[0].point == c[1].point && c[1].point == c[2].point) {
		return 0;
	}

	if (c[1].point == c[2].point && c[2].point == c[3].point) {
		return 1;
	}

	if (c[2].point == c[3].point && c[3].point == c[4].point) {
		return 2;
	}

	return -1;
}

/**
  return the highest card if straight is contained. -1 if not.
  Warn: ace 2 3 4 5 need to be treated specially.
  */
int contain_straight(struct card *c)
{
	int i = 0;

	//Check card 0 to 3.
	for (i = 1; i < EFFECTIVE_CARD_NUM - 1; i++) {
		if (c[i].point != c[i - 1].point + 1) {
			return -1;
		}
	}

	//Check the last card.
	if (c[i].point == c[i - 1].point + 1) {
		return c[EFFECTIVE_CARD_NUM - 1].point;
	}

	if (c[i - 1].point == 5 && c[i].point == 14) {
		return c[EFFECTIVE_CARD_NUM - 2].point;
	}

	return -1;
}

/**
  Check the special straight 2 3 4 5 Ace
  */
int contain_lowest_straight(struct card *c)
{
	if (c[4].point != 14) {
		return -1;
	}

	int i = 0;
	for (i = 0; i < EFFECTIVE_CARD_NUM - 1; i++) {
		if (c[i].point != i + 2) {
			return -1;
		}
	}

	return 5;
}

int contain_flush(struct card *c)
{
	int i = 0;

	for (i = 1; i < EFFECTIVE_CARD_NUM; i++) {
		if (c[i].suit != c[i - 1].suit) {
			return -1;
		}
	}

	return c[EFFECTIVE_CARD_NUM - 1].point;
}

/**
	The following 9 functions determine the final card type.
	WARNING: For following functions, the caller should guarantee that the card num equals 5
	and the card is sorted by calling sort_cards function.
  */
int is_high_card(struct card *c)
{
	if (contain_pair(c) >= 0 || contain_flush(c) >= 0 || contain_straight(c) >= 0) {
		return -1;
	}

	return c[EFFECTIVE_CARD_NUM - 1].point;
}

int is_one_pair(struct card *c)
{
	if (is_two_pairs(c) >= 0 || contain_set(c) >= 0 || is_kingkong(c) >= 0) {
		return -1;
	}

	return contain_pair(c);
}

int is_two_pairs(struct card *c)
{
	if (contain_set(c) >= 0 || is_kingkong(c) >= 0) {
		return -1;
	}

	if (c[0].point == c[1].point && c[2].point == c[3].point) {
		return c[2].point;
	}

	if (c[0].point == c[1].point && c[3].point == c[4].point) {
		return c[3].point;
	}

	if (c[1].point == c[2].point && c[3].point == c[4].point) {
		return c[3].point;
	}

	return -1;
}

int is_set(struct card *c)
{
	int pos = contain_set(c);

	if (pos >= 0 && is_fullhouse(c) < 0) {
		return c[pos].point;
	}

	return -1;
}

int is_straight(struct card *c)
{
	if (contain_flush(c) < 0 && contain_straight(c) >= 0) {
		return c[EFFECTIVE_CARD_NUM - 1].point;
	}

	return -1;
}

int is_flush(struct card *c)
{
	if (contain_flush(c) >= 0 && contain_straight(c) < 0) {
		return c[EFFECTIVE_CARD_NUM - 1].point;
	}

	return -1;
}

/**
  return the set point if true. else return -1.
  */
int is_fullhouse(struct card *c)
{
	int pos = contain_set(c);

	if (pos < 0 || pos == 1) {
		return -1;
	}

	if (pos == 0 && c[3].point == c[4].point) {
		return c[0].point;
	}

	if (pos == 2 && c[0].point == c[1].point) {
		return c[2].point;
	}

	return -1;
}

int is_kingkong(struct card *c)
{
	if ((c[0].point == c[1].point) && (c[1].point == c[2].point) &&
			(c[2].point == c[3].point)) {
		return c[0].point;
	}

	if ((c[1].point == c[2].point) && (c[2].point == c[3].point) &&
			(c[3].point == c[4].point)) {
		return c[1].point;
	}

	return -1;
}

int is_straight_flush(struct card *c)
{
	if (contain_straight(c) >= 0 && contain_flush(c) >= 0) {
		return c[EFFECTIVE_CARD_NUM - 1].point;
	}

	return -1;
}

int check_eff_card_type(struct card *c)
{
	if (is_straight_flush(c) >= 0) {
		return STRAIGHT_FLUSH;
	}

	if (is_kingkong(c) >= 0) {
		return KINGKONG;
	}

	if (is_fullhouse(c) >= 0) {
		return FULLHOUSE;
	}

	if (is_flush(c) >= 0) {
		return FLUSH;
	}

	if (is_straight(c) >= 0) {
		return STRAIGHT;
	}

	if (is_set(c) >= 0) {
		return SET;
	}

	if (is_two_pairs(c) >= 0) {
		return TWO_PAIRS;
	}

	if (is_one_pair(c) >= 0) {
		return ONE_PAIR;
	}

	if (is_high_card(c) >= 0) {
		return HIGH_CARD;
	}

	printf("ERROR when check effective card type! Unknown type!");
	return -1;
}

void swap_cards(struct card *c1, struct card *c2)
{
	int tmp_point = c1->point;
	enum suit tmp_suit = c1->suit;

	c1->point = c2->point;
	c1->suit = c2->suit;

	c2->point = tmp_point;
	c2->suit = tmp_suit;
}

int cmp_cards(struct card *c1, struct card *c2)
{
	if (c1->point > c2->point) {
		return 1;
	}
	if (c1->point < c2->point) {
		return -1;
	}

	if (c1->suit > c2->suit) {
		return 1;
	}
	if (c1->suit < c2->suit) {
		return -1;
	}

	return 0;
}

void copy_card(struct card *src, struct card *dest)
{
	dest->suit = src->suit;
	dest->point = src->point;
}

void copy_cards(struct card *src, struct card *dest, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		copy_card(&src[i], &dest[i]);
	}
}

void add_cards(struct card *src, int start, struct card *dest, int dsize)
{
	int i,j;

	for (i = start, j = 0; j < dsize; i++, j++) {
		copy_card(&dest[j], &src[i]);
	}
}

void sort_cards(struct card *c, int size)
{
	int i,j;

	for (i = 0; i < size; i++) {
		for (j = size - 1; j > i; j--) {
			if (cmp_cards(&c[j], &c[j - 1]) < 0) {
				swap_cards(&c[j], &c[j - 1]);
			}
		}
	}
}

int cmp_int_arr(int *arr1, int *arr2, int size)
{
	int i = 0;

	for (i = 0; i < size; i++) {
		if (arr1[i] > arr2[i]) {
			return 1;
		}
		if (arr1[i] < arr2[i]) {
			return -1;
		}
	}

	return 0;
}

/**
  Compare two straight. They could be stright flush. The caller should ensure it.
  */
int cmp_straight(struct card *c1, struct card *c2)
{
	int l1 = contain_lowest_straight(c1);
	int l2 = contain_lowest_straight(c2);

	if (l1 < 0 && l2 >= 0) {
		return 1;
	}

	if (l1 >= 0 && l2 < 0) {
		return -1;
	}

	if (l1 >= 0 && l2 >= 0) {
		return 0;
	}

	return c1[4].point - c2[4].point;
}

int cmp_kingkong(struct card *c1, struct card *c2)
{
	int point1[2];
	int point2[2];

	//Get the point which needs to be compared first.
	//Key point is stored in index 0.
	if (c1[0].point == c1[1].point) {
		point1[0] = c1[0].point;
		point1[1] = c1[4].point;
	}
	else {
		point1[0] = c1[1].point;
		point1[1] = c1[0].point;
	}

	if (c2[0].point == c2[1].point) {
		point2[0] = c2[0].point;
		point2[1] = c2[4].point;
	}
	else {
		point2[0] = c2[1].point;
		point2[1] = c2[0].point;
	}

	return cmp_int_arr(point1, point2, 2);
}

int cmp_fullhouse(struct card *c1, struct card *c2)
{
	int point1[2];
	int point2[2];

	//Get the point which needs to be compared first.
	//Key point is stored in index 0.
	if (c1[2].point == c1[1].point) {
		point1[0] = c1[0].point;
		point1[1] = c1[4].point;
	}
	else {
		point1[0] = c1[4].point;
		point1[1] = c1[0].point;
	}

	if (c2[2].point == c2[1].point) {
		point2[0] = c2[0].point;
		point2[1] = c2[4].point;
	}
	else {
		point2[0] = c2[4].point;
		point2[1] = c2[0].point;
	}

	return cmp_int_arr(point1, point2, 2);
}

int cmp_high_card(struct card *c1, struct card *c2)
{
	int i = 4;

	for (i = 4; i >= 0; i--) {
		if (c1[i].point > c2[i].point) {
			return 1;
		}
		if (c1[i].point < c2[i].point) {
			return -1;
		}
	}

	return 0;
}

void get_set_cmp_point(struct card *c, int *point)
{
	int set_pos = contain_set(c);

	if (set_pos == 0) {
		point[0] = c[0].point;
		point[1] = c[4].point;
		point[2] = c[3].point;
	}

	if (set_pos == 1) {
		point[0] = c[1].point;
		point[1] = c[4].point;
		point[2] = c[0].point;
	}

	if (set_pos == 2) {
		point[0] = c[2].point;
		point[1] = c[1].point;
		point[2] = c[0].point;
	}
}

int cmp_set(struct card *c1, struct card *c2)
{
	int point1[3];
	int point2[3];

	get_set_cmp_point(c1, point1);
	get_set_cmp_point(c2, point2);

	return cmp_int_arr(point1, point2, 3);
}

void get_tp_cmp_point(struct card *c, int *point)
{
	//Kicker is at the beginning.
	if (c[0].point != c[1].point) {
		point[0] = c[3].point;
		point[1] = c[1].point;
		point[2] = c[0].point;
	}

	//Kicker is in the middle.
	if (c[2].point != c[1].point && c[2].point != c[3].point) {
		point[0] = c[3].point;
		point[1] = c[0].point;
		point[2] = c[2].point;
	}

	//Kicker is in the end.
	if (c[4].point != c[3].point) {
		point[0] = c[2].point;
		point[1] = c[0].point;
		point[2] = c[4].point;
	}
}

void get_op_cmp_point(struct card *c, int *point)
{
	int i,j;

	for (i = 4, j = 1; i > 0; i--) {
		if(c[i].point != c[i - 1].point) {
			point[j++] = c[i].point;
		}
		else {
			point[0] = c[i].point;
			i--;
		}
	}
}

int cmp_two_pairs(struct card *c1, struct card *c2)
{
	int point1[3];
	int point2[3];

	get_tp_cmp_point(c1, point1);
	get_tp_cmp_point(c2, point2);

	return cmp_int_arr(point1, point2, 3);
}

int cmp_one_pair(struct card *c1, struct card *c2)
{
	int point1[4];
	int point2[4];

	get_op_cmp_point(c1, point1);
	get_op_cmp_point(c2, point2);

	return cmp_int_arr(point1, point2, 4);
}

/**
  Compare card with same type. Notice that cards have already been sorted
  by cmp_comp_card.
  */
int cmp_comp_card_same_type(struct card *c1, struct card *c2, int type)
{
	switch (type) {
		case STRAIGHT_FLUSH:
		case STRAIGHT:
			return cmp_straight(c1, c2);
		case KINGKONG:
			return cmp_kingkong(c1, c2);
		case FULLHOUSE:
			return cmp_fullhouse(c1, c2);
		case FLUSH:
		case HIGH_CARD:
			return cmp_high_card(c1, c2);
		case SET:
			return cmp_set(c1, c2);
		case TWO_PAIRS:
			return cmp_two_pairs(c1, c2);
		case ONE_PAIR:
			return cmp_one_pair(c1, c2);
		default:
			printf("Error when compare card with same type! Unknow type!");
			return 0;
	}
}

/**
  Compare completed card with size 5.
  Notice that c1 and c2 may have save card because there are community cards.
  return a positive integer if c1 > c2, a negtive integer if c1 < c2, 0 if c1 = c2.
  */
int cmp_comp_card(struct card *c1, struct card *c2)
{
	sort_cards(c1, EFFECTIVE_CARD_NUM);
	sort_cards(c2, EFFECTIVE_CARD_NUM);

	//Check type first.
	int type1 = check_eff_card_type(c1);
	int type2 = check_eff_card_type(c2);

	if (type1 > type2) {
		return 1;
	}
	if (type1 < type2) {
		return -1;
	}

	//If type is the same, compare point.
	return cmp_comp_card_same_type(c1, c2, type1);
}

void combination(int *arr, int size, int n, int start, int selected, int *result, 
		struct card *avail_card, struct card *biggest)
{
	int i;
	if (n == 0) {
		/*printf("Comb result: ");
		for (i = 0; i < selected; i++) {
			printf("%d ", result[i]);
		}
		printf("\n");*/
		struct card selected_card[5];
		for (i = 0; i < 5; i++) {
			selected_card[i].suit = avail_card[result[i]].suit;
			selected_card[i].point = avail_card[result[i]].point;
		}
		
		sort_cards(selected_card, 5);

		//Use equal to override the initial value.
		if (cmp_comp_card(selected_card, biggest) >= 0) {
			copy_cards(selected_card, biggest, 5);
		}
		return;
	}

	for (i = start; i <= size - n; i++) {
		result[selected] = arr[i];
		combination(arr, size, n - 1, i + 1, selected + 1, result, avail_card, biggest);
	}
}

/**
  Determine player's final card type. Choosing the biggest 5 cards among 7 cards.
  */
void deter_final_card_type(struct card *card, struct card *result_card)
{
	int arr[7] = {0, 1, 2, 3, 4, 5, 6};
	int r[5];

	combination(arr, 7, 5, 0, 0, r, card, result_card);
}

void deter_win(struct card (*pocket_card)[2], int pocket_num, struct card *com_card, double *sttt)
{
	//Determine each player's final card type.
	int i;
	struct card ava_card[7];
	struct card *eff_card = (struct card *) malloc( sizeof(struct card) * 5 * pocket_num );

	//Init eff_card to smallest card.
	for (i = 0; i < pocket_num; i++) {
		add_cards(&eff_card[i * 5], 0, k_smallest, 5);
	}

	for (i = 0; i < pocket_num; i++) {
		add_cards(ava_card, 0, pocket_card[i], POCKET_CARD_NUM);
		add_cards(ava_card, POCKET_CARD_NUM, com_card, COMMUNITY_CARD_NUM);
		deter_final_card_type(ava_card, &eff_card[i * 5]);
	}

	//Sort according to each player's effective card.
	int *order = (int *) malloc( sizeof(int) * pocket_num );

	for (i = 0; i < pocket_num; i++) {
		order[i] = i;
	}

	int j;
	int tmp;

	//Actually we only need to know the first and the second and check if they are the same.
	for (i = 0; i < 2; i++) {
		for (j = pocket_num - 1; j > i; j--) {
			if (cmp_comp_card(&eff_card[j * 5], &eff_card[(j - 1) * 5]) > 0) {
				tmp = order[j];
				order[j] = order[j - 1];
				order[j - 1] = tmp;
			}
		}
	}

	printf("==========================================\n");
	printf("for community card: \n");
	print_cards(com_card, 5, 0);

	printf("Player final card: \n");
	print_cards(eff_card, 10, 0);

	//Check if there is tie. Win percent don't care about tie.
	if (cmp_comp_card(&eff_card[order[0] * 5], &eff_card[order[1] * 5]) != 0) {
		sttt[order[0]] += 1;
		printf("Player %d win\n", order[0]);
	}
	else {
		//Use the last index to stand for tie.
		sttt[pocket_num] += 1;
		printf("Tie\n");
	}

	free(eff_card);
	free(order);
}

void win_percent_recur(int *arr, int size, int n, int start, int selected, int *result, 
		struct card (*pocket_card)[2], int pocket_num, struct card *com_card, enum game_proc proc,
		struct card *rest_card, int rest_size, double *sttt)
{
	int i;
	if (n == 0) {
		struct card final_com_card[5]; 
		int cur_com_size = proc_com_card_num[proc];

		if (cur_com_size > 0) {
			add_cards(final_com_card, 0, com_card, cur_com_size);
		}

		int j;
		for (i = cur_com_size, j = 0; i < 5; i++, j++) {
			copy_card(&rest_card[result[j]], &final_com_card[i]);
		}

		deter_win(pocket_card, pocket_num, final_com_card, sttt);
		return;
	}

	for (i = start; i <= size - n; i++) {
		result[selected] = arr[i];
		win_percent_recur(arr, size, n - 1, i + 1, selected + 1, result, 
				pocket_card, pocket_num, com_card, proc, rest_card, rest_size, sttt);
	}
}

/**
  Calculate win percent for several pockets.
  1. Get the rest unrevealed cards.
  2. User combination to choose from rest cards so that available cards num reaches 7.
  3. From each group of available cards, determine who can win.
  4. Get the final win percent.
  */
void win_percent(struct card (*pocket_card)[2], int pocket_num, 
		enum game_proc proc, struct card *com_card, double *sttt)
{
	int community_size = proc_com_card_num[proc];
	int rest_com_size = COMMUNITY_CARD_NUM - community_size;
	int known_size = pocket_num * POCKET_CARD_NUM + community_size;
	int rest_size = TOTAL_CARD_NUM - known_size;

	struct card *known_card = (struct card *) malloc( sizeof(struct card) * known_size );
	struct card *rest_card = (struct card *) malloc( sizeof(struct card) * rest_size );

	int i;

	for (i = 0; i < pocket_num; i++) {
		add_cards(known_card, i * 2, pocket_card[i], 2);
	}

	if (community_size > 0) {
		add_cards(known_card, i * 2, com_card, community_size);
	}

	init_card_arr(rest_card, rest_size, known_card, known_size);

	int *arr = (int *) malloc( sizeof(int) * rest_size );

	for (i = 0; i < rest_size; i++) {
		arr[i] = i;
	}

	int *result = (int *) malloc( sizeof(int) * rest_com_size);
	memset(sttt, 0, sizeof(int) * rest_com_size);

	printf("Rest card: \n");
	print_cards(rest_card, rest_size, 0);

	win_percent_recur(arr, rest_size, rest_com_size, 0, 0, result,
			pocket_card, pocket_num, com_card, proc, rest_card, rest_size, sttt);

	free(known_card);
	free(rest_card);
	free(arr);
	free(result);
}

void cal_win_percent(double *sttt, int size)
{
	double sum = 0;
	int i;

	for (i = 0; i < size; i++) {
		sum += sttt[i];
	}

	double percent;
	for (i = 0; i < size - 1; i++) {
		percent = sttt[i] / sum;
		printf("Player %d win percent: %e\n", i, percent);
	}

	percent = sttt[i] / sum;

	printf("Tie percent: %e\n", percent);
	printf("\n");
}

int main()
{
	struct card pocket_card[][2] = {
		{
			{0, 14},
			{0, 8},
		},
		{
			{0, 9},
			{2, 6},
		}
	};

	struct card com_card[4] = {
		{1, 3},
		{3, 5},
		{0, 7},
		{0, 4}
	};

	int pocket_num = 2;

	double *sttt = (double *) malloc( sizeof(double) * 3 );
	memset(sttt, 0, sizeof(double) * 3);

	win_percent(pocket_card, 2, TURN, com_card, sttt);

	cal_win_percent(sttt, 3);
	
	free(sttt);

	/*struct card com_card[7] = {
		{0, 2},
		{0, 3},
		{0, 4},
		{1, 4},
		{3, 14},
		{0, 9},
		{2, 6},
	};

	struct card final_card[5];

	deter_final_card_type(com_card, final_card);

	print_cards(final_card, 5, 0);
	*/

	return 0;
}

