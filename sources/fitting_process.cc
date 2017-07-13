#include "fitting_process.h"

using namespace RooFit;

const bool DRAW_PLOT = true;
const double RANGE_MIN = 250;
const double RANGE_MAX = 3000;
const double SIGMEAN_MIN = 335;
const double SIGMEAN_MAX = 1380;

void FittingProcess::resetCoefs(int iter)
{
  sigmean->setConstant(false);
  sigwidth->setConstant(false);
  if (iter <= 0) {
    sigtail->setConstant(false);
    exppar->setConstant(false);
    nsig->setConstant(false);
    nbkg->setConstant(false);
  }
}

void FittingProcess::fitHist(int crystal, TH1F *hist, bool signal_only, RooDataHist &dh)
{
  /*** Specific range limitations ***/
  // These are range limitations for the specific task of
  // Belle2 ECL energy calibrations.
  Double_t min = RANGE_MIN;
  Double_t max = RANGE_MAX;

  ECLCrystalGeometry* geom = ECLCrystalGeometry::getInstance();

  Int_t N = hist->GetEntries();

  Double_t peak_pos = hist->GetMean(); // Approximate peak position:

  // --- Parameters ---

  en->SetTitle(TString("Channel ") + Long_t(crystal));
  en->setMin(min);
  en->setMax(max);

  sigmean ->setMin(0.5*peak_pos);
  sigmean ->setMax(1.4*peak_pos);
  sigmean ->setVal(0.78*peak_pos);
  sigwidth->setMin(0.07*peak_pos);
  sigwidth->setMax(0.28*peak_pos);
  sigwidth->setVal(0.14*peak_pos);
  sigtail ->setMin(-0.7);
  sigtail ->setMax(-0.02);
  sigtail ->setVal(-0.2);

  // --- If signal_only, estimate asymmetry coefficient from phi_id. ---
  double estimate = 0;
  if (signal_only && peak_pos > 500) {
    int phi_id = geom->getPhiId(crystal);
    double phi = phi_id * TMath::Pi() / 72;

    estimate = -0.65 + 0.14*pow(cos(phi), 2);
    sigtail->setMin(-0.9); // TODO: Not the best approach to fitting.
    sigtail->setVal(estimate);
  }

  // --- If signal_only, set narrow range: ---
  double left = min, right = max;

  if (signal_only) {
    if (N > 200) {
      double l = sigtail ->getVal();
      double s = sigwidth->getVal();
      double a = sigmean ->getVal();

      // 0.015 means that the selected range will cover 100-2*(1.5) == 97%
      // of the initial distribution area.
      double e = sqrt(pow(l, 4) - pow(l, 2)*2.0*log(0.015));

      left  = a - (exp(-e) - 1) * s * sqrt(log(4.)) / sinh(l*sqrt(log(4.)));
      right = a - (exp( e) - 1) * s * sqrt(log(4.)) / sinh(l*sqrt(log(4.)));
      if (min < left)  min = left;
      if (max > right) max = right;
      if (sigmean->getMin() < min) sigmean->setMin(min);
      if (sigmean->getMax() > max) sigmean->setMax(max);
    }
  }

  // --- Fitting only in specified range ---
  en->setRange("Cut", min, max);

  exppar->setMin(-16e-2);
  exppar->setMax(-0.1e-6);
  exppar->setVal(-1.0/hist->GetMean());

  nsig->setMin(10);
  nbkg->setMin(10);
  nsig->setMax(N);
  nbkg->setMax(N);
  nsig->setVal(0.33*N);
  nbkg->setVal(0.66*N);

  // --- Reset parameters for next fit ---
  resetCoefs(0);

  double* sigval = new double[iter_count];
  double* sigerr = new double[iter_count];

  // --- Perform extended ML fit of composite PDF ---
//  RooFitResult* result;
  if (!signal_only)
    result = model->fitTo(dh, Range("Cut"));
  else
    result = signal->fitTo(dh, Range("Cut"));

  int i = 0;
  // --- Make consecutive fits in the smaller range ---

  sigval[0] = sigmean->getVal();
  sigerr[0] = sigmean->getError();

  for (i = 1; i < iter_count; i++) {
    std::cout << "=============================================" << std::endl;
    std::cout << "Iteration " << i+1 << std::endl;
    std::cout << "=============================================" << std::endl;
    // --- Updating range ---
    if (!signal_only) {
      min   = sigmean->getVal() - 0.65*sigmean->getVal();
      max   = sigmean->getVal() + 1.25*sigmean->getVal();
    } else {
      double l = sigtail ->getVal();
      double s = sigwidth->getVal();
      double a = sigmean ->getVal();

      // 0.015 means that the selected range will cover 100-2*(1.5) == 97%
      // of the initial distribution area.
      double e = sqrt(pow(l, 4) - pow(l, 2)*2.0*log(0.015));

      left  = a - (exp(-e) - 1) * s * sqrt(log(4.)) / sinh(l*sqrt(log(4.)));
      right = a - (exp( e) - 1) * s * sqrt(log(4.)) / sinh(l*sqrt(log(4.)));
      min = left;
      max = right;
    }

    if (min < RANGE_MIN) min = RANGE_MIN;
    if (max > RANGE_MAX) max = RANGE_MAX;

    sigmean->setMin(min);
    sigmean->setMax(max);

    en->setRange("Cut", min, max);

    resetCoefs(i);

    if (!signal_only) {
      result = model->fitTo(dh, Range("Cut"));
    } else {
      result = signal->fitTo(dh, Range("Cut"));
    }

    sigval[i] = sigmean->getVal();
    sigerr[i] = sigmean->getError();
    // --- Checking the process for convergence
    if (sigmean->getError() / sigmean->getVal() < 0.5 / sqrt(N)) break;
  }
  // TODO: Determine iteration count correctly.
  std::cout << "=============================================" << std::endl;
  std::cout << "Stopped at iteration " << i+1 << std::endl;
  std::cout << "=============================================" << std::endl;

  double chi2 = -1;

  // --- Plot data and composite PDF overlaid ---
  if (DRAW_PLOT) {
    double min2, max2;
    dh.getRange(*en, min2, max2);
    en->setRange("Cut", min2, max2);

    RooPlot* enframe = en->frame();
    // --- Drawing data ---
    // dh.plotOn(enframe, Name("data"), LineColor(kWhite)); // Draw without error bars
    dh.plotOn(enframe, Name("data"), LineColor(kBlack), MarkerSize(0.5));
    if (!signal_only) {
      // Plotting background
      model->plotOn(enframe, Range("Cut"), NormRange("Cut"),
                    Components(*background),
                    LineStyle(kDashed));
      // Plotting signal
      model->plotOn(enframe, Range("Cut"), NormRange("Cut"));
      model->plotOn(enframe, LineColor(kRed), Name("model"), Range(min, max),
                    NormRange("Cut"));
      chi2 = enframe->chiSquare("model", "data", 6);
    } else {
      // Plotting signal
      signal->plotOn(enframe, Name("signal_pdf"), Range("Cut"), NormRange("Cut"));
      signal->plotOn(enframe, LineColor(kRed), Name("model"), Range(min, max), NormRange("Cut"));
      chi2 = enframe->chiSquare("model", "data", 3);
    }
    enframe->Draw();

    enframe->Write(TString("plot_") + Long_t(crystal));
  }

  // --- Saving data to FittingTree ---

  m_ftree->setInt("crystal", crystal);

  m_ftree->setDouble("sigmean", sigmean->getVal());
  m_ftree->setDouble("sigmean_err", sigmean->getError());
  m_ftree->setDouble("sigwidth", sigwidth->getVal());
  m_ftree->setDouble("sigtail", sigtail->getVal());
  m_ftree->setDouble("exppar", exppar->getVal());
  m_ftree->setDouble("chi2", chi2);
  m_ftree->setDouble("min", min);
  m_ftree->setDouble("max", max);
  m_ftree->setInt("iter", i);

  m_ftree->setInt("evt_count", N);

  m_ftree->setInt("phi", geom->getPhiId(crystal));
  m_ftree->setInt("theta", geom->getThetaId(crystal));

  // --- Small debug output after each fit ---

  std::cout << "N: " << N << std::endl;
  std::cout << "sigmean_err: " << sigmean->getError() << std::endl;
  std::cout << "Peak: " << peak_pos << std::endl;
  if (signal_only)
    std::cout << "Estimate: " << estimate << std::endl;
  std::cout << "Chi2: " << chi2 << std::endl;
  std::cout << "MINMAX: " << left << " " << right << std::endl;

}

