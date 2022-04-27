#include "MiniginPCH.h"
#include "TextureComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TransformComponent.h"

dae::TextureComponent::TextureComponent(std::shared_ptr<GameObject> gameObject)
	:BaseComponent{gameObject}
{
	if (gameObject)
	{
		m_TransformComponent = gameObject->GetComponent<TransformComponent>();

		if (m_TransformComponent == nullptr)
		{
			m_TransformComponent = std::make_shared<TransformComponent>(gameObject);
			gameObject->AddComponent(m_TransformComponent);
		}
	}
}

dae::TextureComponent::~TextureComponent()
{
	m_pTexture = nullptr;
}

void dae::TextureComponent::Render() const
{
	assert(m_TransformComponent);

	const auto& pos = m_TransformComponent->GetPosition();
	Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x, pos.y);
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(filename);
}
