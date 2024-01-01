#include <libidk/idk_platform.hpp>

#include <IDKengine/IDKengine.hpp>
#include <IDKengine/idk_engine_api.hpp>

#include <IDKengine/IDKthreading/idk_threadpool.hpp>
#include <IDKengine/IDKmodules/idk_game.hpp>



class idk::internal::ThreadPoolAPI
{
public:
    static void update ( idk::ThreadPool &threadpool ) { threadpool._update(); };
};

class idk::internal::EngineAPI
{
public:
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

using namespace idk::internal;



int IDK_ENTRY( int argc, char **argv )
{
    // Load engine code
    // -----------------------------------------------------------------------------------------
    idk::APILoader loader("IDKGE/runtime/libIDKengine");
    idk::EngineAPI *api = loader.getEngineAPI();

    auto &engine = api->getEngine();
    auto &ren    = api->getRenderer();
    // -----------------------------------------------------------------------------------------


    // Load game code
    // -----------------------------------------------------------------------------------------
    idk::GameHandle handle("IDKGE/runtime/libgame");
    idk::Game *game = handle.getInstance();
    std::cout << "game->name(): " << game->name() << "\n";
    // -----------------------------------------------------------------------------------------


    // Engine loop
    // -----------------------------------------------------------------------------------------
    game->registerModules(engine);
    EngineAPI::initModules(engine);

    game->setup(engine, ren);

    while (engine.running())
    {
        game->mainloop(engine, ren);

        EngineAPI::beginFrame(engine, ren);
        EngineAPI::endFrame(engine, ren);

        // ThreadPoolAPI::update(threadpool);
    }
    // -----------------------------------------------------------------------------------------


    return 0;
}


