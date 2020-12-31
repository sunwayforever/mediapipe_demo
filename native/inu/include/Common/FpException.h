#pragma once

#if defined(_WIN32)
#define WINDOWS_SPECIFIC_CODE (1)
#else
// This code has no affect in Linux... FP_DIVISION is recognized, by never an
// exception is generated.
#define WINDOWS_SPECIFIC_CODE (0)
#endif

// https://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/
// Also, https://docs.oracle.com/cd/E19957-01/806-3568/ncg_handle.html

#include <float.h>

#if !WINDOWS_SPECIFIC_CODE
#include <cfenv>
#if defined(_MSC_VER)
#pragma fenv_access(on)
#else
#pragma STDC FENV_ACCESS ON
#endif
#endif

// Declare an object of this type in a scope in order to suppress
// all floating-point exceptions temporarily. The old exception
// state will be reset at the end.
class FPExceptionDisabler {
#if WINDOWS_SPECIFIC_CODE
    unsigned int mOldValues;
#else
    fexcept_t mOldValues;
#endif

    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionDisabler(const FPExceptionDisabler&);
    FPExceptionDisabler& operator=(const FPExceptionDisabler&);

   public:
    FPExceptionDisabler() {
#if WINDOWS_SPECIFIC_CODE
        // Retrieve the current state of the exception flags. This
        // must be done before changing them. _MCW_EM is a bit
        // mask representing all available exception masks.
        _controlfp_s(&mOldValues, _MCW_EM, _MCW_EM);

        // Set all of the exception flags, which suppresses FP
        // exceptions on the x87 and SSE units.
        _controlfp_s(0, _MCW_EM, _MCW_EM);
#else
        fegetexceptflag(&mOldValues, FE_ALL_EXCEPT);
        feclearexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
#endif
    }

    ~FPExceptionDisabler() {
#if WINDOWS_SPECIFIC_CODE
        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a ‘deferred crash’ as soon the exceptions are
        // enabled.
        _clearfp();

        // Reset (possibly enabling) the exception status.
        _controlfp_s(0, mOldValues, _MCW_EM);
#else
        feclearexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
        fesetexceptflag(&mOldValues, FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
#endif
    }
};

// Declare an object of this type in a scope in order to enable a
// specified set of floating-point exceptions temporarily. The old
// exception state will be reset at the end.
// This class can be nested.
class FPExceptionEnabler {
#if WINDOWS_SPECIFIC_CODE
    unsigned int mOldValues;
#else
    fexcept_t mOldValues;
#endif

    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionEnabler(const FPExceptionEnabler&);
    FPExceptionEnabler& operator=(const FPExceptionEnabler&);

   public:
    // Overflow, divide-by-zero, and invalid-operation are the FP
    // exceptions most frequently associated with bugs.
#if WINDOWS_SPECIFIC_CODE
    FPExceptionEnabler(unsigned int enableBits = _EM_OVERFLOW | _EM_ZERODIVIDE |
                                                 _EM_INVALID)
#else
    FPExceptionEnabler(fexcept_t enableBits = FE_DIVBYZERO | FE_OVERFLOW |
                                              FE_INVALID)
#endif
    {
        // Retrieve the current state of the exception flags. This
        // must be done before changing them. _MCW_EM is a bit
        // mask representing all available exception masks.
#if WINDOWS_SPECIFIC_CODE
        _controlfp_s(&mOldValues, _MCW_EM, _MCW_EM);
#else
        fegetexceptflag(&mOldValues, FE_ALL_EXCEPT);
#endif

        // Make sure no non-exception flags have been specified,
        // to avoid accidental changing of rounding modes, etc.
#if WINDOWS_SPECIFIC_CODE
        enableBits &= _MCW_EM;
#else
        enableBits &= FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID;
#endif

        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a ‘deferred crash’ as soon the exceptions are
        // enabled.
#if WINDOWS_SPECIFIC_CODE
        _clearfp();
#else
        feclearexcept(enableBits);
#endif

        // Zero out the specified bits, leaving other bits alone.
#if WINDOWS_SPECIFIC_CODE
        _controlfp_s(0, ~enableBits, enableBits);
#else
        fesetexceptflag(&mOldValues, FE_ALL_EXCEPT);
#endif
    }

    ~FPExceptionEnabler() {
#if WINDOWS_SPECIFIC_CODE
        // Reset the exception state.
        _controlfp_s(0, mOldValues, _MCW_EM);
#else
        fesetexceptflag(&mOldValues, FE_ALL_EXCEPT);
#endif
    }
};
