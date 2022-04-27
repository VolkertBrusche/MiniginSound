#pragma once
struct SDL_Window;
namespace dae
{
	class Minigin
	{
	public:
		void Initialize();
		void LoadGame() const;
		void Cleanup();
		void Run();
	private:
		void CreateBackground() const;
		void CreateFPSCounter() const;
		void TestParenting() const;
		void TestInput() const;

		static const int MsPerFrame = 16; //16 for 60 fps, 33 for 30 fps
		float m_TimeStamp = 0.02f; //Fixed timestamp for FixedUpdate() (default from UnityEngine)
		SDL_Window* m_Window{};
	};
}