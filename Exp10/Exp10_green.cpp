#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cctype>
#include <unordered_map>

using namespace std;

int tempCount = 1;
int loc = 1;
vector<string> threeAddressCode;

struct Quad {
    string op, arg1, arg2, result;
};
vector<Quad> quadruples;

struct Triple {
    string op, arg1, arg2;
};
vector<Triple> triples;

// Maps temp variable (T1, T2...) to LOC index in triples
unordered_map<string, int> tempLocMap;

string newTemp() {
    return "T" + to_string(tempCount++);
}

inline int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '~': return 3; // unary minus
        default: return 0;
    }
}

string infixToPostfix(const string& infix) {
    stack<char> s;
    string postfix;
    bool lastWasOperator = true;

    for (char ch : infix) {
        if (isalnum(ch)) {
            postfix += ch;
            lastWasOperator = false;
        } else if (ch == '(') {
            s.push(ch);
            lastWasOperator = true;
        } else if (ch == ')') {
            while (!s.empty() && s.top() != '(') {
                postfix += s.top();
                s.pop();
            }
            if (!s.empty()) s.pop(); // pop '('
            lastWasOperator = false;
        } else {
            if (ch == '-' && lastWasOperator) {
                s.push('~'); // unary minus as ~
            } else {
                while (!s.empty() && precedence(s.top()) >= precedence(ch)) {
                    postfix += s.top();
                    s.pop();
                }
                s.push(ch);
            }
            lastWasOperator = true;
        }
    }

    while (!s.empty()) {
        postfix += s.top();
        s.pop();
    }

    return postfix;
}

string getLOCRef(const string& operand) {
    if (tempLocMap.find(operand) != tempLocMap.end())
        return "(" + to_string(tempLocMap[operand]) + ")";
    return operand;
}

void generateCodeFromPostfix(const string& postfix) {
    stack<string> st;

    for (char ch : postfix) {
        if (isalnum(ch)) {
            st.push(string(1, ch));
        } else {
            if (ch == '~') {
                string op = st.top(); st.pop();
                string temp = newTemp();
                threeAddressCode.push_back(temp + " = uminus " + op);
                quadruples.push_back({"uminus", op, "", temp});
                triples.push_back({"uminus", getLOCRef(op), ""});
                tempLocMap[temp] = loc++;
                st.push(temp);
            } else {
                string op2 = st.top(); st.pop();
                string op1 = st.top(); st.pop();
                string temp = newTemp();
                string op(1, ch);

                if (op == "=") {
                    threeAddressCode.push_back(op1 + " = " + op2);
                    quadruples.push_back({"=", op2, "", op1});
                    triples.push_back({"=", getLOCRef(op1), getLOCRef(op2)});
                    st.push(op1);
                } else {
                    threeAddressCode.push_back(temp + " = " + op1 + " " + op + " " + op2);
                    quadruples.push_back({op, op1, op2, temp});
                    triples.push_back({op, getLOCRef(op1), getLOCRef(op2)});
                    tempLocMap[temp] = loc++;
                    st.push(temp);
                }
            }
        }
    }
}

void displayOutput() {
    cout << "\n--- Three Address Code ---\n";
    for (const auto& line : threeAddressCode)
        cout << line << endl;

    cout << "\n--- Quadruple Representation ---\n";
    cout << "Loc\tOp\tArg1\tArg2\tResult\n";
    int qloc = 1;
    for (const auto& q : quadruples)
        cout << "(" << qloc++ << ")\t" << q.op << "\t" << q.arg1 << "\t" << q.arg2 << "\t" << q.result << endl;

    cout << "\n--- Triple Representation (using LOCs) ---\n";
    cout << "Loc\tOp\tArg1\tArg2\n";
    for (size_t i = 0; i < triples.size(); ++i)
        cout << "(" << i+1 << ")\t" << triples[i].op << "\t" << triples[i].arg1 << "\t" << triples[i].arg2 << endl;

    cout << "\n--- Indirect Triple Representation ---\n";
    int base = 35;
    for (size_t i = 0; i < triples.size(); ++i)
        cout << base + i << "\t(" << i+1 << ")" << endl;

    cout << "\nLOC\tOp\tArg1\tArg2\n";
    for (size_t i = 0; i < triples.size(); ++i)
        cout << "(" << i+1 << ")\t" << triples[i].op << "\t" << triples[i].arg1 << "\t" << triples[i].arg2 << endl;
}

int main() {
    string infix;
    cout << "Enter infix expression (with assignment): ";
    getline(cin, infix);

    string postfix = infixToPostfix(infix);

    generateCodeFromPostfix(postfix);
    displayOutput();

    return 0;
}