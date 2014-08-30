#include "cut_prism.h"

#include <kvs/UnstructuredVolumeObject>
#include <vector>

kvs::UnstructuredVolumeObject* CutPrism(float min_value, float max_value, kvs::UnstructuredVolumeObject* volume){

  std::vector<unsigned int> tmp_connection;
  unsigned int count=0;

  for(unsigned int i=0;i<volume->numberOfCells();i++){
    if(volume->coords()[3*volume->connections()[i*6] ] < min_value || volume->coords()[3*volume->connections()[i*6] ] > max_value){
      count++;
    }else{
      for(unsigned int j=0;j<6;j++){
	tmp_connection.push_back(volume->connections()[i*6+j]);
      }
    }
  }
  printf("count %u\n",count);
  kvs::ValueArray<kvs::UInt32> connections( tmp_connection.data(), tmp_connection.size() );
  volume->setConnections( connections );
  volume->setNumberOfCells( tmp_connection.size() / 6 );
  return (volume);
}
