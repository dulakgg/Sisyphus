#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fmod.hpp>

using namespace geode::prelude;

struct MusicState {
	static FMOD::System* system;
	static FMOD::Sound* sound;     
 static FMOD::Channel* channel;
 static bool playing;
};

FMOD::System* MusicState::system = nullptr;
FMOD::Sound* MusicState::sound = nullptr;
FMOD::Channel* MusicState::channel = nullptr;
bool MusicState::playing = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	bool init() {
		if (!GJBaseGameLayer::init()) return false;
		if (!MusicState::system) {
	  MusicState::system = FMODAudioEngine::sharedEngine()->m_system;
  }
		if (!MusicState::sound) {
	  MusicState::system->createSound("Sisyphus.mp3", FMOD_DEFAULT, nullptr, &MusicState::sound);
		}
		return true;
	}
	void checkpointActivated(CheckpointGameObject* p0) {
		if (MusicState::playing) {
			if (MusicState::channel) {
    MusicState::channel->setPosition(0, FMOD_TIMEUNIT_PCM);
				MusicState::channel->setPaused(true);
			}
			MusicState::playing = false;
		}
		GJBaseGameLayer::checkpointActivated(p0);
	}
	void onExit() {
  if (MusicState::channel) {
   MusicState::channel->stop();
	  MusicState::channel = nullptr;
  }
  if (MusicState::sound) {
   MusicState::sound->release();
	  MusicState::sound = nullptr;
  }
  MusicState::playing = false;
	 GJBaseGameLayer::onExit();
	}
};
class $modify(MyPlayLayer, PlayLayer) {
	void destroyPlayer(PlayerObject* p0, GameObject* p1) {
		int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
		int currentAttempts = this->m_attempts;
		if (currentAttempts % howManyDeaths == 0 && !MusicState::playing) {
			if (MusicState::system && MusicState::sound) {
		  MusicState::system->playSound(MusicState::sound, nullptr, true, &MusicState::channel);
				MusicState::channel->setLoopCount(-1); 
    MusicState::channel->setPaused(false); 
    MusicState::playing = true;
	  }
		}
		PlayLayer::destroyPlayer(p0, p1);
	}
};