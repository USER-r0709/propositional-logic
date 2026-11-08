#include<iostream>
#include<stack>
#include<string>
using namespace std;

struct TreeNode {
	char op;
	char var;
	TreeNode* left = nullptr;
	TreeNode* right = nullptr;

	TreeNode() = default;
	TreeNode(char op_, char var_) :op(op_), var(var_) {}
};

bool isalpha(char cur) {
	if (cur >= 97 && cur <= 122) { return true; }
	else { return false; }
}

bool reduce(stack<TreeNode*>& A, stack<char>& B) {
	if (B.empty()) { return false; }
	char op = B.top();
	B.pop();
	if (op == '!') {
		if (A.empty()) { return false; }
		TreeNode* child = A.top();
		A.pop();
		TreeNode* node = new TreeNode(op, ' ');
		node->left = child;
		A.push(node);
	}
	else if(op == '&' || op == '|') {
		if (A.size() < 2) { return false; }
		TreeNode* r = A.top();
		A.pop();
		TreeNode* l = A.top();
		A.pop();
		TreeNode* node = new TreeNode(op, ' ');
		node->right = r;
		node->left = l;
		A.push(node);
	}
	else { return false; }
	return true;
}

bool parse(const string& input, TreeNode*& root) {
	stack<TreeNode*> A;
	stack<char> B;

	for (int i = 0; i < input.length(); ++i) {
		char cur = input[i];

		if (isalpha(cur)) {
			A.push(new TreeNode(0, cur));
		}
		else if (cur == '(' || cur == '!') {
			B.push(cur);
		}
		else if (cur == '&') {
			if (i + 1 == input.length() || input[i + 1] != '&') { return false; }
			++i;
			while (!B.empty() && B.top() != '(' && B.top() != '|') {
				if (!reduce(A, B)) { return false; }
			}
			B.push('&');
		}
		else if (cur == '|') {
			if (i + 1 == input.length() || input[i + 1] != '|') { return false; }
			++i;
			while (!B.empty() && B.top() != '(') {
				if (!reduce(A, B)) { return false; }
			}
			B.push('|');
		}
		else if (cur == ')') {
			while (!B.empty() && B.top() != '(') {
				if (!reduce(A, B)) { return false; }
			}
			if (B.empty()) { return false; }
			B.pop();
		}
		else { return false; }
	}

	while (!B.empty()) {
		if (B.top() == '(') { return false; }
		if (!reduce(A, B)) { return false; }
	}

	if (A.size() != 1) { return false; }
	root = A.top();
	return true;
}


int main() {
	string s;
	
	while (getline(cin, s)) {
		if (s.empty()) { continue; }
		TreeNode* root = nullptr;
		if (parse(s, root)) {
			cout << '/' << s << '/' << "是命题公式" << endl;
		}
		else {
			cout << '/' << s << '/' << "不是命题公式" << endl;
		}
	}
}