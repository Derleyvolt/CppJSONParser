#include <iostream>
#include <vector>
#include <map>
#include <string.h>

using namespace std;

struct CJSON;

struct JSONList {
    vector<CJSON*> list;
};

struct CJSON {
    enum Type { JSON, JSONLIST, STR, BOOLEAN, REAL };

    union {
        CJSON*    json;
        JSONList* jsonList;
        string*   str;
        bool      boolean;
        float     real;
    } data;

    Type type;
};

union Teste {
    int32_t a;
    int16_t b[2];
    uint8_t c;
};

// 3F3F3F3F

map<string, CJSON*> JSON;

void printBits(uint32_t n, int countBits) {
    if(countBits <= 0) {
        return;
    }

    printBits(n>>1, countBits-1);
    printf("%d", (n&1));
}

int main() {
    Teste t;

    //t.a    = 0x3F4F5F6F; // Little Indian
    //int x = 0xFFFFFFFF;
    // memcpy(&t, &x, 4);

    t.b[0] = 0xFFFF;
    t.b[1] = 0xFFFF;

    printBits(*(int32_t*)&t, 32);

    printf("\n");

    // cout << hex << t.a << endl;
    cout << hex << t.a << endl;
    return 0;
}
