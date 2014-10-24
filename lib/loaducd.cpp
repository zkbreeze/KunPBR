#include "loaducd.h"
#include <kvs/DebugNew>
#include <kvs/AVSUcd>
#include <kvs/Message>
#include <kvs/Vector3>
#include <fstream>

#define TETRA 4
#define PRISM 6

namespace
{
const kvs::UnstructuredVolumeObject::CellType StringToCellType( const std::string& cell_type )
{
    if (      cell_type == "tetrahedra" ) { return kvs::UnstructuredVolumeObject::Tetrahedra; }
    else if ( cell_type == "quadratic tetrahedra" ) { return kvs::UnstructuredVolumeObject::QuadraticTetrahedra; }
    else if ( cell_type == "hexahedra"  ) { return kvs::UnstructuredVolumeObject::Hexahedra;  }
    else if ( cell_type == "quadratic hexahedra"  ) { return kvs::UnstructuredVolumeObject::QuadraticHexahedra;  }
    else if ( cell_type == "pyramid"  ) { return kvs::UnstructuredVolumeObject::Pyramid;  }
    else if ( cell_type == "point"  ) { return kvs::UnstructuredVolumeObject::Point;  }
    else if ( cell_type == "prism"  ) { return kvs::UnstructuredVolumeObject::Prism;  }
    else
    {
        kvsMessageError( "Unknown cell type '%s'.", cell_type.c_str() );
        return kvs::UnstructuredVolumeObject::UnknownCellType;
    }
}

} // end of namespace


namespace takami
{

  long int coord_num;
  long int connection_num;
  long int tet_connection_num;
  long int prism_connection_num;


  int LoadUcd::CheckFileLong(const char *filename){
    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    char* keyward = new char[100];
    memset( keyward, 0, 100 );
    fp.read( keyward, sizeof(char)*7 );
    fp.close();
    if(strcmp(keyward, "AVSUC64") == 0){
      return 0;
    }else if(strcmp(keyward, "AVS UCD") == 0){
      return 1;
    }
    return -1;
  }

LoadUcd::LoadUcd()
{
}

  LoadUcd::LoadUcd( const char* filename , char cell_type)
{
  if(cell_type == TETRA ){
    if(this->CheckFileLong(filename)==0){
      std::cout << "tetra long object" << std::endl;
      this->LoadTetraObject_binary_long(filename);
    }else if(this->CheckFileLong(filename)==1){
      std::cout << "tetra short object" << std::endl;
      kvsMessageError( "LoadUcd cannot import short ucd file");
      //this->LoadTetraObject_binary_short(filename);
    }
  }else if(cell_type == PRISM ){
    if(this->CheckFileLong(filename)==0){
      std::cout << "prism long object" << std::endl;
      this->LoadPrismObject_binary_long(filename);
    }else if(this->CheckFileLong(filename)==1){
      std::cout << "prism short object" << std::endl;
      kvsMessageError( "LoadUcd cannot import short ucd file");
      //this->LoadPrismObject_binary_short(filename);
    }
  }else{
    kvsMessageError( "This file is not binary ucd or file does not exist.");
  }
  return;
}

LoadUcd::LoadUcd( const char* filename , char cell_type, int NthValue)
{
  if(cell_type == TETRA ){
    if(this->CheckFileLong(filename)==0){
      std::cout << "tetra long object" << std::endl;
      this->LoadTetraCoordAndConnection(filename, NthValue);
    }else if(this->CheckFileLong(filename)==1){
      std::cout << "tetra short object" << std::endl;
      kvsMessageError( "LoadUcd cannot import short ucd file");
      //this->LoadTetraObject_binary_short(filename);
    }
  }else if(cell_type == PRISM ){
    if(this->CheckFileLong(filename)==0){
      std::cout << "prism long object" << std::endl;
      this->LoadPrismCoordAndConnection(filename, NthValue);
    }else if(this->CheckFileLong(filename)==1){
      std::cout << "prism short object" << std::endl;
      kvsMessageError( "LoadUcd cannot import short ucd file");
      //this->LoadPrismObject_binary_short(filename);
    }
  }else{
    kvsMessageError( "This file is not binary ucd or file does not exist.");
  }
  return;
}


LoadUcd::~LoadUcd()
{
}

