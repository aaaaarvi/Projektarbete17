int tracking_QA()
{
  // using only the standard cuts for the reconstructability of the Tracks!

  Int_t iVerbose = 0;
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  Bool_t timebased = kFALSE; // "Countinuous ReadOut"


  TString simFile = "sim.root";
  TString parFile = "par.root";
  TString digiFile = "digi.root";
  TString recoFile = "reco.root";  // only if reco is already performed
  TString outFile =  "qa.root";
  TString digiParFile = "all.par";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(digiFile);
  fRun->AddFriend(recoFile); // only if reco is already perfomed
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

  // Here you can also run your ideal track finder and the Real Trackfinder instead of a separate reco.macro

      /// ------------------------ QA task --------------------------

    PndTrackingQATask* trackingQA = new PndTrackingQATask("CombiTrack", "idealTrack");
    trackingQA->SetVerbose(0);
    trackingQA->AddHitsBranchName("STTHit"); // if you want to benchmark only certain Hits like only STT Hits for the Celltrackfinder
    trackingQA->SetFunctorName("OnlySttFunctor");
    fRun->AddTask(trackingQA);



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
