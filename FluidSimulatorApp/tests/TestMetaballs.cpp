#include "TestMetaballs.h"
#include <windows.h>
#include "Renderer.h"
#include "imgui/imgui.h"
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <functional>
#include <algorithm>


namespace test {


	TestMetaballs::TestMetaballs(unsigned int * heightPtr, unsigned int * widthPtr)
		: Test(heightPtr, widthPtr),
		m_Proj(glm::ortho(-(float)*m_WindowWidthPtr / 2, (float)*m_WindowWidthPtr / 2, -(float)*m_WindowHeightPtr / 2, (float)*m_WindowHeightPtr / 2, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))), m_StartingHeight(*m_WindowHeightPtr), m_StartingWidth(*m_WindowWidthPtr)
	{
		m_Grid = MetaballGrid(m_Rows, m_Cols, (float)*heightPtr, (float)*widthPtr);

		
		int x, y;
		for (int i = 0; i < m_DesiredParticleCount; i++)
		{
			createRandomParticle(x, y);
		}

		m_ContourShader = new Shader("res/shaders/SolidColor.shader");
		m_ContourShader->Bind();



	}

	TestMetaballs::~TestMetaballs()
	{
		for (auto& p : m_Particles) {
			p.DestroyParticle();
		}
		m_Grid.DestroyGrid();
		

	}

	void TestMetaballs::OnRender()
	{
		if (m_DisplayGrid)
			m_Grid.render(m_Proj, m_View);

		if (m_DrawCircles)
		{
			for (auto& p : m_Particles) {
				p.render(m_Proj, m_View, m_RatioH, m_RatioW, m_CircleColor);
			}
		}

		if (m_DisplayContour)
		{
			for (Edge<MetaballVertex>* e : m_ContourEdges)
			{
				e->render(m_Proj, m_View);
			}

		}

	}

	void TestMetaballs::OnImGuiRender()
	{
		ImGui::Text("----------------");
		//	int max_dim = glm::max(*m_WindowHeightPtr, *m_WindowWidthPtr);
		ImGui::Text("Metaball Menu");
		ImGui::Text("----------------");
		ImGui::Checkbox("Recreate Particles", &m_RecreateParticles);
		ImGui::Checkbox("Randomize Radius", &m_RandomizeRadius);
		ImGui::SliderInt("Num. Particles", (int*)&m_DesiredParticleCount, 1, 100);
		ImGui::SliderFloat("Speed", &m_Speed, 1e-3, 1000);
		ImGui::SliderInt("Radius", &m_ParticleRadii, 1, 500);  
		ImGui::Checkbox("ClearFields", &m_ClearFields);
		ImGui::Checkbox("Apply Field on Cells", &m_ApplyFieldOnCells);
		ImGui::Checkbox("Apply Field on Vertices", &m_ApplyFieldOnVertices);
		ImGui::Checkbox("Use Threshold", &m_FieldThreshold);
		ImGui::SliderFloat("Threshold", &m_Threshold, 1e-1, 10);
		ImGui::Checkbox("Display Circles", &m_DrawCircles);
		ImGui::Checkbox("Display Contours", &m_DisplayContour);
		ImGui::ColorEdit4("Circle Color", m_CircleColor);
		ImGui::Text("----------------");
		//	ImGui::SliderFloat2("Translation B", &m_TranslationB.x, -max_dim / 2, max_dim/2);
		ImGui::Text("Grid Menu");
		ImGui::Text("----------------");
		
		ImGui::Checkbox("Display Grid", &m_DisplayGrid);
		if (m_DisplayGrid) 
		{
			m_Grid.OnImGuiRender();
			
			ImGui::Checkbox("Modify Grid", &m_ModifyGrid);
			ImGui::InputInt("Rows", &m_DesiredRows, 1, 30);
			ImGui::InputInt("Columns", &m_DesiredColumns, 1, 30);
		}
		


		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	}

	void TestMetaballs::step(float dt)
	{
		if (m_RecreateParticles)
		{
			recreateParticles();
			m_RecreateParticles = false;
		}else
			maintainDesiredCount();

		if (m_ModifyGrid)
		{
			maintainResolution();
			m_ModifyGrid = false;
		}
		
		m_Grid.SetThresholdedField(m_FieldThreshold);
		m_Grid.SetThreshold(m_Threshold);
		//m_Grid.SetSaturationColor(m_ClearColor);
		m_Grid.ClearCellFields();
		m_Grid.ClearVertexFields();
		
		
		float maxh = m_Grid.GetGridHeight() / 2;
		float maxw = m_Grid.GetGridWidth() / 2;
		for (auto& p : m_Particles) {

			p.step(dt, m_ApplyFieldOnCells, m_ApplyFieldOnVertices);
			p.normalizeVelocity(m_Speed);
			if (p.getPosition().x > maxw) {
				p.setPosition(glm::vec2(-maxw, p.getPosition().y));
			}
			if (p.getPosition().x < -maxw) {
				p.setPosition(glm::vec2(maxw, p.getPosition().y));
			}

			if (p.getPosition().y > maxh) {
				p.setPosition(glm::vec2(p.getPosition().x, -maxh));
			}
			if (p.getPosition().y < -maxh) {
				p.setPosition(glm::vec2(p.getPosition().x, maxh));
			}


			if (m_Current == m_Period) {
				m_Current = 0;
				//set random acceleration
				p.setRandomAcceleration(10);
			}
		}
		if (m_DisplayContour)
			makeContourEdges();
		m_Current += 1;

	}

	void TestMetaballs::maintainDesiredCount()
	{
		//create some
		if (m_Particles.size() < m_DesiredParticleCount) {
			int x, y;
			for (int i = 0; i < m_DesiredParticleCount - m_Particles.size(); i++)
			{
				createRandomParticle(x, y);
			}
		}
		//destroy some
		if (m_Particles.size() > m_DesiredParticleCount) {
			int difference = m_Particles.size() - m_DesiredParticleCount;
			for (int i = 0; i < difference; i++) {
				m_Particles[m_Particles.size() - 1].DestroyParticle();
				m_Particles.pop_back();
			}

		}

	}

	void TestMetaballs::maintainResolution()
	{
		if (m_Rows != m_DesiredRows || m_Cols != m_DesiredColumns)
		{
			m_Rows = m_DesiredRows;
			m_Cols = m_DesiredColumns;

			//destroy old grid
			m_Grid.DestroyGrid();
		

			//rebuild new grid

			m_Grid = MetaballGrid(m_Rows, m_Cols, m_StartingHeight, m_StartingWidth);

		}

	}

	void TestMetaballs::createRandomParticle(int& x, int& y)
	{
		x = rand() % *m_WindowWidthPtr - *m_WindowWidthPtr / 2;
		y = rand() % *m_WindowHeightPtr - *m_WindowHeightPtr / 2;
		float r = (m_RandomizeRadius ? getRandomRadius() : m_ParticleRadii);
		Metaball p(glm::vec2(x, y), r, &m_Grid);
		p.setRandomVelocity(m_Speed);
		m_Particles.push_back(p);
	}

	void TestMetaballs::OnWindowResize()
	{
		//change projection matrix
		m_Proj = glm::ortho(-(float)*m_WindowWidthPtr / 2, (float)*m_WindowWidthPtr / 2, -(float)*m_WindowHeightPtr / 2, (float)*m_WindowHeightPtr / 2, -1.0f, 1.0f);
		m_RatioH = m_Grid.GetGridHeight() / *m_WindowHeightPtr;
		m_RatioW = m_Grid.GetGridWidth() / *m_WindowWidthPtr;

	}

	float TestMetaballs::getRandomRadius() const
	{
		float r = (float) (rand() % m_ParticleRadii);

		return m_ParticleRadii + r - (m_ParticleRadii / 2);

	}

	void TestMetaballs::recreateParticles()
	{
		m_Particles.clear();
		if (m_Particles.size() < m_DesiredParticleCount) {
			int x, y;
			for (int i = 0; i < m_DesiredParticleCount - m_Particles.size(); i++)
			{
				createRandomParticle(x, y);
			}
		}
	}


	void TestMetaballs::makeContourEdges()
	{
		//loops through each grid cells
		for (auto& edge : m_ContourEdges)
			edge->DestroyEdge();
		m_ContourEdges.clear();
		m_ContourEdges.reserve(1000);
		//apply  field to grids
		glm::vec2 center;
		glm::vec2 displacement;
		float distance;
		float field;

		

		float index = 0;
	
		auto myfun = [this](MetaballCell* cell) {
	
			//assume threshold = 1
			float index = 0;
			index += (cell->getV3()->getMetaballField() > m_Threshold) << 3;
			index += (cell->getV4()->getMetaballField() > m_Threshold) << 2;
			index += (cell->getV2()->getMetaballField() > m_Threshold) << 1;
			index += (cell->getV1()->getMetaballField() > m_Threshold) << 0;


			if (index != 0 && index != 15)
				this->makeContourEdge(index, *cell->getV1(), *cell->getV2(), *cell->getV3(), *cell->getV4()); //5 is a combination of 1 and 4, //10 is a combination of 2 and 8

			if (index == 5) {
				this->makeContourEdge(1, *cell->getV1(), *cell->getV2(), *cell->getV3(), *cell->getV4());

			}
			if (index == 10)
			{
				this->makeContourEdge(2, *cell->getV1(), *cell->getV2(), *cell->getV3(), *cell->getV4());
			}

			
		};
		
		std::vector<MetaballCell*> cells = *(m_Grid.getOneDCells());
		for_each(cells.begin(), cells.end(), myfun);
	

	}

	void TestMetaballs::makeContourEdge(int index,  MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4)
	{
		float threshold = m_Threshold;
		MetaballVertex*(*midLerpFunction)( MetaballVertex, MetaballVertex, float t) = []( MetaballVertex v1,  MetaballVertex v2, float t)
		{
			if (v1.GetPosition().x == v2.GetPosition().x)
			{
				glm::vec2 mid = glm::vec2(v1.GetPosition().x, 0);
				mid.y = v1.GetPosition().y + (v2.GetPosition().y - v1.GetPosition().y) * (t - v1.getMetaballField())/ ((v2.getMetaballField() - v1.getMetaballField()));
				 MetaballVertex* midV = new  MetaballVertex(mid, 1, false);
				return midV;
			}
			else
			{
				glm::vec2 mid = glm::vec2(0, v1.GetPosition().y);

				mid.x = v1.GetPosition().x + (v2.GetPosition().x - v1.GetPosition().x) * ( t - v1.getMetaballField()) / ((v2.getMetaballField() - v1.getMetaballField()));
				 MetaballVertex* midV = new  MetaballVertex(mid, 1, false);
				return midV;
			}
		};
		Edge<MetaballVertex> e = TestMetaballs::m_MarchingSquaresLookupTable[index](v1, v2, v3, v4, m_Threshold, m_ContourShader, midLerpFunction);
		m_ContourEdges.push_back(new Edge<MetaballVertex>(e.GetV1(), e.GetV2(), m_ContourShader, 1, true));

	}
}