#pragma once
#include "FastSIMD/InlInclude.h"

namespace Primes
{
    static constexpr int32_t X = 1619;
    static constexpr int32_t Y = 31337;
    static constexpr int32_t Z = 6971;
    static constexpr int32_t W = 1013;

    static constexpr int32_t Lookup[] = { X,Y,Z,W };
}

#define ROOT3 1.7320508075688772935f

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level < FastSIMD::Level_SSE41>* = nullptr>
FS_INLINE float32v GetGradientDotFancy( int32v hash, float32v fX, float32v fY )
{
    int32v index = FS_Convertf32_i32( FS_Converti32_f32( hash & int32v( 0x3FFFFF ) ) * float32v( 1.3333333333333333f ) );

    // Bit-4 = Choose X Y ordering
    int32v xy = (index << 29) >> 31;
    float32v a = FS_Select_f32( xy, fY, fX );
    float32v b = FS_Select_f32( xy, fX, fY );

    // Bit-1 = b flip sign
    b = FS_BitwiseXor_f32( b, FS_Casti32_f32( index << 31 ) );

    // Bit-2 = Mul a by 2 or Root3
    int32v aMul2 = (index << 30) >> 31;
    a *= FS_Select_f32( aMul2, float32v( 2 ), float32v( ROOT3 ) );
    // b zero value if a mul 2
    b = FS_BitwiseAndNot_f32( b, FS_Casti32_f32( aMul2 ) );

    // Bit-8 = Flip sign of a + b
    return FS_BitwiseXor_f32( a + b, FS_Casti32_f32( (index >> 3) << 31 ) );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_SSE41>* = nullptr>
FS_INLINE float32v GetGradientDotFancy( int32v hash, float32v fX, float32v fY )
{
    int32v index = FS_Convertf32_i32( FS_Converti32_f32( hash & int32v( 0x3FFFFF ) ) * float32v( 1.3333333333333333f ) );

    // Bit-4 = Choose X Y ordering
    int32v xy = index << 29;
    float32v a = FS_Select_f32( xy, fY, fX );
    float32v b = FS_Select_f32( xy, fX, fY );

    // Bit-1 = b flip sign
    b = FS_BitwiseXor_f32( b, FS_Casti32_f32( index << 31 ) );

    // Bit-2 = Mul a by 2 or Root3
    int32v aMul2 = index << 30;
    a *= FS_Select_f32( aMul2, float32v( 2 ), float32v( ROOT3 ) );
    // b zero value if a mul 2
    b = FS_BitwiseAndNot_f32( b, FS_Casti32_f32( aMul2 >> 31 ) );

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

#undef ROOT3

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level < FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    // ( 0, 1) (-1, 0) ( 0,-1) ( 1, 0)
    // ( 1, 1) (-1, 1) (-1,-1) ( 1,-1)

    int32v bit1 = (hash << 31);
    int32v bit2 = (hash >> 1) << 31;
    int32v bit4 = (hash << 29);

    fX = FS_BitwiseXor_f32( fX, FS_Casti32_f32( bit1 ^ bit2 ));
    fY = FS_BitwiseXor_f32( fY, FS_Casti32_f32( bit2 ));

    int32v zeroX = bit1;
    int32v zeroY = ~zeroX;

    fX = FS_BitwiseAnd_f32( fX, FS_Casti32_f32( (zeroX | bit4) >> 31 ));
    fY = FS_BitwiseAnd_f32( fY, FS_Casti32_f32( (zeroY | bit4) >> 31 ));

    return fX + fY;
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX2>* = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    // ( 0, 1) (-1, 0) ( 0,-1) ( 1, 0)
    // ( 1, 1) (-1, 1) (-1,-1) ( 1,-1)

    float32v gX = _mm256_permutevar8x32_ps( float32v( 0, -1, 0, 1, 1, -1, -1, 1 ), hash );
    float32v gY = _mm256_permutevar8x32_ps( float32v( 1, 0, -1, 0, 1, 1, -1, -1 ), hash );

    return FS_FMulAdd_f32( gX, fX, fY * gY );
}

template<typename FS = FS_SIMD_CLASS, std::enable_if_t<FS::SIMD_Level == FastSIMD::Level_AVX512> * = nullptr>
FS_INLINE float32v GetGradientDot( int32v hash, float32v fX, float32v fY )
{
    // ( 0, 1) (-1, 0) ( 0,-1) ( 1, 0)
    // ( 1, 1) (-1, 1) (-1,-1) ( 1,-1)

    float32v gX = _mm512_permutexvar_ps( hash, float32v( 0, -1, 0, 1, 1, -1, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1 ) );
    float32v gY = _mm512_permutexvar_ps( hash, float32v( 1, 0, -1, 0, 1, 1, -1, -1, 1, 0, -1, 0, 1, 1, -1, -1 ) );

    return FS_FMulAdd_f32(gX, fX, fY * gY);
}

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

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE int32v HashPrimes( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);

    hash = hash * int32v( 0x27d4eb2d );
    return (hash >> 15) ^ hash;
}

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE int32v HashPrimesHB( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);
    
    hash = hash * int32v( 0x27d4eb2d );
    return hash;
}  

template<typename FS = FS_SIMD_CLASS, typename... P>
FS_INLINE float32v GetValueCoord( int32v seed, P... primedPos )
{
    int32v hash = seed;
    hash ^= (primedPos ^ ...);
    
    hash = hash * int32v( 0x27d4eb2d );
    return FS_Converti32_f32( hash ) * float32v( (float)(1.0 / INT_MAX) );
}

template<typename FS = FS_SIMD_CLASS>
FS_INLINE float32v Lerp( float32v a, float32v b, float32v t )
{
    return FS_FMulAdd_f32( t, b - a, a );
}

template<typename FS = FS_SIMD_CLASS>
FS_INLINE float32v InterpQuintic( float32v t )
{
    return t * t * t * FS_FMulAdd_f32( t, FS_FMulAdd_f32( t, float32v( 6 ), float32v( -15 )), float32v( 10 ) );
}
