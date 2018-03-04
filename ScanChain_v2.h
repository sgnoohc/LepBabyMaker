#ifndef ScanChain_v2_h
#define ScanChain_v2_h

// C++
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

// ROOT
#include "TChain.h"
#include "TDirectory.h"
#include "TTreeCache.h"
#include "Math/VectorUtil.h"
#include "TVector2.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TH2.h"
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH2.h"
#include "TString.h"
#include "TMVA/Reader.h"
#include "Math/LorentzVector.h"
#include "Math/GenVector/LorentzVector.h"

// CORE
#include "../CORE/CMS3.h"
#include "../CORE/Base.h"
#include "../CORE/OSSelections.h"
#include "../CORE/SSSelections.h"
#include "../CORE/VVVSelections.h"
#include "../CORE/ElectronSelections.h"
#include "../CORE/IsolationTools.h"
#include "../CORE/JetSelections.h"
#include "../CORE/MuonSelections.h"
#include "../CORE/IsoTrackVeto.h"
#include "../CORE/PhotonSelections.h"
#include "../CORE/TriggerSelections.h"
#include "../CORE/VertexSelections.h"
#include "../CORE/MCSelections.h"
#include "../CORE/MetSelections.h"
#include "../CORE/SimPa.h"
//#include "../CORE/Tools/jetcorr/FactorizedJetCorrector.h"
//#include "../CORE/Tools/JetCorrector.h"
//#include "../CORE/Tools/jetcorr/JetCorrectionUncertainty.h"
#include "../CORE/Tools/MT2/MT2.h"
#include "../CORE/Tools/hemJet.h"
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/btagsf/BTagCalibrationStandalone.h"
#include "../CORE/Tools/btagsf/BTagCalibrationStandalone.h"
#include "../CORE/Tools/datasetinfo/getDatasetInfo.h"

// RooUtil
#include "rooutil/looper.h"
#include "rooutil/ttreex.h"

// CoreUtil
#include "coreutil/jec.h"
#include "coreutil/btag.h"
#include "coreutil/puwgt.h"
#include "coreutil/genpart.h"
#include "coreutil/trigger.h"
#include "coreutil/electron.h"
#include "coreutil/muon.h"
#include "coreutil/grl.h"
#include "coreutil/datasetinfo.h"
#include "coreutil/jet.h"
#include "coreutil/met.h"
#include "coreutil/track.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

// `````````````````````````````````````````````````````````````````````````````````````````````````````````````
//
//
//
// Baby Maker Class
//
//
//
// ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

class babyMaker_v2
{

private:

    SimPa simpa;
    CoreUtil::jec coreJec;
    CoreUtil::grl coreGRL;
    CoreUtil::btag coreBtagSF;
    CoreUtil::puwgt corePUWgt;
    CoreUtil::trigger coreTrigger;
    CoreUtil::genpart coreGenPart;
    CoreUtil::electron coreElectron;
    CoreUtil::muon coreMuon;
    CoreUtil::datasetinfo coreDatasetInfo;
    CoreUtil::jet coreJet;
    CoreUtil::met coreMET;
    CoreUtil::track coreTrack;

    TFile* ofile;
    TTree* t;
    RooUtil::TTreeX* tx;

    bool isData;

public:

    babyMaker_v2() {}
    ~babyMaker_v2() {}
    void ScanChain_v2(TChain*, std::string = "testSample", int max_events = -1, int index = 1, bool verbose = false);

    void CreateOutput(int index=1);

    void ConfigureGoodRunsList();

    void ProcessTriggers();
    void ProcessGenParticles();
    void ProcessElectrons();
    void ProcessMuons();
    void ProcessJets();
    void ProcessMET();
    void ProcessTracks();

    bool PassPresel();

    void FillOutput();

    void FillEventInfo();
    void FillElectrons();
    void FillMuons();
    void FillJets();
    void FillMET();
    void FillTracks();
    void FillGenParticles();
    void SortLeptonBranches();
    void SortJetBranches();
    void FillTrigger();
    void FillVertexInfo();
    void FillMETFilter();
    void FillSummaryVariables();
    void FillTTree();

    bool isLeptonOverlappingWithJet(int ijet);
    bool isLeptonOverlappingWithJet_OldVersion(int ijet);
    bool isLeptonOverlappingWithTrack(int ijet);
    static bool isLooseMuon(int);
    static bool isLooseElectron(int);
    static bool isVetoMuon(int);
    static bool isVetoElectron(int);
    static bool isVetoMuonNoIso_OldVersion(int);
    static bool isVetoElectronNoIso_OldVersion(int);

    // Calculator
    static int passCount(const vector<int>& vec);
    int nSFOS();
    void FillJetVariables(int variation);
    void FillLeptonVariables();
    void FillSSLeptonVariables(int, int);
    void Fill3LLeptonVariables();

    tuple<bool, int, int> isSSCR();
    float get0SFOSMll();
    float get0SFOSMee();
    float get1SFOSMll();
    float get2SFOSMll0();
    float get2SFOSMll1();
    float calcMTmax(LorentzVector MET, bool compareSSpairs);
    float mT(LV p4, LV met);

};


#endif
