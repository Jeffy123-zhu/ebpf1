// main.c
// Time Machine - User-space event collector and analyzer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "tracer.skel.h"
#include "events.h"

#define MAX_EVENTS 100000
#define WINDOW_SEC 5

static volatile int running = 1;
static struct tracer_bpf *skel = NULL;

typedef struct {
    struct event_data events[MAX_EVENTS];
    int count;
    int head;
    int wrapped;
} store_t;

static store_t store = {0};

typedef struct {
    unsigned long total;
    unsigned long funcs;
    unsigned long allocs;
    unsigned long frees;
    unsigned long crashes;
    time_t start;
} stats_t;

static stats_t stats = {0};

void sig_handler(int sig) {
    running = 0;
}

static void store_event(struct event_data *evt) {
    store.events[store.head] = *evt;
    store.head = (store.head + 1) % MAX_EVENTS;
    
    if (store.count < MAX_EVENTS) {
        store.count++;
    } else {
        store.wrapped = 1;
    }
    
    stats.total++;
    switch(evt->type) {
        case EVENT_FUNC_ENTRY:
        case EVENT_FUNC_EXIT:
            stats.funcs++;
            break;
        case EVENT_ALLOC:
            stats.allocs++;
            break;
        case EVENT_FREE:
            stats.frees++;
            break;
        case EVENT_SIGNAL:
            stats.crashes++;
            break;
    }
}

static int handle_event(void *ctx, void *data, size_t size) {
    struct event_data *evt = (struct event_data *)data;
    
    store_event(evt);
    
    if (stats.total % 1000 == 0) {
        printf("\r[%lu events] %.1f MB", 
               stats.total,
               (stats.total * sizeof(struct event_data)) / (1024.0 * 1024.0));
        fflush(stdout);
    }
    
    if (evt->type == EVENT_SIGNAL) {
        printf("\n\n");
        printf("═══════════════════════════════════════════════════\n");
        printf("🚨 CRASH DETECTED!\n");
        printf("═══════════════════════════════════════════════════\n");
        printf("Timestamp: %lu\n", evt->timestamp);
        printf("Signal: %s\n", evt->data.sig.info);
        printf("PID: %u\n\n", evt->pid);
        printf("Use rewind:\n");
        printf("  ./timemachine rewind %lu\n", evt->timestamp);
        printf("═══════════════════════════════════════════════════\n\n");
    }
    
    return 0;
}

static const char* type_name(int type) {
    switch(type) {
        case EVENT_FUNC_ENTRY: return "FUNC_ENTRY";
        case EVENT_FUNC_EXIT:  return "FUNC_EXIT";
        case EVENT_ALLOC:      return "ALLOC";
        case EVENT_FREE:       return "FREE";
        case EVENT_SYSCALL:    return "SYSCALL";
        case EVENT_FILE_OP:    return "FILE";
        case EVENT_SIGNAL:     return "💥CRASH";
        default:               return "?";
    }
}

static void rewind_to(unsigned long target_ts) {
    unsigned long window = (unsigned long)WINDOW_SEC * 1000000000UL;
    unsigned long start = target_ts - window;
    unsigned long end = target_ts + window;
    
    printf("\n");
    printf("═══════════════════════════════════════════════════\n");
    printf("⏰ REWINDING to %lu\n", target_ts);
    printf("   Showing [-%ds to +%ds]\n", WINDOW_SEC, WINDOW_SEC);
    printf("═══════════════════════════════════════════════════\n\n");
    
    printf("%-20s %-8s %-12s %-18s %s\n",
           "Timestamp", "PID", "Type", "Address", "Details");
    printf("────────────────────────────────────────────────────────────\n");
    
    int shown = 0;
    int start_idx = store.wrapped ? store.head : 0;
    
    for (int i = 0; i < store.count && shown < 100; i++) {
        int idx = (start_idx + i) % MAX_EVENTS;
        struct event_data *e = &store.events[idx];
        
        if (e->timestamp < start || e->timestamp > end)
            continue;
        
        if (e->timestamp >= target_ts - 1000000 && 
            e->timestamp <= target_ts + 1000000) {
            printf(">>> ");
        } else {
            printf("    ");
        }
        
        printf("%-20lu %-8u %-12s ", 
               e->timestamp, e->pid, type_name(e->type));
        
        switch(e->type) {
            case EVENT_FUNC_ENTRY:
                printf("0x%-16lx args=%lx,%lx", 
                       e->data.func.func_addr,
                       e->data.func.args[0],
                       e->data.func.args[1]);
                break;
            case EVENT_ALLOC:
                if (e->data.mem.addr)
                    printf("0x%-16lx size=%lu",
                           e->data.mem.addr, e->data.mem.size);
                else
                    printf("size=%lu", e->data.mem.size);
                break;
            case EVENT_FREE:
                printf("0x%-16lx (freed)", e->data.mem.addr);
                break;
            case EVENT_SIGNAL:
                printf("%-18s sig=%d", 
                       e->data.sig.info, e->data.sig.signal);
                break;
            case EVENT_FILE_OP:
                printf("%-18s %s", "", e->data.file.path);
                break;
            default:
                printf("0x%-16lx", e->ip);
        }
        printf("\n");
        shown++;
    }
    printf("\n");
}

