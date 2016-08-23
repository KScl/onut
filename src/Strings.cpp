// Onut
#include <onut/Strings.h>

// STL
#include <algorithm>
#include <codecvt>
#include <regex>

namespace onut
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> g_stringConverter;

    std::wstring utf8ToUtf16(const std::string& utf8)
    {
        return g_stringConverter.from_bytes(utf8);
    }

    std::string utf16ToUtf8(const std::wstring& utf16)
    {
        return g_stringConverter.to_bytes(utf16);
    }

    std::vector<std::string> splitString(const std::string& in_string, char in_delimiter, bool in_removeEmptyElements)
    {
        std::vector<std::string> elems;
        unsigned int start = 0;
        unsigned int end = 0;
        for (; end < in_string.length(); ++end)
        {
            if (in_string[end] == in_delimiter)
            {
                if (end - start || !in_removeEmptyElements)
                {
                    elems.push_back(in_string.substr(start, end - start));
                }
                start = end + 1;
            }
        }
        if (!in_removeEmptyElements && start == in_string.length())
        {
            elems.push_back("");
        }
        if (start < in_string.length())
        {
            if (end - start || !in_removeEmptyElements)
            {
                elems.push_back(in_string.substr(start, end - start));
            }
        }
        return elems;
    }

    std::string removeChars(const std::string& str, const std::string& charsToRemove)
    {
        auto ret = str;
        for (decltype(charsToRemove.size()) i = 0; i < charsToRemove.size(); ++i)
        {
            ret.erase(remove(ret.begin(), ret.end(), charsToRemove[i]), ret.end());
        }
        return ret;
    }

    std::string toUpper(const std::string& str)
    {
        auto ret = str;
        std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
        return ret;
    }

    std::string toLower(const std::string& str)
    {
        auto ret = str;
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
        return ret;
    }

    void stripOutComments(std::string& source)
    {
        // Group
        std::smatch m1;
        std::smatch m2;
        size_t offset = 0;
        while (std::regex_search(source.cbegin() + offset, source.cend(), m1, std::regex("\\/\\*")))
        {
            offset += m1.position();
            if (std::regex_search(source.cbegin() + offset, source.cend(), m2, std::regex("\\*\\/")))
            {
                source.erase(source.cbegin() + offset,
                             source.cbegin() + offset + m2.position() + m2.length());
            }
            else break;
        }

        // Single
        offset = 0;
        while (std::regex_search(source.cbegin() + offset, source.cend(), m1, std::regex("\\/\\/.*")))
        {
            offset += m1.position();
            source.erase(source.cbegin() + offset,
                         source.cbegin() + offset + m1.length());
        }
    }

    std::string stripOutComments(const std::string& source)
    {
        auto ret = source;
        stripOutComments(ret);
        return std::move(ret);
    }

    void replace(std::string& source, const std::string& reg, const std::string& substitution)
    {
        source = std::regex_replace(source, std::regex(reg), substitution);
    }

    std::string replace(const std::string& source, const std::string& reg, const std::string& substitution)
    {
        auto ret = source;
        replace(ret, reg, substitution);
        return std::move(ret);
    }
}
