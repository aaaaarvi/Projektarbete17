int mergePatterns() {
  PatternDBMerger dbMerger;
  dbMerger.SetInputFilename("patternDB.root");
  dbMerger.SetInputTreename("trackPatterns");
  dbMerger.SetOutputFileName("patternDBclean.root");

  dbMerger.Execute();

  return 0;
}