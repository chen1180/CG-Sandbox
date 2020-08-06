#include"pch.h"
#include"Texture.h"
#include"RenderAPI.h"
#include"platform/opengl/OpenGLTexture.h"
namespace CGCore {
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (RenderAPI::Get())
		{
		case RenderAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);
		case RenderAPI::API::Vulkan:
			break;
		case RenderAPI::API::DirectX:
			break;
		}
		CG_CORE_ASSERT(false, "RenderAPI::API::Unknown");
		return nullptr;
	}
}
