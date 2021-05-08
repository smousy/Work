#include "demo.grpc.pb.h"
#include "UserService.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <windows.h>
#include <ctime>

static HANDLE hTimer = 0;

void PrintTitle(const std::string& name)
{
	std::time_t t = std::time(0);

	std::cout << std::endl << name.c_str() << " - " << std::ctime(&t);
}

class MedicalGeneratorService final : public MedicalGeneratorGrpc::Service
{
	ExposureParametersDto exposureDto;

	void SetGeneratorStateDto(GeneratorStateDto& generatorDto)
	{
		static bool state = true;

		DeviceStateDto* deviceDto = new DeviceStateDto();
		deviceDto->set_connected(state);
		generatorDto.set_allocated_devicestate(deviceDto);
		generatorDto.set_hasfaults(state);
		generatorDto.set_xrayenabled(state);
		generatorDto.set_isreadyforexposure(state);
		generatorDto.set_isinterlockopen(state);
		generatorDto.set_needwarmup(state);
		generatorDto.set_isovertemperature(state);

		state = !state;
	}

	void SetDeviceErrorDto(DeviceErrorDto& errorDto)
	{
		static int value = 0;
		errorDto.set_adanicode(std::to_string(value));
		errorDto.set_internalcode(std::to_string(value));
		errorDto.set_description("Error: " + std::to_string(value));

		value++;
	}

	virtual grpc::Status GetState(grpc::ServerContext* context, const google::protobuf::Empty* request, GeneratorStateDto* response) override
	{
		PrintTitle("GetState");

		SetGeneratorStateDto(*response);

		return grpc::Status::OK;
	}

	virtual grpc::Status SubscribeToStateUpdate(grpc::ServerContext* context, const google::protobuf::Empty* request, grpc::ServerWriter<GeneratorStateDto>* writer) override
	{
		PrintTitle("SubscribeToStateUpdate - begin");

		int index = 0;
		GeneratorStateDto generatorDto;

		while (!context->IsCancelled())
		{
			WaitForSingleObject(hTimer, 10000);

			std::cout << "Write " << ++index << std::endl;

			SetGeneratorStateDto(generatorDto);
			writer->Write(generatorDto);
		}

		PrintTitle("SubscribeToStateUpdate - end");

		return grpc::Status::OK;
	}

	virtual grpc::Status GetDeviceErrors(grpc::ServerContext* context, const ::google::protobuf::Empty* request, DeviceErrorsCollectionDto* response) override
	{
		PrintTitle("GetDeviceErrors");

		for (int i = 0; i < 10; i++)
		{
			DeviceErrorDto* deviceDto = response->add_deviceerrors();
			SetDeviceErrorDto(*deviceDto);
		}

		return grpc::Status::OK;
	}

	virtual grpc::Status SubscribeToDeviceErrors(grpc::ServerContext* context, const google::protobuf::Empty* request, grpc::ServerWriter<DeviceErrorDto>* writer) override
	{
		PrintTitle("SubscribeToDeviceErrors");

		int index = 0;
		DeviceErrorDto errorDto;

		while (!context->IsCancelled())
		{
			WaitForSingleObject(hTimer, 1000);

			std::cout << "Write " << ++index << std::endl;

			SetDeviceErrorDto(errorDto);
			writer->Write(errorDto);
		}

		return grpc::Status::OK;
	}

	virtual grpc::Status ClearErrors(grpc::ServerContext* context, const google::protobuf::Empty* request, ResultMessageDto* response) override
	{
		PrintTitle("ClearErrors");

		response->set_result(true);

		return grpc::Status::OK;
	}

	virtual grpc::Status SetExposureParameters(grpc::ServerContext* context, const ExposureParametersDto* request, ExposureParametersDto* response) override
	{
		PrintTitle("SetExposureParameters");

		exposureDto = *request;

		if (exposureDto.kv() > 8000)
			exposureDto.set_kv(8000);

		if (exposureDto.ma() > 1.25)
			exposureDto.set_ma(1.25);

		google::protobuf::Duration dt = exposureDto.exposuretime();
		if (dt.seconds() > 8)
		{
			dt.set_seconds(8);
			exposureDto.set_allocated_exposuretime(new google::protobuf::Duration(dt));
		}

		*response = exposureDto;

		return grpc::Status::OK;
	}

	virtual grpc::Status GetExposureParameters(grpc::ServerContext* context, const google::protobuf::Empty* request, ExposureParametersDto* response)  override
	{
		PrintTitle("GetExposureParameters");

		response->set_kv(exposureDto.kv());
		response->set_ma(exposureDto.ma());
		response->set_allocated_exposuretime(new google::protobuf::Duration(exposureDto.exposuretime()));

		return grpc::Status::OK;
	}
};

class UserService final : public UserServiceOperations::Service
{
	virtual grpc::Status StartNotificationStream(grpc::ServerContext* context, const google::protobuf::Empty* request, grpc::ServerWriter<NotificationMessageDto>* writer) override
	{
		PrintTitle("StartNotificationStream - begin");

		NotificationMessageDto messageDto;

		while (!context->IsCancelled())
		{
			WaitForSingleObject(hTimer, 10000);

			std::cout << "Write " << std::endl;

			setMessage(messageDto);

			writer->Write(messageDto);
		}

		PrintTitle("StartNotificationStream - end");

		return grpc::Status::OK;
	}
	virtual ::grpc::Status ProcessFile(::grpc::ServerContext* context, const ::DicomFileDto* request, ::ProcessResult* response)
	{
		PrintTitle("ProcessFile");
		setMessageXML(*response);

		return grpc::Status::OK;
	}

	void setMessageXML(ProcessResult& response)
	{
		MetadataDto* meta = new MetadataDto;
		
		meta = response.add_metadata();
		meta->set_key("XML");
		meta->set_value("some");
	}
	

	void setMessage(NotificationMessageDto& messageDto)
	{
		messageDto.set_message("Message");
	}
};

void RunServer()
{
	PrintTitle("RunServer");

	const std::string server_address("localhost:50051"); //("0.0.0.0:50051");//("192.168.105.21:50051"); 

	//MedicalGeneratorService service;
	UserService service;

	grpc::ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	hTimer = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();

	CloseHandle(hTimer);
}

int main(int argc, char* argv[])
{
	RunServer();

	return 0;
}