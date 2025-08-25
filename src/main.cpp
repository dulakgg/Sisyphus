#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fmod.hpp>
#include <cocos2d.h>

using namespace geode::prelude;

struct MusicState {
    static FMOD::System* system;
    static FMOD::Sound* sound;
    static FMOD::Sound* sound2;
    static FMOD::Sound* sound3;
    static FMOD::Sound* sound4;
    static FMOD::Channel* channel;
    static bool playing;
};

FMOD::System* MusicState::system = nullptr;
FMOD::Sound* MusicState::sound = nullptr;
FMOD::Sound* MusicState::sound2 = nullptr;
FMOD::Sound* MusicState::sound3 = nullptr;
FMOD::Sound* MusicState::sound4 = nullptr;
FMOD::Channel* MusicState::channel = nullptr;
bool MusicState::playing = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        if (!MusicState::system) {
            MusicState::system = FMODAudioEngine::sharedEngine()->m_system;
        }

        if (MusicState::system && !MusicState::sound) {
            MusicState::system->createSound("Sisyphus.mp3", FMOD_DEFAULT, nullptr, & MusicState::sound);
			MusicState::system->createSound("Sisyphus2.mp3", FMOD_DEFAULT, nullptr, & MusicState::sound2);
			MusicState::system->createSound("Sisyphus3.mp3", FMOD_DEFAULT, nullptr, & MusicState::sound3);
			MusicState::system->createSound("Sisyphus4.mp3", FMOD_DEFAULT, nullptr, & MusicState::sound4);
        }
        return true;
    }

    void checkpointActivated(CheckpointGameObject* p0) {
        if (MusicState::playing) {
            if (MusicState::channel) {
                MusicState::channel->setPosition(0, FMOD_TIMEUNIT_MS);
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
        if (MusicState::sound2) {
            MusicState::sound2->release();
            MusicState::sound2 = nullptr;
        }
        if (MusicState::sound3) {
            MusicState::sound3->release();
            MusicState::sound3 = nullptr;
        }
        if (MusicState::sound4) {
            MusicState::sound4->release();
            MusicState::sound4 = nullptr;
        }
        MusicState::playing = false;
        GJBaseGameLayer::onExit();
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void togglePracticeMode(bool practiceMode) {
        if (!MusicState::system) {
            PlayLayer::togglePracticeMode(practiceMode);
            return;
        }

        if (practiceMode) { 
            if (MusicState::channel) {
                MusicState::channel->stop();
                MusicState::channel = nullptr;
                MusicState::playing = false;
            }
            if (MusicState::sound2) {
                MusicState::system->playSound(MusicState::sound2, nullptr, true, &MusicState::channel);
                if (MusicState::channel) {
                    MusicState::channel->setPaused(false);
                    MusicState::playing = true;
                }
            }
        } else {
            if (MusicState::channel) {
                MusicState::channel->stop();
                MusicState::channel = nullptr;
            }
            MusicState::playing = false;
        }

        PlayLayer::togglePracticeMode(practiceMode);
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
        int currentAttempts = this->m_attempts;
        if (howManyDeaths > 0 && currentAttempts % howManyDeaths == 0 && !MusicState::playing) {
            if (MusicState::system && MusicState::sound) {
                MusicState::system->playSound(MusicState::sound, nullptr, true, &MusicState::channel);
                if (MusicState::channel) {
                    MusicState::channel->setPaused(false);
                    MusicState::playing = true;
                }
            }
        }
        PlayLayer::destroyPlayer(p0, p1);
    }
};
