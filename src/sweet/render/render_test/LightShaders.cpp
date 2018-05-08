
#include <UnitTest++/UnitTest++.h>
#include <sweet/render/Value.hpp>
#include <sweet/render/Grid.hpp>
#include <sweet/render/Shader.hpp>
#include <sweet/render/SymbolTable.hpp>
#include <sweet/render/ErrorPolicy.hpp>
#include <sweet/render/Renderer.hpp>
#include <sweet/render/assert.hpp>

using std::vector;
using namespace sweet;
using namespace sweet::render;

SUITE( TestLightShaders )
{
    TEST( AmbientLight )
    {
        SymbolTable symbol_table;
        ErrorPolicy error_policy;
        Shader shader( SHADERS_PATH "ambientlight.sl", symbol_table, error_policy );
    }

    TEST( DistantLight )
    {
        SymbolTable symbol_table;
        ErrorPolicy error_policy;
        Shader shader( SHADERS_PATH "distantlight.sl", symbol_table, error_policy );
    }
    
    TEST( PointLight )
    {
        SymbolTable symbol_table;
        ErrorPolicy error_policy;
        Shader shader( SHADERS_PATH "pointlight.sl", symbol_table, error_policy );
    }
    
    TEST( SpotLight )
    {
        SymbolTable symbol_table;
        ErrorPolicy error_policy;
        Shader shader( SHADERS_PATH "spotlight.sl", symbol_table, error_policy );
    }
    
    TEST( TestShadowPointLight )
    {
        SymbolTable symbol_table;
        ErrorPolicy error_policy;
        Shader shader( SHADERS_PATH "shadowpointlight.sl", symbol_table, error_policy );
    }
    
    TEST( active_light_list )
    {
        Renderer renderer;

        Shader shader( SHADERS_PATH "ambientlight.sl", renderer.symbol_table(), renderer.error_policy() );

        renderer.begin();
        Grid& light = renderer.light_shader( &shader );
        Grid grid;
        grid.resize( 2, 2 );
        grid.value("P", TYPE_POINT).zero();
        renderer.light_shade( grid );
        CHECK( grid.lights().size() == 1 );
        
        renderer.deactivate_light_shader( light );
        grid = Grid();
        grid.resize( 2, 2 );
        grid.value("P", TYPE_POINT).zero();
        renderer.light_shade( grid );
        CHECK( grid.lights().size() == 0 );
        
        renderer.activate_light_shader( light );
        grid = Grid();
        grid.resize( 2, 2 );
        grid.value("P", TYPE_POINT).zero();
        renderer.light_shade( grid );
        CHECK( grid.lights().size() == 1 );
    }
}
