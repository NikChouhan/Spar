#include "Application.h"


Spar::Application::Application()
{
	
}

Spar::Application::~Application()
{
	//relese recources here
}

void Spar::Application::Init()
{
	m_renderer->InitWindow();
	m_renderer->InitD3D11();

}

void Spar::Application::Render()
{

}

void Spar::Application::Update()
{

}

void Spar::Application::ShutDown()
{

}