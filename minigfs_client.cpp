
// ecs251 miniGFS

#include <iostream>
#include "Shadow_Directory.h"
#include "Shadow_Replica.h"

using namespace std;

int
main()
{
  Shadow_Directory gfs_master 
  { "http://127.0.0.1:8384", "1234567890", "Directory", "00000000" };

  std::string fhandle = "00000002";

  Json::Value result, result_P, result_A, result_B;

  //Step1-2
  result = gfs_master.ObtainChunkURL("my_ecs251_file", fhandle, "0");
  while((result["status"]).asString() != "URLpassed"){
    result = gfs_master.ObtainChunkURL("my_ecs251_file", fhandle, "0");
  }

  std::string url_primary = (result["primary"]).asString();
  Shadow_Replica gfs_primary
  { url_primary, "1234567890", "Replica", "00000001" };

  std::string url_secondary_A = (result["secondary_A"]).asString();
  Shadow_Replica gfs_secondary_A
  { url_secondary_A, "1234567890", "Replica", "00000002" };

  std::string url_secondary_B = (result["secondary_B"]).asString();
  Shadow_Replica gfs_secondary_B
  { url_secondary_B, "1234567890", "Replica", "00000003" };

  std::string my_chunk_data = { "ecs251 data" };

  while(true){
    //Step3
    result_P = gfs_primary.PushChunk2Replica("my_ecs251_file", fhandle, "0", my_chunk_data);
    result_A = gfs_secondary_A.PushChunk2Replica("my_ecs251_file", fhandle, "0", my_chunk_data);
    result_B = gfs_secondary_B.PushChunk2Replica("my_ecs251_file", fhandle, "0", my_chunk_data);

    //Step4
    if (((result_P["vote"]).asString() == "commit") &&
      ((result_A["vote"]).asString() == "commit") &&
      ((result_B["vote"]).asString() == "commit"))
    {
      //Step7
      //仅primary回复数据写入执行的结果到client
      result_P = gfs_primary.CommitAbort("my_ecs251_file", fhandle, "0", "commit");
      
      if(((result_P["status"]).asString() == "committed")) break;
    }
  }

  return 0;
}
