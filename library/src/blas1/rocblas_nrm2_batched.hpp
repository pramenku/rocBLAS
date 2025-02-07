/* ************************************************************************
 * Copyright 2016-2019 Advanced Micro Devices, Inc.
 * ************************************************************************ */
#pragma once

#include "rocblas_nrm2.hpp"

template <rocblas_int NB, typename U, typename To>
rocblas_status rocblas_nrm2_batched_template(rocblas_handle handle,
                                             rocblas_int    n,
                                             U              x,
                                             rocblas_int    shiftx,
                                             rocblas_int    incx,
                                             rocblas_int    batch_count,
                                             To*            workspace,
                                             To*            results)
{
    static constexpr bool           isbatched = true;
    static constexpr rocblas_stride stridex_0 = 0;
    return rocblas_reduction_template<NB,
                                      isbatched,
                                      rocblas_fetch_nrm2<To>,
                                      rocblas_reduce_sum,
                                      rocblas_finalize_nrm2>(
        handle, n, x, shiftx, incx, stridex_0, batch_count, results, workspace);
}
