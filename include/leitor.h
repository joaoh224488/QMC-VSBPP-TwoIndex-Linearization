#ifndef LEITOR_H
#define LEITOR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <optional>

struct Bin
{
    int custo;
    std::vector<int> dimensoes;
};

struct Item
{
    std::vector<int> dimensoes;
};

struct InstanciaInfo
{
    std::string nomeInstancia;
    int n;
    int m;
    int d;
    std::vector<Item> itens; // Tamanho n
    std::vector<Bin> tiposBins; // Tamanho m
    std::vector<std::vector<int>> custosLink; // Tamanho nxn

    std::vector<int> custoMaximoItem;

    int custoMaximoPossivel;

    InstanciaInfo() :
        nomeInstancia(""), 
        n(-1), 
        m(-1), 
        d(-1), 
        itens(std::vector<Item>()), 
        tiposBins(std::vector<Bin>()), 
        custosLink(std::vector<std::vector<int>>()) {}

    void printInfo();
};

std::optional<InstanciaInfo> readInstance(const std::string &);
void strip_str_left(std::string &);
int read_int_by_string(std::string &);
std::string read_type_instance(std::string & str);

#endif