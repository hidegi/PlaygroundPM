#include <pacman/pacman.h>
#include <pacman/ecs/animation_system.h>
#include <pacman/ecs/collision_system.h>
#include <pacman/ecs/movement_system.h>
#include <pacman/ecs/animation_component.h>
#include <pacman/ecs/position_component.h>
#include <pacman/ecs/sprite_component.h>

#define DIR_LEFT  0
#define DIR_RIGHT 1
#define DIR_UP    2
#define DIR_DOWN  3

#define KEY_A     65
#define KEY_S     83
#define KEY_W     87
#define KEY_D     68

namespace
{
    float SCREEN_HALT_TIME = 2.f;
    static AnimationSystem::Ptr animation_system;
    static MovementSystem::Ptr  movement_system;
    static CollisionSystem::Ptr collision_system;
}
PacMan::PacMan(unsigned int w, unsigned int h) :
    m_width         {w},
    m_height        {h},
    m_screen_halt   {false},
    m_run_game      {false},
    m_player_win    {false},
    m_killed_ghost  {(unsigned)-1}
{
}
PacMan::~PacMan()
{
    for(Ghost* g : m_ghosts)
        delete g;
    terminate();
}

#define SET_ALL_X(b)   \
    m_directions[DIR_LEFT]  =   \
    m_directions[DIR_UP]    =   \
    m_directions[DIR_DOWN]  =   \
    m_directions[DIR_RIGHT] = b

