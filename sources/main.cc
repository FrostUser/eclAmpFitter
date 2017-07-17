#include "fitting_tree.h"
#include "fitting_process.h"

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TThread.h>

#include <RooMsgService.h>

#include "config_loader.h"

#include <iostream>

using namespace RooFit;

int main(int argc, const char** argv)
{
  if (argc < 2) {
    // See config files in config/*.cfg for examples.
    std::cout << "Usage:" << std::endl
              << "   " << argv[0] << " config_file" << std::endl;

    return -1;
  }

  ConfigLoader cfg_loader(argv[1]);

  TFile* in = new TFile(cfg_loader.get("input").c_str(), "READ");
  TH1F* hist;
  FittingProcess proc(cfg_loader.get("output").c_str());

  // Quiet mode for fitting.
  RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);
  // Add this for silent mode.
  RooMsgService::instance().setSilentMode(true);

  for (int i = cfg_loader.getInt("min_crystal"); i <= cfg_loader.getInt("max_crystal"); i++) {
    TString hist_name = TString(cfg_loader.get("hist_name_prefix")) + Long_t(i);
    hist = (TH1F*)in->Get(hist_name);

    if (hist == 0) {
      std::cout << "Warning: no histogram at " << hist_name << std::endl;
      continue;
    }

    proc.addHistogram(i, hist);
  }

  proc.setIterCount(cfg_loader.getInt("iter_count"));

  proc.run(cfg_loader.getBool("signal_only"));

  in->Close();

  std::cout << "Done." << std::endl;

  return 0;
}
