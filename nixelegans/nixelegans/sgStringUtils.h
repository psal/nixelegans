//
//  sgStringUtils.h
//  sgGeneralUtilities
//
//  Created by Sean Grimes on 09/15/14.
//  Modified continuously
//  Last Modification: 10/28/14
//  Copyright (c) 2014 Grimes. All rights reserved.
//  Contact: spg63@cs.drexel.edu
//
//  You are free to use the code found in this file.
//  It's provided "as-is", no warranties, implicit or otherwise.
//  Not distributed under any license, so do as you wish.


#ifndef sgGeneralUtilities_sgStringUtils_h
#define sgGeneralUtilities_sgStringUtils_h
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>

/**
 * String functions are placed in namespace strUtils to help make the function
 *      calls unambigious (to the writer/reader).
 * As with any namespace, you can use:
 *      strUtils::trim();
 */

// --------- Function Headers --------------------------------------------------
// Append an 'error' log
void appendErrLog(const std::string &s);

// Append a 'normal' log
void appendLog(const std::string &s);

// Remove a specific character from a string
std::string removeChFromString(std::string &s, char &ch);

// Convert string to upper case, works on reference, be careful
void toUpper(std::string &s);

// Convert string to lower case, works on reference, be careful
void toLower(std::string &s);

// Trim leading and trailing white space
std::string trim(std::string s);

// Trim each line of a vector and return a vector of trimmed strings
std::vector<std::string> trimStringVector(std::vector<std::string> &v);

// Pass a num (int, double, float, char) and convert it to string w/sstream
template <typename T>
std::string numToString(T a);

// --------- End Function Headers ----------------------------------------------
namespace strUtils{
    /**
     * Both functions append a log file, and do not erase the log file between
     *      program runs. The user is responsible for calling and passing the string
     *      to the function within their .cpp files
     *
     * The log file will be created in the current working directory
     *
     * @param: s - The string to be appended to the log file
     * @return: void - Nothing
     */
    void appendErrLog(const std::string &s){
        std::ofstream out("errorLog.txt", std::ios_base::out | std::ios_base::app);
        out << s << std::endl;
    }
    
    void appendLog(const std::string &s){
        std::ofstream out("log.txt", std::ios_base::out | std::ios_base::app);
        out << s << std::endl;
    }
    
    /**
     * Recursively removes a specific character from a string
     *  First converts the char to a string w/ sstream then searchs the
     *      string for the char and removes and matching char
     *
     * WARNING: This is case sensitive, ie 'S' != 's'
     *
     * @param: s - The string to remove the character from
     * @param: ch - The char to remove from the string
     * @return: std::string - A new string with the specific char removed
     */
    std::string removeCharFromString(std::string &s, char &ch){
        std::stringstream char_to_string;
        std::string s_ch;
        char_to_string << ch;
        char_to_string >> s_ch;
        
        // lets stop all this recursion
        if (s.length() == 0)
            return "";
        else if (s.length() == 1){
            if (s_ch == s)
                return "";
            else
                return s;
        }
        else{
            size_t end = s.length() - 1;
            std::string test_s = s.substr(0,1);
            std::string rest = s.substr(1, end);
            if (test_s == s_ch)
                return removeCharFromString(rest, ch);
            else
                return test_s + removeCharFromString(rest, ch);
        }
    }
    
    /**
     * Takes a string reference and uses std::transform to convert all
     *  chars to uppercase.
     *
     * @param: s - User passed string to be converted to uppercase
     * @return: void, works on pass by reference, avoid constructor
     *  calls, returns
     */
    void toUpper(std::string &s){
        transform(s.begin(), s.end(), s.begin(), ::toupper);
    }
    
    /**
     * Takes a string reference and uses std::transform to convert all
     *  chars to lowercase.
     *
     * @param: s - User passed string to be converted to lowercase
     * @return: void, works on pass by reference, avoid constructor
     *  calls, returns
     */
    void toLower(std::string &s){
        transform(s.begin(), s.end(), s.begin(), ::tolower);
    }
    
    /**
     * Trims the leading and trailing whitespace
     *  The function trims any blank space and non printing characters
     *  Done by stripping non printing characters with erase/remove idiom
     *      - Algorithm lib: remove --> make sure the erased chars aren't blank
     *      - String method: erase --> erase characters
     *
     *  Blankspace removed with find_first_not_of, find_last_not_of
     *
     * @param: s - User passed string to be trimmed
     * @return: std::string - Returns a trimmed/stripped string
     */
    std::string trim(std::string s){
        std::string trimmed = s;
        // Removing tab chars
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\t'), trimmed.end());
        // Removing new line chars
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\n'), trimmed.end());
        // Removing 'audible bell'
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\a'), trimmed.end());
        // Removing backspace
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\b'), trimmed.end());
        // Removing form-feed
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\f'), trimmed.end());
        // Removing carriage return
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());
        // Removing vertical tab
        trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\v'), trimmed.end());
        size_t first_char = trimmed.find_first_not_of(" ");
        size_t last_char = trimmed.size() - first_char;
        std::string no_leading_ws = trimmed.substr(first_char, last_char);
        // Resetting first_char to 0
        first_char = 0;
        last_char = no_leading_ws.find_last_not_of(" ");
        std::string all_trimmed = no_leading_ws.substr(first_char, last_char + 1);
        return all_trimmed;
    }
    
    /**
     * Trims each line of a vector<string> using the above trim function and
     *  returns a new vector of trimmed items
     *
     * It would be more efficient to use trim and a loop or iterator, but
     * sometimes a new vector of strings is preferable
     *
     * @param: v - The vector of strings to be trimmed
     * @return: std::vector<std::string> - The vector of trimmed strings
     */
    std::vector<std::string> trimStringVector(std::vector<std::string> &v){
        std::vector<std::string> r_vec(v.size());
        for(int i = 0; i < v.size(); ++i){
            r_vec[i] = trim(v[i]);
        }
        return r_vec;
    }
    
    /**
     * Converts an int, double, or float to a string, returns the string
     *      Using strigstream
     *
     * @param: T a - an int, double, or floar
     * @return: std::string - the converted number
     */
    template <typename T>
    std::string numToString(T a){
        std::stringstream vert;
        std::string b;
        vert << a;
        vert >> b;
        return b;
    }
    
    
    
} //namespace strUtils


#endif
