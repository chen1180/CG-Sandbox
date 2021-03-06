#include "Sandbox.h"
#include"glad/glad.h"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
namespace CGCore {

	void SandBox::OnAttach()
	{
		CG_CLIENT_INFO("App layer attached");
		m_Shader = Shader::Create(std::string("assets/shader/Debug_cube.vert.glsl"), std::string("assets/shader/Debug_cube.frag.glsl"));
		//m_phongShader= Shader::Create(std::string("assets/shader/Phong.vert.glsl"), std::string("assets/shader/Phong.frag.glsl"));
		m_SkyboxShader= Shader::Create(std::string("assets/shader/Debug_skybox.vert.glsl"), std::string("assets/shader/Debug_skybox.frag.glsl"));
		m_DepthShader= Shader::Create(std::string("assets/shader/Debug_depth.vert.glsl"), std::string("assets/shader/Debug_depth.frag.glsl"));
		auto width=(float)Application::Get().GetWindow().GetWidth();
		auto height= (float)Application::Get().GetWindow().GetHeight();  
		m_Camera = CreateRef<Camera> ( 60.0f, 0.1f, 100.0f, width / height);
		m_Camera->SetCameraControllerType(ControllerType::MayaCamera);
		//2D camera
		/*m_Camera = CreateRef<Camera>(width / height, 10.0f);
		m_Camera->SetCameraControllerType(ControllerType::Camera2D); 
		*/
		m_Mesh = ModelLoader::LoadModel("assets/mesh/cube.obj");
		m_Cube= ModelLoader::LoadModel("assets/mesh/sphere.obj");
		m_TextureCheckerBoard = Texture2D::Create("assets/texture/Checkerboard.png");
		m_TexturePig= Texture2D::Create("assets/texture/Texture.jpg");
		//cubemap assets
		m_CubeMap = TextureCube::Create("assets/texture/cubemap/skybox");
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		m_skyboxVBO = VertexBuffer::Create(skyboxVertices,sizeof(skyboxVertices));
		BufferLayout layout = { { "position",ShaderDataType::Float3 } };
		m_skyboxVBO->SetLayout(layout);
		m_skyboxVAO = VertexArray::Create();
		m_skyboxVAO->AddVertexBuffer(m_skyboxVBO);
		m_PhongRenderer = PhongRenderer();
		m_Light = CreateRef<Light>(Light(glm::vec3(1.0f) ,glm::vec3(3.0,1.0,-3.0 )));
		m_PhongRenderer.Init();
		m_PhongRenderer.SubmitLight(m_Light);
		m_PhongRenderer.SubmitMesh(m_Cube, { 2.0,-0.5,0.0 });
		m_PhongRenderer.SubmitMesh(m_Mesh, { -3.0,2.0,0.0 });
		m_PhongRenderer.SubmitMesh(m_Mesh, { 1.0,-3.0,0.0 });
		m_PhongRenderer.SubmitMesh(m_Mesh, { -2.0,-1.0,0.0 });
		//Depth buffer
		glGenFramebuffers(1, &m_DepthMapFBO);
		m_DepthMap = DepthTexture::Create((uint32_t)width, (uint32_t)height);

		glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap->GetID(), 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void SandBox::OnDettach()
	{
	}  
	void SandBox::OnUpdate(float deltaTime)
	{
		{
			auto& io=ImGui::GetIO();
			//Camera
			if (!io.WantCaptureMouse) {
				//TODO: move the following functions into event.
				m_Camera->GetController()->HandleKeyboard(m_Camera.get(), deltaTime);
				m_Camera->GetController()->HandleMouse(m_Camera.get(), deltaTime, Input::GetMousePosition().first, Input::GetMousePosition().second);
			}
		
		}
		//Depth map
		glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		m_DepthShader->Bind();
		GLfloat near_plane = -10.0f, far_plane = 10.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(m_Light->Position, glm::vec3(0.0f), glm::vec3(1.0));
		m_DepthShader->UploadUniformMat4("uLightView",   lightView);
		m_DepthShader->UploadUniformMat4("uLightProjection", lightProjection);
		m_PhongRenderer.EndScene(m_DepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		RenderCommand::Clear();
		RenderCommand::ClearColor();
		
		{
			m_PhongRenderer.BeginScene(m_Camera.get());
			
			m_DepthMap->Bind();
			m_PhongRenderer.EndScene();
		}
		{
			//Cubemap

			glDepthFunc(GL_LEQUAL);
			m_SkyboxShader->Bind();
			auto view = glm::mat4(glm::mat3(m_Camera->GetViewMatrix())); // remove translation from the view matrix
			m_SkyboxShader->UploadUniformMat4("view", view);
			m_SkyboxShader->UploadUniformMat4("projection", m_Camera->GetProjectionMatrix());
			m_CubeMap->Bind();
			m_skyboxVAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthFunc(GL_LESS);
		}

	}
	void SandBox::OnImGuiRender()
	{

		ImGui::Begin("New session"); 

		ImGui::Checkbox("Imgui Demo",&m_ShowImguiDemo);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		ImGui::Text("Framebuffer-depth:");
		auto depth = m_DepthMap->GetID();
		ImGui::Image((void*)depth, { viewportSize.x,viewportSize.y });
		if (m_ShowImguiDemo)
			ImGui::ShowDemoWindow(&m_ShowImguiDemo);
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		m_Camera->OnImGui();
		ImGui::Separator();
		ImGui::DragFloat3("Light position",&m_Light->Position[0]);
		ImGui::End();
		Renderer2D::OnImguiRender();

	}

	void SandBox::OnEvent(Event& e) {
		//TODO: move this snippet of code to Camera controller class
		//update camera resize
		if (e.GetEventType() == CGCore::EventType::WindowResize) {
			CGCore::WindowResizeEvent& event = (CGCore::WindowResizeEvent&) e;
			float aspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
			m_Camera->SetAspectRatio(aspectRatio);
		}
	}
} 