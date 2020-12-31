#ifndef __PROCESS_WRAPPER__H__
#define __PROCESS_WRAPPER__H__

#ifdef _WIN32
#include <tchar.h>
#include <windows.h>
#endif

#include <string>
#include <vector>

#include "CommonUtilitiesDefs.h"

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

namespace InuCommon {
namespace ProcessUtils {
// simple helper to make to code more readable - surround string with quotes
inline std::string Q(const std::string& str) {
    std::string ret{"\""};
    ret += str;
    ret += "\"";
    return ret;
};
}  // namespace ProcessUtils

////////////////////////////////////////////////////////////////////////
/// \brief A simple wrapper for running external processes
///
/// A quick and simple solution to avoid calls to system()
/// using popen/pclose with blocking approach
///
/// For more advance needs, async support and multi threading
/// We should consider moving to Boost.process (require upgrade to boost 1.66+)
///   - https://www.boost.org/doc/libs/1_64_0/doc/html/process.html
/// Or use a dedicated library such as reproc
///   - https://github.com/DaanDeMeyer/reproc
////////////////////////////////////////////////////////////////////////

class COMMONUTILITIES_API CProcessWrapper {
   public:
    enum class EResultCodes {
        eSUCCESS,
        eERROR_FAILED_TO_GET_ENVIRONMENT,
        eERROR_FAILED_TO_SET_ENVIRONMENT,
        eERROR_FAILED_TO_ALLOCATE_MEMORY,
        eERROR_FAILED_TO_CREATE_PIPE,
        eERROR_FAILED_TO_SET_HANDLE_INFO,
        eERROR_FAILED_TO_LOCATE_INTERPRETER,
        eERROR_FAILED_TO_LOCATE_SCRIPT,
        eERROR_FAILED_TO_CREATE_CHILD_PROCESS,
        eERROR_FAILED_TO_RUN_EXTERNAL_PROCESS,
        eERROR_PROCESS_FINISED_NON_ZERO_RETURN_CODE
    };

    /// \brief Run an external system command
    EResultCodes RunCommand(const std::string& iCmd, bool iKeepOutput = false,
                            bool iAlwaysUsePopen = false);

    /// \brief Run an external executable, check that it exists
    EResultCodes RunProcess(const std::string& iProcess,
                            const std::string& iArgs, bool iKeepOutput = false);

    /// \brief Run an external script using an interpreter, check that both
    /// exists
    EResultCodes RunScript(const std::string& iInterpeter,
                           const std::string& iScript, const std::string& iArgs,
                           bool iKeepOutput = false);

    const std::vector<std::string>& GetProcessOutput() const {
        return mProcessOutput;
    }

    int GetExitCode() const { return mProcessExitCode; }

   private:
#ifdef _WIN32
    EResultCodes RunProcessWindows(const std::string& iCmd,
                                   bool iKeepOutput = false);
#endif
    EResultCodes RunProcess_popen(const std::string& iCmd,
                                  bool iKeepOutput = false);

#pragma warning(suppress : 4251)
    std::vector<std::string> mProcessOutput;
    int mProcessExitCode;
};
}  // namespace InuCommon

#endif
