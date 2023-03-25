# CppJSONParser


### Exemplo 1, JSON para C++

```cpp
int main() {
    string jsonExample = "{\"Age\": 34,\"Name\": \"Jonh\",\"Pets\": [{\"Age\": 3.4,\"Name\": \"MooMoo\",\"Type\": \"Cat\"},{\"Age\": 7,\"Name\": \"Sandy\",\"Type\": \"Squirrel\"},[\"1\",{\"Idade\": 15,\"Nome\": \"Ratinho Azulado\"},\"3\",\"4\"]],\"StateOfOrigin\": \"England\"}";

    // jsonTree é a árvore gerada na etapa de parser
    shared_ptr<Node> jsonTree = JSONParse(jsonExample);


    // A árvore é utilizada para navegação do JSON, nesse caso
    // descendo até um determinado nó/sub-árvore
    auto jsonSubTree = getNode(path({"Pets", 1, "Name"}), jsonTree.get());

    
    // A função getValue recebe um caminho como entrada e uma árvore.
    // A função espera sempre que o caminho leve a um nó folha
    // a fim de retornar seu valor como string.
    cout << getValue(path({}), jsonSubTree) << endl;
    return 0;
}
```

> Saída: Sandy

### Exemplo 2, JSON para C++

```cpp
int main() {
    string jsonExample = "{\"Age\": 34,\"Name\": \"Jonh\",\"Pets\": [{\"Age\": 3.4,\"Name\": \"MooMoo\",\"Type\": \"Cat\"},{\"Age\": 7,\"Name\": \"Sandy\",\"Type\": \"Squirrel\"},[\"1\",{\"Idade\": 15,\"Nome\": \"Ratinho Azulado\"},\"3\",\"4\"]],\"StateOfOrigin\": \"England\"}";

    // jsonTree é a árvore gerada na etapa de parser
    shared_ptr<Node> jsonTree = JSONParse(jsonExample);


    // Podemos passar o caminho inteiro para a função getValue
    // sem a necessidade de salvar um nó intermediário.
    string val = getValue(path({"Pets", 1, "Name"}), jsonTree.get());
    
    cout << val << endl;
  
    // Nós intermediários são úteis quando possuem muitos caminhos, pois poderam levar a diferentes folhas posteriormente
    return 0;
}
```

> Saída: Sandy

### Exemplo 3, C++ para JSON

