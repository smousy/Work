#include "ThreadWorker.h"

void ThreadWorker::MainLoop()
{
  threadRunning = true;
  threadCancelled = false;

  if (workingProcedure)
    workingProcedure(threadRunning);

  threadRunning = false;
  threadCancelled = true;
}

void ThreadWorker::SetWorkingProcedure(ThreadProc workingProcedure)
{
  this->workingProcedure = workingProcedure;
}

void ThreadWorker::StartWork()
{
  std::thread mainLoopThread(&ThreadWorker::MainLoop, this);

  mainLoopThread.detach();
}

void ThreadWorker::StopWork()
{
  threadRunning = false;
}

bool ThreadWorker::IsCancelled()
{
  return threadCancelled;
}