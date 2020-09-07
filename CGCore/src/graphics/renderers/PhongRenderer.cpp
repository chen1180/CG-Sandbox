#include "pch.h"
#include "PhongRenderer.h"
#include"GLFW/glfw3.h"
namespace CGCore {
	struct CommandBuffer {
		Ref<Mesh> Mesh=nullptr;
		glm::vec3 Position;
		glm::vec3 Size;
		CommandBuffer() = default;

	};
	struct PhongRendererData {
		static const uint32_t MaxMeshCount = 100;
		static const uint32_t MaxVertex = MaxMeshCount * 10000;
		static const uint32_t MaxIndices = MaxVertex * 3;
		std::vector<CommandBuffer> CommandBuffer;
		std::vector<Ref<Light>> LightSources;
		Ref<Shader> PhongShader;

		PhongRendererData() = default;
		~PhongRendererData() {
		}
	};

	PhongRendererData* PhongRenderer::s_PhongRenderData = nullptr;
	void PhongRenderer::Init()
	{
		s_PhongRenderData = new PhongRendererData();
		s_PhongRenderData->PhongShader= Shader::Create(std::string("assets/shader/Phong.vert.glsl"), std::string("assets/shader/Phong.frag.glsl"));
		s_PhongRenderData->CommandBuffer.reserve(100);
		s_PhongRenderData->LightSources.reserve(32);
	}

	void PhongRenderer::ShutDown()
	{
		delete s_PhongRenderData;
	}

	void PhongRenderer::BeginScene(Camera* camera)
	{
		s_PhongRenderData->PhongShader->Bind();
		s_PhongRenderData->PhongShader->UploadUniformMat4("uView", camera->GetViewMatrix());
		s_PhongRenderData->PhongShader->UploadUniformMat4("uProjection", camera->GetProjectionMatrix());
		s_PhongRenderData->PhongShader->UploadUniformFloat3("viewPos", camera->GetPosition());
		
	
	}
	void PhongRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::vec3& position,const glm::vec3& size)
	{
		CommandBuffer buffer;
		buffer.Mesh = mesh;
		buffer.Position = position;
		buffer.Size = size;
		s_PhongRenderData->CommandBuffer.emplace_back(buffer);
	}
	void PhongRenderer::SubmitLight(const Ref<Light>& light)
	{
		s_PhongRenderData->LightSources.emplace_back(light);
	}

	void PhongRenderer::EndScene()
	{
		s_PhongRenderData->PhongShader->Bind();
		for (auto& light : s_PhongRenderData->LightSources) {
			//TODO: remove after shadowmap test
			GLfloat near_plane = 0.1f, far_plane = 100.0f;
			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(light->Position, glm::vec3(0.0f), glm::vec3(1.0));
			s_PhongRenderData->PhongShader->UploadUniformMat4("uLightView", lightView);
			s_PhongRenderData->PhongShader->UploadUniformMat4("uLightProjection", lightProjection);
			s_PhongRenderData->PhongShader->UploadUniformFloat3("lightPos", light->Position);
			s_PhongRenderData->PhongShader->UploadUniformFloat3("lightColor", light->Color);
		}
		for (auto& command : s_PhongRenderData->CommandBuffer) {
			s_PhongRenderData->PhongShader->UploadUniformMat4("uModel", glm::translate(glm::mat4(1.0f), command.Position)*glm::scale(glm::mat4(1.0f),command.Size));
			command.Mesh->Draw(); 
		}
	}
	void PhongRenderer::EndScene(Ref<Shader> shader)
	{
		//TODO: for temperory test shadow map
		for (auto& command : s_PhongRenderData->CommandBuffer) {
			shader->UploadUniformMat4("uModel", glm::translate(glm::mat4(1.0f), command.Position) * glm::scale(glm::mat4(1.0f), command.Size));
			command.Mesh->Draw();
		}
	}
}

