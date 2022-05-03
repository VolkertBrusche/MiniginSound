#include "MiniginPCH.h"
#include "Sound_Systems.h"

std::shared_ptr<BaseSoundSystem> servicelocator::ss_Instance = std::make_shared<NullSoundSystem>(ss_Default);
NullSoundSystem servicelocator::ss_Default;