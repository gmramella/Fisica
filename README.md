# Como fazer um clone da Box2D rodar no VS 2015 no Windows com GLFW

## Primeiros passos

[Clonar Box2D do GitHub](https://github.com/erincatto/Box2D) e extrair pasta

[Baixar binários pré-compilados da GLFW pra versões x32 e x64](https://www.glfw.org/download.html)

Criar pasta glfw na pasta Box2D-master, com subdiretórios 32 e 64

Copiar pasta include da glfw versão 32 para glfw/32

Copiar pasta include da glfw versão 64 para glfw/64

Copiar pasta lib-vc2015 da glfw versão 32 para glfw/32

Copiar pasta lib-vc2015 da glfw versão 64 para glfw/64

## Hierarquia

Feitos os passos anteriores, a hierarquia será:

	Box2D-master
	
		Build
		
		glfw
		
			32
			
				include
				
					GLFW
					
						glfw3.h
						
						glfw3native.h
						
				lib-vc2015
				
					glfw3.dll
					
					glfw3.lib
					
					glfw3dll.lib
					
			64
			
				include
				
					GLFW
					
						glfw3.h
						
						glfw3native.h
						
				lib-vc2015
				
					glfw3.dll
					
					glfw3.lib
					
					glfw3dll.lib
					
		HelloWorld
		
			HelloWorld.cpp
			
		...

## Últimos passos

[Baixar premake versão 5](https://premake.github.io/download.html), extrair executável e colá-lo na pasta Box2D-master

Executar comando "premake5 vs2015" na pasta Box2D-master

Abrir Box2D.sln da pasta Build criada pelo premake

Se o Visual Studio pedir, aceitar redirecionar projetos

Definir HelloWorld como projeto de inicialização

Verificar se o Visual Studio está debugando em x32 ou x64

Provavelmente a Box2D usará x64, neste caso, nos passos a seguir xx significa 64

Clicar no projeto HelloWorld na solução

Alt+Enter

Adicionar ..\glfw\xx\include em Propriedades de Configuração > C/C++ > Geral > Diretórios de Inclusão Adicionais

Adicionar ..\glfw\xx\lib-vc2015 em Propriedades de Configuração > Vinculador > Geral > Diretórios de Biblioteca Adicionais

Adicionar glfw3.lib e opengl32.lib em Propriedades de Configuração > Vinculador > Entrada > Dependências Adicionais

Se quiser debugar com x32, tem que repetir os 3 passos anteriores com xx = 32

# Features

Constantes matemáticas

Métodos para criação de cápsulas, círculos, casas (corpos com múltiplas fixtures), linhas e retângulos

Método de deleção quando objetos ficam abaixo da câmera

Método que calcula a área de um polígono convexo

Renovação do mundo para cada exercício (deleção dos corpos antigos do exercícios anterior e criação dos necessários pro novo)

Método que retorna um número aleatório dentro de um intervalo

Método que configura gravidade e mantém sua magnitude em uma variável acessível

Callbacks de erro, teclado e mouse (movimentação e clique)

# Listas de Exercícios

## Passos

Definir HelloWorld como projeto de inicialização

## Interação do usuário com o programa

**Pressionar 1 a 9 para criar o mundo do exercício 1 da lista de exercícios 1 a 9**

**Pressionar 1 a 9 para criar o mundo conforme o que é pedido nos exercícios 1 a 9 de uma lista de exercícios**

Nos exercícios que pedem interação:

	B cria caixas enquanto pressionada
	
	C cria círculo quando pressionada
	
	F produz uma força
	
	L cria linha quando pressionada
	
	R cria retângulo quando pressionada
	
	Espaço lança um círculo
	
	+ aumenta ângulo de lançamento do círculo
	
	- diminui ângulo de lançamento do círculo
	
	* dobra multiplicador da força de lançamento do círculo
	
	/ divide multiplicador da força de lançamento do círculo
	
	W faz círculo pular
	
	A move círculo para a esquerda
	
	D move círculo para a direita

Falhas:

	Cápsula não se move
	
	Pulo é ilimitado
	
	Movimentação é acelerada ao invés de constante
