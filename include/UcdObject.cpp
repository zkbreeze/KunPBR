//
//  UcdObject.cpp
//
//
//  Created by Kun Zhao on 2015-12-07 18:32:11.
//
//

#include "UcdObject.h"
#include <iostream>
#include <kvs/File>
#include <kvs/IgnoreUnusedVariable>

namespace
{

std::string GetString( std::ifstream& ifs, unsigned int n )
{
    std::string ret( n, '\0' );
    ifs.read( &ret[0], n );
    return ret;
}

template <typename T>
T GetValue( std::ifstream& ifs )
{
    T ret = 0;
    ifs.read( (char*)( &ret ), sizeof(T) );
    return ret;
}

template <typename T>
kvs::ValueArray<T> GetValues( std::ifstream& ifs, const unsigned int size )
{
    kvs::ValueArray<T> ret( size );
    ifs.read( (char*)( ret.data() ), sizeof(T) * size );
    return ret;
}

template <typename T>
void SkipValue( std::ifstream& ifs )
{
    ifs.seekg( sizeof(T), ifs.cur );
}

template <typename T>
void SkipValues( std::ifstream& ifs, unsigned int size )
{
    ifs.seekg( sizeof(T) * size, ifs.cur );
}

kun::UcdObject::Coords GetCoords1( std::ifstream& ifs, unsigned int nnodes )
{
    kun::UcdObject::Coords coords( nnodes * 3 );
    for ( unsigned int i = 0; i < nnodes; i++ )
    {
        const unsigned int index = GetValue<long>( ifs );
        const float x = GetValue<float>( ifs );
        const float y = GetValue<float>( ifs );
        const float z = GetValue<float>( ifs );
        coords[ 3 * index + 0 ] = x;
        coords[ 3 * index + 1 ] = y;
        coords[ 3 * index + 2 ] = z;
    }

    return coords;
}

kun::UcdObject::Coords GetCoords2( std::ifstream& ifs, unsigned int nnodes )
{
    kun::UcdObject::Coords coords( nnodes * 3 );
    kvs::ValueArray<long> indices = GetValues<long>( ifs, nnodes );
    kvs::ValueArray<float> xcoords = GetValues<float>( ifs, nnodes );
    kvs::ValueArray<float> ycoords = GetValues<float>( ifs, nnodes );
    kvs::ValueArray<float> zcoords = GetValues<float>( ifs, nnodes );
    for ( unsigned int i = 0; i < nnodes; i++ )
    {
        const unsigned int index = indices[i];
        coords[ 3 * index + 0 ] = xcoords[index];
        coords[ 3 * index + 1 ] = ycoords[index];
        coords[ 3 * index + 2 ] = zcoords[index];
    }

    return coords;
}

void CropSphere( kvs::UnstructuredVolumeObject* volume )
{
    kvs::Vec3 min_coord = kvs::Vec3::All( -30 );
    kvs::Vec3 max_coord = kvs::Vec3::All( 30 );

    const kvs::ValueArray<float>& coords = volume->coords();
    const float* pvalues = static_cast<const float*>( volume->values().data() );
    const kvs::ValueArray<kvs::UInt32>& connections = volume->connections();
    const unsigned int ncells = volume->numberOfCells();
    const unsigned int ncellnodes = volume->numberOfCellNodes();
    const unsigned int nvertices = volume->numberOfNodes();

    std::vector<float> cropped_coords;
    std::vector<float> cropped_values;
    unsigned int* table = new unsigned int[nvertices]; // table to keep the corresponding relation of the cropped vertices
    unsigned int count = 0;
    for( unsigned int i = 0; i < nvertices; i++ )
    {
        const float x = coords[ 3 * i ];
        const float y = coords[ 3 * i + 1 ];
        const float z = coords[ 3 * i + 2 ];
        if ( min_coord.x() <= x && x <= max_coord.x() &&
             min_coord.y() <= y && y <= max_coord.y() &&
             min_coord.z() <= z && z <= max_coord.z() )
        {
            cropped_coords.push_back( x );
            cropped_coords.push_back( y );
            cropped_coords.push_back( z );

            cropped_values.push_back( pvalues[i] );

            table[i] = i - count;
        }
        else
        {
            table[i] = 0; // these vertices are not used in the connection
            count++;
        }
    }

    std::cout << cropped_values.size() * 3 << std::endl;
    std::vector<kvs::UInt32> cropped_connections;
    for ( unsigned int i = 0; i < ncells; i++ )
    {
        const unsigned int index = connections[ ncellnodes * i ];
        const float x = coords[ 3 * index ];
        const float y = coords[ 3 * index + 1 ];
        const float z = coords[ 3 * index + 2 ];
        if ( min_coord.x() <= x && x <= max_coord.x() &&
             min_coord.y() <= y && y <= max_coord.y() &&
             min_coord.z() <= z && z <= max_coord.z() ) // Only check the first vertex ???
        {
            for ( unsigned int j = 0; j < ncellnodes; j++ )
            {
                cropped_connections.push_back( table[connections[ ncellnodes * i + j ]] );
            }
        }
    }

    volume->setCoords( kvs::ValueArray<float>( cropped_coords ) );
    volume->setValues( kvs::ValueArray<float>( cropped_values ) );
    volume->setConnections( kvs::ValueArray<kvs::UInt32>( cropped_connections ) );
    volume->setNumberOfCells( cropped_connections.size() / ncellnodes );
    volume->setNumberOfNodes( cropped_values.size() );
    volume->setMinMaxObjectCoords( min_coord, max_coord );
    volume->setMinMaxExternalCoords( min_coord, max_coord );
    volume->updateMinMaxValues();
}

} // end of namespace

