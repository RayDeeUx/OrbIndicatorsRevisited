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
#define NOT_BLACKDROP gameObject->m_objectType != GameObjectType::DropRing
#define NON_INDICATOR_ORB NOT_RED && NOT_PINK && NOT_YELLOW && NOT_GRAVITY && NOT_GRAVJUMP && NOT_BLACKDROP

#define RED gameObject->m_objectType == GameObjectType::RedJumpRing
#define PINK gameObject->m_objectType == GameObjectType::PinkJumpRing
#define YELLOW gameObject->m_objectType == GameObjectType::YellowJumpRing
#define GRAVITY gameObject->m_objectType == GameObjectType::GravityRing
#define GRAVJUMP gameObject->m_objectType == GameObjectType::GreenRing
#define BLACKDROP gameObject->m_objectType == GameObjectType::DropRing

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

		const bool sideways = m_player1->m_isSideways;
		const bool flipped = m_player1->m_isUpsideDown;

		int rotation = 0;
		if (sideways) rotation = flipped ? 270 : 90;
		else if (flipped) rotation = 180;

		for (const auto indicatorSprite : CCArrayExt<CCSprite*>(m_indicatorSprites)) {
			if (!indicatorSprite->getParent()) continue;

			const auto gameObject = static_cast<GameObject*>(indicatorSprite->getParent());
			if (NON_INDICATOR_ORB) continue;

			const int gameObjectRotation = gameObject->getRotation();
			indicatorSprite->setRotation(rotation - gameObjectRotation);

			const bool flipY = gameObject->isFlipY();
			const bool flipX = gameObject->isFlipX();

			/*
			i'd check for gameObject rotation modulo 90,
			but rotate triggers exist so that would cause
			some flickering in orb indicators which i could
			work around by checking for groups but that's just
			going to waste clock cycles so i'm just going to avoid
			all of that noise and let the rare edge cases suffer! yay!!!
			--raydeeux
			*/
			
			if ((flipY && flipX) || (flipY && !sideways) || (flipX && sideways)) {
				indicatorSprite->setRotation(indicatorSprite->getRotation() - 180);
			}
		}
	}
};

$on_mod(Loaded) {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	listenForAllSettingChanges([](std::shared_ptr<SettingV3> _) {
		enabled = Mod::get()->getSettingValue<bool>("enabled");
	});
	/*
	Oh, boo hoo! I'm loading ~5.4MB of textures just for one measly little toggle.
	I'd like to see a better solution from you then, if you're worried so much about
	these few lines of code. No one's complained when these lines were used in the
	ChangeFont19 mod; cry me a river or make a pull request.
	--raydeeux
	*/
	if (Mod::get()->getSettingValue<bool>("slightlyMoreHelpfulOrbIndicators") && enabled) {
		const std::string& resourcesDir = Mod::get()->getResourcesDir().string();
		auto directoryVector = std::vector<std::string>{ resourcesDir };
		const auto texturePack = CCTexturePack {
			.m_id = Mod::get()->getID(), // they're the same ID so it doesnt matter
			.m_paths = directoryVector
		};
		CCFileUtils::get()->addTexturePack(texturePack);
	}
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