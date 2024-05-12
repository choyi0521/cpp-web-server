#include <string>
#include <regex>
#include <iostream>
using namespace std;

int main() {
    string var = "/blog/";

    const regex r("/blog/(.*)");
    smatch sm;

    if (regex_search(var, sm, r)) {
        for (int i = 1; i < sm.size(); i++) {
            cout << sm.str(i) << endl;
        }
    }
    return 0;
}