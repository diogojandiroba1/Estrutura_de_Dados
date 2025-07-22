import random
import string

def gerar_palavra_aleatoria(tamanho):
    """Gera uma string aleatória de letras minúsculas."""
    letras = string.ascii_lowercase
    return ''.join(random.choice(letras) for i in range(tamanho))

def gerar_input_final(num_palavras, num_consultas):
    """
    Gera um input onde a busca é feita apenas pelas palavras principais.
    """
    linhas_dicionario = []
    palavras_unicas = set()
    # Lista APENAS com as palavras principais para a busca
    palavras_principais_para_consulta = [] 

    # 1. Gerar o dicionário de palavras e sinônimos
    print("Gerando o dicionário de palavras e sinônimos...")
    while len(linhas_dicionario) < num_palavras:
        # Gera uma palavra principal única
        while True:
            tamanho_palavra = random.randint(4, 10)
            palavra_principal = gerar_palavra_aleatoria(tamanho_palavra)
            if palavra_principal not in palavras_unicas:
                palavras_unicas.add(palavra_principal)
                # Adiciona a palavra principal à lista de consulta
                palavras_principais_para_consulta.append(palavra_principal)
                break
        
        num_sinonimos = random.randint(2, 5)
        sinonimos = []
        for _ in range(num_sinonimos):
            # Gera um sinônimo único (mas não o adiciona à lista de consulta)
            while True:
                tamanho_sinonimo = random.randint(4, 10)
                sinonimo = gerar_palavra_aleatoria(tamanho_sinonimo)
                if sinonimo not in palavras_unicas:
                    palavras_unicas.add(sinonimo)
                    sinonimos.append(sinonimo)
                    break
            
        linha = f"{palavra_principal} {num_sinonimos} {' '.join(sinonimos)}"
        linhas_dicionario.append(linha)

    # 2. Gerar a lista de consultas
    print("Gerando a lista de buscas...")
    # A escolha é feita APENAS da lista de palavras principais
    consultas = random.choices(palavras_principais_para_consulta, k=num_consultas)

    # 3. Montar o arquivo final
    output = []
    output.append(str(num_palavras))
    output.extend(linhas_dicionario)
    output.append(str(num_consultas))
    output.extend(consultas)
    
    return "\n".join(output)

# --- Execução ---
print("Iniciando a geração do arquivo...")
conteudo_final = gerar_input_final(1000000, 1000000)

# Salvar o resultado em um arquivo
try:
    with open('input_busca_palavra_principal.txt', 'w', encoding='utf-8') as f:
        f.write(conteudo_final)
    print("\n✅ Arquivo 'input_busca_palavra_principal.txt' gerado com sucesso!")
    print("As 10.000 buscas contêm apenas as palavras principais do dicionário.")
except Exception as e:
    print(f"\n❌ Ocorreu um erro ao salvar o arquivo: {e}")