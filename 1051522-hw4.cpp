#include <iostream>
using namespace std;
/*
	懶人之修改operator >>版本
*/

struct Triple {
	int row;
	int col;
	int value;
};

class Matrix; // forward declaration

class MatrixNode {
	friend class Matrix;
	friend istream &operator>>(istream &is, Matrix &matrix);
	friend ostream &operator<<(ostream &os, Matrix &matrix);
	public:
	MatrixNode(bool b, Triple &t); // constructor
	private:
	MatrixNode *down;
	MatrixNode *right;
	bool head;
	union {
		MatrixNode *next;
		Triple triple;
	};
};

MatrixNode::MatrixNode(bool b, Triple &t)  // constructor
{
	head = b;
	if (b)
		right = down = next = this; // row/column head node
	else
		triple = t; // element node or header node for list of header nodes
}

class Matrix {
	// read in a matrix and set up its linked representation
	friend istream &operator>>(istream &is, Matrix &matrix);

	// print out matrix in row major form
	friend ostream &operator<<(ostream &os, Matrix &matrix);
	public:
	Matrix(); // default constructor with empty body

	Matrix(const Matrix &matrixToCopy); // copy constructor

	~Matrix(); // destructor in which the matrix is erased

	const Matrix &operator=(const Matrix &op2);

	Matrix operator+(Matrix &op2); // Returns the sum of two sparse matrices

	void printColumnMajor(); // print out the matrix in column major form

	void erase(); // erase the matrix
	private:
	MatrixNode *headNode; // a pointer to the header node for the list of header nodes
};


Matrix::Matrix()
	: headNode(NULL) {
}

Matrix::Matrix(const Matrix &matrixToCopy) {
	headNode = NULL;
	*this = matrixToCopy;
}

Matrix::~Matrix() {
	erase();
}

const Matrix &Matrix::operator=(const Matrix &op2) {
	erase();

	Triple t = { op2.headNode->triple.row, op2.headNode->triple.col, op2.headNode->triple.value };
	headNode = new MatrixNode(false, t);

	int maxSize = (op2.headNode->triple.col > op2.headNode->triple.row ? op2.headNode->triple.col : op2.headNode->triple.row);

	t = {};
	MatrixNode **head = new MatrixNode*[maxSize]();
	for (int i = 0; i < maxSize; ++i)
		head[i] = new MatrixNode(true, t);

	int i = 0, currentRow = 0;
	MatrixNode *last = head[0];
	for (MatrixNode *myHead = head[i], *op2Head = op2.headNode->right; op2Head != op2.headNode; myHead = head[++i], op2Head = op2Head->next) {
		for (MatrixNode *op2Col = op2Head->right; op2Col != op2Head; op2Col = op2Col->right) {
			t = op2Col->triple;
			if (t.row > currentRow) {
				last->right = head[currentRow];
				currentRow = t.row;
				last = head[currentRow];
			}
			last = last->right = new MatrixNode(false, t);
			head[t.col]->next = head[t.col]->next->down = last;
		}
	}
	last->right = head[currentRow];

	for (int i = 0; i < maxSize; i++)
		head[i]->next->down = head[i];

	for (int i = 0; i < maxSize - 1; i++)
		head[i]->next = head[i + 1];

	headNode->right = head[0];
	head[maxSize - 1]->next = headNode;
	return *this;
}

istream& operator>>(istream &is, Matrix &matrix) {  // read in a matrix and set up its linked representation.
	Triple s;
	is >> s.row >> s.col >> s.value; // matrix dimensions

	if (s.col <= 0 || s.row <= 0)
		return is;

	// set up header node for the list of header nodes.
	matrix.headNode = new MatrixNode(false, s);

	int p = (s.col > s.row) ? s.col : s.row;

	// at least one row and column
	MatrixNode **head = new MatrixNode *[p];
	Triple zeroTriple = {};
	for (int i = 0; i < p; i++)
		head[i] = new MatrixNode(true, zeroTriple);

	int currentRow = 0;
	MatrixNode *last = head[0]; // last node in current row

	for (int i = 0; i < s.value; i++) // input triples
	{
		Triple t;
		is >> t.row >> t.col >> t.value;
		if (t.row > currentRow) // close current row
		{
			last->right = head[currentRow];
			currentRow = t.row;
			last = head[currentRow];
		}
		last = last->right = new MatrixNode(false, t); // link new node into row list
		head[t.col]->next = head[t.col]->next->down = last; // link into column list
	}

	last->right = head[currentRow];// close last row

								   // close all column lists
	for (int i = 0; i < s.col; i++)
		head[i]->next->down = head[i];

	// link the header nodes together
	for (int i = 0; i < p - 1; i++)
		head[i]->next = head[i + 1];

	head[p - 1]->next = matrix.headNode;
	matrix.headNode->right = head[0];
	delete[] head;

	return is;
}

