//
//  KVSMLMaker.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 14:30:01.
//
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <kvs/CommandLine>
using namespace std;

int main( int argc, char** argv )
{
    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "p", "Parameter", 1, true );
    param.addOption( "min", "Min value of the data", 1, true );
    param.addOption( "max", "Max value of the data", 1, true );
    if ( !param.parse() ) return 1;
    
    float min = param.optionValue<float>( "min" );
    float max = param.optionValue<float>( "max" );

    char* buffer1=new char[256];
    char* buffer2=new char[256];
    string filename = param.optionValue<string>( "p" );
    std::vector<char> v;for( int i = 1; i <= 48; i++ )
    {
        sprintf( buffer1, "%02d.kvsml", i );
        string str_kvsml = filename + "f" + buffer1;
        sprintf( buffer2, "%02d.bin", i );
        string str_dat = filename + "f" + buffer2;

        ofstream outs(str_kvsml.c_str(),ofstream::out);
        outs << "<KVSML>" << endl;
        outs << "  <Object type=\"StructuredVolumeObject\">" << endl;
        outs << "    <StructuredVolumeObject resolution=\"500 500 100\" grid_type=\"uniform\">" << endl;
        outs << "      <Node>" << endl;
        outs << "        <Value veclen=\"1\" min_value=\""<< min << "\" max_value=\"" << max << "\" >" << endl;
        outs << "          <DataArray type=\"float\" file=\"" << str_dat << "\" format=\"binary\" endian=\"big\"/>" << endl;
        outs << "        </Value>" << endl;
        outs << "      </Node>" << endl;
        outs << "    </StructuredVolumeObject>" << endl;
        outs << "  </Object>" << endl;
        outs << "</KVSML>" << endl;
    }
}