  void LoadUcd::LoadTetraObject_binary_long(const char *filename){
    kvs::ValueArray<kvs::Real32> coords;
    kvs::ValueArray<kvs::Real32> values;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //the number of coord
    fp.read( (char*)&coord_num, sizeof(long int) );

    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //load coord
    std::vector<float> coord_x(coord_num);
    std::vector<float> coord_y(coord_num); 
    std::vector<float> coord_z(coord_num); 

    fp.read( reinterpret_cast<char*>(&coord_x.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_y.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_z.at(0)), sizeof(float) * coord_num);

    coords.allocate( coord_num * 3 );
    for(long int i=0; i<coord_num; i++){
      coords.at(i*3+0) = coord_x[i];
      coords.at(i*3+1) = coord_y[i];
      coords.at(i*3+2) = coord_z[i];
    }

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //calc element histogram
    std::vector<long int>element_histogram(14);

    for(long int i=0;i<connection_num;i++){
      element_histogram.at((int)element_type.at(i))++;
    }
    tet_connection_num = element_histogram.at(4);
    //prism_connection_num = element_histogram.at(6);

    std::vector<long int> tet_connection(tet_connection_num*4);

    long int count=0;
    for(long int i=0;i<connection_num;i++){
      if( (int)element_type.at(i) == 4 ){
	fp.read( reinterpret_cast<char*>(&tet_connection.at(count*4)), 4*sizeof(long int) );
	count++;
      }else if((int)element_type.at(i) == 6 ){
	fp.seekg(6*sizeof(long int), std::ios_base::cur);
      }else{
	std::cout << "this file includes not tetra and prism cell" << std::endl;
      }
    }
    
    kvs::ValueArray<kvs::UInt32> connections;
    connections.allocate(tet_connection_num*4);
    for(long int i=0;i<tet_connection_num*4;i++){
      connections.at(i) = (unsigned int)tet_connection.at(i)  ;
    }

    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    //value load
    long int value_num = coord_num;

    std::vector<float> vel_u(value_num);
    std::vector<float> vel_v(value_num);
    std::vector<float> vel_w(value_num);
    values.allocate( value_num );

    for(int i=0;i<5;i++){
      fp.seekg(sizeof(float)*value_num, std::ios_base::cur);
    }
    fp.read(reinterpret_cast<char*>(&vel_u.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_v.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_w.at(0)), value_num*sizeof(float));
    for(long int i=0;i<value_num;i++){
      values.at(i) = sqrt(vel_u[i]*vel_u[i]+vel_v[i]*vel_v[i]+vel_w[i]*vel_w[i]);
  }


    kvs::UnstructuredVolumeObject::setVeclen(1);
    kvs::UnstructuredVolumeObject::setNumberOfNodes( coord_num );
    kvs::UnstructuredVolumeObject::setNumberOfCells( tet_connection_num );
    kvs::UnstructuredVolumeObject::setCellType( ::StringToCellType( "tetrahedra" ) );
    kvs::UnstructuredVolumeObject::setCoords( coords );
    kvs::UnstructuredVolumeObject::setConnections( connections );
    kvs::UnstructuredVolumeObject::setValues( values );
    kvs::UnstructuredVolumeObject::updateMinMaxCoords();
    kvs::UnstructuredVolumeObject::updateMinMaxValues();

    return;
  }

