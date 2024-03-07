#include <libidk/idk_platform.hpp>
#include <libidk/idk_api_loader.hpp>
#include <libidk/idk_module.hpp>
#include <libidk/idk_game.hpp>
#include <libidk/idk_string.hpp>
#include <libidk/idk_print.hpp>

#include <IDKGameEngine/IDKengine.hpp>
#include <IDKECS/IDKECS.hpp>

#include <IDKEvents/IDKEvents.hpp>

#include <IDKBuiltinCS/IDKBuiltinCS.hpp>
#include <IDKBuiltinUI/EditorUI.hpp>

#include <IDKGameEngine/idk_engine_api.hpp>
#include <IDKThreading/IDKThreading.hpp>

#include <filesystem>




/*
    WHAT TO DO:

    1. You are making an FPS game with similar mechanics to half-life.
        - Must implement quake style FPS camera fully in Lua.

    STORY:
        - Intro is basically the same as "Hole in The Ground".
        - MC rushes home for the bathoom, finds a gaping hole where the toilet is supposed to be.
          They really need to pee, so they venture into the hole in search of the toilet.
        - Underneath the toilet is just the normal world.
        - Sad scene occurs where the toilet is found broken, the MC anguishes upon
          realising they will never pee again.

*/



void
message_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  GLchar const* message, void const* user_param )
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}




class idk_LuaTable
{
private:
    lua_State *L;

public:
    idk_LuaTable( const std::string &path )
    {
        L = luaL_newstate();
        luaL_dofile(L, path.c_str());
        lua_getglobal(L, "Config");
    };

    ~idk_LuaTable()
    {
        this->close();
    }

    void close()
    {
        lua_close(L);
    }

    auto get_string( const std::string &name )
    {
        lua_pushstring(L, name.c_str());
        lua_gettable(L, -2);
        const char *str = lua_tostring(L, -1);
        lua_pop(L, 1);

        return str;
    };

    auto get_int( const std::string &name )
    {
        lua_pushstring(L, name.c_str());
        lua_gettable(L, -2);
        int n = lua_tointeger(L, -1);
        lua_pop(L, 1);

        return n;
    };
};



int main( int argc, char **argv )
{
    std::string win_name;
    int gl_major, gl_minor;

    idk_LuaTable table("IDKGE/config.lua");
    win_name = table.get_string("window-title");
    gl_major = table.get_int("gl-major");
    gl_minor = table.get_int("gl-minor");

    // auto mh = ren.modelAllocator().loadModel("./area.idkvi");


    // Load engine code
    // // -----------------------------------------------------------------------------------------
    idk::GenericLoader<idk::EngineAPI> libapi("IDKGE/runtime/libIDKGameEngine.so");
    idk::EngineAPI &api = *libapi.getInstance();
    api.init(win_name, gl_major, gl_minor);

    auto &eventsys   = api.getEventSys();
    auto &audiosys   = api.getAudioSys();
    auto &ecs        = api.getECS();
    auto &engine     = api.getEngine();
    auto &ren        = api.getRenderer();
    auto &threadpool = api.getThreadPool();
    // -----------------------------------------------------------------------------------------


    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE
    );

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(message_callback, nullptr);



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


    // Load built-in components + systems
    // -----------------------------------------------------------------------------------------
    idk::registerComponents(ecs);
    idk::registerSystems(ecs);
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
    engine.initModules(api);
    ecs.init(api);
    // -----------------------------------------------------------------------------------------
    // ecs.readFile("entry.idksc");


    // Main loop
    // -----------------------------------------------------------------------------------------
    while (engine.running())
    {
        // IDK_NUM_GLCALL = 0;

        eventsys.processKeyInput();
        eventsys.processMouseInput();
        eventsys.update();

        // game->mainloop(api);
        threadpool._update();

        ecs.update(api);
        engine.beginFrame(api);
        engine.endFrame(api);

    //     std::cout << "IDK_NUM_GLCALL: " << IDK_NUM_GLCALL << "\n";
    }
    // -----------------------------------------------------------------------------------------

    // ecs.writeFile("entry.idksc");


    return 0;
}


