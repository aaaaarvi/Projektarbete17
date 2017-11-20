int eventCombiner_and_reco()
{
  // using only the standard cuts for the reconstructability of the Tracks!
  
  Int_t iVerbose = 0;
  Int_t nEvents = 0;  // if 0 all the vents will be processed

  TString simFile = "sim.root";
  TString parFile = "par.root";
  TString digiFile = "digi.root";
  TString outFile = "combi_reco.root";
  TString digiParFile = "all.par";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(digiFile);
  fRun->SetOutputFile(outFile);
  fRun->SetUseFairLinks(kTRUE);
  fRun->SetGenerateRunInfo(kFALSE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiParFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  //-------------------------------------------------------------------------
  //##################### Set all tasks ####################################
  // ------------------------------------------------------------------------

  PndEventCombinerTask* combi2 = new PndEventCombinerTask();  // a task to combine the STTHits of 2 Events
  combi2->AddInputBranch("STTHit");   //Set all input Branch for which you want to generate combined Events
  combi2->SetNEvents(2);              //Set the Number of Events Which should be combinded
  combi2->SetPersistence(kTRUE);      // Persistence set to KTRUE if the combinded events should be stored in the outputroot file
  	  	  	  	  	  	  	  	  	  // if set to kFALSE they are just buffered and the combinded Events can be used within the running macro only
  fRun->AddTask(combi2);             // the OutputBranchName will be comb_2_STTHit

  PndEventCombinerTask* combi3 = new PndEventCombinerTask(); // a task to combine the STTHits of 3 Events
  combi3->AddInputBranch("STTHit");
  combi3->SetNEvents(3);
  combi3->SetPersistence(kFALSE);		// here we decided to not store the data
  fRun->AddTask(combi3); 				// the OutputBranchName will be comb_3_STTHit


  PndSttCellTrackFinderTask *cellTrackFinder = new PndSttCellTrackFinderTask(); // look at the PndSttCellTrackFinderTask.h for information on all Parameters which can be set
  cellTrackFinder->SetPersistence(kTRUE);
  cellTrackFinder->AddHitBranch("STTHit");    // here I decide to only use this InputBranch (which is actuly also the default if nothing is specified)
  fRun->AddTask(cellTrackFinder);


  PndSttCellTrackFinderTask *cellTrackFinder_2 = new PndSttCellTrackFinderTask(); // an other instance off the Tracker using the combined and stored Data
   cellTrackFinder_2->SetPersistence(kTRUE);
   cellTrackFinder_2->AddHitBranch("comb_2_STTHit");            // it should use the combined STT hit of 2 events
   cellTrackFinder_2->SetOutBranchNamePrefix("comb_2");         // the defoult output is already used by the Tracker above, so we can/must specify an prefix
   fRun->AddTask(cellTrackFinder_2);

   PndSttCellTrackFinderTask *cellTrackFinder_3 = new PndSttCellTrackFinderTask(); //again a Tracker thsi time using the buffered Data
    cellTrackFinder_3->SetPersistence(kTRUE);
    cellTrackFinder_3->AddHitBranch("comb_3_STTHit");       // here we use as input the buffered combination of 3 Events. This is possible since we are still in the same macro;-)
    cellTrackFinder_3->SetOutBranchNamePrefix("comb_3");
    fRun->AddTask(cellTrackFinder_3);



  // ---- PndIdealTrackFinder has to stay for QA -----

  PndIdealTrackFinder* idealTracking = new PndIdealTrackFinder();
  //trackStt->SetTrackSelector("NoFtsTrackFunctor");
  idealTracking->SetTrackSelector("OnlySttFunctor"); //choos a Cut on the reconstructibility. See PndTrackFunctor.h/cxx for possible settings
  //trackStt->SetRelativeMomentumSmearing(0.05);
  //trackStt->SetVertexSmearing(0.05, 0.05, 0.05);
  //trackStt->SetTrackingEfficiency(1.);
  //trackStt->AddBranchName("MVDHitsPixel");
  //trackStt->AddBranchName("MVDHitsStrip");
  //trackStt->AddBranchName("STTHit");
  //trackStt->AddBranchName("GEMHit");
  //trackStt->AddBranchName("FTSHit");
  //trackStt->AddBranchName("SciTHit");
  //trackStt->SetOutputBranchName("SttMvdGemIdealTrack");
  idealTracking->SetOutputBranchName("idealTrack");
  idealTracking->SetPersistence(kTRUE);
  fRun->AddTask(idealTracking);



  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  cout << "fRun->Init()" << endl;
  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  return 0;
}
