/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "Box2D/Box2D.h"
#include "Build/View.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

const b2Vec2 b2Vec2_right = b2Vec2(1, 0);
const b2Vec2 b2Vec2_left = b2Vec2(-1, 0);
const b2Vec2 b2Vec2_up = b2Vec2(0, 1);
const b2Vec2 b2Vec2_down = b2Vec2(0, -1);

const b2Vec2 b2Vec2_upright = b2Vec2(cosf(45 * DEGTORAD), sinf(45 * DEGTORAD));
const b2Vec2 b2Vec2_upleft = b2Vec2(cosf(135 * DEGTORAD), sinf(135 * DEGTORAD));
const b2Vec2 b2Vec2_downright = b2Vec2(cosf(225 * DEGTORAD), sinf(225 * DEGTORAD));
const b2Vec2 b2Vec2_downleft = b2Vec2(cosf(315 * DEGTORAD), sinf(315 * DEGTORAD));

using namespace std;

void CreateBodies();//Cria corpos iniciais do cen�rio
b2Body* CreateCapsule(b2Vec2 center = b2Vec2(0, 0), float32 w = 1, float32 h = 1, float32 m = 1, float32 f = 0.5, float32 r = 0.5, b2BodyType bt = b2_dynamicBody);
b2Body* CreateCircle(b2Vec2 center = b2Vec2(0, 0), float32 rad = 1, float32 m = 1, float32 f = 0.5, float32 r = 0.5, b2BodyType bt = b2_dynamicBody);
b2Body* CreateHouses(b2Vec2 center = b2Vec2(0, 0), float32 m = 1000, float32 f = 1, float32 r = 1, b2BodyType bt = b2_staticBody);
b2Body* CreateLine(b2Vec2 center = b2Vec2(0, -39.5), b2Vec2 left = b2Vec2(-39.5, 0), b2Vec2 right = b2Vec2(39.5, 0), float32 m = 2, float32 f = 0.5, float32 r = 0.5, b2BodyType bt = b2_staticBody);
b2Body* CreateRectangle(b2Vec2 center = b2Vec2(0, 0), float32 w = 1, float32 h = 1, float32 m = 1, float32 f = 0.5, float32 r = 0.5, b2BodyType bt = b2_dynamicBody);
void Delete();//Remove caixas que saem de cena
void DeleteBodies();//Remove todos corpos do cen�rio
void DeleteJoints();//Remove todas juntas do cen�rio
void InitBox2D();//Fun��o de inicializa��o da Box2D
float32 PolygonArea(b2Vec2* vertices, int size);//Calcula �rea de um pol�gono CONVEXO
float RandomInRange(float least, float most);//Retorna um float aleat�rio no intervalo [least, most]
void RunBox2D();//Fun��o de Execu��o da Simula��o
void SetGravity(b2Vec2 gravity);//Fun��o de configura��o da gravidade

struct GearJoints {
	b2RevoluteJoint* rJoint;
	b2PrismaticJoint* pJoint;
	b2GearJoint* gJoint;
};

struct WheelJoints {
	b2WheelJoint* wheel1;
	b2WheelJoint* wheel2;
};

b2DistanceJoint* CreateDistanceJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos1 = b2Vec2_zero, b2Vec2 pos2 = b2Vec2_zero, bool collide = true);
b2RevoluteJoint* CreateRevoluteJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos = b2Vec2_zero, bool collide = true);
b2PrismaticJoint* CreatePrismaticJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos = b2Vec2_zero, bool collide = false);
b2PulleyJoint* CreatePulleyJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos = b2Vec2_zero, bool collide = true);
GearJoints* CreateGearJoint(b2Body *bodyA, b2Body *bodyB, b2Body *floor, b2Vec2 pos = b2Vec2_zero, bool collide = true);
WheelJoints* CreateWheelJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos = b2Vec2_zero, bool collide = true);

static void click_callback(GLFWwindow* window, int button, int action, int mode);//Callback de click de mouse da glfw
static void error_callback(int error, const char* description);//Callback de erro da glfw
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);//Callback de teclado da glfw
static void move_callback(GLFWwindow* window, double xpos, double ypos);//Callback de movimenta��o de mouse da glfw

void CreateRobot();

//Algumas globais para interface e simula��o (IDEAL: criar uma classe gerenciadora)
float32 timeStep;
int32 velocityIterations;
int32 positionIterations;
float xMin = -40.0, xMax = 40.0, yMin = -40.0, yMax = 40.0; //ortho2D

b2Vec2 defaultGravity = b2Vec2(0, -9.8f);
float32 gravityMagnitude = sqrtf(defaultGravity.x * defaultGravity.x + defaultGravity.y * defaultGravity.y);
b2World *world;
std::vector<b2Body*> bodies;

int list = 1;
int task = 1;

float fric = 0;
float rest = 0;
bool fricDir = 1;
bool restDir = 1;

float32 angle = 45;
float32 forceMultiplier = 16;
b2Vec2 movement = b2Vec2(0, 0);
bool movementChanged = false;

