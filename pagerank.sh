ALGORITHM=Pagerank
WORKERS=2
GRAPH=input/pagerank
RESULT=result/pagerank
NODES=916428
SNAPSHOT=7
TERMTHRESH=0.01
BUFMSG=500
PORTION=1


./maiter  --runner=$ALGORITHM --workers=$WORKERS --graph_dir=$GRAPH --result_dir=$RESULT --num_nodes=$NODES --snapshot_interval=$SNAPSHOT --portion=$PORTION --termcheck_threshold=$TERMTHRESH --bufmsg=$BUFMSG --v=0 > log


