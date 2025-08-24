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
	
	void createPlayer() {
		int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
		int currentAttempts = this->m_attempts;
		if (currentAttempts == howManyDeaths && m_fields->playing == false) {
			m_fields->playing = true;
			FMODAudioEngine::sharedEngine()->playMusic("Sisyphus.mp3", true, 0.0f, m_fields->channel);
		}
		GJBaseGameLayer::createPlayer();
	}
	void checkpointActivated(CheckpointGameObject* p0) {
		if (m_fields->playing) {
			FMODAudioEngine::sharedEngine()->pauseMusic(m_fields->channel);
			m_fields->playing = false;
		}
		GJBaseGameLayer::checkpointActivated(p0);
	}
};
