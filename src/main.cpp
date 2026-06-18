#include "leitor.h"
#include "resolvedor.h"

/*
int main()
{
    bool gerarSaida = true;
    std::string filePath = "instances/instance=0_n=3_m=2_d=3_type=B1.txt";

    auto instance = readInstance(filePath);

    if(!instance)
    {
        std::cout << "Deu algum erro ao abrir o arquivo!\n";
        return -1;
    }

    InstanciaInfo instancia = instance.value();

    resolve(instancia, gerarSaida, -1, -1);

    return 0;
}
*/

// Segunda main para leitura da instância via args
int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        std::cout << "Uso: " << argv[0] << " <arquivo_de_instancia> <param1> <param2>" << std::endl;
        return -1;
    }

    bool gerarSaida = true;
    std::string filePath = argv[1];

    auto instance = readInstance(filePath);

    if(!instance)
    {
        std::cout << "Deu algum erro ao abrir o arquivo!\n";
        return -1;
    }

    InstanciaInfo instancia = instance.value();

    resolveTwoIndex(instancia, gerarSaida, atoi(argv[2]), atoi(argv[3]));

    return 0;
}