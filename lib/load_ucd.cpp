//UCDbinaryファイルは接続情報の頂点番号（要素の構成配列）が1からはじまる場合と0からはじまる場合で特別な区別がない
//接続情報の頂点番号の最小値を自分で0にする必要がある（kvsは最小値が0のみだったはず）
//ここを追加した

//tetraとprism両方とも64bit 32bit対応



#include "load_ucd.h"

#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>

#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <iostream>

#include <math.h>

#include <fstream>

#define TETRA 4
#define PRISM 6


bool CheckFileLong(const char *filename){
  //FILE *fp;
  //fp = fopen(filename, "rb");

  std::ifstream fp( filename, std::ios::in | std::ios::binary );

  //std::string keyward;
  char* keyward2 = new char[100];
  memset( keyward2, 0, 100 );
  
  fp.read( keyward2, sizeof(char)*7 );

  std::cout << std::string(keyward2) << std::endl;
  //fread(keyward2, sizeof(char), 7, fp);
  //keyward = keyward2;
  //printf("%s\nAVSUC64\n",keyward2);
  //fclose(fp);
  fp.close();
  //if(keyward == "AVSUC64"){
  if(strcmp(keyward2, "AVSUC64") == 0){
    return 0;
  }else{
    return -1;
  }
  return 0;
}

