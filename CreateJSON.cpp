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
    map<string, shared_ptr<Node>> obj;

	shared_ptr<Node> operator()(deque<KeyValue> pairs) {
		return {};
	}
};

// vector<Index> indexes;
class List {
public:
    vector<shared_ptr<Node>> array; 

    shared_ptr<Node> operator[](Index index) {
        
    }
};

#define list        List()
#define object      Object()
#define literal(a)  Index(a)
#define number(a)   Index(a)
#define null        Index("null")

string objectStrigify(shared_ptr<Node> node) {
    string res;
    res.push_back('{');

    auto map = node->get<Object>().obj;

    for(auto [key, value] : map) {
        res.append('"' + key + "\": ");
        res.append(parseToJSON(value));
        res.push_back(',');
    }

    if(res.back() == ',') {
        res.pop_back();
    }

    return res + '}';
}

string listStringify(shared_ptr<Node> node) {
    string res;

    res.push_back('[');

    auto arr = node->get<List>().array;

    for(auto e : arr) {
        res.append(parseToJSON(e));
        res.push_back(',');
    }

    if(res.back() == ',') {
        res.pop_back();
    }

    return res + ']';
}

string parseToJSON(shared_ptr<Node> node) {
    if(node->type == ValueType::OBJECT) {
        return objectStrigify(node);
    }

    if(node->type == ValueType::LIST) {
        return listStringify(node);
    }

    if(node->type == ValueType::STRING) {
        return "\"" + node->get<string>() + "\"";
    }

    if(node->type == ValueType::NUMBER) {
        return node->get<string>();
    }

    if(node->type == ValueType::BOOLEAN) {
        return node->get<string>();
    }

    if(node->type == ValueType::NUL) {
        return node->get<string>();
    }
}

int main() {
	object({
            { "Nome", "Derley" },
            { "Idade", 26 },
            { "Elementos aleatorios", list[literal(false), number(5), null, 
            object({
                {"Regiao", "Brasil"}
            })] },
            { "Ano", 2016 }
        }
    );

    return 0;
}