namespace kun
{

UcdObject::UcdObject():
    m_cycle_type( UcdObject::UnknownCycleType ),
    m_element_type( UcdObject::UnknownElementType ),
    m_total_nnodes( 0 ),
    m_total_nelems( 0 ),
    m_nelems( 0 ),
    m_ncomponents( 0 ),
    m_component_id( 0 ),
    m_component_veclen( 1 ),
    m_is_skipped( false )
{
}

UcdObject::UcdObject( std::string& filename ):
    m_cycle_type( UcdObject::UnknownCycleType ),
    m_element_type( UcdObject::UnknownElementType ),
    m_total_nnodes( 0 ),
    m_total_nelems( 0 ),
    m_nelems( 0 ),
    m_ncomponents( 0 ),
    m_component_id( 0 ),
    m_component_veclen( 1 ),
    m_filename( filename ),
    m_is_skipped( false )
{
}

bool UcdObject::read()
{
    try
    {
        this->read_control_file();
        std::cout << m_data_filenames[0] << std::endl;
        this->read_data_file();
    }
    catch ( const char* const error )
    {
        kvsMessageError( "%s: %s", m_filename.c_str(), error );
        return false;
    }

    return true;
}

bool UcdObject::skipToValues()
{
    try
    {
        this->read_control_file();
        this->skip_to_values();
    }
    catch ( const char* const error )
    {
        kvsMessageError( "%s: %s", m_filename.c_str(), error );
        return false;
    }

    return true;
}

void UcdObject::read_control_file()
{
    std::ifstream ifs( m_filename.c_str(), std::ios::in );
    if ( !ifs.is_open() ) { throw "Cannot open file."; }

    const std::string path = kvs::File( m_filename ).pathName( true );

    std::string buffer;
    while ( ifs && std::getline( ifs, buffer ) )
    {
        if ( buffer[0] == '#' ) { continue; }
        if ( buffer == "data" ) { m_cycle_type = Data; break; }
        if ( buffer == "geom" ) { m_cycle_type = Geom; break; }
        if ( buffer == "data_geom" ) { m_cycle_type = DataGeom; break; }
    }

    while ( ifs && std::getline( ifs, buffer ) )
    {
        if ( buffer[0] == '#' ) { continue; }
        const std::string data_filename = path + kvs::File::Separator() + buffer;
        m_data_filenames.push_back( data_filename );
    }

    ifs.close();
}

void UcdObject::read_data_file()
{
    m_ifs.open( m_data_filenames[0].c_str(), std::ios::in | std::ios::binary );
    if ( !m_ifs.is_open() ) { throw "Cannot open file."; }

    this->read_file_info();
    this->read_step_info();
    this->read_node_info();
    this->read_elem_info();
    this->read_node_data();

    m_ifs.close();
}

void UcdObject::skip_to_values()
{
	m_ifs.open( m_data_filenames[0].c_str(), std::ios::in | std::ios::binary );
	if ( !m_ifs.is_open() ) { throw "Cannot open file."; }

	this->read_file_info();
	this->read_step_info();
	this->skip_node_info();
	this->skip_elem_info();
}

void UcdObject::read_file_info()
{
    std::string keyword = ::GetString( m_ifs, 7 );
    float version = ::GetValue<float>( m_ifs );
    kvs::IgnoreUnusedVariable( version );

    if ( keyword != "AVSUC64" ) throw "Not supported for reading 32bit binary data.";

#if VERBOSE
    std::cout << "keyword = " << keyword << std::endl;
    std::cout << "version = " << version << std::endl;
#endif
}

void UcdObject::read_step_info()
{
    std::string title = ::GetString( m_ifs, 70 );
    int step_number = ::GetValue<int>( m_ifs );
    int step_time = ::GetValue<int>( m_ifs );
    kvs::IgnoreUnusedVariable( step_number );
    kvs::IgnoreUnusedVariable( step_time );

#if VERBOSE
    std::cout << "title = " << title << std::endl;
    std::cout << "step number = " << step_number << std::endl;
    std::cout << "step time = " << step_time << std::endl;
#endif
}

void UcdObject::read_node_info()
{
    m_total_nnodes = ::GetValue<long>( m_ifs );
    int desc_type = ::GetValue<int>( m_ifs );

    m_coords = desc_type == 1 ?
        ::GetCoords1( m_ifs, m_total_nnodes ) :
        ::GetCoords2( m_ifs, m_total_nnodes );

#if VERBOSE
    std::cout << "total nnodes = " << m_total_nnodes << std::endl;
    std::cout << "description type = " << desc_type << std::endl;
#endif
}

void UcdObject::skip_node_info()
{
	m_total_nnodes = ::GetValue<long>( m_ifs );
	::SkipValue<int>( m_ifs ); // Skip desc_type

	// Skip coordinates
	::SkipValues<long>( m_ifs, m_total_nnodes );
	::SkipValues<float>( m_ifs, m_total_nnodes );
	::SkipValues<float>( m_ifs, m_total_nnodes );
	::SkipValues<float>( m_ifs, m_total_nnodes );
}

void UcdObject::read_elem_info()
{
    m_total_nelems = ::GetValue<long>( m_ifs );

    // Element IDs
    m_ifs.seekg( sizeof(long) * m_total_nelems, std::ios_base::cur );

    // Material numbers
    m_ifs.seekg( sizeof(int) * m_total_nelems, std::ios_base::cur );

    // Element types
    kvs::ValueArray<char> element_types = ::GetValues<char>( m_ifs, m_total_nelems );

    kvs::ValueArray<long> element_counter( 15 );
    element_counter.fill(0);
    for ( unsigned int i = 0; i < m_total_nelems; i++ )
    {
        const int element_type = element_types[i];
        element_counter[ element_type ]++;
    }

#if VERBOSE
    std::cout << "total nelems = " << m_total_nelems << std::endl;
    const std::string element_type_names[15] = {
        "Point", "Line", "Tri", "Quad", "Tet", "Pyr", "Prism", "Hex",
        "Line2", "Tri2", "Quad2", "Tet2", "Pyr2", "Prism2", "Hex2"
    };
    for ( unsigned int i = 0; i < 15; i++ )
    {
        std::cout << "\t" << element_type_names[i] << ": " << element_counter[i] << std::endl;
    }
#endif

    const unsigned int nnodes_per_element[15] = {
        1, 2, 3, 4, 4, 5, 6, 8,
        3, 6, 8, 10, 13, 15, 20
    };

    if ( m_element_type == UnknownElementType ) throw "Reading element type is not spefied.";

    const long nelems = element_counter[ m_element_type ];
    const long nnodes = nnodes_per_element[ m_element_type ];
    kvs::ValueArray<kvs::UInt32> connections( nelems * nnodes );
    kvs::UInt32* pconnections = connections.data();
    for ( unsigned int i = 0; i < m_total_nelems; i++ )
    {
        int element_type( element_types[i] );
        if ( element_type == int( m_element_type ) )
        {
            for ( unsigned int j = 0; j < nnodes_per_element[ element_type ]; j++ )
            {
                *(pconnections++) = ::GetValue<long>( m_ifs );
            }
        }
        else
        {
            m_ifs.seekg( nnodes_per_element[ element_type ] * sizeof(long), std::ios_base::cur );
        }
    }

    m_nelems = nelems;
    m_connections = connections;
}

void UcdObject::skip_elem_info()
{
	m_total_nelems = ::GetValue<long>( m_ifs );
	::SkipValues<long>( m_ifs, m_total_nelems ); // Skip element IDs
	::SkipValues<int>( m_ifs, m_total_nelems ); // Skip material numbers

	// The following is only for heated sphere data. 
	// Assign the cell number to save reading time.
	::SkipValues<char>( m_ifs, m_total_nelems ); // Skip element types
	long tet_number = 27545304;
	long prism_number = 18917887;
	::SkipValues<long>( m_ifs, tet_number * 4 ); // Skip tet connections
	::SkipValues<long>( m_ifs, prism_number * 6 ); // Skip prism connections
}

void UcdObject::read_node_data()
{
    m_ncomponents = ::GetValue<int>( m_ifs );
    int desc_type = ::GetValue<int>( m_ifs );

#if VERBOSE
    std::cout << "ncomponents = " << m_ncomponents << std::endl;
    std::cout << "description type = " << desc_type << std::endl;
#endif

    switch ( desc_type )
    {
    case 1:
    {
        // Not yet implemented
        break;
    }
    case 2:
    {

        for ( unsigned int i = 0; i < m_ncomponents; i++ )
        {
            std::string name = ::GetString( m_ifs, 16 );
            std::string unit = ::GetString( m_ifs, 16 );
            int veclen = ::GetValue<int>( m_ifs );
            int flag = ::GetValue<int>( m_ifs );
            float value = ::GetValue<float>( m_ifs );
            #if VERBOSE
            std::cout << "\tcomp id = " << i << std::endl;
            std::cout << "\tname = " << name << std::endl;
            std::cout << "\tunit = " << unit << std::endl;
            std::cout << "\tveclen = " << veclen << std::endl;
            std::cout << "\tflag = " << flag << std::endl;
            std::cout << "\tvalue = " << value << std::endl;
            #endif
        }


        std::vector<kvs::ValueArray<float> > values_set;
        unsigned int counter = 0;
        unsigned int component_id = m_component_id;
        for ( unsigned int i = 0; i < m_ncomponents; i++ )
        {
            kvs::ValueArray<float> values = ::GetValues<float>( m_ifs, m_total_nnodes );
            if ( component_id == i )
            {
                values_set.push_back( values );
                counter++;
                if ( m_component_veclen != 1 ) { component_id++; }
            }

            if ( counter >= m_component_veclen ) { break; }
        }

        if ( m_component_veclen == 1 ) { m_values = values_set[0]; }
        else if ( m_component_veclen == 3 )
        {
            kvs::ValueArray<float> values( m_total_nnodes * 3 );
            for ( unsigned int i = 0; i < m_total_nnodes; i++ )
            {
                values[ 3 * i + 0 ] = values_set[0][i];
                values[ 3 * i + 1 ] = values_set[1][i];
                values[ 3 * i + 2 ] = values_set[2][i];
            }
            m_values = values;
        }
        break;
    }
    case 3:
    {
        // Not yet implemented
        break;
    }
    case 4:
    {
        // Not yet implemented
        break;
    }
    default: break;
    }
}

float UcdObject::getPressureSphereValue( unsigned int index )
{
	static bool is_skipped_to_pressure = false;
	if( !is_skipped_to_pressure ) 
	{
		this->skipToValues();

		m_ncomponents = ::GetValue<int>( m_ifs );
		::SkipValue<int>( m_ifs );

		for ( unsigned int i = 0; i < m_ncomponents; i++ )
		{
			::SkipValues<char>( m_ifs, 16 );
			::SkipValues<char>( m_ifs, 16 );
			::SkipValue<int>( m_ifs );
			::SkipValue<int>( m_ifs );
			::SkipValue<float>( m_ifs );
		}
		m_pressure_fileg_beg = m_ifs.tellg();
		is_skipped_to_pressure = true;
	}

	// Pressure's component ID is 0
	m_ifs.seekg( m_pressure_fileg_beg + sizeof(float) * index, m_ifs.beg );
	return ::GetValue<float>( m_ifs );
}

float UcdObject::getDensitySphereValue( unsigned int index )
{
	static bool is_skipped_to_density = false;
	if( !is_skipped_to_density ) 
	{
		this->skipToValues();

		m_ncomponents = ::GetValue<int>( m_ifs );
		::SkipValue<int>( m_ifs );

		for ( unsigned int i = 0; i < m_ncomponents; i++ )
		{
			::SkipValues<char>( m_ifs, 16 );
			::SkipValues<char>( m_ifs, 16 );
			::SkipValue<int>( m_ifs );
			::SkipValue<int>( m_ifs );
			::SkipValue<float>( m_ifs );
		}

		// Density's component ID is 2
		::SkipValues<float>( m_ifs, m_total_nnodes );
		::SkipValues<float>( m_ifs, m_total_nnodes );

		m_density_fileg_beg = m_ifs.tellg();
		is_skipped_to_density = true;
	}

	m_ifs.seekg( m_density_fileg_beg + sizeof(float) * index, m_ifs.beg );
	return ::GetValue<float>( m_ifs );
}

float UcdObject::getTemperatureSphereValue( unsigned int index )
{
	static bool is_skipped_to_temperature = false;
	if( !is_skipped_to_temperature ) 
	{
		this->skipToValues();

		m_ncomponents = ::GetValue<int>( m_ifs );
		::SkipValue<int>( m_ifs );

		for ( unsigned int i = 0; i < m_ncomponents; i++ )
		{
			::SkipValues<char>( m_ifs, 16 );
			::SkipValues<char>( m_ifs, 16 );
			::SkipValue<int>( m_ifs );
			::SkipValue<int>( m_ifs );
			::SkipValue<float>( m_ifs );
		}

		// temperature's component ID is 3
		::SkipValues<float>( m_ifs, m_total_nnodes );
		::SkipValues<float>( m_ifs, m_total_nnodes );
		::SkipValues<float>( m_ifs, m_total_nnodes );

		m_temperature_fileg_beg = m_ifs.tellg();
		is_skipped_to_temperature = true;
	}

	m_ifs.seekg( m_temperature_fileg_beg + sizeof(float) * index, m_ifs.beg );
	return ::GetValue<float>( m_ifs );
}

kvs::UnstructuredVolumeObject* UcdObject::toKVSUnstructuredVolumeObject()
{
    this->read();
	kvs::UnstructuredVolumeObject::CellType cell_type =
	    ( this->elementType() == kun::UcdObject::Tet ) ?
	    kvs::UnstructuredVolumeObject::Tetrahedra :
	    ( this->elementType() == kun::UcdObject::Prism ) ?
	    kvs::UnstructuredVolumeObject::Prism :
	    kvs::UnstructuredVolumeObject::UnknownCellType;

	kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeObject();
	volume->setVeclen( this->veclen() );
	volume->setNumberOfNodes( this->totalNumberOfNodes() );
	volume->setNumberOfCells( this->numberOfElements() );
	volume->setCellType( cell_type );
	volume->setCoords( this->coords() );
	volume->setConnections( this->connections() );
	volume->setValues( this->values() );
	volume->updateMinMaxCoords();
	volume->updateMinMaxValues();

	return volume;
}

kvs::UnstructuredVolumeObject* UcdObject::toPressureSphere()
{
    this->setComponentID( 0 );
    this->setElementType( UcdObject::Prism );
    this->setComponentVeclen( 1 );
    kvs::UnstructuredVolumeObject* object = this->toKVSUnstructuredVolumeObject();
    ::CropSphere( object );
    return object;
}

kvs::UnstructuredVolumeObject* UcdObject::toDensitySphere()
{
    this->setComponentID( 2 );
    this->setElementType( UcdObject::Prism );
    this->setComponentVeclen( 1 );
    kvs::UnstructuredVolumeObject* object = this->toKVSUnstructuredVolumeObject();
    ::CropSphere( object );
    return object;
}

kvs::UnstructuredVolumeObject* UcdObject::toTemperatureSphere()
{
    this->setComponentID( 3 );
    this->setElementType( UcdObject::Prism );
    this->setComponentVeclen( 1 );
    kvs::UnstructuredVolumeObject* object = this->toKVSUnstructuredVolumeObject();
    ::CropSphere( object );
    return object;
}

} // end of namespace kun