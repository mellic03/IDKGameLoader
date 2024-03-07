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



void
message_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  GLchar const* message, void const* user_param )
{
	auto const src_str = [source]()
    {
		switch (source)
		{
            default:                                return "UNKNOWN";
            case GL_DEBUG_SOURCE_API:               return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:   return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY:       return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION:       return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER:             return "OTHER";
		}
	}();

	auto const type_str = [type]()
    {
		switch (type)
		{
            default:                                return "UNKNOWN";
            case GL_DEBUG_TYPE_ERROR:               return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY:         return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER:              return "MARKER";
            case GL_DEBUG_TYPE_OTHER:               return "OTHER";
		}
	}();

	auto const severity_str = [severity]
    {
		switch (severity)
        {
            default:                                return "UNKNOWN";
		    case GL_DEBUG_SEVERITY_NOTIFICATION:    return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW:             return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM:          return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH:            return "HIGH";
		}
	}();

	std::cout << src_str << ", "
              << type_str << ", "
              << severity_str << ", "
              << id << ": "
              << message << '\n';
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




    // int model_id = ren.modelAllocator().loadModel("assets/models/bathroom.idkvi");

    // Main loop
    // -----------------------------------------------------------------------------------------
    while (engine.running())
    {
        eventsys.processKeyInput();
        eventsys.processMouseInput();
        eventsys.update();

        threadpool._update();

        ecs.update(api);
        engine.beginFrame(api);

        // auto &MA = ren.modelAllocator();
        // MA.pushModelDraw(model_id, glm::mat4(1.0f));

        engine.endFrame(api);

    }
    // -----------------------------------------------------------------------------------------


    return 0;
}


