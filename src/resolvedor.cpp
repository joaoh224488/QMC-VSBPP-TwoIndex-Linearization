#include "resolvedor.h"
#include <cmath>
#include <chrono>
ILOSTLBEGIN


void gerarArquivoSaida(IloCplex &cplex, InstanciaInfo &instance, const std::tuple<IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>, IloArray<IloArray<IloBoolVarArray>>> & vars)
{
    cplex.exportModel(("out/result-" + instance.nomeInstancia + ".lp").c_str());

    std::string nomeArquivo = "out/solucao-" + instance.nomeInstancia + ".txt";
    std::ofstream arquivo(nomeArquivo);

    if(!arquivo.is_open())
    {
        std::cout << "Erro ao criar o arquivo\n";
        return;
    }

    const double epsilon = 1e-9;

    arquivo << "Status: " << cplex.getStatus() << "\n";
    try
    {
        double objValue = cplex.getObjValue();
        if(std::abs(objValue) < epsilon)
        {
            objValue = 0.0;
        }
        arquivo << "Valor obtido pela solução: " << objValue << "\n";
    }
    catch(IloException &e)
    {
        arquivo << "Valor obtido pela solução: NA (erro: " << e.getMessage() << ")\n";
    }
    catch(...)
    {
        arquivo << "Valor obtido pela solução: NA (erro desconhecido)\n";
    }

    // Printando os valores das variáveis de decisão (Que tiveram valor definido)

    for(int i = 0; i < instance.n; i++)
    {
        for(int j = 0; j < std::get<0>(vars)[i].getSize(); j++)
        {
            try
            {
                double value = cplex.getValue(std::get<0>(vars)[i][j]);
                if(std::abs(value) < epsilon)
                {
                    value = 0.0;
                }
                arquivo << "x[" << i << "][" << j << "] = " << value << "\n";
            }
            catch(IloException &e)
            {
                arquivo << "x[" << i << "][" << j << "] = NA (erro: " << e.getMessage() << ")\n";
            }
            catch(...)
            {
                arquivo << "x[" << i << "][" << j << "] = NA (erro desconhecido)\n";
            }
        }
    }

    for(int j = 0; j < std::get<1>(vars).getSize(); j++)
    {
        for(int m = 0; m < instance.m; m++)
        {
            try
            {
                double value = cplex.getValue(std::get<1>(vars)[j][m]);
                if(std::abs(value) < epsilon)
                {
                    value = 0.0;
                }
                arquivo << "y[" << j << "][" << m << "] = " << value << "\n";
            }
            catch(IloException &e)
            {
                arquivo << "y[" << j << "][" << m << "] = NA (erro: " << e.getMessage() << ")\n";
            }
            catch(...)
            {
                arquivo << "y[" << j << "][" << m << "] = NA (erro desconhecido)\n";
            }
        }
    }

    for(int i = 0; i < instance.n; i++)
    {
        for(int j = 0; j < std::get<2>(vars)[i].getSize(); j++)
        {
            for(int s = 0; s < instance.n; s++)
            {
                try
                {
                    double value = cplex.getValue(std::get<2>(vars)[i][j][s]);
                    if(std::abs(value) < epsilon)
                    {
                        value = 0.0;
                    }
                    arquivo << "z[" << i << "][" << j << "][" << s << "] = " << value << "\n";
                }
                catch(IloException &e)
                {
                    arquivo << "z[" << i << "][" << j << "][" << s << "] = NA (erro: " << e.getMessage() << ")\n";
                }
                catch(...)
                {
                    arquivo << "z[" << i << "][" << j << "][" << s << "] = NA (erro desconhecido)\n";
                }
            }
        }
    }

    arquivo.close();
}


void gerarArquivoSaida(IloCplex &cplex, InstanciaInfo &instance)
{
    cplex.exportModel(("out/result-" + instance.nomeInstancia + ".lp").c_str());

    std::string nomeArquivo = "out/solucao-" + instance.nomeInstancia + ".txt";
    std::ofstream arquivo(nomeArquivo);

    if(!arquivo.is_open())
    {
        std::cout << "Erro ao criar o arquivo\n";
        return;
    }

    const double epsilon = 1e-9;

    arquivo << "Status: " << cplex.getStatus() << "\n";
    try
    {
        double objValue = cplex.getObjValue();
        if(std::abs(objValue) < epsilon)
        {
            objValue = 0.0;
        }
        arquivo << "Valor obtido pela solução: " << objValue << "\n";
    }
    catch(IloException &e)
    {
        arquivo << "Valor obtido pela solução: NA (erro: " << e.getMessage() << ")\n";
    }
    catch(...)
    {
        arquivo << "Valor obtido pela solução: NA (erro desconhecido)\n";
    }

   
   

    arquivo.close();
}



