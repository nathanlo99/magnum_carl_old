
Short term:
- Add a statistics module which tracks common counters (number of alpha-beta calls, number of quiescence_search calls, number of static evaluations, number of transposition_table queries, etc) in debug mode and prints them out periodically
- Add null move pruning, and consider MTD-bi search


Longer term:
- Transition to cmake?
- Replace all this info logging stuff with glog
