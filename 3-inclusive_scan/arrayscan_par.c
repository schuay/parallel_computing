#include "arrayscan.h"

#include "exscan.h"

#include <stdio.h>
#include <stdlib.h>

static const char *algorithm_name = "parallel algorithm";

#define NAME_LEN 100
char name[NAME_LEN];

const char *getAlgorithmName(void)
{
    snprintf(name, NAME_LEN, "%s; %s", algorithm_name, exscan_name);
    name[NAME_LEN - 1] = '\0';

    return name;
}

static size_t offset(size_t n, int rank, int processes)
{
    size_t size = n / processes;

    return size * rank;
}

static size_t length(size_t n, int rank, int processes)
{
    size_t size = n / processes;

    size_t off = offset(n, rank, processes);

    return (rank == processes - 1) ? n - off : size;
}

static int *genRecvcounts(int n, int processes)
{
    int *ret = malloc(processes * sizeof(int));
    if (ret == NULL) {
        return NULL;
    }

    for (int i = 0; i < processes; i++) {
        ret[i] = length(n, i, processes);
    }

    return ret;
}

static int *genRecvdispls(int n, int processes)
{
    int *ret = malloc(processes * sizeof(int));
    if (ret == NULL) {
        return NULL;
    }

    for (int i = 0; i < processes; i++) {
        ret[i] = offset(n, i, processes);
    }

    return ret;
}

static int gather(TYPE *snd, size_t len, MPI_Comm comm)
{
    return MPI_Gatherv(snd, len, TYPE_MPI, NULL, NULL, NULL, TYPE_MPI, MASTER, comm);
}

static TYPE *gather_master(TYPE *snd, size_t n, int rank, int processes, MPI_Comm comm)
{
    TYPE *ret = NULL;

    int *rcvcnt = genRecvcounts(n, processes);
    if (rcvcnt == NULL) {
        goto out;
    }

    int *rcvdispls = genRecvdispls(n, processes);
    if (rcvdispls == NULL) {
        goto out_rcvcnt;
    }

    size_t len = length(n, rank, processes);

    ret = malloc(n * sizeof(TYPE));
    if (ret == NULL) {
        goto out_rcvdispls;
    }

    if (MPI_Gatherv(snd, len, TYPE_MPI,
                ret, rcvcnt, rcvdispls, TYPE_MPI,
                MASTER, comm) != MPI_SUCCESS) {
        free(ret);
        ret = NULL;
        goto out_rcvdispls;
    }

out_rcvdispls:
    free(rcvdispls);

out_rcvcnt:
    free(rcvcnt);

out:
    return ret;
}

TYPE *arrayscan(const TYPE A[], size_t n, MPI_Comm comm)
{
    int processes, rank;

    MPI_Comm_size(comm, &processes);
    MPI_Comm_rank(comm, &rank);

    size_t off = offset(n, rank, processes);
    size_t len = length(n, rank, processes);

    TYPE *ret = NULL;

    // time complexity: n / p
    TYPE *a = arrayscan_seq(A + off, len);
    if (a == NULL) {
        return ret;
    }

    TYPE b = (len == 0) ? 0 : a[len - 1];
    TYPE rank_prefix;

    // time complexity: depends
    if (exscan(b, &rank_prefix, comm) != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error: MPI_Exscan failed.\n");
        goto out_a;
    }

    // time complexity: n / p
    for (size_t i = 0; i < len; i++) {
        a[i] = rank_prefix + a[i];
    }

    // time complexity: n / p + log p
    if (rank == MASTER) {
        ret = gather_master(a, n, rank, processes, comm);
    } else {
        gather(a, len, comm);
    }

out_a:
    free(a);

    return ret;
}
