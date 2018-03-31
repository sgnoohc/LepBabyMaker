#include "ScanChain_v2.h"

using namespace std;

//##############################################################################################################
void babyMaker_v2::ScanChain_v2(TChain* chain, std::string baby_name, int max_events, int index, bool verbose)
{

    // Looper
    RooUtil::Looper<CMS3> looper(chain, &cms3, max_events);

    // Output root file
    CreateOutput(index);

    while (looper.nextEvent())
    {
        try
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
        catch (const std::ios_base::failure& e)
        {
            tx->clear(); // clear the TTree of any residual stuff
            if (!looper.handleBadEvent())
                break;
        }
    }

    looper.printStatus();

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
    tx->createBranch<LV>("p4"); 
    tx->createBranch<LV>("tag_p4"); 
    tx->createBranch<LV>("dilep_p4"); 
    tx->createBranch<LV>("mc_p4"); 
    tx->createBranch<LV>("mc_motherp4"); 
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
    tx->createBranch<bool>("passes_VVV_cutbased_3l_tight"); 
    tx->createBranch<bool>("passes_VVV_cutbased_3l_tight_noiso"); 
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
    tx->createBranch<float>("jet_close_lep_dr"); 
    tx->createBranch<int>("jet_close_lep_idx"); 
    tx->createBranch<bool>("jet_close_lep_passlooseid"); 
    tx->createBranch<bool>("jet_close_lep_haslep"); 
    tx->createBranch<LV>("jet_close_lep"); 
    tx->createBranch<LV>("jet_close_lep_rawjet"); 
    tx->createBranch<LV>("jet_close_lep_rawjet_L1");
    tx->createBranch<LV>("jet_close_lep_rawjet_L1_mlepp4");
    tx->createBranch<LV>("jet_close_lep_rawjet_L1_mlepp4_L2L3");
    tx->createBranch<LV>("jet_close_lep_rawjet_L1_mlepp4_L2L3_plepp4");
    tx->createBranch<LV>("jet_close_lep_reco"); 
    tx->createBranch<float>("jet_close_lep_undoJEC"); 
    tx->createBranch<float>("jet_close_lep_area"); 
    tx->createBranch<float>("jet_close_L1"); 
    tx->createBranch<float>("jet_close_L1nc"); 
    tx->createBranch<float>("jet_close_L1ncmc"); 
    tx->createBranch<float>("jet_close_L1L2L3"); 
    tx->createBranch<float>("jet_close_L2L3"); 
    tx->createBranch<float>("ptratio"); 
    tx->createBranch<float>("ptratiov2"); 
    tx->createBranch<float>("ptratiov3"); 
    tx->createBranch<float>("ptratiov4"); 
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
    tx->createBranch<vector<LV>>("jets_p4"); 
    tx->createBranch<vector<LV>>("jets_up_p4"); 
    tx->createBranch<vector<LV>>("jets_dn_p4"); 
    tx->createBranch<vector<LV>>("pfcands_p4"); 
    tx->createBranch<vector<int>>("pfcands_id"); 
    tx->createBranch<vector<int>>("pfcands_fromPV"); 
    tx->createBranch<vector<LV>>("pfcands_p4_dr03"); 
    tx->createBranch<vector<int>>("pfcands_id_dr03"); 
    tx->createBranch<vector<int>>("pfcands_fromPV_dr03"); 
    tx->createBranch<vector<float>>("jets_csv"); 
    tx->createBranch<vector<float>>("jets_up_csv"); 
    tx->createBranch<vector<float>>("jets_dn_csv"); 
    tx->createBranch<LV>("reliso_vector");
    tx->createBranch<float>("reliso_chiso");
    tx->createBranch<float>("reliso_nhiso");
    tx->createBranch<float>("reliso_emiso");
    tx->createBranch<float>("reliso_ncorriso");
    tx->createBranch<LV>("ptratio_vector_chiso_all");
    tx->createBranch<LV>("ptratio_vector_nhiso_all");
    tx->createBranch<LV>("ptratio_vector_emiso_all");
    tx->createBranch<LV>("ptratio_vector_lepiso_all");
    tx->createBranch<LV>("ptratio_vector_chiso");
    tx->createBranch<LV>("ptratio_vector_nhiso");
    tx->createBranch<LV>("ptratio_vector_emiso");
    tx->createBranch<LV>("ptratio_vector_lepiso");
    tx->createBranch<LV>("ptratio_vector_ncorriso");
    tx->createBranch<LV>("ptratio_vector_absiso");
    tx->createBranch<LV>("ptratio_vector_chnhem");
    tx->createBranch<LV>("ptratio_vector_chnhemlep");
    tx->createBranch<LV>("ptratio_vector_chnhem_all");
    tx->createBranch<LV>("ptratio_vector_chnhemlep_all");
    tx->createBranch<float>("ptratio_chiso_all");
    tx->createBranch<float>("ptratio_nhiso_all");
    tx->createBranch<float>("ptratio_emiso_all");
    tx->createBranch<float>("ptratio_ncorriso_all");
    tx->createBranch<float>("ptratio_lepiso_all");
    tx->createBranch<float>("ptratio_hfiso_all");
    tx->createBranch<float>("ptratio_eliso_all");
    tx->createBranch<float>("ptratio_muiso_all");
    tx->createBranch<float>("ptratio_chiso");
    tx->createBranch<float>("ptratio_nhiso");
    tx->createBranch<float>("ptratio_emiso");
    tx->createBranch<float>("ptratio_lepiso");
    tx->createBranch<float>("ptratio_hfiso");
    tx->createBranch<float>("ptratio_eliso");
    tx->createBranch<float>("ptratio_muiso");
    tx->createBranch<float>("ptratio_ncorriso");
    tx->createBranch<float>("EA03v2");
    tx->createBranch<float>("evt_fixgridfastjet_all_rho");
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
    tx->createBranch<bool>("threeChargeAgree"); 
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
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_L1"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_LeadingLeg"); 
    tx->createBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_TrailingLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_MuonLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_ElectronLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg"); 
    tx->createBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_MuonLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_ElectronLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg"); 
    tx->createBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg"); 

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
//void babyMaker_v2::ProcessMuons() { coreMuon.process(isPreselMuon); }

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
    if (cms3.evt_isRealData() && cms3.evt_run() <= 251562)
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
    // Count number of good vertices.
    int nvtx = 0;
    for (unsigned int ivtx = 0; ivtx < cms3.evt_nvtxs(); ivtx++)
    {
        if (!isGoodVertex(ivtx)) { continue; }
        nvtx++;
    }
    tx->setBranch<int>("nvtx", nvtx);

    // Set the TTree branches
    tx->setBranch<bool>("evt_isRealData", cms3.evt_isRealData());
    if (cms3.evt_isRealData())
        tx->setBranch<float>("evt_scale1fb", 1);
    else
        tx->setBranch<float>("evt_scale1fb", coreDatasetInfo.getScale1fb());
    tx->setBranch<unsigned long long>("evt_event", cms3.evt_event());
    tx->setBranch<unsigned long long>("evt_lumiBlock", cms3.evt_lumiBlock());
    tx->setBranch<unsigned long long>("evt_run", cms3.evt_run());
}

