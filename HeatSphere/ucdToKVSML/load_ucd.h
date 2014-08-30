#ifndef LOAD__UCD__H
#define LOAD__UCD__H


//#define AVSUCD_OUTPUT 1
//これを入れると読み取ったデータをavsucdで出力する さらにkvsconv -ucd2kvsml すればkvsmlも作れる

#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>

#include <iostream>


bool CheckFileLong(const char *filename);

kvs::UnstructuredVolumeObject* LoadTetraObject_binary_long(const char *filename );

kvs::UnstructuredVolumeObject* LoadTetraObject_binary_short(const char *filename );

kvs::UnstructuredVolumeObject* LoadPrismObject_binary_long(const char *filename );

kvs::UnstructuredVolumeObject* CreateUnstructuredVolumeObject(const char* filename,char cell_type );


void writeStatusToFile(float *coord,unsigned int *connection, float *value, int coord_num, int connection_num, int value_num, char cell_type);

kvs::UnstructuredVolumeObject* LoadPrismObject_ascii(const char *filename );


#endif
