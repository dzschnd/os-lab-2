#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "cache_impl/cache_impl.h"

constexpr size_t BLOCK_SIZE = 512; 

void run_benchmark(const std::string& file_path, int iterations, bool use_cache) {
    std::vector<char> buffer(BLOCK_SIZE); 
    std::vector<double> durations; 
    durations.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        if (use_cache) {
            const int fd = lab2_open(file_path.c_str(), O_RDONLY, 0);
            if (fd < 0) {
                std::cerr << "Error opening file for IO benchmark!" << std::endl;
                return;
            }

            ssize_t bytes_read = 0;
            do {
                bytes_read = lab2_read(fd, buffer.data(), BLOCK_SIZE);
                if (bytes_read < 0) {
                    std::cerr << "Error reading from file during IO benchmark!" << std::endl;
                    lab2_close(fd);
                    return;
                }
            } while (bytes_read > 0);

            lab2_close(fd);
        } else {
            const int fd = open(file_path.c_str(), O_RDONLY | O_DIRECT);
            if (fd < 0) {
                std::cerr << "Error opening file for IO benchmark!" << std::endl;
                return;
            }

            void* aligned_buffer = nullptr;
            if (posix_memalign(&aligned_buffer, BLOCK_SIZE, BLOCK_SIZE) != 0) {
                std::cerr << "Error allocating aligned buffer!" << std::endl;
                close(fd);
                return;
            }

            ssize_t bytes_read = 0;
            do {
                bytes_read = read(fd, aligned_buffer, BLOCK_SIZE);
                if (bytes_read < 0) {
                    std::cerr << "Error reading from file during IO benchmark!" << std::endl;
                    free(aligned_buffer);
                    close(fd);
                    return;
                }
            } while (bytes_read > 0);

            free(aligned_buffer);
            close(fd);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        durations.push_back(duration.count());
    }

    const double avg_duration = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
    const double min_duration = *std::min_element(durations.begin(), durations.end());
    const double max_duration = *std::max_element(durations.begin(), durations.end());

    std::cout << "\nOverall Stats:\n";
    std::cout << "Average read latency: " << avg_duration << " seconds\n";
    std::cout << "Minimum read latency: " << min_duration << " seconds\n";
    std::cout << "Maximum read latency: " << max_duration << " seconds\n\n";

    if (use_cache) {
        std::cout << "Cache hits: " << lab2_get_cache_hits() << std::endl;
        std::cout << "Cache misses: " << lab2_get_cache_misses() << std::endl << std::endl;
        lab2_reset_cache_counters();
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <file_path> <iterations> <use_cache>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    int iterations = std::stoi(argv[2]);
    bool use_cache = std::stoi(argv[3]) != 0;

    run_benchmark(file_path, iterations, use_cache);

    return 0;
}
