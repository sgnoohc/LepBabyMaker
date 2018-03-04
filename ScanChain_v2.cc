#include "ScanChain_v2.h"

using namespace std;

//##############################################################################################################
void babyMaker_v2::ScanChain_v2(TChain* chain, std::string baby_name, int max_events, int index, bool verbose)
{

    // Looper
    RooUtil::Looper<CMS3> looper(chain, &cms3, max_events);

    // Output root file
    CreateOutput(index);

    try
    {
        while (looper.nextEvent())
        {
            if (verbose)
                cout << "[verbose] Processed " << looper.getNEventsProcessed() << " out of " << chain->GetEntries() << endl;
            
            coreJec.setJECFor(looper.getCurrentFileName());

            // Loop over electrons
            ProcessElectrons();

            // Loop over muons
            ProcessMuons();

            // Check preselection
            if (!PassPresel())
                continue;

            // Triggers
            ProcessTriggers();

            // Loop over gen particles
            ProcessGenParticles();

            // Loop over Jets
            ProcessJets();

            // Process MET (recalculate etc.)
            ProcessMET();

            // Loop over charged particle candidates
            ProcessTracks();

            // Fill baby ntuple
            FillOutput();
        }
    }
    catch (const std::ios_base::failure& e)
    {
        cout << endl;
        cout << "[CheckCorrupt] Caught an I/O failure in the ROOT file." << endl;
        cout << "[CheckCorrupt] Possibly corrupted hadoop file." << endl;
        cout << "[CheckCorrupt] Processed " << looper.getNEventsProcessed() << " out of " << chain->GetEntries() << endl;
        cout << e.what() << endl;
    }

    looper.getTree()->PrintCacheStats();

    ofile->cd();
    t->Write();
}

//##############################################################################################################
void babyMaker_v2::CreateOutput(int index)
{
    ofile = new TFile(Form("output_%d.root", index), "recreate");
    t = new TTree("t", "t");
    tx = new RooUtil::TTreeX(t);

    tx->clear();
}

//##############################################################################################################
void babyMaker_v2::ProcessTriggers() { coreTrigger.process(); }

//##############################################################################################################
void babyMaker_v2::ProcessGenParticles() { coreGenPart.process(); }

//##############################################################################################################
void babyMaker_v2::ProcessElectrons() { coreElectron.process(isVetoElectron); }

//##############################################################################################################
void babyMaker_v2::ProcessMuons() { coreMuon.process(isVetoMuon); }

//##############################################################################################################
void babyMaker_v2::ProcessJets() { coreJet.process(coreJec); }

//##############################################################################################################
void babyMaker_v2::ProcessMET() { coreMET.process(coreJec); }

//##############################################################################################################
void babyMaker_v2::ProcessTracks() { coreTrack.process(); }

//##############################################################################################################
bool babyMaker_v2::PassPresel()
{
    // Place your preselection
}

//##############################################################################################################
void babyMaker_v2::FillOutput()
{
    // Fill TTree (NOTE: also clears internal variables)
    FillTTree();
}

//##############################################################################################################
void babyMaker_v2::FillEventInfo()
{
}

//##############################################################################################################
void babyMaker_v2::FillElectrons()
{
    for (auto& idx : coreElectron.index)
    {
    }
}

//##############################################################################################################
void babyMaker_v2::FillMuons()
{
    for (auto& idx : coreMuon.index)
    {
    }
}

//##############################################################################################################
void babyMaker_v2::FillGenParticles()
{
}

//##############################################################################################################
void babyMaker_v2::SortLeptonBranches()
{
//    tx->sortVecBranchesByPt("lep_p4",
//            {
//            },
//            {
//            },
//            {});
}

//##############################################################################################################
void babyMaker_v2::FillJets()
{
}

//##############################################################################################################
void babyMaker_v2::FillMET()
{
}

//##############################################################################################################
void babyMaker_v2::FillTracks()
{
}

//##############################################################################################################
void babyMaker_v2::SortJetBranches()
{
//    tx->sortVecBranchesByPt("jets_p4", {"jets_csv"}, {}, {});
}

