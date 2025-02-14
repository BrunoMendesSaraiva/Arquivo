#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// Constantes para os caracteres de densidade e o tamanho da densidade
const char* CARACTERES_DENSIDADE = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,^`.";
const int TAMANHO_DENSIDADE = 69; // Número de caracteres na string acima

// Função para ler um arquivo .foo e extrair a largura, altura e dados dos pixels
void lerArquivoFoo(const std::string &nomeArquivo, int &largura, int &altura, unsigned char* &pixels) {
    std::ifstream arquivoEntrada(nomeArquivo, std::ios::binary);
    if (!arquivoEntrada.is_open()) {
        throw std::runtime_error("Erro ao abrir o arquivo de entrada.");
    }

    // Ler a largura e altura da imagem
    arquivoEntrada >> largura >> altura;
    arquivoEntrada.ignore(); // Ignorar o caractere de nova linha após a leitura das dimensões

    int contagemPixels = largura * altura;
    pixels = new unsigned char[contagemPixels];
    
    // Ler os dados dos pixels
    arquivoEntrada.read(reinterpret_cast<char*>(pixels), contagemPixels);
    std::streamsize bytesLidos = arquivoEntrada.gcount();
    
    // Verificar se todos os bytes esperados foram lidos
    if (bytesLidos != contagemPixels) {
        pixels[bytesLidos] = 0;
        bytesLidos++;
    }

    if (bytesLidos != contagemPixels) {
        delete[] pixels;
        throw std::runtime_error("Erro ao ler os dados da imagem: número de bytes lidos não corresponde ao esperado.");
    }
}

// Função para converter os bytes lidos em uma representação ASCII
char** converterBytesParaAscii(int largura, int altura, const unsigned char* pixels) {
    char** imagemAscii = new char*[altura];

    for (int i = 0; i < altura; ++i) {
        imagemAscii[i] = new char[largura + 1]; // +1 para o caractere nulo
        for (int j = 0; j < largura; ++j) {
            int valorPixel = pixels[i * largura + j];
            int indice = (valorPixel * (TAMANHO_DENSIDADE - 1)) / 255;
            imagemAscii[i][j] = CARACTERES_DENSIDADE[indice]; // Conversão do valor do pixel para um caractere ASCII
        }
        imagemAscii[i][largura] = '\0'; // Terminar a linha com caractere nulo
    }

    return imagemAscii;
}

// Função para escrever a imagem ASCII em um arquivo de saída
void escreverArquivoFoo2(const std::string &nomeArquivo, int largura, int altura, char** imagemAscii) {
    std::ofstream arquivoSaida(nomeArquivo);
    if (!arquivoSaida.is_open()) {
        throw std::runtime_error("Erro ao abrir o arquivo de saída.");
    }

    // Escrever a largura e altura no arquivo de saída
    arquivoSaida << largura << ' ' << altura << '\n';

    // Escrever a imagem ASCII linha por linha
    for (int i = 0; i < altura; ++i) {
        arquivoSaida << imagemAscii[i] << '\n';
        if (!arquivoSaida) {
            throw std::runtime_error("Erro ao escrever os dados no arquivo de saída.");
        }
    }

    arquivoSaida.close();
    if (arquivoSaida.fail()) {
        throw std::runtime_error("Erro ao fechar o arquivo de saída.");
    }
}

// Função para liberar a memória alocada para a imagem ASCII
void liberarImagemAscii(int altura, char** imagemAscii) {
    for (int i = 0; i < altura; ++i) {
        delete[] imagemAscii[i];
    }
    delete[] imagemAscii;
}

int main(int argc, char *argv[]) {
    // Verificar se o número correto de argumentos foi passado
    if (argc != 3) {
        std::cerr << "Uso: .\\a.exe img0.foo img1.foo2\n";
        return -1;
    }

    try {
        int largura, altura;
        unsigned char* pixels;

        // Ler o arquivo de entrada
        lerArquivoFoo(argv[1], largura, altura, pixels);
        
        // Converter os bytes lidos para uma imagem ASCII
        char** imagemAscii = converterBytesParaAscii(largura, altura, pixels);
        
        // Escrever a imagem ASCII no arquivo de saída
        escreverArquivoFoo2(argv[2], largura, altura, imagemAscii);

        // Imprimir as dimensões da imagem e mensagem de sucesso
        std::cout << "Dimensões da imagem: " << largura << "x" << altura << '\n';
        std::cout << "Processo concluído com sucesso.\n";

        // Liberar a memória alocada
        delete[] pixels;
        liberarImagemAscii(altura, imagemAscii);

        return 0; // Sucesso
    } catch (const std::exception &e) {
        // Imprimir mensagem de erro em caso de exceção
        std::cerr << e.what() << '\n';
        return -1; // Erro
    }
}
