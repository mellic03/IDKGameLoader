#include <libidk/idk_platform.hpp>
#include <libidk/idk_api_loader.hpp>
#include <libidk/idk_module.hpp>
#include <libidk/idk_game.hpp>
#include <libidk/idk_cppscript.hpp>

#include <IDKGameEngine/IDKengine.hpp>
#include <IDKEvents/IDKEvents.hpp>

#include <IDKBuiltinCS/IDKBuiltinCS.hpp>
#include <IDKBuiltinUI/EditorUI.hpp>

#include <IDKGameEngine/idk_engine_api.hpp>
#include <IDKThreading/IDKThreading.hpp>

#include <filesystem>



int IDK_ENTRY( int argc, char **argv )
{
    // Load game code
    // -----------------------------------------------------------------------------------------
    idk::APILoader libgame("IDKGE/runtime/libgame");
    idk::Game *game = libgame.call<idk::Game>("getInstance");
    // -----------------------------------------------------------------------------------------


    // Load engine code
    // -----------------------------------------------------------------------------------------
    idk::APILoader libengine("IDKGE/runtime/libIDKGameEngine");
    idk::EngineAPI &api = *libengine.call<idk::EngineAPI>("getInstance", game->name());

    auto &eventsys   = api.getEventSys();
    auto &audiosys   = api.getAudioSys();
    auto &engine     = api.getEngine();
    auto &ren        = api.getRenderer();
    auto &threadpool = api.getThreadPool();
    // -----------------------------------------------------------------------------------------


    // Setup resize and exit callbacks
    // -----------------------------------------------------------------------------------------
    auto resize_lambda = [&ren, &eventsys]()
    {
        auto winsize = eventsys.windowSize();
        ren.resize(winsize.x, winsize.y);
    };

    auto exit_lambda = [&engine]()
    {
        engine.shutdown();
    };

    eventsys.onWindowEvent(idk::WindowEvent::RESIZE, resize_lambda);
    eventsys.onWindowEvent(idk::WindowEvent::EXIT,   exit_lambda);
    // -----------------------------------------------------------------------------------------


    // Load builtin component systems
    // -----------------------------------------------------------------------------------------
    engine.registerModule<EditorUI_Module>("EditorUI");

    engine.registerCS<idk::Icon_CS>("Icon");
    engine.registerCS<idk::Transform_CS>("Transform");
    engine.registerCS<idk::Model_CS>("Model");
    engine.registerCS<idk::Camera_CS>("Camera");
    // -----------------------------------------------------------------------------------------


    // Load modules from IDKGE/runtime/modules/
    // -----------------------------------------------------------------------------------------
    std::filesystem::directory_iterator d_iter("IDKGE/runtime/modules/");

    for (auto dir: d_iter)
    {
        std::string name = dir.path().stem();
        std::string path = dir.path();
        engine.registerModule(name, path);
    }
    // -----------------------------------------------------------------------------------------


    // Setup
    // -----------------------------------------------------------------------------------------
    game->registerModules(api);
    engine.initModules(api);
    engine.createGameObject("scene");
    game->setup(api);
    // -----------------------------------------------------------------------------------------

    // auto mh = ren.modelAllocator().loadModel("./area.idkvi");
    // idk::RuntimeScript script("./assets/cppscripts/script.cpp");
    // script.execute(api);

    // Main loop
    // -----------------------------------------------------------------------------------------
    while (engine.running())
    {
        eventsys.processKeyInput();
        eventsys.processMouseInput();
        eventsys.update();

        engine.beginFrame(api);
        engine.endFrame(api);
        game->mainloop(api);

        threadpool._update();
    }
    // -----------------------------------------------------------------------------------------


    return 0;
}


