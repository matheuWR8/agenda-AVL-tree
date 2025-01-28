#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <sstream>

using namespace std;

// Compara strings (case insensitive  (eu deveria tornar diacritic insensitive tbm))
int compararStrings(string& str1, string& str2){
    int tamanho = str1.length(); 
    for(int i = 0; i <= tamanho; i++){    // Atravessa os índices da primeira string (incluindo o "seguinte" ao último)
        if (tolower(str1[i]) > tolower(str2[i]))
            return 1;   // A 1a string é maior que a 2a; retorna 1
        else if (tolower(str1[i]) < tolower(str2[i]))
            return -1;  // A 1a string é menor que a 2a; retorna -1
    }
    return 0;   // Todos os caracteres são iguais; retorna 0
}

class Contato{
public:
    string nome;
    string telefone;
    string email;

    Contato(){
    }

    Contato(string nome, string telefone, string email){
        this->nome = nome;
        this->telefone = telefone;
        this->email = email;
    }
};

class Node{
public:
    Contato contato;
    Node* esq;
    Node* dir;
    int altura;

    Node(Contato dados){
        contato = dados;
        esq = nullptr;
        dir = nullptr;
        altura = 1;
    }
};

class ArvoreAVL{
private:
    Node* raiz;

public:
    ArvoreAVL() {
        raiz = nullptr;
    }

    void inserir(Contato value) {
        raiz = insercao(raiz, value);
    }

    void remover(string nome){
        raiz = remocao(raiz, nome);
    }

    Contato* buscar(string nome){
        return &busca(raiz, nome)->contato;
    }

    void leituraEmOrdem(){
        emOrdem(raiz);
    }

    void leituraPosOrdem(){
        posOrdem(raiz);
    }

    void leituraPreOrdem(){
        preOrdem(raiz);
    }

    // Faz a exportação dos contatos para um csv
    int exportarContatos(string endereco, string nome){
        endereco = endereco == "" ? endereco : endereco + "/"; // endereço do diretório; se não for dado, é o diretório do executavel
        nome = nome == "" ? "contatos.csv" : nome + ".csv"; // nome do arquivo a ser criado + extensão .csv; se não for dado, é 'contatos.csv'
        ofstream fout;
        fout.open(endereco + nome); // abrir o filestream no endereço
        if(fout.is_open()){ // foi possível acessar o arquivo
            escreverEmOrdem(raiz, fout); // escrever no arquivo
            fout.close();
            return 0; // sucesso; retornar 0
        } else{ // não foi possível acessar o endereço
            fout.close();
            return -1; // falha; retornar -1
        }
        
    }

    // Faz a importação de um csv para a árvore
    int importarContatos(string endereco){
        endereco = endereco == "" ? "contatos.csv" : endereco; // endereço do arquivo; se não for dado é o contatos.csv do diretorio do exe
        ifstream fin;
        fin.open(endereco); // abre o filestream no endereço
        if (fin.is_open()){ // foi possível acessar o arquivo
            string linha;

            while(getline(fin, linha)){ // a cada linha
                stringstream str(linha); // criar stream da linha
                string nome, telefone, email;
                getline(str,nome,','); // atribuir os dados entre virgulas a variaveis
                getline(str,telefone,',');
                str >> email;
                inserir(Contato(nome,telefone,email)); // inserir na árvore um contato com os dados
            }
            fin.close(); 
            return 0; // retornar 0
        } else{
            fin.close();
            return -1; // retornar -1
        }
    }

private: 

    // Retorna a altura de um node
    int altura(Node* node){
        if (node == nullptr){ // o node não existe
            return 0;
        }
        return node->altura;
    }

    // Retorna a diferença de altura entre o ramo da direita e o da esquerda
    int balanco(Node* node){
        if (node == nullptr)    // o node não existe
            return 0; 
        return altura(node->dir) - altura(node->esq);   // direita - esquerda
    }

    // Rotação para a direita
    Node* rodarDir(Node* y){
        Node *x = y->esq;
        Node *T2 = x->dir;

        x->dir = y;
        y->esq = T2;

        y->altura = 1 + max(altura(y->esq), altura(y->dir));
        x->altura = 1 + max(altura(x->esq), altura(x->dir)); 

        return x;

    }

