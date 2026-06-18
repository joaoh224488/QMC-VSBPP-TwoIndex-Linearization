#include "leitor.h"

std::optional<InstanciaInfo> readInstance(const std::string & filename)
{
    // Criando o objeto que será retornado
    InstanciaInfo instancia;

    std::string nome = filename.substr(0, filename.find(','));
    nome = nome.substr(nome.find_last_of("/\\") + 1);
    nome = nome.substr(0, nome.find_last_of('.'));

    instancia.nomeInstancia = nome;

    std::ifstream file(filename);

    if(!file)
    {
        std::cout << "Error opening file " << filename << std::endl;
        return std::nullopt;
    }

    std::string line;

    std::getline(file, line);

    // Recolhendo os valores principais da instância
    read_int_by_string(line);
    instancia.n = read_int_by_string(line);
    instancia.m = read_int_by_string(line);
    instancia.d = read_int_by_string(line);
    read_type_instance(line);

    instancia.itens.resize(instancia.n);
    instancia.tiposBins.resize(instancia.m);
    instancia.custosLink.resize(instancia.n);

    // Lendo os bins
    for(int i = 0; i < instancia.m; i++)
    {
        std::getline(file, line);

        // Eliminando o id
        read_int_by_string(line);

        instancia.tiposBins[i].custo = read_int_by_string(line);

        instancia.tiposBins[i].dimensoes.resize(instancia.d);

        // Lendo as dimensões do bin
        for(int j = 0; j < instancia.d; j++)
        {
            std::getline(file, line);

            // Função para limpar os espaços anteriores ao caracteres
            strip_str_left(line);

            int dim = read_int_by_string(line) - 1;

            int size_dim = read_int_by_string(line);

            instancia.tiposBins[i].dimensoes[dim] = size_dim;
        }
    }

    // Lendo os itens
    for(int i = 0; i < instancia.n; i++)
    {
        std::getline(file, line);

        read_int_by_string(line);

        instancia.custosLink[i].resize(instancia.n, 0);

        instancia.itens[i].dimensoes.resize(instancia.d);

        // Recolhendo as informações sobre dimensões
        for(int j = 0; j < instancia.d; j++)
        {
            std::getline(file, line);

            strip_str_left(line);

            int dim = read_int_by_string(line) - 1;

            int size_dim = read_int_by_string(line);

            instancia.itens[i].dimensoes[dim] = size_dim;
        }
    }

    // Lendo as ligações

    // Eliminando a linha que representa o início da leitura das ligações
    std::getline(file, line);

    int itemN1, itemN2, jointCost, auxItem1 = -1;

    instancia.custoMaximoItem = std::vector<int>(instancia.n, 0);
    instancia.custoMaximoPossivel = 0;

    while(std::getline(file, line))
    {
        // Excluindo números desnecessários
        read_int_by_string(line);
        read_int_by_string(line);

        // Recolhendo os valores necessários
        itemN1 = read_int_by_string(line) - 1;

        read_int_by_string(line);

        itemN2 = read_int_by_string(line) - 1;

        jointCost = read_int_by_string(line);

        // Adicionando os links
        if (itemN1 < itemN2){
            instancia.custosLink[itemN1][itemN2] += jointCost;

        }
        else{
            instancia.custosLink[itemN2][itemN1] += jointCost;
        }

        instancia.custoMaximoItem[itemN1] += jointCost;

        instancia.custoMaximoPossivel += jointCost;
    }

  

    file.close();

    return instancia;
}

// Função auxiliar para remover os espaços vazios de uma string
void strip_str_left(std::string & str)
{
    int i = 0;

    for(;i < str.size(); i++) if(str[i] != ' ' && str[i] != '\t') break;

    str = str.substr(i, str.size());
}

// Função auxiliar para fazer a leitura de elementos numéricos
int read_int_by_string(std::string & str)
{
    // Marcadores para a posição do inteiro
    int begin = -1, end = -1;

    // Encontrando as posições
    for(int i = 0; i < str.size(); i++)
    {   
        // Caso tenha encontrado um número
        if(str[i] >= 48 && str[i] <= 57)
        {   
            // Salvo a posição do início
            if(begin == -1) begin = i;
        }
        else if(begin != -1) // Caso não seja numérico e já tenha lido um número anteriormente
        {
            end = i - 1;
            break;
        }
    }

    // Caso não tenha encontrado elementos numéricos
    if(begin == -1 && end == -1) return -1;
    
    // Recolhendo o valor que será retornado
    int returned = std::stoi(str.substr(begin, end));

    // Alterando a string
    str = str.substr(end + 1, str.size());

    return returned;
}

std::string read_type_instance(std::string & str)
{
    // Estruturas auxiliares
    int begin;

    for(int i = 0; str[i] != '\0'; i++)
    {
        if(str[i] == '=') // Achou o início da string de tipo
        {
            begin = i + 1;
        }
    }

    return str.substr(begin, str.size());
}

void InstanciaInfo::printInfo()
{
    if(this->n == -1) return;

    std::cout << "Nome da instância: " << this->nomeInstancia << "\n";
    std::cout << "Quantidade de itens: " << this->n << "\n";
    std::cout << "Quantidade de tipos de bins: " << this->m << "\n";
    std::cout << "Quantidade de dimensões: " << this->d << "\n";
    
    std::cout << "Informações sobre itens:\n";

    for(int i = 0; i < this->n; i++)
    {
        std::cout << "Item[" << i << "] => ";

        for(int d = 0; d < this->d; d++)
        {
            std::cout << "Dim[" << d << "] = " << this->itens[i].dimensoes[d] << (d == this->d - 1 ? "\n" : ", ");
        }
    }

    std::cout << "Informações sobre tipos de bins:\n";

    for(int i = 0; i < this->m; i++)
    {
        std::cout << "Bin[" << i << "], Custo = " << this->tiposBins[i].custo << " => ";

        for(int d = 0; d < this->d; d++)
        {
            std::cout << "Dim[" << d << "] = " << this->tiposBins[i].dimensoes[d] << (d == this->d - 1 ? "\n" : ", ");
        }
    }

    std::cout << "Informações sobre os links:\n";

    for(int i = 0; i < this->n; i++)
    {
        for(int j = 0; j < this->n; j++)
        {
            if(this->custosLink[i][j] == 0) continue;

            std::cout << "Link[" << i << "][" << j << "] = " << this->custosLink[i][j] << "\n";
        }
    }
}