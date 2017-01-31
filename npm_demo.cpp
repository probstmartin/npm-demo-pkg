#include <node.h>
#include <nan.h>
#include <unistd.h>

using namespace v8;

class ReadWorker : public Nan::AsyncWorker {
  public:
    ReadWorker(Nan::Callback *callback): Nan::AsyncWorker(callback) {

    }

    void Execute() {
      Init();
      Read();
    }

    void HandleOKCallback() {
    }

  private:

    void Init() {
    }

    void Read() {
    }
};

void ReadAsync(const Nan::FunctionCallbackInfo<Value>& args) {
  Nan::Callback *callback = new Nan::Callback(args[0].As<Function>());
  Nan::AsyncQueueWorker(new ReadWorker(callback));
}

void ReadSync(const Nan::FunctionCallbackInfo<Value>& args) { 
}

void Read(const Nan::FunctionCallbackInfo<Value>& args) {
  printf("Enter Read function... \n");
}

void Initialize(const Nan::FunctionCallbackInfo<Value>& args) {
  printf("Enter Initialize function... \n");
}

void Init(Handle<Object> exports) {
  printf("Enter Init function... \n");
	Nan::SetMethod(exports, "read", Read);
	Nan::SetMethod(exports, "initialize", Initialize);
}

NODE_MODULE(npm_demo, Init);
