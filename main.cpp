#include "LN.hpp"
#include <vector>
#include <set>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **args) {
    if (argc != 3) {
        cout << "Wrong number of arguments: " << argc << '\n';
        return 2;
    }
    ifstream fin(args[1]);
    if (!fin.is_open()) {
        cout << "Failed to open the file : " << args[1] << '\n';
        return 1;
    }
    vector<LN *> expr;
    string s;
    try {
        while (!fin.eof()) {
            fin >> s;
            switch (s[0]) {
                case '-': {
                    if (s.size() == 1) {
                        LN *b(expr.back());
                        expr.pop_back();
                        LN *c = expr.back();
                        expr.pop_back();
                        LN *d = new LN(*b - *c);
                        expr.push_back(d);
                    } else {
                        LN *a = new LN(s);
                        expr.push_back(a);
                    }
                    break;
                }
                case '+': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d = new LN(*b + *c);
                    expr.push_back(d);
                    break;
                }
                case '/': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d = new LN(*b / *c);
                    expr.push_back(d);
                    break;
                }
                case '*': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d = new LN(*b * *c);
                    expr.push_back(d);
                    break;
                }
                case '>': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d;
                    if (s.size() == 1) {
                        d = new LN(*b > *c);
                    } else {
                        d = new LN(*b >= *c);
                    }
                    expr.push_back(d);
                    break;
                }
                case '=': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d = new LN(*b == *c);
                    expr.push_back(d);
                    break;
                }
                case '<': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d;
                    if (s.size() == 1) {
                        d = new LN(*b < *c);
                    } else {
                        d = new LN(*b <= *c);
                    }
                    expr.push_back(d);
                    break;
                }
                case '!': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *c = expr.back();
                    expr.pop_back();
                    LN *d = new LN(*b != *c);
                    expr.push_back(d);
                    break;
                }
                case '_': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *d = new LN(-*b);
                    expr.push_back(d);
                    break;
                }
                case '~': {
                    LN *b(expr.back());
                    expr.pop_back();
                    LN *d = new LN(~*b);
                    expr.push_back(d);
                    break;
                }
                default: {
                    LN *a = new LN(s);
                    expr.push_back(a);
                    break;
                }
            }
        }
    }
    catch (exception &exp) {
        cout << exp.what() << "\n";
        fin.close();
    }
    fin.close();
    ofstream fout(args[2]);
    if (!fout.is_open()) {
        cout << "Failed to open the file : " << args[2] << '\n';
        fin.close();
        return 1;
    }
    while (!expr.empty()) {
        fout << (string) (*expr.back()) << '\n';
        expr.pop_back();
    }
    fout.close();
    return 0;
}