int main(int argc, char** argv)
{
	B2_NOT_USED(argc);
	B2_NOT_USED(argv);

	srand((unsigned int)time(NULL));

	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "HELLO GLFW!! BYE BYE GLUT!!!", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, click_callback);
	glfwSetCursorPosCallback(window, move_callback);

	InitBox2D();
	CreateBodies();
	View view;

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (width >= height)
		{
			ratio = width / (float)height;
			glOrtho(xMin*ratio, xMax*ratio, yMin, yMax, -1, 1);
		}
		else
		{
			ratio = height / (float)width;
			glOrtho(xMin, xMax, yMin*ratio, yMax*ratio, -1, 1);
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		RunBox2D();
		Delete();
		view.Render(world);
		//view.PrintBodies(world);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	world->~b2World();
	world = NULL;
	exit(EXIT_SUCCESS);
	return 0;
}

void CreateBodies()
{
	float32 w21 = 5;
	float32 h21 = 20;
	float32 gap21 = 4;

	float32 w23 = 5;
	float32 h23 = 20;

	float32 w32 = 8;
	float32 h32 = 30;

	switch (10 * list + task)
	{
	case 11:
		SetGravity(defaultGravity);
		bodies.push_back(CreateRectangle(b2Vec2(0, 0), 10, 10, 1000, 0.5, 0.5, b2_dynamicBody));
		bodies.push_back(CreateCircle(b2Vec2(0, 10), 5, 1, 0.5, 0.5, b2_dynamicBody));
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-39.5, 0), b2Vec2(39.5, 0), 2, 0.5, 0.5, b2_staticBody));
		break;
	case 12:
		SetGravity(b2Vec2(defaultGravity.x * -1, defaultGravity.y * -1));
		bodies.push_back(CreateLine(b2Vec2(0, -40), b2Vec2(-53, 0), b2Vec2(53, 0), 2, 0.5, 0.5, b2_staticBody));
		bodies.push_back(CreateLine(b2Vec2(0, 40), b2Vec2(-53, 0), b2Vec2(53, 0), 2, 0.5, 0.5, b2_staticBody));
		bodies.push_back(CreateLine(b2Vec2(-53, 0), b2Vec2(0, 40), b2Vec2(0, -53), 2, 0.5, 0.5, b2_staticBody));
		bodies.push_back(CreateLine(b2Vec2(53, 0), b2Vec2(0, 40), b2Vec2(0, -53), 2, 0.5, 0.5, b2_staticBody));
		bodies.push_back(CreateCircle(b2Vec2(0, 0), 5, 1, 0.5, 0.5, b2_dynamicBody));
		break;
	case 13:
		SetGravity(defaultGravity);
		break;
	case 14:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-39.5, 0), b2Vec2(39.5, 0), 2, 0.5, 0.0, b2_staticBody));
		break;
	case 15:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -10), b2Vec2(-30, 30), b2Vec2(30, -30), 2, 0.5, 0.0, b2_staticBody));
		break;
	case 16:
		SetGravity(defaultGravity);
		for (int i = 1; i <= 6; i++)
		{
			bodies.push_back(CreateRectangle(b2Vec2(-5.0f, i * 10.0f), 10.0f, 10.0f, 1000.0f, 0.5f, 0.5f, b2_dynamicBody));
			bodies.push_back(CreateCircle(b2Vec2(5.0f, i * 10.0f), 5.0f, 1.0f, 0.5f, 0.5f, b2_dynamicBody));
		}
		bodies.push_back(CreateLine(b2Vec2(0.0f, -39.5), b2Vec2(-39.5, 0.0f), b2Vec2(39.5, 0.0f), 2.0f, 0.5f, 0.5f, b2_staticBody));
		break;
	case 17:
		SetGravity(defaultGravity);
		for (int i = -5; i < 5; i++)
		{
			for (int j = 1; j <= 10; j++)
			{
				float32 side = 2;
				bodies.push_back(CreateRectangle(b2Vec2(44.0f + i * side, -39.5f + (j - 1 / 2.0f) * side), side, side, 1000.0f, 1.0f, 0.0f, b2_dynamicBody));
			}
		}
		bodies.push_back(CreateLine(b2Vec2(0.0f, -39.5), b2Vec2(-60, 0.0f), b2Vec2(60, 0.0f), 2.0f, 1.0f, 0.0f, b2_staticBody));
		break;
	case 18:
		SetGravity(defaultGravity);
		bodies.push_back(CreateHouses(b2Vec2(0.0f, 0.0f), 1000.0f, 1.0f, 1.0f, b2_staticBody));
		break;
	
	case 21:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-50, 0), b2Vec2(50, 0), 2, 1.0f, 0.0f, b2_staticBody));
		for (int i = 0; i < 6; i++)
		{
			bodies.push_back(CreateRectangle(b2Vec2(0.0f + i * (w21 + gap21), -39.5 + h21 / 2), w21, h21, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		}
		break;
	case 22:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-50, 0), b2Vec2(50, 0), 2, 1.0f, 0.0f, b2_staticBody));
		break;
	case 23:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-50, 0), b2Vec2(50, 0), 2, 1.0f, 0.0f, b2_staticBody));
		bodies.push_back(CreateCircle(b2Vec2(0, -39.5 + h23 / 2), w23, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		break;

	case 31:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-50, 0), b2Vec2(50, 0), 2, 1.0f, 0.0f, b2_staticBody));
		
		CreateRobot();
		break;
	case 32:
		SetGravity(defaultGravity);
		bodies.push_back(CreateLine(b2Vec2(0, -39.5), b2Vec2(-50, 0), b2Vec2(50, 0), 2, 1.0f, 0.0f, b2_staticBody));
		
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f, -39.5 + 20 /2), 10, 20, 100.0f, 1.0f, 0.0f, b2_staticBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 1 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 2 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 3 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 4 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 5 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(-35.0f+ 6 * 10, -23.5 + 4 / 2), 9, 4, 100.0f, 1.0f, 0.0f, b2_dynamicBody));
		bodies.push_back(CreateRectangle(b2Vec2(35.0f, -39.5 + 20 / 2), 10, 20, 100.0f, 1.0f, 0.0f, b2_staticBody));

		b2RevoluteJointDef dJointDef;
		b2Vec2 pos;

		pos = 0.5f * (bodies[1]->GetWorldCenter() + bodies[2]->GetWorldCenter()) + b2Vec2(0, 4);
		dJointDef.Initialize(bodies[1], bodies[2], pos);
		dJointDef.collideConnected = true;
		world->CreateJoint(&dJointDef);

		for (int i = 2; i < bodies.size()-2; i++)
		{
			pos = 0.5f * (bodies[i]->GetWorldCenter() + bodies[i+1]->GetWorldCenter());
			dJointDef.Initialize(bodies[i], bodies[i + 1], pos);
			dJointDef.collideConnected = true;
			world->CreateJoint(&dJointDef);
		}

		pos = 0.5f * (bodies[7]->GetWorldCenter() + bodies[8]->GetWorldCenter()) + b2Vec2(0, 4);
		dJointDef.Initialize(bodies[7], bodies[8], pos);
		dJointDef.collideConnected = true;
		world->CreateJoint(&dJointDef);
	}
}

