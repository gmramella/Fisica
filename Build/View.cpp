#include "View.h"

void View::DrawFixture(b2Fixture* fixture, b2Color color)
{
	const b2Transform& xf = fixture->GetBody()->GetTransform();

	switch (fixture->GetType())
	{
	case b2Shape::e_circle:
	{
		b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();
		b2Vec2 center = b2Mul(xf, circle->m_p);
		float32 radius = circle->m_radius;
		renderer.DrawCircle(center, radius, color);
		break;
	}
	case b2Shape::e_edge:
	{
		b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
		int32 vertexCount;
		b2Vec2 vertices[b2_maxPolygonVertices];
		int i = 0;
		if (edge->m_hasVertex0)
		{
			vertices[i] = b2Mul(xf, edge->m_vertex0);
			i++;
		}
		vertices[i] = b2Mul(xf, edge->m_vertex1); i++;
		vertices[i] = b2Mul(xf, edge->m_vertex2); i++;
		if (edge->m_hasVertex3)
		{
			vertices[i] = b2Mul(xf, edge->m_vertex3);
			i++;
		}
		vertexCount = i;
		renderer.DrawPolygon(vertices, vertexCount, color);
		break;
	}
	case b2Shape::e_polygon:
	{
		b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
		int32 vertexCount = poly->m_count;
		b2Assert(vertexCount <= b2_maxPolygonVertices);
		b2Vec2 vertices[b2_maxPolygonVertices];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = b2Mul(xf, poly->m_vertices[i]);
		}
		renderer.DrawPolygon(vertices, vertexCount, color);
		break;
	}
	}
}

void View::DrawBody(b2Body *b, b2Color color)
{
	//Desenha todas as fixtures do corpo rígido
	b2Fixture *f;
	for (f = b->GetFixtureList(); f; f = f->GetNext())
	{
		DrawFixture(f, color);
	}
}

void View::Render(b2World *world)
{
	b2Body *b;
	b2Joint *j;
	glColor3f(1, 0, 0);
	glPointSize(5);
	glLineWidth(3);

	b2Color color(0 / 255, 0 / 255, 0 / 255);

	//Percorre a lista de corpos rígidos do mundo e chama a rotina de desenho para a lista de fixtures de cada um
	for (b = world->GetBodyList(); b; b = b->GetNext())
	{
		DrawBody(b, color);
	}

	for (j = world->GetJointList(); j; j = j->GetNext())
	{
		DrawJoint(j, color);
	}
}

// Função que imprime todos os objetos  
void View::PrintBodies(b2World *world)
{
	b2Body *b;
	float ang;
	b2Vec2 pos;
	for (b = world->GetBodyList(); b; b = b->GetNext())
	{
		pos = b->GetPosition();
		ang = b->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, ang);
	}
}

void View::DrawJoint(b2Joint *joint, b2Color color)
{
	b2Vec2 anchor1, anchor2;
	anchor1 = joint->GetAnchorA();
	anchor2 = joint->GetAnchorB();

	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(anchor1.x, anchor1.y);
	glVertex2f(anchor2.x, anchor2.y);
	glEnd();
	glPointSize(1);

	renderer.DrawSegment(anchor1, anchor2, color);
}