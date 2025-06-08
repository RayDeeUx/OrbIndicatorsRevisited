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
		// i hate hooking ::update() as much as the next person
		// but for some fucking reason rob decided to keep the orb indicator
		// rotation logic inside a func call inside GJBGL::update()
		// and the dedicated func for rotating the sprites is inlined on mac
		// seeing as that func is already being called within update i might as well
		// follow suit to preserve compat

		if (!enabled || !m_orbIndicators || !m_indicatorSprites || !m_player1) return;
		int rotation = 0;
		if (m_player1->m_isSideways) rotation = m_player1->m_isUpsideDown ? 270 : 90;
		else if (m_player1->m_isUpsideDown) rotation = 180;

		for (const auto indicatorSprite : CCArrayExt<CCSprite*>(m_indicatorSprites)) {
			if (!indicatorSprite->getParent()) continue;
			const auto gameObject = static_cast<GameObject*>(indicatorSprite->getParent());
			if (NOT_RED && NOT_PINK && NOT_YELLOW && NOT_GRAVITY && NOT_GRAVJUMP && NOT_BLACK_DROP) continue;
			indicatorSprite->setRotation(rotation - gameObject->getRotation());
			if (m_player1->m_isSideways && gameObject->isFlipX()) indicatorSprite->setFlipX(gameObject->isFlipX());
			if (m_player1->m_isUpsideDown && gameObject->isFlipY()) indicatorSprite->setFlipY(gameObject->isFlipY());
		}
	}
};

$on_mod(Loaded) {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	listenForAllSettingChanges([](std::shared_ptr<SettingV3> _) {
		enabled = Mod::get()->getSettingValue<bool>("enabled");
	});
}