//##############################################################################################################
void babyMaker_v2::FillElectronTrigger(int idx, int tag_idx)
{
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

    // Set the branch of the tag electrons single lepton trigger bit
    int tag_HLT_Ele27_eta2p1_WPTight_Gsf;
    int tag_HLT_Ele32_eta2p1_WPTight_Gsf;
    setHLTBranch("HLT_Ele27_eta2p1_WPTight_Gsf_v", (tag_idx >= 0 ? cms3.els_HLT_Ele27_eta2p1_WPTight_Gsf().at(tag_idx) : 0), tag_HLT_Ele27_eta2p1_WPTight_Gsf);
    setHLTBranch("HLT_Ele32_eta2p1_WPTight_Gsf_v", (tag_idx >= 0 ? cms3.els_HLT_Ele32_eta2p1_WPTight_Gsf().at(tag_idx) : 0), tag_HLT_Ele32_eta2p1_WPTight_Gsf);

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
    tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL" , passHLTTriggerPattern("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_v"));
    tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ" , passHLTTriggerPattern("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"));
    tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL" , passHLTTriggerPattern("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"));
    tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ" , passHLTTriggerPattern("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"));
    tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg" , cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg" , cms3.els_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg" , cms3.els_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg" , cms3.els_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg", cms3.els_HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg()[idx]); 
    tx->setBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg", cms3.els_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_ElectronLeg()[idx]); 
    tx->setBranch<int>("tag_HLT_Ele27_eta2p1_WPTight_Gsf", tag_HLT_Ele27_eta2p1_WPTight_Gsf); 
    tx->setBranch<int>("tag_HLT_Ele32_eta2p1_WPTight_Gsf", tag_HLT_Ele32_eta2p1_WPTight_Gsf); 
}

