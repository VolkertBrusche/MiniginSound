#pragma once
#include "BaseComponent.h"

namespace dae
{
    class GameObject;
    class TextComponent;
    class Font;
    class FPSComponent final : public BaseComponent, public std::enable_shared_from_this<FPSComponent>
    {
    public:
        FPSComponent(const std::string& text, const std::shared_ptr<Font>& font, std::shared_ptr<GameObject> gameObject);
        virtual ~FPSComponent();

        virtual void Update(float deltaTime) override;
        virtual void Render() const override;

        int GetFPS() const;
        
    private:
        int m_FPS{0};
        std::shared_ptr<TextComponent> m_TextComponent{};
    };
}

