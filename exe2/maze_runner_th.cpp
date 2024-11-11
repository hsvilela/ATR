#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

 // Variável compartilhada entre as threads
// Representação do labirinto
using Maze = std::vector<std::vector<char>>;
// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;
bool exit_found = false;
mutex m;
// Função para carregar o labirinto de um arquivo 
Position load_maze(const std::string& file_name) {
    // TODO: Implem\ente esta função seguindo estes passos:
    // 1. Abra o arquivo especificado por file_name usando std::ifstream
    // 2. Leia o número de linhas e colunas do labirinto
    // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    // 5. Encontre e retorne a posição inicial ('e')
    // 6. Trate possíveis erros (arquivo não encontrado, formato inválido, etc.)
    // 7. Feche o arquivo após a leitura
    ifstream inFile(file_name); // 1
    if(!inFile){
        cout << "Erro ao abrir o arquivo do labirinto" << endl;
        exit(1);
    } 
    inFile >> num_rows >> num_cols; //2
    maze.resize(num_rows, vector<char>(num_cols));//3
    Position p_inicial = {-1, -1};
    for(int i = 0; i < num_rows; i++){ //4
        for(int j = 0; j < num_cols; j++){
            inFile >> maze[i][j];
            if(maze[i][j] == 'e'){ //5
                p_inicial.row = i; 
                p_inicial.col = j; 
            }
        }
    }
    if(p_inicial.row == -1 || p_inicial.col == -1){ //6
        cout << "Erro: posição incial não encontrada." << endl;
        exit(1);
    }
    inFile.close(); // 7
    return p_inicial;// Placeholder - substitua pelo valor correto
}

// Função para imprimir o labirinto
void print_maze() {
    // TODO: Implemente esta função
    // 1. Percorra a matriz 'maze' usando um loop aninhado
    // 2. Imprima cada caractere usando std::cout
    // 3. Adicione uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto
    //system("clear"); 
    cout << '\n'; 

    for(int i = 0; i < num_rows; i++){ //1
        for(int j = 0; j < num_cols; j++){
            cout << maze[i][j]; //2
        }
        cout << '\n'; //3
    }

}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    // TODO: Implemente esta função
    // 1. Verifique se a posição está dentro dos limites do labirinto
    //    (row >= 0 && row < num_rows && col >= 0 && col < num_cols)
    // 2. Verifique se a posição é um caminho válido (maze[row][col] == 'x')
    // 3. Retorne true se ambas as condições forem verdadeiras, false caso contrário

    if((row >= 0) && (row < num_rows) && (col >= 0) && (col < num_cols)){ //1
        if((maze[row][col] == 'x')||(maze[row][col] == 's')){ //2
            return true; //3
        } 
        return false; // Placeholder - substitua pela lógica correta
    }
    return false;  
}

 // Função principal para navegar pelo labirinto
bool walk(Position pos){
    // TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    // 2. Chame print_maze() para mostrar o estado atual do labirinto
    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false
    valid_positions.push(pos); //iniciando a pilha de posições válidas com a posição inicial.
    while(!valid_positions.empty() && !exit_found){ //Exploração das posições validas.
        Position prox_pos = valid_positions.top();
        valid_positions.pop();
    
        if(maze[pos.row][pos.col] == 's'){ //4
            exit_found = true;
            return true;
        } 
        maze[pos.row][pos.col] = '.'; //1
        m.lock();
        print_maze(); //2
        this_thread::sleep_for(chrono::milliseconds(50));
        m.unlock();
        vector<Position> pos_adj; //vetor com as posições adjascentes disponiveis.
        Position pos_acima = {pos.row - 1, pos.col};
        Position pos_abaixo = {pos.row + 1, pos.col};
        Position pos_esquerda = {pos.row, pos.col - 1};
        Position pos_direita = {pos.row, pos.col + 1};
        if(is_valid_position(pos_acima.row, pos_acima.col)){
            pos_adj.push_back(pos_acima);
        }
        if(is_valid_position(pos_abaixo.row, pos_abaixo.col)){
            pos_adj.push_back(pos_abaixo);
        }
        if(is_valid_position(pos_esquerda.row, pos_esquerda.col)){
            pos_adj.push_back(pos_esquerda);
        }
        if(is_valid_position(pos_direita.row, pos_direita.col)){
            pos_adj.push_back(pos_direita);
        }
        vector<std::thread> threads; //vetor para armazenar todas as threads
        for (auto& i : pos_adj) {
            threads.push_back(std::thread([i]() {
                walk(i); // Cada thread chama `walk` para explorar seu caminho
            }));
        }
        for (auto& i : threads) {
            if (i.joinable()) {
                i.join();
            }
        }
    }
    return false; //7
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << endl;
        return 1;
    }
    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        cerr << "Posição inicial não encontrada no labirinto." << endl;
        return 1;
    }

    thread explorar(walk, initial_pos); //Trhead para iniciar a exploração.
    explorar.join();
    if (exit_found) {
        cout << "Saída encontrada!" << endl;
    } else {
        cout << "Não foi possível encontrar a saída." << endl;
    }

    return 0;
}

// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.
