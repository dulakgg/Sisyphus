#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/Geode.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
public:
	bool init() {
		if (!GJBaseGameLayer::init()) return false;
		channel =  FMODAudioEngine::sharedEngine()->getNextChannelID();
		return true;
	}
	void destroyPlayer(PlayerObject* p0, GameObject* p1) {
		int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
		int currentAttempts = this->m_attempts.get();
		if (currentAttempts == howManyDeaths && !playing) {
			playing = true;
			FMODAudioEngine::sharedEngine()->playMusic("Sisyphus.mp3", true, 0.0f, channel);
		}
		GJBaseGameLayer::destroyPlayer(p0, p1);
	}
	void checkpointActivated(CheckpointGameObject* p0) {
		if (playing) {
			FMODAudioEngine::sharedEngine()->pauseMusic(channel);
			playing = false;
		}
		GJaseGameLayer::checkpointActivated(p0);
	}
};
