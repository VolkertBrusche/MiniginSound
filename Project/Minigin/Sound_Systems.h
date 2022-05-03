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
	SDLSoundSystem() = default;

	virtual ~SDLSoundSystem()
	{
		Mix_CloseAudio();
	}

	void InitializeSoundSystem() override
	{
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
		{
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
			exit(2);
		}

		m_pAudioclips.resize(m_MaxClips);
	}

	void RegisterSound(const SoundId id, const std::string& fileName)
	{

		m_pAudioclips[id] = std::make_shared<AudioClip>(fileName);
	}

	void PlaySound(const SoundId id, const float volume) override
	{
		if (id > m_pAudioclips.size())
			return;

		auto audioclip = m_pAudioclips[id];
		if (!audioclip->IsLoaded())
			audioclip->LoadSound();
		audioclip->SetVolume((int)(volume * 100));
		audioclip->PlaySound();
	}
private:
	size_t m_MaxClips{ 50 };
	std::vector<std::shared_ptr<AudioClip>> m_pAudioclips;
};

class LoggingSoundSystem final : public BaseSoundSystem
{
	std::shared_ptr<BaseSoundSystem> _real_ss;
public:
	LoggingSoundSystem(std::shared_ptr<BaseSoundSystem> ss) : _real_ss{ss}{}
	virtual ~LoggingSoundSystem() {};

	void InitializeSoundSystem() override
	{
		_real_ss->InitializeSoundSystem();
		std::cout << "Initializing sound system" << std::endl;
	}
	void RegisterSound(const SoundId id, const std::string& path) override {
		_real_ss->RegisterSound(id, path);
		std::cout << "registering sound with name: " << path << " and id: " << id << std::endl;
	}
	void PlaySound(const SoundId id, const float volume) override {
		_real_ss->PlaySound(id, volume);
		std::cout << "playing " << id << " at volume " << volume << std::endl;
	}
};

class NullSoundSystem final : public BaseSoundSystem
{
	void InitializeSoundSystem() override{}
	void PlaySound(const SoundId, const float) override{}
	void RegisterSound(const SoundId, const std::string&) override{}
};

class servicelocator final
{
public:
	static BaseSoundSystem& GetSoundSystem() { return *ss_Instance.get(); }
	static void RegisterSoundSystem(std::shared_ptr<BaseSoundSystem> ss) {
		ss_Instance = ss == nullptr ? std::make_shared<NullSoundSystem>(ss_Default) : ss;
	}

private:
	static std::shared_ptr<BaseSoundSystem> ss_Instance;
	static NullSoundSystem ss_Default;
};
