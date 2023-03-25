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
// O coração do conversor é tranformar o JSON em uma árvore N-ária e a partir dai ir extraindo os dados..

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
	Index(bool arg) {
		node       = shared_ptr<Node>(new Node());
        node->ptr  = new string(arg ? "true" : "false");
        node->type = ValueType::BOOLEAN;
	}

    // Essa versão do overload é também utilizada pra setar um índice que
    // é usado na etapa de conversão JSON -> OBJECT
    Index(const char* arg) {
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
		return *this;
	}

	shared_ptr<Node> getNode() const {
		return this->node;
	}

private:
	string 			 		 index;
	shared_ptr<Node> 		 node;
};

void parserObject(string& JSON, map<string, shared_ptr<Node>>& object);
void parserList(string& JSON, vector<shared_ptr<Node>>& list);

class List {
public:
    vector<shared_ptr<Node>> array; 

    Node* operator[](int32_t index) {
        return array[index].get();
    }

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

    Node* operator[](string index) {
        return this->obj[index].get();
    }

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

// evitando memory leak
Node::~Node() {
    if(ptr != nullptr) {
        if(type == ValueType::OBJECT) {
            delete (Object*)ptr;
        } else if(type == ValueType::LIST) {
            delete (List*)ptr;
        } else {
            delete (string*)ptr;
        }
    }
}

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

	deque<pathIndex> path;
public:
	pathJSON(deque<pathIndex> path) : path(path) {
	}

	string getIndex() {
		string val = this->path.front().getIndex(); 
		this->path.pop_front();
		return val;
	}

	bool isEmpty() {
		return this->path.empty();
	}
};

string getValue(pathJSON path, Node* node) {
    if(!path.isEmpty()) {  
        if(node->type == ValueType::OBJECT) {
            return getValue(path, node->get<Object>()[path.getIndex()]);
        } else {
            return getValue(path, node->get<List>()[stoi(path.getIndex())]);
        } 
    }

    return node->get<string>();
}

// Way Down We Go.

Node* getNode(pathJSON path, Node* node) {
    if(!path.isEmpty()) {
        if(node->type == ValueType::OBJECT) {
            return getNode(path, node->get<Object>()[path.getIndex()]);
        } else {
            return getNode(path, node->get<List>()[stoi(path.getIndex())]);
        } 
    }

    return node;
}

// partido da assumição de que o JSON é sempre válido
ValueType getTypeFromJSON(string& JSON) {
    if(JSON.front() == '[') {
        return ValueType::LIST;
    } 

    if(JSON.front() == '{') {
        return ValueType::OBJECT;
    } 

    if(JSON.front() == '"') {
        return ValueType::STRING;
    } 

    if(JSON == "false" || JSON == "true") {
        return ValueType::BOOLEAN;
    } 

    if(JSON == "null") {
        return ValueType::NUL;
    }

    return ValueType::NUMBER;
}

shared_ptr<Node> JSONParse(string& JSON) {
    shared_ptr<Node> node(new Node());

    if(getTypeFromJSON(JSON) == ValueType::OBJECT) {
        node->ptr  = new Object();
        node->type = ValueType::OBJECT;  
        parserObject(JSON, node->get<Object>().obj);
        return node;
    } 

    if(getTypeFromJSON(JSON) == ValueType::LIST) {
        node->ptr  = new List();
        node->type = ValueType::LIST;
        parserList(JSON, node->get<List>().array);
        return node;
    }

    if(getTypeFromJSON(JSON) == ValueType::STRING) {
        node->ptr  = new string(JSON.substr(1, JSON.size()-2));
        node->type = ValueType::STRING;
        return node;
    } 

    if(getTypeFromJSON(JSON) == ValueType::NUMBER) {
        node->ptr  = new string(JSON);
        node->type = ValueType::NUMBER;
        return node;
    }

    if(getTypeFromJSON(JSON) == ValueType::BOOLEAN) {
        node->ptr  = new string(JSON);
        node->type = ValueType::BOOLEAN;
        return node;
    }

    node->ptr  = new string(JSON);
    node->type = ValueType::NUL;

    return node;
}

