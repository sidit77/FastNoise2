#pragma once
#include "FastSIMD/InlInclude.h"

namespace Primes
{
    static constexpr int32_t X = 501125321;
    static constexpr int32_t Y = 1136930381;
    static constexpr int32_t Z = 1720413743;
    static constexpr int32_t W = 1066037191;

    static constexpr int32_t Lookup[] = { X,Y,Z,W };
}

#define ROOT2 1.4142135623730950488f
#define ROOT3 1.7320508075688772935f

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level < FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDotFancy( int32v hash, float32v fX, float32v fY )
{
    int32v index = FS_Convertf32_i32( FS_Converti32_f32( hash & int32v( 0x3FFFFF ) ) * float32v( 1.3333333333333333f ) );

    // Bit-4 = Choose X Y ordering
    mask32v xy = index << 29;

    if constexpr( FS::SIMD_Level < FastSIMD::Level_SSE41 )
    {
        xy = xy >> 31;
    }

    float32v a = FS_Select_f32( xy, fY, fX );
    float32v b = FS_Select_f32( xy, fX, fY );

    // Bit-1 = b flip sign
    b = FS_BitwiseXor_f32( b, FS_Casti32_f32( index << 31 ) );

    // Bit-2 = Mul a by 2 or Root3
    mask32v aMul2 = (index << 30) >> 31;
    a *= FS_Select_f32( aMul2, float32v( 2 ), float32v( ROOT3 ) );
    // b zero value if a mul 2
    b = FS_NMask_f32( b, aMul2 );

    // Bit-8 = Flip sign of a + b
    return FS_BitwiseXor_f32( a + b, FS_Casti32_f32( (index >> 3) << 31 ) );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDotFancy( int32v hash, float32v fX, float32v fY )
{
    int32v index = FS_Convertf32_i32( FS_Converti32_f32( hash & int32v( 0x3FFFFF ) ) * float32v( 1.3333333333333333f ) );

    float32v gX = _mm256_permutevar8x32_ps( float32v( ROOT3, ROOT3, 2, 2, 1, -1, 0, 0 ), index );
    float32v gY = _mm256_permutevar8x32_ps( float32v( 1, -1, 0, 0, ROOT3, ROOT3, 2, 2 ), index );

    // Bit-8 = Flip sign of a + b
    return FS_BitwiseXor_f32( FS_FMulAdd_f32( gX, fX, fY * gY ), FS_Casti32_f32( (index >> 3) << 31 ) );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<(FS::SIMD_Level == FastSIMD::Level_AVX512)>* = nullptr>
FS_INLINE float32v GetGradientDotFancy( int32v hash, float32v fX, float32v fY )
{
    int32v index = FS_Convertf32_i32( FS_Converti32_f32( hash & int32v( 0x3FFFFF ) ) * float32v( 1.3333333333333333f ) );

    float32v gX = _mm512_permutexvar_ps( index, float32v( ROOT3, ROOT3, 2, 2, 1, -1, 0, 0, -ROOT3, -ROOT3, -2, -2, -1, 1, 0, 0 ) );
    float32v gY = _mm512_permutexvar_ps( index, float32v( 1, -1, 0, 0, ROOT3, ROOT3, 2, 2, -1, 1, 0, 0, -ROOT3, -ROOT3, -2, -2 ) );

    return FS_FMulAdd_f32( gX, fX, fY * gY );
}


template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level < FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    // ( 1+R2, 1 ) ( -1-R2, 1 ) ( 1+R2, -1 ) ( -1-R2, -1 )
    // ( 1, 1+R2 ) ( 1, -1-R2 ) ( -1, 1+R2 ) ( -1, -1-R2 )

    int32v  bit1 = (hash << 31);
    int32v  bit2 = (hash >> 1) << 31;
    mask32v bit4 = (hash << 29);

    if constexpr( FS::SIMD_Level < FastSIMD::Level_SSE41 )
    {
        bit4 = bit4 >> 31;
    }

    fX = FS_BitwiseXor_f32( fX, FS_Casti32_f32( bit1 ) );
    fY = FS_BitwiseXor_f32( fY, FS_Casti32_f32( bit2 ) );
    
    float32v a = FS_Select_f32( bit4, fY, fX );
    float32v b = FS_Select_f32( bit4, fX, fY );
    
    return FS_FMulAdd_f32( float32v( 1.0f + ROOT2 ), a, b );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    float32v gX = _mm256_permutevar8x32_ps( float32v( 1 + ROOT2, -1 - ROOT2, 1 + ROOT2, -1 - ROOT2, 1, -1, 1, -1 ), hash );
    float32v gY = _mm256_permutevar8x32_ps( float32v( 1, 1, -1, -1, 1 + ROOT2, 1 + ROOT2, -1 - ROOT2, -1 - ROOT2 ), hash );

    return FS_FMulAdd_f32( gX, fX, fY * gY );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX512> * = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    float32v gX = _mm512_permutexvar_ps( hash, float32v( 1 + ROOT2, -1 - ROOT2, 1 + ROOT2, -1 - ROOT2, 1, -1, 1, -1, 1 + ROOT2, -1 - ROOT2, 1 + ROOT2, -1 - ROOT2, 1, -1, 1, -1 ) );
    float32v gY = _mm512_permutexvar_ps( hash, float32v( 1, 1, -1, -1, 1 + ROOT2, 1 + ROOT2, -1 - ROOT2, -1 - ROOT2, 1, 1, -1, -1, 1 + ROOT2, 1 + ROOT2, -1 - ROOT2, -1 - ROOT2 ) );

    return FS_FMulAdd_f32( gX, fX, fY * gY );
}

#undef ROOT2
#undef ROOT3

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level != FastSIMD::Level_AVX512 > * = nullptr >
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY, float32v fZ )
{
    int32v hasha13 = hash & int32v( 13 );

    //if h < 8 then x, else y
    mask32v l8 = FS_LessThan_i32( hasha13, int32v( 8 ));
    float32v u = FS_Select_f32( l8, fX, fY );

    //if h < 4 then y else if h is 12 or 14 then x else z
    mask32v l4 = FS_LessThan_i32( hasha13, int32v( 2 ) );
    mask32v h12o14 = FS_Equal_i32( hasha13, int32v( 12 ) );
    float32v v = FS_Select_f32( l4, fY, FS_Select_f32( h12o14, fX, fZ ) );

    //if h1 then -u else u
    //if h2 then -v else v
    float32v h1 = FS_Casti32_f32( hash << 31 );
    float32v h2 = FS_Casti32_f32( (hash & int32v( 2 )) << 30 );
    //then add them
    return FS_BitwiseXor_f32( u, h1 ) + FS_BitwiseXor_f32( v, h2 );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX512>* = nullptr>
FS_INLINE float32v GetGradientDot(int32v hash, float32v fX, float32v fY, float32v fZ)
{
    float32v gX = _mm512_permutexvar_ps( hash, float32v( 1, -1, 1, -1, 1, -1, 1, -1, 0, 0, 0, 0, 1, 0, -1, 0 ) );
    float32v gY = _mm512_permutexvar_ps( hash, float32v( 1, 1, -1, -1, 0, 0, 0, 0, 1, -1, 1, -1, 1, -1, 1, -1 ) );
    float32v gZ = _mm512_permutexvar_ps( hash, float32v( 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, -1, 0, 1, 0, -1 ) );

    return FS_FMulAdd_f32( gX, fX, FS_FMulAdd_f32( fY, gY, fZ * gZ ));
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level != FastSIMD::Level_AVX512>* = nullptr >
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY, float32v fZ, float32v fW )
{
    int32v p = hash & int32v( 3 << 3 );

    float32v a = FS_Select_f32( FS_GreaterThan_i32( p, int32v( 0 ) ), fX, fY );
    float32v b;
    if constexpr( FS::SIMD_Level == FastSIMD::Level_SSE2 )
    {
        b = FS_Select_f32( FS_GreaterThan_i32( p, int32v( 1 << 3 ) ), fY, fZ );        
    }
    else
    {
        b = FS_Select_f32( hash << 27, fY, fZ );
    }
    float32v c = FS_Select_f32( FS_GreaterThan_i32( p, int32v( 2 << 3 ) ), fZ, fW );

    float32v aSign = FS_Casti32_f32( hash << 31 );
    float32v bSign = FS_Casti32_f32( (hash << 30) & int32v( 0x80000000 ) );
    float32v cSign = FS_Casti32_f32( (hash << 29) & int32v( 0x80000000 ) );

    return FS_BitwiseXor_f32( a, aSign ) + FS_BitwiseXor_f32( b, bSign ) + FS_BitwiseXor_f32( c, cSign );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX512>* = nullptr>
FS_INLINE float32v GetGradientDot(int32v hash, float32v fX, float32v fY, float32v fZ, float32v fW )
{
    float32v gX = _mm512_permutex2var_ps( float32v( 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 1, -1, 1, -1, 1, -1 ), hash, float32v( 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1 ) );
    float32v gY = _mm512_permutex2var_ps( float32v( 1, -1, 1, -1, 1, -1, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0 ), hash, float32v( 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1 ) );
    float32v gZ = _mm512_permutex2var_ps( float32v( 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1 ), hash, float32v( 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, -1, -1, -1, -1 ) );
    float32v gW = _mm512_permutex2var_ps( float32v( 1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1, 1, -1, -1, -1, -1 ), hash, float32v( 1, 1, 1, 1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0 ) );

    return FS_FMulAdd_f32( gX, fX, FS_FMulAdd_f32( fY, gY, FS_FMulAdd_f32( fZ, gZ, fW * gW ) ));
}

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE int32v HashPrimes( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);

    hash *= int32v( 0x27d4eb2d );
    return (hash >> 15) ^ hash;
}

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE int32v HashPrimesHB( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);
    
    hash *= int32v( 0x27d4eb2d );
    return hash;
}  

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE float32v GetValueCoord( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);
    
    hash *= hash * int32v( 0x27d4eb2d );
    return FS_Converti32_f32( hash ) * float32v( 1.0f / (float)INT_MAX );
}

