#include "A.hpp"

using namespace std;

Index::Index(bool arg) {
    node       = shared_ptr<Node>(new Node());
    node->ptr  = new string(arg ? "true" : "false");
    node->type = ValueType::BOOLEAN;
}

Index::Index(const char* arg) {
    this->index = arg;
    node        = shared_ptr<Node>(new Node());
    node->ptr   = new string(arg);
    node->type  = ValueType::STRING;
}

string Index::operator()() {
    return this->index;
}

Index::Index(int32_t arg, int nullFlag) {
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

Index::Index(double arg) {
    node       = shared_ptr<Node>(new Node());
    node->ptr  = new string(to_string(arg));
    node->type = ValueType::NUMBER;
}

Index::Index(shared_ptr<Node> node) {
    this->node = node;
}

Index Index::operator, (Index rhs) {
    return *this;
}

shared_ptr<Node> Index::getNode() const {
    return this->node;
}

Node* List::operator[](int32_t index) {
    return array[index].get();
}

shared_ptr<Node> List::operator[](deque<Index> Index) {
    shared_ptr<Node> node(new Node());
    node->ptr  = new List();
    node->type = ValueType::LIST;
    for(auto e : Index) {
        node->get<List>().array.push_back(e.getNode());
    }

    return node;
}

KeyValue::KeyValue(string key, Index value) : key(key), index(value) {
}

string KeyValue::getKey() const {
    return key;
}

Index KeyValue::getVal() const {
    return index;
}

Node* Object::operator[](string index) {
    return this->obj[index].get();
}

shared_ptr<Node> Object::operator()(deque<KeyValue> pairs) {
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

pathJSON::pathJSON(deque<pathIndex> path) : pathTree(path) {
}

string pathJSON::getIndex() {
    string val = this->pathTree.front().getIndex(); 
    this->pathTree.pop_front();
    return val;
}

bool pathJSON::isEmpty() {
    return this->pathTree.empty();
}

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

string getKeyFromJSON(string& JSON) {
    erasePrefixSeparators(JSON);

    string value = JSON.substr(1, JSON.find(':')-2);
    JSON         = JSON.substr(JSON.find(':')+1);
    return value;
}

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

void parserObject(string& JSON, map<string, shared_ptr<Node>>& obj) {
    int JSONLen = JSON.size();

    // apaga as chaves do objeto
    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string key   = getKeyFromJSON(JSON);
        string value = getValueFromJSON(JSON);
        obj[key]     = JSONParse(value);
        erasePrefixSeparators(JSON);
    }
}

void parserList(string& JSON, vector<shared_ptr<Node>>& ls) {
    int JSONLen = JSON.size();

    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string value = getValueFromJSON(JSON);
        ls.push_back(JSONParse(value));
        erasePrefixSeparators(JSON);
    }
}

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
