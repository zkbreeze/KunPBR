//
//  KVSMLObjectKunPoint.h
//
//
//  Created by Kun Zhao on 2014-10-03 11:16:52.
//
//

// Use kvs::kvsml::WrittingDataType

#ifndef KUN__KVSMLOBJECTKUNPOINT_H_INCLUDE
#define KUN__KVSMLOBJECTKUNPOINT_H_INCLUDE
 
#include <kvs/FileFormatBase>
#include <kvs/ValueArray>
#include <kvs/AnyValueArray>
#include <kvs/Type>
#include <kvs/Vector3>
#include <kvs/Indent>
#include <Core/FileFormat/KVSML/KVSMLTag.h>
#include <Core/FileFormat/KVSML/ObjectTag.h>
#include <Core/FileFormat/KVSML/DataWriter.h>


namespace kun
{

/*===========================================================================*/
/**
 *  @brief  KVSML point object format.
 */
/*===========================================================================*/
class KVSMLObjectKunPoint : public kvs::FileFormatBase
{
public:

    typedef kvs::FileFormatBase BaseClass;

private:

    kvs::kvsml::KVSMLTag m_kvsml_tag; ///< KVSML tag information
    kvs::kvsml::ObjectTag m_object_tag; ///< Object tag information
    kvs::kvsml::WritingDataType m_writing_type; ///< writing data type
    kvs::ValueArray<kvs::Real32> m_coords; ///< coordinate array
    kvs::ValueArray<kvs::UInt8> m_colors; ///< color(r,g,b) array
    kvs::ValueArray<kvs::Real32> m_normals; ///< normal array
    kvs::ValueArray<kvs::Real32> m_sizes; ///< size array

    /*ADD*/
    size_t m_veclen;
    double m_min_value;
    double m_max_value;
    bool m_has_minmax_values;
    kvs::AnyValueArray m_values;

public:

	/*ADD*/
	bool ReadValueData(
    const kvs::XMLNode::SuperClass* parent,
    const size_t nvalues,
    kvs::AnyValueArray* values );

    /*ADD*/
	bool WriteValueData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::AnyValueArray values );

    static bool CheckExtension( const std::string& filename );
    static bool CheckFormat( const std::string& filename );

public:

    KVSMLObjectKunPoint();
    KVSMLObjectKunPoint( const std::string& filename );

    const kvs::kvsml::KVSMLTag& KVSMLTag() const { return m_kvsml_tag; }
    const kvs::kvsml::ObjectTag& objectTag() const { return m_object_tag; }
    const kvs::ValueArray<kvs::Real32>& coords() const { return m_coords; }
    const kvs::ValueArray<kvs::UInt8>& colors() const { return m_colors; }
    const kvs::ValueArray<kvs::Real32>& normals() const { return m_normals; }
    const kvs::ValueArray<kvs::Real32>& sizes() const { return m_sizes; }

    /*ADD*/
    const kvs::AnyValueArray& values() const { return m_values; }
    void setVeclen( const size_t veclen ) { m_veclen = veclen; }
    size_t veclen() const { return m_veclen; }

    void setWritingDataType( const kvs::kvsml::WritingDataType type ) { m_writing_type = type; }
    void setCoords( const kvs::ValueArray<kvs::Real32>& coords ) { m_coords = coords; }
    void setColors( const kvs::ValueArray<kvs::UInt8>& colors ) { m_colors = colors; }
    void setNormals( const kvs::ValueArray<kvs::Real32>& normals ) { m_normals = normals; }
    void setSizes( const kvs::ValueArray<kvs::Real32>& sizes ) { m_sizes = sizes; }

    /*ADD*/
    void setMinMaxValues( const double min_value, const double max_value ) 
    {
    	m_min_value = min_value;
    	m_max_value = max_value;
    	m_has_minmax_values = true;
    }
    void setValues( const kvs::AnyValueArray values ) { m_values = values; }

    void print( std::ostream& os, const kvs::Indent& indent = kvs::Indent(0) ) const;
    bool read( const std::string& filename );
    bool write( const std::string& filename );
};

} // end of namespace kun
 
#endif // KUN__KVSMLOBJECTKUNPOINT_H_INCLUDE