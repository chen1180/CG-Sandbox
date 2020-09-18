#include "EditorLayer.h"
#include"glad/glad.h"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"platform/opengl/OpenGLFrameBuffer.h"
namespace CGCore {

	void EditorLayer::OnAttach()
	{
		CG_CLIENT_INFO("App layer attached");
		m_Shader = Shader::Create(std::string("assets/shader/Debug_cube.vert.glsl"), std::string("assets/shader/Debug_cube.frag.glsl"));
		m_DepthShader= Shader::Create(std::string("assets/shader/Debug_depth.vert.glsl"), std::string("assets/shader/Debug_depth.frag.glsl"));
		auto width=(float)Application::Get().GetWindow().GetWidth();
		auto height= (float)Application::Get().GetWindow().GetHeight();  
		m_Camera = CreateRef<Camera> ( 60.0f, 0.1f, 100.0f, width / height);
		m_Camera->SetCameraControllerType(ControllerType::MayaCamera);
		//2D camera
		/*m_Camera = CreateRef<Camera>(width / height, 10.0f);
		m_Camera->SetCameraControllerType(ControllerType::Camera2D); 
		*/
		
		m_Scene = CreateRef<Scene>();

		auto cube = ModelLoader::LoadModel("assets/mesh/cube.obj", m_Scene.get());
		auto& transform1 = cube.GetComponent<TransformComponent>();
		transform1.SetWorldMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0, 2.0, 0.0)));

		auto sphere= ModelLoader::LoadModel("assets/mesh/sphere.obj", m_Scene.get());
		auto& transform2 = sphere.GetComponent<TransformComponent>();
		transform2.SetWorldMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(5.0, 0.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f,0.1f,0.1f)));

	//	auto skull= ModelLoader::LoadModel("assets/mesh/12140_Skull_v3_L2.obj", m_Scene.get());
		//transform = sphere.GetComponent<TransformComponent>();
		//transform.SetWorldMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(5.0, 5.0, 0.0))*glm::scale(glm::mat4(1.0f),glm::vec3(0.1f)));

		//TODO: Add texture component
		m_TextureCheckerBoard = Texture2D::Create("assets/texture/Checkerboard.png");
		m_TexturePig= Texture2D::Create("assets/texture/Texture.jpg");
	
		m_PhongRenderer = PhongRenderer();
		m_Light = CreateRef<Light>(Light(glm::vec3(1.0f) ,glm::vec3(3.0,1.0,-3.0 )));
		m_PhongRenderer.Init();
		m_PhongRenderer.SubmitLight(m_Light);
		
		//TODO: move the following function to renderer
		auto& registry=m_Scene->GetRegistry();
		auto view = registry.view<MeshComponent,TransformComponent>();

		for (auto entity : view) {
			// a component at a time ...
			auto& meshcomponent = view.get<MeshComponent>(entity);
			auto& transformComponent = view.get<TransformComponent>(entity);
			auto posiiton = transformComponent.GetWorldPosition();
			auto scale = transformComponent.GetWorldScale();
			m_PhongRenderer.SubmitMesh(meshcomponent.Meshes, posiiton, scale);
		}
		
		SkyboxRenderer::Init();

	}
	void EditorLayer::OnDettach()
	{
	}  
	void EditorLayer::OnUpdate(float deltaTime)
	{
		framerate = deltaTime;
		RenderCommand::Clear();
		RenderCommand::ClearColor();

		{
			m_PhongRenderer.BeginScene(m_Camera.get());
			m_PhongRenderer.EndScene();
		}

		{
			//skybox
			SkyboxRenderer::BeginScene(m_Camera.get());
			SkyboxRenderer::EndScene();
		}
		m_PhongRenderer.GetFrameBuffer()->Unbind();

	}
	void EditorLayer::OnImGuiRender()
	{
		static bool showWindow = true;

		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &showWindow, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
			ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::SmallButton("click here"))
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Docking"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				if (ImGui::MenuItem("Close DockSpace", NULL, false, &showWindow != NULL))
					showWindow = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();

		ImGui::Begin("New session");
		ImGui::Checkbox("Imgui Demo", &m_ShowImguiDemo);
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Text("Framebuffer-depth:");
		auto depth = ShadowRenderer::GetShadowMap()->GetID();
		ImGui::Image((void*)depth, { viewportSize.x,viewportSize.y });
		ImGui::Text("x: %.0f y: %.0f", viewportSize.x, viewportSize.y);

		if (m_ShowImguiDemo)
			ImGui::ShowDemoWindow(&m_ShowImguiDemo);
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		m_Camera->OnImGui();
		ImGui::Separator();
		ImGui::DragFloat3("Light position", &m_Light->Position[0]);
		ImGui::End();
		Renderer2D::OnImguiRender();

		ImGui::Begin("GameWindow");
		{
			// Using a Child allow to fill all the space of the window.
			// It also alows customization
			ImGui::BeginChild("GameRender");
			//Camera
			if (ImGui::IsWindowHovered()) {
				//TODO: move the following functions into event.
				m_Camera->GetController()->HandleKeyboard(m_Camera.get(), framerate);
				m_Camera->GetController()->HandleMouse(m_Camera.get(), framerate, Input::GetMousePosition().first, Input::GetMousePosition().second);
			}

			// Get the size of the child (i.e. the whole draw size of the windows).
			ImVec2 wsize = ImGui::GetWindowSize();
			// Because I use the texture from OpenGL, I need to invert the V from the UV.
			glm::vec2 m_FrameBufferSize = { m_PhongRenderer.GetFrameBuffer()->GetSpecification().Width, m_PhongRenderer.GetFrameBuffer()->GetSpecification().Height };
			if (m_FrameBufferSize != *(glm::vec2*) & wsize) {
				CG_CLIENT_INFO("Viewport size: {0} ,{1}", wsize.x, wsize.y);
				m_PhongRenderer.GetFrameBuffer()->Resize(wsize.x, wsize.y);
				m_Camera->SetAspectRatio(wsize.x / wsize.y);

			}
			auto texture = static_cast<OpenGLFrameBuffer*>(m_PhongRenderer.GetFrameBuffer().get());
			ImGui::Image((void*)texture->GetColor(), wsize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();


	}

	void EditorLayer::OnEvent(Event& e) {
		//TODO: move this snippet of code to Camera controller class
		//update camera resize
		if (e.GetEventType() == CGCore::EventType::WindowResize) {
			CGCore::WindowResizeEvent& event = (CGCore::WindowResizeEvent&) e;
		}
	}
} 