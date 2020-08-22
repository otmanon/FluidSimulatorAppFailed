#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Circle.h"
#include "Particle.h"
#include "MetaballGrid.h"
#include "Metaball.h"
#include "MetaballVertex.h"
#include "MetaballEdge.h"
#include "MetaballCell.h"
#include <memory>
namespace test {

	class TestMetaballs : public Test {

	private:


		MetaballGrid m_Grid;
		unsigned int m_StartingHeight;
		unsigned int m_StartingWidth;
		
		int m_Rows = 10;//keeping the ration 10x17 will keep approximately square grid points
		int m_Cols = 17;

		int m_DesiredRows = 30;
		int m_DesiredColumns = 55;

		glm::mat4 m_Proj;
		glm::mat4 m_View;

		bool m_DisplayGrid = true;
		bool m_ModifyGrid = false;
		bool m_DisplayGridVertices = false;

		float m_RatioH = 1, m_RatioW = 1;
		std::vector<Metaball> m_Particles;

		unsigned int m_Period = 10;
		unsigned int m_Current = 0;
		unsigned int m_DesiredParticleCount = 10;

		float m_Speed = 30;

		int m_ParticleRadii = 100;

		bool m_RecreateParticles = false;
		bool m_RandomizeRadius = true;

		bool m_ApplyFieldOnCells = false;
		bool m_ApplyFieldOnVertices = true;

		bool m_DrawCircles = true;

		bool m_FieldThreshold = false;
		float m_Threshold = 1;

		float m_SaturationColor[4] = { 1.0f, 0.611f, 0.936f, 1.0f };
		float m_CircleColor[4] = { 0.5f, 0.1f, 0.5f, 1.0f };
		bool m_ClearFields = false;

		bool m_DisplayContour = false;
		std::vector<Edge<MetaballVertex>*> m_ContourEdges;

		Shader * m_ContourShader;

		
		public: std::vector<std::function<Edge<MetaballVertex>( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader* shader,  MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t))>> m_MarchingSquaresLookupTable =
		{
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) {  //0000
				return Edge<MetaballVertex>();
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0001
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v1, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0010
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v2, v4, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0011
				return Edge<MetaballVertex>(midFun(v1, v3, t), midFun(v2, v4, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0100
				return Edge<MetaballVertex>(midFun(v2, v4, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0101   //5 is the same as 4 by default... then we'll call 1 to combine them additionally
				return Edge<MetaballVertex>(midFun(v2, v4, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0110  
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //0111 
				return Edge<MetaballVertex>(midFun(v1, v3, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1000 
				return Edge<MetaballVertex>(midFun(v1, v3, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1001
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v3, v4, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1010 same as 8 combined with 2
				return Edge<MetaballVertex>(midFun(v1, v3, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1011
				return Edge<MetaballVertex>(midFun(v2, v4, t), midFun(v4, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1100
				return Edge<MetaballVertex>(midFun(v1, v3, t), midFun(v2, v4, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1101
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v2, v4, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader,MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1110
				return Edge<MetaballVertex>(midFun(v1, v2, t), midFun(v1, v3, t), 1);
			},
			[]( MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4, float t, Shader * shader, MetaballVertex*(*midFun)( MetaballVertex v1,  MetaballVertex v2, float t)) { //1111
				return Edge<MetaballVertex>();
			}

		};

	public:
		TestMetaballs(unsigned int * heightPtr, unsigned int * widthPtr);
		~TestMetaballs();


		void OnRender() override;

		void OnImGuiRender() override;

		void step(float dt) override;

		void maintainDesiredCount();

		void maintainResolution();

		void createRandomParticle(int& x, int& y);

		void OnWindowResize() override;

		float getRandomRadius() const;

		void recreateParticles();


		void makeContourEdges();

		void makeContourEdge(int index,  MetaballVertex v1,  MetaballVertex v2,  MetaballVertex v3,  MetaballVertex v4);

		Vertex* getMidPoint( MetaballVertex v1,  MetaballVertex v2)
		{
			glm::vec2 mid = (v1.GetPosition() + v2.GetPosition()) / 2.0f;
			Vertex* midV = new Vertex(mid, 1);
			return midV;
		}

	
	};



}

