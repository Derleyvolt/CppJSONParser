# CppJSONParser


### Exemplo 1: JSON para C++

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

### Exemplo 2: JSON para C++

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

### Exemplo 3: C++ para JSON

```cpp
int main() {
    auto node = object({ {"Nome", "Derley"}, 
                         {"Moedas", list[{number(1), number(2), number(5)}]}, 
                         {"Informacao", object({ {"Profissao", "Programador"}, 
                                                  {"Idade", 26}, 
                                                  {"Sexo", "Masculino"} })
                        }});

    string JSON = JSONStringify(node);
    cout << JSON << endl;
    return 0;
}
```
> Saída crua: {"Informacao": {"Idade": 26,"Profissao": "Programador","Sexo": "Masculino"},"Moedas": [1,2,5],"Nome": "Derley"}

> Saída formatada

```js
{  
    "Informacao": {  
                 "Idade": 26,
                 "Profissao": "Programador",
                 "Sexo": "Masculino"
    },
    "Moedas": [1,2,5],
    "Nome": "Derley"
}
```

### Exemplo 4: C++ para JSON 

```cpp
int main() {
    auto node = object({ {"Tipo", "Cachorro"}, {"Nome", "Pluto"}, {"Vegetariano", literal(false)},
                         { "Dias da Semana", list[{ "Segunda", "Terca", "Quarta", "Quinta", "Sexta", 
                                                    "Sabado", "Domingo", null }] 
                         }});

    string JSON = JSONStringify(node);
    cout << JSON << endl;
    return 0;
}
```

> Saída crua: {"Dias da Semana": ["Segunda","Terca","Quarta","Quinta","Sexta","Sabado","Domingo",null],"Nome": "Pluto","Tipo": "Cachorro","Vegetariano": false}

> Saída formatada
```js
{
    "Dias da Semana": ["Segunda","Terca","Quarta","Quinta","Sexta","Sabado","Domingo",null],
    "Nome": "Pluto",
    "Tipo": "Cachorro",
    "Vegetariano": false
}
```
