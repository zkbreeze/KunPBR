//
//  PointImporter.h
//
//
//  Created by Kun Zhao on 2014-09-04 16:37:28.
//
//

#ifndef KUN__POINTIMPORTER_H_INCLUDE
#define KUN__POINTIMPORTER_H_INCLUDE
 
#include <kvs/ImporterBase>
#include <kvs/Module>
#include "PointObject.h"
#include "KVSMLObjectKunPoint.h"


namespace kun
{

/*==========================================================================*/
/**
 *  @brief  Point object importer class.
 */
/*==========================================================================*/
class PointImporter : public kvs::ImporterBase, public kun::PointObject
{
    kvsModule( kun::PointImporter, Importer );
    kvsModuleBaseClass( kvs::ImporterBase );
    kvsModuleSuperClass( kun::PointObject );

public:

    PointImporter();
    PointImporter( const std::string& filename );
    PointImporter( const std::string& filename, const float fraction );
    PointImporter( const kvs::FileFormatBase* file_format );
    virtual ~PointImporter();

    SuperClass* exec( const kvs::FileFormatBase* file_format );

private:

    void import( const kun::KVSMLObjectKunPoint* kvsml );
    void set_min_max_coord();
};

} // end of namespace kun
 
#endif // KUN__POINTIMPORTER_H_INCLUDE