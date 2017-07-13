// I intend to later use this file to make a multithreaded version of the project.
// Will I succeed or not? Due to the nature of RooFit, failure is quite likely to occur.

//#include <iostream>

//void testTree()
//{
//  TFile* f = new TFile("out.root", "RECREATE");
//  FittingTree t;

//  t.setDouble("ff", 0.1);
//  t.setInt("gg", 2);
//  t.Fill();

//  t.setDouble("ff", 0.2);
//  t.setInt("gg", 4);
//  t.Fill();

//  t.Write();
//  t.getTree()->Scan();
//  f->Close();
//}

//typedef struct ThreadData
//{
//  TFile* in;
//  int id;
//} ThreadData;

//void* fittingThread(void* data)
//{
//  std::cout << "Thread started" << std::endl;
//  FittingProcess* proc = (FittingProcess*)data;
//  proc->run(false);
////  delete proc;

////  ThreadData* tdata = (ThreadData*)data;
////  int id = tdata->id;
////  TFile* in = tdata->in;
////  TString name = TString("out") + Long_t(id) + TString(".root");

////  TThread::Lock();
////  FittingProcess* proc = new FittingProcess(name, id);

////  for (int i = 1153 + 50*id; i < 1183 + 50*id; i++) {
////    proc->addHistogram(i, (TH1F*)in->Get(TString("hist_") + Long_t(i)));
////  }
////  TThread::UnLock();

////  proc->run(false);

//  return 0;
//}

//void threadedExecution()
//{
//  TFile* in = new TFile("/home/frostuser/Code/Belle2/CrystalCalibration/00_Scripts/histos_2017mar.root", "READ");
////  TFile* in = new TFile("../root_scripts/histos_2017mar.root", "READ");
//  TH1F* hist;

//  const int N_PROC = 2;
//  ThreadData* data[N_PROC];

//  for (int i = 0; i < N_PROC; i++) {
//    data[i] = new ThreadData();
//    data[i]->id = i;
//    data[i]->in = in;
//  }
//  TThread* t[N_PROC];

//  for (int i = 0; i < N_PROC; i++) {
//    t[i] = new TThread(TString("t") + Long_t(i), fittingThread);
//    t[i]->Run((void*)data[i]);
//  }

//  TThread::Ps();

//  for (int i = 0; i < N_PROC; i++) {
//    t[i]->Join();
//  }
//}

//void threadedExecutionOld()
//{
//  TFile* in = new TFile("/home/frostuser/Code/Belle2/CrystalCalibration/00_Scripts/histos_2017mar.root", "READ");
////  TFile* in = new TFile("../root_scripts/histos_2017mar.root", "READ");
//  TH1F* hist;

//  const int N_PROC = 2;
//  FittingProcess* proc[N_PROC];
//  ThreadData* data[N_PROC];

//  for (int i = 0; i < N_PROC; i++) {
//    TString name = TString("out") + Long_t(i) + TString(".root");
//    proc[i] = new FittingProcess(name, i);
//  }
//  for (int i = 1153; i <= 1653; i++) {
//    hist = (TH1F*)in->Get(TString("hist_") + Long_t(i));

//    int k = (i - 1153) * N_PROC / 500;
//    if (k > N_PROC - 1) k = N_PROC - 1;
//    proc[k]->addHistogram(i, hist);
//  }

//  TThread* t[N_PROC];

//  for (int i = 0; i < N_PROC; i++) {
//    t[i] = new TThread(TString("t") + Long_t(i), fittingThread);
//    t[i]->Run((void*)proc[i]);
//  }

//  TThread::Ps();

//  for (int i = 0; i < N_PROC; i++) {
//    t[i]->Join();
//  }
//}
