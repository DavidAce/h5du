
#include "prof.h"
#include "log.h"
#include <fstream>
#include <sstream>
#include <tools/class_tic_toc.h>
void tools::prof::init_profiling() {
    if(t_total != nullptr) return;
    t_total           = std::make_unique<class_tic_toc>(true, 7, "Total Time");
    t_ham_sq_psi_v1   = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version cpu1");
    t_ham_sq_psi_v2   = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version cpu2");
    t_ham_sq_psi_v3   = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version cpu3");
    t_ham_sq_psi_cuda = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version cuda");
    t_ham_sq_psi_acro = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version acro");
    t_ham_sq_psi_cute = std::make_unique<class_tic_toc>(true, 7, "H²|Ψ> version cute");
}
void tools::prof::reset_profiling() {
//    t_total.reset();
    t_ham_sq_psi_v1->reset();
    t_ham_sq_psi_v2->reset();
    t_ham_sq_psi_v3->reset();
    t_ham_sq_psi_cuda->reset();
    t_ham_sq_psi_acro->reset();
    t_ham_sq_psi_cute->reset();
}

double tools::prof::mem_usage_in_mb(std::string_view name) {
    std::ifstream filestream("/proc/self/status");
    std::string   line;
    while(std::getline(filestream, line)) {
        std::istringstream is_line(line);
        std::string        key;
        if(std::getline(is_line, key, ':')) {
            if(key == name) {
                std::string value_str;
                if(std::getline(is_line, value_str)) {
                    // Filter non-digit characters
                    value_str.erase(std::remove_if(value_str.begin(), value_str.end(), [](auto const &c) -> bool { return not std::isdigit(c); }),
                                    value_str.end());
                    // Extract the number
                    long long value = 0;
                    try {
                        std::string::size_type sz; // alias of size_t
                        value = std::stoll(value_str, &sz);
                    } catch(const std::exception &ex) {
                        tools::log->error("Could not read mem usage from /proc/self/status: Failed to parse string [{}]: {}", value_str, ex.what());
                    }
                    // Now we have the value in kb
                    return static_cast<double>(value) / 1024.0;
                }
            }
        }
    }
    return -1.0;
}

double tools::prof::mem_rss_in_mb() { return mem_usage_in_mb("VmRSS"); }
double tools::prof::mem_hwm_in_mb() { return mem_usage_in_mb("VmHWM"); }
double tools::prof::mem_vm_in_mb() { return mem_usage_in_mb("VmPeak"); }

void tools::prof::print_mem_usage() {
    tools::log->info("{:<30}{:>10.1f} MB", "Memory RSS", mem_rss_in_mb());
    tools::log->info("{:<30}{:>10.1f} MB", "Memory Peak", mem_hwm_in_mb());
    tools::log->info("{:<30}{:>10.1f} MB", "Memory Vm", mem_vm_in_mb());
}