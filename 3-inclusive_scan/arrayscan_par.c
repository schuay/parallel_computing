#include "arrayscan.h"

#include "exscan.h"

#include <stdio.h>
#include <stdlib.h>

static const char *algorithm_name = "parallel algorithm";

#define NAME_LEN 100
char name[NAME_LEN];

const char *getAlgorithmName(void)
{
    snprintf(name, NAME_LEN, "%s, %s", algorithm_name, exscan_name);
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

static int *genAlltoallvSendcounts(int n, int rank, int processes)
{
    int *ret = malloc(processes * sizeof(int));
    if (ret == NULL) {
        return NULL;
    }

    size_t len = length(n, rank, processes);

    for (int i = 0; i < processes; i++) {
        ret[i] = len;
    }

    return ret;
}

static int *genAlltoallvSenddispls(int processes)
{
    int *ret = malloc(processes * sizeof(int));
    if (ret == NULL) {
        return NULL;
    }

    size_t off = 0;

    for (int i = 0; i < processes; i++) {
        ret[i] = off;
    }

    return ret;
}

static int *genAlltoallvRecvcounts(int n, int processes)
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

static int *genAlltoallvRecvdispls(int n, int processes)
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

static int alltoall(TYPE *snd, TYPE *rcv, size_t n, int rank, int processes, MPI_Comm comm)
{
    int ret = -1;

    int *sndcnt = genAlltoallvSendcounts(n, rank, processes);
    if (sndcnt == NULL) {
        goto out;
    }

    int *rcvcnt = genAlltoallvRecvcounts(n, processes);
    if (rcvcnt == NULL) {
        goto out_sndcnt;
    }

    int *snddispls = genAlltoallvSenddispls(processes);
    if (snddispls == NULL) {
        goto out_rcvcnt;
    }

    int *rcvdispls = genAlltoallvRecvdispls(n, processes);
    if (snddispls == NULL) {
        goto out_snddispls;
    }

    ret = MPI_Alltoallv(snd, sndcnt, snddispls, TYPE_MPI, rcv, rcvcnt, rcvdispls, TYPE_MPI, comm);

    free(rcvdispls);

out_snddispls:
    free(snddispls);

out_rcvcnt:
    free(rcvcnt);

out_sndcnt:
    free(sndcnt);

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

    TYPE *a = arrayscan_seq(A + off, len);
    if (a == NULL) {
        return ret;
    }

    TYPE b = (len == 0) ? 0 : a[len - 1];
    TYPE rank_prefix;

    if (exscan(b, &rank_prefix, TYPE_MPI, comm) != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error: MPI_Exscan failed.\n");
        goto out_a;
    }

    /* MPI_Exscan() leaves this undefined, we need 0. */
    if (rank == 0) {
        rank_prefix = 0;
    }

    for (size_t i = 0; i < len; i++) {
        a[i] += rank_prefix;
    }

    ret = malloc(n * sizeof(TYPE));
    if (ret == NULL) {
        goto out_a;
    }

    if (alltoall(a, ret, n, rank, processes, comm) != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error: MPI_Alltoallv failed.\n");
        free(ret);
        ret = NULL;
        goto out_a;
    }

out_a:
    free(a);

    return ret;
}
