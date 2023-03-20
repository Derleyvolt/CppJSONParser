#include <iostream>
#include <vector>
#include <map>
#include <string.h>
#include <string>
#include <algorithm>
#include <stack>
#include <fstream>
#include <deque>

// JSON RFC --> https://www.rfc-editor.org/rfc/rfc8259
// O coração do conversor é tranformar o JSON em uma árvore N-ária e a partir dai ir extraindo os dados..

using namespace std;

struct Node;
void   parserObject(string& JSON, map<string, Node>& object);
void   parserList(string& JSON, vector<Node>& list);

struct List {
    vector<Node> list;
};

struct Object {
    map<string, Node> object;
};

enum ValueType {
    OBJECT,
    LIST,
    STRING,
    NUMBER,
    BOOLEAN,
    NUL
};

class TreeJSON {
public:
    Node node;

    TreeJSON(Node node) : node(node) {

    }

    template<class T>
    T getValue(deque<string> indexes, Node* node = node) {
        if(!index.empty()) {
            string key = index.front();

            if(node->type == ValueType::OBJECT) {
                node = node->data.object->object[index.front()];
                indexes.pop_front();
                return <T>getValue(index, node);
            } else if(node->type == ValueType::LIST) {
                int index = stoi(indexes.front());
                node = node->data.list->list[index];
                indexes.pop_front();
                return <T>getValue(indexes, node);
            } else if(node->type == ValueType::NUMBER) {
                return node->data.number;
            } else if(node->type == ValueType::BOOLEAN) {   
                return node->data.boolean;
            } else if(node->type == ValueType::STRING) {
                return *node->data.str;
            } else {
                return null;
            }
        }
    }
};

struct Node {
    // union {
    //     Object*    object;
    //     List*      list;
    //     string*    str;
    //     bool       boolean;
    //     float      number;
    // } data;

    void* ptr;

    ValueType type;
};

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

Node parseJSON(string& JSON) {
    // Node* node = new Node();
    Node node;

    if(getTypeFromJSON(JSON) == ValueType::OBJECT) {
        node.ptr  = new Object();
        node.type = ValueType::OBJECT;  
        parserObject(JSON, ((Object*)node.ptr)->object);
        return node;
    } 

    if(getTypeFromJSON(JSON) == ValueType::LIST) {
        node.ptr = new List();
        parserList(JSON, ((List*)node.ptr)->list);
        return node;
    }

    if(getTypeFromJSON(JSON) == ValueType::STRING) {
        node.ptr   = new string(JSON.substr(1, JSON.size()-2));
        node.type  = ValueType::STRING;
        return node;
    } 

    if(getTypeFromJSON(JSON) == ValueType::NUMBER) {
        node.ptr  = new int(stof(JSON));
        //node->data.number = std::stof(JSON);
        node.type = ValueType::NUMBER;
        return node;
    }

    if(getTypeFromJSON(JSON) == ValueType::BOOLEAN) {
        node.ptr  = new bool(JSON == "false" ? 0 : 1);
        node.type = ValueType::BOOLEAN;
        return node;
    }

    node.type = ValueType::NUL;
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

void parserObject(string& JSON, map<string, Node>& object) {
    int JSONLen = JSON.size();

    // apaga as chaves do objeto
    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string key   = getKeyFromJSON(JSON);
        string value = getValueFromJSON(JSON);
        object[key]  = parseJSON(value);
        erasePrefixSeparators(JSON);
    }
}

void parserList(string& JSON, vector<Node>& list) {
    int JSONLen = JSON.size();

    JSON = JSON.substr(1, JSONLen-2);

    while(!JSON.empty()) {
        string value = getValueFromJSON(JSON);
        list.push_back(parseJSON(value));
        erasePrefixSeparators(JSON);
    }
}

string loadJSON() {
    fstream fs("in", std::ifstream::in);
    string temp, s;

    while(!fs.eof()) {
        getline(fs, temp);
        s.append(temp);
    }

    return s;
}

int main() {
    string s   = loadJSON();
    // Node* node = parseJSON(s);

    TreeJSON JSON(parseJSON(s));

    cout << (*(int*)JSON.node.ptr) << endl;

    // cout << node->data.object->object["Pets"]->data.list->list[0]->data.object->object["Age"]->data.number << endl;
    return 0;
}