void resolveTwoIndex(InstanciaInfo & instance, bool gerarSaida, int upperBound, int maxSizeBins)
{
    if(maxSizeBins == -1)
    {
        maxSizeBins = instance.n;
    }

    IloEnv env; // Criando o ambiente CPLEX(Tudo gira envolta dele)

    try
    {
        IloModel model(env); // Criando o modelo

        // Variáveis de decisão

        IloArray<IloBoolVarArray> x(env, instance.n); // x_ij => Item i no bin j
        for(int i = 0; i < instance.n; i++)
        {
            x[i] = IloBoolVarArray(env, maxSizeBins);

            for(int j = 0; j < maxSizeBins; j++)
            {
                char nome[32];
                snprintf(nome, sizeof(nome), "x_%d_%d", i, j);
                x[i][j].setName(nome);
            }
        }

        IloArray<IloBoolVarArray> y(env, maxSizeBins); // y_jm => Bin j do tipo m
        for(int i = 0; i < maxSizeBins; i++)
        {
            y[i] = IloBoolVarArray(env, instance.m);

            for(int m = 0; m < instance.m; m++)
            {
                char nome[32];
                snprintf(nome, sizeof(nome), "y_%d_%d", i, m);
                y[i][m].setName(nome);
            }
        }

        IloArray<IloBoolVarArray> z(env, instance.n); // z_is => Item i junto do item s        
    
        for(int i = 0; i < instance.n; i++)
        {
            z[i] = IloBoolVarArray(env, instance.n);
            
            for(int s = i + 1; s < instance.n; s++)
            {
               
                z[i][s] = IloBoolVar(env); 

                char nome[32];
                snprintf(nome, sizeof(nome), "z_%d_%d", i, s);
                z[i][s].setName(nome);
            }
        }

        // Função objetivo

        IloExpr custoTipo(env), custoJoint(env), funcFinal(env);

        // Parte dos tipos dos bins
        for(int j = 0; j < maxSizeBins; j++)
        {
            for(int m = 0; m < instance.m; m++)
            {
                custoTipo += instance.tiposBins[m].custo * y[j][m];
            }
        }

        custoJoint += instance.custoMaximoPossivel;
        // Parte dos custos acumulados
        
        for(int i = 0; i < instance.n; i++)
        {
            for(int s = i + 1; s < instance.n; s++)
            {
                custoJoint +=  (- instance.custosLink[i][s] * z[i][s]);
            }
        }

        

        funcFinal = custoTipo +  custoJoint;

        IloObjective objetivo = IloMinimize(env, funcFinal);
        objetivo.setName("FuncaoObjetivo");
        model.add(objetivo);

        custoTipo.end();
        custoJoint.end();
        funcFinal.end();



        // Restrições

        // Restrição para garantir que o item esteja em algum bin
        for(int i = 0; i < instance.n; i++)
        {
            IloExpr somaItemNoBin(env);

            for(int j = 0; j < maxSizeBins; j++)
            {
                somaItemNoBin += x[i][j];
            }

            IloConstraint restricaoItemNoBin(somaItemNoBin == 1);
            char nome[32];
            snprintf(nome, sizeof(nome), "ItemNoBin_%d", i);
            restricaoItemNoBin.setName(nome);
            model.add(restricaoItemNoBin);

            somaItemNoBin.end();
        }

        // Restrição para garantir que um bin utilizado tenha apenas um tipo
        for(int j = 0; j < maxSizeBins; j++)
        {
            IloExpr somaTipoBin(env);

            for(int m = 0; m < instance.m; m++)
            {
                somaTipoBin += y[j][m];
            }

            IloConstraint restricaoTipoBin(somaTipoBin <= 1);
            char nome[32];
            snprintf(nome, sizeof(nome), "TipoUnicoBin_%d", j);
            restricaoTipoBin.setName(nome);
            model.add(restricaoTipoBin);

            somaTipoBin.end();
        }

        // Restrição para garantir a não violação das capacidades dos bins
        for(int j = 0; j < maxSizeBins; j++)
        {
            for(int d = 0; d < instance.d; d++)
            {
                IloExpr espOcup(env), espDisp(env);
                
                for(int i = 0; i < instance.n; i++)
                {
                    espOcup += instance.itens[i].dimensoes[d] * x[i][j];
                }

                for(int m = 0; m < instance.m; m++)
                {
                    espDisp += instance.tiposBins[m].dimensoes[d] * y[j][m];
                }

                IloConstraint restricaoCapacidade(espOcup <= espDisp);
                char nome[32];
                snprintf(nome, sizeof(nome), "Capacidade_%d_%d", j, d);
                restricaoCapacidade.setName(nome);
                model.add(restricaoCapacidade);

                espDisp.end();
                espOcup.end();
            }
        }


        for(int i = 0; i < instance.n; i++)
        {
            for (int j = 0; j < maxSizeBins; j++){
                for(int s = i + 1; s < instance.n; s++)
                {
                    IloConstraint restricaoLink1(z[i][s] + x[i][j] - x[s][j] <= 1);
                    char nome[40];
                    snprintf(nome, sizeof(nome), "Link1_%d_%d_%d", i, s, j);
                    restricaoLink1.setName(nome);
                    model.add(restricaoLink1);
                }
            }
        }

        for(int i = 0; i < instance.n; i++)
        {
            for (int j = 0; j < maxSizeBins; j++){
                for(int s = i + 1; s < instance.n; s++)
                {
                    IloConstraint restricaoLink2(z[i][s] - x[i][j] + x[s][j] <= 1);
                    char nome[40];
                    snprintf(nome, sizeof(nome), "Link2_%d_%d_%d", i, s, j);
                    restricaoLink2.setName(nome);
                    model.add(restricaoLink2);
                }
            }
        }

        for(int i = 0; i < instance.n; i++)
        {
            for (int j = 0; j < maxSizeBins; j++){
                for(int s = i + 1; s < instance.n; s++)
                {
                    IloConstraint restricaoLink3(x[i][j] + x[s][j] - z[i][s] <= 1);
                    char nome[40];
                    snprintf(nome, sizeof(nome), "Link3_%d_%d_%d", i, s, j);
                    restricaoLink3.setName(nome);
                    model.add(restricaoLink3);
                }
            }
        }
        
        // Quebra de simetria:
        // O item i não pode estar em um bin com índice maior que i.
        for(int i = 0; i < instance.n; i++)
        {
            for(int j = i + 1; j < maxSizeBins; j++)
            {
                IloConstraint restricaoSimetria(x[i][j] == 0);
                char nome[40];
                snprintf(nome, sizeof(nome), "QuebraSimetria_%d_%d", i, j);
                restricaoSimetria.setName(nome);
                model.add(restricaoSimetria);
            }
        }

        IloConstraint restricaoFixaItem0(x[0][0] == 1);
        restricaoFixaItem0.setName("FixaItem0NoBin0");
        model.add(restricaoFixaItem0);

        auto inicio = std::chrono::high_resolution_clock::now();

        
        IloCplex cplex(model); // Criando o solver

        cplex.setParam(IloCplex::Param::Threads, 0); // Setando a quantidade de threads
        cplex.setParam(IloCplex::Param::TimeLimit, 7200);

        // Limite de memória RAM (em MB) que o CPLEX pode usar para
        // árvore de busca (working memory). 10 GB = 10240 MB.
        cplex.setParam(IloCplex::Param::WorkMem, 10240.0);

        // Quando o limite de WorkMem é atingido, o CPLEX por padrão
        // passa a gravar nós da árvore em disco (node file). Para
        // realmente travar o uso em memória RAM, mantemos o modo de
        // compressão em memória (1) em vez de permitir gravação em
        // disco (2 ou 3).
        cplex.setParam(IloCplex::Param::MIP::Strategy::File, 1);

        // Limite adicional (em MB) específico para a árvore de busca,
        // como salvaguarda extra ao limite de WorkMem.
        cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 10240.0);

        // Caso possua um custo pré-fixado de solução
        if(upperBound != -1)
        {
            cplex.setParam(IloCplex::Param::MIP::Tolerances::UpperCutoff, upperBound);
        }

        if(cplex.solve())
        {
            std::cout << "Status: " << cplex.getStatus() << "\n";
            std::cout << "Valor obtido: " << cplex.getObjValue() << "\n";
        }
        else
        {
            std::cout << "Deu algum erro ao resolver o modelo\n";
        }
        
        if(gerarSaida)
        {
            auto fim = std::chrono::high_resolution_clock::now();

            double tempo =
                std::chrono::duration<double>(fim - inicio).count();

            std::cout << "Tempo: " << tempo << " segundos\n";
            gerarArquivoSaida(cplex, instance,std::forward_as_tuple(x, y, z), tempo);
        }
    }
    catch(IloException& e)
    {
        std::cout << "Deu pau, erro: " << e.getMessage() << "\n";
    }
    catch(...)
    {
        std::cout << "Deu pau por erro desconhecido\n";
    }

    env.end();
}
void gerarArquivoSaida(IloCplex & cplex, InstanciaInfo & instance, const std::tuple<IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>> & vars, double elapsedTime){
    cplex.exportModel(("out/result-" + instance.nomeInstancia + ".lp").c_str());

    std::string nomeArquivo = "out/solucao-" + instance.nomeInstancia + ".txt";
    std::ofstream arquivo(nomeArquivo);

    if(!arquivo.is_open())
    {
        std::cout << "Erro ao criar o arquivo\n";
        return;
    }

    const double epsilon = 1e-9;

    arquivo << "Status: " << cplex.getStatus() << "\n";

    arquivo << "Time: " << elapsedTime << "\n";
    
    try
    {
        double objValue = cplex.getObjValue();
        if(std::abs(objValue) < epsilon)
        {
            objValue = 0.0;
        }
        arquivo << "Valor obtido pela solução: " << objValue << "\n";
    }
    catch(IloException &e)
    {
        arquivo << "Valor obtido pela solução: NA (erro: " << e.getMessage() << ")\n";
    }
    catch(...)
    {
        arquivo << "Valor obtido pela solução: NA (erro desconhecido)\n";
    }

    // Printando os valores das variáveis de decisão (Que tiveram valor definido)

    for(int i = 0; i < instance.n; i++)
    {
        for(int j = 0; j < std::get<0>(vars)[i].getSize(); j++)
        {
            try
            {
                double value = cplex.getValue(std::get<0>(vars)[i][j]);
                if(std::abs(value) < epsilon)
                {
                    value = 0.0;
                }
                arquivo << "x[" << i << "][" << j << "] = " << value << "\n";
            }
            catch(IloException &e)
            {
                arquivo << "x[" << i << "][" << j << "] = NA (erro: " << e.getMessage() << ")\n";
            }
            catch(...)
            {
                arquivo << "x[" << i << "][" << j << "] = NA (erro desconhecido)\n";
            }
        }
    }

    for(int j = 0; j < std::get<1>(vars).getSize(); j++)
    {
        for(int m = 0; m < instance.m; m++)
        {
            try
            {
                double value = cplex.getValue(std::get<1>(vars)[j][m]);
                if(std::abs(value) < epsilon)
                {
                    value = 0.0;
                }
                arquivo << "y[" << j << "][" << m << "] = " << value << "\n";
            }
            catch(IloException &e)
            {
                arquivo << "y[" << j << "][" << m << "] = NA (erro: " << e.getMessage() << ")\n";
            }
            catch(...)
            {
                arquivo << "y[" << j << "][" << m << "] = NA (erro desconhecido)\n";
            }
        }
    }

    for(int i = 0; i < instance.n; i++)
    {
      
            for(int s = i + 1; s < instance.n; s++)
            {
                try
                {
                    double value = cplex.getValue(std::get<2>(vars)[i][s]);
                    if(std::abs(value) < epsilon)
                    {
                        value = 0.0;
                    }
                    arquivo << "z[" << i << "][" << s << "] = " << value << "\n";
                }
                catch(IloException &e)
                {
                    arquivo << "z[" << i << "][" << s << "] = NA (erro: " << e.getMessage() << ")\n";
                }
                catch(...)
                {
                    arquivo << "z[" << i << "][" << s << "] =  NA (erro desconhecido)\n";
                }
            }
        
    }

    arquivo.close();

}