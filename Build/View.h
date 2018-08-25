#pragma once
#include "Render.h"
class View
{
public:
	//Objeto para a classe que faz o desenho das formas de colisão dos corpos rígidos
	DebugDraw renderer;
	//Função que chama as rotinas de desenho para cada fixture de um corpo rígido, de acordo com o tipo da forma de colisão dela
	void DrawFixture(b2Fixture* fixture, b2Color color);
	//Para chamar a rotina de desenho das fixtures de um corpo
	void DrawBody(b2Body *b, b2Color color);
	//Rotina de render (não é mais callback)
	void Render(b2World *world);
	// Função que imprime todos os objetos  
	void PrintBodies(b2World *world);
};
