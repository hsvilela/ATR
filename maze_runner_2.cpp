#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>

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
std::mutex maze_mutex;  // Mutex para proteger o acesso ao labirinto compartilhado
bool exit_found = false; // Variável global para marcar se a saída foi encontrada

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    std::ifstream labirinto(file_name);
    if (!labirinto) {
        std::cerr << "Erro ao abrir arquivo!" << std::endl;
        std::exit(-1);
    }

    labirinto >> num_rows >> num_cols;
    maze.resize(num_rows);
    for (int i = 0; i < num_rows; i++) {
        maze[i].resize(num_cols);
    }

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            labirinto >> maze[i][j];
        }
    }

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (maze[i][j] == 'e') {
                return {i, j};
            }
        }
    }

    labirinto.close();
    return {-1, -1};
}

// Função para imprimir o labirinto
void print_maze() {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            std::cout << maze[i][j];
        }
        std::cout << '\n';
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
        if (maze[row][col] == 'x' || maze[row][col] == 's' || maze[row][col] == 'e') {
            return true;
        }
    }
    return false;
}

// Função principal para navegar pelo labirinto usando threads
void walk(Position pos) {
    {
        std::lock_guard<std::mutex> lock(maze_mutex);
        if (exit_found) return;  // Se a saída já foi encontrada por outra thread, encerra
        if (maze[pos.row][pos.col] == 's') { // Verifica se é a saída
            exit_found = true;
            return;
        }

        maze[pos.row][pos.col] = '.'; // Marca o labirinto como visitado
        print_maze();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Movimentos possíveis
    std::vector<Position> adjacentes = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };

    std::vector<std::thread> threads;

    for (auto& next : adjacentes) {
        if (is_valid_position(next.row, next.col)) {
            std::lock_guard<std::mutex> lock(maze_mutex);
            if (exit_found) return; // Verifica se outra thread já encontrou a saída
            maze[next.row][next.col] = '.'; // Marca a posição como visitada antes de iniciar a thread
            threads.emplace_back(walk, next); // Cria uma nova thread para explorar o caminho
        }
    }

    // Espera todas as threads criadas terminarem
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
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

    // Inicia a primeira thread a partir da posição inicial
    std::thread main_thread(walk, initial_pos);
    main_thread.join();

     if (exit_found=true) {
        std::cout << "Saída encontrada!" << std::endl;
    } //else {
        //std::cout << "Não foi possível encontrar a saída." << std::endl;
       // }

    return 0;
}

