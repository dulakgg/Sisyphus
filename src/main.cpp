#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	struct Fields {
        int channel = 0;
        bool playing = false;
    };
	bool init() {
		if (!GJBaseGameLayer::init()) return false;
		m_fields->channel =  FMODAudioEngine::sharedEngine()->getNextChannelID();
		return true;
	}
	
	void destroyPlayer(PlayerObject* p0, GameObject* p1) {
		int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
		int currentAttempts = this->m_attempts;
		if (currentAttempts == howManyDeaths && !playing) {
			m_fields->playing = true;
			FMODAudioEngine::sharedEngine()->playMusic("Sisyphus.mp3", true, 0.0f, channel);
		}
		GJBaseGameLayer::destroyPlayer(p0, p1);
	}
	void checkpointActivated(CheckpointGameObject* p0) {
		if (playing) {
			FMODAudioEngine::sharedEngine()->pauseMusic(channel);
			m_fields->playing = false;
		}
		GJBaseGameLayer::checkpointActivated(p0);
	}
};
