#include <node.h>
#include <nan.h>
#include <unistd.h>
#include <mutex>
#include "ad-da.h"

using namespace v8;

extern int initialized;

std::mutex sensorMutex;

int _channel = 0; // default ADC channel
int _max_retries = 3;

class ReadWorker : public Nan::AsyncWorker {
  public:
    ReadWorker(Nan::Callback *callback, int channel): Nan::AsyncWorker(callback), channel(channel) { }

    void Execute() {
      printf("Enter Execute function... \n");
      sensorMutex.lock();
      Init();
      Read();
      sensorMutex.unlock();
    }

    void HandleOKCallback() {
      Nan:: HandleScope scope;

      Local<Value> argv[3];
      argv[1] = Nan::New<Array>(*adc),
      argv[2] = Nan::New<Array>(*volt);

      if (!initialized) {
        argv[0] = Nan::Error("failed to initialize ADC");
      } else if (failed) {
        argv[0] = Nan::Error("failed to read ADC");
      } else {
        argv[0] = Nan::Null();
      }

      printf("%8ld, %03ld V \r\n", adc[7], volt[7]);

      callback->Call(3, argv);
    }

  private:
    int channel;
    int32_t adc[8];
    int32_t volt[8];
    bool failed = false;

    void Init() {
      //printf("Enter ReadWorker::Init function... \n");
      if (!initialized) {
        initialized = initialize() == 0;
      }
    }

    void Read() {
      //printf("Enter ReadWorker::Read function... \n");

      //todo init adc and volt

      int retry = _max_retries;
      int result = 0;
      while (true) {
        result = readADC(adc, volt);
        //printf("%8ld, %03ld V \r\n", adc[7], volt[7]);
        if (result == 0 || --retry < 0) break;
        usleep(450000);
      }
      failed = result != 0;
    }
};

void ReadAsync(const Nan::FunctionCallbackInfo<Value>& args) {
  //printf("Enter ReadAsync function... \n");
  int channel = Nan::To<int>(args[0]).FromJust();
  Nan::Callback *callback = new Nan::Callback(args[1].As<Function>());

  Nan::AsyncQueueWorker(new ReadWorker(callback, channel));
}

void ReadSync(const Nan::FunctionCallbackInfo<Value>& args) {
  //printf("Enter ReadSync function... \n");
  int channel;

  if (args.Length() == 2) {
    
    channel = args[0]->Uint32Value();

    // initialization (on demand)
    if (!initialized) {
      initialized = initialize() == 0;
      if (!initialized) {
        Nan::ThrowTypeError("failed to initialize");
        return;
      }
    }
  } else {
    channel = _channel;
  }
  
  int32_t adc[8];
  int32_t volt[8];
  int retry = _max_retries;
  int result = 0;
  while (true) {
    result = readADC(adc, volt);
    if (result == 0 || --retry < 0) break;
    usleep(450000);
  }

  //printf("%8ld, %03ld V \r\n", adc[7], volt[7]);

  Local<Object> readout = Nan::New<Object>();
  readout->Set(Nan::New("adc").ToLocalChecked(), Nan::New<Array>(*adc));
  readout->Set(Nan::New("volt").ToLocalChecked(), Nan::New<Array>(*volt));
  readout->Set(Nan::New("isValid").ToLocalChecked(), Nan::New<Boolean>(result == 0));
  readout->Set(Nan::New("errors").ToLocalChecked(), Nan::New<Number>(_max_retries - retry));

  args.GetReturnValue().Set(readout);
}

void Read(const Nan::FunctionCallbackInfo<Value>& args) {
  //printf("Enter global Read function... \n");
  int params = args.Length();
  switch(params) {
    case 1: // AD channel given
      ReadSync(args);
      break;
    case 2: // AD channel + callback function given
      ReadAsync(args);
      break;
    default:
      Nan::ThrowTypeError("invalid number of arguments");
  }
}

void SetMaxRetries(const Nan::FunctionCallbackInfo<Value>& args) {
    if (args.Length() != 1) {
      Nan::ThrowTypeError("Wrong number of arguments");
      return;
    }
    _max_retries = args[0]->Uint32Value();
}

void Initialize(const Nan::FunctionCallbackInfo<Value>& args) {
  //printf("Enter Initialize function... \n");

}

void Init(Handle<Object> exports) {
  //printf("Enter global Init function... \n");
	Nan::SetMethod(exports, "read", Read);
	Nan::SetMethod(exports, "initialize", Initialize);
  Nan::SetMethod(exports, "setMaxRetries", SetMaxRetries);
}

NODE_MODULE(npm_demo, Init);
