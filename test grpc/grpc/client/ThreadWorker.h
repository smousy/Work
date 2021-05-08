#pragma once

#include <thread>
#include <functional>

typedef std::function<void(const bool&)> ThreadProc;

class ThreadWorker
{
private:
  bool threadRunning = false;
  bool threadCancelled = true;
  ThreadProc workingProcedure = NULL;

  void MainLoop();
public:
  void SetWorkingProcedure(ThreadProc workingProcedure);
  void StartWork();
  void StopWork();
  bool IsCancelled();
};