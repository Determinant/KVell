#include <getopt.h>
#include "headers.h"

struct option longopts[] = {
    {"set-key-size", required_argument, 0, 'k'},
    {"set-val-size", required_argument, 0, 'v'},
    {"set-nitems", required_argument, 0, 'm'},
    {"set-nops", required_argument, 0, 'n'},
    {"set-crash-point", required_argument, 0, 'c'},
    {"workload", required_argument, 0, 'w'},
    {"nthread", required_argument, 0, 'T'},
    {0, 0, 0, 0}
};

const char *ws[] = {
    "a-uniform",
    "b-uniform",
    "c-uniform",
    "a-zipfian",
    "b-zipfian",
    "c-zipfian",
};

int main(int argc, char **argv) {
   int idx;
   size_t key_size = 32, val_size = 128;
   size_t nitems = 100000000, nops = 100000000;
   size_t crash_point = nops;
   size_t nthreads = 1;
   int wi = -1;
   for (;;)
   {
        int c = getopt_long(argc, argv, "k:v:n:m:T:c:w:", longopts, &idx);
        if (c == -1) break;
        switch (c)
        {
            case 'k': key_size = atoi(optarg);
                      assert(key_size > 0);
                      break;
            case 'v': val_size = atoi(optarg);
                      assert(val_size > 0);
                      break;
            case 'n': nitems = atoi(optarg);
                      break;
            case 'm': nops = atoi(optarg);
                      break;
            case 'c': crash_point = atoi(optarg);
                      break;
            case 'w': 
                      for (size_t i = 0; i < 6; i++) {
                          if (strcmp(optarg, ws[i]) == 0)
                          {
                              wi = i;
                              break;
                          }
                      }
                      break;
            case 'T': nthreads = atoi(optarg);
                      assert(nthreads > 0 && nthreads <= 32);
                      break;
        }
   }
   assert(wi != -1);
   assert(crash_point <= nops);
   int nb_disks, nb_workers_per_disk;
   declare_timer;

   /* Definition of the workload, if changed you need to erase the DB before relaunching */
   struct workload w = {
      .api = &YCSB,
      .nb_items_in_db = nitems,
      .nb_load_injectors = nthreads,
      //.nb_load_injectors = 12, // For scans (see scripts/run-aws.sh and OVERVIEW.md)
      .key_size = key_size,
      .value_size = val_size,
      .crash_point = crash_point,
   };


   /* Parsing of the options */
   if(argc - optind < 2)
      die("Usage: ./main <nb disks> <nb workers per disk>\n\tData is stored in %s\n", PATH);
   nb_disks = atoi(argv[optind]);
   nb_workers_per_disk = atoi(argv[optind + 1]);

   /* Pretty printing useful info */
   printf("# Configuration:\n");
   printf("# \tPage cache size: %lu GB\n", PAGE_CACHE_SIZE/1024/1024/1024);
   printf("# \tWorkers: %d working on %d disks\n", nb_disks*nb_workers_per_disk, nb_disks);
   printf("# \tIO configuration: %d queue depth (capped: %s, extra waiting: %s)\n", QUEUE_DEPTH, NEVER_EXCEED_QUEUE_DEPTH?"yes":"no", WAIT_A_BIT_FOR_MORE_IOS?"yes":"no");
   printf("# \tQueue configuration: %d maximum pending callbaks per worker\n", MAX_NB_PENDING_CALLBACKS_PER_WORKER);
   printf("# \tDatastructures: %d (memory index) %d (pagecache)\n", MEMORY_INDEX, PAGECACHE_INDEX);
   printf("# \tThread pinning: %s\n", PINNING?"yes":"no");
   printf("# \tBench: %s (%lu elements) key_size = %zu value_size = %zu crash at %zu\n", w.api->api_name(), w.nb_items_in_db, w.key_size, w.value_size, w.crash_point);

   /* Initialization of random library */
   start_timer {
      printf("Initializing random number generator (Zipf) -- this might take a while for large databases...\n");
      init_zipf_generator(0, w.nb_items_in_db - 1); /* This takes about 3s... not sure why, but this is legacy code :/ */
   } stop_timer("Initializing random number generator (Zipf)");

   /* Recover database */
   start_timer {
      slab_workers_init(nb_disks, nb_workers_per_disk);
   } stop_timer("Init found %lu elements", get_database_size());

   /* Add missing items if any */
   repopulate_db(&w);

   /* Launch benchs */
   bench_t workload, workloads[] = {
      ycsb_a_uniform, ycsb_b_uniform, ycsb_c_uniform,
      ycsb_a_zipfian, ycsb_b_zipfian, ycsb_c_zipfian,
      //ycsb_e_uniform, ycsb_e_zipfian, // Scans
   };
   workload = workloads[wi];
   //foreach(workload, workloads) {
      //if(workload == ycsb_e_uniform || workload == ycsb_e_zipfian) {
      //   w.nb_requests = 2000000LU; // requests for YCSB E are longer (scans) so we do less
      //} else {
         w.nb_requests = nops;
      //}
      run_workload(&w, workload);
   //}
   return 0;
}