b2Body* CreateCapsule(b2Vec2 center, float32 w, float32 h, float32 m, float32 f, float32 r, b2BodyType bt)
{
	float32 rad = w / 2;

	b2Body *b;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = b2_staticBody;

	b = world->CreateBody(&bd);

	b2PolygonShape ps;
	ps.SetAsBox(w / 2, h / 2 - rad);

	b2FixtureDef fd;
	fd.shape = &ps;
	fd.density = m / (w * h * 1);
	fd.friction = f;
	fd.restitution = r;

	b->CreateFixture(&fd);

	b2CircleShape cs;
	cs.m_p.Set(0, h / 2 - rad);
	cs.m_radius = rad;

	fd.shape = &cs;
	fd.density = m / (4 * b2_pi * rad * rad * rad / 3);
	b->CreateFixture(&fd);

	cs.m_p.Set(0, -h / 2 + rad);
	fd.shape = &cs;
	b->CreateFixture(&fd);

	return b;
}

b2Body* CreateCircle(b2Vec2 center, float32 rad, float32 m, float32 f, float32 r, b2BodyType bt)
{
	b2Body *b;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = bt;

	b2CircleShape cs;
	cs.m_p.Set(0, 0);
	cs.m_radius = rad;

	b2FixtureDef fd;
	fd.shape = &cs;
	fd.density = m / (4 * b2_pi * rad * rad * rad / 3);
	fd.friction = f;
	fd.restitution = r;

	b = world->CreateBody(&bd);
	b->CreateFixture(&fd);

	return b;
}

b2Body* CreateHouses(b2Vec2 center, float32 m, float32 f, float32 r, b2BodyType bt)
{
	b2Body *b;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = bt;
	b = world->CreateBody(&bd);

	b2Vec2 vertices1[5];
	vertices1[0].Set(center.x - 20, center.y + 10);
	vertices1[1].Set(center.x - 20, center.y + 0);
	vertices1[2].Set(center.x + 0, center.y + 0);
	vertices1[3].Set(center.x + 0, center.y + 10);
	vertices1[4].Set(center.x - 10, center.y + 15);

	b2Vec2 vertices2[5];
	vertices2[0].Set(center.x + 0, center.y + 10);
	vertices2[1].Set(center.x + 0, center.y + 0);
	vertices2[2].Set(center.x + 20, center.y + 0);
	vertices2[3].Set(center.x + 20, center.y + 10);
	vertices2[4].Set(center.x + 10, center.y + 15);

	float32 area = PolygonArea(vertices1, (sizeof(vertices1) / sizeof(b2Vec2))) + PolygonArea(vertices2, (sizeof(vertices2) / sizeof(b2Vec2)));

	b2PolygonShape ps;
	b2FixtureDef fd;
	fd.density = m / (area * 1);
	fd.friction = f;
	fd.restitution = r;

	ps.Set(vertices1, (sizeof(vertices1) / sizeof(b2Vec2)));
	fd.shape = &ps;
	b->CreateFixture(&fd);

	ps.Set(vertices2, (sizeof(vertices2) / sizeof(b2Vec2)));
	fd.shape = &ps;
	b->CreateFixture(&fd);

	return b;
}

