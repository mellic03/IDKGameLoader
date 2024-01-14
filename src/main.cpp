#include <libidk/idk_platform.hpp>
#include <libidk/idk_api_loader.hpp>
#include <libidk/idk_module.hpp>
#include <libidk/idk_game.hpp>
#include <libidk/idk_cppscript.hpp>

#include <IDKGameEngine/IDKengine.hpp>
#include <IDKBuiltinCS/IDKBuiltinCS.hpp>
#include <IDKBuiltinUI/EditorUI.hpp>

#include <IDKGameEngine/idk_engine_api.hpp>
#include <IDKGameEngine/IDKthreading/idk_threadpool.hpp>



class idk::internal::ThreadPoolAPI
{
public:
    static void update ( idk::ThreadPool &threadpool ) { threadpool._update(); };
};

using internal_ThreadAPI = idk::internal::ThreadPoolAPI;



int IDK_ENTRY( int argc, char **argv )
{
    // Load game code
    // -----------------------------------------------------------------------------------------
    idk::APILoader libgame("IDKGE/runtime/libgame");
    idk::Game *game = libgame.call<idk::Game>("getInstance");
    std::cout << "window_title: " << game->name() << "\n";
    // -----------------------------------------------------------------------------------------


    // Load engine code
    // -----------------------------------------------------------------------------------------
    idk::APILoader libengine("IDKGE/runtime/libIDKGameEngine");
    idk::EngineAPI &api = *libengine.call<idk::EngineAPI>("getInstance", game->name());

    auto &engine     = api.getEngine();
    engine.APIptr    = &api;

    auto &ren        = api.getRenderer();
    auto &threadpool = api.getThreadPool();
    // -----------------------------------------------------------------------------------------


    // Load builtin component systems
    // -----------------------------------------------------------------------------------------
    engine.registerModule<EditorUI_Module>("EditorUI");

    engine.registerCS<idk::Icon_CS>("Icon");
    engine.registerCS<idk::Transform_CS>("Transform");
    engine.registerCS<idk::Model_CS>("Model");
    engine.registerCS<idk::Camera_CS>("Camera");
    // -----------------------------------------------------------------------------------------


    // Load modules specified in loadlist.txt
    // -----------------------------------------------------------------------------------------
    std::ifstream stream("IDKGE/runtime/loadlist.txt");
    std::string line;

    while (std::getline(stream, line))
    {
        engine.registerModule(line.substr(3), "IDKGE/runtime/" + line);
    }

    stream.close();
    // -----------------------------------------------------------------------------------------


    idk::RuntimeScript script("./script.cpp");
    script.execute(api);


    // Setup
    // -----------------------------------------------------------------------------------------
    game->registerModules(api);
    engine.initModules();
    engine.createGameObject("scene");
    game->setup(api);
    // -----------------------------------------------------------------------------------------

    // auto mh = ren.modelAllocator().loadModel("./area.idkvi");

    // Main loop
    // -----------------------------------------------------------------------------------------
    while (engine.running())
    {
        engine.beginFrame(ren);
        engine.endFrame(ren);
        game->mainloop(api);

        internal_ThreadAPI::update(threadpool);
    }
    // -----------------------------------------------------------------------------------------


    return 0;
}