//##############################################################################################################
void babyMaker_v2::FillElectronIDVariables(int idx, int tag_idx)
{
    tx->setBranch<LV>("p4", cms3.els_p4()[idx]);
    tx->setBranch<LV>("tag_p4", cms3.els_p4()[tag_idx]);
    tx->setBranch<int>("tag_charge", cms3.els_charge()[tag_idx]);
    if (!cms3.evt_isRealData())
        tx->setBranch<int>("tag_mc_motherid", cms3.els_mc_motherid()[tag_idx]);
    LV dilep_p4 = cms3.els_p4()[idx] + cms3.els_p4()[tag_idx];
    float dilep_mass = dilep_p4.mass();
    tx->setBranch<LV>("dilep_p4", dilep_p4);
    tx->setBranch<float>("dilep_mass", dilep_mass);
    tx->setBranch<int>("id", -11.0 * cms3.els_charge()[idx]);
    tx->setBranch<float>("dxyPV", cms3.els_dxyPV()[idx]);
    tx->setBranch<float>("dZ", cms3.els_dzPV()[idx]);
    tx->setBranch<float>("ip3d", cms3.els_ip3d()[idx]);
    tx->setBranch<float>("ip3derr", cms3.els_ip3derr()[idx]);
    tx->setBranch<bool>("passes_VVV_cutbased_veto", electronID(idx, VVV_cutbased_veto_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_veto_noiso", electronID(idx, VVV_cutbased_veto_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_fo", electronID(idx, VVV_cutbased_fo_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_fo_noiso", electronID(idx, VVV_cutbased_fo_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_tight_noiso", electronID(idx, VVV_cutbased_tight_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_3l_tight_noiso", electronID(idx, VVV_cutbased_3l_tight_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_tight", electronID(idx, VVV_cutbased_tight_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_3l_tight", electronID(idx, VVV_cutbased_3l_tight_v2));
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
    tx->setBranch<float>("ptratio", elPtRatio(idx));
    tx->setBranch<float>("ptratiov4", elPtRatio(idx, 2));
    tx->setBranch<float>("RelIso03EAv2", eleRelIso03EA(idx, 2)); 
    tx->setBranch<float>("mva", getMVAoutput(idx));
}

//##############################################################################################################
void babyMaker_v2::FillIsolationVariables(int id, int idx)
{
    // Set electron or muon
    const LV lep_p4 = id == 11 ? cms3.els_p4()[idx] : cms3.mus_p4()[idx];
    float EA = id == 11 ? elEA03(idx, 2) : muEA03(idx, 2);

    int ijet = closestJetIdx(lep_p4, 0.4, 3.0);
    LV closejet = closestJet(lep_p4, 0.4, 3.0, 2);
    LV rawjet = ijet >= 0 ? cms3.pfjets_p4()[ijet] * cms3.pfjets_undoJEC()[ijet] : LV();
    float JEC1 = getJEC1();
    float JEC2 = getJEC2();
    tx->setBranch<float>("jet_close_L1", JEC1);
    tx->setBranch<float>("jet_close_L2L3", JEC2);
    tx->setBranch<float>("jet_close_lep_undoJEC", ijet >= 0 ? cms3.pfjets_undoJEC()[ijet] : 1.); 

    tx->setBranch<LV>("jet_close_lep", closejet); 
    tx->setBranch<LV>("jet_close_lep_rawjet", rawjet); 
    tx->setBranch<LV>("jet_close_lep_rawjet_L1", rawjet*JEC1); 
    tx->setBranch<LV>("jet_close_lep_rawjet_L1_mlepp4", rawjet*JEC1-lep_p4); 
    tx->setBranch<LV>("jet_close_lep_rawjet_L1_mlepp4_L2L3", (rawjet*JEC1-lep_p4)*JEC2); 
    tx->setBranch<LV>("jet_close_lep_rawjet_L1_mlepp4_L2L3_plepp4", (rawjet*JEC1-lep_p4)*JEC2+lep_p4); 
    tx->setBranch<int>("jet_close_lep_idx", ijet);
    tx->setBranch<float>("jet_close_lep_dr", closejet.pt() > 0 ? ROOT::Math::VectorUtil::DeltaR(closejet, lep_p4) : 0.); 

    bool haslep = true;
    if (ijet >= 0)
    {
        haslep = false;
        float ptratio_chiso = 0;
        float ptratio_nhiso = 0;
        float ptratio_emiso = 0;
        float ptratio_lepiso = 0;
        float ptratio_hfiso = 0;
        float ptratio_eliso = 0;
        float ptratio_muiso = 0;
        float ptratio_chiso_all = 0;
        float ptratio_nhiso_all = 0;
        float ptratio_emiso_all = 0;
        float ptratio_lepiso_all = 0;
        float ptratio_hfiso_all = 0;
        float ptratio_eliso_all = 0;
        float ptratio_muiso_all = 0;
        LV ptratio_chiso_vec;
        LV ptratio_nhiso_vec;
        LV ptratio_emiso_vec;
        LV ptratio_lepiso_vec;
        LV ptratio_chiso_vec_all;
        LV ptratio_nhiso_vec_all;
        LV ptratio_emiso_vec_all;
        LV ptratio_lepiso_vec_all;
        for (unsigned ipf = 0; ipf < cms3.pfjets_pfcandIndicies()[ijet].size(); ++ipf)
        {
            int pfidx = cms3.pfjets_pfcandIndicies()[ijet][ipf];
            tx->pushbackToBranch<LV>("pfcands_p4", cms3.pfcands_p4()[pfidx]);
            tx->pushbackToBranch<int>("pfcands_id", cms3.pfcands_particleId()[pfidx]);
            tx->pushbackToBranch<int>("pfcands_fromPV", cms3.pfcands_fromPV()[pfidx]);
            if (ROOT::Math::VectorUtil::DeltaR(cms3.pfcands_p4()[pfidx], lep_p4) < 0.1 && cms3.pfcands_particleId()[pfidx] == tx->getBranch<int>("id"))
                haslep = true;
            const int& pf_id = cms3.pfcands_particleId()[pfidx];
            const LV& p4 = cms3.pfcands_p4()[pfidx];
            const int& fromPV = cms3.pfcands_fromPV()[pfidx];
            if (abs(pf_id) == 211)
            {
                if (fromPV > 1)
                {
                    ptratio_chiso += p4.pt();
                    ptratio_chiso_vec += p4;
                }
                ptratio_chiso_all += p4.pt();
                ptratio_chiso_vec_all += p4;
            }
            else if (abs(pf_id) == 130)
            {
                if (p4.pt() > 0.5)
                {
                    ptratio_nhiso += p4.pt();
                    ptratio_nhiso_vec += p4;
                }
                ptratio_nhiso_all += p4.pt();
                ptratio_nhiso_vec_all += p4;
            }
            else if (abs(pf_id) == 22)
            {
                if (fromPV > 1)
                {
                    ptratio_emiso += p4.pt();
                    ptratio_emiso_vec += p4;
                }
                ptratio_emiso_all += p4.pt();
                ptratio_emiso_vec_all += p4;
            }
            else if (abs(pf_id) == 11)
            {
                if (fromPV > 1)
                {
                    ptratio_lepiso += p4.pt();
                    ptratio_eliso += p4.pt();
                    ptratio_lepiso_vec += p4;
                }
                ptratio_lepiso_all += p4.pt();
                ptratio_eliso_all += p4.pt();
                ptratio_lepiso_vec_all += p4;
            }
            else if (abs(pf_id) == 13)
            {
                if (p4.pt() > 0.5)
                {
                    ptratio_lepiso += p4.pt();
                    ptratio_muiso += p4.pt();
                    ptratio_lepiso_vec += p4;
                }
                ptratio_lepiso_all += p4.pt();
                ptratio_muiso_all += p4.pt();
                ptratio_lepiso_vec_all += p4;
            }
            else
            {
                ptratio_hfiso_all += p4.pt();
                if (p4.pt() > 0.5)
                    ptratio_hfiso += p4.pt();
            }
        }
        float ptratio_ncorriso = std::max(float(0.0), ptratio_nhiso + ptratio_emiso - cms3.evt_fixgridfastjet_all_rho() * EA);
        LV ptratio_ncorriso_vec = (ptratio_nhiso_vec + ptratio_emiso_vec).pt() > 0 ? (ptratio_nhiso_vec + ptratio_emiso_vec) * std::max(float(0.0), (ptratio_nhiso_vec + ptratio_emiso_vec).pt() - cms3.evt_fixgridfastjet_all_rho() * EA) / (ptratio_nhiso_vec + ptratio_emiso_vec).pt() : LV();
        float ptratio_ncorriso_all = std::max(float(0.0), ptratio_nhiso_all + ptratio_emiso_all - cms3.evt_fixgridfastjet_all_rho() * EA);
        tx->setBranch<float>("ptratio_chiso", ptratio_chiso);
        tx->setBranch<float>("ptratio_nhiso", ptratio_nhiso);
        tx->setBranch<float>("ptratio_emiso", ptratio_emiso);
        tx->setBranch<float>("ptratio_ncorriso", ptratio_ncorriso);
        tx->setBranch<float>("ptratio_eliso", ptratio_eliso);
        tx->setBranch<float>("ptratio_muiso", ptratio_muiso);
        tx->setBranch<float>("ptratio_lepiso", ptratio_lepiso);
        tx->setBranch<float>("ptratio_hfiso", ptratio_hfiso);
        tx->setBranch<float>("ptratio_chiso_all", ptratio_chiso_all);
        tx->setBranch<float>("ptratio_nhiso_all", ptratio_nhiso_all);
        tx->setBranch<float>("ptratio_emiso_all", ptratio_emiso_all);
        tx->setBranch<float>("ptratio_ncorriso_all", ptratio_ncorriso_all);
        tx->setBranch<float>("ptratio_eliso_all", ptratio_eliso_all);
        tx->setBranch<float>("ptratio_muiso_all", ptratio_muiso_all);
        tx->setBranch<float>("ptratio_lepiso_all", ptratio_lepiso_all);
        tx->setBranch<float>("ptratio_hfiso_all", ptratio_hfiso_all);
        tx->setBranch<LV>("ptratio_vector_chiso", ptratio_chiso_vec);
        tx->setBranch<LV>("ptratio_vector_nhiso", ptratio_nhiso_vec);
        tx->setBranch<LV>("ptratio_vector_emiso", ptratio_emiso_vec);
        tx->setBranch<LV>("ptratio_vector_lepiso", ptratio_lepiso_vec);
        tx->setBranch<LV>("ptratio_vector_chiso_all", ptratio_chiso_vec_all);
        tx->setBranch<LV>("ptratio_vector_nhiso_all", ptratio_nhiso_vec_all);
        tx->setBranch<LV>("ptratio_vector_emiso_all", ptratio_emiso_vec_all);
        tx->setBranch<LV>("ptratio_vector_lepiso_all", ptratio_lepiso_vec_all);
        tx->setBranch<LV>("ptratio_vector_chnhem", (ptratio_chiso_vec + ptratio_nhiso_vec + ptratio_emiso_vec));
        tx->setBranch<LV>("ptratio_vector_chnhemlep", (ptratio_chiso_vec + ptratio_nhiso_vec + ptratio_emiso_vec + ptratio_lepiso_vec));
        tx->setBranch<LV>("ptratio_vector_chnhem_all", (ptratio_chiso_vec_all + ptratio_nhiso_vec_all + ptratio_emiso_vec_all));
        tx->setBranch<LV>("ptratio_vector_chnhemlep_all", (ptratio_chiso_vec_all + ptratio_nhiso_vec_all + ptratio_emiso_vec_all + ptratio_lepiso_vec_all));
        tx->setBranch<LV>("ptratio_vector_ncorriso", ptratio_ncorriso_vec);
        tx->setBranch<LV>("ptratio_vector_absiso", ptratio_ncorriso_vec + ptratio_chiso_vec);
    }
    tx->setBranch<bool>("jet_close_lep_haslep", haslep);

    tx->setBranch<float>("EA03v2", EA);
    tx->setBranch<float>("evt_fixgridfastjet_all_rho", cms3.evt_fixgridfastjet_all_rho());
    float reliso_chiso = 0;
    float reliso_nhiso = 0;
    float reliso_emiso = 0;
    LV reliso_vector;
    for (unsigned ipf = 0; ipf < cms3.pfcands_p4().size(); ++ipf)
    {
        const LV& pf_p4 = cms3.pfcands_p4()[ipf];
        float dr = ROOT::Math::VectorUtil::DeltaR(pf_p4, lep_p4);
        if (dr < 0.3)
        {
            tx->pushbackToBranch<LV>("pfcands_p4_dr03", pf_p4);
            tx->pushbackToBranch<int>("pfcands_id_dr03", cms3.pfcands_particleId()[ipf]);
            tx->pushbackToBranch<int>("pfcands_fromPV_dr03", cms3.pfcands_fromPV()[ipf]);
            const int& pf_id = cms3.pfcands_particleId()[ipf];
            const LV& p4 = cms3.pfcands_p4()[ipf];
            const int& fromPV = cms3.pfcands_fromPV()[ipf];
            if (abs(pf_id) == 211 && fromPV > 1)
                reliso_chiso += p4.pt();
            else if (abs(pf_id) == 130 && p4.pt() > 0.5)
                reliso_nhiso += p4.pt();
            else if (abs(pf_id) == 22 && p4.pt() > 0.5)
                reliso_emiso += p4.pt();
            reliso_vector += p4;
        }
    }
    float reliso_ncorriso = std::max(float(0.0), reliso_nhiso + reliso_emiso - cms3.evt_fixgridfastjet_all_rho() * EA);
    tx->setBranch<float>("reliso_chiso", reliso_chiso);
    tx->setBranch<float>("reliso_nhiso", reliso_nhiso);
    tx->setBranch<float>("reliso_emiso", reliso_emiso);
    tx->setBranch<float>("reliso_ncorriso", reliso_ncorriso);
    tx->setBranch<LV>("reliso_vector", reliso_vector);

    for (unsigned ijet = 0; ijet < coreJet.index.size(); ++ijet)
    {
        int jetidx = coreJet.index[ijet];
        float corr = coreJet.corrs[ijet];
        float shift = coreJet.shifts[ijet];
        float current_csv_val = cms3.getbtagvalue("pfCombinedInclusiveSecondaryVertexV2BJetTags", jetidx);

        // Check whether this jet overlaps with any of the leptons
        if (isLeptonOverlappingWithJet(ijet))
            continue;

        LV jet = cms3.pfjets_p4()[jetidx] * cms3.pfjets_undoJEC()[jetidx] * corr;
        if (jet.pt() > 20)
        {
            tx->pushbackToBranch<LV>("jets_p4", jet);
            tx->pushbackToBranch<float>("jets_csv", current_csv_val);
            coreBtagSF.accumulateSF(jetidx, jet.pt(), jet.eta());
        }

        LV jet_up = jet * (1. + shift);
        if (jet_up.pt() > 20)
        {
            tx->pushbackToBranch<LV>("jets_up_p4", jet_up);
            tx->pushbackToBranch<float>("jets_up_csv", current_csv_val);
        }

        LV jet_dn = jet * (1. - shift);
        if (jet_dn.pt() > 20)
        {
            tx->pushbackToBranch<LV>("jets_dn_p4", jet_dn);
            tx->pushbackToBranch<float>("jets_dn_csv", current_csv_val);
        }
    }
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

        FillEventInfo();
        FillElectronTrigger(idx, tag_idx);
        FillElectronIDVariables(idx, tag_idx);
        FillIsolationVariables(11, idx);
        FillTTree();
    }
}

//##############################################################################################################
void babyMaker_v2::FillMuonTrigger(int idx, int tag_idx)
{
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

    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL = passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v");
    HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL = passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");
    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ = passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v");
    HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ = passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");
    HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ = passHLTTriggerPattern("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");

    // Tag single muon trigger
    int tag_HLT_IsoMu24;
    int tag_HLT_IsoTkMu24;
    setHLTBranch("HLT_IsoMu24_v", (tag_idx >= 0 ? cms3.mus_HLT_IsoMu24().at(tag_idx) : 0), tag_HLT_IsoMu24);
    setHLTBranch("HLT_IsoTkMu24_v", (tag_idx >= 0 ? cms3.mus_HLT_IsoTkMu24().at(tag_idx) : 0), tag_HLT_IsoTkMu24);

    tx->setBranch<int>("HLT_Mu8", HLT_Mu8);
    tx->setBranch<int>("HLT_Mu17", HLT_Mu17);
    tx->setBranch<int>("HLT_Mu8_TrkIsoVVL", HLT_Mu8_TrkIsoVVL);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL", HLT_Mu17_TrkIsoVVL);
    tx->setBranch<int>("HLT_IsoMu24", HLT_IsoMu24);
    tx->setBranch<int>("HLT_IsoTkMu24", HLT_IsoTkMu24);
    tx->setBranch<int>("HLT_Mu50", HLT_Mu50);
    tx->setBranch<int>("HLT_Mu55", HLT_Mu55);
    tx->setBranch<int>("HLT_TkMu50", HLT_TkMu50);
    // Triggers
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ", HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ);
    tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ", HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL", HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL);
    // Trigger matching
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg", cms3.mus_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg", cms3.mus_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_LeadingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg", cms3.mus_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_TrailingLeg()[idx]);
    tx->setBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg", cms3.mus_HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg()[idx]); 
    tx->setBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg", cms3.mus_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_MuonLeg()[idx]); 
    tx->setBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL", passHLTTriggerPattern("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL"));
    tx->setBranch<int>("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ", passHLTTriggerPattern("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ"));
    tx->setBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", passHLTTriggerPattern("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"));
    tx->setBranch<int>("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", passHLTTriggerPattern("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"));
    tx->setBranch<int>("tag_HLT_IsoMu24", tag_HLT_IsoMu24);
    tx->setBranch<int>("tag_HLT_IsoTkMu24", tag_HLT_IsoTkMu24);
}

//##############################################################################################################
void babyMaker_v2::FillMuonIDVariables(int idx, int tag_idx)
{
    tx->setBranch<LV>("p4", cms3.mus_p4()[idx]);
    tx->setBranch<LV>("tag_p4", cms3.mus_p4()[tag_idx]);
    tx->setBranch<int>("tag_charge", cms3.mus_charge()[tag_idx]);
    if (!cms3.evt_isRealData())
        tx->setBranch<int>("tag_mc_motherid", cms3.mus_mc_motherid()[tag_idx]);
    LV dilep_p4 = cms3.mus_p4()[idx] + cms3.mus_p4()[tag_idx];
    float dilep_mass = dilep_p4.mass();
    tx->setBranch<LV>("dilep_p4", dilep_p4);
    tx->setBranch<float>("dilep_mass", dilep_mass);
    tx->setBranch<int>("id", -13.0 * cms3.mus_charge()[idx]);
    tx->setBranch<float>("dxyPV", cms3.mus_dxyPV()[idx]);
    tx->setBranch<float>("dZ", cms3.mus_dzPV()[idx]);
    tx->setBranch<float>("ip3d", cms3.mus_ip3d()[idx]);
    tx->setBranch<float>("ip3derr", cms3.mus_ip3derr()[idx]);
    tx->setBranch<float>("ptErr", cms3.mus_ptErr()[idx]); 
    tx->setBranch<float>("trk_pt", cms3.mus_trk_p4()[idx].pt()); 
    tx->setBranch<bool>("passes_VVV_cutbased_veto", muonID(idx, VVV_cutbased_veto_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_veto_noiso", muonID(idx, VVV_cutbased_veto_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_fo", muonID(idx, VVV_cutbased_fo_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_fo_noiso", muonID(idx, VVV_cutbased_fo_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_tight_noiso", muonID(idx, VVV_cutbased_tight_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_3l_tight_noiso", muonID(idx, VVV_cutbased_3l_tight_noiso_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_tight", muonID(idx, VVV_cutbased_tight_v2));
    tx->setBranch<bool>("passes_VVV_cutbased_3l_tight", muonID(idx, VVV_cutbased_3l_tight_v2));
    tx->setBranch<bool>("passes_POG_mediumID", isMediumMuonPOG(idx)); 
    tx->setBranch<float>("ptratio", muPtRatio(idx));
    tx->setBranch<float>("ptratiov4", muPtRatio(idx, 2));
    tx->setBranch<float>("RelIso03EAv2", muRelIso03EA(idx, 2)); 
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

        FillEventInfo();
        FillMuonTrigger(idx, tag_idx);
        FillMuonIDVariables(idx, tag_idx);
        FillIsolationVariables(13, idx);
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

    //for (auto& imu : coreMuon.index)
    for (unsigned int imu = 0; imu < cms3.mus_p4().size(); ++imu)
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

    //for (auto& iel : coreElectron.index)
    for (unsigned int iel = 0; iel < cms3.els_p4().size(); ++iel)
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
//    if (!( fabs((cms3.mus_p4()[i] + cms3.mus_p4()[j]).mass() - 90.) <  30.   )) return false;
    return true;
}

//##############################################################################################################
bool babyMaker_v2::checkElectronTag(int i, int j)
{
    if (!( cms3.els_p4()[j].pt()                                    >= 20.0  )) return false;
    if (!( fabs(cms3.els_etaSC()[j])                                <=  2.5  )) return false;
    if (!( cms3.els_passMediumId()[j]                                        )) return false;
    if (!( fabs(cms3.els_ip3d()[j] / cms3.els_ip3derr()[j])         <=  4    )) return false;
//    if (!( fabs((cms3.els_p4()[i] + cms3.els_p4()[j]).mass() - 90.) <  30.   )) return false;
    return true;
}

//eof

