// AudioPeeperConsole.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <iostream>
#include <fstream>

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

int main()
{
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    WAVEFORMATEX* pWaveFormat = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;

    // 播放指令：ffplay.exe -ar 48000 -ac 2 -f f32le -i d:\peep.pcm
    std::ofstream outputFile("D:\\peep.pcm", std::ios::binary);
    UINT32 audioFrameSize = 0;
    UINT32 counter = 0;

    // 初始化COM组件
    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    EXIT_ON_ERROR(hr)

    // 获取音频设备枚举器
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    // 获取音频输出设备
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice); // eCapture
    EXIT_ON_ERROR(hr)

    // 打开音频输出设备
    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    // 创建音频客户端
    hr = pAudioClient->GetMixFormat(&pWaveFormat);
    EXIT_ON_ERROR(hr)
    // 计算音频帧的大小（单位：字节）
    audioFrameSize = (pWaveFormat->wBitsPerSample / 8) * pWaveFormat->nChannels;
    std::cout << "Audio format >> Channels: " << pWaveFormat->nChannels << " Sample-rate: " << pWaveFormat->nSamplesPerSec << " Bit-depth: " << pWaveFormat->wBitsPerSample << std::endl;

    // https://learn.microsoft.com/en-us/windows/win32/coreaudio/loopback-recording
    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 10000000, 0, pWaveFormat, nullptr);
    EXIT_ON_ERROR(hr)

    // 获取音频捕获客户端
    hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // 启动音频流
    hr = pAudioClient->Start();
    EXIT_ON_ERROR(hr)

    // 循环截获音频数据
    while (true) {
        // 获取音频数据缓冲区
        BYTE* pData;
        UINT32 numFramesAvailable;
        DWORD flags;
        hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, nullptr, nullptr);
        if (FAILED(hr)) {
            break;
        }

        // 处理音频数据（示例：打印数据长度）
        if (numFramesAvailable > 0) {
            std::cout << "Received audio data: " << numFramesAvailable << " frames" << std::endl;
            // 将缓冲区的指定长度数据写入文件
            outputFile.write(reinterpret_cast<char*>(pData), numFramesAvailable * audioFrameSize);

            if (++counter > 1000) break;
        }

        // 释放缓冲区
        hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
        if (FAILED(hr)) {
            break;
        }
    }

    // 停止音频流
    pAudioClient->Stop();
    outputFile.close();

Exit:
    // 释放资源
    CoTaskMemFree(pWaveFormat);
    SAFE_RELEASE(pCaptureClient)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pEnumerator)
    // 反初始化COM组件
    CoUninitialize();

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
