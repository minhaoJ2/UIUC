/**
 * savvy_scheduler
 * CS 241 - Fall 2020
 * partner: chenlei2, yunzhez2, minhaoj2
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"

/**
 * The struct to hold the information about a given job
 */
typedef struct _job_info {
    int id;
    double prior;
    double t_arrival;
    double first_time;
    double prev_time;
    double finish_time;
    double runtime_r;
    double runtime_e;
    double rr_flag;
    /* TODO: Add any other information and bookkeeping you need into this
     * struct. */
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;
double sum_wait_time;
double sum_resp_time;
double sum_turn_time;
double sum_finished_job;


void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any additional set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    // TODO: Implement me!
    job* ja = (job*) a, *jb = (job*)b;
    job_info* a_info = ja->metadata;
    job_info* b_info = jb->metadata;
    int diff_t_arrival = a_info->t_arrival - b_info ->t_arrival;
    int diff_id = a_info->id < b_info->id;
    if(diff_t_arrival > 0) return 1;
    else if(diff_t_arrival < 0) return -1;
    else {
        if(diff_id == 1) 
            return -1;
    }
    return 1;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    // TODO: Implement me!
    job* ja = (job*) a, *jb = (job*)b;
    job_info* a_info = ja->metadata;
    job_info* b_info = jb->metadata;
    if(a_info->prior > b_info ->prior) return 1;
    else if(a_info->prior < b_info ->prior) return -1;
    else return break_tie(a, b);
}

int comparer_psrtf(const void *a, const void *b) {
    // TODO: Implement me!
    job* ja = (job*) a, *jb = (job*)b;
    job_info* a_info = ja->metadata;
    job_info* b_info = jb->metadata;
    int diff_duration = a_info->runtime_r - a_info->runtime_e - b_info ->runtime_r - b_info->runtime_e;
    if(diff_duration > 0) return 1;
    else if(diff_duration < 0) return -1;
    else return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
    // TODO: Implement me!
    job* ja = (job*) a, *jb = (job*)b;
    job_info* a_info = ja->metadata;
    job_info* b_info = jb->metadata;
    return a_info->rr_flag < b_info->rr_flag? -1: 1;
}

int comparer_sjf(const void *a, const void *b) {
    // TODO: Implement me!
    job* ja = (job*) a, *jb = (job*)b;
    job_info* a_info = ja->metadata;
    job_info* b_info = jb->metadata;
    int diff_runtime = a_info->runtime_r - b_info->runtime_r;
    if(diff_runtime > 0) return 1;
    else if(diff_runtime < 0) return -1;
    return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO: Implement me!
    job_info* info = calloc(1, sizeof(job_info));
    info->first_time = -1;
    info->id = job_number;
    info->runtime_r = sched_data->running_time;
    info->rr_flag = time;
    info->runtime_e = 0;
    info->prior = sched_data->priority;
    info->t_arrival = time;
    newjob->metadata = info;
    priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO: Implement me!
    // edge cases
    job* next = (job*)priqueue_peek(&pqueue);
    if(!job_evicted && !next) {
        priqueue_poll(&pqueue);
        return NULL;
    }
    else if(!job_evicted && next) {
        priqueue_poll(&pqueue);
        job_info* n_info = (job_info*)next->metadata;
        n_info->prev_time = time;
        n_info->first_time = n_info->first_time == -1 ? time: n_info->first_time;
        return next;
    }

    job_info* job_evicted_info = job_evicted->metadata;
    job_evicted_info->runtime_e = job_evicted_info->runtime_e + time - job_evicted_info->prev_time;
    job_evicted_info->prev_time = time;
    job* tmp = NULL;
    job* peek = (job*) priqueue_peek(&pqueue);

    if((pqueue_scheme == PPRI && peek && comparer_ppri(job_evicted, peek) == -1)
    || (pqueue_scheme == PSRTF && peek && comparer_psrtf(job_evicted, peek) == -1)
    ) 
        return job_evicted;
    
    else if(pqueue_scheme == PPRI || pqueue_scheme == PSRTF) {
        job_evicted->state = Ready;
        priqueue_offer(&pqueue, job_evicted);
        tmp = (job*) priqueue_poll(&pqueue);
        tmp->state = Running;
    }
    else if(pqueue_scheme == RR) {
        job_evicted->state = Ready;
        job_evicted_info->rr_flag = time;
        priqueue_offer(&pqueue, job_evicted);
        tmp = (job*) priqueue_poll(&pqueue);
    }
    else {
        return job_evicted;
    }

    job_info* n_info = (job_info*) tmp->metadata;
    n_info->first_time = n_info->first_time == -1 ? time: n_info->first_time;
    n_info->prev_time = time;
    return tmp;
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO: Implement me!
    job_info *d_info = job_done->metadata;
    sum_wait_time += time - d_info->t_arrival - d_info->runtime_r;
    sum_turn_time += time - d_info ->t_arrival;
    sum_resp_time = d_info->first_time - d_info->t_arrival;
    sum_finished_job += 1;
    d_info->finish_time = time;
    job_done->state = Unused;
    free(d_info);
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO: Implement me!
    return sum_wait_time / sum_finished_job;
}

double scheduler_average_turnaround_time() {
    // TODO: Implement me!
    return sum_turn_time / sum_finished_job;
}

double scheduler_average_response_time() {
    // TODO: Implement me!
    return sum_resp_time / sum_finished_job;
}

void scheduler_show_queue() {
    // OPTIONAL: Implement this if you need it!
    // while(priqueue_size(&pqueue) != 0) {
    //     job* cur = (job*)priqueue_peek(&pqueue);
    // }
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}