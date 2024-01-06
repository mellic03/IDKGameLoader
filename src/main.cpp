#include <libidk/idk_platform.hpp>
#include <libidk/idk_api_loader.hpp>
#include <libidk/idk_module.hpp>
#include <libidk/idk_game.hpp>


#include <IDKGameEngine/IDKengine.hpp>
#include <IDKBuiltinCS/IDKBuiltinCS.hpp>

#include <IDKGameEngine/idk_engine_api.hpp>

#include <IDKGameEngine/IDKthreading/idk_threadpool.hpp>



class idk::internal::ThreadPoolAPI
{
public:
    static void update ( idk::ThreadPool &threadpool ) { threadpool._update(); };
};

class idk::internal::EngineAPI
{
public:
    static void setAPIptr( idk::Engine &engine, idk::EngineAPI *api )
    {
        engine.APIptr = api;
    };

    static void initModules( idk::Engine &engine )
    {
        engine._idk_modules_init();
    };

    static void beginFrame( idk::Engine &engine, idk::RenderEngine &ren )
    {
        engine._begin_frame(ren);
    };

    static void endFrame( idk::Engine &engine, idk::RenderEngine &ren )
    {
        engine._end_frame(ren);
    };
};

using internal_EngineAPI = idk::internal::EngineAPI;
using internal_ThreadAPI = idk::internal::ThreadPoolAPI;



int IDK_ENTRY( int argc, char **argv )
{
    // Load game code
    // -----------------------------------------------------------------------------------------
    idk::APILoader looder("IDKGE/runtime/libgame");
    idk::Game *game = looder.call<idk::Game>("getInstance");
    const char *window_title = game->name().c_str();
    // -----------------------------------------------------------------------------------------


    // Load engine code
    // -----------------------------------------------------------------------------------------
    idk::APILoader loader("IDKGE/runtime/libIDKGameEngine");
    idk::EngineAPI &api = *loader.call<idk::EngineAPI>("getInstance", window_title);

    auto &engine     = api.getEngine();
    auto &ren        = api.getRenderer();
    auto &threadpool = api.getThreadPool();
    internal_EngineAPI::setAPIptr(engine, &api);
    // -----------------------------------------------------------------------------------------


    // Load builtin component systems
    // -----------------------------------------------------------------------------------------
    // engine.registerCS<idk::Name_CS>("Name");
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


    // Setup
    // -----------------------------------------------------------------------------------------
    game->registerModules(api);
    internal_EngineAPI::initModules(engine);

    engine.createGameObject("scene");

    game->setup(api);
    // -----------------------------------------------------------------------------------------


    // Main loop
    // -----------------------------------------------------------------------------------------
    while (engine.running())
    {
        game->mainloop(api);

        internal_EngineAPI::beginFrame(engine, ren);
        internal_EngineAPI::endFrame(engine, ren);
        internal_ThreadAPI::update(threadpool);
    }
    // -----------------------------------------------------------------------------------------


    return 0;
}


