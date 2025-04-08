#include "pch.h"
#include "SimpleCapture.h"

using namespace winrt;
using namespace Windows;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::Graphics;
using namespace Windows::Graphics::Capture;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Composition;

SimpleCapture::SimpleCapture()
{
    // Создаем Direct3D устройство
    m_device = CreateDirect3DDevice();

    // Получаем первый монитор
    auto interop = get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
    HWND hwnd = nullptr;
    
    // Получаем handle основного монитора
    HMONITOR hmon = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    
    // Создаем CaptureItem для монитора
    check_hresult(interop->CreateForMonitor(
        hmon,
        guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(put_abi(m_item))));

    // Настраиваем Direct3D
    auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    d3dDevice->GetImmediateContext(m_d3dContext.put());

    auto size = m_item.Size();

    // Создаем swap chain
    m_swapChain = CreateDXGISwapChain(
        d3dDevice, 
        static_cast<uint32_t>(size.Width),
        static_cast<uint32_t>(size.Height),
        static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized),
        2);

    // Создаем пул кадров
    m_framePool = Direct3D11CaptureFramePool::Create(
        m_device,
        DirectXPixelFormat::B8G8R8A8UIntNormalized,
        2,
        size);

    m_session = m_framePool.CreateCaptureSession(m_item);
    m_lastSize = size;
    m_frameArrived = m_framePool.FrameArrived(auto_revoke, { this, &SimpleCapture::OnFrameArrived });
}

IDirect3DDevice SimpleCapture::CreateDirect3DDevice()
{
    // Создаем D3D11 устройство
    com_ptr<ID3D11Device> d3dDevice;
    D3D_FEATURE_LEVEL featureLevel;
    com_ptr<ID3D11DeviceContext> context;
    
    D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        d3dDevice.put(),
        &featureLevel,
        context.put());

    // Конвертируем в WinRT устройство
    com_ptr<::IInspectable> inspectable;
    check_hresult(CreateDirect3D11DeviceFromDXGIDevice(
        d3dDevice.as<IDXGIDevice>().get(),
        inspectable.put()));

    return inspectable.as<IDirect3DDevice>();
}

// Start sending capture frames
void SimpleCapture::StartCapture()
{
    CheckClosed();
    m_session.StartCapture();
}

ICompositionSurface SimpleCapture::CreateSurface(
    Compositor const& compositor)
{
    CheckClosed();
    return CreateCompositionSurfaceForSwapChain(compositor, m_swapChain.get());
}

// Process captured frames
void SimpleCapture::Close()
{
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {
		m_frameArrived.revoke();
		m_framePool.Close();
        m_session.Close();

        m_swapChain = nullptr;
        m_framePool = nullptr;
        m_session = nullptr;
        m_item = nullptr;
    }
}

void SimpleCapture::OnFrameArrived(
    Direct3D11CaptureFramePool const& sender,
    winrt::Windows::Foundation::IInspectable const&)
{
    auto newSize = false;

    {
        auto frame = sender.TryGetNextFrame();
		auto frameContentSize = frame.ContentSize();

        if (frameContentSize.Width != m_lastSize.Width ||
			frameContentSize.Height != m_lastSize.Height)
        {
            // The thing we have been capturing has changed size.
            // We need to resize our swap chain first, then blit the pixels.
            // After we do that, retire the frame and then recreate our frame pool.
            newSize = true;
            m_lastSize = frameContentSize;
            m_swapChain->ResizeBuffers(
                2, 
				static_cast<uint32_t>(m_lastSize.Width),
				static_cast<uint32_t>(m_lastSize.Height),
                static_cast<DXGI_FORMAT>(DirectXPixelFormat::B8G8R8A8UIntNormalized), 
                0);
        }

        {
            auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
            
            com_ptr<ID3D11Texture2D> backBuffer;
            check_hresult(m_swapChain->GetBuffer(0, guid_of<ID3D11Texture2D>(), backBuffer.put_void()));

            m_d3dContext->CopyResource(backBuffer.get(), frameSurface.get());
        }
    }

    DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
    m_swapChain->Present1(1, 0, &presentParameters);

    if (newSize)
    {
        m_framePool.Recreate(
            m_device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            m_lastSize);
    }
}