b2Body* CreateLine(b2Vec2 center, b2Vec2 left, b2Vec2 right, float32 m, float32 f, float32 r, b2BodyType bt)
{
	b2Body* b;

	b2BodyDef bd;

	bd.position.Set(center.x, center.y);
	bd.type = bt;

	b2EdgeShape es;
	es.Set(left, right);

	float32 w = right.x - left.x; if (w == 0) w = 1 / INFINITY;
	float32 h = right.y - left.y; if (h == 0) h = 1 / INFINITY;

	b2FixtureDef fd;
	fd.shape = &es;
	fd.density = m / (w * h * 1);
	fd.friction = f;
	fd.restitution = r;

	b = world->CreateBody(&bd);
	b->CreateFixture(&fd);

	return b;
}

b2Body* CreateRectangle(b2Vec2 center, float32 w, float32 h, float32 m, float32 f, float32 r, b2BodyType bt)
{
	b2Body *b;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = bt;

	b2PolygonShape ps;
	ps.SetAsBox(w / 2, h / 2);

	b2FixtureDef fd;
	fd.shape = &ps;
	fd.density = m / (w * h * 1);
	fd.friction = f;
	fd.restitution = r;

	b = world->CreateBody(&bd);
	b->CreateFixture(&fd);

	return b;
}

void Delete()
{
	std::vector<b2Body*> toDelete;
	b2Body *temp;
	for (temp = world->GetBodyList(); temp; temp = temp->GetNext())
	{
		if (temp->GetFixtureList()->GetType() == b2Shape::e_polygon)
		{
			if (temp->GetPosition().y <= -50)
			{
				toDelete.push_back(temp);
			}
		}
	}
	for (std::vector<b2Body*>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		b2Body* dyingBox = *it;
		world->DestroyBody(dyingBox);
	}
	toDelete.clear();
}

void DeleteBodies()
{
	std::vector<b2Body*> toDelete;
	b2Body *temp;
	for (temp = world->GetBodyList(); temp; temp = temp->GetNext())
	{
		toDelete.push_back(temp);
	}
	for (std::vector<b2Body*>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		world->DestroyBody(*it);
	}
	toDelete.clear();
	bodies.clear();
}

void DeleteJoints()
{
	std::vector<b2Joint*> toDelete;
	b2Joint *temp;
	for (temp = world->GetJointList(); temp; temp = temp->GetNext())
	{
		toDelete.push_back(temp);
	}
	for (std::vector<b2Joint*>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		world->DestroyJoint(*it);
	}
	toDelete.clear();
	bodies.clear();
}

void InitBox2D()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world = new b2World(gravity);
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f;
}

float32 PolygonArea(b2Vec2* vertices, int size)
{
	float32 area = 0.0f;
	int i;
	for (i = 0; i < size - 1; i++)
	{
		area += (vertices[i].x * vertices[i + 1].y) - (vertices[i + 1].x * vertices[i].y);
	}
	return area / 2.0f;
}

float RandomInRange(float least, float most)
{
	return least + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (most - least)));
}

void RunBox2D()
{
	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();
}

void SetGravity(b2Vec2 gravity)
{
	world->SetGravity(gravity);
	b2Vec2 grav = world->GetGravity();
	gravityMagnitude = sqrtf(grav.x * grav.x + grav.y * grav.y);
}

b2DistanceJoint* CreateDistanceJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos1, b2Vec2 pos2, bool collide)
{
	b2DistanceJointDef dJointDef;
	dJointDef.Initialize(bodyA, bodyB, pos1, pos2);
	dJointDef.collideConnected = collide;

	b2DistanceJoint* dJoint = (b2DistanceJoint*)world->CreateJoint(&dJointDef);
	return dJoint;
}

b2RevoluteJoint* CreateRevoluteJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos, bool collide)
{
	b2RevoluteJointDef rJointDef;
	rJointDef.Initialize(bodyA, bodyB, pos);
	//rJointDef.collideConnected = true;

	//Para limitar o angulo
	//rJointDef.lowerAngle = GrausParaRadianos(-45); // -45 degrees
	//rJointDef.upperAngle =GrausParaRadianos(45); // 45 degrees
	//rJointDef.enableLimit = true;

	//Para criar um motor
	//rJointDef.maxMotorTorque = 100.0f; //N*m
	//rJointDef.motorSpeed = 10.0f; //radianos por segundo 
	//rJointDef.enableMotor = true;

	b2RevoluteJoint* rJoint = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);
	return rJoint;
}

