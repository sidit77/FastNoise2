#pragma once

#include <immintrin.h>

#include "VecTools.h"

namespace FastSIMD
{

    struct AVX512_f32x16
    {
        FASTSIMD_INTERNAL_TYPE_SET( AVX512_f32x16, __m512 );

        constexpr FS_INLINE static uint8_t Size()
        {
            return 16;
        }

        FS_INLINE static AVX512_f32x16 Zero()
        {
            return _mm512_setzero_ps();
        }

        FS_INLINE static AVX512_f32x16 Incremented()
        {
            return _mm512_set_ps( 15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f );
        }

        FS_INLINE explicit AVX512_f32x16( float f )
        {
            *this = _mm512_set1_ps( f );
        }

        FS_INLINE explicit AVX512_f32x16( float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12, float f13, float f14, float f15 )
        {
            *this = _mm512_set_ps( f15, f14, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f0 );
        }

        FS_INLINE AVX512_f32x16& operator+=( const AVX512_f32x16& rhs )
        {
            *this = _mm512_add_ps( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_f32x16& operator-=( const AVX512_f32x16& rhs )
        {
            *this = _mm512_sub_ps( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_f32x16& operator*=( const AVX512_f32x16& rhs )
        {
            *this = _mm512_mul_ps( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_f32x16& operator/=( const AVX512_f32x16& rhs )
        {
            *this = _mm512_div_ps( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_f32x16 operator-() const
        {
#if FASTSIMD_CONFIG_GENERATE_CONSTANTS
            const __m512i minInt = _mm512_slli_epi32( _mm512_cmpeq_epi32( _mm512_setzero_si512(), _mm512_setzero_si512() ), 31 );
#else
            const __m512i minInt = _mm512_set1_epi32( -1 << 31 );
#endif
            return _mm512_xor_ps( *this, _mm512_castsi512_ps( minInt ) );
        }
    };

    FASTSIMD_INTERNAL_OPERATORS_FLOAT( AVX512_f32x16 )


    struct AVX512_i32x16
    {
        FASTSIMD_INTERNAL_TYPE_SET( AVX512_i32x16, __m512i );

        constexpr FS_INLINE static uint8_t Size()
        {
            return 16;
        }

        FS_INLINE static AVX512_i32x16 Zero()
        {
            return _mm512_setzero_si512();
        }

        FS_INLINE static AVX512_i32x16 Incremented()
        {
            return _mm512_set_epi32( 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 );
        }

        FS_INLINE explicit AVX512_i32x16( int32_t f )
        {
            *this = _mm512_set1_epi32( f );
        }

        FS_INLINE explicit AVX512_i32x16( int32_t i0, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5, int32_t i6, int32_t i7, int32_t i8, int32_t i9, int32_t i10, int32_t i11, int32_t i12, int32_t i13, int32_t i14, int32_t i15 )
        {
            *this = _mm512_set_epi32( i15, i14, i13, i12, i11, i10, i9, i8, i7, i6, i5, i4, i3, i2, i1, i0 );
        }

        FS_INLINE AVX512_i32x16& operator+=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_add_epi32( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator-=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_sub_epi32( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator*=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_mullo_epi32( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator&=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_and_si512( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator|=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_or_si512( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator^=( const AVX512_i32x16& rhs )
        {
            *this = _mm512_xor_si512( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator>>=( int32_t rhs )
        {
            *this = _mm512_srai_epi32( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16& operator<<=( int32_t rhs )
        {
            *this = _mm512_slli_epi32( *this, rhs );
            return *this;
        }

        FS_INLINE AVX512_i32x16 operator~() const
        {
#if FASTSIMD_CONFIG_GENERATE_CONSTANTS
            const __m512i neg1 = _mm512_cmpeq_epi32( _mm512_setzero_si512(), _mm512_setzero_si512() );
#else
            const __m512i neg1 = _mm512_set1_epi32( -1 );
#endif
            return _mm512_xor_si512( *this, neg1 );
        }

        FS_INLINE AVX512_i32x16 operator-() const
        {
            return _mm512_sub_epi32( _mm512_setzero_si512(), *this );
        }
    };

    FASTSIMD_INTERNAL_OPERATORS_INT( AVX512_i32x16, int32_t )

    template<eLevel LEVEL_T>
    class AVX512_T
    {
    public:
        static_assert(LEVEL_T >= Level_AVX512 && LEVEL_T <= Level_AVX512BWDQ, "Cannot create template with unsupported SIMD level");

        static const Level SIMD_Level = LEVEL_T;
        static const size_t VectorSize = 512 / 8;

        typedef AVX512_f32x16  float32v;
        typedef AVX512_i32x16  int32v;
        typedef uint16_t       mask32v;

        // Load

        FS_INLINE static float32v Load_f32( void const* p )
        {
            return _mm512_loadu_ps( p );
        }

        FS_INLINE static int32v Load_i32( void const* p )
        {
            return _mm512_loadu_si512( p );
        }

        // Store

        FS_INLINE static void Store_f32( void* p, float32v a )
        {
            _mm512_storeu_ps( p, a );
        }

        FS_INLINE static void Store_i32( void* p, int32v a )
        {
            _mm512_storeu_si512( p, a );
        }

        // Cast

        FS_INLINE static float32v Casti32_f32( int32v a )
        {
            return _mm512_castsi512_ps( a );
        }

        FS_INLINE static int32v Castf32_i32( float32v a )
        {
            return _mm512_castps_si512( a );
        }

        // Convert

        FS_INLINE static float32v Converti32_f32( int32v a )
        {
            return _mm512_cvtepi32_ps( a );
        }

        FS_INLINE static int32v Convertf32_i32( float32v a )
        {
            return _mm512_cvtps_epi32( a );
        }

        // Comparisons

        FS_INLINE static mask32v Equal_f32( float32v a, float32v b )
        {
            return _mm512_cmp_ps_mask( a, b, _CMP_EQ_OS );
        }

        FS_INLINE static mask32v GreaterThan_f32( float32v a, float32v b )
        {
            return _mm512_cmp_ps_mask( a, b, _CMP_GT_OS );
        }

        FS_INLINE static mask32v LessThan_f32( float32v a, float32v b )
        {
            return _mm512_cmp_ps_mask( a, b, _CMP_LT_OS );
        }

        FS_INLINE static mask32v GreaterEqualThan_f32( float32v a, float32v b )
        {
            return _mm512_cmp_ps_mask( a, b, _CMP_GE_OS );
        }

        FS_INLINE static mask32v LessEqualThan_f32( float32v a, float32v b )
        {
            return _mm512_cmp_ps_mask( a, b, _CMP_LE_OS );
        }

        FS_INLINE static mask32v Equal_i32( int32v a, int32v b )
        {
            return _mm512_cmpeq_epi32_mask( a, b );
        }

        FS_INLINE static mask32v GreaterThan_i32( int32v a, int32v b )
        {
            return _mm512_cmpgt_epi32_mask( a, b );
        }

        FS_INLINE static mask32v LessThan_i32( int32v a, int32v b )
        {
            return _mm512_cmpgt_epi32_mask( b, a );
        }

        // Select

        FS_INLINE static float32v Select_f32( mask32v m, float32v a, float32v b )
        {
            return _mm512_mask_blend_ps( m, b, a );
        }

        FS_INLINE static int32v Select_i32( mask32v m, int32v a, int32v b )
        {
            return _mm512_mask_blend_epi32( m, b, a );
        }

        // Min, Max

        FS_INLINE static float32v Min_f32( float32v a, float32v b )
        {
            return _mm512_min_ps( a, b );
        }

        FS_INLINE static float32v Max_f32( float32v a, float32v b )
        {
            return _mm512_max_ps( a, b );
        }

        FS_INLINE static int32v Min_i32( int32v a, int32v b )
        {
            return _mm512_min_epi32( a, b );
        }

        FS_INLINE static int32v Max_i32( int32v a, int32v b )
        {
            return _mm512_max_epi32( a, b );
        }

        // Bitwise

        FS_INLINE static float32v BitwiseAnd_f32( float32v a, float32v b )
        {
            return _mm512_and_ps( a, b );
        }

        FS_INLINE static float32v BitwiseOr_f32( float32v a, float32v b )
        {
            return _mm512_or_ps( a, b );
        }

        FS_INLINE static float32v BitwiseXor_f32( float32v a, float32v b )
        {
            return _mm512_xor_ps( a, b );
        }

        FS_INLINE static float32v BitwiseNot_f32( float32v a )
        {
            const __m512i neg1 = _mm512_set1_epi32( -1 );

            return _mm512_xor_ps( a, _mm512_castsi512_ps( neg1 ) );
        }

        FS_INLINE static float32v BitwiseAndNot_f32( float32v a, float32v b )
        {
            return _mm512_andnot_ps( b, a );
        }

        FS_INLINE static int32v BitwiseAndNot_i32( int32v a, int32v b )
        {
            return _mm512_andnot_si512( b, a );
        }

        // Abs

        FS_INLINE static float32v Abs_f32( float32v a )
        {
            return _mm512_abs_ps( a );
        }

        FS_INLINE static int32v Abs_i32( int32v a )
        {
            return _mm512_abs_epi32( a );
        }

        // Float math

        FS_INLINE static float32v Sqrt_f32( float32v a )
        {
            return _mm512_sqrt_ps( a );
        }

        FS_INLINE static float32v InvSqrt_f32( float32v a )
        {
            return _mm512_rsqrt14_ps( a );
        }

        FS_INLINE static float32v Reciprocal_f32( float32v a )
        {
            return _mm512_rcp14_ps( a );
        }

        // Floor, Ceil, Round

        FS_INLINE static float32v Floor_f32( float32v a )
        {
            return _mm512_roundscale_ps( a, _MM_FROUND_TO_NEG_INF );
        }

        FS_INLINE static float32v Ceil_f32( float32v a )
        {
            return _mm512_ceil_ps( a );
        }

        FS_INLINE static float32v Round_f32( float32v a )
        {
            return _mm512_roundscale_ps( a, _MM_FROUND_TO_NEAREST_INT );
        }

        //Mask

        FS_INLINE static int32v Mask_i32( int32v a, mask32v m )
        {
            return _mm512_maskz_mov_epi32( m, a );
        }

        FS_INLINE static float32v Mask_f32( float32v a, mask32v m )
        {
            return _mm512_maskz_mov_ps( m, a );
        }

        FS_INLINE static int32v NMask_i32( int32v a, mask32v m )
        {
            return _mm512_maskz_mov_epi32( ~m, a );
        }

        FS_INLINE static float32v NMask_f32( float32v a, mask32v m )
        {
            return _mm512_maskz_mov_ps( ~m, a );
        }
    };

#if FASTSIMD_COMPILE_AVX512
    typedef AVX512_T<Level_AVX512> AVX512;
#endif

#if FASTSIMD_USE_FMA
    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v MaskedAdd_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::float32v c )
    {
        return _mm512_fmadd_ps( a, b, c );
    }
#endif

    // Masked float

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v MaskedAdd_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_add_ps( a, m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v MaskedSub_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_sub_ps( a, m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v MaskedMul_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_mul_ps( a, m, a, b );
    }

    // Masked int32

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v MaskedAdd_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_add_epi32( a, m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v MaskedSub_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_sub_epi32( a, m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v MaskedMul_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_mul_epi32( a, m, a, b );
    }

    // NMasked float

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v NMaskedAdd_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_add_ps( a, ~m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v NMaskedSub_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_sub_ps( a, ~m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::float32v NMaskedMul_f32( typename AVX512_T<L>::float32v a, typename AVX512_T<L>::float32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_mul_ps( a, ~m, a, b );
    }

    // NMasked int32

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v NMaskedAdd_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_add_epi32( a, ~m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v NMaskedSub_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_sub_epi32( a, ~m, a, b );
    }

    template<eLevel L>
    FS_INLINE typename AVX512_T<L>::int32v NMaskedMul_i32( typename AVX512_T<L>::int32v a, typename AVX512_T<L>::int32v b, typename AVX512_T<L>::mask32v m )
    {
        return _mm512_mask_mul_epi32( a, ~m, a, b );
    }
    
}