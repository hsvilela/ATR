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
Position load_maze(const std::string& file_name) {
    std::ifstream labirinto(file_name);
    if (!labirinto) {
        std::cerr << "Erro ao abrir arquivo!" << std::endl;
        std::exit(1); // Aplique a saída com código de erro
    }

    // Leia o número de linhas e colunas do labirinto
    labirinto >> num_rows >> num_cols;

    // Redimensione a matriz 'maze' de acordo
    maze.resize(num_rows);
    for (int i = 0; i < num_rows; i++) {
        maze[i].resize(num_cols); // Cada linha do labirinto será redimensionada
    }

    // Leia o conteúdo do labirinto do arquivo, caractere por caractere
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            labirinto >> maze[i][j];
        }
    }

    // Encontre e retorne a posição inicial ('e')
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (maze[i][j] == 'e') {
                return {i, j};
            }
        }
    }

    labirinto.close();
    return {-1, -1}; // Se não encontrar a posição inicial, retorna inválida
}

// Função para imprimir o labirinto
void print_maze() {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            std::cout << maze[i][j];
        }
        std::cout << '\n'; // Linha nova para cada linha do labirinto
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
        if (maze[row][col] == 'x' || maze[row][col] == 's' || maze[row][col] == 'e') {
            return true; // Caminho válido ou saída ou entrada
        }
    }
    return false;
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    // Marque a posição atual como visitada
    maze[pos.row][pos.col] = '.';
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Atraso para visualização

    // Verifique se a posição atual é a saída
    if (maze[pos.row][pos.col] == 's') {
        return true; // Saída encontrada
    }

    // Verifique as posições adjacentes
    std::vector<Position> adjacents = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };

    for (const auto& adj : adjacents) {
        if (is_valid_position(adj.row, adj.col)) {
            valid_positions.push(adj);
        }
    }

    while (!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();
        if (walk(next_pos)) {
            return true;
        }
    }

    return false; // Se não encontrou a saída
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
    } 
    else {
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
