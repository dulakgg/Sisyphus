#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <fmod.hpp>
#include <cocos2d.h>

using namespace geode::prelude;
struct MusicState   {
    static bool playing;
    static bool isPlatformer;
    static float idleTimer;
    static float howLongToFade;
    static bool fadingImage;
    static CCSprite* overlaySprite;
    static float howLongIdling;
    static int effectId;
};

int MusicState::effectId = -1;
float MusicState::howLongIdling = Mod::get()->getSettingValue<float>("time-idling");
float MusicState::howLongToFade = Mod::get()->getSettingValue<float>("time-to-fade");
float MusicState::idleTimer = 0;
bool MusicState::fadingImage = false;
CCSprite* MusicState::overlaySprite = nullptr;
bool MusicState::isPlatformer = false;
bool MusicState::playing = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
    bool init() {
        if (!GJBaseGameLayer::init()) return false;
        if (this->m_isPlatformer) {
            MusicState::isPlatformer = true;
        }
        else  {
            MusicState::isPlatformer = false;
        }
        return true;
    }

    void checkpointActivated(CheckpointGameObject* p0) {
        if (MusicState::playing) {
            FMODAudioEngine::get()->stopEffect(effectId);
            MusicState::playing = false;
        }
        GJBaseGameLayer::checkpointActivated(p0);
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void togglePracticeMode(bool practiceMode) {
        if (practiceMode) {
            if (MusicState::playing) {
                FMODAudioEngine::get()->stopEffect(effectId);
                effectId = FMODAudioEngine::get()->playEffect("Sisyphus2.mp3");
            }
        }
        PlayLayer::togglePracticeMode(practiceMode);
    }

    void fullReset() {
        if (MusicState::playing) {
            FMODAudioEngine::get()->stopEffect(effectId);
            effectId = FMODAudioEngine::get()->playEffect("Sisyphus2.mp3");
        }
        PlayLayer::fullReset();
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
        int currentAttempts = this->m_attempts;
        if (currentAttempts > 0 && howManyDeaths > 0 && currentAttempts % howManyDeaths == 0 && !MusicState::playing && MusicState::isPlatformer) {
            effectId = FMODAudioEngine::get()->playEffect("Sisyphus.mp3");
            MusicState::playing = true;
        }
        PlayLayer::destroyPlayer(p0, p1);
    }

    void postUpdate(float dt) {
        MusicState::idleTimer += dt;
        if (MusicState::idleTimer >= MusicState::howLongIdling && !MusicState::fadingImage) {
            MusicState::fadingImage = true;
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto sprite = CCSprite::create("IMG_1184.jpeg");
            float scaleRatio = (winSize.height / sprite->getContentSize().height);
            sprite->setScaleX(scaleRatio);
            sprite->setScaleY(scaleRatio);
            sprite->setOpacity(0);
            sprite->setPosition( {winSize.width / 2, winSize.height / 2} );
            this->addChild(sprite, 1000);
            sprite->runAction(CCFadeIn::create(MusicState::howLongToFade));
            MusicState::overlaySprite = sprite;
        }
        PlayLayer::postUpdate(dt);
    }
};
class $modify(PlayerObject) {
    void pushButton(PlayerButton p0) {
        if (MusicState::overlaySprite) {
            MusicState::overlaySprite->removeFromParent();
            MusicState::overlaySprite = nullptr;
            MusicState::fadingImage = false;
            MusicState::idleTimer = 0.f;
        }
        PlayerObject::pushButton(p0);
    }
};