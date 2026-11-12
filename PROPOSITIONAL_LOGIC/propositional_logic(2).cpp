#include<iostream>
#include<stack>
#include<string>
#include<vector>
#include<algorithm>
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
	else if (op == '&' || op == '|') {
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

		if (cur == ' ' || cur == '\t' ) { continue; }
		else if (isalpha(cur)) {
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

void dfs(TreeNode* node, vector<char>& vars) {
	if (!node) return;
	if (node->op == 0) vars.push_back(node->var);
	dfs(node->left, vars);
	dfs(node->right, vars);
}

vector<char> getVariables(TreeNode* root) {
	vector<char> vars;
	dfs(root, vars); 
	sort(vars.begin(), vars.end());
	vars.erase(unique(vars.begin(), vars.end()), vars.end());
	return vars;
}

vector<TreeNode*> getPostOrder(TreeNode* root){
	vector<TreeNode*> postOrder;
	if (!root) { return postOrder; }
	stack<TreeNode*> s1, s2;
	s1.push(root);
	
	while (!s1.empty()) {
		TreeNode* node = s1.top();
		s1.pop();
		s2.push(node);
		if (node->left) {
			s1.push(node->left);
		}
		if (node->right) {
			s1.push(node->right);
		}
	}

	while (!s2.empty()) {
		postOrder.push_back(s2.top());
		s2.pop();
	}
	return postOrder;
}

bool evaluate(const vector<TreeNode*>& postOrder, const vector<char>& vars, int mask) {
	stack<bool> s;
	int n = vars.size();
	for (TreeNode* node : postOrder) {
		if (node->op == 0) {
			int idx = 0;
			for (int i = 0; i < n; ++i) {
				if (vars[i] == node->var) {
					idx = i;
					break;
				}
			}
			bool val = mask >> (n - 1 - idx) & 1;
			s.push(val);
		}
		else if (node->op == '!') {
			bool v = s.top();
			s.pop();
			s.push(!v);
		}
		else if (node->op == '&') {
			bool r = s.top(); s.pop();
			bool l = s.top(); s.pop();
			s.push(l && r);
		}
		else if (node->op == '|') {
			bool r = s.top(); s.pop();
			bool l = s.top(); s.pop();
			s.push(l || r);
		}
	}
	return s.top();
}

vector<bool> generateTruthTable(const vector<TreeNode*>& post, const vector<char>& vars) {
	int n = vars.size();
	int total_rows = 1 << n;
	vector<bool> results(total_rows);

	for (char v : vars) cout << v << " ";
	cout << "result" << endl;
	cout << "-----------------" << endl;

	for (int mask = 0; mask < total_rows; ++mask) {
		for (int i = 0; i < n; ++i) {
			cout << ((mask >> (n - 1 - i)) & 1) << " ";
		}

		results[mask] = evaluate(post, vars, mask);
		cout << "| " << (results[mask] ? "1" : "0") << endl;
	}
	cout << "-----------------" << endl;
	return results;
}

void judgeFormulaType(const vector<bool>& results) {
	bool has_true = false, has_false = false;
	for (bool r : results) {
		if (r) has_true = true;
		else has_false = true;
	}

	cout << "公式类型: ";
	if (has_true && !has_false) cout << "重言式" << endl;
	else if (!has_true && has_false) cout << "矛盾式" << endl;
	else cout << "可满足式" << endl;
}

string buildMinTerm(const vector<char>& vars, int mask) {
	string term = "";
	int n = vars.size();
	for (int i = 0; i < n; ++i) {
		if (((mask >> (n - 1 - i)) & 1) == 1) term += vars[i];
		else term += "!" + string(1, vars[i]);
		if (i < n - 1) term += "&&";
	}
	return term;
}

string buildMaxTerm(const vector<char>& vars, int mask) {
	string term = "";
	int n = vars.size();
	for (int i = 0; i < n; ++i) {
		if (((mask >> (n - 1 - i)) & 1) == 0) term += vars[i];
		else term += "!" + string(1, vars[i]);
		if (i < n - 1) term += "||";
	}
	return term;
}

void printCanonicalForms(const vector<bool>& results, const vector<char>& vars) {
	vector<string> pdnf_terms, pcnf_terms;

	for (int mask = 0; mask < results.size(); ++mask) {
		if (results[mask]) pdnf_terms.push_back(buildMinTerm(vars, mask));
		else pcnf_terms.push_back(buildMaxTerm(vars, mask));
	}

	cout << "主析取范式 (PDNF): ";
	if (pdnf_terms.empty()) cout << "无 " << endl;
	else if (pdnf_terms.size() == 1) {
		cout << pdnf_terms[0] << endl;
	}
	else {
		for (int i = 0; i < pdnf_terms.size(); ++i) {
			cout << "(" << pdnf_terms[i] << ")";
			if (i < pdnf_terms.size() - 1) cout << " || ";
		}
		cout << endl;
	}

	cout << "主合取范式 (PCNF): ";
	if (pcnf_terms.empty()) cout << "无 " << endl;
	else if (pcnf_terms.size() == 1) {
		cout << pcnf_terms[0] << endl;
	}
	else {
		for (int i = 0; i < pcnf_terms.size(); ++i) {
			cout << "(" << pcnf_terms[i] << ")";
			if (i < pcnf_terms.size() - 1) cout << " && ";
		}
		cout << endl;
	}
}

void processFormula(TreeNode* root) {
	vector<char> vars = getVariables(root);
	vector<TreeNode*> post = getPostOrder(root);

	vector<bool> results = generateTruthTable(post, vars);

	judgeFormulaType(results);

	printCanonicalForms(results, vars);
}

void deleteTree(TreeNode* root) {
	if (root == nullptr) { return; }
	deleteTree(root->left);
	deleteTree(root->right);
	delete root;
}

int main() {
	string s;

	while (getline(cin, s)) {
		if (s.empty()) { continue; }
		TreeNode* root = nullptr;
		if (parse(s, root)) {
			cout << '/' << s << '/' << "是命题公式" << endl;
			processFormula(root);
		}
		else {
			cout << '/' << s << '/' << "不是命题公式" << endl;
		}
		deleteTree(root);
	}
	return 0;
}
		