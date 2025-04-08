#include <napi.h>
#include "SimpleCapture.h"

class SimpleCaptureWrapper : public Napi::ObjectWrap<SimpleCaptureWrapper> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        SimpleCaptureWrapper(const Napi::CallbackInfo& info);
    
    private:
        Napi::Value StartCapture(const Napi::CallbackInfo& info);
        Napi::Value CreateSurface(const Napi::CallbackInfo& info);
        Napi::Value Close(const Napi::CallbackInfo& info);
    
        std::unique_ptr<SimpleCapture> m_capture;
};
    
Napi::Object SimpleCaptureWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SimpleCapture", {
        InstanceMethod("startCapture", &SimpleCaptureWrapper::StartCapture),
        InstanceMethod("createSurface", &SimpleCaptureWrapper::CreateSurface),
        InstanceMethod("close", &SimpleCaptureWrapper::Close),
    });

    exports.Set("SimpleCapture", func);
    return exports;
}

SimpleCaptureWrapper::SimpleCaptureWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SimpleCaptureWrapper>(info) 
{
    Napi::Env env = info.Env();

    try {
        // Теперь не требуется никаких аргументов
        m_capture = std::make_unique<SimpleCapture>();
    } catch (const winrt::hresult_error& e) {
        throw Napi::Error::New(env, winrt::to_string(e.message()).c_str());
    }
}

Napi::Value SimpleCaptureWrapper::StartCapture(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    try {
        if (m_capture) {
            m_capture->StartCapture();
        } else {
            throw Napi::Error::New(env, "SimpleCapture is not initialized");
        }
    } catch (const winrt::hresult_error& e) {
        throw Napi::Error::New(env, winrt::to_string(e.message()).c_str());
    }

    return env.Undefined();
}

Napi::Value SimpleCaptureWrapper::CreateSurface(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsExternal()) {
        throw Napi::Error::New(env, "Expected Compositor as first argument");
    }

    try {
        auto compositor = info[0].As<Napi::External<
            winrt::Windows::UI::Composition::Compositor>>().Data();
        
        if (m_capture) {
            auto surface = m_capture->CreateSurface(*compositor);
            return Napi::External<winrt::Windows::UI::Composition::ICompositionSurface>::
                New(env, &surface);
        }
        throw Napi::Error::New(env, "SimpleCapture is not initialized");
    } catch (const winrt::hresult_error& e) {
        throw Napi::Error::New(env, winrt::to_string(e.message()).c_str());
    }
}

Napi::Value SimpleCaptureWrapper::Close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (m_capture) {
        m_capture->Close();
    }

    return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    SimpleCaptureWrapper::Init(env, exports);
    return exports;
}

NODE_API_MODULE(addon, Init)