//##############################################################################################################
void babyMaker_v2::FillTrigger()
{
}

//##############################################################################################################
void babyMaker_v2::FillVertexInfo()
{
}

//##############################################################################################################
void babyMaker_v2::FillMETFilter()
{
}

//##############################################################################################################
void babyMaker_v2::FillSummaryVariables()
{
}

//##############################################################################################################
void babyMaker_v2::FillTTree()
{
    tx->fill();
    tx->clear();
}

//##############################################################################################################
bool babyMaker_v2::isLeptonOverlappingWithJet(int ijet)
{
    bool is_overlapping = false;

    int idx = coreJet.index[ijet];

    for (auto& imu : coreMuon.index)
    {
        if (!(isLooseMuon(imu)))
            continue;

        if (ROOT::Math::VectorUtil::DeltaR(cms3.pfjets_p4()[idx], cms3.mus_p4()[imu]) < 0.4)
        {
            is_overlapping = true;
            break;
        }
    }

    if (is_overlapping)
        return true;

    for (auto& iel : coreElectron.index)
    {
        if (!(isLooseElectron(iel)))
            continue;

        if (ROOT::Math::VectorUtil::DeltaR(cms3.pfjets_p4()[idx], cms3.els_p4()[iel]) < 0.4)
        {
            is_overlapping = true;
            break;
        }
    }

    if (is_overlapping)
        return true;

    return false;
}

//##############################################################################################################
bool babyMaker_v2::isLeptonOverlappingWithTrack(int idx)
{
    bool is_overlapping = false;

    for (auto& imu : coreMuon.index)
    {
        if (!(isVetoMuon(imu)))
            continue;

        if (ROOT::Math::VectorUtil::DeltaR(cms3.pfcands_p4()[idx], cms3.mus_p4()[imu]) < 0.01)
        {
            is_overlapping = true;
            break;
        }
    }

    if (is_overlapping)
        return true;

    for (auto& iel : coreElectron.index)
    {
        if (!(isVetoElectron(iel)))
            continue;

        if (ROOT::Math::VectorUtil::DeltaR(cms3.pfcands_p4()[idx], cms3.els_p4()[iel]) < 0.01)
        {
            is_overlapping = true;
            break;
        }
    }

    if (is_overlapping)
        return true;

    return false;
}

//##############################################################################################################
// Used to overlap remova against jets
bool babyMaker_v2::isLooseMuon(int idx)
{
    if (!( cms3.mus_p4()[idx].pt() > 20.                     )) return false;
    if (!( passMuonSelection_VVV(idx, VVV_cutbased_3l_fo_v2) )) return false;
    return true;
}

//##############################################################################################################
// Used to overlap remova against jets
bool babyMaker_v2::isLooseElectron(int idx)
{
    if (!( cms3.els_p4()[idx].pt() > 20.                         )) return false;
    if (!( passElectronSelection_VVV(idx, VVV_cutbased_3l_fo_v2) )) return false;
    return true;
}

//##############################################################################################################
// Used to overlap remova against tracks
bool babyMaker_v2::isVetoMuon(int idx)
{
    if (!( passMuonSelection_VVV(idx, VVV_cutbased_veto_v2) )) return false;
    return true;
}

//##############################################################################################################
// Used to overlap remova against tracks
bool babyMaker_v2::isVetoElectron(int idx)
{
    if (!( passElectronSelection_VVV(idx, VVV_cutbased_veto_v2) )) return false;
    return true;
}

//##############################################################################################################
int babyMaker_v2::passCount(const vector<int>& v)
{
    return std::count_if(v.begin(), v.end(), [](int i){return i > 0;});
}

//##############################################################################################################
void babyMaker_v2::FillJetVariables(int variation)
{
}

//##############################################################################################################
void babyMaker_v2::FillLeptonVariables()
{
}

//##############################################################################################################
void babyMaker_v2::FillSSLeptonVariables(int idx0, int idx1)
{
}

//##############################################################################################################
void babyMaker_v2::Fill3LLeptonVariables()
{
}

//eof
