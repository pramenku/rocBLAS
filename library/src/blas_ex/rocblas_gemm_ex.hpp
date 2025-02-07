/* ************************************************************************
 * Copyright 2016-2019 Advanced Micro Devices, Inc.
 * ************************************************************************ */
#ifndef __ROCBLAS_GEMM_EX_HPP
#define __ROCBLAS_GEMM_EX_HPP

#include "Tensile.h"
#include "TensileTypes.h"
#include "gemm.hpp"
#include "handle.h"
#include "logging.h"
#include "rocblas.h"
#include "utility.h"

/////////////////
// Device Side //
/////////////////
template <typename To>
static rocblas_status device_strided_batched_matrix_copy(const To*      src,
                                                         rocblas_stride ld_src,
                                                         rocblas_stride stride_src,
                                                         To*            dst,
                                                         rocblas_stride ld_dst,
                                                         rocblas_stride stride_dst,
                                                         rocblas_int    n1,
                                                         rocblas_int    n2,
                                                         rocblas_int    batch_count)
{
    if(src == dst && ld_src == ld_dst && stride_src == stride_dst)
        return rocblas_status_success; // no copy if src matrix == dst matrix

    if(n1 == ld_src && n1 == ld_dst && stride_src == n2 * ld_src && stride_dst == n2 * ld_dst)
    {
        // src and dst batch matrices are contiguous, use single copy
        RETURN_IF_HIP_ERROR(
            hipMemcpy(dst, src, sizeof(To) * n1 * n2 * batch_count, hipMemcpyDeviceToDevice));
    }
    else if(n1 == ld_src && n1 == ld_dst)
    {
        // individual matrices in batch matrix are contiguous, one copy for each matrix
        for(size_t i3 = 0; i3 < batch_count; i3++)
            RETURN_IF_HIP_ERROR(hipMemcpy(dst + i3 * stride_dst,
                                          src + i3 * stride_src,
                                          sizeof(To) * n1 * n2,
                                          hipMemcpyDeviceToDevice));
    }
    else
    {
        // individual matrices not contiguous, one copy for each contiguous column
        for(int i3 = 0; i3 < batch_count; i3++)
            for(int i2 = 0; i2 < n2; i2++)
                RETURN_IF_HIP_ERROR(hipMemcpy(dst + i2 * ld_dst + i3 * stride_dst,
                                              src + i2 * ld_src + i3 * stride_src,
                                              sizeof(To) * n1,
                                              hipMemcpyDeviceToDevice));
    }
    return rocblas_status_success;
}

//------------------------------------------------------------------------------
#define TENSILE_IN_ARGS(Ti, To, Tc)                                                         \
    To *dataD, const To *dataC, const Ti *dataA, const Ti *dataB, Tc alpha, Tc beta,        \
        unsigned strideD1J, unsigned strideD2K, unsigned strideC1J, unsigned strideC2K,     \
        unsigned strideA1L, unsigned strideA2K, unsigned strideB1J, unsigned strideB2K,     \
        unsigned sizeI, unsigned sizeJ, unsigned sizeK, unsigned sizeL, hipStream_t stream, \
        unsigned numInputEvents, void *dummy1, void *dummy2

#define TENSILE_OUT_ARGS                                                                   \
    dataD, dataC, dataA, dataB, alpha, beta, strideD1J, strideD2K, strideC1J, strideC2K,   \
        strideA1L, strideA2K, strideB1J, strideB2K, sizeI, sizeJ, sizeK, sizeL, stream, 0, \
        nullptr, nullptr

// Ti is typename for input data, To is typename for output data, Tc is typename for compute
template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensileStatusFailure;
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensileStatusFailure;
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Alik_Bljk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensileStatusFailure;
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensileStatusFailure;
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Ailk_BjlkC_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensile_Cijk_Ailk_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_AlikC_Bljk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensile_Cijk_Alik_Bljk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_Alik_BjlkC_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensile_Cijk_Alik_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_AlikC_Bjlk_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensile_Cijk_Alik_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus tensile_Cijk_AlikC_BjlkC_B(TENSILE_IN_ARGS(Ti, To, Tc))
{
    return tensile_Cijk_Alik_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
}