kvs::UnstructuredVolumeObject* LoadTetraObject_binary_long(const char *filename ){

  kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
  FILE *fp;
  fp = fopen(filename, "rb");

  //char keyward[7];
  //fread(keyward, sizeof(char), 7, fp);
  //printf("%s\n",keyward);
  fseek(fp,7*sizeof(char),SEEK_CUR);

  //float version;
  //fread(&version, sizeof(float), 1, fp);
  //printf("%f\n",version);
  fseek(fp,1*sizeof(float),SEEK_CUR);

  char title[70];
  fread(title, sizeof(char), 70, fp);
  //printf("%s\n",title);

  int step_num;
  fread(&step_num, sizeof(int), 1, fp);
  float step_time;
  fread(&step_time, sizeof(float), 1, fp);
  printf("stemp is %d\ntime is %f\n",step_num,step_time);

  long int coord_num;
  fread(&coord_num, sizeof(long int), 1, fp);
  printf("coord is %ld\n",coord_num);
  
  int coord_describing_type;
  fread(&coord_describing_type, sizeof(int), 1, fp);


  long int *coord_num2;
  float *coord_x;
  float *coord_y;
  float *coord_z;
  coord_num2 = (long int *)calloc(coord_num, sizeof(long int));
  coord_x = (float *)calloc(coord_num, sizeof(float));
  coord_y = (float *)calloc(coord_num, sizeof(float));
  coord_z = (float *)calloc(coord_num, sizeof(float));

  fread(coord_num2, sizeof(long int), coord_num, fp);
  long int count = 0;
  for(long int i=0;i<coord_num-1;i++){
    if(coord_num2[i+1] - coord_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("coordが番号順に並んでいないからcoord_num2を使うようにプログラムを作る  %ld\n",count);
  }
  free(coord_num2);

  fread(coord_x, sizeof(float), coord_num, fp);
  fread(coord_y, sizeof(float), coord_num, fp);
  fread(coord_z, sizeof(float), coord_num, fp); 
  
  float *coord;
  coord = (float *)calloc(coord_num*3, sizeof(float));

  float min_coord[3]={100,100,100}, max_coord[3] = {-100,-100,-100};
  for(long int i=0;i<coord_num;i++){
    if(min_coord[0]>coord_x[i]){
      min_coord[0]=coord_x[i];
    }
    if(min_coord[1]>coord_y[i]){
      min_coord[1]=coord_y[i];
    }
    if(min_coord[2]>coord_z[i]){
      min_coord[2]=coord_z[i];
    }
    if(max_coord[0]<coord_x[i]){
      max_coord[0]=coord_x[i];
    }
    if(max_coord[1]<coord_y[i]){
      max_coord[1]=coord_y[i];
    }
    if(max_coord[2]<coord_z[i]){
      max_coord[2]=coord_z[i];
    }
  }
  printf("min_coord is %f %f %f\nmax_coord is %f %f %f\n",min_coord[0], min_coord[1], min_coord[2], max_coord[0], max_coord[1], max_coord[2]);

  /////////////////coord[i*3+0]がcoord[i+0]になってたからエラーになってた
  for(long int i=0;i<coord_num;i++){
    coord[i*3+0] = coord_x[i];
    coord[i*3+1] = coord_y[i];
    coord[i*3+2] = coord_z[i];
  }
  free(coord_x);
  free(coord_y);
  free(coord_z);

  long int connection_num;
  fread(&connection_num, sizeof(long int), 1, fp);
  printf("total connection = %ld\n",connection_num);
  
  long int *connection_num2;
  connection_num2 = (long int *)calloc(connection_num, sizeof(long int));
  fread(connection_num2, sizeof(long int), connection_num, fp);

  count = 0;
  for(long int i=0;i<connection_num-1;i++){
    if(connection_num2[i+1] - connection_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("connectionが番号順に並んでいないからconnection_num2を使うようにプログラムを作る\n");
  }
  free(connection_num2);
  
  //int *material_array;
  //material_array = (int *)calloc(connection_num, sizeof(int));
  //fread(material_array, sizeof(int), connection_num, fp);
  fseek(fp,connection_num*sizeof(int),SEEK_CUR);

  char *element_type;
  element_type = (char *)calloc(connection_num, sizeof(char));
  fread(element_type, sizeof(char), connection_num, fp);

  long int element_histogram[14]={0};
  for(long int i=0;i<connection_num;i++){
    element_histogram[(int)element_type[i]]++;
  }

  for(int i=0;i<14;i++){
    if(i==4){
      printf("type 4  is %ld  (tetrahedra)\n",element_histogram[i]);
    }else if(i==6){
      printf("type 6  is %ld  (prism)\n",element_histogram[i]);
    }else if(i==7){
      printf("type 7  is %ld  (hexahedra)\n",element_histogram[i]);
    }else{
      printf("type%2d  is %ld\n",i, element_histogram[i]);
    }
  }
  long int tet_connection_num = element_histogram[4];

  long int *connection;

  connection = (long int *)calloc(element_histogram[4]*4 + element_histogram[6]*6,sizeof(long int));

  fread(connection, sizeof(long int), element_histogram[4]*4 + element_histogram[6]*6, fp);

  //connectionの最小値が0か1かを判定

  long int min_connection=10;
  for(long i=0;i<element_histogram[4]*4+element_histogram[6]*6;i++){
    if(min_connection > connection[i]){
      min_connection = connection[i];
    }
  }

  unsigned int *tet_connection;
  tet_connection = (unsigned int *)calloc(element_histogram[4]*4,sizeof(unsigned int));

  long int tet_count=0,prism_count=0;
  count = 0;
  for(long int i=0;i<connection_num;i++){
   if(element_type[i] == 4){
     for(long int j=0;j<4;j++){
       tet_connection[tet_count*4+j] = (unsigned int)(connection[count + j]-min_connection);
     }
     count = count + 4;
     tet_count++;
   }
   if(element_type[i] == 6){
     count = count + 6;
     prism_count++;
   }
 }

 free(connection);

 int veclen;
 fread(&veclen, sizeof(int), 1, fp);

 int value_describing_type;
 fread(&value_describing_type, sizeof(int), 1, fp);

 printf("veclen %d  value type %d\n",veclen, value_describing_type);

 char *value_name;
 value_name = (char *)calloc(16*veclen, sizeof(char));

 char *value_unit;
 value_unit = (char *)calloc(16*veclen, sizeof(char));

 int *value_null_setting;
 value_null_setting = (int *)calloc(1*veclen, sizeof(int));

 int *value_veclen;
 value_veclen = (int *)calloc(1*veclen, sizeof(int));

 for(int i=0;i<veclen;i++){
   fread(&value_name[i*16], sizeof(char), 16, fp);
   fread(&value_unit[i*16], sizeof(char), 16, fp);
   fread(&value_veclen[i*1], sizeof(int), 1, fp);
   fread(&value_null_setting[i*1], sizeof(int), 1, fp);
   fseek(fp,1*sizeof(float),SEEK_CUR);
 }

 char tmp_name[16]={0};
 char tmp_unit[16]={0};
 for(int i=0;i<veclen;i++){
   memcpy(tmp_name, &value_name[i*16], 15);
   memcpy(tmp_unit, &value_unit[i*16], 15);
   printf("%s 単位は%s 次元は%d null設定は%d\n",tmp_name, tmp_unit, value_veclen[i], value_null_setting[i]);
   memset(tmp_name, '\0', 16);
   memset(tmp_unit, '\0', 16);
 }


 long int value_num = coord_num;
 float *ValueArray;
 ValueArray = (float *)calloc(value_num, sizeof( float ) );

 for(int i=0;i<5;i++){
   fseek(fp, sizeof(float)*value_num, SEEK_CUR);
 }

 float *velocity_u;
 float *velocity_v;
 float *velocity_w;
 velocity_u = (float *)calloc(value_num, sizeof(float));
 velocity_v = (float *)calloc(value_num, sizeof(float));
 velocity_w = (float *)calloc(value_num, sizeof(float));

 fread(velocity_u, sizeof(float), value_num, fp); 
 fread(velocity_v, sizeof(float), value_num, fp);
 fread(velocity_w, sizeof(float), value_num, fp);

 for(long int i=0;i<value_num;i++){
  ValueArray[i] = sqrt(velocity_u[i]*velocity_u[i]+velocity_v[i]*velocity_v[i]+velocity_w[i]*velocity_w[i]);
}

fclose(fp);


kvs::ValueArray<kvs::Real32> coords( coord, coord_num*3  );
kvs::ValueArray<kvs::UInt32> connections( tet_connection, tet_connection_num*4 );
kvs::ValueArray<kvs::Real32> values( ValueArray, value_num );



object->setVeclen( 1 );
object->setNumberOfNodes( coord_num );
object->setNumberOfCells( tet_connection_num );
object->setCellType( kvs::UnstructuredVolumeObject::Tetrahedra );
object->setCoords( coords );
object->setConnections( connections );
object->setValues( values );

  //writeStatusToFile(coord,tet_connection, ValueArray,coord_num, tet_connection_num, value_num,TETRA);
return(object);
}


kvs::UnstructuredVolumeObject* LoadTetraObject_binary_short(const char *filename ){

  kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
  FILE *fp;
  fp = fopen(filename, "rb");

  //char keyward[7];
  //fread(keyward, sizeof(char), 7, fp);
  //printf("%s\n",keyward);
  fseek(fp,7*sizeof(char),SEEK_CUR);

  //float version;
  //fread(&version, sizeof(float), 1, fp);
  //printf("%f\n",version);
  fseek(fp,1*sizeof(float),SEEK_CUR);

  char title[70];
  fread(title, sizeof(char), 70, fp);
  //printf("%s\n",title);

  int step_num;
  fread(&step_num, sizeof(int), 1, fp);
  float step_time;
  fread(&step_time, sizeof(float), 1, fp);
  printf("stemp is %d\ntime is %f\n",step_num,step_time);

  int coord_num;
  fread(&coord_num, sizeof(int), 1, fp);
  printf("coord is %d\n",coord_num);
  
  int coord_describing_type;
  fread(&coord_describing_type, sizeof(int), 1, fp);


  int *coord_num2;
  float *coord_x;
  float *coord_y;
  float *coord_z;
  coord_num2 = (int *)calloc(coord_num, sizeof(int));
  coord_x = (float *)calloc(coord_num, sizeof(float));
  coord_y = (float *)calloc(coord_num, sizeof(float));
  coord_z = (float *)calloc(coord_num, sizeof(float));

  fread(coord_num2, sizeof(int), coord_num, fp);
  int count = 0;
  for(int i=0;i<coord_num-1;i++){
    if(coord_num2[i+1] - coord_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("coordが番号順に並んでいないからcoord_num2を使うようにプログラムを作る  %d\n",count);
  }
  free(coord_num2);

  fread(coord_x, sizeof(float), coord_num, fp);
  fread(coord_y, sizeof(float), coord_num, fp);
  fread(coord_z, sizeof(float), coord_num, fp); 
  
  float *coord;
  coord = (float *)calloc(coord_num*3, sizeof(float));


  /////////////////coord[i*3+0]がcoord[i+0]になってたからエラーになってた
  for(int i=0;i<coord_num;i++){
    coord[i*3+0] = coord_x[i];
    coord[i*3+1] = coord_y[i];
    coord[i*3+2] = coord_z[i];
  }
  free(coord_x);
  free(coord_y);
  free(coord_z);

  int connection_num;
  fread(&connection_num, sizeof(int), 1, fp);
  printf("total connection = %d\n",connection_num);
  
  int *connection_num2;
  connection_num2 = (int *)calloc(connection_num, sizeof(int));
  fread(connection_num2, sizeof(int), connection_num, fp);

  count = 0;
  for(int i=0;i<connection_num-1;i++){
    if(connection_num2[i+1] - connection_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("connectionが番号順に並んでいないからconnection_num2を使うようにプログラムを作る\n");
  }
  free(connection_num2);
  
  //int *material_array;
  //material_array = (int *)calloc(connection_num, sizeof(int));
  //fread(material_array, sizeof(int), connection_num, fp);
  fseek(fp,connection_num*sizeof(int),SEEK_CUR);

  char *element_type;
  element_type = (char *)calloc(connection_num, sizeof(char));
  fread(element_type, sizeof(char), connection_num, fp);

  int element_histogram[14]={0};
  for(int i=0;i<connection_num;i++){
    element_histogram[(int)element_type[i]]++;
  }

  for(int i=0;i<14;i++){
    if(i==4){
      printf("type 4  is %d  (tetrahedra)\n",element_histogram[i]);
    }else if(i==6){
      printf("type 6  is %d  (prism)\n",element_histogram[i]);
    }else if(i==7){
      printf("type 7  is %d  (hexahedra)\n",element_histogram[i]);
    }else{
      printf("type%2d  is %d\n",i, element_histogram[i]);
    }
  }
  int tet_connection_num = element_histogram[4];

  int *connection;

  connection = (int *)calloc(element_histogram[4]*4 + element_histogram[6]*6,sizeof(int));

  fread(connection, sizeof(int), element_histogram[4]*4 + element_histogram[6]*6, fp);

  int min_connection=10;
  for(int i=0;i<element_histogram[4]*4+element_histogram[6]*6;i++){
    if(min_connection > connection[i]){
      min_connection = connection[i];
    }
  }

  unsigned int *tet_connection;
  tet_connection = (unsigned int *)calloc(element_histogram[4]*4,sizeof(unsigned int));

  int tet_count=0,prism_count=0;
  count = 0;
  for(int i=0;i<connection_num;i++){
   if(element_type[i] == 4){
     for(int j=0;j<4;j++){
       tet_connection[tet_count*4+j] = (unsigned int)(connection[count + j]-min_connection);
     }
     count = count + 4;
     tet_count++;
   }
   if(element_type[i] == 6){
     count = count + 6;
     prism_count++;
   }
 }

 free(connection);

 int veclen;
 fread(&veclen, sizeof(int), 1, fp);

 int value_describing_type;
 fread(&value_describing_type, sizeof(int), 1, fp);

 printf("veclen %d  value type %d\n",veclen, value_describing_type);

 char *value_name;
 value_name = (char *)calloc(16*veclen, sizeof(char));

 char *value_unit;
 value_unit = (char *)calloc(16*veclen, sizeof(char));

 int *value_null_setting;
 value_null_setting = (int *)calloc(1*veclen, sizeof(int));

 int *value_veclen;
 value_veclen = (int *)calloc(1*veclen, sizeof(int));

 for(int i=0;i<veclen;i++){
   fread(&value_name[i*16], sizeof(char), 16, fp);
   fread(&value_unit[i*16], sizeof(char), 16, fp);
   fread(&value_veclen[i*1], sizeof(int), 1, fp);
   fread(&value_null_setting[i*1], sizeof(int), 1, fp);
   fseek(fp,1*sizeof(float),SEEK_CUR);
 }

 char tmp_name[16]={0};
 char tmp_unit[16]={0};
 for(int i=0;i<veclen;i++){
   memcpy(tmp_name, &value_name[i*16], 15);
   memcpy(tmp_unit, &value_unit[i*16], 15);
   printf("%s 単位は%s 次元は%d null設定は%d\n",tmp_name, tmp_unit, value_veclen[i], value_null_setting[i]);
   //printf("%s\n",tmp_name);
   memset(tmp_name, '\0', 16);
   memset(tmp_unit, '\0', 16);
 }


 int value_num = coord_num;
 float *ValueArray;
 ValueArray = (float *)calloc(value_num, sizeof( float ) );

 for(int i=0;i<5;i++){
   fseek(fp, sizeof(float)*value_num, SEEK_CUR);
 }

 float *velocity_u;
 float *velocity_v;
 float *velocity_w;
 velocity_u = (float *)calloc(value_num, sizeof(float));
 velocity_v = (float *)calloc(value_num, sizeof(float));
 velocity_w = (float *)calloc(value_num, sizeof(float));

 fread(velocity_u, sizeof(float), value_num, fp); 
 fread(velocity_v, sizeof(float), value_num, fp);
 fread(velocity_w, sizeof(float), value_num, fp);


 for(int i=0;i<value_num;i++){
  ValueArray[i] = sqrt(velocity_u[i]*velocity_u[i]+velocity_v[i]*velocity_v[i]+velocity_w[i]*velocity_w[i]);
}


fclose(fp);

kvs::ValueArray<kvs::Real32> coords( coord, coord_num*3  );
kvs::ValueArray<kvs::UInt32> connections( tet_connection, tet_connection_num*4 );
kvs::ValueArray<kvs::Real32> values( ValueArray, value_num );



object->setVeclen( 1 );
object->setNumberOfNodes( coord_num );
object->setNumberOfCells( tet_connection_num );
object->setCellType( kvs::UnstructuredVolumeObject::Tetrahedra );
object->setCoords( coords );
object->setConnections( connections );
object->setValues( values );

  //writeStatusToFile(coord,tet_connection, ValueArray,coord_num, tet_connection_num, value_num,TETRA);
return(object);
}



kvs::UnstructuredVolumeObject* LoadPrismObject_binary_short(const char *filename ){
  kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
  FILE *fp;
  fp = fopen(filename, "rb");

  //char keyward[7];
  //fread(keyward, sizeof(char), 7, fp);
  //printf("%s\n",keyward);
  fseek(fp,7*sizeof(char),SEEK_CUR);

  //float version;
  //fread(&version, sizeof(float), 1, fp);
  //printf("%f\n",version);
  fseek(fp,1*sizeof(float),SEEK_CUR);

  char title[70];
  fread(title, sizeof(char), 70, fp);
  //printf("%s\n",title);

  int step_num;
  fread(&step_num, sizeof(int), 1, fp);
  float step_time;
  fread(&step_time, sizeof(float), 1, fp);
  printf("stemp is %d\ntime is %f\n",step_num,step_time);

  int coord_num;
  fread(&coord_num, sizeof(int), 1, fp);
  printf("coord is %d\n",coord_num);
  
  int coord_describing_type;
  fread(&coord_describing_type, sizeof(int), 1, fp);


  int *coord_num2;
  float *coord_x;
  float *coord_y;
  float *coord_z;
  coord_num2 = (int *)calloc(coord_num, sizeof(int));
  coord_x = (float *)calloc(coord_num, sizeof(float));
  coord_y = (float *)calloc(coord_num, sizeof(float));
  coord_z = (float *)calloc(coord_num, sizeof(float));

  fread(coord_num2, sizeof(int), coord_num, fp);
  int count = 0;
  for(int i=0;i<coord_num-1;i++){
    if(coord_num2[i+1] - coord_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("coordが番号順に並んでいないからcoord_num2を使うようにプログラムを作る\n");
  }
  free(coord_num2);

  fread(coord_x, sizeof(float), coord_num, fp);
  fread(coord_y, sizeof(float), coord_num, fp);
  fread(coord_z, sizeof(float), coord_num, fp); 
  
  float *coord;
  coord = (float *)calloc(coord_num*3, sizeof(float));


  /////////////////coord[i*3+0]がcoord[i+0]になってたからエラーになってた
  for(int i=0;i<coord_num;i++){
    coord[i*3+0] = coord_x[i];
    coord[i*3+1] = coord_y[i];
    coord[i*3+2] = coord_z[i];
  }
  free(coord_x);
  free(coord_y);
  free(coord_z);

  int connection_num;
  fread(&connection_num, sizeof(int), 1, fp);
  printf("total connection = %d\n",connection_num);
  
  int *connection_num2;
  connection_num2 = (int *)calloc(connection_num, sizeof(int));
  fread(connection_num2, sizeof(int), connection_num, fp);

  count = 0;
  for(int i=0;i<connection_num-1;i++){
    if(connection_num2[i+1] - connection_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("connectionが番号順に並んでいないからconnection_num2を使うようにプログラムを作る\n");
  }
  free(connection_num2);
  
  //int *material_array;
  //material_array = (int *)calloc(connection_num, sizeof(int));
  //fread(material_array, sizeof(int), connection_num, fp);
  fseek(fp,connection_num*sizeof(int),SEEK_CUR);

  char *element_type;
  element_type = (char *)calloc(connection_num, sizeof(char));
  fread(element_type, sizeof(char), connection_num, fp);

  int element_histogram[14]={0};
  for(int i=0;i<connection_num;i++){
    element_histogram[(int)element_type[i]]++;
  }

  for(int i=0;i<14;i++){
    if(i==4){
      printf("type 4  is %d  (tetrahedra)\n",element_histogram[i]);
    }else if(i==6){
      printf("type 6  is %d  (prism)\n",element_histogram[i]);
    }else if(i==7){
      printf("type 7  is %d  (hexahedra)\n",element_histogram[i]);
    }else{
      printf("type%2d  is %d\n",i, element_histogram[i]);
    }
  }
  int prism_connection_num = element_histogram[6];

  int *connection;

  connection = (int *)calloc(element_histogram[4]*4 + element_histogram[6]*6,sizeof(int));

  fread(connection, sizeof(int), element_histogram[4]*4 + element_histogram[6]*6, fp);

  int min_connection=10;
  for(int i=0;i<element_histogram[4]*4+element_histogram[6]*6;i++){
    if(min_connection > connection[i]){
      min_connection = connection[i];
    }
  }

  unsigned int *prism_connection;
  prism_connection = (unsigned int *)calloc(6*prism_connection_num,sizeof(unsigned int));

  int prism_count=0;
  count = 0;
  for(int i=0;i<connection_num;i++){
   if(element_type[i] == 6){
     for(int j=0;j<6;j++){
       prism_connection[prism_count*6+j] = (unsigned int)(connection[count + j]-min_connection);
     }
     count = count + 6;
     prism_count++;
   }
   if(element_type[i] == 4){
     count = count+4;
   }
 }

 free(connection);

 int veclen;
 fread(&veclen, sizeof(int), 1, fp);

 int value_describing_type;
 fread(&value_describing_type, sizeof(int), 1, fp);

 printf("veclen %d  value type %d\n",veclen, value_describing_type);

 char *value_name;
 value_name = (char *)calloc(16*veclen, sizeof(char));

 char *value_unit;
 value_unit = (char *)calloc(16*veclen, sizeof(char));

 int *value_null_setting;
 value_null_setting = (int *)calloc(1*veclen, sizeof(int));

 int *value_veclen;
 value_veclen = (int *)calloc(1*veclen, sizeof(int));

 for(int i=0;i<veclen;i++){
   fread(&value_name[i*16], sizeof(char), 16, fp);
   fread(&value_unit[i*16], sizeof(char), 16, fp);
   fread(&value_veclen[i*1], sizeof(int), 1, fp);
   fread(&value_null_setting[i*1], sizeof(int), 1, fp);
   fseek(fp,1*sizeof(float),SEEK_CUR);
 }

 char tmp_name[16]={0};
 char tmp_unit[16]={0};
 for(int i=0;i<veclen;i++){
   memcpy(tmp_name, &value_name[i*16], 15);
   memcpy(tmp_unit, &value_unit[i*16], 15);
   printf("%s 単位は%s 次元は%d null設定は%d\n",tmp_name, tmp_unit, value_veclen[i], value_null_setting[i]);
   memset(tmp_name, '\0', 16);
   memset(tmp_unit, '\0', 16);
 }


 int value_num = coord_num;
 float *ValueArray;
 ValueArray = (float *)calloc(value_num, sizeof( float ) );

 for(int i=0;i<5;i++){
   fseek(fp, sizeof(float)*value_num, SEEK_CUR);
 }

 float *velocity_u;
 float *velocity_v;
 float *velocity_w;
 velocity_u = (float *)calloc(value_num, sizeof(float));
 velocity_v = (float *)calloc(value_num, sizeof(float));
 velocity_w = (float *)calloc(value_num, sizeof(float));

 fread(velocity_u, sizeof(float), value_num, fp); 
 fread(velocity_v, sizeof(float), value_num, fp);
 fread(velocity_w, sizeof(float), value_num, fp);


 for(int i=0;i<value_num;i++){
  ValueArray[i] = sqrt(velocity_u[i]*velocity_u[i]+velocity_v[i]*velocity_v[i]+velocity_w[i]*velocity_w[i]);
}


fclose(fp);




kvs::ValueArray<kvs::Real32> coords( coord, coord_num*3  );
kvs::ValueArray<kvs::UInt32> connections( prism_connection, prism_connection_num*6 );
kvs::ValueArray<kvs::Real32> values( ValueArray, value_num );



object->setVeclen( 1 );
object->setNumberOfNodes( coord_num );
object->setNumberOfCells( prism_connection_num );
object->setCellType( kvs::UnstructuredVolumeObject::Prism );
object->setCoords( coords );
object->setConnections( connections );
object->setValues( values );

  //writeStatusToFile(coord,prism_connection, ValueArray,coord_num, prism_connection_num, value_num,PRISM);
return(object);
}



kvs::UnstructuredVolumeObject* LoadPrismObject_binary_long(const char *filename ){
  kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
  FILE *fp;
  fp = fopen(filename, "rb");

  //char keyward[7];
  //fread(keyward, sizeof(char), 7, fp);
  //printf("%s\n",keyward);
  fseek(fp,7*sizeof(char),SEEK_CUR);

  //float version;
  //fread(&version, sizeof(float), 1, fp);
  //printf("%f\n",version);
  fseek(fp,1*sizeof(float),SEEK_CUR);

  char title[70];
  fread(title, sizeof(char), 70, fp);
  //printf("%s\n",title);

  int step_num;
  fread(&step_num, sizeof(int), 1, fp);
  float step_time;
  fread(&step_time, sizeof(float), 1, fp);
  printf("stemp is %d\ntime is %f\n",step_num,step_time);

  long int coord_num;
  fread(&coord_num, sizeof(long int), 1, fp);
  printf("coord is %ld\n",coord_num);
  
  int coord_describing_type;
  fread(&coord_describing_type, sizeof(int), 1, fp);


  long int *coord_num2;
  float *coord_x;
  float *coord_y;
  float *coord_z;
  coord_num2 = (long int *)calloc(coord_num, sizeof(long int));
  coord_x = (float *)calloc(coord_num, sizeof(float));
  coord_y = (float *)calloc(coord_num, sizeof(float));
  coord_z = (float *)calloc(coord_num, sizeof(float));

  fread(coord_num2, sizeof(long int), coord_num, fp);
  long int count = 0;
  for(long int i=0;i<coord_num-1;i++){
    if(coord_num2[i+1] - coord_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("coordが番号順に並んでいないからcoord_num2を使うようにプログラムを作る\n");
  }
  free(coord_num2);

  fread(coord_x, sizeof(float), coord_num, fp);
  fread(coord_y, sizeof(float), coord_num, fp);
  fread(coord_z, sizeof(float), coord_num, fp); 
  
  float *coord;
  coord = (float *)calloc(coord_num*3, sizeof(float));


  /////////////////coord[i*3+0]がcoord[i+0]になってたからエラーになってた
  for(long int i=0;i<coord_num;i++){
    coord[i*3+0] = coord_x[i];
    coord[i*3+1] = coord_y[i];
    coord[i*3+2] = coord_z[i];
  }
  free(coord_x);
  free(coord_y);
  free(coord_z);

  long int connection_num;
  fread(&connection_num, sizeof(long int), 1, fp);
  printf("total connection = %ld\n",connection_num);
  
  long int *connection_num2;
  connection_num2 = (long int *)calloc(connection_num, sizeof(long int));
  fread(connection_num2, sizeof(long int), connection_num, fp);

  count = 0;
  for(long int i=0;i<connection_num-1;i++){
    if(connection_num2[i+1] - connection_num2[i] != 1){
      count++;
    }
  }
  if(count != 0){
    printf("connectionが番号順に並んでいないからconnection_num2を使うようにプログラムを作る(要素配列番号)\n");
  }
  free(connection_num2);
  
  //int *material_array;
  //material_array = (int *)calloc(connection_num, sizeof(int));
  //fread(material_array, sizeof(int), connection_num, fp);
  fseek(fp,connection_num*sizeof(int),SEEK_CUR);
  //マテリアル番号配列はスキップ

  char *element_type;
  element_type = (char *)calloc(connection_num, sizeof(char));
  fread(element_type, sizeof(char), connection_num, fp);

  long int element_histogram[14]={0};
  for(long int i=0;i<connection_num;i++){
    element_histogram[(int)element_type[i]]++;
  }

  for(int i=0;i<14;i++){
    if(i==4){
      printf("type 4  is %ld  (tetrahedra)\n",element_histogram[i]);
    }else if(i==6){
      printf("type 6  is %ld  (prism)\n",element_histogram[i]);
    }else if(i==7){
      printf("type 7  is %ld  (hexahedra)\n",element_histogram[i]);
    }else{
      printf("type%2d  is %ld\n",i, element_histogram[i]);
    }
  }
  long int prism_connection_num = element_histogram[6];

  long int *connection;

  connection = (long int *)calloc(element_histogram[4]*4 + element_histogram[6]*6,sizeof(long int));

  fread(connection, sizeof(long int), element_histogram[4]*4 + element_histogram[6]*6, fp);


  long int min_connection=10;
  for(long i=0;i<element_histogram[4]*4+element_histogram[6]*6;i++){
    if(min_connection > connection[i]){
      min_connection = connection[i];
    }
  }


  unsigned int *prism_connection;
  prism_connection = (unsigned int *)calloc(6*prism_connection_num,sizeof(unsigned int));

  long int prism_count=0;
  count = 0;
  for(long int i=0;i<connection_num;i++){
   if(element_type[i] == 6){
     for(long int j=0;j<6;j++){
       prism_connection[prism_count*6+j] = (unsigned int)(connection[count + j]-min_connection);
     }
     count = count + 6;
     prism_count++;
   }
   if(element_type[i] == 4){
     count = count+4;
   }
 }

 free(connection);

 float ball_min_coord[3]={100,100,100}, ball_max_coord[3]={-100,-100,-100};

 for(long int i=0;i<prism_connection_num;i++){
   for(long int j=0;j<6;j++){
     if(coord[prism_connection[i*6+j]*3+0]<1000 && coord[prism_connection[i*6+j]*3+0]>(-100)){
       if(ball_min_coord[0]>coord[prism_connection[i*6+j]*3+0]){
        ball_min_coord[0]=coord[prism_connection[i*6+j]*3+0];
      }
      if(ball_min_coord[1]>coord[prism_connection[i*6+j]*3+1]){
        ball_min_coord[1]=coord[prism_connection[i*6+j]*3+1];
      }
      if(ball_min_coord[2]>coord[prism_connection[i*6+j]*3+2]){
        ball_min_coord[2]=coord[prism_connection[i*6+j]*3+2];
      }
      if(ball_max_coord[0]<coord[prism_connection[i*6+j]*3+0]){
        ball_max_coord[0]=coord[prism_connection[i*6+j]*3+0];
      }
      if(ball_max_coord[1]<coord[prism_connection[i*6+j]*3+1]){
        ball_max_coord[1]=coord[prism_connection[i*6+j]*3+1];
      }
      if(ball_max_coord[2]<coord[prism_connection[i*6+j]*3+2]){
        ball_max_coord[2]=coord[prism_connection[i*6+j]*3+2];
      }
    }
  }
}

printf("ball min %f %f %f\nball max %f %f %f\n",ball_min_coord[0], ball_min_coord[1], ball_min_coord[2], ball_max_coord[0], ball_max_coord[1], ball_max_coord[2]);

int veclen;
fread(&veclen, sizeof(int), 1, fp);

int value_describing_type;
fread(&value_describing_type, sizeof(int), 1, fp);

printf("veclen %d  value type %d\n",veclen, value_describing_type);

char *value_name;
value_name = (char *)calloc(16*veclen, sizeof(char));

char *value_unit;
value_unit = (char *)calloc(16*veclen, sizeof(char));

int *value_null_setting;
value_null_setting = (int *)calloc(1*veclen, sizeof(int));

int *value_veclen;
value_veclen = (int *)calloc(1*veclen, sizeof(int));

for(int i=0;i<veclen;i++){
 fread(&value_name[i*16], sizeof(char), 16, fp);
 fread(&value_unit[i*16], sizeof(char), 16, fp);
 fread(&value_veclen[i*1], sizeof(int), 1, fp);
 fread(&value_null_setting[i*1], sizeof(int), 1, fp);
 fseek(fp,1*sizeof(float),SEEK_CUR);
}

char tmp_name[16]={0};
char tmp_unit[16]={0};
for(int i=0;i<veclen;i++){
 memcpy(tmp_name, &value_name[i*16], 15);
 memcpy(tmp_unit, &value_unit[i*16], 15);
 printf("%s 単位は%s 次元は%d null設定は%d\n",tmp_name, tmp_unit, value_veclen[i], value_null_setting[i]);
 memset(tmp_name, '\0', 16);
 memset(tmp_unit, '\0', 16);
}


long int value_num = coord_num;
float *ValueArray;
ValueArray = (float *)calloc(value_num, sizeof( float ) );

for(int i=0;i<5;i++){
 fseek(fp, sizeof(float)*value_num, SEEK_CUR);
}

float *velocity_u;
float *velocity_v;
float *velocity_w;
velocity_u = (float *)calloc(value_num, sizeof(float));
velocity_v = (float *)calloc(value_num, sizeof(float));
velocity_w = (float *)calloc(value_num, sizeof(float));

fread(velocity_u, sizeof(float), value_num, fp); 
fread(velocity_v, sizeof(float), value_num, fp);
fread(velocity_w, sizeof(float), value_num, fp);


for(long int i=0;i<value_num;i++){
  ValueArray[i] = sqrt(velocity_u[i]*velocity_u[i]+velocity_v[i]*velocity_v[i]+velocity_w[i]*velocity_w[i]);
}


fclose(fp);



  //printf("ball min %f %f %f\nball max %f %f %f\n",ball_min_coord[0], ball_min_coord[1], ball_min_coord[2], ball_max_coord[0], ball_max_coord[1], ball_max_coord[2]);

kvs::ValueArray<kvs::Real32> coords( coord, coord_num*3  );
kvs::ValueArray<kvs::UInt32> connections( prism_connection, prism_connection_num*6 );
kvs::ValueArray<kvs::Real32> values( ValueArray, value_num );


object->setVeclen( 1 );
object->setNumberOfNodes( coord_num );
object->setNumberOfCells( prism_connection_num );
object->setCellType( kvs::UnstructuredVolumeObject::Prism );
object->setCoords( coords );
object->setConnections( connections );
object->setValues( values );
object->setMinMaxObjectCoords(kvs::Vec3(ball_min_coord[0], ball_min_coord[1], ball_min_coord[2]),kvs::Vec3(ball_max_coord[0], ball_max_coord[1], ball_max_coord[2]) );
  //writeStatusToFile(coord,prism_connection, ValueArray,coord_num, prism_connection_num, value_num,PRISM);
return(object);
}


kvs::UnstructuredVolumeObject* CreateUnstructuredVolumeObject( const char* filename,char cell_type )
{
  kvs::UnstructuredVolumeObject* object = NULL;
  
  if(cell_type == TETRA ){
    if(CheckFileLong(filename)==0){
      printf("tetra long object\n");
      object = LoadTetraObject_binary_long(filename);
    }else{
      printf("tetra short object\n");
      object = LoadTetraObject_binary_short(filename);
    }
  }
  
  if(cell_type == PRISM ){
    if(CheckFileLong(filename)==0){
      printf("prism long object\n");
      object = LoadPrismObject_binary_long(filename);
    }else{
      printf("prism short object\n");
      object = LoadPrismObject_binary_short(filename);
    }
    //return(LoadPrismObject_ascii(filename) );
  }

  object->updateMinMaxCoords();
  object->updateMinMaxValues();

  return(object);
}

kvs::UnstructuredVolumeObject* CreateCutPrismObject( const char* filename )
{
  kvs::UnstructuredVolumeObject* volume = NULL;

  if(CheckFileLong(filename)==0){
    printf("prism long object\n");
    volume = LoadPrismObject_binary_long( filename );
  }
  else
  {
    printf("prism short object\n");
    volume = LoadPrismObject_binary_short(filename);
  }

  std::vector<unsigned int> tmp_connection;
  unsigned int count=0;

  float min_value = -300.0;
  float max_value = 1000.0;
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





void writeStatusToFile(float *coord,unsigned int *connection, float *value, int coord_num, int connection_num, int value_num, char cell_type){

#ifdef AVSUCD_OUTPUT
  if(cell_type == TETRA){
    for(int i=0;i<4*connection_num;i++){
      connection[i]++;
    }
  }else{
    for(int i=0;i<6*connection_num;i++){
      connection[i]++;
    }
  }
  
  FILE *fp_write;
  fp_write = fopen("./output.inp","w");


  if(cell_type == TETRA){
    fprintf(fp_write,"%d %d 1 0 0\n",coord_num, connection_num);

    for(int i=0;i<coord_num;i++){
      fprintf(fp_write,"%d %f %f %f\n",i+1, coord[i*3+0], coord[i*3+1], coord[i*3+2] );
    }
    for(int i=0;i<connection_num;i++){
      fprintf(fp_write,"%d 1 tet %u %u %u %u\n",i+1, connection[i*4+0], connection[i*4+1], connection[i*4+2], connection[i*4+3]);
    }
    fprintf(fp_write,"1 1\ndensity\n");

    for(int i=0;i<coord_num;i++){
      fprintf(fp_write,"%d %f\n",i+1, value[i]);
    }
    fclose(fp_write);
  }


  if(cell_type == PRISM){
    fprintf(fp_write,"%d %d 1 0 0\n",coord_num, connection_num);

    for(int i=0;i<coord_num;i++){
      fprintf(fp_write,"%d %f %f %f\n",i+1, coord[i*3+0], coord[i*3+1], coord[i*3+2] );
    }
    for(int i=0;i<connection_num;i++){
      fprintf(fp_write,"%d 1 prism %u %u %u %u %u %u\n",i+1, connection[i*4+0], connection[i*4+1], connection[i*4+2], connection[i*4+3], connection[i*4+4], connection[i*4+5]);
    }
    fprintf(fp_write,"1 1\ndensity\n");

    for(int i=0;i<coord_num;i++){
      fprintf(fp_write,"%d %f\n",i+1, value[i]);
    }
    fclose(fp_write);
  }

#endif
  ////////////////////////ここまでできた
  return;


}

kvs::UnstructuredVolumeObject* LoadPrismObject_ascii( const char* filename )
{
  kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
  int cell_type = PRISM;
  if(cell_type != TETRA && cell_type != PRISM){
    printf("TETRA or PRISM\n");
    return(object);
  }

  FILE *fp;

  fp = fopen(filename, "r");


  char line[256];

  while(fgets(line, 256, fp)!=NULL){
    if(strstr(line, "step1") !=NULL){
      break;
    }
  }

  fgets(line,256,fp);

  int coord_num;
  int connection_num;

  sscanf(line,"%d %d\n",&coord_num, &connection_num);
  printf("coord is %d\n total connection is %d\n",coord_num,connection_num);
  
  float *coord;
  coord = (float *)calloc(coord_num*3, sizeof(float));

  for(int i=0;i<coord_num;i++){
    fgets(line, 256, fp);
    sscanf(line, "%*d %f %f %f",&coord[i*3+0],&coord[i*3+1],&coord[i*3+2]);
  }

  long int pos;

  pos = ftell(fp);

  unsigned int tet_connection_num=0, prism_connection_num=0;

  char cell_type2[16];

  while(fgets(line,256,fp)!=NULL){
    if(sscanf(line, "%*d %*d %s", cell_type2) != 1){
      break;
    }
    if(strstr(cell_type2,"tet")!=NULL){
      tet_connection_num++;
    }else if(strstr(cell_type2,"prism")!=NULL){
      prism_connection_num++;
    }else{
      break;
    }
  }
  printf("tetra cell is %d \nprism cell is %d \ntotal cell is%d\n",tet_connection_num, prism_connection_num,tet_connection_num+ prism_connection_num);


  unsigned int *connection;
  if(cell_type == TETRA){
    connection = (unsigned int *)calloc(4 * tet_connection_num, sizeof(unsigned int));
  }else if(cell_type == PRISM){
    connection = (unsigned int *)calloc(6 * prism_connection_num, sizeof(unsigned int) );
  }

  bool *connection_cell_type;
  connection_cell_type = (bool *)calloc(connection_num, sizeof(bool) );



  rewind(fp);
  fseek(fp, pos, SEEK_SET);


  unsigned int count = 0;
  unsigned int count_cell_type = 0;

  while(fgets(line,256,fp)!=NULL){
    if(sscanf(line, "%*d %*d %s", cell_type2) != 1){
      break;
    }
    if(cell_type == TETRA){
      if(strstr(cell_type2, "tet") != NULL){
       connection_cell_type[count_cell_type] = 1;
       sscanf(line, "%*d %*d %*s %u %u %u %u",&connection[count],&connection[count+1],&connection[count+2],&connection[count+3]);
       count = count + 4;
       if(count == 4*tet_connection_num){
         break;
       }
     }
   }
   if(cell_type == PRISM){
    if(strstr(cell_type2, "prism") != NULL){
     sscanf(line, "%*d %*d %*s %u %u %u %u %u %u",&connection[count],&connection[count+1],&connection[count+2],&connection[count+3], &connection[count+4], &connection[count+5]);
     count = count + 6;
     if(count == 6*prism_connection_num){
       break;
     }
   }
 }
 count_cell_type++;
}

  /////connection と coordは入った
  //  connection_cell_type[i] = 1ならi-th cell is tetra,  0ならcell is prism


unsigned int temp_connection_num=0;




  ////////////////ここをtemp = TETRA,temp=PRISMにしてた 
if(cell_type == TETRA){
  temp_connection_num = tet_connection_num;
}else{
  temp_connection_num = prism_connection_num;
}


float *value;

value = (float *)calloc(coord_num, sizeof(float) );



char temp_line[128];
while(fgets(line,256,fp)!=NULL){
  if(sscanf(line, "%s %*s", temp_line) == 1){
    if(strstr(temp_line,"HPC_region") != NULL){
     break;
   }
 }
}

count = 0;
for(unsigned int i=0;i<(unsigned int)coord_num;i++){
  fgets(line,256,fp);
  sscanf(line, "%*d %f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f", &value[i]);
}



kvs::ValueArray<kvs::Real32> kvs_coords( coord, coord_num*3  );
kvs::ValueArray<kvs::UInt32> kvs_connections( connection, cell_type*temp_connection_num );
kvs::ValueArray<kvs::Real32> kvs_values( value, coord_num );


object->setVeclen( 1 );
object->setNumberOfNodes( coord_num );
if(cell_type == TETRA){
  object->setNumberOfCells( tet_connection_num );
  object->setCellType( kvs::UnstructuredVolumeObject::Tetrahedra );
}
if(cell_type == PRISM){
  object->setNumberOfCells( prism_connection_num  );
  object->setCellType( kvs::UnstructuredVolumeObject::Prism );
}
object->setCoords( kvs_coords );
object->setConnections( kvs_connections );
object->setValues( kvs_values );


  // printf("%f \n%u \n",object->coords()[20], object->connections()[0]);

writeStatusToFile(coord, connection, value, coord_num, temp_connection_num, coord_num, cell_type);

if(cell_type == TETRA){
  printf("tetra load finish\n");
}else{
  printf("prism load finish\n");
}

return( object );
}


