// Fill out your copyright notice in the Description page of Project Settings.


#include "PathTracingCalcThread.h"
#include "Hw7_Main.h"

FPathTracingCalcThread::FPathTracingCalcThread(FString _ThreadName, AHw7_Main* _RenderCore)
	:m_ThreadName(_ThreadName), RenderCore(_RenderCore)
{
	m_ThreadName = _ThreadName;
	ThreadIns = FRunnableThread::Create(this, *m_ThreadName);
	m_ThreadID = ThreadIns->GetThreadID();
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__" thread %d create"), m_ThreadID);
}

FPathTracingCalcThread::~FPathTracingCalcThread()
{
}

bool FPathTracingCalcThread::Init()
{
	return IsValid(RenderCore);
}

uint32 FPathTracingCalcThread::Run()
{
	static FCriticalSection m_mutex; //声明 static 可以让线程之间互锁
	while (IsValid(RenderCore) && !RenderCore->bTaskFinished)
	{
		ScreenCoords screenCoords;
		bool bDequeSucc = false;
		
			
			m_mutex.Lock();  // 上锁
			FPlatformMisc::MemoryBarrier(); // 内存屏障
			bDequeSucc = RenderCore->rayTraceQueue.Dequeue(screenCoords);
			m_mutex.Unlock();

		if (bDequeSucc) {
			float fov = RenderCore->fov;
			float width = RenderCore->width;
			float height = RenderCore->height;

			float scale = UKismetMathLibrary::DegTan(fov * 0.5f);
			float imageAspectRatio = width / (float)height;
			FVector eye_pos = RenderCore->cameraComp->GetComponentLocation();

			float x = (2 * (screenCoords.x + 0.5) / (float)width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (screenCoords.y + 0.5) / (float)height) * scale;

			FVector dir = FVector(1, x, y);
			dir.Normalize();
			FVector hit_color = RenderCore->Render(Ray(eye_pos, dir), 0);
			
			ensure(RenderCore->pixelQueue.Enqueue(ScreenPixel(screenCoords.y * width + screenCoords.x, hit_color)));
			
			

			}
		else {
			FPlatformProcess::Sleep(.01f); //延时太长 RenderCore->ComputeQueue 会析构
			//UE_LOG(LogTemp, Warning, TEXT("Deque Failed "));
		}



	}
	return 0;
}

void FPathTracingCalcThread::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__" Thread %d exit"), m_ThreadID);
}
void FPathTracingCalcThread::ShutDown(bool bShouldWait)
{
	if (ThreadIns)
	{
		ThreadIns->Kill(bShouldWait); // bShouldWait 为false，Suspend(true)时，会崩
	}
}