// https://www.rfc-editor.org/rfc/rfc8259#page-5
// JSON Lista de simbolos que podem ser ignorados
bool skipSymbols(char c) {
    return c == ',' ? 1 : c == ' ' ? 1 : c == ':' ? 1 : c == 0xA ? 1 : c == 0x9 ? 1 : c == 0xD ? 1 : 0;
}

void erasePrefixSeparators(string& JSON) {
    for(int i = 0; i < JSON.size(); i++) {
        if(!skipSymbols(JSON[i])) {
            JSON = JSON.substr(i);
            return;
        }
    }

    JSON.clear();
}   

// espera apenas espaços
string getKeyFromJSON(string& JSON) {
    erasePrefixSeparators(JSON);

    string value = JSON.substr(1, JSON.find(':')-2);
    JSON         = JSON.substr(JSON.find(':')+1);
    return value;
}

// espera apenas espaços ou vírgulas precedendo o value
string getValueFromJSON(string& JSON) {
    stack<char> aux;
    string value;

    // skip vírgula e blank spaces
    erasePrefixSeparators(JSON);

    char search = JSON.front();
    value.push_back(search);

    map<char, char> mapper;

    mapper['['] = ']';
    mapper['{'] = '}';
    mapper['"'] = '"';

    // Essa branch trata os casos onde temos uma lista, um objeto ou uma string

    // A ideia é só generalizar o problema do 'valid parentheses problem'.
    if(search == '[' || search == '{' || search == '"') {
        aux.push(search);

        for(int i = 1; i < JSON.size(); i++) {
            char e = JSON[i];
            // se eu encontro um simbolo que case com o topo da stack
            // eu tiro o par da stack (sempre terá um par na stack visto que o JSON recebido é sempre válido [O front-end me garante que é válido]).
            if(mapper[search] == e) {
                aux.pop();
                // se a stack estiver vazio significa que chegamos no fim da lista/objeto/string
                if(aux.empty()) {
                    value.push_back(e);
                    JSON = JSON.substr(i+1);
                    return value;
                }
            }

            value.push_back(e);

            if(e == search) {
                aux.push(search);
            }
        }

        return "";
    } else {
        int i;
        // Nessa branch temos um Number ou um Literal
        for(i = 1; i < JSON.size(); i++) {
            char e = JSON[i];
            if(e != '[' && e != '{' && e != ',') {
                value.push_back(e);
            } else {
                JSON = JSON.substr(i+!!(e == ','));
                return value;
            }
        }

        // se chego aqui significa que a string acabou
        JSON.clear();
        return value;
    }
}

void parserObject(string& JSON, map<string, shared_ptr<Node>>& object) {
    int JSONLen = JSON.size();

    // apaga as chaves do objeto
    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string key   = getKeyFromJSON(JSON);
        string value = getValueFromJSON(JSON);
        object[key]  = JSONParse(value);
        erasePrefixSeparators(JSON);
    }
}

void parserList(string& JSON, vector<shared_ptr<Node>>& list) {
    int JSONLen = JSON.size();

    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string value = getValueFromJSON(JSON);
        list.push_back(JSONParse(value));
        erasePrefixSeparators(JSON);
    }
}

#define path        pathJSON
#define list        List()
#define object      Object()
#define literal(a)  Index(a)
#define number(a)   Index(a)
#define null        Index(0, 1)

string JSONStringify(shared_ptr<Node> node);

string objectStrigify(shared_ptr<Node> node) {
    string res;
    res.push_back('{');

    auto map = node->get<Object>().obj;

    for(auto e : map) {
        res.append('"' + e.first + "\": ");
        res.append(JSONStringify(e.second));
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
        res.append(JSONStringify(e));
        res.push_back(',');
    }

    if(res.back() == ',') {
        res.pop_back();
    }

    return res + ']';
}

string JSONStringify(shared_ptr<Node> node) {
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

int main() {
    return 0;
}
