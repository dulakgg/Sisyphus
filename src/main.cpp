#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

struct MusicState {
    static int channel;
    static bool playing;
};

int MusicState::channel = 0;
bool MusicState::playing = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	bool init() {
		if (!GJBaseGameLayer::init()) return false;
		return true;
	}
	void checkpointActivated(CheckpointGameObject* p0) {
		if (MusicState::playing) {
			FMODAudioEngine::sharedEngine()->pauseMusic(MusicState::channel);
			MusicState::playing = false;
		}
		GJBaseGameLayer::checkpointActivated(p0);
	}
};
class $modify(MyPlayLayer, PlayLayer) {
	void destroyPlayer(PlayerObject* p0, GameObject* p1) {
		int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
		int currentAttempts = this->m_attempts;
		if (currentAttempts == howManyDeaths && MusicState::playing == false) {
			MusicState::playing = true;
			FMODAudioEngine::sharedEngine()->playMusic("Sisyphus.mp3", true, 0.0f, MusicState::channel);
		}
		PlayLayer::destroyPlayer(p0, p1);
	}
};