    // Rotação para a esquerda
    Node* rodarEsq(Node* x){
        Node* y = x->dir;
        Node* temp = y->esq;

        y->esq = x;
        x->dir = temp;
    
        x->altura = 1 + max(altura(x->esq), altura(x->dir)); 
        y->altura = 1 + max(altura(y->esq), altura(y->dir));

        return y;
    }

    // Balanceia a árvore conforme o caso de desequilíbrio
    Node* balanceamento(Node* node){

        int diferenca = balanco(node);  // cálculo da diferença de altura entre os dois ramos (dir-esq)
        int diferencaDir = balanco(node->dir);
        int diferencaEsq = balanco(node->esq);
        
        if (diferenca > 1 && diferencaDir >= 0)     // desbalanceamento entre ramos é para a direita (2)
                                                    // ramo da direita balanceado ou desb. para a direita (0 ou 1) 
            return rodarEsq(node);  // rotação simples para a esquerda                      

        if (diferenca < -1 && diferencaEsq <= 0)    // desbalanceamento entre ramos é para a esquerda (-2)
                                                    // ramo da esquerda balanceado ou desb. para a esquerda (0 ou -1)
            return rodarDir(node);  // rotação simples para a direita

        if (diferenca > 1 && diferencaDir < 0){     // desbalanceamento entre ramos é para a direita (2)
                                                    // desbalanceamento no ramo da direita é para a esquerda (-1)
            node->dir = rodarDir(node->dir);    // rotação para a direita no ramo da direita
            return rodarEsq(node);              // rotação geral para a esquerda
        }
        if (diferenca < -1 && diferencaEsq > 0){    // desbalanceamento entre ramos é para a esquerda (-2)
                                                    // desbalanceamento no ramo da esquerda é para a direita (1)
            node->esq = rodarEsq(node->esq);    // rotação para a esquerda no ramo da esquerda
            return rodarDir(node); // rotação geral para a direita
        }

        return node; // não foi preciso rebalancear
    }

    // Retorna o node mais à esquerda do ramo
    Node* menorNode(Node* node){ 
        Node * atual = node;

        while (atual->esq != nullptr)
            atual = atual ->esq;

        return atual;
    }

    // Inserção de um node
    Node* insercao (Node* node, Contato dados){

        if (node == nullptr)    // o node atual não existe
            return new Node(dados);     // criar novo node com o contato

        if (compararStrings(dados.nome, node->contato.nome) < 0)    // o nome do contato inserido é menor que o do node atual
            node->esq = insercao(node->esq, dados); // recursão para a esquerda
        else if (compararStrings(dados.nome, node->contato.nome) > 0)   // nome maior que do node atual
            node->dir = insercao(node->dir, dados); // recursão para a direita
        else
            return node; // mesmo nome; não adicionar [capaz d'eu mudar isso]

        node->altura = 1 + max(altura(node->esq), altura(node->dir)); // a altura do node atual é a altura do ramo mais alto + sua própria (1)

        return balanceamento(node); // balanceamento após a inserção

    }

    // Remoção de um node
    Node* remocao(Node* node, string nome){

        if (node == nullptr) // o node não existe; não há remoção
            return node;

        if (compararStrings(nome, node->contato.nome) < 0)  // o item buscado está à esquerda do node atual
            node->esq = remocao(node->esq, nome); // recursão para a esquerda
        else if (compararStrings(nome, node->contato.nome) > 0) // o item buscado está à direita do node atual
            node->dir = remocao(node->dir, nome); // recursão para a direita
        else{   // item encontrado; seguir com a remoção
            if (node->esq == nullptr || node->dir == nullptr){ // o node tem um ou nenhum filho
                Node* temp = node->esq ? node->esq : node->dir;
                if (temp == nullptr){ // node não tem filhos
                    temp = node;    // mover endereço do node para o ponteiro temporário
                    node = nullptr; // remover o endereço do node da árvore
                } else 
                    *node = *temp; // mover o conteúdo do filho para o node-pai

                free(temp); // liberar o espaço ocupado pelo temporário
            }else{  // o node tem dois filhos
                Node* temp = menorNode(node->dir); 
                node->contato = temp->contato;  // mover o conteúdo do menor node do ramo da direita para o node atual
                node->dir = remocao(node->dir, temp->contato.nome); // recursão para remover o item movido
            }
        }

        if (node == nullptr) // a árvore só tinha um node, que foi removido
            return node;

        node->altura = 1 + max(altura(node->esq), altura(node->dir)); // cálculo da altura dos nodes

        return balanceamento(node);     // balanceamento

    }

