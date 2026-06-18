#ifndef RESOLVEDOR_H
#define RESOLVEDOR_H

#include "leitor.h"
#include <ilcplex/ilocplex.h>



// -1 => Sem upperBound | Sem maxSizeBins
void resolveBase(InstanciaInfo &, bool, int upperBound, int maxSizeBins);

void resolveLinearizado(InstanciaInfo &, bool, int upperBound, int maxSizeBins);

void resolveTwoIndex(InstanciaInfo &, bool, int upperBound, int maxSizeBins);

void resolveLinearizadoComOtimizacoes(InstanciaInfo &, bool, int upperBound, int maxSizeBins);

void resolveTwoIndexComOtimizacoes(InstanciaInfo &, bool, int upperBound, int maxSizeBins);

// Função para gerar o arquivo de saída, caso seja necessário
void gerarArquivoSaida(IloCplex &, InstanciaInfo &, const std::tuple<IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>, IloArray<IloArray<IloBoolVarArray>>> &);
void gerarArquivoSaida(IloCplex &, InstanciaInfo &, const std::tuple<IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>, IloArray<IloBoolVarArray>> &, double seconds);
void gerarArquivoSaida(IloCplex &cplex, InstanciaInfo &instance);
#endif