b2PrismaticJoint* CreatePrismaticJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos, bool collide)
{
	b2Vec2 axis(0, 1);
	//Para definir um outro eixo (em termos de um vetor normalizado)
	axis = b2Vec2(1 * cos(b2_pi / 4), 1 * sin(b2_pi / 4));

	b2PrismaticJointDef pJointDef;
	pJointDef.Initialize(bodyA, bodyB, pos, axis);
	pJointDef.collideConnected = collide;

	//Para limitar a transla��o
	/*pJointDef.lowerTranslation = -5.0;
	pJointDef.upperTranslation = 5.0;
	pJointDef.enableLimit = true;*/

	//Para criar um motor
	//pJointDef.maxMotorForce = 50.0f; //N
	//pJointDef.motorSpeed = -10.0f; //metros por segundo 
	//pJointDef.enableMotor = true;

	b2PrismaticJoint* pJoint = (b2PrismaticJoint*)world->CreateJoint(&pJointDef);
	return pJoint;
}

b2PulleyJoint* CreatePulleyJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos, bool collide)
{
	b2Vec2 worldAnchorOnBody1 = bodyA->GetWorldCenter();
	b2Vec2 worldAnchorOnBody2 = bodyB->GetWorldCenter();

	b2Vec2 ground1(worldAnchorOnBody1.x, worldAnchorOnBody1.y + 10);
	b2Vec2 ground2(worldAnchorOnBody2.x, worldAnchorOnBody2.y + 10);

	b2PulleyJointDef puJointDef;
	puJointDef.Initialize(bodyA, bodyB, ground1, ground2, worldAnchorOnBody1, worldAnchorOnBody2, 1.0);
	puJointDef.collideConnected = collide;

	b2PulleyJoint* puJoint = (b2PulleyJoint*)world->CreateJoint(&puJointDef);
	return puJoint;
}

GearJoints* CreateGearJoint(b2Body *bodyA, b2Body *bodyB, b2Body *floor, b2Vec2 pos, bool collide)
{
	b2Vec2 worldAnchorOnBody1 = bodyA->GetWorldCenter();
	b2Vec2 worldAnchorOnBody2 = bodyB->GetWorldCenter();

	//Cria��o da junta revoluta
	b2RevoluteJointDef rJointDef;
	rJointDef.Initialize(bodyA, bodyB, worldAnchorOnBody1);
	//rJointDef.maxMotorTorque = 100.0f; //N-m
	//rJointDef.motorSpeed = -10.0f; //radianos por segundo 
	//rJointDef.enableMotor = true;

	b2RevoluteJoint* rJoint = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);

	//Cria��o da junta prism�tica
	b2Vec2 eixo(0, 1);
	b2PrismaticJointDef pJointDef;
	pJointDef.Initialize(floor, bodyB, worldAnchorOnBody2, eixo);
	pJointDef.collideConnected = true;
	b2PrismaticJoint* pJoint = (b2PrismaticJoint*)world->CreateJoint(&pJointDef);

	//Cria��o da junta de engrenagem
	b2GearJointDef gJointDef;
	gJointDef.bodyA = bodyA;
	gJointDef.bodyB = bodyB;
	gJointDef.joint1 = rJoint;
	gJointDef.joint2 = pJoint;
	gJointDef.ratio = b2_pi;//2 * b2_pi;
	b2GearJoint* gJoint = (b2GearJoint*)world->CreateJoint(&gJointDef);

	GearJoints* gearJoints;
	gearJoints->rJoint = rJoint;
	gearJoints->pJoint = pJoint;
	gearJoints->gJoint = gJoint;
	return gearJoints;
}