void PacMan::init()
{
    SP_Config config;
	config.width 		= m_width;
	config.height 		= m_height;

	config.title		= "test";
	config.contextMajor = 3;
	config.contextMinor = 0;
	config.decorated 	= true;
	config.profile 		= SP_Profile::Compatible;
	config.fullscreen 	= false;
	config.iconify		= false;
	if(!sp::Controller::create(config))
    {
        printf("failed to initialize controller..\n");
        return;
    }
    m_controller = sp::Controller::get();


    m_arcade_font = sp::Font::create();
    m_arcade_font->loadFromFile("fonts/emulogic.TTF");

    m_tooltip_label = sp::Label::create();
    m_tooltip_label->setFont(m_arcade_font);
    m_tooltip_label->setCharacterSize(10);
    m_tooltip_label->setString("READY??");
    m_tooltip_label->setColor(sp::Color{255, 255, 0});

    m_score_label = sp::Label::create();
    m_score_label->setFont(m_arcade_font);
    m_score_label->setCharacterSize(15);
    m_score_label->setString("SCORE: " + sp::String(m_score));
    m_score_label->setPosition(10.f, 27 * 16 + 10.f);
    m_controller->addDrawable(m_score_label);

    float x_offset = m_width / 2 - m_tooltip_label->getSize().x / 2 + 4;
    m_tooltip_label->setPosition(x_offset, 27 * 16 / 2 + 28);

    //m_tooltip_label->setVisible(false);




    m_motex_texture.loadFromFile("assets/thesp_.png");
    m_motex_texture_2.loadFromFile("assets/the_sp.png");

    m_debug_sprite = sp::Sprite::create();
    m_debug_sprite->setZOrder(1);
    m_debug_sprite->setTextureSprite(m_motex_texture);
    //m_controller->addDrawable(m_debug_sprite);


    m_debug_sprite_2 = sp::Sprite::create();
    m_debug_sprite_2->setTextureSprite(m_motex_texture_2);

    m_sprite_batch = sp::SpriteBatch::create();

    texture.loadFromFile("assets/spritesheet.png");

    m_board.setController(m_controller);
	m_board.setRulePath("levels/ghost.rsp");
	m_board.setSourceTexture(texture);
	m_board.build(sp::lvl1, sp::Color{0, 0, 255});
	m_board.nextLevel();
	m_board.build(sp::lvl1, sp::Color{255, 0, 0});
	m_board.build(sp::lvl1, sp::Color{0, 255, 0});
	m_board.build(sp::lvl1, sp::Color{255, 0, 255});
	m_board.build(sp::lvl1, sp::Color{0, 255, 255});
	m_board.build(sp::lvl1, sp::Color{255, 255, 0});
	//m_board.build(sp::lvl1, sp::Color{0, 0, 255});

	//m_board.addLevel();
    //m_board.addLevel();

    m_controller->loadImageIcon("assets/motex.png");




    /*
    m_controller->subscribe(sp::SP_SystemEvent::EventUpdate, this);
    m_controller->subscribe(sp::SP_SystemEvent::EventCursorPos, this);
	m_controller->subscribe(sp::SP_SystemEvent::EventKey, this);
	*/
	animation_system = std::make_shared<AnimationSystem>(m_controller);
	movement_system  = std::make_shared<MovementSystem>(m_controller);
	collision_system  = std::make_shared<CollisionSystem>(m_controller);


	m_observer = m_controller->createObserver();
	m_observer->listen<sp::SysEventUpdate>([this](const sp::SysEventUpdate& event){update(event.data.data);});
	m_observer->listen<sp::SysEventCursorPos>([this](const sp::SysEventCursorPos& event){cursorPos(event.data.x, event.data.y);});
	m_observer->listen<sp::SysEventKey>([this](const sp::SysEventKey& event){keyEvent(event.data.key, event.data.scancode, event.data.action, event.data.mods);});
	m_observer->listen<PowerEvent>([this](const PowerEvent& event){resetGhost();});
	m_observer->listen<CollisionEvent>([this](const CollisionEvent& event){handleCollision(event.target);});
	m_observer->listen<WinEvent>([this](const WinEvent& event){handleWin();});
	m_observer->listen<PointEvent>([this](const PointEvent& event){handlePoint();});


	motex.loadFromFile("assets/thesp_.png");
	sp::vec2u offset = {0, 0};

	const int margin   = 2;
	const int unit_size = 64;
	m_player.setController(m_controller);
	m_player.setTexture(texture);


	m_player.addAnimationFrame(sp::recti{unit_size * 2 + margin * 2 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_RIGHT);
	m_player.addAnimationFrame(sp::recti{unit_size * 7 + margin * 7 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_RIGHT);
	m_player.addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_RIGHT);

	m_player.addAnimationFrame(sp::recti{unit_size * 5 + margin * 5 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 7 + margin * 7 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 8 + margin * 8 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 2 + margin * 2 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 3 + margin * 3 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 4 + margin * 4 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);
	m_player.addAnimationFrame(sp::recti{unit_size * 5 + margin * 5 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, VULNERABLE);

	//ok..
	m_player.addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_LEFT);
	m_player.addAnimationFrame(sp::recti{unit_size * 4 + margin * 4 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_LEFT);
	m_player.addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_LEFT);

	m_player.addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_UP);
	m_player.addAnimationFrame(sp::recti{unit_size * 5 + margin * 5 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_UP);
	m_player.addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_UP);

	m_player.addAnimationFrame(sp::recti{unit_size * 8 + margin * 8 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_DOWN);
	m_player.addAnimationFrame(sp::recti{unit_size * 3 + margin * 3 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_DOWN);
	m_player.addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_DOWN);
    m_player.setAnimation(DIR_LEFT);

	m_player.setFrameDuration(.05f);
	m_player.setLoopAnimation(true);
    m_player.setStartPosition(11 * 16 - 8, 21 * 16 - 8);
    m_player.setEntitySpeed(100.f);


    ///RED..
	Ghost* ghost_red = new Ghost;//
    ghost_red->setController(m_controller);
	ghost_red->setTexture(texture);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, COOLDOWN);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, COOLDOWN);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 3 + margin * 3 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_LEFT);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 4 + margin * 4 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_LEFT);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 5 + margin * 5 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_RIGHT);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_RIGHT);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 7 + margin * 7 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_UP);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 8 + margin * 8 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_UP);

	ghost_red->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_DOWN);
	ghost_red->addAnimationFrame(sp::recti{unit_size * 2 + margin * 2 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, DIR_DOWN);
    ghost_red->setAnimation(DIR_LEFT);

	ghost_red->setFrameDuration(.05f);
	ghost_red->setLoopAnimation(true);
    ghost_red->setStartPosition(11 * 16 - 8, 11 * 16 - 8);
    ghost_red->setEntitySpeed(80.f);
    ghost_red->markAsFreeGhost(true);
    ghost_red->setInitialDirection(DIR_LEFT);
    m_ghosts.push_back(ghost_red);



    ///CYAN..
	Ghost* ghost_cyan = new Ghost;
    ghost_cyan->setController(m_controller);
	ghost_cyan->setTexture(texture);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, COOLDOWN);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, COOLDOWN);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_LEFT);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 2 + margin * 2 + margin, unit_size, unit_size}, DIR_LEFT);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 3 + margin * 3 + margin, unit_size, unit_size}, DIR_RIGHT);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 4 + margin * 4 + margin, unit_size, unit_size}, DIR_RIGHT);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 5 + margin * 5 + margin, unit_size, unit_size}, DIR_UP);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 6 + margin * 6 + margin, unit_size, unit_size}, DIR_UP);

	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, DIR_DOWN);
	ghost_cyan->addAnimationFrame(sp::recti{unit_size * 8 + margin * 8 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_DOWN);
    ghost_cyan->setAnimation(DIR_LEFT);

	ghost_cyan->setFrameDuration(.05f);
	ghost_cyan->setLoopAnimation(true);
    ghost_cyan->setStartPosition(10 * 16 - 8, 13 * 16 - 8);
    ghost_cyan->setEntitySpeed(80.f);
    ghost_cyan->setStartUpDuration(5.f);
    ghost_cyan->setInitialDirection(DIR_UP);
    m_ghosts.push_back(ghost_cyan);


    ///PINK..
    Ghost* ghost_pink = new Ghost;
    ghost_pink->setController(m_controller);
	ghost_pink->setTexture(texture);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, COOLDOWN);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, COOLDOWN);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 2 + margin * 2 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_LEFT);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 3 + margin * 3 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_LEFT);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 4 + margin * 4 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_RIGHT);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 5 + margin * 5 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_RIGHT);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_UP);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 7 + margin * 7 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_UP);

	ghost_pink->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_DOWN);
	ghost_pink->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, DIR_DOWN);
    ghost_pink->setAnimation(DIR_LEFT);

	ghost_pink->setFrameDuration(.05f);
	ghost_pink->setLoopAnimation(true);
    ghost_pink->setStartPosition(11 * 16 - 8, 14 * 16 - 8);
    ghost_pink->setEntitySpeed(80.f);
    ghost_pink->setStartUpDuration(10.f);
    ghost_pink->setInitialDirection(DIR_DOWN);
    m_ghosts.push_back(ghost_pink);

    ///ORANGE..
    Ghost* ghost_orange = new Ghost;
    ghost_orange->setController(m_controller);
	ghost_orange->setTexture(texture);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, VULNERABLE);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 7 + margin * 7 + margin, unit_size, unit_size}, COOLDOWN);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 9 + margin * 9 + margin, unit_size * 8 + margin * 8 + margin, unit_size, unit_size}, COOLDOWN);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 8 + margin * 8 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, DIR_LEFT);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 0 + margin * 0 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_LEFT);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 1 + margin * 1 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_RIGHT);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 2 + margin * 2 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_RIGHT);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 3 + margin * 3 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_UP);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 4 + margin * 4 + margin, unit_size * 1 + margin * 1 + margin, unit_size, unit_size}, DIR_UP);

	ghost_orange->addAnimationFrame(sp::recti{unit_size * 6 + margin * 6 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, DIR_DOWN);
	ghost_orange->addAnimationFrame(sp::recti{unit_size * 7 + margin * 7 + margin, unit_size * 0 + margin * 0 + margin, unit_size, unit_size}, DIR_DOWN);
    ghost_orange->setAnimation(DIR_LEFT);

	ghost_orange->setFrameDuration(.05f);
	ghost_orange->setLoopAnimation(true);
    ghost_orange->setStartPosition(12 * 16 - 8, 13 * 16 - 8);
    ghost_orange->setEntitySpeed(80.f);
    ghost_orange->setStartUpDuration(15.f);
    ghost_orange->setInitialDirection(DIR_UP);
    m_ghosts.push_back(ghost_orange);

    ghost_red->setEasyMode(false);
    ghost_red->registerComponents();
    ghost_cyan->registerComponents();
    ghost_pink->registerComponents();
    ghost_orange->registerComponents();
    m_player.registerComponents();

    collision_system->setControlEntity(m_player.getEntityID());
	//m_board.build(sp::lvl1, sp::Color{255, 0, 0});


	m_player.setMap(m_board.getCurrentMap());
    ghost_red->setMap(m_board.getCurrentMap());
    ghost_cyan->setMap(m_board.getCurrentMap());
    ghost_pink->setMap(m_board.getCurrentMap());
    ghost_orange->setMap(m_board.getCurrentMap());

    m_controller->addDrawable(m_tooltip_label, true);
    //m_controller->setClearColor(sp::Color{50, 50, 50});
	memset(m_keys, false, 1024);
	SET_ALL_X(false);

	mainLoop();

}

