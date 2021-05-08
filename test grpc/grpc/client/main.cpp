#include "demo.grpc.pb.h"
#include "UserService.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <ctime>
#include "ThreadWorker.h"

void PrintTitle(const std::string& name)
{
  std::time_t t = std::time(0);

  std::cout << std::endl << name.c_str() << " - " << std::ctime(&t);
}
class UserServiceClient
{
public:
  UserServiceClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(UserServiceOperations::NewStub(channel)) {}

  void ProcessFile() {
      PrintTitle("ProcessFile");

      ProcessResult reply;
      DicomFileDto dicomFile;
      grpc::ClientContext context;

      grpc::Status status = stub_->ProcessFile(&context, dicomFile, &reply);
      //std::cout<<reply.metadata
      const MetadataDto& meta =reply.metadata(0);
      std::cout<<meta.key()<<meta.value();
  }

  void StartNotificationStream() {
      PrintTitle("StartNotificationStream");

      google::protobuf::Empty request;
      grpc::ClientContext context;
      NotificationMessageDto messageDto;

      //context.set_wait_for_ready(true);

      std::unique_ptr<grpc::ClientReader<NotificationMessageDto> > reader(stub_->StartNotificationStream(&context, request));
      int index = 0;

      while (reader->Read(&messageDto))
      {
            std::cout << messageDto.message();
          
          //std::cout << messageDto.message;
      }

      grpc::Status status = reader->Finish();
  }

private:
  std::unique_ptr<UserServiceOperations::Stub> stub_;
};

class MedicalGeneratorClient
{
public:
  MedicalGeneratorClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(MedicalGeneratorGrpc::NewStub(channel)) {}

  void PrintGeneratorStateDto(GeneratorStateDto& generateDto)
  {
    printf("%d %d %d %d %d %d %d\n"
      , generateDto.devicestate().connected()
      , generateDto.hasfaults()
      , generateDto.xrayenabled()
      , generateDto.isreadyforexposure()
      , generateDto.isinterlockopen()
      , generateDto.needwarmup()
      , generateDto.isovertemperature());
  }

  void PrintDeviceErrorDto(DeviceErrorDto& errorDto)
  {
    printf("%s %s %s\n", errorDto.adanicode().c_str(), errorDto.internalcode().c_str(), errorDto.description().c_str());
  }

  void PrintDeviceErrorsCollectionDto(DeviceErrorsCollectionDto& errorsDto)
  {
    for (int i = 0; i < errorsDto.deviceerrors_size(); i++)
    {
      DeviceErrorDto  errorDto = errorsDto.deviceerrors(i);
      printf("%s %s %s\n", errorDto.adanicode().c_str(), errorDto.internalcode().c_str(), errorDto.description().c_str());
    }
  }

  bool CheckState(grpc::Status& status)
  {
    status.ok() ? std::cout << "Status: Ok" << std::endl : std::cout << "Status: " << status.error_code() << " - " << status.error_message() << std::endl;
    return status.ok();
  }

  void GetState()
  {
    PrintTitle("GetState");

    google::protobuf::Empty request;
    GeneratorStateDto stateDto;
    grpc::ClientContext context;

    grpc::Status status = stub_->GetState(&context, request, &stateDto);

    if (CheckState(status))
      PrintGeneratorStateDto(stateDto);
  }

  void SubscribeToStateUpdate()
  {
    PrintTitle("SubscribeToStateUpdate");

    google::protobuf::Empty request;
    grpc::ClientContext context;
    GeneratorStateDto generateDto;

    //context.set_wait_for_ready(true);

    std::unique_ptr<grpc::ClientReader<GeneratorStateDto> > reader(stub_->SubscribeToStateUpdate(&context, request));
    int index = 0;

    while (reader->Read(&generateDto))
    {
      std::cout << "Read " << ++index << std::endl;
      PrintGeneratorStateDto(generateDto);

      if (index > 7)
        context.TryCancel();
    }

    grpc::Status status = reader->Finish();

    CheckState(status);
  }

  void GetDeviceErrors()
  {
    PrintTitle("GetDeviceErrors");

    google::protobuf::Empty request;
    grpc::ClientContext context;
    DeviceErrorsCollectionDto errorsDto;

    grpc::Status status = stub_->GetDeviceErrors(&context, request, &errorsDto);

    if (CheckState(status))
      PrintDeviceErrorsCollectionDto(errorsDto);
  }