Matrix Matrix::operator+(Matrix &op2) {
	Matrix sum;
	Triple t = {};

	int maxSize = (headNode->triple.col > headNode->triple.row) ? headNode->triple.col : headNode->triple.row;

	MatrixNode **head = new MatrixNode*[maxSize]();
	for (int i = 0; i < maxSize; ++i)
		head[i] = new MatrixNode(true, t);

	int i = 0, count = 0, currentRow = 0;
	MatrixNode *last = head[0]; 
	for (MatrixNode *sumHead = head[i], *myHead = headNode->right, *op2Head = op2.headNode->right; myHead != headNode; sumHead = head[++i], myHead = myHead->next, op2Head = op2Head->next) {
		MatrixNode *temp1 = myHead->right, *temp2 = op2Head->right;
		while (temp1 != myHead || temp2 != op2Head) {
			if (temp2 == op2Head || (temp1->triple.col < temp2->triple.col && temp1 != myHead)) { t = temp1->triple; temp1 = temp1->right; }
			else if (temp1 == myHead || (temp1->triple.col > temp2->triple.col && temp2 != op2Head)) { t = temp2->triple; temp2 = temp2->right; }
			else {
				t = { temp1->triple.row, temp1->triple.col, temp1->triple.value + temp2->triple.value };
				temp1 = temp1->right, temp2 = temp2->right;
				if (t.value == 0) continue;
			}
			++count;
			if (t.row > currentRow) {
				last->right = head[currentRow];
				currentRow = t.row;
				last = head[currentRow];
			}

			last = last->right = new MatrixNode(false, t);
			head[t.col]->next = head[t.col]->next->down = last;
		}
	}
	last->right = head[currentRow];

	for (int i = 0; i < maxSize; i++)
		head[i]->next->down = head[i];

	for (int i = 0; i < maxSize - 1; i++)
		head[i]->next = head[i + 1];

	t = { headNode->triple.row, headNode->triple.col, count };
	sum.headNode = new MatrixNode(false, t);
	sum.headNode->right = head[0];
	head[maxSize - 1]->next = sum.headNode;
	return sum;
}

void Matrix::printColumnMajor() {  // print out the matrix in column major form
	if (headNode != NULL) {
		cout << headNode->triple.row << " ";
		cout << headNode->triple.col << " ";
		cout << headNode->triple.value << endl;

		for (MatrixNode *head = headNode->right; head != headNode; head = head->next)
			for (MatrixNode *entry = head->down; entry != head; entry = entry->down) {
				cout << entry->triple.row << " ";
				cout << entry->triple.col << " ";
				cout << entry->triple.value << endl;
			}
	}
}

void Matrix::erase() {  // erase the matrix, return the nodes to the heap
	MatrixNode *x, *y;
	if (headNode != NULL) {
		MatrixNode *head = headNode->right;

		while (head != headNode) // free the triple and header nodes by row
		{
			y = head->right;
			while (y != head) {
				x = y;
				y = y->right;
				delete x;
			}

			x = head;
			head = head->next;
			delete x;
		}

		delete headNode;
		headNode = NULL;
	}
}

ostream& operator<<(ostream &os, Matrix &matrix) {  // print out the matrix in row major form
	if (matrix.headNode != NULL) {
		os << matrix.headNode->triple.row << " ";
		os << matrix.headNode->triple.col << " ";
		os << matrix.headNode->triple.value << endl;

		MatrixNode *head = matrix.headNode;
		for (head = head->right; head != matrix.headNode; head = head->next)
			for (MatrixNode *entry = head->right; entry != head; entry = entry->right) {
				os << entry->triple.row << " ";
				os << entry->triple.col << " ";
				os << entry->triple.value << endl;
			}
	}

	return os; // enables cout << x << y;
}

int main() {
	Matrix addend;
	Matrix adder;
	cin >> addend;
	cin >> adder;

	Matrix sum;
	sum = addend + adder;

	cout << sum << endl;
	sum.printColumnMajor();

	system("pause");
}