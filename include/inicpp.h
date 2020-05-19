/*
 * inicpp.h
 *
 * Created on: 26 Dec 2015
 *     Author: Fabian Meyer
 *    License: MIT
 */

#ifndef INICPP_H_
#define INICPP_H_

#include <fstream>
#include <sstream>
#include <map>
//#include <stdexcept>

//#include <limits>
//#include <cmath>

namespace ini
{
    /**
     * Represents a value in a key value pair of an ini-file. 
     */
    class IniField
    {
    private:
        /**
	 * Represents the value as a string which may be empty. 
	 */
        std::string value_;
        /**
	 * Whether the last cast conversion of value_ 
	 * e.g. into <c>int</>. 
	 * This is insignificant but false if there was no conversion yet. 
	 * This is used by #as() to throw an exception 
	 * and by #orDefault(T) to set the defalt value. 
	 *
	 * @see failedLastConversion()
	 */
        mutable bool failedLastConversion_;
        mutable std::string typeLastConversion;

    public:
      // TBC: needed? 
         IniField() : value_(), failedLastConversion_(false)
         {}

        IniField(const std::string &value)
	  : value_(value), failedLastConversion_(false)
        {}
        IniField(const IniField &field)
	  : value_(field.value_),
	    failedLastConversion_(field.failedLastConversion_)
        {}

        ~IniField()
        {}

        // template<typename T>
        // T as() const
        // {
	//     T result = static_cast<T>(*this);
	//     if (failedLastConversion_)
	//       throw std::invalid_argument("field is no " + typeLastConversion);

        //     return result;
        // }

        template<typename T>
        T asUnconditional() const
        {
	    return static_cast<T>(*this);
	}

#ifndef THROW_PREVENTED
        template<typename T>
        T as() const
        {
	    T result = asUnconditional<T>();
	    if (failedLastConversion_)
	      throw std::invalid_argument
		("field '" + value_ + "' is no " + typeLastConversion);
            return result;
        }
#endif

        template<typename T>
        T orDefault(T defaultValue)
        {
	    T result = static_cast<T>(*this);
	    return failedLastConversion_ ? defaultValue : result;
 	}
      
        // template<typename T>
        // T &operator ||(T defaultValue)
        // {
	//     T result = static_cast<T>(*this);
	//     return failedLastConversion_ ? defaultValue : result;
 	// }

        const std::string toString() const
        {
	    return value_;
	}

        bool failedLastConversion()
        {
	  return failedLastConversion_;
	}

      
        /**********************************************************************
         * Assignment Operators
         *********************************************************************/


        IniField &operator=(const char *value)
        {
            value_ = std::string(value);
            return *this;
        }

        IniField &operator=(const std::string &value)
        {
            value_ = value;
            return *this;
        }

        IniField &operator=(const int value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const unsigned int value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const long int value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const unsigned long int value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const double value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const float value)
        {
            std::stringstream ss;
            ss << value;
            value_ = ss.str();
            return *this;
        }

        IniField &operator=(const bool value)
        {
	    value_ = value ? "true" : "false";
            return *this;
        }

        /**********************************************************************
         * Cast Operators
         *********************************************************************/

        const char* castToCString() const 
        {
	    failedLastConversion_ = false;
            return value_.c_str();
	}
      
        explicit operator const char *() const
        {
	    typeLastConversion = "char*";
	    const char* result = castToCString();
 	    return result;
        }

        const std::string castToString() const 
        {
	    failedLastConversion_ = false;
            return value_;
	}
      
        explicit operator std::string() const
        {
	    typeLastConversion = "std::string";
	    const std::string result = castToString();
            return result;
        }

 	// strtol has a no throw guarantee 
        long int castToLongIntCheckFail() const 
        {
            char *endptr;
            long int result = std::strtol(value_.c_str(), &endptr, 0);
	    failedLastConversion_ = *endptr != '\0' || value_.empty();
	    return result;
 	}

        /**
	 * Returns the value of this field, i.e. value_ 
	 * as a long int if possible; else returns 0. 
	 * As a side effect sets #typeLastConversion. 
	 */
        explicit operator long int() const
        {
	    typeLastConversion = "long int";
 	    long int result = castToLongIntCheckFail();
	    return result;
        }
      
        explicit operator int() const
        {
	    typeLastConversion = "int";
	    long int result = castToLongIntCheckFail();

	    if (result > std::numeric_limits<int>::max())
	      result = std::numeric_limits<int>::max();
	    else if (result < std::numeric_limits<int>::min())
	      result = std::numeric_limits<int>::min();
	    return (int)result;
        }

