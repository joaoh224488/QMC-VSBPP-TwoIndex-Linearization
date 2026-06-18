import os
import re
import pandas as pd

def extrair_dados_instancia(pasta_resultados):
    melhores_resultados = {}

    # Expressões regulares para capturar o Custo e os Bins
    re_bins = re.compile(r'Bins:\s*([\d\s,]+)')
    re_cost = re.compile(r'Cost:\s*(\d+)')

    # Percorre todos os arquivos contidos na pasta informada
    for nome_arquivo in os.listdir(pasta_resultados):
        # Verifica se o arquivo segue o padrão esperado: LNS_instance={instancia}_{rodada}
        match_nome = re.match(r'LNS_instance=(\d+)_(\d+)', nome_arquivo)
        if not match_nome:
            continue
        
        instancia = int(match_nome.group(1))
        rodada = int(match_nome.group(2))
        
        caminho_completo = os.path.join(pasta_resultados, nome_arquivo)
        
        try:
            with open(caminho_completo, 'r', encoding='utf-8') as f:
                conteudo = f.read()
                
                # Busca o custo e os bins no corpo do arquivo
                match_cost = re_cost.search(conteudo)
                match_bins = re_bins.search(conteudo)
                
                if match_cost and match_bins:
                    custo = int(match_cost.group(1))
                    
                    # Processa a string de bins para contar a quantidade de números
                    bins_str = match_bins.group(1).strip()
                    # Divide pelas vírgulas e remove elementos vazios causados por vírgula no final
                    lista_bins = [b.strip() for b in bins_str.split(',') if b.strip()]
                    num_bins = len(lista_bins)
                    
                    # Critério de seleção do melhor resultado para a instância:
                    # Menor Custo. Se empatar, escolhe o que tiver Menor número de Bins.
                    if instancia not in melhores_resultados:
                        melhores_resultados[instancia] = {
                            'Instancia': instancia,
                            'Melhor_Rodada': rodada,
                            'Custo': custo,
                            'Num_Bins': num_bins,
                            'Arquivo': nome_arquivo
                        }
                    else:
                        atual = melhores_resultados[instancia]
                        # Atualiza se encontrar um custo menor OU custo igual com menos Bins
                        if (custo < atual['Custo']) or (custo == atual['Custo'] and num_bins < atual['Num_Bins']):
                            melhores_resultados[instancia] = {
                                'Instancia': instancia,
                                'Melhor_Rodada': rodada,
                                'Custo': custo,
                                'Num_Bins': num_bins,
                                'Arquivo': nome_arquivo
                            }
        except Exception as e:
            print(f"Erro ao ler o arquivo {nome_arquivo}: {e}")

    # Ordena o dicionário pelas chaves das instâncias (1 a 25)
    resultados_finais = [melhores_resultados[i] for i in sorted(melhores_resultados.keys())]
    return resultados_finais

# Configuração da pasta de entrada
pasta = 'results_20M'

print("Iniciando o processamento dos arquivos...")
dados_extraidos = extrair_dados_instancia(pasta)

# Exibição e exportação dos resultados obtidos
if dados_extraidos:
    df = pd.DataFrame(dados_extraidos)
    
    print("\n" + "="*23 + " MELHORES RESULTADOS POR INSTÂNCIA " + "="*23)
    print(df[['Instancia', 'Melhor_Rodada', 'Custo', 'Num_Bins', 'Arquivo']].to_string(index=False))
    
    # Salva os resultados compilados em uma planilha CSV
    df.to_csv('melhores_resultados.csv', index=False, encoding='utf-8')
    print("\nResultados consolidados salvos com sucesso em 'melhores_resultados.csv'!")
else:
    print(f"\n[Aviso] Nenhum arquivo compatível foi encontrado na pasta '{pasta}'.")
    print("Verifique se o script está na mesma pasta onde 'results_20M' está localizada.")


def encontrar_arquivo_instancia(numero_instancia, pasta_instances='instances'):
    """
    Busca na pasta especificada o arquivo correspondente ao número da instância.
    O formato esperado do nome é: instance={numero_instancia}_{resto}.txt
    
    Retorna o caminho completo do arquivo se encontrado, ou None caso contrário.
    """
    # Cria o padrão de busca, ex: "instances/instance=1_*.txt"
    padrao_busca = os.path.join(pasta_instances, f"instance={numero_instancia}_*.txt")
    
    # Procura por todos os arquivos que casam com o padrão
    arquivos_encontrados = glob.glob(padrao_busca)
    
    if arquivos_encontrados:
        # Se houver mais de um (o que não deve acontecer), pega o primeiro
        return arquivos_encontrados[0]
    else:
        print(f"[Aviso] Arquivo para a instância {numero_instancia} não foi encontrado na pasta '{pasta_instances}'.")
        return None
    
import glob
import subprocess
import random
pasta_instancias_originais = 'instances'
file = 'program'

for dados in dados_extraidos:
    num_instancia = dados['Instancia']
    melhor_custo = dados['Custo']
    num_bins = dados['Num_Bins']
    
    caminho_instancia = encontrar_arquivo_instancia(num_instancia, pasta_instancias_originais)

    # Sem o stdout=PIPE, ele joga direto no terminal
    proc = subprocess.Popen(
        [f"./{file}", caminho_instancia, str(melhor_custo), str(num_bins)]
    )
    
    # É uma boa prática esperar o processo terminar antes de ir para a próxima instância
    proc.wait()