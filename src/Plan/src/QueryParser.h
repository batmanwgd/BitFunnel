#pragma once

#include <iosfwd>   // std::istream parameter.
#include <stdexcept>
#include <string>  // std::string.


// Primitive things you can talk about are terms and phrases. And then we'll
// have boolean operators (AND, OR, NOT) and a few other things.
//
// Note that Google and Bing AND terms by default. Lucene uses OR by default.
//
// TODO: what should TERM be called?
//
// This doesn't have any unicode specific support. However, since UTF-8 bytes
// cannot be ASCII characters unless the UTF-8 character actually is the ASCII
// character, the operator and escaping code here should work with UTF-8.

namespace BitFunnel
{
    class IAllocator;
    class TermMatchNode;

    class QueryParser
    {
    public:
        QueryParser(std::istream& input, IAllocator& allocator);

        TermMatchNode const * Parse();

        //
        // ParseError records the character position and cause of an error
        // during parsing.
        //
        class ParseError : public std::runtime_error
        {
        public:
            ParseError(char const * message, size_t position);

            friend std::ostream& operator<< (std::ostream &out, const ParseError &e);

        private:
            // Character position where error occurred.
            size_t m_position;
        };

    private:
        // OR:
        //   AND (| AND)*
        TermMatchNode const * ParseOr();

        // AND:
        //   SIMPLE ([&] SIMPLE)*
        TermMatchNode const * ParseAnd();

        // TERM:
        //   [StreamId:]'"' PHRASE '"'
        //   [StreamId:]UNIGRAM
        TermMatchNode const * ParseTerm();

        // SIMPLE:
        //   '-' SIMPLE
        //   '(' OR ')'
        //   TERM
        TermMatchNode const * ParseSimple();

        // UNIGRAM:
        //   ![SPACE|SPECIAL]+
        TermMatchNode const * ParseUnigram();
        TermMatchNode const * ParseCachedUnigram(char const * cache);

        // PHRASE:
        //   '"' UNIGRAM (SPACE* UNIGRAM)* '"'
        TermMatchNode const * ParsePhrase();

        // DESIGN NOTE:
        // one-two is parsed as one NOT two instead of the unigram one-two.
        // This should probably be changed as this is counter-intuitive for
        // most users.

        char const * ParseToken();

        bool AtEOF();
        // Note that delimeters must be ASCII.
        void ExpectDelimeter(char c);
        void SkipWhite();
        char PeekChar();
        char GetChar();
        char GetWithEscape();

        std::istream& m_input;
        IAllocator& m_allocator;

        // Used for errors.
        size_t m_currentPosition;
        bool m_haveChar;
        char m_nextChar;
    };
}
