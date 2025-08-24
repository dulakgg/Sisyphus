#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/Geode.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	bool init() {
		if (!GJBaseGameLayer::init()) return false;
		return true;
	}
	channel =  FMODAudioEngine::sharedEngine()->getNextChannelID();
	bool playing = false;
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
		GJBaseGameLayer::checkpointActivated(p0);
	}
};