//----- typename_data = tensile_bfloat16 ----- typename_compute = float -----------------------
template <>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B<tensile_bfloat16, tensile_bfloat16, float>(
    TENSILE_IN_ARGS(tensile_bfloat16, tensile_bfloat16, float))
{
    return tensile_Cijk_Ailk_Bljk_BBH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B<tensile_bfloat16, tensile_bfloat16, float>(
    TENSILE_IN_ARGS(tensile_bfloat16, tensile_bfloat16, float))
{
    return tensile_Cijk_Ailk_Bjlk_BBH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bljk_B<tensile_bfloat16, tensile_bfloat16, float>(
    TENSILE_IN_ARGS(tensile_bfloat16, tensile_bfloat16, float))
{
    return tensile_Cijk_Alik_Bljk_BBH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B<tensile_bfloat16, tensile_bfloat16, float>(
    TENSILE_IN_ARGS(tensile_bfloat16, tensile_bfloat16, float))
{
    return tensile_Cijk_Alik_Bjlk_BBH(TENSILE_OUT_ARGS);
}

//----- typename_data = TensileHalf ----- typename_compute = float---------------------------
#define TENSILE_OUT_ARGS_HALF                                                                      \
    dataD, dataC, dataA, dataB, alpha_half, beta_half, strideD1J, strideD2K, strideC1J, strideC2K, \
        strideA1L, strideA2K, strideB1J, strideB2K, sizeI, sizeJ, sizeK, sizeL, stream, 0,         \
        nullptr, nullptr

template <>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B<TensileHalf, TensileHalf, float>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, float))
{
    //TODO: alpha and beta need to have precision equal to compute type, not data type
    TensileHalf alpha_half(alpha);
    TensileHalf beta_half(beta);
    return tensile_Cijk_Ailk_Bljk_HBH(TENSILE_OUT_ARGS_HALF);
}
template <>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B<TensileHalf, TensileHalf, float>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, float))
{
    //TODO: alpha and beta need to have precision equal to compute type, not data type
    TensileHalf alpha_half(alpha);
    TensileHalf beta_half(beta);
    return tensile_Cijk_Ailk_Bjlk_HBH(TENSILE_OUT_ARGS_HALF);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bljk_B<TensileHalf, TensileHalf, float>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, float))
{
    //TODO: alpha and beta need to have precision equal to compute type, not data type
    TensileHalf alpha_half(alpha);
    TensileHalf beta_half(beta);
    return tensile_Cijk_Alik_Bljk_HBH(TENSILE_OUT_ARGS_HALF);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B<TensileHalf, TensileHalf, float>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, float))
{
    //TODO: alpha and beta need to have precision equal to compute type, not data type
    TensileHalf alpha_half(alpha);
    TensileHalf beta_half(beta);
    return tensile_Cijk_Alik_Bjlk_HBH(TENSILE_OUT_ARGS_HALF);
}
#undef TENSILE_OUT_ARGS_HALF

