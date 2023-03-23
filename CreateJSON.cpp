#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <array>
#include <string>

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
    void* ptr;

    template<typename T>
    T& get() {
        return *(T*)ptr;
    }

    ValueType type;
};

class Index {
public:
	Index(bool arg) {
		node       = shared_ptr<Node>(new Node());
        node->ptr  = new string(arg ? "true" : "false");
        node->type = ValueType::STRING;
        listArgs.push_back(node);
	}

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
    Index(const char* arg) {
        this->index = arg;
        node        = shared_ptr<Node>(new Node());
        node->ptr   = new string(arg);
        node->type  = ValueType::STRING;
        listArgs.push_back(node);
    }
    
    string operator()() {
        return this->index;
    }

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
	Index(int32_t arg) {
		this->index = to_string(arg);
		node 	    = shared_ptr<Node>(new Node());
		node->ptr   = new int(arg);
		node->type  = ValueType::NUMBER;
		listArgs.push_back(node);
	}

	Index(double arg) {
		node       = shared_ptr<Node>(new Node());
		node->ptr  = new double(arg);
		node->type = ValueType::NUMBER;
		listArgs.push_back(node);
	}

	Index(shared_ptr<Node> node) {
		this->node = node;
		listArgs.push_back(node);
	}

	Index operator, (Index rhs) {
		listArgs.push_back(rhs.listArgs.front());
		return *this;
	}

	vector<shared_ptr<Node>> getIndexes() const {
		return this->listArgs;
	}

	shared_ptr<Node> getNode() const {
		return this->node;
	}

private:
	string 			 		 index;
	shared_ptr<Node> 		 node;
	vector<shared_ptr<Node>> listArgs;
};

class KeyValue {
public:
    KeyValue(string key, Index value) : key(key), index(value) {       
    }

private:
    string key;
    Index index;
};

class Object {
public:
    map<string, Node*> object;

	shared_ptr<Node> operator()(deque<KeyValue> pairs) {
		return {};
	}
};

// vector<Index> indexes;
class List {
public:
    shared_ptr<Node> operator[](Index index) {
        // Aqui a gente vai tirar em index.getIndexes();
    }
};

#define LIST        List()
#define OBJECT      Object()
#define LITERAL(a)  Index(a)
#define NUMBER(a)   Index(a)
#define NUL         Index("null")

int main() {
	OBJECT({
            { "Nome", "Derley"},
            { "Idade", 26},
            { "Elementos aleatorios", LIST[LITERAL(false), NUMBER(5), NUL, 
            OBJECT({
                {"Regiao", "Brasil"}
            })]},
            { "Ano", 2016 }
        }
    );

    return 0;
}
