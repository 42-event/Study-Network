// NChatServer_Select.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <SDSSelect.h>
#include <SDSWorker.h>
#include <SDSException.h>

#include "NChatServer.h"
#include "NChatClient.h"

//TODO: Load from config files
constexpr const char* HOST = ""; //"127.0.0.1", Empty means ANY
constexpr int PORT = 11021;
constexpr int BACKLOG = 10;

int main()
{
    try
    {
        SDSGlobal _RAII;

        auto worker = std::make_shared<SDSWorker>();
        worker->Init();

        auto svr = std::make_shared<NChat::NChatServer>(HOST, PORT, BACKLOG);
        svr->CreateSocket();
        svr->Listen();
        svr->SetWorker(worker);
        worker->InsertUnit(svr);

        worker->BeginThread();

        std::cout << "Server online" << std::endl;
        worker->JoinThread();
        std::cout << "END!!" << std::endl;

        return 0;
    }
    catch (const SDSWSException& ex)
    {
        std::cout << "Crash cause Samdasoo Windows Socket Exception!!: " << ex.what() << std::endl;
    }
    catch (const SDSException& ex)
    {
        std::cout << "Crash cause Samdasoo Exception!!: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Crash cause Exception!!: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Crash cause Unknown Exception!!" << std::endl;
    }
    return 1;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