//----- typename_data = TensileHalf ----- typename_compute = TensileHalf ---------------------
template <>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B<TensileHalf, TensileHalf, TensileHalf>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, TensileHalf))
{
    return tensile_Cijk_Ailk_Bljk_HB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B<TensileHalf, TensileHalf, TensileHalf>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, TensileHalf))
{
    return tensile_Cijk_Ailk_Bjlk_HB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bljk_B<TensileHalf, TensileHalf, TensileHalf>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, TensileHalf))
{
    return tensile_Cijk_Alik_Bljk_HB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B<TensileHalf, TensileHalf, TensileHalf>(
    TENSILE_IN_ARGS(TensileHalf, TensileHalf, TensileHalf))
{
    return tensile_Cijk_Alik_Bjlk_HB(TENSILE_OUT_ARGS);
}

//----- typename_data = float ----------- typename_compute = float ---------------------------
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bljk_B<float, float, float>(TENSILE_IN_ARGS(float, float, float))
{
    return tensile_Cijk_Ailk_Bljk_SB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bjlk_B<float, float, float>(TENSILE_IN_ARGS(float, float, float))
{
    return tensile_Cijk_Ailk_Bjlk_SB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bljk_B<float, float, float>(TENSILE_IN_ARGS(float, float, float))
{
    return tensile_Cijk_Alik_Bljk_SB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bjlk_B<float, float, float>(TENSILE_IN_ARGS(float, float, float))
{
    return tensile_Cijk_Alik_Bjlk_SB(TENSILE_OUT_ARGS);
}

//----- typename_data = double ---------- typename_compute = double --------------------------
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bljk_B<double, double, double>(TENSILE_IN_ARGS(double, double, double))
{
    return tensile_Cijk_Ailk_Bljk_DB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bjlk_B<double, double, double>(TENSILE_IN_ARGS(double, double, double))
{
    return tensile_Cijk_Ailk_Bjlk_DB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bljk_B<double, double, double>(TENSILE_IN_ARGS(double, double, double))
{
    return tensile_Cijk_Alik_Bljk_DB(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bjlk_B<double, double, double>(TENSILE_IN_ARGS(double, double, double))
{
    return tensile_Cijk_Alik_Bjlk_DB(TENSILE_OUT_ARGS);
}

//----- typename_input = int8 ---- typename_output = int ------ typename_compute = int ------------------
template <>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B<TensileInt8x4, TensileInt32, TensileInt32>(
    TENSILE_IN_ARGS(TensileInt8x4, TensileInt32, TensileInt32))
{
    return tensile_Cijk_Ailk_Bljk_4xi8BH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B<TensileInt8x4, TensileInt32, TensileInt32>(
    TENSILE_IN_ARGS(TensileInt8x4, TensileInt32, TensileInt32))
{
    return tensile_Cijk_Ailk_Bjlk_4xi8BH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bljk_B<TensileInt8x4, TensileInt32, TensileInt32>(
    TENSILE_IN_ARGS(TensileInt8x4, TensileInt32, TensileInt32))
{
    return tensile_Cijk_Alik_Bljk_4xi8BH(TENSILE_OUT_ARGS);
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B<TensileInt8x4, TensileInt32, TensileInt32>(
    TENSILE_IN_ARGS(TensileInt8x4, TensileInt32, TensileInt32))
{
    return tensile_Cijk_Alik_Bjlk_4xi8BH(TENSILE_OUT_ARGS);
}

//----- typename_data=rocblas_float_complex ---------- typename_compute = rocblas_float_complex --------------------------
#define TENSILE_COMPLEX_OUT_ARGS(Ti, To, Tc)                                             \
    (To*)dataD, (const To*)dataC, (const Ti*)dataA, (const Ti*)dataB, *((Tc*)&alpha),    \
        *((Tc*)&beta), strideD1J, strideD2K, strideC1J, strideC2K, strideA1L, strideA2K, \
        strideB1J, strideB2K, sizeI, sizeJ, sizeK, sizeL, stream, 0, nullptr, nullptr

static_assert(std::is_standard_layout<TensileComplexFloat>{},
              "TensileComplexFloat is not a standard layout type, and thus is "
              "incompatible with C.");

static_assert(std::is_trivial<TensileComplexFloat>{},
              "TensileComplexFloat is not a trivial type, and thus is "
              "incompatible with C.");

static_assert(sizeof(rocblas_float_complex) == sizeof(TensileComplexFloat),
              "TensileComplexFloat does not match public rocblas_float_complex");
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bljk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Ailk_Bljk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_Ailk_Bjlk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Ailk_Bjlk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bljk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Alik_Bljk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_Bjlk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Alik_Bjlk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
// Complex Conjugate
template <>
inline TensileStatus
    tensile_Cijk_Ailk_BjlkC_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Ailk_BjlkC_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_AlikC_Bljk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_AlikC_Bljk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_Alik_BjlkC_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_Alik_BjlkC_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_AlikC_Bjlk_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_AlikC_Bjlk_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}
template <>
inline TensileStatus
    tensile_Cijk_AlikC_BjlkC_B<rocblas_float_complex, rocblas_float_complex, rocblas_float_complex>(
        TENSILE_IN_ARGS(rocblas_float_complex, rocblas_float_complex, rocblas_float_complex))
{
    return tensile_Cijk_AlikC_BjlkC_CB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexFloat, TensileComplexFloat, TensileComplexFloat));
}

//----- typename_data = rocblas_double_complex ---------- typename_compute = rocblas_double_complex --------------------------
static_assert(std::is_standard_layout<TensileComplexDouble>{},
              "TensileComplexDouble is not a standard layout type, and thus is "
              "incompatible with C.");

static_assert(std::is_trivial<TensileComplexDouble>{},
              "TensileComplexDouble is not a trivial type, and thus is "
              "incompatible with C.");

static_assert(sizeof(rocblas_double_complex) == sizeof(TensileComplexDouble),
              "TensileComplexDouble does not match rocblas_double_complex");
template <>
inline TensileStatus tensile_Cijk_Ailk_Bljk_B<rocblas_double_complex,
                                              rocblas_double_complex,
                                              rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Ailk_Bljk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_Ailk_Bjlk_B<rocblas_double_complex,
                                              rocblas_double_complex,
                                              rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Ailk_Bjlk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bljk_B<rocblas_double_complex,
                                              rocblas_double_complex,
                                              rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Alik_Bljk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_Alik_Bjlk_B<rocblas_double_complex,
                                              rocblas_double_complex,
                                              rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Alik_Bjlk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
// Complex Conjugate
template <>
inline TensileStatus tensile_Cijk_Ailk_BjlkC_B<rocblas_double_complex,
                                               rocblas_double_complex,
                                               rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Ailk_BjlkC_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_AlikC_Bljk_B<rocblas_double_complex,
                                               rocblas_double_complex,
                                               rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_AlikC_Bljk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_Alik_BjlkC_B<rocblas_double_complex,
                                               rocblas_double_complex,
                                               rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_Alik_BjlkC_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_AlikC_Bjlk_B<rocblas_double_complex,
                                               rocblas_double_complex,
                                               rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_AlikC_Bjlk_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}
template <>
inline TensileStatus tensile_Cijk_AlikC_BjlkC_B<rocblas_double_complex,
                                                rocblas_double_complex,
                                                rocblas_double_complex>(
    TENSILE_IN_ARGS(rocblas_double_complex, rocblas_double_complex, rocblas_double_complex))
{
    return tensile_Cijk_AlikC_BjlkC_ZB(
        TENSILE_COMPLEX_OUT_ARGS(TensileComplexDouble, TensileComplexDouble, TensileComplexDouble));
}

template <typename Ti, typename To, typename Tc>
inline TensileStatus call_tensile_ex(To*            dataD,
                                     const To*      dataC,
                                     const Ti*      dataA,
                                     const Ti*      dataB,
                                     Tc             alpha,
                                     Tc             beta,
                                     unsigned       strideD1J,
                                     unsigned       strideD2K,
                                     unsigned       strideC1J,
                                     unsigned       strideC2K,
                                     unsigned       strideA1L,
                                     unsigned       strideA2K,
                                     unsigned       strideB1J,
                                     unsigned       strideB2K,
                                     unsigned       sizeI,
                                     unsigned       sizeJ,
                                     unsigned       sizeK,
                                     unsigned       sizeL,
                                     hipStream_t    stream,
                                     transpose_mode transposeMode)
{
    switch(transposeMode)
    {
    case NN:
        return tensile_Cijk_Ailk_Bljk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case NT:
        return tensile_Cijk_Ailk_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case NC:
        return tensile_Cijk_Ailk_BjlkC_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case TN:
        return tensile_Cijk_Alik_Bljk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case CN:
        return tensile_Cijk_AlikC_Bljk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case TT:
        return tensile_Cijk_Alik_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case TC:
        return tensile_Cijk_Alik_BjlkC_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case CT:
        return tensile_Cijk_AlikC_Bjlk_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    case CC:
        return tensile_Cijk_AlikC_BjlkC_B<Ti, To, Tc>(TENSILE_OUT_ARGS);
    }

    return tensileStatusFailure;
}

#undef TENSILE_COMPLEX_OUT_ARGS
#undef TENSILE_IN_ARGS
#undef TENSILE_OUT_ARGS

//------------------------------------------------------------------------------

///////////////
// Host Side //
///////////////
template <typename Ti, typename To, typename Tc>
rocblas_status gemm_ex_handle_transpose(rocblas_handle    handle,
                                        rocblas_operation trans_a,
                                        rocblas_operation trans_b,
                                        rocblas_int       m,
                                        rocblas_int       n,
                                        rocblas_int       k,
                                        const Tc*         alpha,
                                        const Ti*         a[],
                                        size_t            offset_a,
                                        rocblas_int       lda,
                                        rocblas_stride    stride_a,
                                        const Ti*         b[],
                                        size_t            offset_b,
                                        rocblas_int       ldb,
                                        rocblas_stride    stride_b,
                                        const Tc*         beta,
                                        const To*         c[],
                                        size_t            offset_c,
                                        rocblas_int       ldc,
                                        rocblas_stride    stride_c,
                                        To*               d[],
                                        size_t            offset_d,
                                        rocblas_int       ldd,
                                        rocblas_stride    stride_d,
                                        rocblas_int       batch_count)
{
    // BATCHED VERSION
    // Host arrays of device pointers.
    Ti* hostA[batch_count];
    Ti* hostB[batch_count];
    To* hostC[batch_count];
    To* hostD[batch_count];

    RETURN_IF_HIP_ERROR(hipMemcpy(hostA, a, sizeof(hostA), hipMemcpyDeviceToHost));
    RETURN_IF_HIP_ERROR(hipMemcpy(hostB, b, sizeof(hostB), hipMemcpyDeviceToHost));
    RETURN_IF_HIP_ERROR(hipMemcpy(hostC, c, sizeof(hostC), hipMemcpyDeviceToHost));
    RETURN_IF_HIP_ERROR(hipMemcpy(hostD, d, sizeof(hostD), hipMemcpyDeviceToHost));

    stride_a = rocblas_stride(lda) * (trans_a == rocblas_operation_none ? k : m);
    stride_b = rocblas_stride(ldb) * (trans_b == rocblas_operation_none ? n : k);
    stride_c = rocblas_stride(ldc) * n;
    stride_d = rocblas_stride(ldd) * n;

    rocblas_status status = rocblas_status_internal_error;
    for(rocblas_int bi = 0; bi < batch_count; bi++)
    {
        // Tensile does not support batched gemm_ex yet, must do naive version
        status = gemm_ex_handle_transpose(handle,
                                          trans_a,
                                          trans_b,
                                          m,
                                          n,
                                          k,
                                          alpha,
                                          hostA[bi],
                                          offset_a,
                                          lda,
                                          stride_a,
                                          hostB[bi],
                                          offset_b,
                                          ldb,
                                          stride_b,
                                          beta,
                                          hostC[bi],
                                          offset_c,
                                          ldc,
                                          stride_c,
                                          hostD[bi],
                                          offset_d,
                                          ldd,
                                          stride_d,
                                          1);
        if(status != rocblas_status_success)
            break;
    }
    return status;
}

template <typename Ti, typename To, typename Tc>
rocblas_status gemm_ex_handle_transpose(rocblas_handle    handle,
                                        rocblas_operation trans_a,
                                        rocblas_operation trans_b,
                                        rocblas_int       m,
                                        rocblas_int       n,
                                        rocblas_int       k,
                                        const Tc*         alpha,
                                        const Ti*         a,
                                        size_t            offset_a,
                                        rocblas_int       lda,
                                        rocblas_stride    stride_a,
                                        const Ti*         b,
                                        size_t            offset_b,
                                        rocblas_int       ldb,
                                        rocblas_stride    stride_b,
                                        const Tc*         beta,
                                        const To*         c,
                                        size_t            offset_c,
                                        rocblas_int       ldc,
                                        rocblas_stride    stride_c,
                                        To*               d,
                                        size_t            offset_d,
                                        rocblas_int       ldd,
                                        rocblas_stride    stride_d,
                                        rocblas_int       batch_count)
{
    a += offset_a;
    b += offset_b;
    c += offset_c;
    d += offset_d;

    TensileStatus  t_status;
    rocblas_status rb_status;

    static const bool arch_lt906 = handle->device_arch_id() < 906;
    const To*         c_in;
    unsigned          ldi, stride_i;

    if(!arch_lt906 && (std::is_same<Ti, float>{} || std::is_same<Ti, double>{})
       && ((ldc >= ldd && stride_c >= stride_d && m == ldd)
           || (ldc == ldd && stride_c == stride_d)))
    {
        c_in     = c;
        ldi      = ldc;
        stride_i = stride_c;
    }
    else
    {
        device_strided_batched_matrix_copy(c, ldc, stride_c, d, ldd, stride_d, m, n, batch_count);
        c_in     = d;
        ldi      = ldd;
        stride_i = stride_d;
    }

    t_status = call_tensile_ex<Ti, To, Tc>(d,
                                           c_in,
                                           a,
                                           b,
                                           *alpha,
                                           *beta,
                                           ldd,
                                           stride_d,
                                           ldi,
                                           stride_i,
                                           lda,
                                           stride_a,
                                           ldb,
                                           stride_b,
                                           m,
                                           n,
                                           batch_count,
                                           k,
                                           handle->rocblas_stream,
                                           GetTransposeMode(trans_a, trans_b));

    rb_status = (t_status == tensileStatusSuccess) ? rocblas_status_success
                                                   : rocblas_status_internal_error;
    return rb_status;
}

template <bool BATCHED, typename Ti, typename To, typename Tc>
rocblas_status gemm_ex_typecasting(rocblas_handle    handle,
                                   rocblas_operation trans_a,
                                   rocblas_operation trans_b,
                                   rocblas_int       m,
                                   rocblas_int       n,
                                   rocblas_int       k,
                                   const void*       alpha,
                                   const void*       a,
                                   rocblas_int       offsetAin,
                                   rocblas_int       lda,
                                   rocblas_stride    stride_a,
                                   const void*       b,
                                   rocblas_int       offsetBin,
                                   rocblas_int       ldb,
                                   rocblas_stride    stride_b,
                                   const void*       beta,
                                   const void*       c,
                                   rocblas_int       offsetCin,
                                   rocblas_int       ldc,
                                   rocblas_stride    stride_c,
                                   void*             d,
                                   rocblas_int       offsetDin,
                                   rocblas_int       ldd,
                                   rocblas_stride    stride_d,
                                   rocblas_int       batch_count)
{
    Tc alpha_h, beta_h;

    if(rocblas_pointer_mode_device == handle->pointer_mode)
    {
        RETURN_IF_HIP_ERROR(hipMemcpy(&alpha_h, alpha, sizeof(alpha_h), hipMemcpyDeviceToHost));
        RETURN_IF_HIP_ERROR(hipMemcpy(&beta_h, beta, sizeof(beta_h), hipMemcpyDeviceToHost));
    }
    else
    {
        alpha_h = *reinterpret_cast<const Tc*>(alpha);
        beta_h  = *reinterpret_cast<const Tc*>(beta);
    }

    // check alignment of pointers before casting
    if(BATCHED)
    {
        if(!isAligned(a, sizeof(Ti*)) || !isAligned(b, sizeof(Ti*)) || !isAligned(c, sizeof(To*))
           || !isAligned(d, sizeof(To*)))
            return rocblas_status_invalid_size;

        // Pass alpha and beta as simple array (stride of 1)
        // since Tensile does not have gemm_batched, we will have to iterate
        // over batches either way
        return gemm_ex_handle_transpose(handle,
                                        trans_a,
                                        trans_b,
                                        unsigned(m),
                                        unsigned(n),
                                        unsigned(k),
                                        &alpha_h,
                                        (const Ti**)a,
                                        unsigned(offsetAin),
                                        unsigned(lda),
                                        unsigned(stride_a),
                                        (const Ti**)b,
                                        unsigned(offsetBin),
                                        unsigned(ldb),
                                        unsigned(stride_b),
                                        &beta_h,
                                        (const To**)c,
                                        unsigned(offsetCin),
                                        unsigned(ldc),
                                        unsigned(stride_c),
                                        (To**)d,
                                        unsigned(offsetDin),
                                        unsigned(ldd),
                                        unsigned(stride_d),
                                        unsigned(batch_count));
    }
    else
    {
        if(!isAligned(a, sizeof(Ti)) || !isAligned(b, sizeof(Ti)) || !isAligned(c, sizeof(To))
           || !isAligned(d, sizeof(To)))
            return rocblas_status_invalid_size;

        return gemm_ex_handle_transpose(handle,
                                        trans_a,
                                        trans_b,
                                        unsigned(m),
                                        unsigned(n),
                                        unsigned(k),
                                        &alpha_h,
                                        (const Ti*)a,
                                        unsigned(offsetAin),
                                        unsigned(lda),
                                        unsigned(stride_a),
                                        (const Ti*)b,
                                        unsigned(offsetBin),
                                        unsigned(ldb),
                                        unsigned(stride_b),
                                        &beta_h,
                                        (const To*)c,
                                        unsigned(offsetCin),
                                        unsigned(ldc),
                                        unsigned(stride_c),
                                        (To*)d,
                                        unsigned(offsetDin),
                                        unsigned(ldd),
                                        unsigned(stride_d),
                                        unsigned(batch_count));
    }
}

template <bool BATCHED>
rocblas_status rocblas_gemm_ex_template(rocblas_handle    handle,
                                        rocblas_operation trans_a,
                                        rocblas_operation trans_b,
                                        rocblas_int       m,
                                        rocblas_int       n,
                                        rocblas_int       k,
                                        const void*       alpha,
                                        const void*       a,
                                        rocblas_datatype  a_type,
                                        rocblas_int       offsetAin,
                                        rocblas_int       lda,
                                        rocblas_stride    stride_a,
                                        const void*       b,
                                        rocblas_datatype  b_type,
                                        rocblas_int       offsetBin,
                                        rocblas_int       ldb,
                                        rocblas_stride    stride_b,
                                        const void*       beta,
                                        const void*       c,
                                        rocblas_datatype  c_type,
                                        rocblas_int       offsetCin,
                                        rocblas_int       ldc,
                                        rocblas_stride    stride_c,
                                        void*             d,
                                        rocblas_datatype  d_type,
                                        rocblas_int       offsetDin,
                                        rocblas_int       ldd,
                                        rocblas_stride    stride_d,
                                        rocblas_int       batch_count,
                                        rocblas_datatype  compute_type)
{
    // Note: k==0 is not an early exit, since C still needs to be multiplied by beta
    if(!m || !n || !batch_count)
        return rocblas_status_success;

    if(BATCHED)
    {
        stride_a = rocblas_stride(lda) * (trans_a == rocblas_operation_none ? k : m);
        stride_b = rocblas_stride(ldb) * (trans_b == rocblas_operation_none ? n : k);
        stride_c = rocblas_stride(ldc) * n;
        stride_d = rocblas_stride(ldd) * n;
    }

    rocblas_status rb_status = rocblas_status_internal_error;

#define EX_TYPECASTING_PARM                                                                   \
    handle, trans_a, trans_b, m, n, k, alpha, a, offsetAin, lda, stride_a, b, offsetBin, ldb, \
        stride_b, beta, c, offsetCin, ldc, stride_c, d, offsetDin, ldd, stride_d, batch_count

    if(a_type == rocblas_datatype_f64_r && b_type == rocblas_datatype_f64_r
       && c_type == rocblas_datatype_f64_r && d_type == rocblas_datatype_f64_r
       && compute_type == rocblas_datatype_f64_r)
    {
        rb_status = gemm_ex_typecasting<BATCHED, double, double, double>(EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_f32_r && b_type == rocblas_datatype_f32_r
            && c_type == rocblas_datatype_f32_r && d_type == rocblas_datatype_f32_r
            && compute_type == rocblas_datatype_f32_r)
    {
        rb_status = gemm_ex_typecasting<BATCHED, float, float, float>(EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_f16_r && b_type == rocblas_datatype_f16_r
            && c_type == rocblas_datatype_f16_r && d_type == rocblas_datatype_f16_r
            && compute_type == rocblas_datatype_f16_r)
    {
        rb_status = gemm_ex_typecasting<BATCHED, rocblas_half, rocblas_half, rocblas_half>(
            EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_f16_r && b_type == rocblas_datatype_f16_r
            && c_type == rocblas_datatype_f16_r && d_type == rocblas_datatype_f16_r
            && compute_type == rocblas_datatype_f32_r)
    {
        rb_status
            = gemm_ex_typecasting<BATCHED, rocblas_half, rocblas_half, float>(EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_bf16_r && b_type == rocblas_datatype_bf16_r
            && c_type == rocblas_datatype_bf16_r && d_type == rocblas_datatype_bf16_r
            && compute_type == rocblas_datatype_f32_r)
    {
        rb_status = gemm_ex_typecasting<BATCHED, tensile_bfloat16, tensile_bfloat16, float>(
            EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_i8_r && b_type == rocblas_datatype_i8_r
            && c_type == rocblas_datatype_i32_r && d_type == rocblas_datatype_i32_r
            && compute_type == rocblas_datatype_i32_r)
    {
        // For now, K must be a multiple of 4
        if(k % 4 != 0 || ((trans_a == rocblas_operation_transpose) && (lda % 4 != 0))
           || ((trans_b == rocblas_operation_none) && (ldb % 4 != 0)) || stride_a % 4 != 0
           || stride_b % 4 != 0)
        {
            rb_status = rocblas_status_invalid_size;
        }
        else
        {
            // adjust by 4 for Tensile
            lda = (trans_a == rocblas_operation_none) ? lda : lda / 4;
            ldb = (trans_b == rocblas_operation_none) ? ldb / 4 : ldb;
            k   = k / 4;
            if(!BATCHED)
            {
                stride_a = stride_a / 4;
                stride_b = stride_b / 4;
            }

            rb_status = gemm_ex_typecasting<BATCHED, TensileInt8x4, TensileInt32, TensileInt32>(
                EX_TYPECASTING_PARM);
        }
    }
    else if(a_type == rocblas_datatype_f32_c && b_type == rocblas_datatype_f32_c
            && c_type == rocblas_datatype_f32_c && d_type == rocblas_datatype_f32_c
            && compute_type == rocblas_datatype_f32_c)
    {
        rb_status = gemm_ex_typecasting<BATCHED,
                                        rocblas_float_complex,
                                        rocblas_float_complex,
                                        rocblas_float_complex>(EX_TYPECASTING_PARM);
    }
    else if(a_type == rocblas_datatype_f64_c && b_type == rocblas_datatype_f64_c
            && c_type == rocblas_datatype_f64_c && d_type == rocblas_datatype_f64_c
            && compute_type == rocblas_datatype_f64_c)
    {
        rb_status = gemm_ex_typecasting<BATCHED,
                                        rocblas_double_complex,
                                        rocblas_double_complex,
                                        rocblas_double_complex>(EX_TYPECASTING_PARM);
    }
    else
    {
        rb_status = rocblas_status_not_implemented;
    }
#undef EX_TYPECASTING_PARM

    return rb_status;
}

#endif
