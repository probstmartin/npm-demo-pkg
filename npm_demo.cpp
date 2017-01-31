#include <node.h>
#include <nan.h>
#include <unistd.h>
#include "ad-da.h"

using namespace v8;

extern int initialized;

int _channel = 0; // default ADC channel
int _max_retries = 3;

class ReadWorker : public Nan::AsyncWorker {
  public:
    ReadWorker(Nan::Callback *callback, int channel): Nan::AsyncWorker(callback), channel(channel) {

    }

    void Execute() {
      Init();
      Read();
    }

    void HandleOKCallback() {
    }

  private:
    int channel;

    void Init() {
    }

    void Read() {
    }
};

void ReadAsync(const Nan::FunctionCallbackInfo<Value>& args) {
  int channel = Nan::To<int>(args[0]).FromJust();
  Nan::Callback *callback = new Nan::Callback(args[1].As<Function>());

  Nan::AsyncQueueWorker(new ReadWorker(callback, channel));
}

void ReadSync(const Nan::FunctionCallbackInfo<Value>& args) {
  int channel;

  if (args.Length() == 1) {
    
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

  Local<Object> readout = Nan::New<Object>();
  readout->Set(Nan::New("humidity").ToLocalChecked(), Nan::New<Number>(humidity));
  readout->Set(Nan::New("temperature").ToLocalChecked(), Nan::New<Number>(temperature));
  readout->Set(Nan::New("isValid").ToLocalChecked(), Nan::New<Boolean>(result == 0));
  readout->Set(Nan::New("errors").ToLocalChecked(), Nan::New<Number>(_max_retries - retry));

  args.GetReturnValue().Set(readout);
}

void Read(const Nan::FunctionCallbackInfo<Value>& args) {
  printf("Enter Read function... \n");
  int params = args.Length();
  switch(params) {
    case 1: // AD channel given
      ReadSync(args);
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
  printf("Enter Initialize function... \n");

}

void Init(Handle<Object> exports) {
  printf("Enter Init function... \n");
	Nan::SetMethod(exports, "read", Read);
	Nan::SetMethod(exports, "initialize", Initialize);
  Nan::SetMethod(exports, "setMaxRetries", SetMaxRetries);
}

NODE_MODULE(npm_demo, Init);
