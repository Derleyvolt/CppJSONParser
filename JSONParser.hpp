#include <iostream>
#include <vector>
#include <map>
#include <string.h>
#include <string>
#include <algorithm>
#include <stack>
#include <fstream>
#include <deque>
#include <memory>

// JSON RFC --> https://www.rfc-editor.org/rfc/rfc8259
// O coração do parser é tranformar o JSON em uma árvore N-ária e a partir dai ir extraindo os dados..

using namespace std;

enum ValueType {
    OBJECT,
    LIST,
    STRING,
    BOOLEAN,
    NUMBER,
    NUL
};

struct Node {
    void* ptr = nullptr;

    template<typename T>
    T& get() {
        return *(T*)ptr;
    }

    ~Node();

    ValueType type;
};

class Index {
public:
	Index(bool arg);

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
    Index(const char* arg);
    
    string operator()();

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
	Index(int32_t arg, int nullFlag = 0);

	Index(double arg);

	Index(shared_ptr<Node> node);

	Index operator, (Index rhs);

	shared_ptr<Node> getNode() const;
private:
	string 			 		 index;
	shared_ptr<Node> 		 node;
};

void parserObject(string& JSON, map<string, shared_ptr<Node>>& object);
void parserList(string& JSON, vector<shared_ptr<Node>>& list);

class List {
public:
    vector<shared_ptr<Node>> array; 

    Node* operator[](int32_t index);

    shared_ptr<Node> operator[](deque<Index> Index);
};

class KeyValue {
public:
    KeyValue(string key, Index value);

    string getKey() const;

    Index getVal() const;

private:
    string key;
    Index index;
};

class Object {
public:
    map<string, shared_ptr<Node>> obj;

    Node* operator[](string index);

	shared_ptr<Node> operator()(deque<KeyValue> pairs);
};

class pathJSON {
private:
	class pathIndex {
	public:
		pathIndex(const char* index) {
			this->index = index;
		}

		pathIndex(int index) {
			this->index = to_string(index);
		}

		string getIndex() {
        	return this->index;
    	}
	private:
		string index;
	};

	deque<pathIndex> pathTree;
public:
	pathJSON(deque<pathIndex> pathTree);

	string getIndex();

	bool isEmpty();
};

string getValue(pathJSON pathTree, Node* node);

// Way Down We Go.

Node* getNode(pathJSON pathTree, Node* node);

// partido da assumição de que o JSON é sempre válido
ValueType getTypeFromJSON(string& JSON);

shared_ptr<Node> JSONParse(string& JSON);

// https://www.rfc-editor.org/rfc/rfc8259#page-5
// JSON Lista de simbolos que podem ser ignorados
bool skipSymbols(char c);

void erasePrefixSeparators(string& JSON);

// espera apenas espaços
string getKeyFromJSON(string& JSON);

// espera apenas espaços ou vírgulas precedendo o value
string getValueFromJSON(string& JSON);

#define path        pathJSON
#define list        List()
#define object      Object()
#define literal(a)  Index(a)
#define number(a)   Index(a)
#define null        Index(0, 1)

string JSONStringify(shared_ptr<Node> node);
