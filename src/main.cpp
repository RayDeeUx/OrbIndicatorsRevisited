#include <Geode/modify/GJBaseGameLayer.hpp>

#define NOT_RED gameObject->m_objectType != GameObjectType::RedJumpRing
#define NOT_PINK gameObject->m_objectType != GameObjectType::PinkJumpRing
#define NOT_YELLOW gameObject->m_objectType != GameObjectType::YellowJumpRing
#define NOT_GRAVITY gameObject->m_objectType != GameObjectType::GravityRing
#define NOT_GRAVJUMP gameObject->m_objectType != GameObjectType::GreenRing
#define NOT_BLACK_DROP gameObject->m_objectType != GameObjectType::DropRing

using namespace geode::prelude;

bool enabled = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	void update(float dt) {
		GJBaseGameLayer::update(dt);
		if (!enabled || !m_orbIndicators || !m_indicatorSprites) return;
		for (const auto indicatorSprite : CCArrayExt<CCSprite*>(m_indicatorSprites)) {
			if (!indicatorSprite->getParent()) continue;
			const auto gameObject = static_cast<GameObject*>(indicatorSprite->getParent());
			if (NOT_RED && NOT_PINK && NOT_YELLOW && NOT_GRAVITY && NOT_GRAVJUMP && NOT_BLACK_DROP) continue;
			indicatorSprite->setScale(4.f);
		}
	}
};

$on_mod(Loaded) {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	listenForAllSettingChanges([](std::shared_ptr<SettingV3> _) {
		enabled = Mod::get()->getSettingValue<bool>("enabled");
	});
}