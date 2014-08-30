//UnstructuredVolumeを読み込むところはtetraとprismに変えた
//prism tetra bothのオプションを追加

//クンさんのプログラムはそのままだとPointObject->colors()をもたない
//けどPointObject->add()ではcolors()を必ず使っていたのでエラーになった
//colorsの部分をgotoでスキップした


//
//  main.cpp
//
//
//  Created by Kun Zhao on 2014/05/23.
//
//


#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/TransferFunction>
#include <kvs/ColorMap>
#include <kvs/OpacityMap>
#include <kvs/HydrogenVolumeData>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/CommandLine>
#include <kvs/glut/RadioButton>
#include <kvs/glut/RadioButtonGroup>
#include <kvs/PointImporter>

#include <kvs/KVSMLObjectPoint>
#include <kvs/PointExporter>
#include <kvs/KVSMLObjectUnstructuredVolume>

#include "load_ucd.h"
#include "cut_prism.h"
#include <kvs/UnstructuredVolumeExporter>

#include "PointObject.h"

#include <kvs/CellByCellMetropolisSampling>
#include <kvs/ParticleBasedRenderer>

#include <sys/time.h>

#define TETRA 4

#define PRISM 6



int main( int argc, char** argv )
{    
    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "tetra", "Input tetra Volume Object (binary_ucd)", 1, false );
    param.addOption( "prism", "Input prism Volume Object (binary_ucd)", 1, false );
    param.addOption( "output", "output filename", 1, false );
    param.addOption( "no-output", "dont output", 0, false );
    
    if ( !param.parse() ) return 1;

    kvs::VolumeObjectBase* volume = NULL;

    if ( param.hasOption( "output" )==0 &&param.hasOption("no-output")==0){
      printf("output filenameが必要\n");
      return -1;
  }


  if ( param.hasOption( "tetra" ) )
      volume = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "tetra" ).c_str() ,TETRA);
  else if ( param.hasOption( "prism" ) )
  {
      volume = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "prism" ).c_str() ,PRISM);
      CutPrism(-300, 1000, static_cast<kvs::UnstructuredVolumeObject *>(volume) );
  }
  else 
  {
      printf("引数にprismかtetraをつける\n");
      return -1;
  }


  volume->print(std::cout);

  if(param.hasOption("no-output")){
      return 0;
  }
  kvs::KVSMLObjectUnstructuredVolume* kvsml = new kvs::UnstructuredVolumeExporter<kvs::KVSMLObjectUnstructuredVolume>(static_cast<kvs::UnstructuredVolumeObject*>(volume) );
  kvsml -> setWritingDataType(kvs::KVSMLObjectUnstructuredVolume::ExternalBinary);
  kvsml -> write(param.optionValue<std::string>( "output" ).c_str());


  return 0;
}
