import os, sys
from ROOT import TH1D,TH2D,TFile,TTree,TCanvas

File = TFile("TPEHists.root","READ")
clct = File.Get("lctreader/Ev_clcttree")
alct = File.Get("lctreader/Ev_alcttree")

nEntries = clct.GetEntries()

lastEvent = -1
count_c = 0
count_w = 0
numEvents = 0

ncomp=0
nwire=0

X = 7

print "===================== Comparator Digis =====================" 

for i in range(0, nEntries):
  clct.GetEntry(i)
 
  if(clct.t_nComp>X):
    if(clct.t_EventNumberAnalyzed != lastEvent):
      print "Event:", clct.t_EventNumberAnalyzed
      lastEvent = clct.t_EventNumberAnalyzed
    print "---- Endcap =",clct.t_endcap,", Station =",clct.t_station,", Ring =",clct.t_ring,", and chamber =",clct.t_chamber," has nComp =",clct.t_nComp
    count_c+=1

  ncomp+=clct.t_nComp
  numEvents = clct.t_EventNumberAnalyzed  
  
print "======================== Wire Digis ========================"

alct = File.Get("lctreader/Ev_alcttree")

lastEvent = -1

nEntries = alct.GetEntries()
for i in range(0, nEntries):
  alct.GetEntry(i)
 
  if(alct.t_nWire>X):
    if(alct.t_EventNumberAnalyzed != lastEvent):
      print "Event:", alct.t_EventNumberAnalyzed
      lastEvent = alct.t_EventNumberAnalyzed
    print "---- Endcap =",alct.t_endcap,", Station =",alct.t_station,", Ring =",alct.t_ring,", and chamber =",alct.t_chamber," has nWire =",alct.t_nWire
    count_w+=1

  nwire+=alct.t_nWire
  numEvents = alct.t_EventNumberAnalyzed

print "========================== Results =========================="
print "In", numEvents, "events,", count_c, "chambers had >", X, "Comparator Digis with", ncomp, "comparator digis total" 
print "In", numEvents, "events,", count_w, "chambers had >", X, "Wire Digis with", nwire, "wire digis total"