  void LoadUcd::LoadPrismObject_binary_long(const char *filename){
    kvs::ValueArray<kvs::Real32> coords;
    kvs::ValueArray<kvs::Real32> values;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //the number of coord
    fp.read( (char*)&coord_num, sizeof(long int) );

    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //load coord
    std::vector<float> coord_x(coord_num);
    std::vector<float> coord_y(coord_num); 
    std::vector<float> coord_z(coord_num); 

    fp.read( reinterpret_cast<char*>(&coord_x.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_y.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_z.at(0)), sizeof(float) * coord_num);

    coords.allocate( coord_num * 3 );
    for(long int i=0; i<coord_num; i++){
      coords.at(i*3+0) = coord_x[i];
      coords.at(i*3+1) = coord_y[i];
      coords.at(i*3+2) = coord_z[i];
    }

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //calc element histogram
    std::vector<long int>element_histogram(14);

    for(long int i=0;i<connection_num;i++){
      element_histogram.at((int)element_type.at(i))++;
    }
    //tet_connection_num = element_histogram.at(4);
    prism_connection_num = element_histogram.at(6);

    std::vector<long int> prism_connection(prism_connection_num*6);

    long int count=0;
    for(long int i=0;i<connection_num;i++){
      if( (int)element_type.at(i) == 6 ){
	fp.read( reinterpret_cast<char*>(&prism_connection.at(count*6)), 6*sizeof(long int) );
	count++;
      }else if((int)element_type.at(i) == 4 ){
	fp.seekg(4*sizeof(long int), std::ios_base::cur);
      }else{
	std::cout << "this file includes not tetra and prism cell" << std::endl;
      }
    }
    kvs::ValueArray<kvs::UInt32> connections;
    connections.allocate(prism_connection_num*6);
    for(long int i=0;i<prism_connection_num*6;i++){
      connections.at(i) = (unsigned int)prism_connection.at(i)  ; 
    }
    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    //value load
    long int value_num = coord_num;

    std::vector<float> vel_u(value_num);
    std::vector<float> vel_v(value_num);
    std::vector<float> vel_w(value_num);
    values.allocate( value_num );

    for(int i=0;i<5;i++){
      fp.seekg(sizeof(float)*value_num, std::ios_base::cur);
    }
    fp.read(reinterpret_cast<char*>(&vel_u.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_v.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_w.at(0)), value_num*sizeof(float));
    for(long int i=0;i<value_num;i++){
      values.at(i) = sqrt(vel_u[i]*vel_u[i]+vel_v[i]*vel_v[i]+vel_w[i]*vel_w[i]);
    }
    
    std::vector<unsigned int> tmp_connection;
    if(6*(unsigned long int)prism_connection_num != connections.size()){
      kvsMessageError( "volume has %u cells and volume->connections().size() is %u.\nCutPrism's volume must be only prism.",prism_connection_num, connections.size() );
      return;
    }
    count = 0;
    for(unsigned int i=0;i<prism_connection_num;i++){
	if(coords.data()[3*connections.data()[i*6] ] < -300.0 || coords.data()[3*connections.data()[i*6] ] > 1000.0 ){
	  count++;
	}else{
	  for(unsigned int j=0;j<6;j++){
	    tmp_connection.push_back(connections.data()[i*6+j]);
	  }
	}
      }
    kvs::ValueArray<kvs::UInt32> connections_cut( tmp_connection.data(), tmp_connection.size() );
    
    kvs::UnstructuredVolumeObject::setVeclen(1);
    kvs::UnstructuredVolumeObject::setNumberOfNodes( coord_num );
    kvs::UnstructuredVolumeObject::setNumberOfCells( prism_connection_num-count );
    kvs::UnstructuredVolumeObject::setCellType( ::StringToCellType( "prism" ) );
    kvs::UnstructuredVolumeObject::setCoords( coords );
    kvs::UnstructuredVolumeObject::setConnections( connections_cut );
    kvs::UnstructuredVolumeObject::setValues( values );
    kvs::UnstructuredVolumeObject::updateMinMaxCoords();
    kvs::UnstructuredVolumeObject::updateMinMaxValues();
    return;
  }

void LoadUcd::LoadTetraCoordAndConnection( const char* filename, int NthValue){
    kvs::ValueArray<kvs::Real32> coords;
    kvs::ValueArray<kvs::Real32> values;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //the number of coord
    fp.read( (char*)&coord_num, sizeof(long int) );

    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //load coord
    std::vector<float> coord_x(coord_num);
    std::vector<float> coord_y(coord_num); 
    std::vector<float> coord_z(coord_num); 

    fp.read( reinterpret_cast<char*>(&coord_x.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_y.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_z.at(0)), sizeof(float) * coord_num);

    coords.allocate( coord_num * 3 );
    for(long int i=0; i<coord_num; i++){
      coords.at(i*3+0) = coord_x[i];
      coords.at(i*3+1) = coord_y[i];
      coords.at(i*3+2) = coord_z[i];
    }

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //calc element histogram
    std::vector<long int>element_histogram(14);

    for(long int i=0;i<connection_num;i++){
      element_histogram.at((int)element_type.at(i))++;
    }
    tet_connection_num = element_histogram.at(4);
    //prism_connection_num = element_histogram.at(6);

    std::vector<long int> tet_connection(tet_connection_num*4);

    long int count=0;
    for(long int i=0;i<connection_num;i++){
      if( (int)element_type.at(i) == 4 ){
	fp.read( reinterpret_cast<char*>(&tet_connection.at(count*4)), 4*sizeof(long int) );
	count++;
      }else if((int)element_type.at(i) == 6 ){
	fp.seekg(6*sizeof(long int), std::ios_base::cur);
      }else{
	std::cout << "this file includes not tetra and prism cell" << std::endl;
      }
    }
    
    kvs::ValueArray<kvs::UInt32> connections;
    connections.allocate(tet_connection_num*4);
    for(long int i=0;i<tet_connection_num*4;i++){
      connections.at(i) = (unsigned int)tet_connection.at(i)  ;
    }

    kvs::UnstructuredVolumeObject::setVeclen(1);
    kvs::UnstructuredVolumeObject::setNumberOfNodes( coord_num );
    kvs::UnstructuredVolumeObject::setNumberOfCells( tet_connection_num );
    kvs::UnstructuredVolumeObject::setCellType( ::StringToCellType( "tetrahedra" ) );
    kvs::UnstructuredVolumeObject::setCoords( coords );
    kvs::UnstructuredVolumeObject::setConnections( connections );
    kvs::UnstructuredVolumeObject::updateMinMaxCoords();

    if(NthValue == 0){
      return;
    }
    if(NthValue < 0){
      kvsMessageError("In LoadUcd(filename, cell_type, NthValue), NthValue start from 1.");
      return;
    }

    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    //value load
    long int value_num = coord_num;

    std::vector<float> vel_u(value_num);
    std::vector<float> vel_v(value_num);
    std::vector<float> vel_w(value_num);
    values.allocate( value_num );

    for(int i=0;i<NthValue-1;i++){
      fp.seekg(sizeof(float)*value_num, std::ios_base::cur);
    }
    fp.read(reinterpret_cast<char*>(&vel_u.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_v.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_w.at(0)), value_num*sizeof(float));
    for(long int i=0;i<value_num;i++){
      values.at(i) = sqrt(vel_u[i]*vel_u[i]+vel_v[i]*vel_v[i]+vel_w[i]*vel_w[i]);
    }

    kvs::UnstructuredVolumeObject::setValues( values );
    kvs::UnstructuredVolumeObject::updateMinMaxValues();
    return;
  }

void LoadUcd::LoadPrismCoordAndConnection(const char *filename, int NthValue){
    kvs::ValueArray<kvs::Real32> coords;
    kvs::ValueArray<kvs::Real32> values;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //the number of coord
    fp.read( (char*)&coord_num, sizeof(long int) );

    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //load coord
    std::vector<float> coord_x(coord_num);
    std::vector<float> coord_y(coord_num); 
    std::vector<float> coord_z(coord_num); 

    fp.read( reinterpret_cast<char*>(&coord_x.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_y.at(0)), sizeof(float) * coord_num);
    fp.read( reinterpret_cast<char*>(&coord_z.at(0)), sizeof(float) * coord_num);

    coords.allocate( coord_num * 3 );
    for(long int i=0; i<coord_num; i++){
      coords.at(i*3+0) = coord_x[i];
      coords.at(i*3+1) = coord_y[i];
      coords.at(i*3+2) = coord_z[i];
    }

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //calc element histogram
    std::vector<long int>element_histogram(14);

    for(long int i=0;i<connection_num;i++){
      element_histogram.at((int)element_type.at(i))++;
    }
    //tet_connection_num = element_histogram.at(4);
    prism_connection_num = element_histogram.at(6);

    std::vector<long int> prism_connection(prism_connection_num*6);

    long int count=0;
    for(long int i=0;i<connection_num;i++){
      if( (int)element_type.at(i) == 6 ){
	fp.read( reinterpret_cast<char*>(&prism_connection.at(count*6)), 6*sizeof(long int) );
	count++;
      }else if((int)element_type.at(i) == 4 ){
	fp.seekg(4*sizeof(long int), std::ios_base::cur);
      }else{
	std::cout << "this file includes not tetra and prism cell" << std::endl;
      }
    }
    kvs::ValueArray<kvs::UInt32> connections;
    connections.allocate(prism_connection_num*6);
    for(long int i=0;i<prism_connection_num*6;i++){
      connections.at(i) = (unsigned int)prism_connection.at(i)  ; 
    }

    
    std::vector<unsigned int> tmp_connection;
    if(6*(unsigned long int)prism_connection_num != connections.size()){
      kvsMessageError( "volume has %u cells and volume->connections().size() is %u.\nCutPrism's volume must be only prism.",prism_connection_num, connections.size() );
      return;
    }
    count = 0;
    for(unsigned int i=0;i<prism_connection_num;i++){
	if(coords.data()[3*connections.data()[i*6] ] < -300.0 || coords.data()[3*connections.data()[i*6] ] > 1000.0 ){
	  count++;
	}else{
	  for(unsigned int j=0;j<6;j++){
	    tmp_connection.push_back(connections.data()[i*6+j]);
	  }
	}
      }
    kvs::ValueArray<kvs::UInt32> connections_cut( tmp_connection.data(), tmp_connection.size() );
    
    kvs::UnstructuredVolumeObject::setVeclen(1);
    kvs::UnstructuredVolumeObject::setNumberOfNodes( coord_num );
    kvs::UnstructuredVolumeObject::setNumberOfCells( tet_connection_num );
    kvs::UnstructuredVolumeObject::setCellType( ::StringToCellType( "tetrahedra" ) );
    kvs::UnstructuredVolumeObject::setCoords( coords );
    kvs::UnstructuredVolumeObject::setConnections( connections_cut );
    kvs::UnstructuredVolumeObject::updateMinMaxCoords();

    if(NthValue == 0){
      return;
    }
    if(NthValue < 0){
      kvsMessageError("In LoadUcd(filename, cell_type, NthValue), NthValue start from 1.");
      return;
    }

    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    //value load
    long int value_num = coord_num;

    std::vector<float> vel_u(value_num);
    std::vector<float> vel_v(value_num);
    std::vector<float> vel_w(value_num);
    values.allocate( value_num );

    for(int i=0;i<NthValue-1;i++){
      fp.seekg(sizeof(float)*value_num, std::ios_base::cur);
    }
    fp.read(reinterpret_cast<char*>(&vel_u.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_v.at(0)), value_num*sizeof(float));
    fp.read(reinterpret_cast<char*>(&vel_w.at(0)), value_num*sizeof(float));
    for(long int i=0;i<value_num;i++){
      values.at(i) = sqrt(vel_u[i]*vel_u[i]+vel_v[i]*vel_v[i]+vel_w[i]*vel_w[i]);
    }

    kvs::UnstructuredVolumeObject::setValues( values );
    kvs::UnstructuredVolumeObject::updateMinMaxValues();
    return;
  }


std::vector<float> LoadUcdValue(const char *filename, int NthValue){
    kvs::ValueArray<kvs::Real32> coords;
    long int coord_num;
    long int connection_num;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //load coord_num = value_num
    fp.read( (char*)&coord_num, sizeof(long int) );
    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //skip coord
    fp.seekg(coord_num*sizeof(float)*3, std::ios_base::cur);

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //skip connection data
    long int total_connection_size=0;
    for(unsigned long int i=0;i<element_type.size();i++){
      if(element_type.at(i) == 4){
	total_connection_size = total_connection_size+4;
      }else if(element_type.at(i) == 6){
	total_connection_size = total_connection_size+6;
      }
    }
    fp.seekg(total_connection_size*sizeof(long int), std::ios_base::cur);
    
    kvs::ValueArray<kvs::UInt32> connections;

    //load veclen
    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    long int value_num = coord_num;
    std::vector<float> values(value_num);

    char tmp_name[16]={0};
    memcpy(tmp_name, &value_name[NthValue*16], 15);
    std::cout << "loading " << tmp_name << std::endl;

    for(int i=0;i<NthValue;i++){
      fp.seekg(sizeof(float)*value_num, std::ios_base::cur);
    }
    
    fp.read(reinterpret_cast<char*>(&values.at(0)), value_num*sizeof(float));
    fp.close();
    return(values);
}


std::string LoadUcdName(const char *filename, int NthValue){
    kvs::ValueArray<kvs::Real32> coords;
    long int coord_num;
    long int connection_num;

    std::ifstream fp( filename, std::ios::in | std::ios::binary );
    //skip keyward
    fp.seekg(7*sizeof(char), std::ios_base::beg);
    //skip ucd's version
    fp.seekg(1*sizeof(float), std::ios_base::cur);
    //skip file title
    fp.seekg(70*sizeof(char), std::ios_base::cur);
    //skip n-th step
    fp.seekg(1*sizeof(int), std::ios_base::cur);
    //skip time
    fp.seekg(1*sizeof(float), std::ios_base::cur);

    //load coord_num = value_num
    fp.read( (char*)&coord_num, sizeof(long int) );
    //skip coord_describing_type
    fp.seekg(1*sizeof(int), std::ios_base::cur);

    //skip coord identifier
    fp.seekg(coord_num*sizeof(long int), std::ios_base::cur);

    //skip coord
    fp.seekg(coord_num*sizeof(float)*3, std::ios_base::cur);

    //load connection_num 
    fp.read( (char*)&connection_num, sizeof(long int) );

    //skip connection identifier
    fp.seekg(connection_num*sizeof(long int), std::ios_base::cur);

    //skip material array
    fp.seekg(connection_num*sizeof(int), std::ios_base::cur);

    //load element type
    std::vector<char> element_type(connection_num*sizeof(char));
    fp.read( reinterpret_cast<char*>(&element_type.at(0)), sizeof(char)*connection_num );

    //skip connection data
    long int total_connection_size=0;
    for(unsigned long int i=0;i<element_type.size();i++){
      if(element_type.at(i) == 4){
	total_connection_size = total_connection_size+4;
      }else if(element_type.at(i) == 6){
	total_connection_size = total_connection_size+6;
      }
    }
    fp.seekg(total_connection_size*sizeof(long int), std::ios_base::cur);
    
    kvs::ValueArray<kvs::UInt32> connections;

    //load veclen
    int veclen;
    fp.read( (char*)&veclen, sizeof(int) );

    //skip value describing type
    fp.seekg(sizeof(int), std::ios_base::cur);

    std::vector<char> value_name(16*veclen);
    std::vector<int> value_veclen(veclen);

    for(int i=0;i<veclen;i++){
      fp.read(reinterpret_cast<char*>(&value_name.at(16*i)), 16*sizeof(char));
      fp.seekg(sizeof(char)*16, std::ios_base::cur);
      fp.read(reinterpret_cast<char*>(&value_veclen.at(i)), sizeof(int));
      fp.seekg(sizeof(int), std::ios_base::cur);
      fp.seekg(sizeof(float), std::ios_base::cur);
    }

    long int value_num = coord_num;
    std::vector<float> values(value_num);

    char tmp_name[16]={0};
    memcpy(tmp_name, &value_name[NthValue*16], 15);

    std::string return_name;
    return_name.append(tmp_name);

    fp.close();
    return(tmp_name);
}

////end of LoadUcd class

} // end of namespace takami
