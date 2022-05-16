#pragma once
#include <mutex>
#include "AudioClip.h"

using SoundId = unsigned short;
class BaseSoundSystem
{
public:
	virtual ~BaseSoundSystem() = default;
	virtual void InitializeSoundSystem() = 0;
	virtual void PlaySound(const SoundId id, const float volume) = 0;
	virtual void RegisterSound(const SoundId id, const std::string& path) = 0;
};

class SDLSoundSystem final : public BaseSoundSystem
{
public:
	SDLSoundSystem();
	virtual ~SDLSoundSystem();

	void InitializeSoundSystem() override;
	void RegisterSound(const SoundId id, const std::string& fileName);
	void PlaySound(const SoundId id, const float volume) override;

private:
	class SDLSoundSystemImpl;
	SDLSoundSystemImpl* pImpl;
};

class LoggingSoundSystem final : public BaseSoundSystem
{
	std::shared_ptr<BaseSoundSystem> m_RealSS;
public:
	LoggingSoundSystem(std::shared_ptr<BaseSoundSystem> ss) : m_RealSS{ss}{}
	virtual ~LoggingSoundSystem() {};

	void InitializeSoundSystem() override
	{
		m_RealSS->InitializeSoundSystem();
		std::cout << "Initializing sound system" << std::endl;
	}
	void RegisterSound(const SoundId id, const std::string& path) override {
		m_RealSS->RegisterSound(id, path);
		std::cout << "registering sound with name: " << path << " and id: " << id << std::endl;
	}
	void PlaySound(const SoundId id, const float volume) override {
		m_RealSS->PlaySound(id, volume);
		std::cout << "playing " << id << " at volume " << volume << std::endl;
	}
};

class NullSoundSystem final : public BaseSoundSystem
{
	void InitializeSoundSystem() override{}
	void PlaySound(const SoundId, const float) override{}
	void RegisterSound(const SoundId, const std::string&) override{}
};

class ServiceLocator final
{
public:
	ServiceLocator() { ss_Instance = std::make_shared<NullSoundSystem>(); }

	static BaseSoundSystem& GetSoundSystem() { return *ss_Instance.get(); }
	static void RegisterSoundSystem(std::shared_ptr<BaseSoundSystem> ss) {
		if (ss)
			ss_Instance = ss;
	}

private:
	static std::shared_ptr<BaseSoundSystem> ss_Instance;
	static NullSoundSystem ss_Default;
};
