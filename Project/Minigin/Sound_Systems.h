#pragma once
#include <memory>

using sound_id = unsigned short;
class base_sound_system
{
public:
	virtual ~base_sound_system() = default;
	virtual void Play(const sound_id id, const float volume) = 0;
};

class sdl_sound_system final : public base_sound_system
{
public:
	void Play(const sound_id id, const float volume) override
	{
		UNREFERENCED_PARAMETER(id);
		UNREFERENCED_PARAMETER(volume);
	}
};

class servicelocator final
{
public:
	static base_sound_system& get_sound_system() { return *ss_Instance.get(); }
	static void register_sound_system(std::shared_ptr<base_sound_system> ss) { ss_Instance = ss; }

private:
	static std::shared_ptr<base_sound_system> ss_Instance;
};
std::shared_ptr<base_sound_system> servicelocator::ss_Instance{};
