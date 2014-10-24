#ifndef LOAD_UCD_H_INCLUDE
#define LOAD_UCD_H_INCLUDE

#include <kvs/ImporterBase>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/Vector3>
#include <vector>
#include <string>

namespace takami
{

/*==========================================================================*/
/**
 *  Unstructured volume object importer class.
 */
/*==========================================================================*/
class LoadUcd : public kvs::ImporterBase, public kvs::UnstructuredVolumeObject
{
public:

    LoadUcd();
    LoadUcd( const char* filename, char cell_type );
    LoadUcd( const char* filename, char cell_type, int NthValue );
    virtual ~LoadUcd();

private:

    int CheckFileLong(const char *filename);

    void LoadTetraObject_binary_long(const char *filename);
    void LoadPrismObject_binary_long(const char *filename);
    void LoadTetraCoordAndConnection( const char* filename, int NthValue);
    void LoadPrismCoordAndConnection( const char* filename, int NthValue);
    kvs::UnstructuredVolumeObject* exec( const kvs::FileFormatBase* file_format ){return(NULL);};
    //exec is dummy for importerbase

};


  std::vector<float> LoadUcdValue( const char* filename, int NthValue );
  std::string LoadUcdName( const char* filename, int NthValue );

} // end of namespace takami



#endif
