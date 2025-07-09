#!/usr/bin/env python3
#
# (c)2025 Ira Parsons
# so-get-syscalls.py - enumerate syscalls used per so function by
#                           recovering and parsing control flow graph
#

import angr
import networkx
import sys
import re

# validate command line arguments
if not sys.argv[1]:
	sys.exit(1);

# load so and linked libs
p = angr.Project(sys.argv[1], load_options={ 'auto_load_libs': True });

if not p:
	sys.exit(1);

# create a control flow graph for so
#  (this is static so that all possible paths are considered, dynamic would
#   freak out)
cfg = p.analyses.CFGFast();

if not cfg:
	sys.exit(1);

# recover function call graph from control flow graph
cg = cfg.functions.callgraph;

if not cg:
	sys.exit(1);

# prepare regex for deleting sys_*
regex = re.compile(r'^sys_.*$');

# find syscalls required for all outward-facing so functions
for s in list(set(list(p.loader.symbols))):

	if s.is_function and not s.is_local and cfg.functions.function(name=s.name):

		# create subgraph from function
		sg = cg.subgraph(networkx.single_source_shortest_path(cg, cfg.functions.function(name=s.name).addr).keys());

		# locate nodes at the end of the line
		calls = [];
		for n, d in sg.out_degree():
			if (d == 0) and cfg.functions.function(n).is_syscall:
				calls.append(cfg.functions.function(n).name);
		calls = [ i for i in calls if not regex.match(i) ];

		# convert syscall names to syscall.h macro format
		for i, call in enumerate(calls):
			calls[i] = "SYS_" + call;

		# format for header file insertion
		arrent = "{  \"" + s.name + "\" , (int *)&(int []){ " + ", ".join(calls) + ", -1 } },";

		if calls: print(arrent);
