#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#include "lib_sem.h"
#include "lib_set.h"

#define NUM_REINDEER 10
#define NUM_ELVES 9
#define NUM_ELVES_PER_GROUP 3
#define OBSERVABLE_DELAYS 1
#define MAX_WAIT_TIME (INT_MAX >> 17)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static sem_set_t elf_line_set;
static sem_set_t sem_set;
static sem_t santa_busy_mutex;
static sem_t santa_sleep_mutex;
static sem_t reindeer_counting_sem;
static sem_t reindeer_counter_lock;
static int num_reindeer_waiting = 0;
static set_t elves_waiting;
static sem_t elf_counting_sem;
static sem_t elf_mutex;
static sem_t elf_counter_lock;
static int num_elves_being_helped = 0;

static void random_wait(const char *message, const int format_var) {
    unsigned int i = rand() % MAX_WAIT_TIME;
    fprintf(stdout, message, format_var);
    if(OBSERVABLE_DELAYS) {
        for(; --i; );
    }
}

static void help_elves(void) {
    int i;
    int elf;

    printf("santa: one or more elves are waiting\n");

    sem_wait(santa_busy_mutex);
    CRITICAL(elf_counter_lock, {
        num_elves_being_helped = NUM_ELVES_PER_GROUP;
    });

    // help the elves
    CRITICAL(elf_mutex, {

        fprintf(stdout, "santa: %d elves are waiting\n", set_cardinality(elves_waiting));

        for(i = 0; i < NUM_ELVES_PER_GROUP; ++i) {
            elf = set_take(elves_waiting);
            printf("santa: helping elf %d\n", elf);
            sem_signal_index(&elf_line_set, elf, 1);
        }
    });
}

static void prepare_sleigh(void) {
    sem_wait(santa_busy_mutex);
    printf("santa: preparing the sleigh\n");
    sem_signal_ntimes(reindeer_counting_sem, NUM_REINDEER);
}

static void *santa(void *_) {
    static int num_launched = 0;

    assert(1 == ++num_launched);

    while(1) {

        // wait until santa isn't busy to continue
        CRITICAL(santa_busy_mutex, {
            printf("santa: sleeping\n");
        });

        sem_wait(santa_sleep_mutex);

        printf("santa: awake and taking requests\n");

        if(NUM_REINDEER <= num_reindeer_waiting) {

            num_reindeer_waiting = NUM_REINDEER;
            prepare_sleigh();

            // completely lock santa; It's time to deliver presents!
            sem_wait(santa_busy_mutex);
            sem_wait(santa_sleep_mutex);

        } else if(3 <= set_cardinality(elves_waiting)) {
            help_elves();
        }
    }
    return NULL;
}

static void get_help(const int id) {
    printf("elf %d: receiving help from Santa\n", id);

    CRITICAL(elf_counter_lock, {
        --num_elves_being_helped;

        // unlock santa; signal that elves can queue again
        if(!num_elves_being_helped) {
            sem_signal(santa_busy_mutex);
            sem_signal_ntimes(elf_counting_sem, NUM_ELVES_PER_GROUP);
        }
    });
}

static void *elf(void *elf_id) {
    const int id = *((int *) elf_id);

    while(1) {
        random_wait("elf %d: working\n", id);
        fprintf(stdout, "elf %d: needs help from Santa\n", id);

        sem_wait(elf_counting_sem);

        CRITICAL(elf_mutex, {
            set_insert(elves_waiting, id);
            fprintf(stdout, "elf %d: queuing up for Santa's help\n", id);

            // wake up santa
            if(NUM_ELVES_PER_GROUP == set_cardinality(elves_waiting)) {
                fprintf(stdout, "elves: waking up santa\n");
                sem_signal(santa_sleep_mutex);
            }
        });

        sem_wait_index(&elf_line_set, id);
        get_help(id);
    }

    return NULL;
}

static void get_hitched(const int id) {
    printf("reindeer %d: getting hitched to sleigh\n", id);
}

static void *reindeer(void *reindeer_id) {
    const int id = *((int *) reindeer_id);

    random_wait("reindeer %d: heading to the Tropics\n", id);

    CRITICAL(reindeer_counter_lock, {
        ++num_reindeer_waiting;
    });

    fprintf(stdout, "reindeer %d: coming back from the Tropics\n", id);

    if(NUM_REINDEER <= num_reindeer_waiting) {
        printf("reindeer %d: last reindeer, needs to get Santa\n", id);
        sem_signal(santa_sleep_mutex);
    }

    // santa is awake, wait for him to indicate to get hitched
    sem_wait(reindeer_counting_sem);

    // sleigh is ready, prepared to get hitched
    CRITICAL(reindeer_counter_lock, {

        get_hitched(id);
        --(num_reindeer_waiting);

        if(0 == num_reindeer_waiting) {
            fprintf(stdout, "santa: all is set, time to fly\n");
            exit(EXIT_SUCCESS);
        }
    });

    return NULL;
}

static void sequence_pthreads(int num_threads, pthread_t *thread_ids, void *(*func)(void *), int *args) {
    int i;
    for(i = 0; num_threads--; ++i) {
        pthread_create(&(thread_ids[i]), NULL, func, (void *) &(args[i]));
    }
}

static void free_resources(void) {
    static int resources_freed = 0;
    if(!resources_freed) {
        resources_freed = 1;
        printf("\n...Merry Christmas!\n\n");
        sem_empty_set(&sem_set);
        sem_empty_set(&elf_line_set);
        set_exit_free(elves_waiting);
    }
}

static void sigint_handler(int _) {
    exit(EXIT_SUCCESS);
}

static void launch_threads(void) {

    pthread_t thread_ids[1 + NUM_ELVES + NUM_REINDEER];

    int ids[MAX(NUM_ELVES, NUM_REINDEER)];
    int i;

    for(i = 0; i < MAX(NUM_ELVES, NUM_REINDEER); ++i) {
        ids[i] = i;
    }

    pthread_create(&(thread_ids[0]), NULL, &santa, NULL);
    sequence_pthreads(NUM_ELVES, &(thread_ids[1]), &elf, &(ids[0]));
    sequence_pthreads(NUM_REINDEER, thread_ids + 1 + NUM_ELVES, &reindeer, ids);

    for(i = 0; i < (1 + NUM_ELVES + NUM_REINDEER); ++i) {
        pthread_join(thread_ids[i], NULL);
    }
}

int main(void) {

    sem_fill_set(&sem_set, 7);
    sem_fill_set(&elf_line_set, NUM_ELVES);

    elves_waiting = set_alloc(NUM_ELVES);

    if(!atexit(&free_resources)) {
        signal(SIGINT, &sigint_handler);

        sem_unpack_set(&sem_set, &reindeer_counter_lock, &elf_counter_lock, &santa_busy_mutex,
            &santa_sleep_mutex, &reindeer_counting_sem, &elf_counting_sem, &elf_mutex);

        sem_init(elf_mutex, 1);
        sem_init(reindeer_counter_lock, 1);
        sem_init(elf_counter_lock, 1);
        sem_init(santa_busy_mutex, 1);
        sem_init(santa_sleep_mutex, 0); // start mutex locked
        sem_init(reindeer_counting_sem, 0);
        sem_init(elf_counting_sem, NUM_ELVES_PER_GROUP);

        // initialize all elf semaphores as mutexes that start off locked
        sem_init_all(&elf_line_set, 0);

        srand((unsigned int) time(NULL));

        launch_threads();

    } else {
        fprintf(stderr, "unable to register an at-exit event handler\n");
        free_resources();
    }

    set_free(elves_waiting);

    return 0;
}