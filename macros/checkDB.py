from ROOT import TFile, TH1F, TCanvas


myfile = TFile("patternDBclean.root")

tree = myfile.Get("trackPatterns")
entries = tree.GetEntriesFast()
hMomDist = TH1F('momDist','Momentum distribution',100,-0.5,0.5)

print('Number of entries to process: {}'.format(entries))

for jEntry in xrange(entries):
  iEntry = tree.LoadTree(jEntry)
  if iEntry < 0:
    break

  nb = tree.GetEntry(jEntry)
  if nb<=0:
    continue

  pattern = tree.pattern
  count = pattern.GetCount()

  if count<=1:
    continue

  momenta = pattern.getMomenta()
  momEntries = momenta.size()
  
  momAvg = 0.
  for iMom in xrange(momEntries):
    mom = momenta.at(iMom)
    # print(jEntry,iMom,mom.Mag())
    momAvg += mom.Mag()

  momAvg = momAvg / momEntries

  for iMom in xrange(momEntries):
    mom = momenta.at(iMom)
    diffMom = mom.Mag() - momAvg
    # print('entry {}, iMom {}'.format(jEntry,iMom))
    # print('{} - {} = {}'.format(mom.Mag(),momAvg,diffMom))
    hMomDist.Fill(diffMom)

  #print(momEntries)
  #print(pattern.GetCount())

can = TCanvas()
hMomDist.Draw()
can.Print("momDiff.pdf")