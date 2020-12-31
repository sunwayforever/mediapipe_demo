
#ifndef __INIFILESTORAGE_H__
#define __INIFILESTORAGE_H__

#include "CommonUtilitiesDefs.h"
#include "OSUtilities.h"

#include <string>
#include <vector>
#include <type_traits>

#include <boost/property_tree/ptree.hpp>
#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


namespace InuCommon
{
    ///////////////////////////////////////////////////////////////////////
    // Class: CIniFile
    // Role: INI storage for persistent data
    // Inherits: IStorage
    // Responsibilities:
    //          1. Implements IStorage interface
    //          2. Knows how to read/write from INI file
    // Comments: This is a "ConcreteImplementor" of Bridge design pattern
    ///////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CIniFile
    {

    public:

        template <typename T, typename Y>
            bool GetValue(const std::string& iKey, T* oValue, Y& ioExpectedElements);

        template <typename T, typename Y>
            bool GetValue(const std::string& iKey, T* oValue, const Y& ioExpectedElements);

        template <typename T>
            bool GetValue(const std::string& iKey, T* oValue);

        virtual ~CIniFile() {}

        bool Load(const std::string& iFileName);
        bool Save(const std::string& iFileName);

        template<class T>
        bool GetValue(const std::string& iKey, std::vector<T>& oValue);

        template<class T>
        bool SetValue(const std::string& iKey, const std::vector<T>& iValue);

        template<class T>
        bool SetValue(const std::string& iKey, const T* iValue, int iNumberOfElements);

        bool SetValue(const std::string& iKey, const std::string* iValue, int iNumberOfElements);

    protected:

        bool GetValue(const std::string& iKey, std::string& oValue);
        bool SetValue(const std::string& iKey, const std::string& iValue);
        //for trees with hierarchy
        bool SetValue(const std::string& iSection, const std::string& iKey, const std::string& iValue);
        bool GetValue(const std::string& iSection, const std::string& iKey, std::string& oValue);

    private:

#ifdef _MSC_VER
#pragma warning(suppress : 4251)
        boost::property_tree::ptree mCurrentFileContents;
#else
        boost::property_tree::ptree mCurrentFileContents;
#endif

        template<class T>
        bool Tokenizer(std::vector<T>& oResult, const std::string& iString, const std::string& iSeperator);
    };

    template<class T>
    bool CIniFile::Tokenizer(std::vector<T>& oResult, const std::string& iString, const std::string& iSeperator)
    {
        // Split the line into it cells and push it to the output buffer

        boost::char_separator<char> sep(iSeperator.c_str(), "", boost::keep_empty_tokens);
        boost::tokenizer<boost::char_separator<char>> tokensBoost(iString, sep);


        BOOST_FOREACH(const std::string& t, tokensBoost)
        {
            try
            {
                oResult.push_back(boost::lexical_cast<T>(t));
            }
            catch (...)
            {
                return false;
            }

        }
        return true;
    }

    /// \brief Get a single from INI file
    ///
    /// Gets a single values from the INI file
    ///
    /// \param[in] iKey         INI file key for values to be retreived
    /// \param[out] oValue      output
    /// \return success (true) or fail (false)
    template <typename T>
        bool CIniFile::GetValue(const std::string& iKey, T* oValue)
    {
            return CIniFile::GetValue(iKey, oValue, 1);
    }

    /// \brief Get constant number of values from INI file
    ///
    /// Gets exectly the number of values specificed from the INI file. As ioExpectedElements is
    /// constant, and cannot be assigned to, it is an error (function returns false and does not
    /// modify oValue) unless the exect number of parametrs is avaliable.
    ///
    /// \param[in] iKey         INI file key for values to be retreived
    /// \param[out] oValue      output
    /// \param[in] ioExpectedElements
    /// \return success (true) or fail (false)
    template <typename T, typename Y>
        bool CIniFile::GetValue(const std::string& iKey, T* oValue, const Y& ioExpectedElements)
    {
            if (ioExpectedElements <= 0)
            {
                return false;
            }

            Y _ioExpectedElements = ioExpectedElements;

            T* val = new T(ioExpectedElements);

            if (val == nullptr)
            {
                return false;
            }

            bool rc = CIniFile::GetValue(iKey, val, _ioExpectedElements);

            if (!rc || _ioExpectedElements != ioExpectedElements)
            {
                free(val);
                return false;
            }

            for (int i = 0; i < ioExpectedElements; i++)
            {
                oValue[i] = val[i];
            }

            free(val);

            return rc;
    }

        /// \brief Get values from INI file
        ///
        /// Gets values from the INI file. Number of value must not exceed ioExpectedElements.
        ///
        /// \param[in] iKey         INI file key for values to be retreived
        /// \param[out] oValue      output
        /// \param[in,out] ioExpectedElements   Max number of value. Upon successful return, hold the number of actual values retreived.
        /// \return success (true) or fail (false)
    template <typename T, typename Y>
        bool CIniFile::GetValue(const std::string& iKey, T* oValue, Y& ioExpectedElements)
    {
        if (ioExpectedElements <= 0)
        {
            return false;
        }

        bool ret(true);
        std::string tmp;
        if (GetValue(iKey, tmp) == false || tmp.empty())
        {
            ret = false;
        }
        else
        {
            try
            {
                std::vector<std::string> allValues;

                Tokenizer<std::string>(allValues, tmp, ", :;");

                if (allValues.size() > (size_t)ioExpectedElements)
                {
                    ret = false;
                }
                else
                {
                    ioExpectedElements = int(allValues.size());

                    for (const auto& elem : allValues)
                    {
                        *oValue++ = boost::lexical_cast<T>(elem);
                    }
                }
            }
            catch (...)
            {
                ret = false;
            }
        }

        return ret;
    }

    template<typename T>
    bool CIniFile::GetValue(const std::string& iKey, std::vector<T>& oValue)
    {
        int numberOfElements = int(oValue.size());
        bool ret = GetValue<T>(iKey, &oValue[0], numberOfElements);
        if (ret)
        {
            oValue.resize(numberOfElements);
        }

        return ret;
    }

    template<typename T>
    bool CIniFile::SetValue(const std::string& iKey, const T* iValue, int iNumberOfElements)
    {
        std::string value;
        for (int i=0; i<iNumberOfElements; i++)
        {
            if (i > 0)
            {
                value += ",";
            }
            value += std::to_string(iValue[i]);
        }
        return SetValue(iKey, value);
    }

    template<typename T>
    bool CIniFile::SetValue(const std::string& iKey, const std::vector<T>& iValue)
    {
        return SetValue<T>(iKey, &iValue[0], iValue.size());
    }

    inline bool CIniFile::SetValue(const std::string& iKey, const std::string* iValue, int iNumberOfElements)
    {
        std::string value;
        for (int i = 0; i<iNumberOfElements; i++)
        {
            if (i > 0)
            {
                value += ",";
            }
            value += iValue[i];
        }
        return SetValue(iKey, value);
    }
}


#endif
