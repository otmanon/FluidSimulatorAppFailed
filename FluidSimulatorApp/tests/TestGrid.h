#pragma once
#include "Grid.h"
#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Vertex.h"
#include "Edge.h"
#include "Cell.h"
#include <memory>
namespace test {

	class TestGrid : public Test {

	private:
			   
		Grid<Vertex, Edge<Vertex>, Cell<Vertex, Edge<Vertex>>> m_Grid;
		unsigned int m_StartingHeight;
		unsigned int m_StartingWidth;
		
		int m_Rows = 10;//keeping the ration 10x17 will keep approximately square grid points
		int m_Cols = 17;

		int m_DesiredRows = 10;
		int m_DesiredColumns = 17;

		glm::mat4 m_Proj;
		glm::mat4 m_View;

		bool m_DisplayGrid = true;
		bool m_ModifyGrid = false;

	public:
		TestGrid(unsigned int * heightPtr, unsigned int * widthPtr);
		~TestGrid();


		void OnRender() override;

		void OnImGuiRender() override;

		void step(float dt) override;

		void maintainResolution();

		void OnWindowResize() override;

	

	};

	

}

