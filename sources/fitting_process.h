#ifndef FITTING_PROCESS
#define FITTING_PROCESS

#include "fitting_tree.h"
#include "crystal_geometry.h"

#include <vector>
#include <iostream>
#include <string>

#include <TH1.h>
#include <TMath.h>
#include <TFile.h>

#include <RooExponential.h>
#include <RooNovosibirsk.h>
#include <RooRealVar.h>
#include <RooPlot.h>
#include <RooDataHist.h>
#include <RooAddPdf.h>

// TODO: Determine RANGE_MIN, RANGE_MAX dynamically from histogram.
// TODO: Determine SIGMEAN_MIN/MAX dynamically from histogram.
// TODO: Determine DRAW_PLOT from configuration file.

/**
 * Class to process amplitude histograms and output fitting results.
 */
class FittingProcess
{
private:
  std::vector<int> m_crystal;
  std::vector<TH1F*> m_hist;
  std::vector<RooDataHist*> m_dh;
  FittingTree* m_ftree;
  std::string m_fname;

  int iter_count = 10; /**< Max iterations count. */

  /**
   * Thread id. This variable is unused for now since implementing parallel
   * fitting in RooFit seems to be impossible.
   */
  int m_id;

  /**
   * Energy, usually ADC counts.
   */
  RooRealVar* en;
  /**
   * Peak position for log-gauss part of the fitting function.
   */
  RooRealVar* sigmean;
  /**
   * Peak width for log-gauss part of the fitting function.
   */
  RooRealVar* sigwidth;
  /**
   * Asymmetry coefficient for log-gauss part of the fitting function.
   */
  RooRealVar* sigtail;

  /**
   * Fitting function, Log-gauss part (aka Novosibirsk function).
   */
  RooNovosibirsk* signal;

  /**
   * Coefficient for exponential part of the fitting function.
   */
  RooRealVar* exppar;
  /**
   * Fitting function, exponential to account for particles that didn't pass
   * through the opposite sides of the crystal.
   */
  RooExponential* background;
  /**
   * Number of entries in histogram where particle passed through opposite
   * sides of the crystal.
   */
  RooRealVar* nsig;
  /**
   * Number of entries in histogram where particle passed through adjacent
   * sides of the crystal.
   */
  RooRealVar* nbkg;

  /**
   * Sum of signal and background functions for signal_only == false.
   * Signal function only for signal_only == true.
   */
  RooAddPdf* model;

  /**
   * Result of the fit.
   */
  RooFitResult* result;

  /**
   * @brief Drop 'constant' attributes so the fitting can be done again.
   */
  void resetCoefs(int iter);

  void fitHist(int crystal, TH1F* hist, bool signal_only, RooDataHist &dh);

  TString getName(const char* name);

public:
  FittingProcess(const char* fname, int id = 0);

  void addHistogram(int crystal, TH1F* hist);
  int run(bool signal_only = false);

  void setOutputFile(const char* fname);

  void setIterCount(int iter_count);
  void setSigmeanMax(int sigmean_max);
  void setSigmeanMin(int sigmean_min);
  void setRangeMin(int range_min);
  void setRangeMax(int range_max);

  FittingTree* getResult();
};

#endif // FITTING_PROCESS

