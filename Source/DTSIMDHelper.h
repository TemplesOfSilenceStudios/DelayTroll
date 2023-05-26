/*
  ==============================================================================

    DTSIMDHelper.h
    Created: 8 Jul 2022 9:48:57am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

using vec4 = juce::dsp::SIMDRegister<float>;

#if defined(__i386__) || defined(__amd64__) || defined(_M_X64) || defined(_X86_) || defined(_M_IX86)
#include "sse_mathfun_extension.h"
inline vec4 operator / (const vec4 &l, const vec4 &r)
{
    return _mm_div_ps(l.value, r.value);
}

inline vec4 DT_SIMD_sqrt_ps(const vec4 &vec)
{
    return _mm_sqrt_ps(vec.value);
}

inline vec4 DT_SIMD_sin_ps(const vec4 &vec)
{
    return sin_ps(vec.value);
}

inline vec4 DT_SIMD_cos_ps(const vec4 &vec)
{
    return cos_ps(vec.value);
}

inline vec4 DT_SIMD_atan2_ps(const vec4 &y, const vec4 &x)
{
    return atan2_ps(y.value,x.value);
}


#elif defined(_M_ARM64) || defined (__arm64__) || defined (__aarch64__)
#include "neon_mathfun.h"

inline vec4 operator / (const vec4 &l, const vec4 &r)
{
    return vdivq_f32(l.value, r.value);
}
inline vec4 DT_SIMD_sqrt_ps(const vec4 &vec)
{
    return vsqrtq_f32(vec.value);
}

inline vec4 DT_SIMD_sin_ps(const vec4 &vec)
{
    return sin_ps(vec.value);
}

inline vec4 DT_SIMD_cos_ps(const vec4 &vec)
{
    return cos_ps(vec.value);
}

inline vec4 DT_SIMD_atan2_ps(const vec4 &y, const vec4 &x)
{
    vec4 result;
    
    for(int i = 0; i < 4; i++){
        result.value[i] = atan2(y.value[i],x.value[i]);
    }
    
    return result;
    
    //note for future deveopment
    //what i need to do here is make the equivilant of the following but using SIMD masks for the if then error checking stuff...
//    if (x == 0) {
//        if (y > 0) {
//            return M_PI / 2;
//        }
//        else if (y < 0) {
//            return -M_PI / 2;
//        }
//        else {
//            return 0; // undefined
//        }
//    }
//    else {
//        double r = y / x;
//        if (x > 0) {
//            return atan_ps(r);
//        }
//        else {
//            if (y >= 0) {
//                return atan_ps(r) + M_PI;
//            }
//            else {
//                return atan_ps(r) - M_PI;
//            }
//        }
//    }
}




#else
 #error "SIMD register support not implemented for this platform"
#endif
