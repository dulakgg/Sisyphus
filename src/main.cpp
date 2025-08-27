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
float MusicState::howLongIdling = 0.f;
float MusicState::howLongToFade = 0.f;
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
        MusicState::howLongIdling = Mod::get()->getSettingValue<float>("time-idling");
        MusicState::howLongToFade = Mod::get()->getSettingValue<float>("time-to-fade");
        return true;
    }

    void checkpointActivated(CheckpointGameObject* p0) {
        if (MusicState::playing) {
            MusicState::effectId = FMODAudioEngine::get()->playEffect("Sisyphus.mp3");
            FMODAudioEngine::get()->pauseEffect(MusicState::effectId);
            MusicState::playing = false;
        }
        GJBaseGameLayer::checkpointActivated(p0);
    }
    void onExit() {
        if (MusicState::effectId != -1) {
            FMODAudioEngine::get()->pauseEffect(MusicState::effectId); 
            MusicState::effectId = -1;
        }
        if (MusicState::overlaySprite) {
            MusicState::overlaySprite->removeFromParent();
            MusicState::overlaySprite = nullptr;
        }
        MusicState::fadingImage = false;
        MusicState::idleTimer = 0.f;
        GJBaseGameLayer::onExit();
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void togglePracticeMode(bool practiceMode) {
        if (practiceMode) {
            if (MusicState::playing) {
                MusicState::effectId = FMODAudioEngine::get()->playEffect("Sisyphus1.mp3");
            }
        }
        PlayLayer::togglePracticeMode(practiceMode);
    }

    void fullReset() {
        if (MusicState::playing) {
            MusicState::effectId = FMODAudioEngine::get()->playEffect("Sisyphus1.mp3");
        }
        PlayLayer::fullReset();
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        int howManyDeaths = Mod::get()->getSettingValue<int64_t>("how-many-deaths");
        int currentAttempts = this->m_attempts;
        if (currentAttempts > 0 && howManyDeaths > 0 && currentAttempts % howManyDeaths == 0 && !MusicState::playing && MusicState::isPlatformer) {
            MusicState::effectId = FMODAudioEngine::get()->playEffect("Sisyphus.mp3");
            MusicState::playing = true;
        }
        PlayLayer::destroyPlayer(p0, p1);
    }

    void levelComplete() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto sprite = CCSprite::create("its_over.png");
        if (!sprite) { return; }
        auto content = sprite->getContentSize();
        if (content.height <= 0.0f) return;
        float scaleRatio = (winSize.height / content.height);
        sprite->setScale(scaleRatio);
        sprite->setScaleX(scaleRatio);
        sprite->setScaleY(scaleRatio);
        sprite->setOpacity(0);
        sprite->setPosition( {winSize.width / 2, winSize.height / 2} );
        this->addChild(sprite, 1000);
        sprite->runAction(CCSequence::create( CCFadeIn::create(3.0f), CCFadeOut::create(1.0f), CCCallFuncN::create(sprite, callfuncN_selector(CCNode::removeFromParent)), nullptr ));
        PlayLayer::levelComplete();
    }

    // void postUpdate(float dt) {
    //     PlayLayer::postUpdate(dt);
    //     MusicState::idleTimer += dt;
    //     if (MusicState::idleTimer >= MusicState::howLongIdling && !MusicState::fadingImage) {
    //         auto winSize = CCDirector::sharedDirector()->getWinSize();
    //         auto sprite = CCSprite::create("stop.png");
    //         if (!sprite) return;
    //         float scaleRatio = (winSize.height / sprite->getContentSize().height);
    //         sprite->setScaleX(scaleRatio);
    //         sprite->setScaleY(scaleRatio);
    //         sprite->setOpacity(0);
    //         sprite->setPosition( {winSize.width / 2, winSize.height / 2} );
    //         this->addChild(sprite, 1000);
    //         sprite->runAction(CCFadeIn::create(MusicState::howLongToFade));
    //         MusicState::overlaySprite = sprite;
    //         MusicState::fadingImage = true;
    //     }
    // }
};
// class $modify(PlayerObject) {
//     void pushButton(PlayerButton p0) {
//         if (MusicState::overlaySprite) {
//             MusicState::overlaySprite->removeFromParent();
//             MusicState::overlaySprite = nullptr;
//             MusicState::fadingImage = false;
//             MusicState::idleTimer = 0.f;
//         }
//         PlayerObject::pushButton(p0);
//     }
// };