  void SubscribeToDeviceErrors()
  {
    PrintTitle("SubscribeToDeviceErrors");

    google::protobuf::Empty request;
    grpc::ClientContext context;
    DeviceErrorDto errorDto;

    context.set_wait_for_ready(true);

    std::unique_ptr<grpc::ClientReader<DeviceErrorDto> > reader(stub_->SubscribeToDeviceErrors(&context, request));
    int index = 0;

    while (reader->Read(&errorDto))
    {
      std::cout << "Read " << ++index << std::endl;
      PrintDeviceErrorDto(errorDto);

      if (index > 7)
        context.TryCancel();
    }

    grpc::Status status = reader->Finish();

    CheckState(status);
  }

  void ClearErrors()
  {
    PrintTitle("ClearErrors");

    google::protobuf::Empty request;
    ResultMessageDto resultDto;
    grpc::ClientContext context;

    grpc::Status status = stub_->ClearErrors(&context, request, &resultDto);

    if (CheckState(status))
      std::cout << resultDto.result() << std::endl;
  }

  void SetExposureParameters()
  {
    PrintTitle("SetExposureParameters");

    ExposureParametersDto reply;
    ExposureParametersDto exposureDto;
    grpc::ClientContext context;
    google::protobuf::Duration dt;
    dt.set_seconds(10);

    exposureDto.set_kv(10000);
    exposureDto.set_ma(1.5);
    exposureDto.set_allocated_exposuretime(new google::protobuf::Duration(dt));

    printf("%d %f %d\n", exposureDto.kv(), exposureDto.ma(), exposureDto.exposuretime());

    grpc::Status status = stub_->SetExposureParameters(&context, exposureDto, &reply);

    if (CheckState(status))
      printf("%d %f %d\n", reply.kv(), reply.ma(), reply.exposuretime());
  }

  void GetExposureParameters()
  {
    PrintTitle("GetExposureParameters");

    google::protobuf::Empty request;
    ExposureParametersDto exposureDto;
    grpc::ClientContext context;

    grpc::Status status = stub_->GetExposureParameters(&context, request, &exposureDto);
    if (CheckState(status))
      printf("%d %f %d\n", exposureDto.kv(), exposureDto.ma(), exposureDto.exposuretime());

    CheckState(status);
  }

  template <typename T>
  void SubscribeToStateUpdate2(const bool& running)
  {
    PrintTitle("SubscribeToStateUpdate - begin");

    google::protobuf::Empty request;
    grpc::ClientContext context;
    T generateDto;

    std::unique_ptr<grpc::ClientReader<T>> reader(stub_->SubscribeToStateUpdate(&context, request));

    int counter = 0;
    int index = 0;

    while (running && (index < 5))
    {
      if (((++counter % 20) == 0) && reader->Read(&generateDto))
      {
        PrintGeneratorStateDto(generateDto);
        counter = 0;

        index++;
      }
      else
        Sleep(10);
    }

    PrintTitle("SubscribeToStateUpdate - end");
  }

private:
  std::unique_ptr<MedicalGeneratorGrpc::Stub> stub_;
};

void RunClient()
{
  PrintTitle("RunClient");

  //MedicalGeneratorClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  UserServiceClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  client.ProcessFile();
  client.StartNotificationStream();
  //client.GetState();

  //client.GetState();
  //client.GetDeviceErrors();
  //client.ClearErrors();
  //client.SetExposureParameters();
  //client.GetExposureParameters();
}

void RunClientinThread()
{
  MedicalGeneratorClient client(grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials()));
  //client.SubscribeToStateUpdate2<GeneratorStateDto>(true);

  ThreadWorker tw;
  using namespace std::placeholders; // for `_1`
  tw.SetWorkingProcedure(std::bind(&MedicalGeneratorClient::SubscribeToStateUpdate2<GeneratorStateDto>, &client, _1));

  tw.StartWork();
  Sleep(6000);
  tw.StopWork();
}

int main(int argc, char** argv)
{
  RunClient();
  //RunClientinThread();

  std::cin.get();

  return 0;
}