template<typename FS = FS_SIMD_CLASS>
FS_INLINE float32v Lerp( float32v a, float32v b, float32v t )
{
    return FS_FMulAdd_f32( t, b - a, a );
}

template<typename FS = FS_SIMD_CLASS>
FS_INLINE float32v InterpHermite( float32v t )
{
    return t * t * FS_FNMulAdd_f32( t, float32v( 2 ), float32v( 3 ));
}

template<typename FS = FS_SIMD_CLASS>
FS_INLINE float32v InterpQuintic( float32v t )
{
    return t * t * t * FS_FMulAdd_f32( t, FS_FMulAdd_f32( t, float32v( 6 ), float32v( -15 )), float32v( 10 ) );
}

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE float32v CalcDistance( FastNoise::DistanceFunction distFunc, float32v dX, P... d )
{
    switch( distFunc )
    {
    default:
    case DistanceFunction::Euclidean:
    {
        float32v distSqr = dX * dX;
        ((distSqr = FS_FMulAdd_f32( d, d, distSqr )), ...);

        return FS_InvSqrt_f32( distSqr ) * distSqr;
    }

    case DistanceFunction::EuclideanSquared:
    {
        float32v distSqr = dX * dX;
        ((distSqr = FS_FMulAdd_f32( d, d, distSqr )), ...);

        return distSqr;
    }

    case DistanceFunction::Manhattan:
    {
        float32v dist = FS_Abs_f32( dX );
        dist += (FS_Abs_f32( d ) + ...);

        return dist;
    }

    case DistanceFunction::Hybrid:
    {
        float32v both = FS_FMulAdd_f32( dX, dX, FS_Abs_f32( dX ) );
        ((both += FS_FMulAdd_f32( d, d, FS_Abs_f32( d ) )), ...);

        return both;
    }
    }
}