	// strtoul has a no throw guarantee 
        unsigned long int castToUnsignedLongIntCheckFail() const 
        {
 	    char *endptr;
	    // CAUTION: this delivers a value even if string starts with '-'
	    unsigned long int result = std::strtoul(value_.c_str(), &endptr, 0);
	    failedLastConversion_ =
	      *endptr != '\0' || value_.empty() || value_[0] == '-';
	    return result;
 	}

        explicit operator unsigned long int() const
        {
	    typeLastConversion = "unsigned long int";
	    unsigned long int result = castToUnsignedLongIntCheckFail();
	    return result;
        }

        explicit operator unsigned int() const
        {
	    typeLastConversion = "unsigned int";
	    unsigned long int result = castToUnsignedLongIntCheckFail();

	    if (result > std::numeric_limits<unsigned int>::max())
	      result = std::numeric_limits<unsigned int>::max();
	    return result;
        }

 	// strtod has a no throw guarantee 
        double castToDoubleCheckFail() const 
        {
	    char *endptr;
	    double result = std::strtod(value_.c_str(), &endptr);
	    failedLastConversion_ = *endptr != '\0' || value_.empty();
	    return result;
	}

        explicit operator double() const
        {
	    typeLastConversion = "double";
	    double result = castToDoubleCheckFail();
	    return result;
        }

	explicit operator float() const
        {
	    typeLastConversion = "float";
	    float result = (float)castToDoubleCheckFail();
	    return result;
	}



        explicit operator bool() const
        {
	    typeLastConversion = "bool";
            std::string str(value_);
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	    failedLastConversion_ = true;
	    bool result = false;
            if (str == "true")
	    {
	        failedLastConversion_ = false;
                result = true;
	    }
            if (str == "false")
	    {
 	        failedLastConversion_ = false;
	    }

	    return result;
        }
    };


    class IniSection : public std::map<std::string, IniField>
    {
    public:
        IniSection()
        {}
        ~IniSection()
        {}
    };

    /**
     * Enumeration of error codes which may occur during decoding an ini-file, 
     * whether as a stream, a string or a file. 
     */
    enum DecodeErrorCode
    {
        NO_FAILURE = 0,
	SECTION_NOT_CLOSED,
	SECTION_NAME_EMPTY,
	SECTION_TEXT_AFTER,
	FIELD_WITHOUT_SECTION,
	FIELD_WITHOUT_SEPARATOR
    };


  class IniFile : public std::map<std::string, IniSection>
    {
   protected:
        char fieldSep_;
        char comment_;

        static void trim(std::string &str)
        {
            size_t startpos = str.find_first_not_of(" \t");
            if (std::string::npos != startpos)
            {
                size_t endpos = str.find_last_not_of(" \t");
                str = str.substr(startpos, endpos - startpos + 1);
            }
            else
                str = "";
        }

    public:

	    
	class DecodeResult
	{
	    
	public:
	    /**
	     * This is DecodeErrorCode#NO_FAILURE if all ok so far. 
	     */
	    DecodeErrorCode errorCode;
	    /**
	     * This is -1 if no failure occurred yet. 
	     */
	    uint lineNumber;
	    /*
	     * This is <c>null</c> if reading from a stream without file. 
	     */
	    //std::string fileName;

	    // DecodeResult(DecodeErrorCode errorCode,
	    // 		 uint lineNumber,
	    // 		 std::string fileName)
	    // {
	    // 	this->errorCode = errorCode;
	    // 	this->lineNumber = lineNumber;
	    // 	this->fileName = fileName;
	    // }
	    
	    DecodeResult(DecodeErrorCode errorCode,
			 uint lineNumber) //:  DecodeResult(errorCode, lineNumber, (const char*)NULL)
	    {
	     	this->errorCode = errorCode;
	     	this->lineNumber = lineNumber;
	    }

	    DecodeResult() : DecodeResult(NO_FAILURE, -1)//, (const char*)NULL)
	    {
	    }

	    bool isOk()
	    {
	      return this->errorCode == NO_FAILURE;
	    }

	};
	
        IniFile() : IniFile('=', '#')
        {}

        IniFile(const char fieldSep, const char comment)
            : fieldSep_(fieldSep), comment_(comment)
        {}

        ~IniFile()
        {}


# ifndef THROW_PREVENTED
        IniFile(const std::string &filename,
            const char fieldSep = '=',
            const char comment = '#')
	  : IniFile(fieldSep, comment)
        {
	    load(filename);
        }

        IniFile(std::istream &is,
            const char fieldSep = '=',
            const char comment = '#')
	  : IniFile(fieldSep, comment)
        {
           decode(is);
         }
#endif


      