void PacMan::handlePoint()
{
    m_score += 10;
    m_score_label->setString("SCORE: " + sp::String(m_score));
}
void PacMan::handleCollision(sp::entity_type type)
{
    if(m_player.isVulnerable() || m_screen_halt || m_player_win)
        return;

    auto it = std::find_if(m_ghosts.begin(), m_ghosts.end(),
        [=](const Ghost* ghost){
            return ghost->getEntityID() == type;
        });
    if(it == m_ghosts.end())
        return;


    Ghost* ghost = *it;
    if(ghost->isKilled())
        return;

    m_halt_timer.reset();
    auto& posComp = m_controller->getComponent<PositionComponent>(m_player.getEntityID());
    /*
    if(!posComp)
        return;
    */

    if(ghost->isVulnerable())
    {
        sp::vec2f pos = posComp.next_position;

        m_tooltip_label->setColor(sp::Color{255, 255 , 255});
        m_tooltip_label->setVisible(true);
        m_tooltip_label->setCharacterSize(15);
        m_tooltip_label->setFont(m_controller->getDefaultFont());
        m_tooltip_label->setColor(sp::Color{0, 255 , 255});
        m_tooltip_label->setString("100");
        m_tooltip_label->setPosition(pos.x, pos.y);
        m_score_label->setString("SCORE: " + sp::String(m_score));
        m_score += 100;

        m_killed_ghost = type;

        auto& sc = m_controller->getComponent<SpriteComponent>(m_killed_ghost);
        sc.sprite->setVisible(false);

        ghost->setKilled();
        m_screen_halt = true;
    }
    else
    {
        if(m_run_game)
        {
            m_tooltip_label->setColor(sp::Color{255, 255 , 0});
            m_tooltip_label->setVisible(true);

            auto& ac   = m_controller->getComponent<AnimationComponent>(m_player.getEntityID());
            ac.state   = VULNERABLE;
            ac.loop    = false;
            m_run_game = false;
            m_player.setVulnerable();
            m_tooltip_label->setFont(m_arcade_font);
            m_tooltip_label->setString("YOU DIED!!");
            m_tooltip_label->setCharacterSize(10);
            float x_offset = m_width / 2 - m_tooltip_label->getSize().x / 2;
            m_tooltip_label->setPosition(x_offset, 27 * 16 / 2 + 28);
            m_screen_halt = true;
        }
        //player died, decrement life count..
    }

    //m_controller->addDrawable(m_tooltip_label);
}

