#include "MiniginPCH.h"
#include "Minigin.h"
#include <thread>
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"

//Game Components ================
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "FPSComponent.h"
//================================

#include "Commands.h"

using namespace std;

void PrintSDLVersion()
{
	SDL_version compiled{};
	SDL_version linked{};

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("We compiled against SDL version %d.%d.%d ...\n",
		compiled.major, compiled.minor, compiled.patch);
	printf("We are linking against SDL version %d.%d.%d.\n",
		linked.major, linked.minor, linked.patch);
}

void dae::Minigin::Initialize()
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	m_Window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(m_Window);
}

/**
 * Code constructing the scene world starts here
 */
void dae::Minigin::LoadGame() const
{
	std::shared_ptr<Scene> scene = SceneManager::GetInstance().CreateScene("Demo");

	CreateBackground();
	CreateFPSCounter();
	TestParenting();
	TestInput();
	TestSound();
}

void dae::Minigin::Cleanup()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run()
{
	Initialize();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");

	LoadGame();

	{
		auto& renderer = Renderer::GetInstance();
		auto& sceneManager = SceneManager::GetInstance();
		auto& input = InputManager::GetInstance();

		// todo: this update loop could use some work.
		auto lastTime = chrono::high_resolution_clock::now();
		float deltaTime{};

		bool doContinue = true;
		float lag = 0.0f;
		while (doContinue)
		{
			const auto currentTime = chrono::high_resolution_clock::now();
			deltaTime = chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;
			lag += deltaTime;

			doContinue = input.ProcessInput();
			sceneManager.Update(deltaTime);

			//SteamAPI_RunCallbacks();

			while (lag >= m_TimeStamp)
			{
				sceneManager.FixedUpdate(m_TimeStamp);
				lag -= m_TimeStamp;
			}
			renderer.Render();

			auto sleepTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime + std::chrono::milliseconds(MsPerFrame) - std::chrono::high_resolution_clock::now());
			std::this_thread::sleep_for(sleepTime);
		}
	}

	Cleanup();
}

void dae::Minigin::CreateBackground() const
{
	std::shared_ptr<Scene> scene = SceneManager::GetInstance().GetSceneByName("Demo");

	if (scene)
	{
		std::shared_ptr<GameObject> backgroundObject = std::make_shared<GameObject>();
		std::shared_ptr<TextureComponent> textureComponent = std::make_shared<TextureComponent>(backgroundObject);
		textureComponent->SetTexture("background.jpg");
		backgroundObject->AddComponent(textureComponent);

		std::shared_ptr<GameObject> logoObject = std::make_shared<GameObject>();
		textureComponent = std::make_shared<TextureComponent>(logoObject);
		textureComponent->SetTexture("logo.png");
		logoObject->AddComponent(textureComponent);
		logoObject->GetComponent<TransformComponent>()->SetPosition(216, 180);
		scene->Add(logoObject);

		std::shared_ptr<GameObject> titleObject = std::make_shared<GameObject>();
		auto titleFont = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
		std::shared_ptr<TextComponent> textComponent = std::make_shared<TextComponent>("Programming 4 Assignment", titleFont, titleObject);
		titleObject->AddComponent(textComponent);
		titleObject->GetComponent<TransformComponent>()->SetPosition(80, 20);
		scene->Add(titleObject);
	}
}

void dae::Minigin::CreateFPSCounter() const
{
	std::shared_ptr<Scene> scene = SceneManager::GetInstance().GetSceneByName("Demo");

	if (scene)
	{
		std::shared_ptr<GameObject> FPSObject = std::make_shared<GameObject>();
		auto FPSFont = ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);
		std::shared_ptr<FPSComponent> fpsComponent = std::make_shared<FPSComponent>("00 FPS", FPSFont, FPSObject);
		FPSObject->AddComponent(fpsComponent);
		FPSObject->GetComponent<TextComponent>()->SetColor({ 255,255,0 });
		FPSObject->GetComponent<TransformComponent>()->SetPosition(10, 10);
		scene->Add(FPSObject);
	}
}

void dae::Minigin::TestParenting() const
{
	std::shared_ptr<GameObject> go1 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> go2 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> go3 = std::make_shared<GameObject>();

	go1->SetParent(go2);
	go2->SetParent(go3);
	go2->SetParent(go1);
	go2->SetParent(nullptr);
	go1->SetParent(nullptr);
	go3->SetParent(go1);
	go3->SetParent(nullptr);
}

void dae::Minigin::TestInput() const
{
	auto& input = InputManager::GetInstance();

	input.SetButtonCommand(0, XBox360Controller::ControllerButton::ButtonA, new JumpCommand(), CommandState::Down);
	input.SetButtonCommand(0, XBox360Controller::ControllerButton::ButtonB, new FireCommand(), CommandState::Down);
	input.SetButtonCommand(0, XBox360Controller::ControllerButton::ButtonX, new CrouchCommand(), CommandState::Down);
	input.SetButtonCommand(0, XBox360Controller::ControllerButton::ButtonY, new SwapGunCommand(), CommandState::Down);
	input.SetButtonCommand(0, XBox360Controller::ControllerButton::DPadLeft, new SwapGunCommand(), CommandState::Down);
	input.RemoveButtonCommand(0, XBox360Controller::ControllerButton::DPadLeft);
}

void dae::Minigin::TestSound() const
{
#if _DEBUG
	ServiceLocator::RegisterSoundSystem(std::make_shared<LoggingSoundSystem>(std::make_shared<SDLSoundSystem>()));
#else
	ServiceLocator::RegisterSoundSystem(std::make_shared<SDLSoundSystem>());
#endif

	ServiceLocator::GetSoundSystem().InitializeSoundSystem();
	ServiceLocator::GetSoundSystem().RegisterSound(0, "../Data/15_Jingle_06.wav");

	auto& input = InputManager::GetInstance();
	input.SetButtonCommand(0, XBox360Controller::ControllerButton::DPadUp, new PlaySound(), CommandState::Down);
}
