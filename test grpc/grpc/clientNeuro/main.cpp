#include "UserService.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <ctime>
#include "ThreadWorker.h"



void PrintTitle(const std::string& name)
{
    std::time_t t = std::time(0);

    std::cout << std::endl << name.c_str() << " - " ;
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
        const MetadataDto& meta = reply.metadata(0);
        std::cout << meta.key() << meta.value();
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

int main(int argc, char** argv)
{
    RunClient();
    //RunClientinThread();

    std::cin.get();

    return 0;
}