void PacMan::resetGhost()
{
    m_score += 50;
    m_score_label->setString("SCORE: " + sp::String(m_score));
    for(Ghost* g : m_ghosts)
    {
        g->setVulnerable();
    }
}
void PacMan::mainLoop()
{
    m_controller->mainLoop();
}


void PacMan::processInput(double dt)
{
    m_player.moveEntity();

    for(Ghost* g : m_ghosts)
    {
        g->setTargetPoint(m_player.getEntityPosition());
        g->moveEntity();
    }
}

void PacMan::handleWin()
{
    //printf("win event..\n");
    m_halt_timer.reset();
    m_screen_halt = true;
    m_player_win = true;
    m_run_game = false;

    m_tooltip_label->setVisible(true);
    m_tooltip_label->setFont(m_arcade_font);
    m_tooltip_label->setCharacterSize(10);
    m_tooltip_label->setString("YOU WIN!!");
    m_tooltip_label->setColor(sp::Color{255, 255, 0});
    float x_offset = m_width / 2 - m_tooltip_label->getSize().x / 2 + 4;
    m_tooltip_label->setPosition(x_offset, 27 * 16 / 2 + 28);


    //m_board.removeLevel();
    //m_controller->removeDrawable(m_debug_sprite);
    //m_board.removeLevel();
    //
}