    // Busca pelo nome
    Node* busca(Node* node, string nome){
        if (node == NULL || compararStrings(node->contato.nome,nome) == 0) // item encontrado
            return node;
        
        if (compararStrings(node->contato.nome,nome) < 0) // item está à direita
            return busca(node->dir, nome); // recursão para a direita

        return busca(node->esq, nome); // item está à esquerda; recursão para a esquerda

    }

    // Imprimem os nomes seguindo as tres leituras da árvore
    void emOrdem(Node* node){
        if (node != nullptr){ // se a raíz existe
            emOrdem(node->esq); // recursivamente ir até o menor item, o que está mais à esquerda
            cout << node->contato.nome << endl; // imprimir o conteúdo do pai
            emOrdem(node->dir); // repetir para o ramo irmão da direita
        }
    }

    void preOrdem(Node* node){
        if (node != nullptr){ // se a raíz existe
            cout << node->contato.nome << endl; // imprimir o conteúdo da raiz
            preOrdem(node->esq); // seguir para os ramos a esquerda
            preOrdem(node->dir); // repetir para os irmaos da direita
        }
    }

    void posOrdem(Node* node){
        if (node != nullptr){ // se a raíz existe
            posOrdem(node->esq); // imprimir o filho da esquerda
            posOrdem(node->dir); // imprimir seu irmao da direita
            cout << node->contato.nome << endl; // imprimir o conteúdo do pai
        }
    }

    // Escreve na stream de saída o os dados em csv
    void escreverEmOrdem(Node* node, ofstream& out){
        if (node != nullptr){ // se a raíz existe
            escreverEmOrdem(node->esq, out); // recursivamente ir até o menor item, o que está mais à esquerda
            out << node->contato.nome << ',' << node->contato.telefone << ',' << node->contato.email << endl; // imprimir o conteúdo do pai
            escreverEmOrdem(node->dir, out); // repetir para o ramo irmão da direita
        }
    }
};