static void analyze_memory() {
    printf("\n");
    printf("═══════════════════════════════════════════════════\n");
    printf("🔍 MEMORY ANALYSIS\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    int unfreed = 0;
    unsigned long total_alloc = 0;
    unsigned long total_free = 0;
    
    struct {
        unsigned long addr;
        int active;
    } addrs[10000];
    int addr_count = 0;
    
    int start_idx = store.wrapped ? store.head : 0;
    
    for (int i = 0; i < store.count && addr_count < 10000; i++) {
        int idx = (start_idx + i) % MAX_EVENTS;
        struct event_data *e = &store.events[idx];
        
        if (e->type == EVENT_ALLOC && e->data.mem.addr) {
            addrs[addr_count].addr = e->data.mem.addr;
            addrs[addr_count].active = 1;
            addr_count++;
            total_alloc++;
        } else if (e->type == EVENT_FREE) {
            for (int j = 0; j < addr_count; j++) {
                if (addrs[j].addr == e->data.mem.addr && addrs[j].active) {
                    addrs[j].active = 0;
                    total_free++;
                    break;
                }
            }
        }
    }
    
    for (int i = 0; i < addr_count; i++) {
        if (addrs[i].active) unfreed++;
    }
    
    printf("Total allocations: %lu\n", total_alloc);
    printf("Total frees: %lu\n", total_free);
    printf("Potentially leaked: %d\n", unfreed);
    
    if (unfreed > 0) {
        printf("\n⚠️  Possible memory leak detected!\n");
    } else {
        printf("\n✅ No obvious leaks\n");
    }
    printf("\n");
}

static void print_stats() {
    time_t now = time(NULL);
    double dur = difftime(now, stats.start);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════\n");
    printf("📊 STATISTICS\n");
    printf("═══════════════════════════════════════════════════\n\n");
    printf("Duration: %.0fs\n", dur);
    printf("Total events: %lu (%.0f/sec)\n", 
           stats.total, stats.total / dur);
    printf("  Functions: %lu\n", stats.funcs);
    printf("  Allocs: %lu\n", stats.allocs);
    printf("  Frees: %lu\n", stats.frees);
    printf("  Crashes: %lu\n", stats.crashes);
    printf("\nMemory: %.1f MB\n",
           (stats.total * sizeof(struct event_data)) / (1024.0 * 1024.0));
    printf("\n");
}

static int setup_bpf(int pid) {
    struct rlimit rlim = {
        .rlim_cur = 512UL << 20,
        .rlim_max = 512UL << 20,
    };
    setrlimit(RLIMIT_MEMLOCK, &rlim);
    
    printf("Loading BPF...\n");
    skel = tracer_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to load BPF\n");
        return -1;
    }
    
    printf("Attaching BPF...\n");
    if (tracer_bpf__attach(skel)) {
        fprintf(stderr, "Failed to attach\n");
        return -1;
    }
    
    int fd = bpf_map__fd(skel->maps.tracked_pids);
    unsigned char val = 1;
    if (bpf_map_update_elem(fd, &pid, &val, BPF_ANY)) {
        fprintf(stderr, "Failed to add PID\n");
        return -1;
    }
    
    printf("✓ Tracking PID %d\n\n", pid);
    return 0;
}

int main(int argc, char **argv) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║          ⏰  eBPF Time Machine v1.0              ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    printf("\n");
    
    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s record <pid>\n", argv[0]);
        printf("  %s rewind <timestamp>\n", argv[0]);
        return 1;
    }
    
    signal(SIGINT, sig_handler);
    stats.start = time(NULL);
    
    if (strcmp(argv[1], "record") == 0) {
        int pid = atoi(argv[2]);
        
        printf("Recording PID %d\n", pid);
        printf("Press Ctrl+C to stop\n\n");
        
        if (setup_bpf(pid) < 0)
            return 1;
        
        struct ring_buffer *rb;
        rb = ring_buffer__new(bpf_map__fd(skel->maps.events),
                             handle_event, NULL, NULL);
        if (!rb) {
            fprintf(stderr, "Ring buffer failed\n");
            return 1;
        }
        
        printf("Recording...\n");
        
        while (running) {
            int err = ring_buffer__poll(rb, 100);
            if (err == -EINTR) break;
            if (err < 0) {
                fprintf(stderr, "Poll error: %d\n", err);
                break;
            }
        }
        
        printf("\n\nStopped\n");
        print_stats();
        analyze_memory();
        
        ring_buffer__free(rb);
        
    } else if (strcmp(argv[1], "rewind") == 0) {
        unsigned long ts = strtoull(argv[2], NULL, 10);
        
        if (store.count == 0) {
            printf("No events. Run record first.\n");
            return 1;
        }
        
        rewind_to(ts);
        
    } else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }
    
    if (skel) tracer_bpf__destroy(skel);
    return 0;
}