WheelJoints* CreateWheelJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 pos, bool collide)
{
	float scaleFactor = 3.0; //fator de escala para o tamanho do carro
	b2PolygonShape chassis;
	b2Vec2 vertices[8];
	vertices[0].Set(-1.5f*scaleFactor, -0.5f*scaleFactor);
	vertices[1].Set(1.5f*scaleFactor, -0.5f*scaleFactor);
	vertices[2].Set(1.5f*scaleFactor, 0.0f*scaleFactor);
	vertices[3].Set(0.0f*scaleFactor, 0.9f*scaleFactor);
	vertices[4].Set(-1.15f*scaleFactor, 0.9f*scaleFactor);
	vertices[5].Set(-1.5f*scaleFactor, 0.2f*scaleFactor);
	chassis.Set(vertices, 6);

	b2CircleShape circle;
	circle.m_radius = 0.4f*scaleFactor;

	//Posi��o do carro
	float carPosY = -20.0;
	float carPosX = -20.0;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position.Set(carPosX, carPosY);
	b2Body* car = world->CreateBody(&bd);
	car->CreateFixture(&chassis, 1.0f);

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = 1.0f;
	fd.friction = 0.9f;

	bd.position.Set(carPosX - scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body1 = world->CreateBody(&bd);
	body1->CreateFixture(&fd);

	bd.position.Set(carPosX + scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body2 = world->CreateBody(&bd);
	body2->CreateFixture(&fd);

	//Cria��o das juntas de rodas
	b2WheelJointDef jJointDef;
	b2Vec2 axis(0.0f, 1.0f);

	jJointDef.Initialize(car, body1, body1->GetPosition(), axis);
	jJointDef.motorSpeed = -30.0f;
	jJointDef.maxMotorTorque = 30.0f;
	jJointDef.enableMotor = true;
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint1 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	jJointDef.Initialize(car, body2, body2->GetPosition(), axis);
	//jJointDef.motorSpeed = 0.0f;
	//jJointDef.maxMotorTorque = 10.0f;
	//jJointDef.enableMotor = false;
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint2 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	WheelJoints* wheelJoints;
	wheelJoints->wheel1 = wheelJoint1;
	wheelJoints->wheel2 = wheelJoint2;
	return wheelJoints;
}

static void click_callback(GLFWwindow* window, int button, int action, int mode)
{
	/*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		std::cout << "Clicou esquerdo" << std::endl;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		std::cout << "Clicou meio" << std::endl;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		std::cout << "Clicou direito" << std::endl;*/
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if ((key == GLFW_KEY_1) && action == GLFW_PRESS) {
		task = 1;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_2) && action == GLFW_PRESS) {
		task = 2;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_3) && action == GLFW_PRESS) {
		task = 3;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_4) && action == GLFW_PRESS) {
		task = 4;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_5) && action == GLFW_PRESS) {
		task = 5;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_6) && action == GLFW_PRESS) {
		task = 6;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_7) && action == GLFW_PRESS) {
		task = 7;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_8) && action == GLFW_PRESS) {
		task = 8;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}

	if ((key == GLFW_KEY_KP_1) && action == GLFW_PRESS) {
		list = 1;
		task = 1;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_KP_2) && action == GLFW_PRESS) {
		list = 2;
		task = 1;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}
	if ((key == GLFW_KEY_KP_3) && action == GLFW_PRESS) {
		list = 3;
		task = 1;
		DeleteBodies();
		DeleteJoints();
		CreateBodies();
	}

	if (key == GLFW_KEY_B && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if ((10 * list + task) == 14)
		{
			b2Vec2 center = b2Vec2(0, 0);
			float32 w = 2;
			float32 h = 2;
			float32 m = RandomInRange(1, 10);
			float32 f = 1;
			float32 r = rest;
			bodies.push_back(CreateRectangle(center, w, h, m, f, r, b2_dynamicBody));
			rest += 0.1f * (2 * restDir - 1);
			if (rest <= 0.0)
			{
				restDir = !restDir;
				rest = 0.0;
			}
			else if (rest >= 1.0)
			{
				restDir = !restDir;
				rest = 1.0;
			}
		}
		else if ((10 * list + task) == 15)
		{
			b2Vec2 center = b2Vec2(0, 0);
			float32 w = 2;
			float32 h = 2;
			float32 m = RandomInRange(1, 10);
			float32 f = fric;
			float32 r = 0;
			b2Body *body = CreateRectangle(center, w, h, m, f, r, b2_dynamicBody);
			body->SetTransform(body->GetPosition(), 45 * DEGTORAD);
			bodies.push_back(body);
			fric += 0.1f * (2 * fricDir - 1);
			if (fric <= 0.0)
			{
				fricDir = !fricDir;
				fric = 0.0;
			}
			else if (fric >= 1.0)
			{
				fricDir = !fricDir;
				fric = 1.0;
			}
		}
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 13)
		{
			b2Vec2 center = b2Vec2(RandomInRange(-43, 43), RandomInRange(-30, 30));
			float32 rad = RandomInRange(1, 10);
			float32 m = RandomInRange(1, 10);
			float32 f = RandomInRange(0, 1);
			float32 r = RandomInRange(0, 1);
			bodies.push_back(CreateCircle(center, rad, m, f, r, b2_dynamicBody));
		}
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 13)
		{
			b2Vec2 center = b2Vec2(RandomInRange(-43, 43), RandomInRange(-30, 30));
			b2Vec2 left = b2Vec2(RandomInRange(-10, 10), RandomInRange(-10, 10));
			b2Vec2 right = b2Vec2(RandomInRange(-10, 10), RandomInRange(-10, 10));
			float32 m = 2;
			float32 f = 0.5;
			float32 r = 0.5;
			bodies.push_back(CreateLine(center, left, right, m, f, r, b2_staticBody));
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 13)
		{
			b2Vec2 center = b2Vec2(RandomInRange(-43, 43), RandomInRange(-30, 30));
			float32 w = RandomInRange(0, 10);
			float32 h = RandomInRange(0, 10);
			float32 m = RandomInRange(1, 10);
			float32 f = RandomInRange(0, 1);
			float32 r = RandomInRange(0, 1);
			bodies.push_back(CreateRectangle(center, w, h, m, f, r, b2_dynamicBody));
		}
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 21)
		{
			b2Vec2 g = world->GetGravity();
			b2Vec2 force = forceMultiplier * 1000.0f * sqrtf(g.x * g.x + g.y * g.y) * b2Vec2_right;
			bodies[1]->ApplyForce(force, bodies[1]->GetWorldPoint(b2Vec2(0, 20)), true);
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 22)
		{
			if (world->GetBodyCount() < 2)
			{
				float32 r = 5.0f;
				bodies.push_back(CreateCircle(b2Vec2(-39.5 + r, -39.5 + r), r, 1000.0f, 0.0f, 0.9f, b2_dynamicBody));
			}
			b2Vec2 g = world->GetGravity();
			float32 magnitude = forceMultiplier * 1000.0f * sqrtf(g.x * g.x + g.y * g.y);
			b2Vec2 force = magnitude * b2Vec2(cosf(angle * DEGTORAD), sinf(angle * DEGTORAD));
			bodies[1]->ApplyForce(force, bodies[1]->GetWorldPoint(b2Vec2(0, 0)), true);
		}
	}
	if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if ((10 * list + task) == 22)
		{
			angle += 5;
		}
	}
	if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if ((10 * list + task) == 22)
		{
			angle -= 5;
		}
	}
	if (key == GLFW_KEY_KP_MULTIPLY && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if ((10 * list + task) == 22)
		{
			forceMultiplier *= 2;
		}
	}
	if (key == GLFW_KEY_KP_DIVIDE && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if ((10 * list + task) == 22)
		{
			forceMultiplier /= 2;
			if (forceMultiplier < 1) forceMultiplier = 1;
		}
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 23)
		{
			movement.y += 1;
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 23)
		{
			movement.x -= 1;
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		if ((10 * list + task) == 23)
		{
			movement.x += 1;
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_A && action == GLFW_REPEAT)
	{
		if ((10 * list + task) == 23)
		{
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT)
	{
		if ((10 * list + task) == 23)
		{
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		if ((10 * list + task) == 23)
		{
			movement.y -= 1;
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		if ((10 * list + task) == 23)
		{
			movement.x += 1;
			movementChanged = true;
		}
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		if ((10 * list + task) == 23)
		{
			movement.x -= 1;
			movementChanged = true;
		}
	}
	if (movementChanged)
	{
		float32 movementLength = 1;//if 0, division by 0 can occur
		if (movement.x != 0 || movement.y != 0)
			movementLength = sqrtf(movement.x * movement.x + movement.y * movement.y);
		b2Vec2 movementDir = b2Vec2(movement.x / movementLength, movement.y / movementLength);
		b2Vec2 acc = world->GetGravity() + 10 * gravityMagnitude * movementDir;
		b2Vec2 force = 100 * acc;
		bodies[1]->ApplyForce(force, bodies[1]->GetWorldCenter(), true);
		movementChanged = false;
	}
}

static void move_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "Moveu mouse para (" << xpos << ", " << ypos << ")" << std::endl;
}

void CreateRobot()
{
	float scaleFactor = 8; //fator de escala para o tamanho do carro
	b2PolygonShape chassis;
	b2Vec2 vertices[8];
	vertices[0].Set(-1.5f*scaleFactor, -0.5f*scaleFactor);
	vertices[1].Set(1.5f*scaleFactor, -0.5f*scaleFactor);
	vertices[2].Set(1.5f*scaleFactor, 0.5f*scaleFactor);
	vertices[3].Set(-1.5f*scaleFactor, 0.5f*scaleFactor);
	chassis.Set(vertices, 4);

	b2CircleShape circle;
	circle.m_radius = 0.4f*scaleFactor;

	//Posi��o do carro
	float carPosX = -10.0;
	float carPosY = -10.0;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position.Set(carPosX, carPosY);
	b2Body* CreateRobott1 = world->CreateBody(&bd);
	CreateRobott1->CreateFixture(&chassis, 1.0f);

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = 1.0f;
	fd.friction = 1.0f;

	bd.position.Set(carPosX - scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body1 = world->CreateBody(&bd);
	body1->CreateFixture(&fd);

	bd.position.Set(carPosX + scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body2 = world->CreateBody(&bd);
	body2->CreateFixture(&fd);

	//Cria��o das juntas de rodas
	b2WheelJointDef jJointDef;
	b2Vec2 axis(0.0f, 1.0f);

	jJointDef.Initialize(CreateRobott1, body1, body1->GetPosition(), axis);
	jJointDef.motorSpeed = -30.0f;
	jJointDef.maxMotorTorque = 30.0f;
	jJointDef.enableMotor = true;
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint1 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	jJointDef.Initialize(CreateRobott1, body2, body2->GetPosition(), axis);
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint2 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	//Posi��o do carro
	carPosX = 10.0;
	carPosY = -10.0;

	bd.position.Set(carPosX, carPosY);
	b2Body* CreateRobott2 = world->CreateBody(&bd);
	CreateRobott2->CreateFixture(&chassis, 1.0f);

	bd.position.Set(carPosX - scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body3 = world->CreateBody(&bd);
	body3->CreateFixture(&fd);

	bd.position.Set(carPosX + scaleFactor, carPosY - 0.5f*scaleFactor);
	b2Body* body4 = world->CreateBody(&bd);
	body4->CreateFixture(&fd);

	//Cria��o das juntas de rodas
	jJointDef.Initialize(CreateRobott2, body3, body3->GetPosition(), axis);
	jJointDef.motorSpeed = -30.0f;
	jJointDef.maxMotorTorque = 30.0f;
	jJointDef.enableMotor = true;
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint3 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	jJointDef.Initialize(CreateRobott2, body4, body4->GetPosition(), axis);
	jJointDef.dampingRatio = 0.7;
	b2WheelJoint* wheelJoint4 = (b2WheelJoint*)world->CreateJoint(&jJointDef);

	b2Body* leg1 = CreateRectangle(b2Vec2(-10, 2), 24, 16, 1, 1.0, 0.0, b2_dynamicBody);
	b2Body* leg2 = CreateRectangle(b2Vec2(10, 2), 24, 16, 1, 1.0, 0.0, b2_dynamicBody);

	b2DistanceJointDef dJointDef;
	dJointDef.Initialize(CreateRobott1, leg1, CreateRobott1->GetWorldPoint(b2Vec2(-12, 4)), leg1->GetWorldPoint(b2Vec2(-12, -8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* ankle11 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(CreateRobott1, leg1, CreateRobott1->GetWorldPoint(b2Vec2(12, 4)), leg1->GetWorldPoint(b2Vec2(12, -8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* ankle12 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(CreateRobott2, leg2, CreateRobott2->GetWorldPoint(b2Vec2(-12, 4)), leg2->GetWorldPoint(b2Vec2(-12, -8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* ankle21 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(CreateRobott2, leg2, CreateRobott2->GetWorldPoint(b2Vec2(12, 4)), leg2->GetWorldPoint(b2Vec2(12, -8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* ankle22 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	b2Body* belly = CreateRectangle(b2Vec2(0, 27), 48, 32, 1, 1.0, 0.0, b2_dynamicBody);

	dJointDef.Initialize(belly, leg1, belly->GetWorldPoint(b2Vec2(-24, -16)), leg1->GetWorldPoint(b2Vec2(-12, 8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* hip11 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(belly, leg1, belly->GetWorldPoint(b2Vec2(0, -16)), leg1->GetWorldPoint(b2Vec2(-12, 8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* hip12 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(belly, leg2, belly->GetWorldPoint(b2Vec2(24, -16)), leg2->GetWorldPoint(b2Vec2(12, 8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* hip21 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(belly, leg2, belly->GetWorldPoint(b2Vec2(0, -16)), leg2->GetWorldPoint(b2Vec2(12, 8)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* hip22 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	b2Body* arm1 = CreateRectangle(b2Vec2(-36, 39), 24, 8, 1, 1.0, 0.0, b2_dynamicBody);
	b2Body* arm2 = CreateRectangle(b2Vec2(36, 39), 24, 8, 1, 1.0, 0.0, b2_dynamicBody);

	dJointDef.Initialize(belly, arm1, belly->GetWorldPoint(b2Vec2(-24, 12)), arm1->GetWorldPoint(b2Vec2(12, 0)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* shoulder1 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	dJointDef.Initialize(belly, arm2, belly->GetWorldPoint(b2Vec2(24, 12)), arm2->GetWorldPoint(b2Vec2(-12, 0)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* shoulder2 = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	bd.position.Set(-44, 31);
	b2Body* hand1 = world->CreateBody(&bd);
	hand1->CreateFixture(&fd);

	bd.position.Set(44, 31);
	b2Body* hand2 = world->CreateBody(&bd);
	hand2->CreateFixture(&fd);

	bd.position.Set(0, 46);
	b2Body* head = world->CreateBody(&bd);
	head->CreateFixture(&fd);

	dJointDef.Initialize(belly, head, belly->GetWorldPoint(b2Vec2(0, 16)), head->GetWorldPoint(b2Vec2(0, 0)));
	dJointDef.collideConnected = true;
	b2DistanceJoint* neck = (b2DistanceJoint*)world->CreateJoint(&dJointDef);

	b2RevoluteJointDef rJointDef;
	rJointDef.Initialize(arm1, hand1, hand1->GetWorldPoint(b2Vec2(0, 0)));
	rJointDef.collideConnected = false;
	rJointDef.maxMotorTorque = 100.0f;
	rJointDef.motorSpeed = 10.0f;
	rJointDef.enableMotor = true;
	b2RevoluteJoint* wrist1 = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);

	rJointDef.Initialize(arm2, hand2, hand2->GetWorldPoint(b2Vec2(0, 0)));
	rJointDef.collideConnected = false;
	rJointDef.maxMotorTorque = 100.0f;
	rJointDef.motorSpeed = 10.0f;
	rJointDef.enableMotor = true;
	b2RevoluteJoint* wrist2 = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);
}
