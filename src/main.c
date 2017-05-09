#include <stdio.h>	
  
//#ifndef   UINT64_C	
//#define   UINT64_C(value)__CONCAT(value,ULL)  
//#endif	
  
//extern "C"	
//{
#include <libavutil/avutil.h>  
#include <libavformat/avformat.h>  
#include <libavcodec/avcodec.h>
//}  
//#include <iostream>   
//using namespace std;  

int main(int argc,char **argv)	
{  
  
//	av_register_all()
	printf("hell ffmpeg");
	av_register_all();
	return 0;  
}  

