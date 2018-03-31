#!/bin/bash

VERSION=2016_tnp_sample_v1.18

OUTDIR=/nfs-7/userdata/phchang/lepton_babies/$VERSION/

mkdir -p $OUTDIR

PERIOD="B C D E F G H"

for P in $PERIOD; do
    echo $P
    hadd -k -f $OUTDIR/data_Run2016${P}_se.root /hadoop/cms/store/user/phchang/metis/lepbaby/$VERSION/Run2016${P}*SingleElectron*/*.root
    hadd -k -f $OUTDIR/data_Run2016${P}_sm.root /hadoop/cms/store/user/phchang/metis/lepbaby/$VERSION/Run2016${P}*SingleMuon*/*.root
    #echo hadd -k -f $OUTDIR/data_Run2016${P}_sm.root /hadoop/cms/store/user/phchang/metis/lepbaby/$VERSION/Run2016${P}*SingleMuon*/merged_?.root
done

hadd -k -f $OUTDIR/dy.root /hadoop/cms/store/user/phchang/metis/lepbaby/${VERSION}/DY*/merged_?.root