void PacMan::keyEvent(int key, int scancode, int action, int mods)
{
    if(key == 0x100 && action)
    {
        terminate();
    }

    if(!action && key == 0x43)
    {
        m_controller->getSubject()->dispatch(WinEvent());
    }

    if(key >= 0 && key < 1024)
    {
        m_keys[key] = (bool)action;
        m_player.setKey(key, (bool) action);

        if(!m_run_game && !m_screen_halt && (
            key == KEY_A ||
            key == KEY_W ||
            key == KEY_S||
            key == KEY_D))
        {
            m_run_game = true;
            m_tooltip_label->setVisible(false);
        }
    }
}

void PacMan::terminate()
{
    if(m_terminated) return;
    m_controller->terminate();
    m_terminated = true;
}

void PacMan::update(double dt)
{
    if(m_screen_halt)
    {
        if(m_halt_timer.getElapsedSeconds() >= SCREEN_HALT_TIME)
        {

            m_halt_timer.reset();
            m_screen_halt = false;
            if(m_player.isVulnerable())
            {
                //player died..
                m_player.reset();

                for(Ghost* g : m_ghosts)
                    g->reset();



                //m_board.nextLevel();    //updated immediately..
                m_tooltip_label->setFont(m_arcade_font);
                m_tooltip_label->setCharacterSize(10);
                m_tooltip_label->setString("READY??"); //updated later..
                float x_offset = m_width / 2 - m_tooltip_label->getSize().x / 2 + 4;
                m_tooltip_label->setPosition(x_offset, 27 * 16 / 2 + 28);
                m_tooltip_label->setVisible(true);
            }
            else
            {   //player killed some ghost..
                if(m_killed_ghost != (unsigned) -1)
                {
                    auto& sprite_comp = m_controller->getComponent<SpriteComponent>(m_killed_ghost);
                    sprite_comp.sprite->setVisible(true);
                    m_killed_ghost = -1;
                }
                m_tooltip_label->setVisible(false);
            }

            if(m_player_win)
            {
                m_player.reset();
                for(Ghost* g : m_ghosts)
                    g->reset();
                m_player_win = false;
                m_tooltip_label->setVisible(true);
                float x_offset = m_width / 2 - m_tooltip_label->getSize().x / 2 + 4;
                m_tooltip_label->setPosition(x_offset, 27 * 16 / 2 + 28);

                m_board.nextLevel();    //updated immediately..
                m_tooltip_label->setFont(m_arcade_font);
                m_tooltip_label->setCharacterSize(10);
                m_tooltip_label->setString("READY??"); //updated later..

                m_player.setMap(m_board.getCurrentMap());
                for(Ghost* g : m_ghosts)
                    g->setMap(m_board.getCurrentMap());

            }
        }
        return;
    }

    if(!m_run_game)
        return;

    processInput(dt);
}

//what happens??
//map is pushed back (has last position)..  4 vertices..
//label changes size (is front of map), causes artifacts.. x vertices..

void PacMan::cursorPos(double x, double y)
{
}