        void setFieldSep(const char sep)
        {
            fieldSep_ = sep;
        }

        void setCommentChar(const char comment)
        {
            comment_ = comment;
        }

        DecodeResult tryDecode(std::istream &is)
	{
	    clear();
            int lineNo = 0;
	    IniSection *currentSection = NULL;
            // iterate file by line
            while (!is.eof() && !is.fail())
            {
               std::string line;
                std::getline(is, line, '\n');
                trim(line);
                ++lineNo;

                // skip if line is empty
                if(line.size() == 0)
                    continue;

		// skip if line is a comment
                if(line[0] == comment_)
                    continue;
                if(line[0] == '[')
                {
                    // line is a section
                    // check if the section is also closed on same line
                    std::size_t pos = line.find("]");
                    if(pos == std::string::npos)
			return *new DecodeResult(SECTION_NOT_CLOSED, lineNo);
                    // check if the section name is empty
                    if(pos == 1)
			return *new DecodeResult(SECTION_NAME_EMPTY, lineNo);
                    // check if there is a newline following closing bracket
                    if(pos + 1 != line.length())
			return *new DecodeResult(SECTION_TEXT_AFTER, lineNo);

                    // retrieve section name
                    std::string secName = line.substr(1, pos - 1);
                    currentSection = &((*this)[secName]);
                }
                else
                {
                    // line is a field definition
                    // check if section was already opened
                    if(currentSection == NULL)
			return *new DecodeResult(FIELD_WITHOUT_SECTION, lineNo);

                    // find key value separator
                    std::size_t pos = line.find(fieldSep_);
                    if(pos == std::string::npos)
		      return *new DecodeResult(FIELD_WITHOUT_SEPARATOR, lineNo);

                    // retrieve field name and value
                    std::string name = line.substr(0, pos);
                    trim(name);
                    std::string value = line.substr(pos + 1, std::string::npos);
                    trim(value);
                    (*currentSection)[name] = value;
		}
	    }

	    // signifies success
	    return *new DecodeResult();
	}	

	DecodeResult tryDecode(const std::string &content)
	{
            std::istringstream ss(content);
            return tryDecode(ss);
 	}


        DecodeResult tryLoad(const std::string &fileName)
        {
            std::ifstream is(fileName.c_str());
            return tryDecode(is);
        }

        void encode(std::ostream &os) const
        {
            // iterate through all sections in this file
            for(const auto &filePair : *this)
            {
                os << "[" << filePair.first << "]" << std::endl;
                // iterate through all fields in the section
                for(const auto &secPair : filePair.second)
                    os << secPair.first << fieldSep_
                       << secPair.second.toString() << std::endl;
            }
        }

        std::string encode() const
        {
            std::ostringstream ss;
            encode(ss);
            return ss.str();
        }

        void save(const std::string &fileName) const
        {
            std::ofstream os(fileName.c_str());
            encode(os);
        }


      #ifndef THROW_PREVENTED
    private:

      	    // TBD: close stream?
	    void throwIfError(DecodeResult dRes)
	    {
		std::stringstream ss;
		ss << "l" << dRes.lineNumber
		   << ": ini parsing failed, ";
		switch (dRes.errorCode)
		{
		case NO_FAILURE:
		    // all ok 
		    return;
		case SECTION_NOT_CLOSED:
		    ss << "section not closed";
		    break;
		case SECTION_NAME_EMPTY:
		    ss << "section name is empty";
		    break;
		case SECTION_TEXT_AFTER:
		    ss << "no end of line after section";
		    break;
		case FIELD_WITHOUT_SECTION:
		    ss << "field has no section";
		    break;
		case FIELD_WITHOUT_SEPARATOR:
		    ss << "field without separator '" << fieldSep_ << 
			"' found";
		    break;
		default:
		    ss << "unknown failure code " << dRes.errorCode << " found";
		    throw std::logic_error(ss.str());
		}
		// TBD: this shall be a kind of parse error 
		throw std::logic_error(ss.str());
	    }


    public:

      	/**
	 * @throws logic_error if 
	 *   - section not closed 
	 *   - section is empty 
	 *   - section: no end of line after section
	 *   - field has no section 
	 *   - field has no field separator 
	 */
        void decode(std::istream &is)
        {
	    throwIfError(tryDecode(is));
        }

        void decode(const std::string &content)
        {
	    throwIfError(tryDecode(content));
        }
      
        void load(const std::string &fileName)
        {
	    throwIfError(tryLoad(fileName));
        }
#endif

    };


}

#endif
