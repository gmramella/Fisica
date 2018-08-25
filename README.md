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

# Lista de Exercícios 1

## Passos (pessoal)

Após fazer o solicitado, criei classes Render e View e substituí gluOrtho2D por glOrtho com near = -1 e far = 1

## Passos (necessário)

Definir HelloWorld como projeto de inicialização

## Interação do usuário com o programa

Pressionar 0 a 8 para criar o mundo conforme o que é pedido no exercício 1 a 8 da lista 1 (0 é um default)

Nos exercícios que pedem interação:

	B cria caixas enquanto pressionada
	
	C cria círculo quando pressionada
	
	L cria linha quando pressionada
	
	R cria retângulo quando pressionada
