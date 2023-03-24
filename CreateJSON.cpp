#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <array>
#include <string>
#include <fstream>

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
        node->type = ValueType::BOOLEAN;
	}

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
    Index(const char* arg) {
        cout << "teste" << endl;
        this->index = arg;
        node        = shared_ptr<Node>(new Node());
        node->ptr   = new string(arg);
        node->type  = ValueType::STRING;
    }
    
    string operator()() {
        return this->index;
    }

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
	Index(int32_t arg, int nullFlag = 0) {
		this->index = to_string(arg);
		node 	    = shared_ptr<Node>(new Node());
        if(nullFlag) {
		    node->ptr   = new string("null");
		    node->type  = ValueType::NUL;
        } else {
            node->ptr   = new string(to_string(arg));
		    node->type  = ValueType::NUMBER;
        }
	}

	Index(double arg) {
		node       = shared_ptr<Node>(new Node());
		node->ptr  = new string(to_string(arg));
		node->type = ValueType::NUMBER;
	}

	Index(shared_ptr<Node> node) {
		this->node = node;
	}

	Index operator, (Index rhs) {
        cout << "teste" << endl;
		return *this;
	}

	shared_ptr<Node> getNode() const {
		return this->node;
	}

private:
	string 			 		 index;
	shared_ptr<Node> 		 node;
};

class KeyValue {
public:
    KeyValue(string key, Index value) : key(key), index(value) {

    }

    string getKey() const {
        return key;
    }

    Index getVal() const {
        return index;
    }

private:
    string key;
    Index index;
};

class Object {
public:
    map<string, shared_ptr<Node>> obj;

	shared_ptr<Node> operator()(deque<KeyValue> pairs) {
        shared_ptr<Node> node(new Node());

        node->type = ValueType::OBJECT;

        node->ptr = new Object();

        for(auto e : pairs) {
            string key      = e.getKey();
            auto   auxNode  = e.getVal().getNode();
            node->get<Object>().obj[key] = auxNode;
        }

        return node;
	}
};

// vector<Index> indexes;
class List {
public:
    vector<shared_ptr<Node>> array; 

    shared_ptr<Node> operator[](deque<Index> Index) {
        shared_ptr<Node> node(new Node());
        node->ptr  = new List();
        node->type = ValueType::LIST;
        for(auto e : Index) {
            node->get<List>().array.push_back(e.getNode());
        }
        return node;
    }
};

#define list        List()
#define object      Object()
#define literal(a)  Index(a)
#define number(a)   Index(a)
#define null        Index(0, 1)

string parseToJSON(shared_ptr<Node> node);

string objectStrigify(shared_ptr<Node> node) {
    string res;
    res.push_back('{');

    auto map = node->get<Object>().obj;

    for(auto e : map) {
        res.append('"' + e.first + "\": ");
        res.append(parseToJSON(e.second));
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

    return "";
}

void saveJSON(string JSON) {
    fstream fs("out", std::ifstream::out);
    string temp, s;

    fs << JSON;

    fs.close();
}

int main() {
	auto node = object( { {"Nome", "Jonh" }, { "Age", 34 }, { "StateOfOrigin", "England" },
    {"Pets", list[{object({ {"Type", "Cat"}, {"Name", "MooMoo"}, {"Age", number(3.4)}  }), 
                  object({ {"Type", "Squirrel"}, {"Name", "Sandy"}, {"Age", number(7)}  }),
                  list[{"1", object({ {"Nome", "Ratinho Azulado"}, {"Idade", 15}  }), "3", "4"}]}]}
    } );

    string s = parseToJSON(node);

    cout << s << endl;

    saveJSON(s);
    return 0;
}
