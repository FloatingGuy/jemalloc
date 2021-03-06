#ifndef JEMALLOC_INTERNAL_CTL_STRUCTS_H
#define JEMALLOC_INTERNAL_CTL_STRUCTS_H

#include "jemalloc/internal/jemalloc_internal_types.h"
#include "jemalloc/internal/ql.h"
#include "jemalloc/internal/size_classes.h"
#include "jemalloc/internal/stats.h"

struct ctl_node_s {
	bool			named;
};

struct ctl_named_node_s {
	struct ctl_node_s	node;
	const char		*name;
	/* If (nchildren == 0), this is a terminal node. */
	size_t			nchildren;
	const			ctl_node_t *children;
	int			(*ctl)(tsd_t *, const size_t *, size_t, void *,
	    size_t *, void *, size_t);
};

struct ctl_indexed_node_s {
	struct ctl_node_s	node;
	const ctl_named_node_t	*(*index)(tsdn_t *, const size_t *, size_t,
	    size_t);
};

struct ctl_arena_stats_s {
	arena_stats_t		astats;

	/* Aggregate stats for small size classes, based on bin stats. */
	size_t			allocated_small;
	uint64_t		nmalloc_small;
	uint64_t		ndalloc_small;
	uint64_t		nrequests_small;

	malloc_bin_stats_t	bstats[NBINS];
	malloc_large_stats_t	lstats[NSIZES - NBINS];
};

struct ctl_stats_s {
	size_t			allocated;
	size_t			active;
	size_t			metadata;
	size_t			resident;
	size_t			mapped;
	size_t			retained;

	mutex_prof_data_t	mutex_prof_data[num_global_prof_mutexes];
};

struct ctl_arena_s {
	unsigned		arena_ind;
	bool			initialized;
	ql_elm(ctl_arena_t)	destroyed_link;

	/* Basic stats, supported even if !config_stats. */
	unsigned		nthreads;
	const char		*dss;
	ssize_t			dirty_decay_time;
	ssize_t			muzzy_decay_time;
	size_t			pactive;
	size_t			pdirty;
	size_t			pmuzzy;

	/* NULL if !config_stats. */
	ctl_arena_stats_t	*astats;
};

struct ctl_arenas_s {
	uint64_t		epoch;
	unsigned		narenas;
	ql_head(ctl_arena_t)	destroyed;

	/*
	 * Element 0 corresponds to merged stats for extant arenas (accessed via
	 * MALLCTL_ARENAS_ALL), element 1 corresponds to merged stats for
	 * destroyed arenas (accessed via MALLCTL_ARENAS_DESTROYED), and the
	 * remaining MALLOCX_ARENA_MAX+1 elements correspond to arenas.
	 */
	ctl_arena_t		*arenas[MALLOCX_ARENA_MAX + 3];
};

#endif /* JEMALLOC_INTERNAL_CTL_STRUCTS_H */
