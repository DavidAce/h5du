#pragma once
#include <memory>
#include <string_view>
class class_tic_toc;

namespace tools::prof{
    void init_profiling();
    void reset_profiling();
    inline std::unique_ptr<class_tic_toc> t_total;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_v1;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_v2;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_v3;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_cuda;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_acro;
    inline std::unique_ptr<class_tic_toc> t_ham_sq_psi_cute;



    extern void print_mem_usage();
    extern double mem_usage_in_mb(std::string_view name);
    extern double mem_rss_in_mb();
    extern double mem_hwm_in_mb();
    extern double mem_vm_in_mb();
}