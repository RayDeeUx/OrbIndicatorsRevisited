#include <Geode/modify/GJBaseGameLayer.hpp>
/*
#include <Geode/ui/GeodeUI.hpp>
#include <random>
#include <ctime>
*/

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
		/*
		i hate hooking GJBGL::update() as much as the next person, trust me.
		but for some absurd reason, rob decided to keep the orb indicator
		rotation logic inside a func call inside GJBGL::update(), and the
		func for accessing the indicator sprites is inlined for macos ARM.
		seeing as that func is already being called within GLBGL::update(),
		i might as well follow along to preserve compat.
		--raydeeux
		*/
		if (!enabled || !m_orbIndicators || !m_indicatorSprites || !m_player1) return;
		int rotation = 0;
		if (m_player1->m_isSideways) rotation = m_player1->m_isUpsideDown ? 270 : 90;
		else if (m_player1->m_isUpsideDown) rotation = 180;

		for (const auto indicatorSprite : CCArrayExt<CCSprite*>(m_indicatorSprites)) {
			if (!indicatorSprite->getParent()) continue;
			const auto gameObject = static_cast<GameObject*>(indicatorSprite->getParent());
			if (NOT_RED && NOT_PINK && NOT_YELLOW && NOT_GRAVITY && NOT_GRAVJUMP && NOT_BLACK_DROP) continue;
			indicatorSprite->setRotation(rotation - gameObject->getRotation());
		}
	}
};

$on_mod(Loaded) {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	listenForAllSettingChanges([](std::shared_ptr<SettingV3> _) {
		enabled = Mod::get()->getSettingValue<bool>("enabled");
	});
}

/*
$execute {
	new EventListener<EventFilter<ModLogoUIEvent>>(+[](ModLogoUIEvent* event) {
		std::filesystem::path nwo5LogoPath = Mod::get()->getResourcesDir() / "nwo5.png";
		if (!event->getSprite() || event->getModID() != Mod::get()->getID() || !std::filesystem::exists(nwo5LogoPath)) return ListenerResult::Propagate;
		if (event->getSprite()->getChildByID("raydeeux.custommodlogos/raydeeux.orbindicatorsrevisited-custom-logo")) return ListenerResult::Propagate;

		std::string nwo5SpriteID = "nwo5-logo"_spr;
		if (event->getSprite()->getChildByID(nwo5SpriteID)) return ListenerResult::Propagate;

		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		if ((1 + std::rand() % 1000) != 500) return ListenerResult::Propagate;

		CCSprite* nwo5 = CCSprite::create(nwo5LogoPath.string().c_str());
		if (!nwo5) return ListenerResult::Propagate;
		nwo5->setID(nwo5SpriteID);
		
		const CCSize originalSize = event->getSprite()->getContentSize();
		const CCSize replacementSize = nwo5->getContentSize();
		nwo5->setScale(std::min(originalSize.width / replacementSize.width, originalSize.height / replacementSize.height));
		event->getSprite()->addChildAtPosition(nwo5, Anchor::Center);
		
		return ListenerResult::Propagate;
	});
}
*/