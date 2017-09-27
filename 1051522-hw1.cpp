#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;
#define PAIR pair<int, int>

/*
1051522 HW1 Final
*/

struct Card {
	//2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A
	vector<PAIR> orderCard;      //���Ƥ@�ˮɤ������P�j�p
	int cardCount[5] = { 0 };    //1/2/3/4�i�@�˪��P�`�M���}�C
	int myCard[14][5] = { 0 };   //[index][suit] �����⪺���@�i�P, [0][index] �P��⪺�P�`�M, [index][0] �P�Ʀr���P�`�M
};

struct Player {
	Card card;
	int score;
};

int CheckScore(Player &p) {
	//�έp1/2/3/4�i�@�˪��P���������X��
	for (int i = 1; i <= 13; ++i) {
		int total = p.card.myCard[i][0];

		if (total > 0) {
			p.card.cardCount[total]++;
			p.card.orderCard.push_back(PAIR(total, i));
		}
	}
	sort(p.card.orderCard.begin(), p.card.orderCard.end());

	//�ˬd�@��2 ���3 �T��4 ��Ī7 �|��8
	if (p.card.cardCount[4] == 1)
		return 8;
	else if (p.card.cardCount[3] == 1 && p.card.cardCount[2] == 1)
		return 7;
	else if (p.card.cardCount[3] == 1)
		return 4;
	else if (p.card.cardCount[2] == 2)
		return 3;
	else if (p.card.cardCount[2] == 1)
		return 2;

	//�ˬd���P1 ���l5 �P��6 �P�ᶶ9
	bool flush = false, straight = false;

	//�P��
	if (p.card.myCard[0][1] == 5 || p.card.myCard[0][2] == 5 || p.card.myCard[0][3] == 5 || p.card.myCard[0][4] == 5)
		flush = true;

	//���l
	if (p.card.orderCard[4].second - p.card.orderCard[0].second == 4)
		straight = true;

	//�P�_�O����
	if (flush == true && straight == true)
		return 9;
	else if (straight == true)
		return 5;
	else if (flush == true)
		return 6;
	return 1;
}

//0: Black win, 1: White win, 2: Tie
int Compare(Player &W, Player &B) {
	if (W.score != B.score)
		return (W.score > B.score);
	else {
		for (auto it1 = B.card.orderCard.rbegin(), it2 = W.card.orderCard.rbegin(); it1 != B.card.orderCard.rend(); ++it1, ++it2) {
			if ((*it1).second > (*it2).second) return 0;
			if ((*it1).second < (*it2).second) return 1;
		}
		return 2;
	}
}

//�ഫinput���ۤv����Ƶ��c
void Convert(string input, Player &B, Player &W) {
	map<char, int> valueTable = {
		{ 'A', 13 },{ '2', 1 },{ '3', 2 },{ '4', 3 },{ '5', 4 },{ '6', 5 },{ '7', 6 },{ '8', 7 },{ '9', 8 },{ 'T', 9 },{ 'J', 10 },{ 'Q', 11 },{ 'K', 12 },
		{ 'C', 1 },{ 'D', 2 },{ 'H', 3 },{ 'S', 4 }
	};

	//�έp��P���Ʀr�M���
	for (int i = 0; i < input.size(); i += 3) {
		int digit = valueTable[input[i]];
		int suit = valueTable[input[i + 1]];

		if (i < input.size() / 2) {
			B.card.myCard[0][suit]++;
			B.card.myCard[digit][0]++;
			B.card.myCard[digit][suit]++;
		}
		else {
			W.card.myCard[0][suit]++;
			W.card.myCard[digit][0]++;
			W.card.myCard[digit][suit]++;
		}
	}
}

int main() {
	string str;

	while (getline(cin, str)) {
		Player W, B;

		Convert(str, B, W);
		B.score = CheckScore(B);
		W.score = CheckScore(W);

		int ans = Compare(W, B);

		if (ans == 0)
			cout << "Black wins." << endl;
		else if (ans == 1)
			cout << "White wins." << endl;
		else
			cout << "Tie." << endl;
	}

	system("PAUSE");
	return 0;
}