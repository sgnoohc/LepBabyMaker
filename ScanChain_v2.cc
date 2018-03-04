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
    tx->createBranch<int>("evt_event"); 
    tx->createBranch<int>("evt_lumiBlock"); 
    tx->createBranch<int>("evt_run"); 
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
void babyMaker_v2::ProcessElectrons() { coreElectron.process(isPreselElectron); }

//##############################################################################################################
void babyMaker_v2::ProcessMuons() { coreMuon.process(isPreselMuon); }

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
    return true;
}

//##############################################################################################################
void babyMaker_v2::FillOutput()
{
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
    for (auto& idx : coreElectron.index)
    {
    }
}

//##############################################################################################################
void babyMaker_v2::FillMuons()
{
    for (auto& idx : coreMuon.index)
    {
        // It is guaranteed to be found due to preselection
        int tag_idx = checkMuonTag(idx);

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
        setHLTBranch("HLT_IsoMu24_v", (j >= 0 ? tas::mus_HLT_IsoMu24().at(j) : 0), tag_HLT_IsoMu24);
        setHLTBranch("HLT_IsoTkMu24_v", (j >= 0 ? tas::mus_HLT_IsoTkMu24().at(j) : 0), tag_HLT_IsoTkMu24);

        // Set the TTree branches
        tx->setBranch<bool>("evt_isRealData", cms3.evt_isRealData());
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
    if (!( passMuonSelection_VVV(idx, VVV_cutbased_veto_v2) )) return false;
    if (!( checkMuonTag(idx) >= 0                           )) return false;
    return true;
}

//##############################################################################################################
// Used to overlap remova against tracks
bool babyMaker_v2::isPreselElectron(int idx)
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
int babyMaker_v2::checkMuonTag(unsigned int i)
{
    for (unsigned int j = 0; j < tas::mus_p4().size(); j++)
    {
        // Tag muon selection
        if (!( i != j                                                         )) continue; // Tag can't be same as probe
        if (!( cms3.mus_p4().at(j).pt()                               > 20.0  )) continue;
        if (!( fabs(cms3.mus_p4().at(j).eta())                        <  2.4  )) continue;
        if (!( fabs(cms3.mus_dxyPV().at(j))                           <  0.02 )) continue;
        if (!( fabs(cms3.mus_dzPV().at(j))                            <  0.05 )) continue;
        if (!( fabs(cms3.mus_ip3d().at(j) / cms3.mus_ip3derr().at(j)) <  4    )) continue;
        if (!( !isTightMuonPOG(j)                                             )) continue;
        if (!( muRelIso03EA(j)                                        <  0.2  )) continue;

        // Exit with true when tag muon found
        return j;
    }
    return -1;

        // dilep

        // Retrieve tag muon information
//        tag_p4         = cms3.mus_p4().at(j);
//        tag_charge     = cms3.mus_charge().at(j);
//        tag_RelIso03EA = muRelIso03EA(j);

//        // If MC, get the motherID of the tag
//        if (!evt_isRealData)
//        {
//            tag_mc_motherid = cms3.mus_mc_motherid().at(j);
//        }

        // Set the branch of the tag muons single lepton trigger bit
//        setHLTBranch("HLT_IsoMu24_v"  , (j >= 0 ? cms3.mus_HLT_IsoMu24().at(j)   : 0), tag_HLT_IsoMu24  );
//        setHLTBranch("HLT_IsoTkMu24_v", (j >= 0 ? cms3.mus_HLT_IsoTkMu24().at(j) : 0), tag_HLT_IsoTkMu24);

//        // Randomize if needed
//        if (usedMu == false && ((rndm < 0.5 && tag_charge < 0) || (rndm >= 0.5 && tag_charge > 0)))
//        {
//            isRandom = true;
//            usedMu = true;
//        }
//        else
//        {
//            isRandom = false;
//        }

//        // Exit with true when tag muon found
//        return true;
//    }
//    return false;
}

//eof

