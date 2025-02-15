#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define SAMPLE_SIZE 1000
#define WINDOW_SIZE 50

// Get high precision timestamp
uint64_t get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

// Simple moving average calculation
double calculate_moving_average(uint64_t* deltas, int start, int window) {
    double sum = 0;
    for(int i = 0; i < window; i++) {
        sum += deltas[start + i];
    }
    return sum / window;
}

// Detect patterns in timing data
void analyze_timing_patterns() {
    uint64_t timestamps[SAMPLE_SIZE];
    uint64_t deltas[SAMPLE_SIZE-1];
    double moving_averages[SAMPLE_SIZE-WINDOW_SIZE];
    
    // Collect timing samples
    for(int i = 0; i < SAMPLE_SIZE; i++) {
        timestamps[i] = get_timestamp();
        // Small busy-wait to allow system state to change
        for(volatile int j = 0; j < 1000; j++);
    }
    
    // Calculate time deltas between samples
    for(int i = 0; i < SAMPLE_SIZE-1; i++) {
        deltas[i] = timestamps[i+1] - timestamps[i];
    }
    
    // Calculate moving averages
    for(int i = 0; i < SAMPLE_SIZE-WINDOW_SIZE; i++) {
        moving_averages[i] = calculate_moving_average(deltas, i, WINDOW_SIZE);
    }
    
    // Print ASCII visualization of patterns
    printf("Timing Pattern Visualization:\n");
    for(int i = 0; i < SAMPLE_SIZE-WINDOW_SIZE; i++) {
        int bar_length = (int)((moving_averages[i] - 900) * 100);
        printf("%4d: ", i);
        for(int j = 0; j < bar_length; j++) {
            printf("*");
        }
        printf("\n");
    }
    
    // Look for repeating patterns
    printf("\nPotential Pattern Periods:\n");
    for(int period = 2; period < WINDOW_SIZE; period++) {
        double correlation = 0;
        for(int i = 0; i < SAMPLE_SIZE-WINDOW_SIZE-period; i++) {
            correlation += (moving_averages[i] - moving_averages[i+period]) *
                          (moving_averages[i] - moving_averages[i+period]);
        }
        correlation /= (SAMPLE_SIZE-WINDOW_SIZE-period);
        
        // If correlation is low, might indicate a pattern
        if(correlation < 1000) {
            printf("Possible cycle every %d samples (correlation: %.2f)\n", 
                   period, correlation);
        }
    }
}

int main() {
    printf("Analyzing system timing patterns...\n");
    analyze_timing_patterns();
    return 0;
}
