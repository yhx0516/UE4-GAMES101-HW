// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
//#include "Hw7_Main.h"

class AHw7_Main;
/**
 * 
 */
class GAMES101_API FPathTracingCalcThread: public FRunnable
{
public:
	FPathTracingCalcThread(FString _ThreadName, AHw7_Main* _RenderCore);
	~FPathTracingCalcThread();
	
	void ShutDown(bool bShouldWait);
private:
	int32 m_ThreadID;
	FString m_ThreadName;	
	FRunnableThread* ThreadIns;
	
	AHw7_Main* RenderCore; 
	

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	
};
