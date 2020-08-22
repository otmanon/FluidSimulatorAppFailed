#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Rectangle.h"
#include <memory>
namespace test {
	class TestCircle : public Test {

	private:

		


		glm::mat4 m_Proj, m_View;
		VertexArray* m_VAO;
		VertexBuffer* m_VertexBuffer;
		IndexBuffer* m_IndexBuffer;
		Shader* m_Shader;

	public:
		TestCircle(unsigned int * heightPtr, unsigned int * widthPtr);
		//	~TestCircle();


		void OnRender() override;
		void OnImGuiRender() override;


	};



}

