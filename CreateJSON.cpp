#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <deque>

using namespace std;

// void bswap(void* const ptr, int bcount) {
//     uint8_t* bytes = new uint8_t[bcount];
//     for(int i = 0; i < bcount; i++) {
//         bytes[bcount-1-i] = *(uint8_t*)(ptr+i);
//     }

//     memcpy(ptr, bytes, bcount);
//     delete [] bytes;
// }

enum ValueType {
    OBJECT,
    LIST,
    STRING,
    BOOLEAN,
    NUMBER,
    NUL
};

struct Node {
    void* ptr;

    template<typename T>
    T& get() {
        return *(T*)ptr;
    }

    ValueType type;
};
class Object {
    map<string, Node*> object;
};

class Index {
public:
    Index(int node) {

    }

    Index(const char* index) {

    }
    
    Index(Node* node) {

    }
private:
    shared_ptr<Node> node;
};

// vector<Index> indexes;
class List {
public:
    Node* operator[](deque<Index> index) {
        return nullptr;
    }
};

#define LIST List()

int main() {
    LIST[{"1", "2", "3"}]

    // [ { {"Nome", 1547}, {"Age", "17"} }, ]

    return 0;
}
