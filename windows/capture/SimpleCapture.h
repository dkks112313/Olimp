#pragma once
#include "pch.h"

class SimpleCapture
{
public:
    SimpleCapture();
    void StartCapture();
    winrt::Windows::UI::Composition::ICompositionSurface CreateSurface(
        winrt::Windows::UI::Composition::Compositor const& compositor);
    void Close();

private:
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice CreateDirect3DDevice();
    void OnFrameArrived(
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
        winrt::Windows::Foundation::IInspectable const& args);

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
    winrt::com_ptr<ID3D11DeviceContext> m_d3dContext;
    winrt::com_ptr<IDXGISwapChain1> m_swapChain;
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
    winrt::Windows::Graphics::SizeInt32 m_lastSize;
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived;
    std::atomic<bool> m_closed = false;

    void CheckClosed()
    {
        if (m_closed)
        {
            throw winrt::hresult_error(RO_E_CLOSED);
        }
    }
};