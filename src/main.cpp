#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

bool enabled = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	void update(float dt) {
		GJBaseGameLayer::update(dt);
		if (!enabled || !m_orbIndicators || !m_indicatorSprites) return;
		for (const auto indicatorSprite : CCArrayExt<CCSprite*>(m_indicatorSprites)) {
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