int main(){
    ArvoreAVL arvere = ArvoreAVL();
    list<Contato> favoritos;   // lista dos contatos favoritos (copias dos dados nas árvores)
    // [em um caso da exclusão da árvore, um contato é copiado de um node para o outro sem que aquele [node] troque de lugar com este, sendo apenas excuído;
    // em consequencia, dados referenciados por ponteiros podem ser afetados;
    // de inicio eu queria que os favoritos fossem ponteiros dos contatos, mas vi que seria mais seguro esta lista conter cópias dos contatos ao invés disso; 
    // de outro modo, eu deveria reformular o trecho em questão algoritmo de remoção, mas não sei como]

    char opcao;
    while (opcao != '0'){ // voltar ao menu se não optar por sair
        cout << "[MENU PRINCIPAL] Escolha uma opção: " << endl;
        cout << "[1] Inserir contato;" << endl 
            << "[2] Remover contato;" << endl
            << "[3] Buscar contato;" << endl
            << "[4] Listar contatos;" << endl
            << "[5] Favoritos;" << endl
            << "[6] Exportar contatos;" << endl
            << "[7] Importar contatos;" << endl
            << "[0] Sair;" << endl;
        cin >> opcao;
        cin.ignore();
        if (opcao == '1'){ // inserção
            string nome, telefone, email;
            cout << "Digite os dados do contato" << endl;
            cout << "Nome: "; // receber os dados
            getline(cin, nome);
            cout << "Telefone: ";
            getline(cin, telefone);
            cout << "E-mail: ";
            getline(cin, email);

            Contato cont = Contato(nome,telefone,email); // criar o objeto
            arvere.inserir(cont); // inserir o objeto
        } else if (opcao == '2'){ // remoção
            string nome;
            cout << "Nome do contato: ";
            getline(cin, nome); // receber o nome
            for (auto iter = favoritos.begin(); iter != favoritos.end(); ++iter){ // iterar pela lista para remover o favorito correspondente
                if (compararStrings((*iter).nome, nome) == 0){
                    favoritos.erase(iter);
                    break;
                }
            }
            arvere.remover(nome); // apagar o contato com o nome

        } else if (opcao == '3'){ // busca
            string nome;
            cout << "Nome do contato: ";
            getline(cin, nome); // receber o nome
            Contato* cont = arvere.buscar(nome); // buscar pelo nome
            if (cont != NULL) // há item com esse nome, imprimir os dados
                cout << "Nome: " << cont->nome << ";\t Telefone: " << cont->telefone << ";\t E-mail:" << cont->email << ";" << endl;
            else // não há item com esse nome;
                cout << "Não encontrado" << endl;
        } else if (opcao == '4'){ // listagem
            cout << "[LISTAGEM] Escolha o tipo de leitura: " << endl 
                << "[1] Em ordem;" << endl 
                << "[2] Pré-ordem;" << endl 
                << "[3] Pós-ordem;" << endl;
            char opcaoLista;
            cin >> opcaoLista; // escolha das opções de leitura
            if (opcaoLista == '1'){ // leitura em ordem
                cout << "CONTATOS: " << endl;
                arvere.leituraEmOrdem(); 
            }else if (opcaoLista == '2'){ // leitura pre-ordem
                cout << "CONTATOS: " << endl;
                arvere.leituraPreOrdem();
            }else if (opcaoLista == '3'){ // leitura pos-ordem
                cout << "CONTATOS: " << endl;
                arvere.leituraPosOrdem();
            } else
                cout << "Opção inválida!" << endl;
        } else if (opcao == '5'){ // favoritos
            cout << "[FAVORITOS] Escolha uma opção: " << endl 
                << "[1] Adicionar contato favorito;" << endl 
                << "[2] Remover dos favoritos;" << endl 
                << "[3] Buscar favorito;" << endl
                << "[4] Listar favoritos;" << endl;
            char opcaoFavs;
            cin >> opcaoFavs;
            cin.ignore();
            if (opcaoFavs == '1'){ // adição
                string nome;
                cout << "Nome do contato: ";
                getline(cin, nome); // receber o nome
                favoritos.push_front(*arvere.buscar(nome)); // buscar na árvore e incluir na lista
            } else if (opcaoFavs == '2'){ // remoção
                string nome;
                cout << "Nome do contato: ";
                getline(cin, nome); // receber o nome
                for (auto iter = favoritos.begin(); iter != favoritos.end(); ++iter){ // iterar pela lista e removê-lo
                    if (compararStrings((*iter).nome, nome) == 0){
                        favoritos.erase(iter);
                        break;
                    }
                }
            } else if (opcaoFavs == '3'){ // busca
                string nome;
                cout << "Nome do contato: ";
                getline(cin, nome); // receber o nome
                Contato* cont;
                for (auto iter = favoritos.begin(); iter != favoritos.end(); ++iter){ // itera a lista em busca do nome
                    if (compararStrings(iter->nome, nome) == 0){
                        cont = &(*iter);
                        break;
                    }
                }
                if (cont != NULL) // favorito encontrado; imprimir dados
                    cout << "Nome: " << cont->nome << ";\t Telefone: " << cont->telefone << ";\t E-mail:" << cont->email << ";" << endl;
                else // favorito não encontrado
                    cout << "Não encontrado" << endl;
            } else if (opcaoFavs == '4'){ // listagem
                cout << "FAVORITOS" << endl; 
                for (auto iter = favoritos.begin(); iter != favoritos.end(); ++iter){ // itera e imprime os favoritos
                    cout << (*iter).nome << endl;
                }
            } else
                cout << "Opção inválida!" << endl;


        } else if (opcao == '6'){ // exportação
            string endereco, nome;
            cout << "Endereço do diretório (padrão: diretório do programa): ";
            getline(cin, endereco);
            cout << "Nome do arquivo (padrão: 'contatos'): ";
            getline(cin, nome);
            arvere.exportarContatos(endereco, nome);
            if (arvere.exportarContatos(endereco, nome) == 0)
                cout << "Contatos exportados" << endl;
            else 
                cout << "Ocorreu um erro" << endl;
        } else if (opcao == '7'){ // importação
            string endereco;
            cout << "Endereço do arquivo (padrão: './contatos.csv'): ";
            getline(cin, endereco);
            if(arvere.importarContatos(endereco) == 0)
                cout << "Contatos importados" << endl;
            else 
                cout << "Ocorreu um erro" << endl;
        } else if (opcao != '0'){ // sair
            cout << "Digite uma opção válida" << endl;
        }
    }

    cout << "Adeus" << endl;
    return 0;
}