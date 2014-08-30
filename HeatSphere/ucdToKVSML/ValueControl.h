#ifndef VALUE_CONTROL__H
#define VALUE_CONTROL__H

#include <kvs/UnstructuredVolumeObject>
#include <kvs/LineObject>

void ValueControl(float *ValueArray, long int value_num){
float minv=58.0, maxv=65.0;
  for(long int i=0;i<value_num;i++){
    if(ValueArray[i]<minv){
      ValueArray[i] = minv;
    }
    if(ValueArray[i]>maxv){
      ValueArray[i]=maxv;
    }
  }
printf("最大値%f 最小値%fに調整した\n",maxv,minv);
  return;
}

#endif
