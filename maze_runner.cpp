#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

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

// Função para carregar o labirinto de um arquivo
// 1. Abra o arquivo especificado por file_name usando std::ifstream
Position load_maze(const std::string& file_name) {
    std::ifstream labirinto(file_name);
    if (!labirinto) {
        std::cerr << "Erro ao abrir arquivo!" << std::endl;
        std::exit(-1);
    }
// 2. Leia o número de linhas e colunas do labirinto
    labirinto >> num_rows >> num_cols;
// 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    maze.resize(num_rows);
    for (int i = 0; i < num_rows; i++) { 
        maze[i].resize(num_cols);
    }
// 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            labirinto >> maze[i][j];
        }
    }
// 5. Encontre e retorne a posição inicial ('e')
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (maze[i][j] == 'e') {
                return {i, j};
            }
        }
    }
 // 7. Feche o arquivo após a leitura
    labirinto.close();
    return {-1, -1};
}

// Função para imprimir o labirinto
void print_maze() {
 // 1. Percorra a matriz 'maze' usando um loop aninhado
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            std::cout << maze[i][j]; // 2. Imprime cada caractere usando std::cout
        }
        std::cout << '\n'; // 3. Adiciona uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    bool posicao;
    posicao = false;
 // 1. Verifique se a posição está dentro dos limites do labirinto
    if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
        posicao = true; // 3. posição é válida
    }
    else {
        return posicao; // false caso contrário
    }
    if (maze[row][col] == 'x' || maze[row][col] == 's' || maze[row][col] == 'e') {
        return posicao;
    }
    return false;
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    if (maze[pos.row][pos.col] == 's') { // verifica se a posição atual é a saída
        return true;
    }

    maze[pos.row][pos.col] = '.'; // labirinto sendo percorrido
    print_maze(); // mostra atual estado do labirinto
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::vector<Position> adjacentes = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };
    for(auto i:adjacentes){
        if(is_valid_position(pos.row+i.row, pos.col+i.col)){
            valid_positions.push({pos.row+i.row, pos.col+i.col});
        }
    };

    while(!valid_positions.empty()){
        Position proxima = valid_positions.top();
        valid_positions.pop();
        if(walk(proxima)){
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
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