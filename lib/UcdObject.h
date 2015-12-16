//
//  UcdObject.h
//
//
//  Created by Kun Zhao on 2015-12-07 17:22:32.
//
//

#ifndef KUN__UCDOBJECT_H_INCLUDE
#define KUN__UCDOBJECT_H_INCLUDE

#include <vector>
#include <string>
#include <fstream>
#include <kvs/FileFormatBase>
#include <kvs/ValueArray>
#include <kvs/Type>
#include <kvs/UnstructuredVolumeObject>

#define VERBOSE 1

namespace kun
{

class UcdObject
{
public:
    typedef kvs::ValueArray<kvs::Real32> Coords;
    typedef kvs::ValueArray<kvs::UInt32> Connections;
    typedef kvs::ValueArray<kvs::Real32> Values;

    enum CycleType
    {
        UnknownCycleType = -1,
        Data = 0,
        Geom,
        DataGeom
    };

    enum ElementType
    {
        UnknownElementType = -1,
        Point = 0,
        Line,
        Tri,
        Quad,
        Tet,
        Pyr,
        Prism,
        Hex,
        Line2,
        Tri2,
        Quad2,
        Tet2,
        Pyr2,
        Prism2,
        Hex2
    };

private:

    CycleType m_cycle_type;
    ElementType m_element_type;
    std::vector<std::string> m_data_filenames;
    unsigned int m_total_nnodes; // total num. of nodes
    unsigned int m_total_nelems; // total num. of elems
    unsigned int m_nelems; // num. of elems specified by the given element type
    unsigned int m_ncomponents;
    unsigned int m_component_id;
    unsigned int m_component_veclen;
    Coords m_coords; // coordinate array
    Connections m_connections; // connection array
    Values m_values; // value array

    std::string m_filename; // the control filename
    bool m_is_skipped;
    std::ifstream m_ifs; // file stream of the data file
    long m_pressure_fileg_beg;
    long m_density_fileg_beg;
    long m_temperature_fileg_beg;

public:

    UcdObject();
    UcdObject( std::string& filename );
    void setFilename( std::string& filename ){ m_filename = filename; } // set the control file name
    void setElementType( ElementType element_type ) { m_element_type = element_type; }
    void setComponentID( unsigned int id ) { m_component_id = id; }
    void setComponentVeclen( unsigned int veclen ) { m_component_veclen = veclen; }
    unsigned int totalNumberOfNodes() const { return m_total_nnodes; }
    unsigned int totalNumberOfElements() const { return m_total_nelems; }
    unsigned int numberOfElements() const { return m_nelems; }
    unsigned int veclen() const { return m_component_veclen; }
    ElementType elementType() const { return m_element_type; }
    const Coords& coords() const { return m_coords; }
    const Connections& connections() const { return m_connections; }
    const Values& values() const { return m_values; }
    bool read();
    kvs::UnstructuredVolumeObject* toKVSUnstructuredVolumeObject(); // The component ID need to be set before this.
                                                                    // Default component ID is 0 without any setting.

    bool skipToValues();
    // Load one value without consuming other memory
    float getPressureSphereValue( unsigned int index ); 
    float getDensitySphereValue( unsigned int index );
    float getTemperatureSphereValue( unsigned int index );

    kvs::UnstructuredVolumeObject* toPressureSphere();
    kvs::UnstructuredVolumeObject* toDensitySphere();
    kvs::UnstructuredVolumeObject* toTemperatureSphere();

private:

    void read_control_file();
    void read_data_file();
    void read_file_info();
    void read_step_info();
    void read_node_info();
    void read_elem_info();
    void read_node_data();

    void skip_to_values();
    void skip_node_info();
    void skip_elem_info();

private:

    bool write( const std::string& ) { return false; }
};

} // end of namespace kun

 
#endif // KUN__UCDOBJECT_H_INCLUDE

