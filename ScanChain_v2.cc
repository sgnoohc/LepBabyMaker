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

    tx->createBranch<float>("evt_pfmet"); 
    tx->createBranch<float>("evt_pfmetPhi"); 
    tx->createBranch<float>("evt_trackmet"); 
    tx->createBranch<float>("evt_trackmetPhi"); 
    tx->createBranch<float>("evt_corrMET"); 
    tx->createBranch<float>("evt_corrMETPhi"); 
    tx->createBranch<float>("evt_corrMET_up"); 
    tx->createBranch<float>("evt_corrMETPhi_up"); 
    tx->createBranch<float>("evt_corrMET_dn"); 
    tx->createBranch<float>("evt_corrMETPhi_dn"); 
    tx->createBranch<float>("evt_pfsumet"); 
    tx->createBranch<float>("evt_pfmetSig"); 
    tx->createBranch<unsigned long long>("evt_event"); 
    tx->createBranch<unsigned long long>("evt_lumiBlock"); 
    tx->createBranch<unsigned long long>("evt_run"); 
    tx->createBranch<bool>("filt_csc"); 
    tx->createBranch<bool>("filt_hbhe"); 
    tx->createBranch<bool>("passes_met_filters"); 
    tx->createBranch<bool>("filt_hcallaser"); 
    tx->createBranch<bool>("filt_ecaltp"); 
    tx->createBranch<bool>("filt_trkfail"); 
    tx->createBranch<bool>("filt_eebadsc"); 
    tx->createBranch<bool>("evt_isRealData"); 
    tx->createBranch<float>("scale1fb"); 
    tx->createBranch<float>("evt_xsec_incl"); 
    tx->createBranch<float>("evt_kfactor"); 
    tx->createBranch<float>("gen_met"); 
    tx->createBranch<float>("gen_metPhi"); 
    tx->createBranch<float>("njets"); 
    tx->createBranch<float>("ht"); 
    tx->createBranch<float>("ht_SS"); 
    tx->createBranch<vector<LV>>("jets"); 
    tx->createBranch<vector<float>>("jets_disc"); 
    tx->createBranch<vector<float>>("jets_area"); 
    tx->createBranch<vector<float>>("jets_undoJEC"); 
    tx->createBranch<vector<float>>("jets_unc"); 
    tx->createBranch<TString>("sample"); 
    tx->createBranch<int>("nFOs_SS"); 
    tx->createBranch<int>("nvtx"); 
    tx->createBranch<int>("pu_ntrue"); 
    tx->createBranch<int>("instantLumi"); 
    tx->createBranch<float>("rho"); 
    tx->createBranch<float>("rho_neut_centr"); 
    tx->createBranch<float>("rho_calo"); 
    tx->createBranch<float>("rho_calo_centr"); 
    tx->createBranch<LorentzVector>("p4"); 
    tx->createBranch<LorentzVector>("tag_p4"); 
    tx->createBranch<LorentzVector>("dilep_p4"); 
    tx->createBranch<LorentzVector>("mc_p4"); 
    tx->createBranch<LorentzVector>("mc_motherp4"); 
    tx->createBranch<int>("mc_motherid"); 
    tx->createBranch<int>("id"); 
    tx->createBranch<int>("isPF"); 
    tx->createBranch<int>("idx"); 
    tx->createBranch<float>("dxyPV"); 
    tx->createBranch<float>("dZ"); 
    tx->createBranch<float>("dxyPV_err"); 
    tx->createBranch<int>("motherID"); 
    tx->createBranch<int>("mc_id"); 
    tx->createBranch<float>("RelIso03"); 
    tx->createBranch<float>("RelIso03EA"); 
    tx->createBranch<float>("RelIso03EAv2"); 
    tx->createBranch<float>("RelIso04EAv2"); 
    tx->createBranch<float>("tag_RelIso03EA"); 
    tx->createBranch<float>("RelIso03DB"); 
    tx->createBranch<float>("pfChargedHadronIso"); 
    tx->createBranch<float>("pfPhotonIso"); 
    tx->createBranch<float>("pfNeutralHadronIso"); 
    tx->createBranch<float>("tkIso"); 
    tx->createBranch<float>("sumPUPt"); 
    tx->createBranch<bool>("passes_SS_tight_v3"); 
    tx->createBranch<bool>("passes_SS_tight_noiso_v3"); 
    tx->createBranch<bool>("passes_SS_fo_v3"); 
    tx->createBranch<bool>("passes_SS_fo_noiso_v3"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_v3"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_noiso_v3"); 
    tx->createBranch<bool>("passes_SS_veto_v3"); 
    tx->createBranch<bool>("passes_SS_veto_noiso_v3"); 
    tx->createBranch<bool>("passes_SS_tight_v4"); 
    tx->createBranch<bool>("passes_SS_tight_noiso_v4"); 
    tx->createBranch<bool>("passes_SS_fo_v4"); 
    tx->createBranch<bool>("passes_SS_fo_noiso_v4"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_v4"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_noiso_v4"); 
    tx->createBranch<bool>("passes_SS_veto_v4"); 
    tx->createBranch<bool>("passes_SS_veto_noiso_v4"); 
    tx->createBranch<bool>("passes_SS_tight_v5"); 
    tx->createBranch<bool>("passes_SS_tight_noiso_v5"); 
    tx->createBranch<bool>("passes_SS_fo_v5"); 
    tx->createBranch<bool>("passes_SS_fo_noiso_v5"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_v5"); 
    tx->createBranch<bool>("passes_SS_fo_looseMVA_noiso_v5"); 
    tx->createBranch<bool>("passes_SS_veto_v5"); 
    tx->createBranch<bool>("passes_SS_veto_noiso_v5"); 
    tx->createBranch<bool>("passes_WW_medium_v2"); 
    tx->createBranch<bool>("passes_WW_medium_noiso_v2"); 
    tx->createBranch<bool>("passes_WW_fo_v2"); 
    tx->createBranch<bool>("passes_WW_fo_noiso_v2"); 
    tx->createBranch<bool>("passes_WW_veto_v2"); 
    tx->createBranch<bool>("passes_WW_veto_noiso_v2"); 
    tx->createBranch<bool>("passes_HAD_veto_v3"); 
    tx->createBranch<bool>("passes_HAD_veto_noiso_v3"); 
    tx->createBranch<bool>("passes_HAD_loose_v3"); 
    tx->createBranch<bool>("passes_HAD_loose_noiso_v3"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto_bak1"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto_noiso_bak1"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto_noiso_noip_bak1"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto_noiso"); 
    tx->createBranch<bool>("passes_VVV_cutbased_veto_noiso_noip"); 
    tx->createBranch<bool>("passes_VVV_cutbased_fo"); 
    tx->createBranch<bool>("passes_VVV_cutbased_fo_noiso"); 
    tx->createBranch<bool>("passes_VVV_baseline"); 
    tx->createBranch<bool>("passes_VVV_MVAbased_tight"); 
    tx->createBranch<bool>("passes_VVV_MVAbased_tight_noiso"); 
    tx->createBranch<bool>("passes_VVV_cutbased_tight"); 
    tx->createBranch<bool>("passes_VVV_cutbased_tight_noiso"); 
    tx->createBranch<bool>("passes_POG_vetoID"); 
    tx->createBranch<bool>("passes_POG_looseID"); 
    tx->createBranch<bool>("passes_POG_mediumID"); 
    tx->createBranch<bool>("passes_POG_tightID"); 
    tx->createBranch<bool>("passes_isTriggerSafe_v1"); 
    tx->createBranch<bool>("passes_isTriggerSafe_v2"); 
    tx->createBranch<float>("ip3d"); 
    tx->createBranch<float>("ip3derr"); 
    tx->createBranch<int>("type"); 
    tx->createBranch<float>("mt"); 
    tx->createBranch<float>("ptrelv0"); 
    tx->createBranch<float>("ptrelv1"); 
    tx->createBranch<float>("miniiso"); 
    tx->createBranch<float>("miniisoDB"); 
    tx->createBranch<float>("reliso04"); 
    tx->createBranch<float>("annulus04"); 
    tx->createBranch<float>("AbsTrkIso"); 
    tx->createBranch<float>("TrkAn04"); 
    tx->createBranch<float>("iso03sumPt"); 
    tx->createBranch<float>("iso03emEt"); 
    tx->createBranch<float>("iso03hadEt"); 
    tx->createBranch<int>("jet_close_lep_idx"); 
    tx->createBranch<LorentzVector>("jet_close_lep"); 
    tx->createBranch<float>("jet_close_lep_undoJEC"); 
    tx->createBranch<float>("jet_close_lep_area"); 
    tx->createBranch<float>("jet_close_L1"); 
    tx->createBranch<float>("jet_close_L1nc"); 
    tx->createBranch<float>("jet_close_L1ncmc"); 
    tx->createBranch<float>("jet_close_L1L2L3"); 
    tx->createBranch<float>("jet_close_L2L3"); 
    tx->createBranch<float>("ptratio"); 
    tx->createBranch<int>("tag_charge"); 
    tx->createBranch<int>("tag_mc_motherid"); 
    tx->createBranch<float>("tag_eSeed"); 
    tx->createBranch<float>("tag_eSCRaw"); 
    tx->createBranch<float>("tag_eSC"); 
    tx->createBranch<float>("tag_ecalEnergy"); 
    tx->createBranch<int>("exp_innerlayers"); 
    tx->createBranch<int>("exp_outerlayers"); 
    tx->createBranch<int>("tag_HLT_Ele27_eta2p1_WPTight_Gsf"); 
    tx->createBranch<int>("tag_HLT_Ele32_eta2p1_WPTight_Gsf"); 
    tx->createBranch<int>("tag_HLT_IsoMu24"); 
    tx->createBranch<int>("tag_HLT_IsoTkMu24"); 
    tx->createBranch<float>("dilep_mass"); 
    tx->createBranch<bool>("isRandom"); 
    tx->createBranch<float>("sigmaIEtaIEta_full5x5"); 
    tx->createBranch<float>("sigmaIEtaIEta"); 
    tx->createBranch<float>("etaSC"); 
    tx->createBranch<float>("dEtaIn"); 
    tx->createBranch<float>("dPhiIn"); 
    tx->createBranch<float>("hOverE"); 
    tx->createBranch<float>("eSeed"); 
    tx->createBranch<float>("scSeedEta"); 
    tx->createBranch<float>("ecalEnergy"); 
    tx->createBranch<float>("eOverPIn"); 
    tx->createBranch<bool>("conv_vtx_flag"); 
    tx->createBranch<int>("charge"); 
    tx->createBranch<int>("sccharge"); 
    tx->createBranch<int>("ckf_charge"); 
    tx->createBranch<bool>("threeChargeAgree_branch"); 
    tx->createBranch<float>("mva"); 
    tx->createBranch<float>("mva_25ns"); 
    tx->createBranch<float>("tag_mva_25ns"); 
    tx->createBranch<float>("ecalIso"); 
    tx->createBranch<float>("hcalIso"); 
    tx->createBranch<float>("ecalPFClusterIso"); 
    tx->createBranch<float>("hcalPFClusterIso"); 
    tx->createBranch<int>("ckf_laywithmeas"); 
    tx->createBranch<float>("sigmaIPhiIPhi_full5x5"); 
    tx->createBranch<float>("e1x5_full5x5"); 
    tx->createBranch<float>("e5x5_full5x5"); 
    tx->createBranch<float>("r9_full5x5"); 
    tx->createBranch<float>("tag_r9_full5x5"); 
    tx->createBranch<float>("etaSCwidth"); 
    tx->createBranch<float>("phiSCwidth"); 
    tx->createBranch<float>("eSCRaw"); 
    tx->createBranch<float>("eSC"); 
    tx->createBranch<float>("eSCPresh"); 
    tx->createBranch<float>("ckf_chi2"); 
    tx->createBranch<int>("ckf_ndof"); 
    tx->createBranch<float>("chi2"); 
    tx->createBranch<int>("ndof"); 
    tx->createBranch<float>("fbrem"); 
    tx->createBranch<float>("eOverPOut"); 
    tx->createBranch<float>("dEtaOut"); 
    tx->createBranch<float>("dPhiOut"); 
    tx->createBranch<int>("gsf_validHits"); 
    tx->createBranch<float>("conv_vtx_prob"); 
    tx->createBranch<int>("pid_PFMuon"); 
    tx->createBranch<float>("gfit_chi2"); 
    tx->createBranch<int>("gfit_validSTAHits"); 
    tx->createBranch<int>("numberOfMatchedStations"); 
    tx->createBranch<int>("validPixelHits"); 
    tx->createBranch<int>("nlayers"); 
    tx->createBranch<float>("chi2LocalPosition"); 
    tx->createBranch<float>("trkKink"); 
    tx->createBranch<int>("validHits"); 
    tx->createBranch<int>("lostHits"); 
    tx->createBranch<float>("segmCompatibility"); 
    tx->createBranch<float>("ptErr"); 
    tx->createBranch<float>("trk_pt"); 
    tx->createBranch<int>("HLT_Mu8"); 
    tx->createBranch<int>("HLT_Mu17"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_IsoMu24"); 
    tx->createBranch<int>("HLT_IsoTkMu24"); 
    tx->createBranch<int>("HLT_Mu50"); 
    tx->createBranch<int>("HLT_Mu55"); 
    tx->createBranch<int>("HLT_TkMu50"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Ele8_CaloIdM_TrackIdM_PFJet30"); 
    tx->createBranch<int>("HLT_Ele12_CaloIdM_TrackIdM_PFJet30"); 
    tx->createBranch<int>("HLT_Ele17_CaloIdM_TrackIdM_PFJet30"); 
    tx->createBranch<int>("HLT_Ele23_CaloIdM_TrackIdM_PFJet30"); 
    tx->createBranch<int>("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30"); 
    tx->createBranch<int>("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"); 
    tx->createBranch<int>("HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30"); 
    tx->createBranch<int>("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30"); 
    tx->createBranch<int>("HLT_Ele27_eta2p1_WPTight_Gsf"); 
    tx->createBranch<int>("HLT_Ele32_eta2p1_WPTight_Gsf"); 
    tx->createBranch<int>("HLT_Ele105_CaloIdVT_GsfTrkIdT"); 
    tx->createBranch<int>("HLT_Ele115_CaloIdVT_GsfTrkIdT"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_L1"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_L1"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 

    tx->clear();
}

//##############################################################################################################
void babyMaker_v2::ProcessTriggers() { coreTrigger.process(); }

//##############################################################################################################
void babyMaker_v2::ProcessGenParticles() { coreGenPart.process(); }

//##############################################################################################################
void babyMaker_v2::ProcessElectrons() { coreElectron.process(isPreselElectron, checkElectronTag); }

//##############################################################################################################
void babyMaker_v2::ProcessMuons() { coreMuon.process(isPreselMuon, checkMuonTag); }

//##############################################################################################################
void babyMaker_v2::ProcessJets() { coreJet.process(coreJec); }

//##############################################################################################################
void babyMaker_v2::ProcessMET() { coreMET.process(coreJec); }

//##############################################################################################################
void babyMaker_v2::ProcessTracks() { coreTrack.process(); }

//##############################################################################################################
bool babyMaker_v2::PassPresel()
{
    // Why? I don't know
    if (cms3.evt_isRealData() && looper.getCurrentFileName().Contains("PromptReco") && cms3.evt_run() <= 251562)
        return false;

    if (cms3.evt_isRealData() && !goodrun(cms3.evt_run(), cms3.evt_lumiBlock()))
        return false;

    // Place your preselection
    return true;
}

//##############################################################################################################
void babyMaker_v2::FillOutput()
{
    // Fill Electrons
    FillElectrons();

    // Fill Muons
    FillMuons();
}

//##############################################################################################################
void babyMaker_v2::FillEventInfo()
{
}

//##############################################################################################################
void babyMaker_v2::FillElectrons()
{
    for (unsigned int i = 0; i < coreElectron.index.size(); ++i)
    {
        // Check whether I found a tag
        int tag_idx = coreElectron.tagindex[i];

        if (tag_idx < 0)
            continue;

        int idx = coreElectron.index[i];

        // Get the tag p4
        LV dilep_p4 = cms3.els_p4()[idx] + cms3.els_p4()[tag_idx];
        float dilep_mass = dilep_p4.mass();

        // Skip if off the Z peak
        if (!( dilep_mass > 60 && dilep_mass < 120 ))
            continue;

        int HLT_Ele8_CaloIdM_TrackIdM_PFJet30;
        int HLT_Ele12_CaloIdM_TrackIdM_PFJet30;
        int HLT_Ele17_CaloIdM_TrackIdM_PFJet30;
        int HLT_Ele23_CaloIdM_TrackIdM_PFJet30;
        int HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;
        int HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;
        int HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30;
        int HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;
        int HLT_Ele27_eta2p1_WPTight_Gsf;
        int HLT_Ele32_eta2p1_WPTight_Gsf;
        int HLT_Ele105_CaloIdVT_GsfTrkIdT;
        int HLT_Ele115_CaloIdVT_GsfTrkIdT;
        int HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL;

        //Single Electron Trigger with Jet
        setHLTBranch("HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v"        , cms3.els_HLT_Ele8_CaloIdM_TrackIdM_PFJet30_ElectronLeg()[idx]        , HLT_Ele8_CaloIdM_TrackIdM_PFJet30);
        setHLTBranch("HLT_Ele12_CaloIdM_TrackIdM_PFJet30_v"       , cms3.els_HLT_Ele12_CaloIdM_TrackIdM_PFJet30_ElectronLeg()[idx]       , HLT_Ele12_CaloIdM_TrackIdM_PFJet30);
        setHLTBranch("HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v"       , cms3.els_HLT_Ele17_CaloIdM_TrackIdM_PFJet30_ElectronLeg()[idx]       , HLT_Ele17_CaloIdM_TrackIdM_PFJet30);
        setHLTBranch("HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v"       , cms3.els_HLT_Ele23_CaloIdM_TrackIdM_PFJet30_ElectronLeg()[idx]       , HLT_Ele23_CaloIdM_TrackIdM_PFJet30);
        setHLTBranch("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v"  , cms3.els_HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_ElectronLeg()[idx]  , HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30);
        setHLTBranch("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v" , cms3.els_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_ElectronLeg()[idx] , HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30);
        setHLTBranch("HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v" , cms3.els_HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_ElectronLeg()[idx] , HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30);
        setHLTBranch("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v" , cms3.els_HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_ElectronLeg()[idx] , HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30);
        //Single Electron Trigger
        setHLTBranch("HLT_Ele27_eta2p1_WPTight_Gsf_v"             , cms3.els_HLT_Ele27_eta2p1_WPTight_Gsf()[idx]                         , HLT_Ele27_eta2p1_WPTight_Gsf);
        setHLTBranch("HLT_Ele32_eta2p1_WPTight_Gsf_v"             , cms3.els_HLT_Ele32_eta2p1_WPTight_Gsf()[idx]                         , HLT_Ele32_eta2p1_WPTight_Gsf);
        setHLTBranch("HLT_Ele105_CaloIdVT_GsfTrkIdT_v"            , cms3.els_HLT_Ele105_CaloIdVT_GsfTrkIdT()[idx]                        , HLT_Ele105_CaloIdVT_GsfTrkIdT);
        setHLTBranch("HLT_Ele115_CaloIdVT_GsfTrkIdT_v"            , cms3.els_HLT_Ele115_CaloIdVT_GsfTrkIdT()[idx]                        , HLT_Ele115_CaloIdVT_GsfTrkIdT);
        setHLTBranch("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_v"   , cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL()[idx]               , HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL);

//        int HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
//        int HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
//        setHLTBranch("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v", cms3.els_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ()[idx], HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
//        setHLTBranch("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v", cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ()[idx], HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);

        // Set the branch of the tag electrons single lepton trigger bit
        int tag_HLT_Ele27_eta2p1_WPTight_Gsf;
        int tag_HLT_Ele32_eta2p1_WPTight_Gsf;
        setHLTBranch("HLT_Ele27_eta2p1_WPTight_Gsf_v", (tag_idx >= 0 ? cms3.els_HLT_Ele27_eta2p1_WPTight_Gsf().at(tag_idx) : 0), tag_HLT_Ele27_eta2p1_WPTight_Gsf);
        setHLTBranch("HLT_Ele32_eta2p1_WPTight_Gsf_v", (tag_idx >= 0 ? cms3.els_HLT_Ele32_eta2p1_WPTight_Gsf().at(tag_idx) : 0), tag_HLT_Ele32_eta2p1_WPTight_Gsf);

        // Set the TTree branches
        tx->setBranch<bool>("evt_isRealData", cms3.evt_isRealData());
        if (cms3.evt_isRealData())
            tx->setBranch<float>("evt_scale1fb", 1);
        else
            tx->setBranch<float>("evt_scale1fb", coreDatasetInfo.getScale1fb());
        tx->setBranch<unsigned long long>("evt_event", cms3.evt_event());
        tx->setBranch<unsigned long long>("evt_lumiBlock", cms3.evt_lumiBlock());
        tx->setBranch<unsigned long long>("evt_run", cms3.evt_run());
        tx->setBranch<LV>("p4", cms3.els_p4()[idx]);
        tx->setBranch<LV>("tag_p4", cms3.els_p4()[tag_idx]);
        tx->setBranch<LV>("dilep_p4", dilep_p4);
        tx->setBranch<float>("dilep_mass", dilep_mass);
        tx->setBranch<int>("id", -11.0 * cms3.els_charge()[idx]);
        tx->setBranch<float>("dxyPV", cms3.els_dxyPV()[idx]);
        tx->setBranch<float>("dZ", cms3.els_dzPV()[idx]);
        tx->setBranch<float>("ip3d", cms3.els_ip3d()[idx]);
        tx->setBranch<float>("ip3derr", cms3.els_ip3derr()[idx]);
        tx->setBranch<bool>("passes_VVV_cutbased_tight", electronID(idx, VVV_cutbased_tight));
        tx->setBranch<int>("HLT_Ele8_CaloIdM_TrackIdM_PFJet30" , HLT_Ele8_CaloIdM_TrackIdM_PFJet30);
        tx->setBranch<int>("HLT_Ele12_CaloIdM_TrackIdM_PFJet30" , HLT_Ele12_CaloIdM_TrackIdM_PFJet30);
        tx->setBranch<int>("HLT_Ele17_CaloIdM_TrackIdM_PFJet30" , HLT_Ele17_CaloIdM_TrackIdM_PFJet30);
        tx->setBranch<int>("HLT_Ele23_CaloIdM_TrackIdM_PFJet30" , HLT_Ele23_CaloIdM_TrackIdM_PFJet30);
        tx->setBranch<int>("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30" , HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30);
        tx->setBranch<int>("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30" , HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30);
        tx->setBranch<int>("HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30" , HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30);
        tx->setBranch<int>("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30" , HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30);
        tx->setBranch<int>("HLT_Ele27_eta2p1_WPTight_Gsf" , HLT_Ele27_eta2p1_WPTight_Gsf);
        tx->setBranch<int>("HLT_Ele32_eta2p1_WPTight_Gsf" , HLT_Ele32_eta2p1_WPTight_Gsf);
        tx->setBranch<int>("HLT_Ele105_CaloIdVT_GsfTrkIdT" , HLT_Ele105_CaloIdVT_GsfTrkIdT);
        tx->setBranch<int>("HLT_Ele115_CaloIdVT_GsfTrkIdT" , HLT_Ele115_CaloIdVT_GsfTrkIdT);
        tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL" , HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL);
        tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg" , cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg" , cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg()[idx]);
        tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg" , cms3.els_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg" , cms3.els_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg()[idx]);

        tx->setBranch<float>("sigmaIEtaIEta_full5x5", cms3.els_sigmaIEtaIEta_full5x5()[idx]);
        tx->setBranch<float>("etaSC", cms3.els_etaSC()[idx]);
        tx->setBranch<float>("dEtaIn", cms3.els_dEtaIn()[idx]);
        tx->setBranch<float>("dPhiIn", cms3.els_dPhiIn()[idx]);
        tx->setBranch<float>("hOverE", cms3.els_hOverE()[idx]);
        tx->setBranch<float>("ecalEnergy", cms3.els_ecalEnergy()[idx]);
        tx->setBranch<float>("eOverPIn", cms3.els_eOverPIn()[idx]);
        tx->setBranch<float>("ecalPFClusterIso", cms3.els_ecalPFClusterIso()[idx]);
        tx->setBranch<float>("hcalPFClusterIso", cms3.els_hcalPFClusterIso()[idx]);
        tx->setBranch<float>("tkIso", cms3.els_tkIso()[idx]);

//        conv_vtx_flag = cms3.els_conv_vtx_flag()[idx];
//        exp_innerlayers = cms3.els_exp_innerlayers()[idx];
//        exp_outerlayers = cms3.els_exp_outerlayers()[idx];
//        charge = cms3.els_charge()[idx];
//        sccharge = cms3.els_sccharge()[idx];
//        ckf_charge = cms3.els_ckf_charge()[idx];
//        trk_charge = cms3.els_trk_charge()[idx];
//        threeChargeAgree_branch = threeChargeAgree(i);
//        mva = getMVAoutput(i);
//        mva_25ns = v25nsMVAreader->MVA(i);
//        type = cms3.els_type()[idx];
//        ecalIso = cms3.els_ecalIso()[idx];
//        hcalIso = cms3.els_hcalIso()[idx];
//        sigmaIEtaIEta = cms3.els_sigmaIEtaIEta()[idx];
//        ckf_laywithmeas = cms3.els_ckf_laywithmeas()[idx];
//        sigmaIPhiIPhi_full5x5 = cms3.els_sigmaIPhiIPhi_full5x5()[idx];
//        e1x5_full5x5 = cms3.els_e1x5_full5x5()[idx];
//        e5x5_full5x5 = cms3.els_e5x5_full5x5()[idx];
//        r9_full5x5 = cms3.els_r9_full5x5()[idx];
//        etaSCwidth = cms3.els_etaSCwidth()[idx];
//        phiSCwidth = cms3.els_phiSCwidth()[idx];
//        eSeed = cms3.els_eSeed()[idx];
//        scSeedEta = cms3.els_scSeedEta()[idx];
//        eSCRaw = cms3.els_eSCRaw()[idx];
//        eSC = cms3.els_eSC()[idx];
//        eSCPresh = cms3.els_eSCPresh()[idx];
//        ckf_chi2 = cms3.els_ckf_chi2()[idx];
//        ckf_ndof = cms3.els_ckf_ndof()[idx];
//        chi2 = cms3.els_chi2()[idx];
//        ndof = cms3.els_ndof()[idx];
//        fbrem = cms3.els_fbrem()[idx];
//        eOverPOut = cms3.els_eOverPOut()[idx];
//        dEtaOut = cms3.els_dEtaOut()[idx];
//        dPhiOut = cms3.els_dPhiOut()[idx];
//        gsf_validHits = cms3.els_validHits()[idx];
//        conv_vtx_prob = cms3.els_conv_vtx_prob()[idx];

        FillTTree();

    }
}

//##############################################################################################################
void babyMaker_v2::FillMuons()
{
    for (unsigned int i = 0; i < coreMuon.index.size(); ++i)
    {
        // Check whether I found a tag
        int tag_idx = coreMuon.tagindex[i];

        if (tag_idx < 0)
            continue;

        int idx = coreMuon.index[i];

        // Get the tag p4
        LV dilep_p4 = cms3.mus_p4()[idx] + cms3.mus_p4()[tag_idx];
        float dilep_mass = dilep_p4.mass();

        // Skip if off the Z peak
        if (!( dilep_mass > 60 && dilep_mass < 120 ))
            continue;

        //Single Muon Triggers
        int HLT_Mu8;
        int HLT_Mu17;
        int HLT_Mu8_TrkIsoVVL;
        int HLT_Mu17_TrkIsoVVL;
        int HLT_IsoMu24;
        int HLT_IsoTkMu24;
        int HLT_Mu50;
        int HLT_Mu55;
        int HLT_TkMu50;
        int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
        int HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
        int HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
        int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
        int HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL;
        setHLTBranch("HLT_Mu8_v"                                 , cms3.mus_HLT_Mu8()[idx]                                 , HLT_Mu8                                 ); 
        setHLTBranch("HLT_Mu17_v"                                , cms3.mus_HLT_Mu17()[idx]                                , HLT_Mu17                                ); 
        setHLTBranch("HLT_Mu8_TrkIsoVVL_v"                       , cms3.mus_HLT_Mu8_TrkIsoVVL()[idx]                       , HLT_Mu8_TrkIsoVVL                       ); 
        setHLTBranch("HLT_Mu17_TrkIsoVVL_v"                      , cms3.mus_HLT_Mu17_TrkIsoVVL()[idx]                      , HLT_Mu17_TrkIsoVVL                      ); 
        setHLTBranch("HLT_IsoMu24_v"                             , cms3.mus_HLT_IsoMu24()[idx]                             , HLT_IsoMu24                             ); 
        setHLTBranch("HLT_IsoTkMu24_v"                           , cms3.mus_HLT_IsoTkMu24()[idx]                           , HLT_IsoTkMu24                           ); 
        setHLTBranch("HLT_Mu50_v"                                , cms3.mus_HLT_Mu50()[idx]                                , HLT_Mu50                                ); 
        setHLTBranch("HLT_Mu55_v"                                , cms3.mus_HLT_Mu55()[idx]                                , HLT_Mu55                                ); 
        setHLTBranch("HLT_TkMu50_v"                              , cms3.mus_HLT_TkMu50()[idx]                              , HLT_TkMu50                              ); 
        setHLTBranch("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"     , cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ()[idx]     , HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ     ); 
        setHLTBranch("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v"   , cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ()[idx]   , HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ   ); 
        setHLTBranch("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v" , cms3.mus_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ()[idx] , HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ ); 
        setHLTBranch("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"        , cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL()[idx]        , HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL        ); 
        setHLTBranch("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"      , cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL()[idx]      , HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL      ); 

        // Tag single muon trigger
        int tag_HLT_IsoMu24;
        int tag_HLT_IsoTkMu24;
        setHLTBranch("HLT_IsoMu24_v", (tag_idx >= 0 ? cms3.mus_HLT_IsoMu24().at(tag_idx) : 0), tag_HLT_IsoMu24);
        setHLTBranch("HLT_IsoTkMu24_v", (tag_idx >= 0 ? cms3.mus_HLT_IsoTkMu24().at(tag_idx) : 0), tag_HLT_IsoTkMu24);

        // Set the TTree branches
        tx->setBranch<bool>("evt_isRealData", cms3.evt_isRealData());
        if (cms3.evt_isRealData())
            tx->setBranch<float>("evt_scale1fb", 1);
        else
            tx->setBranch<float>("evt_scale1fb", coreDatasetInfo.getScale1fb());
        tx->setBranch<unsigned long long>("evt_event", cms3.evt_event());
        tx->setBranch<unsigned long long>("evt_lumiBlock", cms3.evt_lumiBlock());
        tx->setBranch<unsigned long long>("evt_run", cms3.evt_run());
        tx->setBranch<LV>("p4", cms3.mus_p4()[idx]);
        tx->setBranch<LV>("tag_p4", cms3.mus_p4()[tag_idx]);
        tx->setBranch<LV>("dilep_p4", dilep_p4);
        tx->setBranch<float>("dilep_mass", dilep_mass);
        tx->setBranch<int>("id", -13.0 * cms3.mus_charge()[idx]);
        tx->setBranch<float>("dxyPV", cms3.mus_dxyPV()[idx]);
        tx->setBranch<float>("dZ", cms3.mus_dzPV()[idx]);
        tx->setBranch<float>("ip3d", cms3.mus_ip3d()[idx]);
        tx->setBranch<float>("ip3derr", cms3.mus_ip3derr()[idx]);
        tx->setBranch<bool>("passes_VVV_cutbased_tight", muonID(idx, VVV_cutbased_tight));
        tx->setBranch<int>("HLT_Mu8", HLT_Mu8);
        tx->setBranch<int>("HLT_Mu17", HLT_Mu17);
        tx->setBranch<int>("HLT_Mu8_TrkIsoVVL", HLT_Mu8_TrkIsoVVL);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL", HLT_Mu17_TrkIsoVVL);
        tx->setBranch<int>("HLT_IsoMu24", HLT_IsoMu24);
        tx->setBranch<int>("HLT_IsoTkMu24", HLT_IsoTkMu24);
        tx->setBranch<int>("HLT_Mu50", HLT_Mu50);
        tx->setBranch<int>("HLT_Mu55", HLT_Mu55);
        tx->setBranch<int>("HLT_TkMu50", HLT_TkMu50);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ", HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ);
        tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ", HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL", HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
        tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg()[idx]);
        tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg()[idx]);
        tx->setBranch<int>("tag_HLT_IsoMu24", tag_HLT_IsoMu24);
        tx->setBranch<int>("tag_HLT_IsoTkMu24", tag_HLT_IsoTkMu24);

        FillTTree();
    }
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
void babyMaker_v2::SortJetBranches()
{
//    tx->sortVecBranchesByPt("jets_p4", {"jets_csv"}, {}, {});
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
bool babyMaker_v2::isPreselMuon(int idx)
{
    if (!( cms3.mus_p4()[idx].pt() >= 10. )) return false;
    return true;
}

//##############################################################################################################
// Used to overlap remova against tracks
bool babyMaker_v2::isPreselElectron(int idx)
{
    if (!( cms3.els_p4()[idx].pt() >= 10.        )) return false;
    return true;
}

//##############################################################################################################
int babyMaker_v2::passCount(const vector<int>& v)
{
    return std::count_if(v.begin(), v.end(), [](int i){return i > 0;});
}

//##############################################################################################################
bool babyMaker_v2::checkMuonTag(int i, int j)
{
    // Tag muon selection
    if (!( cms3.mus_p4()[j].pt()                                    >= 20.0  )) return false;
    if (!( fabs(cms3.mus_p4()[j].eta())                             <=  2.4  )) return false;
    if (!( fabs(cms3.mus_dxyPV()[j])                                <=  0.02 )) return false;
    if (!( fabs(cms3.mus_dzPV()[j])                                 <=  0.05 )) return false;
    if (!( fabs(cms3.mus_ip3d()[j] / cms3.mus_ip3derr()[j])         <=  4    )) return false;
    if (!( isTightMuonPOG(j)                                                 )) return false;
    if (!( muRelIso03EA(j)                                          <=  0.2  )) return false;
    if (!( fabs((cms3.mus_p4()[i] + cms3.mus_p4()[j]).mass() - 90.) <  30.   )) return false;
    return true;
}

//##############################################################################################################
bool babyMaker_v2::checkElectronTag(int i, int j)
{
    if (!( cms3.els_p4()[j].pt()                                    >= 20.0  )) return false;
    if (!( fabs(cms3.els_etaSC()[j])                                <=  2.5  )) return false;
    if (!( cms3.els_passMediumId()[j]                                        )) return false;
    if (!( fabs(cms3.els_ip3d()[j] / cms3.els_ip3derr()[j])         <=  4    )) return false;
    if (!( fabs((cms3.els_p4()[i] + cms3.els_p4()[j]).mass() - 90.) <  30.   )) return false;
    return true;
}

//eof

