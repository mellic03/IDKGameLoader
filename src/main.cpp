#include <libidk/idk_platform.hpp>

#include <IDKGameEngine/IDKengine.hpp>
#include <IDKGameEngine/idk_engine_api.hpp>

#include <IDKGameEngine/IDKthreading/idk_threadpool.hpp>
#include <IDKGameEngine/IDKmodules/idk_game.hpp>
#include <IDKGameEngine/idk_api_loader.hpp>


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
    idk::GenericAPILoader looder("IDKGE/runtime/libgame");
    idk::Game *game = looder.getAPI<idk::Game>("getInstance");

    // idk::GameHandle handle("IDKGE/runtime/libgame");
    // idk::Game *game = handle.getInstance();

    const char *window_title = game->name().c_str();
    // -----------------------------------------------------------------------------------------


    // Load engine code
    // -----------------------------------------------------------------------------------------
    idk::APILoader loader("IDKGE/runtime/libIDKengine");
    idk::EngineAPI &api = loader.getEngineAPI(window_title);

    auto &engine     = api.getEngine();
    auto &ren        = api.getRenderer();
    auto &threadpool = api.getThreadPool();
    internal_EngineAPI::setAPIptr(engine, &api);
    // -----------------------------------------------------------------------------------------


    // Setup
    // -----------------------------------------------------------------------------------------
    game->registerModules(api);
    internal_EngineAPI::initModules(engine);
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