TString FittingProcess::getName(const char *name)
{
  return TString(name);// + Long_t(m_id);
}

FittingProcess::FittingProcess(const char *fname, int id) :
  m_id(id)
{
  en = new RooRealVar("en", "en", 0);
  sigmean = new RooRealVar("sigmean", "sigmean", 0);
  sigtail = new RooRealVar("sigtail", "sigtail", 0);
  sigwidth = new RooRealVar("sigwidth", "sigwidth", 0);

  // --- Build asymmetric Gaussian PDF ---
  signal = new RooNovosibirsk("signal", "signal PDF", *en, *sigmean,
                              *sigwidth, *sigtail);
  // --- Build exponential background PDF ---
  exppar = new RooRealVar("exppar", "exponential parameter", 0);
  background = new RooExponential("background", "Exp PDF", *en, *exppar);

  // --- Construct signal+background PDF ---
  nsig = new RooRealVar("nsig", "#signal events", 0);
  nbkg = new RooRealVar("nbkg", "#background events", 0);
  model = new RooAddPdf("model", "sum", RooArgList(*signal, *background),
                        RooArgList(*nsig, *nbkg));

  m_ftree = 0;
  m_fname = fname;
}

void FittingProcess::addHistogram(int crystal, TH1F *hist)
{
  m_crystal.push_back(crystal);
  m_hist.push_back(new TH1F(*hist));
  TString name = TString("dh") + Long_t(crystal);
  m_dh.push_back(new RooDataHist(name, name, *en, Import(*hist)));
}

int FittingProcess::run(bool signal_only)
{
  TFile* file = new TFile(m_fname.c_str(), "RECREATE");
  m_ftree = new FittingTree();

  for (unsigned int i = 0; i < m_crystal.size(); i++) {
    std::cout << "==============================================" << std::endl;
    std::cout << "= CRYSTAL " << m_crystal[i] << std::endl;
    std::cout << "==============================================" << std::endl;

    fitHist(m_crystal[i], m_hist[i], signal_only, *(m_dh[i]));
    m_ftree->setDouble("mean", m_hist[i]->GetMean());
    m_ftree->setDouble("dev",  m_hist[i]->GetStdDev());
    m_ftree->setDouble("asym", m_hist[i]->GetSkewness());
    m_ftree->Fill();

    std::cout << "=============================================="
              << std::endl << std::endl;
  }

  m_ftree->Write();

  file->Close();

  return 0;
}

void FittingProcess::setOutputFile(const char *fname)
{
  m_fname = fname;
}

void FittingProcess::setIterCount(int iter_count)
{
  this->iter_count = iter_count;
}

FittingTree* FittingProcess::getResult()
{
  return m_ftree;
}
