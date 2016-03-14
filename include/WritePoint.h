//
//  WritePoint.h
//
//
//  Created by Kun Zhao on 2016-01-26 14:11:14.
//
//

#ifndef KUN__WRITEPOINT_H_INCLUDE
#define KUN__WRITEPOINT_H_INCLUDE
 
#include "PointObject.h"
#include "PointExporter.h"
#include "KVSMLObjectKunPoint.h"

namespace kun
{
	
bool WritePoint( kun::PointObject* point, std::string filename )
{
	kun::KVSMLObjectKunPoint* kvsml = new kun::PointExporter<kun::KVSMLObjectKunPoint>( point );
	kvsml -> setWritingDataType(kvs::kvsml::ExternalBinary);
	kvsml -> write( filename.c_str() );
	std::cout << "Finish writing KunPoint object: " << filename << std::endl;
	return true;
}

} // end of namespace kun

 
#endif // KUN__WRITEPOINT_H_INCLUDE