#pragma once
#include <memory>
#include "SDL_mixer.h"

using sound_id = unsigned short;
class base_sound_system
{
public:
	virtual ~base_sound_system() = default;
	virtual void play(const sound_id id, const float volume) = 0;
	virtual void register_sound(const sound_id id, const std::string& path) = 0;
};

class sdl_sound_system final : public base_sound_system
{
public:
	void register_sound(const sound_id id, const std::string& path) //https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_19.html#SEC19 source of code in this function
	{
		Mix_Chunk* sample;
		sample = Mix_LoadWAV(path.c_str());
		if (!sample)
			printf("Mix_LoadWav: %s\n", Mix_GetError());
	}

	void play(const sound_id id, const float volume) override
	{
		auto audioclip = audioclips[id];
		if (!audioclip->is_loaded())
			audioclip->load();
		audioclip->set_volume((int)(volume * 100));
		audioclip->play();
	}
private:
	std::vector<Mix_Chunk*> audioclips;
};

class logging_sound_system final : public base_sound_system
{
	base_sound_system* _real_ss;
public:
	logging_sound_system(base_sound_system* ss) : _real_ss{ss}{}
	~logging_sound_system() { delete _real_ss; }

	void register_sound(const sound_id id, const std::string& path) override {
		_real_ss->register_sound(id, path);
	}
	void play(const sound_id id, const float volume) override {
		_real_ss->play(id, volume);
		std::cout << "playing " << id << " at volume " << volume << std::endl;
	}
};

class null_sound_system final : public base_sound_system
{
	void play(const sound_id, const float) override{}
};

class servicelocator final
{
public:
	static base_sound_system& get_sound_system() { return *ss_Instance.get(); }
	static void register_sound_system(std::shared_ptr<base_sound_system> ss) { 
		ss_Instance = ss == nullptr ? _default_ss : ss; 
	}

private:
	static std::shared_ptr<base_sound_system> ss_Instance;
	static std::shared_ptr<null_sound_system> _default_ss;
};
std::shared_ptr<base_sound_system> servicelocator::ss_Instance = _default_ss;
std::shared_ptr<null_sound_system> servicelocator::_default_ss;
